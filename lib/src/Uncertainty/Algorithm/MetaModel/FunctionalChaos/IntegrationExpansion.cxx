//                                               -*- C++ -*-
/**
 *  @brief The class building chaos expansions based on a dot-product
 *         approach.
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/IntegrationExpansion.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/DistributionTransformation.hxx"
#include "openturns/IdentityFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef Collection<Function> FunctionCollection;

CLASSNAMEINIT(IntegrationExpansion)

static const Factory<IntegrationExpansion> Factory_IntegrationExpansion;


/* Default constructor */
IntegrationExpansion::IntegrationExpansion()
  : FunctionalChaosAlgorithm()
{
  // Nothing to do
}


/* Constructor */
IntegrationExpansion::IntegrationExpansion(const Sample & inputSample,
    const Sample & outputSample,
    const Distribution & distribution)
  : IntegrationExpansion(inputSample, Point(inputSample.getSize(), 1.0 / inputSample.getSize()), outputSample, distribution)
{
  // Nothing to do
}

/* Constructor */
IntegrationExpansion::IntegrationExpansion(const Sample & inputSample,
    const Point & weights,
    const Sample & outputSample,
    const Distribution & distribution)
  : FunctionalChaosAlgorithm(inputSample, weights, outputSample, distribution)
{
  // Extract the basis from the adaptive strategy attribute of the base class
  basis_ = getAdaptiveStrategy().getBasis();
  // The basis size is the maximum dimension of the adaptive strategy
  basisSize_ = getAdaptiveStrategy().getMaximumDimension();
  // The active functions. By default all the functions are active
  activeFunctions_ = Indices(basisSize_);
  activeFunctions_.fill();
}

/* Constructor */
IntegrationExpansion::IntegrationExpansion(const Sample & inputSample,
    const Sample & outputSample,
    const Distribution & distribution,
    const OrthogonalBasis & basis,
    const UnsignedInteger basisSize)
  : IntegrationExpansion(inputSample, Point(inputSample.getSize(), 1.0 / inputSample.getSize()), outputSample, distribution, basis, basisSize)
{
  // Nothing to do
}

/* Constructor */
IntegrationExpansion::IntegrationExpansion(const Sample & inputSample,
    const Point & weights,
    const Sample & outputSample,
    const Distribution & distribution,
    const OrthogonalBasis & basis,
    const UnsignedInteger basisSize)
  : FunctionalChaosAlgorithm(inputSample, weights, outputSample, distribution)
  , basis_(basis)
  , basisSize_(basisSize)
{
  // The arguments are checked in the base class, excepted the basis and its size
  if (basis.getMeasure().getDimension() != distribution.getDimension()) throw InvalidArgumentException(HERE) << "Error: the basis must have a measure with the same dimension as the input distribution, heare measure dimension=" << basis.getMeasure().getDimension() << " and distribution dimension=" << distribution.getDimension();
  if (basisSize == 0) throw InvalidArgumentException(HERE) << "Error: cannot project on a basis of size zero";
  // The active functions. By default all the functions are active
  activeFunctions_ = Indices(basisSize_);
  activeFunctions_.fill();
}


/* Virtual constructor */
IntegrationExpansion * IntegrationExpansion::clone() const
{
  return new IntegrationExpansion(*this);
}


/* Computes the functional chaos */
void IntegrationExpansion::run()
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
  const Matrix designMatrix(designProxy_.computeDesign(activeFunctions_));
  // The design matrix M is a (sampleSize, basisSize) matrix
  // The output sample is a (sampleSize, outputDimension) sample
  // The coefficient Cj,k j associated to the basis function j
  // of the output Y[:, k] is the weighted dot-product
  // Cj,k = \sum_i w[i] * M[i,j], Y[i, k])
  //
  // We see that these coefficients can be obtained through a
  // multiplication between M and the row-wise weighted output
  const UnsignedInteger sampleSize = inputSample_.getSize();
  const UnsignedInteger outputDimension = outputSample_.getDimension();
  Matrix weightedOutput(outputDimension, sampleSize);
  for (UnsignedInteger i = 0; i < sampleSize; ++i)
  {
    const Scalar wI = weights_[i];
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
      weightedOutput(j, i) = wI * outputSample_(i, j);
  } // i
  // The design matrix has to be transposed, not the weighted output
  // The coefficients are obtained as a (activeFunctions_.getSize(), outputDimension) matrix
  // As they will be used as a Sample of size activeFunctions_.getSize() and
  // dimension outputDimension, it is better to compute them in a transposed
  // form and copy the internal representation
  Matrix coefficientsAsMatrix(weightedOutput * designMatrix);
  SampleImplementation coefficients(activeFunctions_.getSize(), outputDimension);
  coefficients.setData(*coefficientsAsMatrix.getImplementation());
  // Compute the output approximation. We rely on the genProd() method to avoid an explicit transposition
  const Matrix predictedOutput(coefficientsAsMatrix.getImplementation()->genProd(*designMatrix.getImplementation(), false, true));
  Point relativeErrors(outputDimension);
  Point residuals(outputDimension);
  for (UnsignedInteger j = 0; j < outputDimension; ++j)
  {
    const Sample marginalOutputSample(outputSample_.getMarginal(j));
    // Now the two errors
    const Scalar quadraticResidual = (Point(*predictedOutput.getRow(j).getImplementation()) - marginalOutputSample.asPoint()).normSquare();
    residuals[j] = std::sqrt(quadraticResidual) / sampleSize;
    const Scalar empiricalError = quadraticResidual / sampleSize;
    relativeErrors[j] = empiricalError / marginalOutputSample.computeVariance()[0];
  }
  // Build the result
  result_ = FunctionalChaosResult(inputSample_, outputSample_, distribution_, transformation_, inverseTransformation_, basis_, activeFunctions_, coefficients, designProxy_.getBasis(activeFunctions_), residuals, relativeErrors);
}

/* Method to get/set the active functions */
Indices IntegrationExpansion::getActiveFunctions() const
{
  return activeFunctions_;
}

void IntegrationExpansion::setActiveFunctions(const Indices & activeFunctions)
{
  if (!activeFunctions.check(basisSize_)) throw InvalidArgumentException(HERE) << "Error: the active functions must have indices less than " << basisSize_;
  activeFunctions_ = activeFunctions;
}

/* String converter */
String IntegrationExpansion::__repr__() const
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
String IntegrationExpansion::__str__(const String & ) const
{
  return OSS() << getClassName() << "("
         << "basis=" << basis_
         << ", basisSize=" << basisSize_
         << ", activeFunctions=" << activeFunctions_
         << ")";
}

/* Method save() stores the object through the StorageManager */
void IntegrationExpansion::save(Advocate & adv) const
{
  FunctionalChaosAlgorithm::save(adv);
  adv.saveAttribute( "basis_", basis_ );
  adv.saveAttribute( "basisSize_", basisSize_ );
  adv.saveAttribute( "activeFunctions_", activeFunctions_ );
  adv.saveAttribute( "transformation_", transformation_ );
  adv.saveAttribute( "inverseTransformation_", inverseTransformation_ );
}


/* Method load() reloads the object from the StorageManager */
void IntegrationExpansion::load(Advocate & adv)
{
  FunctionalChaosAlgorithm::load(adv);
  adv.loadAttribute( "basis_", basis_ );
  adv.loadAttribute( "basisSize_", basisSize_ );
  adv.loadAttribute( "activeFunctions_", activeFunctions_ );
  adv.loadAttribute( "transformation_", transformation_ );
  adv.loadAttribute( "inverseTransformation_", inverseTransformation_ );
}



END_NAMESPACE_OPENTURNS
