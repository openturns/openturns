//                                               -*- C++ -*-
/**
 *  @brief The FrankCopula distribution
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
#include <cmath>
#include "openturns/FrankCopula.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FrankCopula)

static const Factory<FrankCopula> Factory_FrankCopula;

/* Default constructor */
FrankCopula::FrankCopula()
  : ArchimedeanCopula()
  , theta_(2.0)
{
  setName( "FrankCopula" );
  // We set the dimension of the FrankCopula distribution
  setDimension( 2 );
  computeRange();
}

/* Parameters constructor */
FrankCopula::FrankCopula(const Scalar theta)
  : ArchimedeanCopula()
  , theta_(theta)
{
  setName( "FrankCopula" );
  // We set the dimension of the FrankCopula distribution
  setDimension( 2 );
  computeRange();
}

/* Comparison operator */
Bool FrankCopula::operator ==(const FrankCopula & other) const
{
  if (this == &other) return true;
  return theta_ == other.theta_;
}

Bool FrankCopula::equals(const DistributionImplementation & other) const
{
  const FrankCopula* p_other = dynamic_cast<const FrankCopula*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String FrankCopula::__repr__() const
{
  OSS oss(true);
  oss << "class=" << FrankCopula::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " theta=" << theta_;
  return oss;
}

String FrankCopula::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(theta = " << theta_ << ")";
  return oss;
}

/* Virtual constructor */
FrankCopula * FrankCopula::clone() const
{
  return new FrankCopula(*this);
}

/* Get one realization of the distribution */
Point FrankCopula::getRealization() const
{
  Point realization(2);
  // We use the general algorithm based on conditional CDF inversion
  const Scalar u = RandomGenerator::Generate();
  realization[0] = u;
  const Scalar v = RandomGenerator::Generate();
  if (theta_ == 0.0)
  {
    realization[1] = v;
    return realization;
  }
  const Scalar factor = (v - 1.0) * std::exp(-theta_ * u);
  realization[1] = 1.0 + std::log((factor - v) / (factor * std::exp(theta_) - v)) / theta_;
  return realization;
}

/* Get the DDF of the distribution */
Point FrankCopula::computeDDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Scalar u = point[0];
  const Scalar v = point[1];
  // A copula has a null DDF outside of ]0, 1[^2
  if ((u <= 0.0) || (u >= 1.0) || (v <= 0.0) || (v >= 1.0))
  {
    return Point(2, 0.0);
  }
  // Independent case
  if (theta_ == 0.0) return Point(2, 0.0);
  // Optimized version given by Maple 11, as there are a lot of exp's involved
  const Scalar theta2 = theta_ * theta_;
  const Scalar expMinusTheta = std::exp(-theta_);
  const Scalar expMinusThetaU = std::exp(-theta_ * u);
  const Scalar expMinusThetaV = std::exp(-theta_ * v);
  const Scalar product1 = expMinusThetaU * expMinusThetaV;
  const Scalar sum1 = expMinusTheta + product1 - expMinusThetaU - expMinusThetaV;
  const Scalar product2 = sum1 * sum1;
  const Scalar factor1 = -theta2 * expMinusThetaU * expMinusThetaV * (expMinusTheta - 1.0) / (sum1 * product2);
  Point result(2);
  result[0] = factor1 * (-expMinusTheta + product1 - expMinusThetaU + expMinusThetaV);
  result[1] = factor1 * (-expMinusTheta + product1 + expMinusThetaU - expMinusThetaV);
  return result;
}

/* Get the PDF of the distribution */
Scalar FrankCopula::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Scalar u = point[0];
  const Scalar v = point[1];
  // A copula has a null PDF outside of ]0, 1[^2
  if ((u <= 0.0) || (u >= 1.0) || (v <= 0.0) || (v >= 1.0))
  {
    return 0.0;
  }
  // Independent case
  if (theta_ == 0.0) return 1.0;
  // General case
  const Scalar expMinusTheta = std::exp(-theta_);
  const Scalar expMinusThetaU = std::exp(-theta_ * point[0]);
  const Scalar expMinusThetaV = std::exp(-theta_ * point[1]);
  const Scalar sum1 = expMinusTheta + expMinusThetaU * expMinusThetaV - expMinusThetaU - expMinusThetaV;
  return -theta_ * expMinusThetaU * expMinusThetaV * expm1(-theta_) / (sum1 * sum1);
}

