//                                               -*- C++ -*-
/**
 *  @brief The class building chaos expansions based on a least-squares
 *         approach.
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <cstdlib>
#include <map>

#include "openturns/LARSExpansion.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/DistributionTransformation.hxx"
#include "openturns/LARSMethod.hxx"
#include "openturns/IdentityFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef Collection<Function> FunctionCollection;

CLASSNAMEINIT(LARSExpansion)

static const Factory<LARSExpansion> Factory_LARSExpansion;


/* Default constructor */
LARSExpansion::LARSExpansion()
  : FunctionalChaosAlgorithm()
{
  // Nothing to do
}


/* Constructor */
LARSExpansion::LARSExpansion(const Sample & inputSample,
			     const Sample & outputSample,
			     const Distribution & distribution,
			     const Bool isLARS)
  : LARSExpansion(inputSample, Point(inputSample.getSize(), 1.0 / inputSample.getSize()), outputSample, distribution, isLARS)
{
  // Nothing to do
}

/* Constructor */
LARSExpansion::LARSExpansion(const Sample & inputSample,
			     const Point & weights,
			     const Sample & outputSample,
			     const Distribution & distribution,
			     const Bool isLARS)
  : FunctionalChaosAlgorithm(inputSample, weights, outputSample, distribution)
{
  // Extract the basis from the adaptive strategy attribute of the base class
  basis_ = getAdaptiveStrategy().getBasis();
  // The basis size is the maximum dimension of the adaptive strategy
  basisSize_ = getAdaptiveStrategy().getMaximumDimension();
  // The active functions. By default all the functions are active
  activeFunctions_ = Indices(basisSize_);
  activeFunctions_.fill();
  // The method name is set to the default one, given by ResourceMap
  methodName_ = ResourceMap::GetAsString("LARSExpansion-DecompositionMethod");
  // The isLARS flag
  isLARS_ = isLARS;
}

/* Constructor */
LARSExpansion::LARSExpansion(const Sample & inputSample,
			     const Sample & outputSample,
			     const Distribution & distribution,
			     const OrthogonalBasis & basis,
			     const UnsignedInteger basisSize,
			     const String & methodName,
			     const Bool isLARS,
			     const FittingAlgorithm & fitting)
  : LARSExpansion(inputSample, Point(inputSample.getSize(), 1.0 / inputSample.getSize()), outputSample, distribution, basis, basisSize, methodName, isLARS, fitting)
{
  // Nothing to do
}

/* Constructor */
LARSExpansion::LARSExpansion(const Sample & inputSample,
			     const Point & weights,
			     const Sample & outputSample,
			     const Distribution & distribution,
			     const OrthogonalBasis & basis,
			     const UnsignedInteger basisSize,
			     const String & methodName,
			     const Bool isLARS,
			     const FittingAlgorithm & fitting)
  : FunctionalChaosAlgorithm(inputSample, weights, outputSample, distribution)
  , basis_(basis)
  , basisSize_(basisSize)
  , methodName_(methodName)
  , isLARS_(isLARS)
  , fitting_(fitting)
{
  // The arguments are checked in the base class, excepted the basis and its size
  if (basis.getMeasure().getDimension() != distribution.getDimension()) throw InvalidArgumentException(HERE) << "Error: the basis must have a measure with the same dimension as the input distribution, here measure dimension=" << basis.getMeasure().getDimension() << " and distribution dimension=" << distribution.getDimension();
  if (basisSize == 0) throw InvalidArgumentException(HERE) << "Error: cannot project on a basis of size zero";
  // The active functions. By default all the functions are active
  activeFunctions_ = Indices(basisSize_);
  activeFunctions_.fill();
}


/* Virtual constructor */
LARSExpansion * LARSExpansion::clone() const
{
  return new LARSExpansion(*this);
}


