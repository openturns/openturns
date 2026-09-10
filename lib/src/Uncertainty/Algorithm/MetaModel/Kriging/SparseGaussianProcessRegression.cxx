//                                               -*- C++ -*-
/**
 *  @brief The class building sparse gaussian process regression
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

#include "openturns/SparseGaussianProcessRegression.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SparseGaussianProcessEvaluation.hxx"
#include "openturns/SparseGaussianProcessGradient.hxx"
#include "openturns/SparseGaussianProcessHessian.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SparseGaussianProcessRegression)

static const Factory<SparseGaussianProcessRegression> Factory_SparseGaussianProcessRegression;


/* Default constructor */
SparseGaussianProcessRegression::SparseGaussianProcessRegression()
  : MetaModelAlgorithm()
  , sparseGaussianProcessFitterResult_()
  , result_()
{
  // Nothing to do
}

/* Constructor from a sparse gaussian process fitter result */
SparseGaussianProcessRegression::SparseGaussianProcessRegression(const SparseGaussianProcessFitterResult & result)
  : MetaModelAlgorithm(result.getInputSample(), result.getOutputSample())
  , sparseGaussianProcessFitterResult_(result)
  , result_()
{
  // Nothing to do
}

/* Constructor with parameters */
SparseGaussianProcessRegression::SparseGaussianProcessRegression(const Sample & inputSample,
    const Sample & outputSample,
    const CovarianceModel & covarianceModel,
    const Sample & inducingPoints)
  : MetaModelAlgorithm(inputSample, outputSample)
  , sparseGaussianProcessFitterResult_()
  , result_()
  , covarianceModel_(covarianceModel)
  , inducingPoints_(inducingPoints)
  , needsFit_(true)
{
  // Nothing to do, run() performs the fitting
}

/* Virtual constructor */
SparseGaussianProcessRegression * SparseGaussianProcessRegression::clone() const
{
  return new SparseGaussianProcessRegression(*this);
}

/* Linear algebra method accessor */
SparseGaussianProcessRegression::LinearAlgebra SparseGaussianProcessRegression::getMethod() const
{
  return method_;
}

/* Linear algebra method setter */
void SparseGaussianProcessRegression::setMethod(const LinearAlgebra method)
{
  if (method != method_)
  {
    method_ = method;
    if (needsFit_) hasRun_ = false;
  }
}

/* Perform regression */
void SparseGaussianProcessRegression::run()
{
  if (hasRun_) return;
  // When constructed from a fitter result, no fitting is needed
  if (!needsFit_)
  {
    LOGDEBUG("Build the output meta-model");
    buildMetaModel();
    hasRun_ = true;
    return;
  }
  LOGDEBUG("Fit a sparse gaussian process");
  SparseGaussianProcessFitter algorithm(inputSample_, outputSample_, covarianceModel_, inducingPoints_);
  algorithm.setMethod(method_);
  algorithm.run();
  sparseGaussianProcessFitterResult_ = algorithm.getResult();
  LOGDEBUG("Build the output meta-model");
  buildMetaModel();
  hasRun_ = true;
}

void SparseGaussianProcessRegression::buildMetaModel()
{
  const CovarianceModel covarianceModel(sparseGaussianProcessFitterResult_.getCovarianceModel());
  const Sample inducingPoints(sparseGaussianProcessFitterResult_.getInducingPoints());
  const TriangularMatrix whiteningFactor(sparseGaussianProcessFitterResult_.getWhiteningFactor());
  const HMatrix whiteningFactorHMatrix(sparseGaussianProcessFitterResult_.getWhiteningFactorHMatrix());
  const SparseGaussianProcessFitterResult::LinearAlgebra linearAlgebraMethod(sparseGaussianProcessFitterResult_.getLinearAlgebraMethod());
  const Point posteriorMean(sparseGaussianProcessFitterResult_.getPosteriorMean());
  const CovarianceMatrix posteriorCovariance(sparseGaussianProcessFitterResult_.getPosteriorCovariance());

  Function metaModel;
  metaModel.setEvaluation(new SparseGaussianProcessEvaluation(covarianceModel, inducingPoints, whiteningFactor, posteriorMean, posteriorCovariance, whiteningFactorHMatrix, linearAlgebraMethod));
  metaModel.setGradient(new SparseGaussianProcessGradient(covarianceModel, inducingPoints, whiteningFactor, posteriorMean, whiteningFactorHMatrix, linearAlgebraMethod));
  metaModel.setHessian(new SparseGaussianProcessHessian(covarianceModel, inducingPoints, whiteningFactor, posteriorMean, whiteningFactorHMatrix, linearAlgebraMethod));

  result_ = sparseGaussianProcessFitterResult_;
  result_.setMetaModel(metaModel);
}

/* String converter */
String SparseGaussianProcessRegression::__repr__() const
{
  return OSS() << "class=" << getClassName();
}

/* Result accessor */
SparseGaussianProcessFitterResult SparseGaussianProcessRegression::getResult() const
{
  if (!hasRun_)
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessRegression::getResult, call run() first";
  return result_;
}

/* Method save() stores the object through the StorageManager */
void SparseGaussianProcessRegression::save(Advocate & adv) const
{
  MetaModelAlgorithm::save(adv);
  adv.saveAttribute("sparseGaussianProcessFitterResult_", sparseGaussianProcessFitterResult_);
  adv.saveAttribute("result_", result_);
  adv.saveAttribute("covarianceModel_", covarianceModel_);
  adv.saveAttribute("inducingPoints_", inducingPoints_);
  UnsignedInteger method = static_cast<UnsignedInteger>(method_);
  adv.saveAttribute("method_", method);
  adv.saveAttribute("hasRun_", hasRun_);
  adv.saveAttribute("needsFit_", needsFit_);
}

/* Method load() reloads the object from the StorageManager */
void SparseGaussianProcessRegression::load(Advocate & adv)
{
  MetaModelAlgorithm::load(adv);
  adv.loadAttribute("sparseGaussianProcessFitterResult_", sparseGaussianProcessFitterResult_);
  adv.loadAttribute("result_", result_);
  adv.loadAttribute("covarianceModel_", covarianceModel_);
  adv.loadAttribute("inducingPoints_", inducingPoints_);
  UnsignedInteger method = 0;
  adv.loadAttribute("method_", method);
  method_ = static_cast<LinearAlgebra>(method);
  adv.loadAttribute("hasRun_", hasRun_);
  adv.loadAttribute("needsFit_", needsFit_);
}

END_NAMESPACE_OPENTURNS
