//                                               -*- C++ -*-
/**
 *  @brief The class building sparse gaussian process evaluation
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

#include "openturns/SparseGaussianProcessEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SparseGaussianProcessEvaluation)

static const Factory<SparseGaussianProcessEvaluation> Factory_SparseGaussianProcessEvaluation;


/* Constructor with parameters */
SparseGaussianProcessEvaluation::SparseGaussianProcessEvaluation()
  : EvaluationImplementation()
  , covarianceModel_()
  , inducingPoints_()
  , whiteningFactor_()
  , posteriorMean_()
  , posteriorCovariance_()
{
  // Nothing to do
}


/* Constructor with parameters */
SparseGaussianProcessEvaluation::SparseGaussianProcessEvaluation(const CovarianceModel & covarianceModel,
    const Sample & inducingPoints,
    const TriangularMatrix & whiteningFactor,
    const Point & posteriorMean,
    const CovarianceMatrix & posteriorCovariance,
    const HMatrix & whiteningFactorHMatrix,
    const SparseGaussianProcessFitterResult::LinearAlgebra linearAlgebraMethod)
  : EvaluationImplementation()
  , covarianceModel_(covarianceModel)
  , inducingPoints_(inducingPoints)
  , whiteningFactor_(whiteningFactor)
  , whiteningFactorHMatrix_(whiteningFactorHMatrix)
  , linearAlgebraMethod_(linearAlgebraMethod)
  , posteriorMean_(posteriorMean)
  , posteriorCovariance_(posteriorCovariance)
{
  if (covarianceModel.getOutputDimension() != 1)
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessEvaluation::SparseGaussianProcessEvaluation, the covariance model must have output dimension 1, here output dimension = " << covarianceModel.getOutputDimension();
  if (covarianceModel.getInputDimension() != inducingPoints.getDimension())
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessEvaluation::SparseGaussianProcessEvaluation, the input dimension=" << covarianceModel.getInputDimension() << " of the covariance model should match the dimension=" << inducingPoints.getDimension() << " of the inducing points";
  const UnsignedInteger M = inducingPoints.getSize();
  if ((linearAlgebraMethod == SparseGaussianProcessFitterResult::LAPACK) && (whiteningFactor.getDimension() != M))
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessEvaluation::SparseGaussianProcessEvaluation, the whitening factor dimension=" << whiteningFactor.getDimension() << " should match the number of inducing points=" << M;
  if ((linearAlgebraMethod == SparseGaussianProcessFitterResult::HMAT) && (whiteningFactorHMatrix.getNbRows() != M))
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessEvaluation::SparseGaussianProcessEvaluation, the whitening factor HMatrix size=" << whiteningFactorHMatrix.getNbRows() << " should match the number of inducing points=" << M;
  if (posteriorMean.getDimension() != M)
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessEvaluation::SparseGaussianProcessEvaluation, the posterior mean dimension=" << posteriorMean.getDimension() << " should match the number of inducing points=" << M;
  if (posteriorCovariance.getDimension() != M)
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessEvaluation::SparseGaussianProcessEvaluation, the posterior covariance dimension=" << posteriorCovariance.getDimension() << " should match the number of inducing points=" << M;
  setInputDescription(Description::BuildDefault(getInputDimension(), "x"));
  setOutputDescription(Description::BuildDefault(getOutputDimension(), "y"));
}


