//                                               -*- C++ -*-
/**
 *  @brief Validation of a functional chaos expansion
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/FunctionalChaosValidation.hxx"
#include "openturns/LeastSquaresMethod.hxx"
#include "openturns/KFoldSplitter.hxx"
#include "openturns/MetaModelValidation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FunctionalChaosValidation)

static const Factory<FunctionalChaosValidation> Factory_FunctionalChaosValidation;

/* Default constructor */
FunctionalChaosValidation::FunctionalChaosValidation()
  : MetaModelValidation()
{
  // Nothing to do
}

/* Parameter constructor */
FunctionalChaosValidation::FunctionalChaosValidation(const FunctionalChaosResult & functionalChaosResult)
  : FunctionalChaosValidation(functionalChaosResult, LeaveOneOutSplitter(functionalChaosResult.getSampleResiduals().getSize()))
{
  // Nothing to do
}

/* LOO constructor */
FunctionalChaosValidation::FunctionalChaosValidation(const FunctionalChaosResult & functionalChaosResult,
    const LeaveOneOutSplitter & splitter)
  : MetaModelValidation(functionalChaosResult.getOutputSample()
                        , ComputeMetamodelLeaveOneOutPredictions(functionalChaosResult, splitter))
  , functionalChaosResult_(functionalChaosResult)
  , splitter_ (splitter)
{
  const UnsignedInteger sampleSize = functionalChaosResult_.getSampleResiduals().getSize();
  if ((splitter_.getN() != sampleSize))
    throw InvalidArgumentException(HERE) << "The parameter N in the splitter is " << splitter_.getN()
                                         << " but the sample size is " << sampleSize;
  if (!ResourceMap::GetAsBool("FunctionalChaosValidation-ModelSelection") && \
      functionalChaosResult_.involvesModelSelection())
    throw InvalidArgumentException(HERE) << "Cannot perform fast cross-validation "
                                         << "with a polynomial chaos expansion involving model selection";
  if (!functionalChaosResult.isLeastSquares())
    throw InvalidArgumentException(HERE) << "Error: the polynomial chaos expansion was not computed from least squares.";
}

/* K-Fold constructor */
FunctionalChaosValidation::FunctionalChaosValidation(const FunctionalChaosResult & functionalChaosResult,
    const KFoldSplitter & splitter)
  : MetaModelValidation(functionalChaosResult.getOutputSample()
                        , ComputeMetamodelKFoldPredictions(functionalChaosResult, splitter))
  , functionalChaosResult_(functionalChaosResult)
  , splitter_ (splitter)
{
  const UnsignedInteger sampleSize = functionalChaosResult_.getSampleResiduals().getSize();
  if ((splitter_.getN() != sampleSize))
    throw InvalidArgumentException(HERE) << "The parameter N in the splitter is " << splitter_.getN()
                                         << " but the sample size is " << sampleSize;
  if (!ResourceMap::GetAsBool("FunctionalChaosValidation-ModelSelection") && \
      functionalChaosResult_.involvesModelSelection())
    throw InvalidArgumentException(HERE) << "Cannot perform fast cross-validation "
                                         << "with a polynomial chaos expansion involving model selection";
  if (!functionalChaosResult.isLeastSquares())
    throw InvalidArgumentException(HERE) << "Error: the polynomial chaos expansion was not computed from least squares.";
}

/* Virtual constructor */
FunctionalChaosValidation * FunctionalChaosValidation::clone() const
{
  return new FunctionalChaosValidation(*this);
}

/* String converter */
String FunctionalChaosValidation::__repr__() const
{
  OSS oss;
  oss << "class=" << FunctionalChaosValidation::GetClassName()
      << " functional chaos result=" << functionalChaosResult_
      << " splitter_=" << splitter_;
  return oss;
}

/* Get result*/
FunctionalChaosResult FunctionalChaosValidation::getFunctionalChaosResult() const
{
  return functionalChaosResult_;
}

