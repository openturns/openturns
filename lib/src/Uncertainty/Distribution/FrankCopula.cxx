//                                               -*- C++ -*-
/**
 *  @brief The FrankCopula distribution
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
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

CLASSNAMEINIT(FrankCopula);

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
FrankCopula::FrankCopula(const NumericalScalar theta)
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

String FrankCopula::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(theta = " << theta_ << ")";
  return oss;
}

/* Virtual constructor */
FrankCopula * FrankCopula::clone() const
{
  return new FrankCopula(*this);
}

/* Get one realization of the distribution */
NumericalPoint FrankCopula::getRealization() const
{
  NumericalPoint realization(2);
  // We use the general algorithm based on conditional CDF inversion
  const NumericalScalar u = RandomGenerator::Generate();
  realization[0] = u;
  const NumericalScalar v = RandomGenerator::Generate();
  if (theta_ == 0.0)
  {
    realization[1] = v;
    return realization;
  }
  const NumericalScalar factor = (v - 1.0) * std::exp(-theta_ * u);
  realization[1] = 1.0 + std::log((factor - v) / (factor * std::exp(theta_) - v)) / theta_;
  return realization;
}

/* Get the DDF of the distribution */
NumericalPoint FrankCopula::computeDDF(const NumericalPoint & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const NumericalScalar u = point[0];
  const NumericalScalar v = point[1];
  // A copula has a null DDF outside of ]0, 1[^2
  if ((u <= 0.0) || (u >= 1.0) || (v <= 0.0) || (v >= 1.0))
  {
    return NumericalPoint(2, 0.0);
  }
  // Independent case
  if (theta_ == 0.0) return NumericalPoint(2, 0.0);
  // Optimized version given by Maple 11, as there are a lot of exp's involved
  const NumericalScalar theta2 = theta_ * theta_;
  const NumericalScalar expMinusTheta = std::exp(-theta_);
  const NumericalScalar expMinusThetaU = std::exp(-theta_ * u);
  const NumericalScalar expMinusThetaV = std::exp(-theta_ * v);
  const NumericalScalar product1 = expMinusThetaU * expMinusThetaV;
  const NumericalScalar sum1 = expMinusTheta + product1 - expMinusThetaU - expMinusThetaV;
  const NumericalScalar product2 = sum1 * sum1;
  const NumericalScalar factor1 = -theta2 * expMinusThetaU * expMinusThetaV * (expMinusTheta - 1.0) / (sum1 * product2);
  NumericalPoint result(2);
  result[0] = factor1 * (-expMinusTheta + product1 - expMinusThetaU + expMinusThetaV);
  result[1] = factor1 * (-expMinusTheta + product1 + expMinusThetaU - expMinusThetaV);
  return result;
}

/* Get the PDF of the distribution */
NumericalScalar FrankCopula::computePDF(const NumericalPoint & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const NumericalScalar u = point[0];
  const NumericalScalar v = point[1];
  // A copula has a null PDF outside of ]0, 1[^2
  if ((u <= 0.0) || (u >= 1.0) || (v <= 0.0) || (v >= 1.0))
  {
    return 0.0;
  }
  // Independent case
  if (theta_ == 0.0) return 1.0;
  // General case
  const NumericalScalar expMinusTheta = std::exp(-theta_);
  const NumericalScalar expMinusThetaU = std::exp(-theta_ * point[0]);
  const NumericalScalar expMinusThetaV = std::exp(-theta_ * point[1]);
  const NumericalScalar sum1 = expMinusTheta + expMinusThetaU * expMinusThetaV - expMinusThetaU - expMinusThetaV;
  return -theta_ * expMinusThetaU * expMinusThetaV * expm1(-theta_) / (sum1 * sum1);
}

