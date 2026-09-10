//                                               -*- C++ -*-
/**
 *  @brief The class building sparse gaussian process regression hessian
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

#include "openturns/SparseGaussianProcessHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SparseGaussianProcessHessian)

static const Factory<SparseGaussianProcessHessian> Factory_SparseGaussianProcessHessian;


/* Constructor with parameters */
SparseGaussianProcessHessian::SparseGaussianProcessHessian()
  : HessianImplementation()
  , covarianceModel_()
  , inducingPoints_()
  , whiteningFactor_()
  , posteriorMean_()
{
  // Nothing to do here
}


/* Constructor with parameters */
SparseGaussianProcessHessian::SparseGaussianProcessHessian(const CovarianceModel & covarianceModel,
    const Sample & inducingPoints,
    const TriangularMatrix & whiteningFactor,
    const Point & posteriorMean,
    const HMatrix & whiteningFactorHMatrix,
    const SparseGaussianProcessFitterResult::LinearAlgebra linearAlgebraMethod)
  : HessianImplementation()
  , covarianceModel_(covarianceModel)
  , inducingPoints_(inducingPoints)
  , whiteningFactor_(whiteningFactor)
  , whiteningFactorHMatrix_(whiteningFactorHMatrix)
  , linearAlgebraMethod_(linearAlgebraMethod)
  , posteriorMean_(posteriorMean)
{
  if (covarianceModel.getInputDimension() != inducingPoints.getDimension())
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessHessian::SparseGaussianProcessHessian, the input dimension=" << covarianceModel.getInputDimension() << " of the covariance model should match the dimension=" << inducingPoints.getDimension() << " of the inducing points";
  const UnsignedInteger M = inducingPoints.getSize();
  if ((linearAlgebraMethod == SparseGaussianProcessFitterResult::LAPACK) && (whiteningFactor.getDimension() != M))
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessHessian::SparseGaussianProcessHessian, the whitening factor dimension=" << whiteningFactor.getDimension() << " should match the number of inducing points=" << M;
  if ((linearAlgebraMethod == SparseGaussianProcessFitterResult::HMAT) && (whiteningFactorHMatrix.getNbRows() != M))
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessHessian::SparseGaussianProcessHessian, the whitening factor HMatrix size=" << whiteningFactorHMatrix.getNbRows() << " should match the number of inducing points=" << M;
  if (posteriorMean.getDimension() != M)
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessHessian::SparseGaussianProcessHessian, the posterior mean dimension=" << posteriorMean.getDimension() << " should match the number of inducing points=" << M;
}

/* Virtual constructor */
SparseGaussianProcessHessian * SparseGaussianProcessHessian::clone() const
{
  return new SparseGaussianProcessHessian(*this);
}
/* Comparison operator */
Bool SparseGaussianProcessHessian::operator ==(const SparseGaussianProcessHessian & other) const
{
  if (this == &other) return true;
  return (covarianceModel_ == other.covarianceModel_) && (inducingPoints_ == other.inducingPoints_) && (whiteningFactor_ == other.whiteningFactor_) && (posteriorMean_ == other.posteriorMean_) && (linearAlgebraMethod_ == other.linearAlgebraMethod_);
}
Bool SparseGaussianProcessHessian::equals(const HessianImplementation & other) const
{
  const SparseGaussianProcessHessian * p_other = dynamic_cast<const SparseGaussianProcessHessian *>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String SparseGaussianProcessHessian::__repr__() const
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
String SparseGaussianProcessHessian::__str__(const String & ) const
{
  return OSS(false) << GetClassName();
}

SymmetricTensor SparseGaussianProcessHessian::hessian(const Point & inP) const
{
  const UnsignedInteger p = inP.getSize();
  if (p != getInputDimension())
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessHessian::hessian, input point should have the same dimension as the SparseGaussianProcessHessian input dimension. Here, inP dimension = " << inP.getSize()
                                         << " and SparseGaussianProcessHessian dimension = " << getInputDimension();

  // alpha = Luu^{-T} m_w
  Point alpha;
  if (linearAlgebraMethod_ == SparseGaussianProcessFitterResult::HMAT)
    alpha = whiteningFactorHMatrix_.solveLower(posteriorMean_, true);
  else
    alpha = whiteningFactor_.transpose().solveLinearSystem(posteriorMean_);
  const UnsignedInteger M = inducingPoints_.getSize();
  // result(j, k, 0) = sum_m alpha[m] * d^2 k(x, Z_m)/dx_j dx_k
  SymmetricTensor result(p, getOutputDimension());
  for (UnsignedInteger k = 0; k < p; ++k)
  {
    for (UnsignedInteger j = 0; j <= k; ++j)
    {
      Scalar value = 0.0;
      for (UnsignedInteger m = 0; m < M; ++m)
      {
        const SymmetricMatrix partialHessian(covarianceModel_.partialHessian(inP, inducingPoints_[m]));
        value += alpha[m] * partialHessian(j, k);
      }
      result(j, k, 0) = value;
    }
  }
  callsNumber_.fetchAndAdd(p * p);
  return result;
}


/* Accessor for input point dimension */
UnsignedInteger SparseGaussianProcessHessian::getInputDimension() const
{
  return covarianceModel_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger SparseGaussianProcessHessian::getOutputDimension() const
{
  return covarianceModel_.getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void SparseGaussianProcessHessian::save(Advocate & adv) const
{
  HessianImplementation::save(adv);
  adv.saveAttribute("covarianceModel_", covarianceModel_);
  adv.saveAttribute("inducingPoints_", inducingPoints_);
  adv.saveAttribute("whiteningFactor_", whiteningFactor_);
  UnsignedInteger linearAlgebraMethod = static_cast<UnsignedInteger>(linearAlgebraMethod_);
  adv.saveAttribute("linearAlgebraMethod_", linearAlgebraMethod);
  adv.saveAttribute("posteriorMean_", posteriorMean_);
}

/* Method load() reloads the object from the StorageManager */
void SparseGaussianProcessHessian::load(Advocate & adv)
{
  HessianImplementation::load(adv);
  adv.loadAttribute("covarianceModel_", covarianceModel_);
  adv.loadAttribute("inducingPoints_", inducingPoints_);
  adv.loadAttribute("whiteningFactor_", whiteningFactor_);
  UnsignedInteger linearAlgebraMethod = 0;
  adv.loadAttribute("linearAlgebraMethod_", linearAlgebraMethod);
  linearAlgebraMethod_ = static_cast<SparseGaussianProcessFitterResult::LinearAlgebra>(linearAlgebraMethod);
  if (linearAlgebraMethod_ != SparseGaussianProcessFitterResult::LAPACK)
  {
    LOGDEBUG("SparseGaussianProcessHessian::load: forcing LAPACK after reload (HMatrix is not serializable)");
    linearAlgebraMethod_ = SparseGaussianProcessFitterResult::LAPACK;
    whiteningFactor_ = covarianceModel_.discretize(inducingPoints_).computeRegularizedCholesky();
  }
  adv.loadAttribute("posteriorMean_", posteriorMean_);
}

END_NAMESPACE_OPENTURNS