/* Get the CDF of the distribution */
Scalar FrankCopula::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Scalar u = point[0];
  const Scalar v = point[1];
  // If we are outside of the support, in the lower parts
  if ((u <= 0.0) || (v <= 0.0))
  {
    return 0.0;
  }
  // If we are outside of the support, in the upper part
  if ((u >= 1.0) && (v >= 1.0))
  {
    return 1.0;
  }
  // If we are outside of the support for u, in the upper part
  if (u >= 1.0)
  {
    return v;
  }
  // If we are outside of the support for v, in the upper part
  if (v >= 1.0)
  {
    return u;
  }
  // If we are in the support
  // Independent case
  if (theta_ == 0.0) return u * v;
  // General case
  const Scalar expm1MinusTheta = expm1(-theta_);
  const Scalar expm1MinusThetaU = expm1(-theta_ * point[0]);
  const Scalar expm1MinusThetaV = expm1(-theta_ * point[1]);
  return -log1p(expm1MinusThetaU * expm1MinusThetaV / expm1MinusTheta) / theta_;
}

/* Compute the covariance of the distribution */
void FrankCopula::computeCovariance() const
{
  covariance_ = CovarianceMatrix(2);
  covariance_(0, 0) = 1.0 / 12.0;
  covariance_(0, 1) = (theta_ < 1.0e-3 ? theta_ / 72.0 * (1.0 - theta_ * theta_ / 75.0) : 1.0 / 12.0 + (SpecFunc::Debye(theta_, 2) - SpecFunc::Debye(theta_, 1)) / theta_);
  covariance_(1, 1) = 1.0 / 12.0;
  isAlreadyComputedCovariance_ = true;
}

/* Get the Spearman correlation of the distribution
We use the formula given here:
https://stats.stackexchange.com/questions/458372/what-is-the-spearmans-rho-for-frank-clayton-gumbel-and-fgm-copulas
*/
CorrelationMatrix FrankCopula::getSpearmanCorrelation() const
{
  CorrelationMatrix rho(2);
  const Scalar t = std::abs(theta_);
  const Scalar theta2 = theta_ * theta_;
  if (t < 1.0e-3)
  {
    rho(1, 0) = theta_ * (1.0 / 6.0 + theta2 * (-1.0 / 450.0 + theta2 / 23520.0));
    return rho;
  }
  rho(1, 0) = 1.0 - 12.0 / theta_ * (SpecFunc::Debye(theta_, 1) - SpecFunc::Debye(theta_, 2));
  return rho;
}

/* Get the Kendall concordance of the distribution */
CorrelationMatrix FrankCopula::getKendallTau() const
{
  CorrelationMatrix tau(2);
  tau(0, 1) = (std::abs(theta_) < 1.0e-3 ? theta_ / 9.0 * (1.0 - 0.01 * theta_ * theta_) : 1.0 + 4.0 * (SpecFunc::Debye(theta_, 1) - 1.0) / theta_);
  return tau;
}

/* Get the PDFGradient of the distribution */
Point FrankCopula::computePDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In FrankCopula::computePDFGradient(const Point & point) const";
}

/* Get the CDFGradient of the distribution */
Point FrankCopula::computeCDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In FrankCopula::computeCDFGradient(const Point & point) const";
}

