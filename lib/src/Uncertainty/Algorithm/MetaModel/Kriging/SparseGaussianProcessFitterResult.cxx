//                                               -*- C++ -*-
/**
 *  @brief The result of a sparse gaussian process fitter
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
#include "openturns/SparseGaussianProcessFitterResult.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SparseGaussianProcessFitterResult)
static const Factory<SparseGaussianProcessFitterResult> Factory_SparseGaussianProcessFitterResult;

/* Default constructor */
SparseGaussianProcessFitterResult::SparseGaussianProcessFitterResult()
  : MetaModelResult()
{
  // Nothing to do
}

/* Constructor with parameters */
SparseGaussianProcessFitterResult::SparseGaussianProcessFitterResult(const Sample & inputSample,
    const Sample & outputSample,
    const CovarianceModel & covarianceModel,
    const Sample & inducingPoints,
    const TriangularMatrix & whiteningFactor,
    const Point & posteriorMean,
    const CovarianceMatrix & posteriorCovariance,
    const Scalar noiseStdDev,
    const Scalar optimalELBO,
    const Function & metaModel,
    const LinearAlgebra linearAlgebraMethod)
  : MetaModelResult(inputSample, outputSample, metaModel)
  , covarianceModel_(covarianceModel)
  , inducingPoints_(inducingPoints)
  , whiteningFactor_(whiteningFactor)
  , linearAlgebraMethod_(linearAlgebraMethod)
  , posteriorMean_(posteriorMean)
  , posteriorCovariance_(posteriorCovariance)
  , noiseStdDev_(noiseStdDev)
  , optimalELBO_(optimalELBO)
{
  const UnsignedInteger size = inputSample.getSize();
  if (size != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessFitterResult, mismatched input sample size=" << size << " and output sample size=" << outputSample.getSize();
}

/* Virtual constructor */
SparseGaussianProcessFitterResult * SparseGaussianProcessFitterResult::clone() const
{
  return new SparseGaussianProcessFitterResult(*this);
}

/* String converter */
String SparseGaussianProcessFitterResult::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << ", covariance model=" << covarianceModel_
         << ", inducing points=" << inducingPoints_
         << ", noiseStdDev=" << noiseStdDev_
         << ", optimal ELBO=" << optimalELBO_;
}

String SparseGaussianProcessFitterResult::__str__(const String & offset) const
{
  OSS oss(false);
  oss << getClassName() << "("
      << "covariance model=" << covarianceModel_.__str__(offset)
      << ", inducing points=" << inducingPoints_.__str__(offset)
      << ", noise variance=" << noiseStdDev_
      << ", optimal ELBO=" << optimalELBO_ << ")";
  return oss;
}

/* Covariance model accessor */
CovarianceModel SparseGaussianProcessFitterResult::getCovarianceModel() const
{
  return covarianceModel_;
}

/* Inducing points accessor */
Sample SparseGaussianProcessFitterResult::getInducingPoints() const
{
  return inducingPoints_;
}

/* Whitening factor accessor */
TriangularMatrix SparseGaussianProcessFitterResult::getWhiteningFactor() const
{
  return whiteningFactor_;
}

/* Whitening factor accessor, in HMAT form */
HMatrix SparseGaussianProcessFitterResult::getWhiteningFactorHMatrix() const
{
  return whiteningFactorHMatrix_;
}

/* Whitening factor setter */
void SparseGaussianProcessFitterResult::setWhiteningFactorHMatrix(const HMatrix & whiteningFactorHMatrix)
{
  whiteningFactorHMatrix_ = whiteningFactorHMatrix;
}

/* Linear algebra method accessor */
SparseGaussianProcessFitterResult::LinearAlgebra SparseGaussianProcessFitterResult::getLinearAlgebraMethod() const
{
  return linearAlgebraMethod_;
}

/* Posterior mean accessor */
Point SparseGaussianProcessFitterResult::getPosteriorMean() const
{
  return posteriorMean_;
}

/* Posterior covariance accessor */
CovarianceMatrix SparseGaussianProcessFitterResult::getPosteriorCovariance() const
{
  return posteriorCovariance_;
}

