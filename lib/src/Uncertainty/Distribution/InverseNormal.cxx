//                                               -*- C++ -*-
/**
 *  @brief The InverseNormal distribution
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/InverseNormal.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(InverseNormal);

static const Factory<InverseNormal> Factory_InverseNormal;

/* Default constructor */
InverseNormal::InverseNormal()
  : ContinuousDistribution()
  , lambda_(1.0)
  , mu_(1.0)
{
  setName( "InverseNormal" );
  setDimension( 1 );
  computeRange();
}

/* Parameters constructor */
InverseNormal::InverseNormal(const NumericalScalar lambda,
                             const NumericalScalar mu)
  : ContinuousDistribution()
  , lambda_(0.0)
  , mu_(0.0)
{
  setName( "InverseNormal" );
  // This call set also the range
  setLambdaMu(lambda, mu);
  setDimension( 1 );
}

/* Comparison operator */
Bool InverseNormal::operator ==(const InverseNormal & other) const
{
  if (this == &other) return true;
  return (lambda_ == other.lambda_) && (mu_ == other.mu_);
}

Bool InverseNormal::equals(const DistributionImplementation & other) const
{
  const InverseNormal* p_other = dynamic_cast<const InverseNormal*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String InverseNormal::__repr__() const
{
  OSS oss(true);
  oss << "class=" << InverseNormal::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " lambda=" << lambda_
      << " mu=" << mu_;
  return oss;
}

String InverseNormal::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(lambda = " << lambda_ << ", mu = " << mu_ << ")";
  return oss;
}

/* Virtual constructor */
InverseNormal * InverseNormal::clone() const
{
  return new InverseNormal(*this);
}

/* Get one realization of the distribution */
Point InverseNormal::getRealization() const
{
  const NumericalScalar nu = DistFunc::rNormal();
  const NumericalScalar y = nu * nu;
  const NumericalScalar w = mu_ * y / lambda_;
  // Lower bound computed by Maple to insure double precision
  const NumericalScalar x = w < 5.015e5 ? mu_ * (1.0 + 0.5 * w * (1.0 - std::sqrt(1.0 + 4.0 / w))) : mu_ * ((5.0 / w - 2.0) / w + 1.0) / w;
  const NumericalScalar z = RandomGenerator::Generate();
  if (z * (mu_ + x) <= mu_) return Point(1, x);
  return Point(1, mu_ * mu_ / x);
}


/* Get the PDF of the distribution */
NumericalScalar InverseNormal::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  if (x <= 0.0) return 0.0;
  return std::sqrt(lambda_ / (2.0 * M_PI * x * x * x)) * std::exp(- lambda_ * (x - mu_) * (x - mu_) / (2.0 * x * mu_ * mu_));
}

NumericalScalar InverseNormal::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  if (x <= 0.0) return SpecFunc::LogMinScalar;
  return 0.5 * ( std::log(lambda_) - std::log(2.0 * M_PI * x * x * x)) - lambda_ * (x - mu_) * (x - mu_) / (2.0 * x * mu_ * mu_);
}

/* Get the CDF of the distribution */
NumericalScalar InverseNormal::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  if (x <= 0.0) return 0.0;
  const NumericalScalar lx = std::sqrt(lambda_ / x);
  const NumericalScalar phiArg1 = lx * ( x / mu_ - 1.0);
  // Quick return if in the far right tail. The pNormal() function is constant
  // equal to 1 in double precision for argument greater than 8.24, and the
  // InverseNormal CDF is greater than pNormal()
  if (phiArg1 > 8.24) return 1.0;
  const NumericalScalar phiArg2 = -lx * ( x / mu_ + 1.0);
  return DistFunc::pNormal(phiArg1) + std::exp(2.0 * lambda_ / mu_ + std::log(DistFunc::pNormal(phiArg2)));
}