/* Computes the functional chaos */
void LARSExpansion::run()
{
  // Check if we have to transform the input sample in order to adapt it
  // to the basis
  if (designProxy_.getSampleSize() == 0)
  {
    const Distribution measure(basis_.getMeasure());
    Sample transformedInputSample;
    if (distribution_ == measure)
    {
      transformation_ = IdentityFunction(distribution_.getDimension());
      inverseTransformation_ = IdentityFunction(distribution_.getDimension());
      transformedInputSample = inputSample_;
    }
    else
    {
      transformation_ = DistributionTransformation(distribution_, basis_.getMeasure());
      inverseTransformation_ = DistributionTransformation(basis_.getMeasure(), distribution_);
      transformedInputSample = transformation_(inputSample_);
    }
    FunctionCollection functions(basisSize_);
    for (UnsignedInteger i = 0; i < basisSize_; ++i)
      functions[i] = basis_.build(i);
    designProxy_ = DesignProxy(transformedInputSample, functions);
  } // Design proxy and transformation initialization
  // Now build the design proxy and the least-squares method
  LARSMethod LARSMethod = LARSMethod::Build(methodName_, designProxy_, weights_, activeFunctions_);
  const UnsignedInteger outputDimension = outputSample_.getDimension();
  SampleImplementation coefficients(activeFunctions_.getSize(), outputDimension);
  const Matrix weightedDesign(LARSMethod.computeWeightedDesign());
  Point residuals(outputDimension);
  Point relativeErrors(outputDimension);
  const UnsignedInteger sampleSize = inputSample_.getSize();
  for (UnsignedInteger j = 0; j < outputDimension; ++j)
  {
    const Sample marginalOutputSample(outputSample_.getMarginal(j));
    const Point rhs(marginalOutputSample.asPoint());
    const Point coeffsJ(LARSMethod.solve(rhs));
    for (UnsignedInteger i = 0; i < activeFunctions_.getSize(); ++i)
      coefficients(i, j) = coeffsJ[i];
    // Now the two errors
    const Scalar quadraticResidual = (weightedDesign * coeffsJ - rhs).normSquare();
    residuals[j] = std::sqrt(quadraticResidual) / sampleSize;
    const Scalar empiricalError = quadraticResidual / sampleSize;
    relativeErrors[j] = empiricalError / marginalOutputSample.computeVariance()[0];
  }
  // Build the result
  result_ = FunctionalChaosResult(inputSample_, outputSample_, distribution_, transformation_, inverseTransformation_, basis_, activeFunctions_, coefficients, designProxy_.getBasis(activeFunctions_), residuals, relativeErrors);
  result_.setIsLARS(true);
  result_.setInvolvesModelSelection(false);
}

/* Method to get/set the active functions */
Indices LARSExpansion::getActiveFunctions() const
{
  return activeFunctions_;
}

void LARSExpansion::setActiveFunctions(const Indices & activeFunctions)
{
  if (!activeFunctions.check(basisSize_)) throw InvalidArgumentException(HERE) << "Error: the active functions must have indices less than " << basisSize_;
  activeFunctions_ = activeFunctions;
}

/* Method to get/set the fitting algorithm */
FittingAlgorithm LARSExpansion::getFittingAlgorithm() const
{
  return fitting_;
}

void LARSExpansion::setFittingAlgorithm(const FittingAlgorithm & fitting)
{
  fitting_ = fitting;
}

/* Method to get/set the LARS update wrt the OMP update */
Bool LARSExpansion::getIsLARS() const
{
  return isLARS_;
}

void LARSExpansion::setIsLARS(const Bool isLARS)
{
  isLARS_ = isLARS;
}

/* String converter */
String LARSExpansion::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " basis=" << basis_
         << " basisSize=" << basisSize_
         << " activeFunctions=" << activeFunctions_
         << " designProxy=" << designProxy_
         << " transformation=" << transformation_
         << " inverseTransformation=" << inverseTransformation_;
}


/* String converter */
String LARSExpansion::__str__(const String & ) const
{
  return OSS() << getClassName() << "("
         << "basis=" << basis_
         << ", basisSize=" << basisSize_
         << ", activeFunctions=" << activeFunctions_
         << ", methodName=" << methodName_
         << ")";
}

/* Method save() stores the object through the StorageManager */
void LARSExpansion::save(Advocate & adv) const
{
  FunctionalChaosAlgorithm::save(adv);
  adv.saveAttribute( "basis_", basis_ );
  adv.saveAttribute( "basisSize_", basisSize_ );
  adv.saveAttribute( "activeFunctions_", activeFunctions_ );
  adv.saveAttribute( "methodName_", methodName_ );
  adv.saveAttribute( "transformation_", transformation_ );
  adv.saveAttribute( "inverseTransformation_", inverseTransformation_ );
}


/* Method load() reloads the object from the StorageManager */
void LARSExpansion::load(Advocate & adv)
{
  FunctionalChaosAlgorithm::load(adv);
  adv.loadAttribute( "basis_", basis_ );
  adv.loadAttribute( "basisSize_", basisSize_ );
  adv.loadAttribute( "activeFunctions_", activeFunctions_ );
  adv.loadAttribute( "methodName_", methodName_ );
  adv.loadAttribute( "transformation_", transformation_ );
  adv.loadAttribute( "inverseTransformation_", inverseTransformation_ );
}



END_NAMESPACE_OPENTURNS