/* Noise variance accessor */
Scalar SparseGaussianProcessFitterResult::getNoiseStdDev() const
{
  return noiseStdDev_;
}

/* optimal ELBO accessor */
Scalar SparseGaussianProcessFitterResult::getOptimalELBO() const
{
  return optimalELBO_;
}

/* Conditional variance accessor */
Scalar SparseGaussianProcessFitterResult::getConditionalVariance(const Point & point) const
{
  if (point.getDimension() != covarianceModel_.getInputDimension())
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessFitterResult::getConditionalVariance, input point should have the same dimension as the covariance model input dimension. Here, point dimension = " << point.getDimension()
                                         << " and covariance model input dimension = " << covarianceModel_.getInputDimension();
  if (covarianceModel_.getOutputDimension() != 1)
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessFitterResult::getConditionalVariance, the covariance model must have output dimension 1, here output dimension = " << covarianceModel_.getOutputDimension();
  // Whitened cross-covariance a = Luu^{-1} k(Z, point)
  const Matrix kZx(covarianceModel_.computeCrossCovariance(inducingPoints_, point));
  Point kZX(inducingPoints_.getSize());
  for (UnsignedInteger i = 0; i < kZx.getNbRows(); ++i)
    kZX[i] = kZx(i, 0);
  Point a;
  if (linearAlgebraMethod_ == HMAT)
    a = whiteningFactorHMatrix_.solveLower(kZX);
  else
    a = whiteningFactor_.solveLinearSystem(kZX);
  // k(point, point)
  const Scalar kxx = covarianceModel_(point, point)(0, 0);
  // a^T S_ww a
  const Point Swwa(posteriorCovariance_ * a);
  // v = k(x, x) - a^T a + a^T S_ww a
  return kxx - a.normSquare() + a.dot(Swwa);
}

Point SparseGaussianProcessFitterResult::getConditionalVariance(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  Point result(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    result[i] = getConditionalVariance(sample[i]);
  return result;
}

/* Method save() stores the object through the StorageManager */
void SparseGaussianProcessFitterResult::save(Advocate & adv) const
{
  MetaModelResult::save(adv);
  adv.saveAttribute("covarianceModel_", covarianceModel_);
  adv.saveAttribute("inducingPoints_", inducingPoints_);
  adv.saveAttribute("whiteningFactor_", whiteningFactor_);
  UnsignedInteger linearAlgebraMethod = static_cast<UnsignedInteger>(linearAlgebraMethod_);
  adv.saveAttribute("linearAlgebraMethod_", linearAlgebraMethod);
  adv.saveAttribute("posteriorMean_", posteriorMean_);
  adv.saveAttribute("posteriorCovariance_", posteriorCovariance_);
  adv.saveAttribute("noiseStdDev_", noiseStdDev_);
  adv.saveAttribute("optimalELBO_", optimalELBO_);
}

/* Method load() reloads the object from the StorageManager */
void SparseGaussianProcessFitterResult::load(Advocate & adv)
{
  MetaModelResult::load(adv);
  adv.loadAttribute("covarianceModel_", covarianceModel_);
  adv.loadAttribute("inducingPoints_", inducingPoints_);
  adv.loadAttribute("whiteningFactor_", whiteningFactor_);
  UnsignedInteger linearAlgebraMethod = 0;
  adv.loadAttribute("linearAlgebraMethod_", linearAlgebraMethod);
  linearAlgebraMethod_ = static_cast<LinearAlgebra>(linearAlgebraMethod);
  if (linearAlgebraMethod_ != LAPACK)
  {
    LOGDEBUG("SparseGaussianProcessFitterResult::load: forcing LAPACK after reload (HMatrix is not serializable)");
    linearAlgebraMethod_ = LAPACK;
    whiteningFactor_ = covarianceModel_.discretize(inducingPoints_).computeRegularizedCholesky();
  }
  adv.loadAttribute("posteriorMean_", posteriorMean_);
  adv.loadAttribute("posteriorCovariance_", posteriorCovariance_);
  adv.loadAttribute("noiseStdDev_", noiseStdDev_);
  adv.loadAttribute("optimalELBO_", optimalELBO_);
}

END_NAMESPACE_OPENTURNS
