//                                               -*- C++ -*-
/**
 *  @brief The result of a heteroscedastic sparse gaussian process fitter
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
#include "openturns/HeteroscedasticSparseGaussianProcessFitterResult.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HeteroscedasticSparseGaussianProcessFitterResult)
static const Factory<HeteroscedasticSparseGaussianProcessFitterResult> Factory_HeteroscedasticSparseGaussianProcessFitterResult;

/* Default constructor */
HeteroscedasticSparseGaussianProcessFitterResult::HeteroscedasticSparseGaussianProcessFitterResult()
  : MetaModelResult()
{
  // Nothing to do
}

/* Constructor with parameters */
HeteroscedasticSparseGaussianProcessFitterResult::HeteroscedasticSparseGaussianProcessFitterResult(const Sample & inputSample,
    const Sample & outputSample,
    const CovarianceModel & covarianceModelF,
    const CovarianceModel & covarianceModelG,
    const Sample & inducingPointsF,
    const Sample & inducingPointsG,
    const TriangularMatrix & whiteningFactorF,
    const TriangularMatrix & whiteningFactorG,
    const Point & posteriorMeanF,
    const CovarianceMatrix & posteriorCovarianceF,
    const Point & posteriorMeanG,
    const CovarianceMatrix & posteriorCovarianceG,
    const Scalar mu0,
    const Scalar optimalELBO,
    const Function & metaModel)
  : MetaModelResult(inputSample, outputSample, metaModel)
  , covarianceModelF_(covarianceModelF)
  , covarianceModelG_(covarianceModelG)
  , inducingPointsF_(inducingPointsF)
  , inducingPointsG_(inducingPointsG)
  , whiteningFactorF_(whiteningFactorF)
  , whiteningFactorG_(whiteningFactorG)
  , posteriorMeanF_(posteriorMeanF)
  , posteriorCovarianceF_(posteriorCovarianceF)
  , posteriorMeanG_(posteriorMeanG)
  , posteriorCovarianceG_(posteriorCovarianceG)
  , mu0_(mu0)
  , optimalELBO_(optimalELBO)
{
  const UnsignedInteger size = inputSample.getSize();
  if (size != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "In HeteroscedasticSparseGaussianProcessFitterResult, mismatched input sample size=" << size << " and output sample size=" << outputSample.getSize();
}

/* Virtual constructor */
HeteroscedasticSparseGaussianProcessFitterResult * HeteroscedasticSparseGaussianProcessFitterResult::clone() const
{
  return new HeteroscedasticSparseGaussianProcessFitterResult(*this);
}

/* String converter */
String HeteroscedasticSparseGaussianProcessFitterResult::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << ", covariance model F=" << covarianceModelF_
         << ", covariance model G=" << covarianceModelG_
         << ", mu0=" << mu0_
         << ", optimal ELBO=" << optimalELBO_;
}

String HeteroscedasticSparseGaussianProcessFitterResult::__str__(const String & offset) const
{
  OSS oss(false);
  oss << getClassName() << "("
      << "covariance model F=" << covarianceModelF_.__str__(offset)
      << ", covariance model G=" << covarianceModelG_.__str__(offset)
      << ", mu0=" << mu0_
      << ", optimal ELBO=" << optimalELBO_ << ")";
  return oss;
}

/* Covariance models accessors */
CovarianceModel HeteroscedasticSparseGaussianProcessFitterResult::getCovarianceModelF() const
{
  return covarianceModelF_;
}

CovarianceModel HeteroscedasticSparseGaussianProcessFitterResult::getCovarianceModelG() const
{
  return covarianceModelG_;
}

/* Inducing points accessors */
Sample HeteroscedasticSparseGaussianProcessFitterResult::getInducingPointsF() const
{
  return inducingPointsF_;
}

Sample HeteroscedasticSparseGaussianProcessFitterResult::getInducingPointsG() const
{
  return inducingPointsG_;
}

/* Whitening factors accessors */
TriangularMatrix HeteroscedasticSparseGaussianProcessFitterResult::getWhiteningFactorF() const
{
  return whiteningFactorF_;
}

TriangularMatrix HeteroscedasticSparseGaussianProcessFitterResult::getWhiteningFactorG() const
{
  return whiteningFactorG_;
}

