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
  // The predictions sit on the training points: score them with the design
  // weights so that R2 and MSE converge to their continuous counterparts
  setWeights(functionalChaosResult_.getWeights());
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
  // The predictions sit on the training points: score them with the design
  // weights so that R2 and MSE converge to their continuous counterparts
  setWeights(functionalChaosResult_.getWeights());
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
  // The design weights stored in the result enter the method so that the
  // leverages are the weighted ones: dropping an observation is then a
  // rank-one downdate of the weighted normal equations and r_i/(1-h_i)
  // is the exact leave-one-out residual for any weights
  const Point weights(functionalChaosResult.getWeights());
  const Bool useUniformWeights = (weights.getSize() == 1);
  if (!useUniformWeights && (weights.getSize() != sampleSize)) throw InvalidArgumentException(HERE) << "FunctionalChaosValidation: design weights size (" << weights.getSize() << ") must match the sample size (" << sampleSize << ") or be a single uniform value";
  LeastSquaresMethod leastSquaresMethod(useUniformWeights ?
      LeastSquaresMethod::Build(methodName, designProxy, allIndices) :
      LeastSquaresMethod::Build(methodName, designProxy, weights, allIndices));
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
  const UnsignedInteger sampleSize = inputSample.getSize();
  const Function transformation(functionalChaosResult.getTransformation());
  const Sample standardSample(transformation(inputSample));
  DesignProxy designProxy(standardSample, reducedBasis);
  Indices allIndices(reducedBasisSize);
  allIndices.fill();
  // The method name is set to the default one, given by ResourceMap
  const String methodName(ResourceMap::GetAsString("LeastSquaresExpansion-DecompositionMethod"));
  // The design weights stored in the result select the exact downdate:
  // uniform weights reuse the legacy (I - P_TT) block formula, other
  // weights use the weighted downdate below
  const Point weights(functionalChaosResult.getWeights());
  const Bool useUniformWeights = (weights.getSize() == 1);
  if (!useUniformWeights && (weights.getSize() != sampleSize)) throw InvalidArgumentException(HERE) << "FunctionalChaosValidation: design weights size (" << weights.getSize() << ") must match the sample size (" << sampleSize << ") or be a single uniform value";
  LeastSquaresMethod leastSquaresMethod(useUniformWeights ?
    LeastSquaresMethod::Build(methodName, designProxy, allIndices) :
    LeastSquaresMethod::Build(methodName, designProxy, weights, allIndices));
  leastSquaresMethod.update(Indices(0), allIndices, Indices(0));
  if (useUniformWeights)
  {
    const SymmetricMatrix projectionMatrix(leastSquaresMethod.getH());
    const Sample cvPredictions(MetaModelValidation::ComputeMetamodelKFoldPredictions(
                                 outputSample, residualsSample, projectionMatrix, splitter));
    return cvPredictions;
  }
  // Weighted case: on each test block T solve (I - A W_T) u = r_T with
  // A = X_T G^{-1} X_T^T, G = Psi^T W Psi the weighted Gram matrix.
  // Removing the block is a downdate of the weighted normal equations,
  // so y_T - u holds the exact KFold predictions for any weights. For
  // uniform weights this reduces to the (I - P_TT) formula above.
  const Matrix design(leastSquaresMethod.computeDesign());
  const SymmetricMatrix gramInverseSym(leastSquaresMethod.getGramInverse());
  const UnsignedInteger outputDimension = outputSample.getDimension();
  Sample cvPredictions(sampleSize, outputDimension);
  const UnsignedInteger kParameter = splitter.getSize();
  Indices indicesTest;
  for (UnsignedInteger foldIndex = 0; foldIndex < kParameter; ++foldIndex)
  {
    splitter.generate(indicesTest);
    const UnsignedInteger foldSize = indicesTest.getSize();
    Matrix testDesign(foldSize, reducedBasisSize);
    for (UnsignedInteger i = 0; i < foldSize; ++i)
      for (UnsignedInteger j = 0; j < reducedBasisSize; ++j)
        testDesign(i, j) = design(indicesTest[i], j);
    const Matrix downdate((testDesign * gramInverseSym) * testDesign.transpose());
    Matrix reducedMatrix(foldSize, foldSize);
    for (UnsignedInteger i1 = 0; i1 < foldSize; ++i1)
      for (UnsignedInteger i2 = 0; i2 < foldSize; ++i2)
        reducedMatrix(i1, i2) = (i1 == i2 ? 1.0 : 0.0) - downdate(i1, i2) * weights[indicesTest[i2]];
    Matrix multipleRightHandSide(foldSize, outputDimension);
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
      for (UnsignedInteger i = 0; i < foldSize; ++i)
        multipleRightHandSide(i, j) = residualsSample(indicesTest[i], j);
    const Matrix inflatedResiduals(reducedMatrix.solveLinearSystem(multipleRightHandSide));
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
      for (UnsignedInteger i = 0; i < foldSize; ++i)
        cvPredictions(indicesTest[i], j) = outputSample(indicesTest[i], j) - inflatedResiduals(i, j);
  } // For folds
  return cvPredictions;
}

/* Method save() stores the object through the StorageManager */
void FunctionalChaosValidation::save(Advocate & adv) const
{
  MetaModelValidation::save(adv);
  adv.saveAttribute("functionalChaosResult_", functionalChaosResult_);
  adv.saveAttribute("splitter_", splitter_ );
}

/* Method load() reloads the object from the StorageManager */
void FunctionalChaosValidation::load(Advocate & adv)
{
  MetaModelValidation::load(adv);
  adv.loadAttribute("functionalChaosResult_", functionalChaosResult_);
  adv.loadAttribute("splitter_", splitter_ );
}

END_NAMESPACE_OPENTURNS
