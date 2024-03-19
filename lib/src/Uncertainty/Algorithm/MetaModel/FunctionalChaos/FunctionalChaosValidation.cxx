//                                               -*- C++ -*-
/**
 *  @brief Validation of a functional chaos expansion
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
#include <algorithm>
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/FunctionalChaosValidation.hxx"
#include "openturns/Os.hxx"
#include "openturns/OSS.hxx"
#include "openturns/DesignProxy.hxx"
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

/* Default constructor */

FunctionalChaosValidation::FunctionalChaosValidation(const FunctionalChaosResult & functionalChaosResult,
                                                     const CrossValidationMethod cvMethod, 
                                                     const UnsignedInteger & kParameter)
  : MetaModelValidation(functionalChaosResult.getOutputSample(), 
      ComputeMetamodelCrossValidationPredictions(functionalChaosResult, cvMethod, kParameter))
  , functionalChaosResult_(functionalChaosResult)
{
  if ((cvMethod != LEAVEONEOUT) && (cvMethod != KFOLD))
    throw InvalidArgumentException(HERE) << "The method " << cvMethod << " is not available.";
  cvMethod_ = cvMethod;
  if (kParameter < 1)
    throw InvalidArgumentException(HERE) << "Cannot set k parameter of K-Fold method to " << kParameter << " which is lower than 1";
  const UnsignedInteger sampleSize = functionalChaosResult_.getInputSample().getSize();
  if (kParameter > sampleSize)
    throw InvalidArgumentException(HERE) << "Cannot set k parameter of K-Fold method to " << kParameter 
      << " which is larger than the sample size =" << sampleSize;
  kParameter_ = kParameter;
  if (ResourceMap::GetAsBool("FunctionalChaosValidation-NoModelSelection") && \
    functionalChaosResult_.getIsModelSelection())
    throw InvalidArgumentException(HERE) << "Cannot perform fast cross-validation "
      << "with a polynomial chaos expansion involving model selection";
  if (!functionalChaosResult.getIsLeastSquares())
    throw InvalidArgumentException(HERE) << "Error: the polynomial chaos expansion was not computed from regression.";  
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
      << " kParameter_=" << kParameter_
      << " cvMethod_=" << cvMethod_;
  return oss;
}

/* Get result*/
FunctionalChaosResult FunctionalChaosValidation::getFunctionalChaosResult() const
{
  return functionalChaosResult_;
}

/* Get the K parameter */
UnsignedInteger FunctionalChaosValidation::getKParameter() const
{
  return kParameter_;
}

/* Get the CV Method */
UnsignedInteger FunctionalChaosValidation::getMethod() const
{
  return cvMethod_;
}

/* Compute cross-validation metamodel predictions */
Sample FunctionalChaosValidation::ComputeMetamodelCrossValidationPredictions(
    const FunctionalChaosResult & functionalChaosResult,
    const CrossValidationMethod cvMethod, 
    const UnsignedInteger & kParameter)
{
  const Sample residualsSample(functionalChaosResult.getSampleResiduals());
  const Sample inputSample(functionalChaosResult.getInputSample());
  const UnsignedInteger sampleSize = inputSample.getSize();
  const FunctionCollection reducedBasis(functionalChaosResult.getReducedBasis());
  const UnsignedInteger reducedBasisSize = reducedBasis.getSize();
  if (reducedBasisSize > sampleSize)
    throw InvalidArgumentException(HERE) << "Error: the sample size is: " << sampleSize << 
      " which is lower than the basis size: " << reducedBasisSize;
  const Sample outputSample(functionalChaosResult.getOutputSample());
  const UnsignedInteger outputDimension = outputSample.getDimension();
  const Function metamodel(functionalChaosResult.getMetaModel());
  const Function transformation(functionalChaosResult.getTransformation());
  const Sample standardSample(transformation(inputSample));
  DesignProxy designProxy(standardSample, reducedBasis);
  Indices allIndices(reducedBasisSize);
  allIndices.fill();
  const Matrix designMatrix(designProxy.computeDesign(allIndices));
  // The method name is set to the default one, given by ResourceMap
  const String methodName(ResourceMap::GetAsString("LeastSquaresExpansion-DecompositionMethod"));
  LeastSquaresMethod leastSquaresMethod(LeastSquaresMethod::Build(methodName, designProxy, allIndices));
  leastSquaresMethod.update(Indices(0), allIndices, Indices(0));
  Sample cvPredictions(sampleSize, outputDimension);
  if (cvMethod == LEAVEONEOUT)
  {
    const Point diagonalH = leastSquaresMethod.getHDiag();
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
      for (UnsignedInteger i = 0; i < sampleSize; ++i)
        cvPredictions(i, j) = outputSample(i, j) - residualsSample(i, j) / (1.0 - diagonalH[i]);
  }
  else if (cvMethod == KFOLD)
  {
    const SymmetricMatrix projectionMatrix(leastSquaresMethod.getH());
    // Compute K-Fold error
    KFoldSplitter splitter(sampleSize, kParameter);
    for (UnsignedInteger foldIndex = 0; foldIndex < kParameter; ++foldIndex)
    {
      Indices indicesTest;
      const Indices indicesTrain(splitter.generate(indicesTest));
      const UnsignedInteger foldSize = indicesTest.getSize();
      SymmetricMatrix projectionKFoldMatrix(foldSize);
      for (UnsignedInteger i1 = 0; i1 < foldSize; ++i1)
        for (UnsignedInteger i2 = 0; i2 < 1 + i1; ++i2)
          projectionKFoldMatrix(i1, i2) = projectionMatrix(indicesTest[i1], indicesTest[i2]);
      const IdentityMatrix identityMatrix(foldSize);
      const SymmetricMatrix reducedMatrix(identityMatrix - projectionKFoldMatrix);
      const Sample residualsSampleKFoldTest(residualsSample.select(indicesTest));
      Matrix multipleRightHandSide(foldSize, outputDimension);
      for (UnsignedInteger j = 0; j < outputDimension; ++j)
        for (UnsignedInteger i = 0; i < foldSize; ++i)
          multipleRightHandSide(i, j) = residualsSampleKFoldTest(i, j);
      const Matrix residualsKFoldMatrix(reducedMatrix.solveLinearSystem(multipleRightHandSide));
      for (UnsignedInteger j = 0; j < outputDimension; ++j)
        for (UnsignedInteger i = 0; i < foldSize; ++i)
          cvPredictions(indicesTest[i], j) = outputSample(indicesTest[i], j) - residualsKFoldMatrix(i, j);
    } // For fold indices
  }
  else
    throw InvalidArgumentException(HERE) << "The method " << cvMethod << " is not available.";
  return cvPredictions;
}

/* Method save() stores the object through the StorageManager */
void FunctionalChaosValidation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "functionalChaosResult_", functionalChaosResult_ );
}

/* Method load() reloads the object from the StorageManager */
void FunctionalChaosValidation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "functionalChaosResult_", functionalChaosResult_ );
}

END_NAMESPACE_OPENTURNS
