//                                               -*- C++ -*-
/**
 *  @brief The class building sparse gaussian process regression gradient
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

#include "openturns/SparseGaussianProcessGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SparseGaussianProcessGradient)

static const Factory<SparseGaussianProcessGradient> Factory_SparseGaussianProcessGradient;


/* Constructor with parameters */
SparseGaussianProcessGradient::SparseGaussianProcessGradient()
  : GradientImplementation()
  , covarianceModel_()
  , inducingPoints_()
  , whiteningFactor_()
  , posteriorMean_()
{
  // Nothing to do here
}


/* Constructor with parameters */
SparseGaussianProcessGradient::SparseGaussianProcessGradient(const CovarianceModel & covarianceModel,
    const Sample & inducingPoints,
    const TriangularMatrix & whiteningFactor,
    const Point & posteriorMean,
    const HMatrix & whiteningFactorHMatrix,
    const SparseGaussianProcessFitterResult::LinearAlgebra linearAlgebraMethod)
  : GradientImplementation()
  , covarianceModel_(covarianceModel)
  , inducingPoints_(inducingPoints)
  , whiteningFactor_(whiteningFactor)
  , whiteningFactorHMatrix_(whiteningFactorHMatrix)
  , linearAlgebraMethod_(linearAlgebraMethod)
  , posteriorMean_(posteriorMean)
{
  if (covarianceModel.getInputDimension() != inducingPoints.getDimension())
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessGradient::SparseGaussianProcessGradient, the input dimension=" << covarianceModel.getInputDimension() << " of the covariance model should match the dimension=" << inducingPoints.getDimension() << " of the inducing points";
  const UnsignedInteger M = inducingPoints.getSize();
  if ((linearAlgebraMethod == SparseGaussianProcessFitterResult::LAPACK) && (whiteningFactor.getDimension() != M))
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessGradient::SparseGaussianProcessGradient, the whitening factor dimension=" << whiteningFactor.getDimension() << " should match the number of inducing points=" << M;
  if ((linearAlgebraMethod == SparseGaussianProcessFitterResult::HMAT) && (whiteningFactorHMatrix.getNbRows() != M))
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessGradient::SparseGaussianProcessGradient, the whitening factor HMatrix size=" << whiteningFactorHMatrix.getNbRows() << " should match the number of inducing points=" << M;
  if (posteriorMean.getDimension() != M)
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessGradient::SparseGaussianProcessGradient, the posterior mean dimension=" << posteriorMean.getDimension() << " should match the number of inducing points=" << M;
}

/* Virtual constructor */
SparseGaussianProcessGradient * SparseGaussianProcessGradient::clone() const
{
  return new SparseGaussianProcessGradient(*this);
}
/* Comparison operator */
Bool SparseGaussianProcessGradient::operator ==(const SparseGaussianProcessGradient & other) const
{
  if (this == &other) return true;
  return (covarianceModel_ == other.covarianceModel_) && (inducingPoints_ == other.inducingPoints_) && (whiteningFactor_ == other.whiteningFactor_) && (posteriorMean_ == other.posteriorMean_) && (linearAlgebraMethod_ == other.linearAlgebraMethod_);
}
Bool SparseGaussianProcessGradient::equals(const GradientImplementation & other) const
{
  const SparseGaussianProcessGradient * p_other = dynamic_cast<const SparseGaussianProcessGradient *>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String SparseGaussianProcessGradient::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " covarianceModel=" << covarianceModel_
      << " inducingPoints=" << inducingPoints_
      << " posteriorMean=" << posteriorMean_;
  return oss;
}

/* String converter */
String SparseGaussianProcessGradient::__str__(const String & ) const
{
  return OSS(false) << GetClassName();
}

Matrix SparseGaussianProcessGradient::gradient(const Point & inP) const
{
  const UnsignedInteger p = inP.getSize();
  if (p != getInputDimension())
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessGradient::gradient, input point should have the same dimension as the SparseGaussianProcessGradient input dimension. Here, inP dimension = " << inP.getSize()
                                         << " and SparseGaussianProcessGradient dimension = " << getInputDimension();

  // alpha = Luu^{-T} m_w
  Point alpha;
  if (linearAlgebraMethod_ == SparseGaussianProcessFitterResult::HMAT)
    alpha = whiteningFactorHMatrix_.solveLower(posteriorMean_, true);
  else
    alpha = whiteningFactor_.transpose().solveLinearSystem(posteriorMean_);
  const UnsignedInteger M = inducingPoints_.getSize();
  // result(j, 0) = sum_m alpha[m] * dk(x, Z_m)/dx_j
  Matrix result(p, getOutputDimension());
  for (UnsignedInteger j = 0; j < p; ++j)
  {
    Scalar value = 0.0;
    for (UnsignedInteger m = 0; m < M; ++m)
    {
      const Matrix partialGradient(covarianceModel_.partialGradient(inP, inducingPoints_[m]));
      value += alpha[m] * partialGradient(j, 0);
    }
    result(j, 0) = value;
  }
  callsNumber_.fetchAndAdd(p);
  return result;
}


/* Accessor for input point dimension */
UnsignedInteger SparseGaussianProcessGradient::getInputDimension() const
{
  return covarianceModel_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger SparseGaussianProcessGradient::getOutputDimension() const
{
  return covarianceModel_.getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void SparseGaussianProcessGradient::save(Advocate & adv) const
{
  GradientImplementation::save(adv);
  adv.saveAttribute("covarianceModel_", covarianceModel_);
  adv.saveAttribute("inducingPoints_", inducingPoints_);
  adv.saveAttribute("whiteningFactor_", whiteningFactor_);
  UnsignedInteger linearAlgebraMethod = static_cast<UnsignedInteger>(linearAlgebraMethod_);
  adv.saveAttribute("linearAlgebraMethod_", linearAlgebraMethod);
  adv.saveAttribute("posteriorMean_", posteriorMean_);
}

/* Method load() reloads the object from the StorageManager */
void SparseGaussianProcessGradient::load(Advocate & adv)
{
  GradientImplementation::load(adv);
  adv.loadAttribute("covarianceModel_", covarianceModel_);
  adv.loadAttribute("inducingPoints_", inducingPoints_);
  adv.loadAttribute("whiteningFactor_", whiteningFactor_);
  UnsignedInteger linearAlgebraMethod = 0;
  adv.loadAttribute("linearAlgebraMethod_", linearAlgebraMethod);
  linearAlgebraMethod_ = static_cast<SparseGaussianProcessFitterResult::LinearAlgebra>(linearAlgebraMethod);
  if (linearAlgebraMethod_ != SparseGaussianProcessFitterResult::LAPACK)
  {
    LOGDEBUG("SparseGaussianProcessGradient::load: forcing LAPACK after reload (HMatrix is not serializable)");
    linearAlgebraMethod_ = SparseGaussianProcessFitterResult::LAPACK;
    whiteningFactor_ = covarianceModel_.discretize(inducingPoints_).computeRegularizedCholesky();
  }
  adv.loadAttribute("posteriorMean_", posteriorMean_);
}

END_NAMESPACE_OPENTURNS
