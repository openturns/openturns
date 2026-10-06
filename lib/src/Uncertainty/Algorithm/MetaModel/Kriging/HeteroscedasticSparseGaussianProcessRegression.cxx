//                                               -*- C++ -*-
/**
 *  @brief The class building heteroscedastic sparse gaussian process regression
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

#include "openturns/HeteroscedasticSparseGaussianProcessRegression.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SparseGaussianProcessEvaluation.hxx"
#include "openturns/SparseGaussianProcessGradient.hxx"
#include "openturns/SparseGaussianProcessHessian.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HeteroscedasticSparseGaussianProcessRegression)

static const Factory<HeteroscedasticSparseGaussianProcessRegression> Factory_HeteroscedasticSparseGaussianProcessRegression;


/* Default constructor */
HeteroscedasticSparseGaussianProcessRegression::HeteroscedasticSparseGaussianProcessRegression()
  : MetaModelAlgorithm()
  , heteroscedasticSparseGaussianProcessFitterResult_()
  , result_()
{
  // Nothing to do
}

/* Constructor from a heteroscedastic sparse gaussian process fitter result */
HeteroscedasticSparseGaussianProcessRegression::HeteroscedasticSparseGaussianProcessRegression(const HeteroscedasticSparseGaussianProcessFitterResult & result)
  : MetaModelAlgorithm(result.getInputSample(), result.getOutputSample())
  , heteroscedasticSparseGaussianProcessFitterResult_(result)
  , result_()
{
  // Nothing to do
}

/* Constructor with parameters */
HeteroscedasticSparseGaussianProcessRegression::HeteroscedasticSparseGaussianProcessRegression(const Sample & inputSample,
    const Sample & outputSample,
    const CovarianceModel & covarianceModelF,
    const CovarianceModel & covarianceModelG,
    const Sample & inducingPointsF,
    const Sample & inducingPointsG)
  : MetaModelAlgorithm(inputSample, outputSample)
  , heteroscedasticSparseGaussianProcessFitterResult_()
  , result_()
  , covarianceModelF_(covarianceModelF)
  , covarianceModelG_(covarianceModelG)
  , inducingPointsF_(inducingPointsF)
  , inducingPointsG_(inducingPointsG)
  , needsFit_(true)
{
  // Nothing to do, run() performs the fitting
}

/* Virtual constructor */
HeteroscedasticSparseGaussianProcessRegression * HeteroscedasticSparseGaussianProcessRegression::clone() const
{
  return new HeteroscedasticSparseGaussianProcessRegression(*this);
}

/* Perform regression */
void HeteroscedasticSparseGaussianProcessRegression::run()
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
  LOGDEBUG("Fit a heteroscedastic sparse gaussian process");
  HeteroscedasticSparseGaussianProcessFitter algorithm(inputSample_, outputSample_, covarianceModelF_, covarianceModelG_, inducingPointsF_, inducingPointsG_);
  algorithm.run();
  heteroscedasticSparseGaussianProcessFitterResult_ = algorithm.getResult();
  LOGDEBUG("Build the output meta-model");
  buildMetaModel();
  hasRun_ = true;
}

void HeteroscedasticSparseGaussianProcessRegression::buildMetaModel()
{
  const CovarianceModel covarianceModel(heteroscedasticSparseGaussianProcessFitterResult_.getCovarianceModelF());
  const Sample inducingPoints(heteroscedasticSparseGaussianProcessFitterResult_.getInducingPointsF());
  const TriangularMatrix whiteningFactor(heteroscedasticSparseGaussianProcessFitterResult_.getWhiteningFactorF());
  const Point posteriorMean(heteroscedasticSparseGaussianProcessFitterResult_.getPosteriorMeanF());
  const CovarianceMatrix posteriorCovariance(heteroscedasticSparseGaussianProcessFitterResult_.getPosteriorCovarianceF());

  Function metaModel;
  metaModel.setEvaluation(new SparseGaussianProcessEvaluation(covarianceModel, inducingPoints, whiteningFactor, posteriorMean, posteriorCovariance, HMatrix(), SparseGaussianProcessFitterResult::LAPACK));
  metaModel.setGradient(new SparseGaussianProcessGradient(covarianceModel, inducingPoints, whiteningFactor, posteriorMean, HMatrix(), SparseGaussianProcessFitterResult::LAPACK));
  metaModel.setHessian(new SparseGaussianProcessHessian(covarianceModel, inducingPoints, whiteningFactor, posteriorMean, HMatrix(), SparseGaussianProcessFitterResult::LAPACK));
  metaModel.setInputDescription(inputSample_.getDescription());
  metaModel.setOutputDescription(outputSample_.getDescription());

  result_ = heteroscedasticSparseGaussianProcessFitterResult_;
  result_.setMetaModel(metaModel);
}

/* String converter */
String HeteroscedasticSparseGaussianProcessRegression::__repr__() const
{
  return OSS() << "class=" << getClassName();
}

/* Result accessor */
HeteroscedasticSparseGaussianProcessFitterResult HeteroscedasticSparseGaussianProcessRegression::getResult() const
{
  if (!hasRun_)
    throw InvalidArgumentException(HERE) << "In HeteroscedasticSparseGaussianProcessRegression::getResult, call run() first";
  return result_;
}

/* Method save() stores the object through the StorageManager */
void HeteroscedasticSparseGaussianProcessRegression::save(Advocate & adv) const
{
  MetaModelAlgorithm::save(adv);
  adv.saveAttribute("heteroscedasticSparseGaussianProcessFitterResult_", heteroscedasticSparseGaussianProcessFitterResult_);
  adv.saveAttribute("result_", result_);
  adv.saveAttribute("covarianceModelF_", covarianceModelF_);
  adv.saveAttribute("covarianceModelG_", covarianceModelG_);
  adv.saveAttribute("inducingPointsF_", inducingPointsF_);
  adv.saveAttribute("inducingPointsG_", inducingPointsG_);
  adv.saveAttribute("hasRun_", hasRun_);
  adv.saveAttribute("needsFit_", needsFit_);
}

/* Method load() reloads the object from the StorageManager */
void HeteroscedasticSparseGaussianProcessRegression::load(Advocate & adv)
{
  MetaModelAlgorithm::load(adv);
  adv.loadAttribute("heteroscedasticSparseGaussianProcessFitterResult_", heteroscedasticSparseGaussianProcessFitterResult_);
  adv.loadAttribute("result_", result_);
  adv.loadAttribute("covarianceModelF_", covarianceModelF_);
  adv.loadAttribute("covarianceModelG_", covarianceModelG_);
  adv.loadAttribute("inducingPointsF_", inducingPointsF_);
  adv.loadAttribute("inducingPointsG_", inducingPointsG_);
  adv.loadAttribute("hasRun_", hasRun_);
  adv.loadAttribute("needsFit_", needsFit_);
}

END_NAMESPACE_OPENTURNS