/* Posterior moments accessors */
Point HeteroscedasticSparseGaussianProcessFitterResult::getPosteriorMeanF() const
{
  return posteriorMeanF_;
}

CovarianceMatrix HeteroscedasticSparseGaussianProcessFitterResult::getPosteriorCovarianceF() const
{
  return posteriorCovarianceF_;
}

Point HeteroscedasticSparseGaussianProcessFitterResult::getPosteriorMeanG() const
{
  return posteriorMeanG_;
}

CovarianceMatrix HeteroscedasticSparseGaussianProcessFitterResult::getPosteriorCovarianceG() const
{
  return posteriorCovarianceG_;
}

/* Prior mean of the log-variance process accessor */
Scalar HeteroscedasticSparseGaussianProcessFitterResult::getMu0() const
{
  return mu0_;
}

/* optimal ELBO accessor */
Scalar HeteroscedasticSparseGaussianProcessFitterResult::getOptimalELBO() const
{
  return optimalELBO_;
}

/* Whitened cross-covariance of a process at a point */
Point HeteroscedasticSparseGaussianProcessFitterResult::computeWhitenedVector(const CovarianceModel & covarianceModel,
    const Sample & inducingPoints,
    const TriangularMatrix & whiteningFactor,
    const Point & point) const
{
  const Matrix kZx(covarianceModel.computeCrossCovariance(inducingPoints, point));
  Point kZX(inducingPoints.getSize());
  for (UnsignedInteger i = 0; i < kZx.getNbRows(); ++i)
    kZX[i] = kZx(i, 0);
  return whiteningFactor.solveLinearSystem(kZX);
}

/* Conditional variance accessor (latent mean process) */
Scalar HeteroscedasticSparseGaussianProcessFitterResult::getConditionalVariance(const Point & point) const
{
  if (point.getDimension() != covarianceModelF_.getInputDimension())
    throw InvalidArgumentException(HERE) << "In HeteroscedasticSparseGaussianProcessFitterResult::getConditionalVariance, input point should have the same dimension as the covariance model input dimension. Here, point dimension = " << point.getDimension()
                                         << " and covariance model input dimension = " << covarianceModelF_.getInputDimension();
  if (covarianceModelF_.getOutputDimension() != 1)
    throw InvalidArgumentException(HERE) << "In HeteroscedasticSparseGaussianProcessFitterResult::getConditionalVariance, the covariance model must have output dimension 1, here output dimension = " << covarianceModelF_.getOutputDimension();
  // Whitened cross-covariance a = Luu^{-1} k(Z, point)
  const Point a(computeWhitenedVector(covarianceModelF_, inducingPointsF_, whiteningFactorF_, point));
  // k(point, point)
  const Scalar kxx = covarianceModelF_(point, point)(0, 0);
  // a^T S_ww a
  const Point Swwa(posteriorCovarianceF_ * a);
  // v = k(x, x) - a^T a + a^T S_ww a
  return kxx - a.normSquare() + a.dot(Swwa);
}