/* Get the CDF of the distribution */
NumericalScalar FrankCopula::computeCDF(const NumericalPoint & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const NumericalScalar u = point[0];
  const NumericalScalar v = point[1];
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
  const NumericalScalar expm1MinusTheta = expm1(-theta_);
  const NumericalScalar expm1MinusThetaU = expm1(-theta_ * point[0]);
  const NumericalScalar expm1MinusThetaV = expm1(-theta_ * point[1]);
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

/* Get the Kendall concordance of the distribution */
CorrelationMatrix FrankCopula::getKendallTau() const
{
  CorrelationMatrix tau(2);
  tau(0, 1) = (std::abs(theta_) < 1.0e-3 ? theta_ / 9.0 * (1.0 - 0.01 * theta_ * theta_) : 1.0 + 4.0 * (SpecFunc::Debye(theta_, 1) - 1.0) / theta_);
  return tau;
}

/* Get the PDFGradient of the distribution */
NumericalPoint FrankCopula::computePDFGradient(const NumericalPoint & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In FrankCopula::computePDFGradient(const NumericalPoint & point) const";
}

/* Get the CDFGradient of the distribution */
NumericalPoint FrankCopula::computeCDFGradient(const NumericalPoint & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In FrankCopula::computeCDFGradient(const NumericalPoint & point) const";
}

/* Get the quantile of the distribution */
NumericalPoint FrankCopula::computeQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  if ((prob < 0.0) || (prob > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a quantile for a probability level outside of [0, 1]";
  // Special case for boarding values
  if (prob == 0.0) return getRange().getLowerBound();
  if (prob == 1.0) return getRange().getUpperBound();
  // Independent case
  if (theta_ == 0.0) return NumericalPoint(2, std::sqrt(prob));
  // General case
  const NumericalScalar thetaProb = theta_ * prob;
  const NumericalScalar expTheta = std::exp(theta_);
  const NumericalScalar expm1Theta = expm1(theta_);
  const NumericalScalar sqrtRatio = std::sqrt(expm1(thetaProb) * expTheta / (expm1Theta * std::exp(thetaProb)));
  return NumericalPoint(2, 1.0 - std::log(expTheta - sqrtRatio * expm1Theta) / theta_);
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
NumericalScalar FrankCopula::computeConditionalCDF(const NumericalScalar x, const NumericalPoint & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return x;
  const NumericalScalar u = y[0];
  const NumericalScalar v = x;
  // If we are in the support
  const NumericalScalar alpha = std::exp(-theta_ * v);
  const NumericalScalar beta = std::exp(-theta_ * u) * (alpha - 1.0);
  return -beta / (alpha - std::exp(-theta_) - beta);
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
NumericalScalar FrankCopula::computeConditionalQuantile(const NumericalScalar q, const NumericalPoint & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if ((q < 0.0) || (q > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
  if (q == 0.0) return 0.0;
  if (q == 1.0) return 1.0;
  // Initialize the conditional quantile with the quantile of the i-th marginal distribution
  // Special case when no contitioning or independent copula
  if ((conditioningDimension == 0) || hasIndependentCopula()) return q;
  const NumericalScalar u = y[0];
  const NumericalScalar factor = (q - 1.0) * std::exp(-theta_ * u);
  return 1.0 + std::log((factor - q) / (factor * std::exp(theta_) - q)) / theta_;
}

/* Compute the archimedean generator of the archimedean copula, i.e.
 * the function phi such that the CDF of the copula can
 * be written as CDF(t) = phi^{-1}(phi(u)+phi(v))
 */
NumericalScalar FrankCopula::computeArchimedeanGenerator(const NumericalScalar t) const
{
  // Independent case
  if (theta_ == 0.0) return -std::log(t);
  // General case
  return std::log(expm1(-theta_) / expm1(-theta_ * t));
}

/* Compute the inverse of the archimedean generator */
NumericalScalar FrankCopula::computeInverseArchimedeanGenerator(const NumericalScalar t) const
{
  // Independent case
  if (theta_ == 0.0) return std::exp(-t);
  // General case
  return 1.0 + (t - log1p(std::exp(theta_) * expm1(t))) / theta_;
}

/* Compute the derivative of the density generator */
NumericalScalar FrankCopula::computeArchimedeanGeneratorDerivative(const NumericalScalar t) const
{
  // Independent case
  if (theta_ == 0.0) return -1.0 / t;
  // General case
  return -theta_ / expm1(-theta_ * t);
}

/* Compute the seconde derivative of the density generator */
NumericalScalar FrankCopula::computeArchimedeanGeneratorSecondDerivative(const NumericalScalar t) const
{
  // Independent case
  if (theta_ == 0.0) return 1.0 / (t * t);
  // General case
  NumericalScalar thetaT = theta_ * t;
  NumericalScalar ratio = theta_ / expm1(thetaT);
  return ratio * ratio * std::exp(thetaT);
}

/* Parameters value accessor */
NumericalPoint FrankCopula::getParameter() const
{
  return NumericalPoint(1, theta_);
}

void FrankCopula::setParameter(const NumericalPoint & parameter)
{
  if (parameter.getSize() != 1) throw InvalidArgumentException(HERE) << "Error: expected 1 value, got " << parameter.getSize();
  const NumericalScalar w = getWeight();
  *this = FrankCopula(parameter[0]);
  setWeight(w);
}

/* Parameters description accessor */
Description FrankCopula::getParameterDescription() const
{
  return Description(1, "theta");
}

/* Tell if the distribution has independent copula */
Bool FrankCopula::hasIndependentCopula() const
{
  return (theta_ == 0.0);
}

/* Theta accessor */
void FrankCopula::setTheta(const NumericalScalar theta)
{
  theta_ = theta;
}

/* Theta accessor */
NumericalScalar FrankCopula::getTheta() const
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