/** Get the minimum volume level set containing a given probability of the distribution */
LevelSet InverseNormal::computeMinimumVolumeLevelSetWithThreshold(const NumericalScalar prob, NumericalScalar & threshold) const
{
  const Interval interval(computeMinimumVolumeInterval(prob));
  Function minimumVolumeLevelSetFunction(MinimumVolumeLevelSetEvaluation(clone()).clone());
  minimumVolumeLevelSetFunction.setGradient(MinimumVolumeLevelSetGradient(clone()).clone());
  NumericalScalar minusLogPDFThreshold = -computeLogPDF(interval.getLowerBound()[0]);
  threshold = std::exp(-minusLogPDFThreshold);
  return LevelSet(minimumVolumeLevelSetFunction, minusLogPDFThreshold);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex InverseNormal::computeCharacteristicFunction(const NumericalScalar x) const
{
  if (std::abs(x) < pdfEpsilon_) return 1.0;
  return std::exp(computeLogCharacteristicFunction(x));
}

NumericalComplex InverseNormal::computeLogCharacteristicFunction(const NumericalScalar x) const
{
  if (std::abs(x) < pdfEpsilon_) return 0.0;
  const NumericalComplex logCF(lambda_ * (1.0 - std::sqrt(NumericalComplex(1.0, - 2.0 * mu_ * mu_ * x / lambda_))) / mu_);
  return logCF;
}

/* Compute the numerical range of the distribution given the parameters values */
void InverseNormal::computeRange()
{
  const Point lowerBound(1, 0.0);
  const NumericalScalar q = DistFunc::qNormal(cdfEpsilon_, true);
  const Point upperBound(1, 2.0 * lambda_ * mu_ / (2.0 * lambda_ + mu_ * q * q - q * std::sqrt(mu_ * (q * q * mu_ + 4.0 * lambda_))));
  const Interval::BoolCollection finiteLowerBound(1, true);
  const Interval::BoolCollection finiteUpperBound(1, false);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Compute the mean of the distribution */
void InverseNormal::computeMean() const
{
  mean_ = Point(1, mu_);
  isAlreadyComputedMean_ = true;
}

/* Compute the mean of the distribution */
void InverseNormal::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = mu_ * mu_ * mu_ / lambda_;
  isAlreadyComputedCovariance_ = true;
}

/* Get the standard deviation of the distribution */
Point InverseNormal::getStandardDeviation() const
{
  return Point(1, std::sqrt(getCovariance()(0, 0)));
}

/* Get the skewness of the distribution */
Point InverseNormal::getSkewness() const
{
  return Point(1, 3.0 * std::sqrt(mu_ / lambda_));
}

/* Get the kurtosis of the distribution */
Point InverseNormal::getKurtosis() const
{
  return Point(1, 3.0 + 15.0 * mu_ / lambda_);
}

/* Get the moments of the standardized distribution */
Point InverseNormal::getStandardMoment(const UnsignedInteger n) const
{
  if (n == 0) return Point(1, 1.0);
  NumericalScalar moment = 1.0;
  NumericalScalar rho = 0.5 * mu_ / lambda_;
  NumericalScalar product = 1.0;
  for (UnsignedInteger k = 1; k < n; ++k)
  {
    product *= (n - k) * (n + k - 1) * rho / k;
    moment += product;
  }
  return Point(1, std::pow(mu_, static_cast<int>(n)) * moment);
}

/* Interface specific to InverseNormal */

void InverseNormal::setLambdaMu(const NumericalScalar lambda,
                                const NumericalScalar mu)
{
  if ( (lambda <= 0.0) || (mu <= 0.0) ) throw InvalidArgumentException(HERE) << "lambda and mu MUST be positive";
  if ((lambda_ != lambda) || (mu_ != mu))
  {
    lambda_ = lambda;
    mu_ = mu;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* lambda accessor */
NumericalScalar InverseNormal::getLambda() const
{
  return lambda_;
}


/* mu accessor */
NumericalScalar InverseNormal::getMu() const
{
  return mu_;
}

/* Parameters value accessor */
Point InverseNormal::getParameter() const
{
  Point point(2);
  point[0] = lambda_;
  point[1] = mu_;
  return point;
}

void InverseNormal::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize();
  const NumericalScalar w = getWeight();
  *this = InverseNormal(parameter[0], parameter[1]);
  setWeight(w);
}

/* Parameters description accessor */
Description InverseNormal::getParameterDescription() const
{
  Description description(2);
  description[0] = "lambda";
  description[1] = "mu";
  return description;
}

/* Method save() stores the object through the StorageManager */
void InverseNormal::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "lambda_", lambda_ );
  adv.saveAttribute( "mu_", mu_ );
}

/* Method load() reloads the object from the StorageManager */
void InverseNormal::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "lambda_", lambda_ );
  adv.loadAttribute( "mu_", mu_ );
  computeRange();
}


END_NAMESPACE_OPENTURNS