Point HeteroscedasticSparseGaussianProcessFitterResult::getConditionalVariance(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  Point result(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    result[i] = getConditionalVariance(sample[i]);
  return result;
}

/* Predictive variance accessor (latent process plus lognormal noise moment) */
Scalar HeteroscedasticSparseGaussianProcessFitterResult::getPredictiveVariance(const Point & point) const
{
  if (point.getDimension() != covarianceModelG_.getInputDimension())
    throw InvalidArgumentException(HERE) << "In HeteroscedasticSparseGaussianProcessFitterResult::getPredictiveVariance, input point should have the same dimension as the covariance model input dimension. Here, point dimension = " << point.getDimension()
                                          << " and covariance model input dimension = " << covarianceModelG_.getInputDimension();
  const Scalar conditionalVariance = getConditionalVariance(point);
  // Log-variance marginal moments
  const Point b(computeWhitenedVector(covarianceModelG_, inducingPointsG_, whiteningFactorG_, point));
  Scalar muG = mu0_;
  for (UnsignedInteger j = 0; j < b.getSize(); ++j)
    muG += b[j] * posteriorMeanG_[j];
  const Scalar kxx = covarianceModelG_(point, point)(0, 0);
  const Point Swwb(posteriorCovarianceG_ * b);
  const Scalar vg = kxx - b.normSquare() + b.dot(Swwb);
  // v = v_f + E[exp(g)] with the lognormal moment
  return conditionalVariance + std::exp(muG + 0.5 * vg);
}

Point HeteroscedasticSparseGaussianProcessFitterResult::getPredictiveVariance(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  Point result(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    result[i] = getPredictiveVariance(sample[i]);
  return result;
}

/* Sequential design criteria: Active Learning MacKay (maximal predictive variance) */
UnsignedInteger HeteroscedasticSparseGaussianProcessFitterResult::computeALM(const Sample & candidateSample) const
{
  const UnsignedInteger size = candidateSample.getSize();
  if (size == 0)
    throw InvalidArgumentException(HERE) << "In HeteroscedasticSparseGaussianProcessFitterResult::computeALM, the candidate sample should not be empty";
  UnsignedInteger best = 0;
  Scalar bestVariance = getPredictiveVariance(candidateSample[0]);
  for (UnsignedInteger i = 1; i < size; ++i)
  {
    const Scalar variance = getPredictiveVariance(candidateSample[i]);
    if (variance > bestVariance)
    {
      bestVariance = variance;
      best = i;
    }
  }
  return best;
}

/* Sequential design criteria: integrated predictive variance over a reference sample */
Scalar HeteroscedasticSparseGaussianProcessFitterResult::computeIMSPE(const Sample & referenceSample) const
{
  const UnsignedInteger size = referenceSample.getSize();
  if (size == 0)
    throw InvalidArgumentException(HERE) << "In HeteroscedasticSparseGaussianProcessFitterResult::computeIMSPE, the reference sample should not be empty";
  Scalar total = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
    total += getPredictiveVariance(referenceSample[i]);
  return total / size;
}

/* Method save() stores the object through the StorageManager */
void HeteroscedasticSparseGaussianProcessFitterResult::save(Advocate & adv) const
{
  MetaModelResult::save(adv);
  adv.saveAttribute("covarianceModelF_", covarianceModelF_);
  adv.saveAttribute("covarianceModelG_", covarianceModelG_);
  adv.saveAttribute("inducingPointsF_", inducingPointsF_);
  adv.saveAttribute("inducingPointsG_", inducingPointsG_);
  adv.saveAttribute("whiteningFactorF_", whiteningFactorF_);
  adv.saveAttribute("whiteningFactorG_", whiteningFactorG_);
  adv.saveAttribute("posteriorMeanF_", posteriorMeanF_);
  adv.saveAttribute("posteriorCovarianceF_", posteriorCovarianceF_);
  adv.saveAttribute("posteriorMeanG_", posteriorMeanG_);
  adv.saveAttribute("posteriorCovarianceG_", posteriorCovarianceG_);
  adv.saveAttribute("mu0_", mu0_);
  adv.saveAttribute("optimalELBO_", optimalELBO_);
}

/* Method load() reloads the object from the StorageManager */
void HeteroscedasticSparseGaussianProcessFitterResult::load(Advocate & adv)
{
  MetaModelResult::load(adv);
  adv.loadAttribute("covarianceModelF_", covarianceModelF_);
  adv.loadAttribute("covarianceModelG_", covarianceModelG_);
  adv.loadAttribute("inducingPointsF_", inducingPointsF_);
  adv.loadAttribute("inducingPointsG_", inducingPointsG_);
  adv.loadAttribute("whiteningFactorF_", whiteningFactorF_);
  adv.loadAttribute("whiteningFactorG_", whiteningFactorG_);
  adv.loadAttribute("posteriorMeanF_", posteriorMeanF_);
  adv.loadAttribute("posteriorCovarianceF_", posteriorCovarianceF_);
  adv.loadAttribute("posteriorMeanG_", posteriorMeanG_);
  adv.loadAttribute("posteriorCovarianceG_", posteriorCovarianceG_);
  adv.loadAttribute("mu0_", mu0_);
  adv.loadAttribute("optimalELBO_", optimalELBO_);
}

END_NAMESPACE_OPENTURNS
