//                                               -*- C++ -*-
/**
 *  @brief The linear model validation
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
#include "openturns/LinearModelValidation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/NormalityTest.hxx"
#include "openturns/OSS.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Text.hxx"
#include "openturns/Normal.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/FittingTest.hxx"
#include "openturns/VisualTest.hxx"
#include "openturns/FisherSnedecor.hxx"
#include "openturns/DesignProxy.hxx"
#include "openturns/KFoldSplitter.hxx"
#include "openturns/MetaModelValidation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LinearModelValidation)

static const Factory<LinearModelValidation> Factory_LinearModelValidation;

/* Default constructor */
LinearModelValidation::LinearModelValidation()
  : MetaModelValidation()
{
  // Nothing to do
}

/* Parameter constructor */
LinearModelValidation::LinearModelValidation(const LinearModelResult & linearModelResult,
                                             const CrossValidationMethod cvMethod,
                                             const UnsignedInteger & kParameter)
  : MetaModelValidation(linearModelResult.getOutputSample(), 
      ComputeMetamodelCrossValidationPredictions(linearModelResult, cvMethod, kParameter))
  , linearModelResult_(linearModelResult)
{
  if ((cvMethod != LEAVEONEOUT) && (cvMethod != KFOLD))
    throw InvalidArgumentException(HERE) << "The method " << cvMethod << " is not available.";
  cvMethod_ = cvMethod;
  if (kParameter < 1)
    throw InvalidArgumentException(HERE) << "Cannot set k parameter of K-Fold method to " << kParameter << " which is lower than 1";
  const UnsignedInteger sampleSize = linearModelResult_.getSampleResiduals().getSize();
  if (kParameter > sampleSize)
    throw InvalidArgumentException(HERE) << "Cannot set k parameter of K-Fold method to " << kParameter 
      << " which is larger than the sample size =" << sampleSize;
  kParameter_ = kParameter;
  if (ResourceMap::GetAsBool("LinearModelValidation-NoModelSelection") && \
    linearModelResult.getIsModelSelection())
    throw InvalidArgumentException(HERE) << "Cannot perform fast cross-validation "
      << "with a linear model involving model selection";
}

/* Virtual constructor */
LinearModelValidation * LinearModelValidation::clone() const
{
  return new LinearModelValidation(*this);
}


/* String converter */
String LinearModelValidation::__repr__() const
{
  return OSS(true)
         << "class=" << getClassName()
         << ", linearModelResult=" << linearModelResult_
         << ", kParameter_=" << kParameter_
         << ", cvMethod_=" << cvMethod_;
}

/* Compute cross-validation predictions */
Sample LinearModelValidation::ComputeMetamodelCrossValidationPredictions(
    const LinearModelResult & linearModelResult, const CrossValidationMethod cvMethod, 
    const UnsignedInteger & kParameter)
{
  // The residuals is ri = g(xi) - tilde{g}(xi) where g is the model
  // and tilde(g) is the metamodel.
  // Hence the metamodel prediction is tilde{g}(xi) = yi - ri.
  const Sample residualsSample(linearModelResult.getSampleResiduals());
  const UnsignedInteger sampleSize = residualsSample.getSize();
  const UnsignedInteger basisSize = linearModelResult.getBasis().getSize();
  if (basisSize > sampleSize)
    throw InvalidArgumentException(HERE) << "Error: the sample size is: " << sampleSize << 
      " which is lower than the basis size: " << basisSize;
  const Sample outputSample(linearModelResult.getOutputSample());
  Sample cvPredictions(sampleSize, 1);
  if (cvMethod == LEAVEONEOUT)
  {
    const Point diagonalH(linearModelResult.getLeverages());
    for (UnsignedInteger i = 0; i < sampleSize; ++i)
      cvPredictions(i, 0) = outputSample(i, 0) - residualsSample(i, 0) / (1.0 - diagonalH[i]);
  }
  else if (cvMethod == KFOLD)
  {
    SymmetricMatrix projectionMatrix(linearModelResult.computeProjectionMatrix());
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
      const SymmetricMatrix reducedMatrix(IdentityMatrix(foldSize) - projectionKFoldMatrix);
      const Sample residualsSampleKFoldTest(residualsSample.select(indicesTest));
      const Point residualsKFold(reducedMatrix.solveLinearSystem(residualsSampleKFoldTest.asPoint()));
      for (UnsignedInteger i = 0; i < foldSize; ++i)
        cvPredictions(indicesTest[i], 0) = outputSample(indicesTest[i], 0) - residualsKFold[i];
    } // Loop over the folds
  }
  else
    throw InvalidArgumentException(HERE) << "The method " << cvMethod << " is not available.";
  return cvPredictions;
}

/* Get the K parameter */
UnsignedInteger LinearModelValidation::getKParameter() const
{
  return kParameter_;
}

/* Linear model accessor */
LinearModelResult LinearModelValidation::getLinearModelResult() const
{
  return linearModelResult_;
}

/* Get the CV Method */
UnsignedInteger LinearModelValidation::getMethod() const
{
  return cvMethod_;
}

/* Method save() stores the object through the StorageManager */
void LinearModelValidation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "linearModelResult_", linearModelResult_ );
  adv.saveAttribute( "kParameter_", kParameter_ );
}


/* Method load() reloads the object from the StorageManager */
void LinearModelValidation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "linearModelResult_", linearModelResult_ );
  adv.loadAttribute( "kParameter_", kParameter_ );
}

END_NAMESPACE_OPENTURNS