/* Virtual constructor */
SparseGaussianProcessEvaluation * SparseGaussianProcessEvaluation::clone() const
{
  return new SparseGaussianProcessEvaluation(*this);
}
/* Comparison operator */
Bool SparseGaussianProcessEvaluation::operator ==(const SparseGaussianProcessEvaluation & other) const
{
  if (this == &other) return true;
  return (covarianceModel_ == other.covarianceModel_) && (inducingPoints_ == other.inducingPoints_) && (whiteningFactor_ == other.whiteningFactor_) && (posteriorMean_ == other.posteriorMean_) && (posteriorCovariance_ == other.posteriorCovariance_) && (linearAlgebraMethod_ == other.linearAlgebraMethod_);
}
Bool SparseGaussianProcessEvaluation::equals(const EvaluationImplementation & other) const
{
  const SparseGaussianProcessEvaluation * p_other = dynamic_cast<const SparseGaussianProcessEvaluation *>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String SparseGaussianProcessEvaluation::__repr__() const
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
String SparseGaussianProcessEvaluation::__str__(const String & ) const
{
  return OSS(false) << GetClassName();
}

/* Whitened cross-covariance accessor */
Point SparseGaussianProcessEvaluation::computeWhitenedCovariance(const Point & inP) const
{
  // k(Z, x)
  const Matrix kZx(covarianceModel_.computeCrossCovariance(inducingPoints_, inP));
  Point kZX(inducingPoints_.getSize());
  for (UnsignedInteger i = 0; i < kZx.getNbRows(); ++i)
    kZX[i] = kZx(i, 0);
  // a = Luu^{-1} k(Z, x)
  if (linearAlgebraMethod_ == SparseGaussianProcessFitterResult::HMAT)
    return whiteningFactorHMatrix_.solveLower(kZX);
  return whiteningFactor_.solveLinearSystem(kZX);
}

/* Operator () */
Point SparseGaussianProcessEvaluation::operator()(const Point & inP) const
{
  if (inP.getDimension() != getInputDimension())
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessEvaluation::operator(), input point should have the same dimension as the SparseGaussianProcessEvaluation input dimension. Here, inP dimension = " << inP.getSize()
                                         << " and SparseGaussianProcessEvaluation dimension = " << getInputDimension();
  const Point a(computeWhitenedCovariance(inP));
  Point value(getOutputDimension());
  value[0] = a.dot(posteriorMean_);
  callsNumber_.increment();
  return value;
}

Sample SparseGaussianProcessEvaluation::operator()(const Sample & inS) const
{
  const UnsignedInteger size = inS.getSize();
  Sample result(size, getOutputDimension());
  for (UnsignedInteger i = 0; i < size; ++i)
    result[i] = operator()(inS[i]);
  return result;
}

/* Conditional variance accessor */
Scalar SparseGaussianProcessEvaluation::getConditionalVariance(const Point & inP) const
{
  if (inP.getDimension() != getInputDimension())
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessEvaluation::getConditionalVariance, input point should have the same dimension as the SparseGaussianProcessEvaluation input dimension. Here, inP dimension = " << inP.getSize()
                                         << " and SparseGaussianProcessEvaluation dimension = " << getInputDimension();
  if (covarianceModel_.getOutputDimension() != 1)
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessEvaluation::getConditionalVariance, the covariance model must have output dimension 1, here output dimension = " << covarianceModel_.getOutputDimension();
  const Point a(computeWhitenedCovariance(inP));
  // k(x, x)
  const Scalar kxx = covarianceModel_(inP, inP)(0, 0);
  // a^T S_ww a
  const Point Swwa(posteriorCovariance_ * a);
  // v = k(x, x) - a^T a + a^T S_ww a
  return kxx - a.normSquare() + a.dot(Swwa);
}

Point SparseGaussianProcessEvaluation::getConditionalVariance(const Sample & inS) const
{
  const UnsignedInteger size = inS.getSize();
  Point result(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    result[i] = getConditionalVariance(inS[i]);
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger SparseGaussianProcessEvaluation::getInputDimension() const
{
  return covarianceModel_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger SparseGaussianProcessEvaluation::getOutputDimension() const
{
  return covarianceModel_.getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void SparseGaussianProcessEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute("covarianceModel_", covarianceModel_);
  adv.saveAttribute("inducingPoints_", inducingPoints_);
  adv.saveAttribute("whiteningFactor_", whiteningFactor_);
  UnsignedInteger linearAlgebraMethod = static_cast<UnsignedInteger>(linearAlgebraMethod_);
  adv.saveAttribute("linearAlgebraMethod_", linearAlgebraMethod);
  adv.saveAttribute("posteriorMean_", posteriorMean_);
  adv.saveAttribute("posteriorCovariance_", posteriorCovariance_);
}

/* Method load() reloads the object from the StorageManager */
void SparseGaussianProcessEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute("covarianceModel_", covarianceModel_);
  adv.loadAttribute("inducingPoints_", inducingPoints_);
  adv.loadAttribute("whiteningFactor_", whiteningFactor_);
  UnsignedInteger linearAlgebraMethod = 0;
  adv.loadAttribute("linearAlgebraMethod_", linearAlgebraMethod);
  linearAlgebraMethod_ = static_cast<SparseGaussianProcessFitterResult::LinearAlgebra>(linearAlgebraMethod);
  // HMatrix is not serializable; force LAPACK after reload
  if (linearAlgebraMethod_ != SparseGaussianProcessFitterResult::LAPACK)
  {
    LOGDEBUG("SparseGaussianProcessEvaluation::load: forcing LAPACK after reload (HMatrix is not serializable)");
    linearAlgebraMethod_ = SparseGaussianProcessFitterResult::LAPACK;
    whiteningFactor_ = covarianceModel_.discretize(inducingPoints_).computeRegularizedCholesky();
  }
  adv.loadAttribute("posteriorMean_", posteriorMean_);
  adv.loadAttribute("posteriorCovariance_", posteriorCovariance_);
}

END_NAMESPACE_OPENTURNS