/* Get the splitter */
SplitterImplementation FunctionalChaosValidation::getSplitter() const
{
  return splitter_;
}

/* Compute cross-validation Leave-One-Out metamodel predictions */
Sample FunctionalChaosValidation::ComputeMetamodelLeaveOneOutPredictions(
  const FunctionalChaosResult & functionalChaosResult,
  const LeaveOneOutSplitter & splitter)
{
  const Sample outputSample(functionalChaosResult.getOutputSample());
  const Sample residualsSample(functionalChaosResult.getSampleResiduals());
  const Sample inputSample(functionalChaosResult.getInputSample());
  const FunctionCollection reducedBasis(functionalChaosResult.getReducedBasis());
  const UnsignedInteger reducedBasisSize = reducedBasis.getSize();
  const UnsignedInteger sampleSize = inputSample.getSize();

  if (reducedBasisSize >= sampleSize)
    throw InvalidArgumentException(HERE) << "FunctionalChaosValidation: basis size for LOO (" << reducedBasisSize << ") must be lesser than the sample size (" << sampleSize << ")";

  const Function transformation(functionalChaosResult.getTransformation());
  const Sample standardSample(transformation(inputSample));
  DesignProxy designProxy(standardSample, reducedBasis);
  Indices allIndices(reducedBasisSize);
  allIndices.fill();
  // The method name is set to the default one, given by ResourceMap
  const String methodName(ResourceMap::GetAsString("LeastSquaresExpansion-DecompositionMethod"));
  LeastSquaresMethod leastSquaresMethod(LeastSquaresMethod::Build(methodName, designProxy, allIndices));
  leastSquaresMethod.update(Indices(0), allIndices, Indices(0));
  const Point hMatrixDiag = leastSquaresMethod.getHDiag();
  const Sample cvPredictions(MetaModelValidation::ComputeMetamodelLeaveOneOutPredictions(
                               outputSample, residualsSample, hMatrixDiag, splitter));
  return cvPredictions;
}

/* Compute cross-validation K-Fold metamodel predictions */
Sample FunctionalChaosValidation::ComputeMetamodelKFoldPredictions(
  const FunctionalChaosResult & functionalChaosResult,
  const KFoldSplitter & splitter)
{
  const Sample outputSample(functionalChaosResult.getOutputSample());
  const Sample residualsSample(functionalChaosResult.getSampleResiduals());
  const Sample inputSample(functionalChaosResult.getInputSample());
  const FunctionCollection reducedBasis(functionalChaosResult.getReducedBasis());
  const UnsignedInteger reducedBasisSize = reducedBasis.getSize();
  const Function transformation(functionalChaosResult.getTransformation());
  const Sample standardSample(transformation(inputSample));
  DesignProxy designProxy(standardSample, reducedBasis);
  Indices allIndices(reducedBasisSize);
  allIndices.fill();
  // The method name is set to the default one, given by ResourceMap
  const String methodName(ResourceMap::GetAsString("LeastSquaresExpansion-DecompositionMethod"));
  LeastSquaresMethod leastSquaresMethod(LeastSquaresMethod::Build(methodName, designProxy, allIndices));
  leastSquaresMethod.update(Indices(0), allIndices, Indices(0));
  const SymmetricMatrix projectionMatrix(leastSquaresMethod.getH());
  const Sample cvPredictions(MetaModelValidation::ComputeMetamodelKFoldPredictions(
                               outputSample, residualsSample, projectionMatrix, splitter));
  return cvPredictions;
}

/* Method save() stores the object through the StorageManager */
void FunctionalChaosValidation::save(Advocate & adv) const
{
  MetaModelValidation::save(adv);
  adv.saveAttribute( "functionalChaosResult_", functionalChaosResult_ );
}

/* Method load() reloads the object from the StorageManager */
void FunctionalChaosValidation::load(Advocate & adv)
{
  MetaModelValidation::load(adv);
  adv.loadAttribute( "functionalChaosResult_", functionalChaosResult_ );
}

END_NAMESPACE_OPENTURNS