/* Get the quantile of the distribution */
Point FrankCopula::computeQuantile(const Scalar prob,
                                   const Bool tail) const
{
  if (!((prob >= 0.0) && (prob <= 1.0))) throw InvalidArgumentException(HERE) << "Error: cannot compute a quantile for a probability level outside of [0, 1]";
  const Scalar q = tail ? 1.0 - prob : prob;
  // Special case for boarding values
  if (q == 0.0) return getRange().getLowerBound();
  if (q == 1.0) return getRange().getUpperBound();
  // Independent case
  if (theta_ == 0.0) return Point(2, std::sqrt(q));
  // General case
  const Scalar thetaProb = theta_ * q;
  const Scalar expTheta = std::exp(theta_);
  const Scalar expm1Theta = expm1(theta_);
  const Scalar sqrtRatio = std::sqrt(expm1(thetaProb) * expTheta / (expm1Theta * std::exp(thetaProb)));
  return Point(2, 1.0 - std::log(expTheta - sqrtRatio * expm1Theta) / theta_);
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar FrankCopula::computeConditionalCDF(const Scalar x, const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return x;
  const Scalar u = y[0];
  const Scalar v = x;
  // If we are in the support
  const Scalar alpha = std::exp(-theta_ * v);
  const Scalar beta = std::exp(-theta_ * u) * (alpha - 1.0);
  return -beta / (alpha - std::exp(-theta_) - beta);
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
Scalar FrankCopula::computeConditionalQuantile(const Scalar q, const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if ((q < 0.0) || (q > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
  if (q == 0.0) return 0.0;
  if (q == 1.0) return 1.0;
  // Initialize the conditional quantile with the quantile of the i-th marginal distribution
  // Special case when no contitioning or independent copula
  if ((conditioningDimension == 0) || hasIndependentCopula()) return q;
  const Scalar u = y[0];
  const Scalar factor = (q - 1.0) * std::exp(-theta_ * u);
  return 1.0 + std::log((factor - q) / (factor * std::exp(theta_) - q)) / theta_;
}

/* Compute the archimedean generator of the archimedean copula, i.e.
 * the function phi such that the CDF of the copula can
 * be written as CDF(t) = phi^{-1}(phi(u)+phi(v))
 */
Scalar FrankCopula::computeArchimedeanGenerator(const Scalar t) const
{
  // Independent case
  if (theta_ == 0.0) return -std::log(t);
  // General case
  return std::log(expm1(-theta_) / expm1(-theta_ * t));
}

/* Compute the inverse of the archimedean generator */
Scalar FrankCopula::computeInverseArchimedeanGenerator(const Scalar t) const
{
  // Independent case
  if (theta_ == 0.0) return std::exp(-t);
  // General case
  return 1.0 + (t - log1p(std::exp(theta_) * expm1(t))) / theta_;
}

/* Compute the derivative of the density generator */
Scalar FrankCopula::computeArchimedeanGeneratorDerivative(const Scalar t) const
{
  // Independent case
  if (theta_ == 0.0) return -1.0 / t;
  // General case
  return -theta_ / expm1(-theta_ * t);
}

/* Compute the seconde derivative of the density generator */
Scalar FrankCopula::computeArchimedeanGeneratorSecondDerivative(const Scalar t) const
{
  // Independent case
  if (theta_ == 0.0) return 1.0 / (t * t);
  // General case
  Scalar thetaT = theta_ * t;
  Scalar ratio = theta_ / expm1(thetaT);
  return ratio * ratio * std::exp(thetaT);
}

/* Dependence coefficients */
CorrelationMatrix FrankCopula::computeUpperTailDependenceMatrix() const
{
  return CorrelationMatrix(getDimension());
}

CorrelationMatrix FrankCopula::computeLowerTailDependenceMatrix() const
{
  return CorrelationMatrix(getDimension());
}

/* Parameters value accessor */
Point FrankCopula::getParameter() const
{
  return Point(1, theta_);
}

void FrankCopula::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 1) throw InvalidArgumentException(HERE) << "Error: expected 1 value, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = FrankCopula(parameter[0]);
  setWeight(w);
}

/* Parameters description accessor */
Description FrankCopula::getParameterDescription() const
{
  return Description(1, "theta");
}

/* Compute the entropy of the distribution */
Scalar FrankCopula::computeEntropy() const
{
  // To insure full relative precision in double precision
  const Scalar theta2 = theta_ * theta_;
  if (theta_ < 0.01636160042996688)
    return theta2 * (-1.0 / 72.0 + theta2 * (1.0 / 4800.0 - theta2 / 254016.0));
  const Scalar t1 = expm1(theta_);
  const Scalar t2 = std::log(t1);
  const Scalar t3 = std::exp(theta_);
  const Scalar t7 = std::log(theta_);
  const Scalar t11 = SpecFunc::DiLog(-t1);
  const Scalar t23 = SpecFunc::DiLog(1.0 / t3);
  return 2.0 - t7 + t11 / theta_ + (t3 * t2 + (t11 * t3 + SpecFunc::PI2_6 - 0.5 * theta2 - t23) / theta_) / t1;
}

/* Tell if the distribution has independent copula */
Bool FrankCopula::hasIndependentCopula() const
{
  return (theta_ == 0.0);
}

/* Theta accessor */
void FrankCopula::setTheta(const Scalar theta)
{
  if (theta != theta_)
  {
    theta_ = theta;
    isAlreadyComputedCovariance_ = false;
  }
}

/* Theta accessor */
Scalar FrankCopula::getTheta() const
{
  return theta_;
}

/* Method save() stores the object through the StorageManager */
void FrankCopula::save(Advocate & adv) const
{
  ArchimedeanCopula::save(adv);
  adv.saveAttribute( "theta_", theta_ );
}

/* Method load() reloads the object from the StorageManager */
void FrankCopula::load(Advocate & adv)
{
  ArchimedeanCopula::load(adv);
  adv.loadAttribute( "theta_", theta_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
