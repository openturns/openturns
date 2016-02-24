//                                               -*- C++ -*-
/**
 *  @brief The Skellam distribution
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
#include "openturns/Skellam.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Skellam);

static const Factory<Skellam> RegisteredFactory;

/* Default constructor */
Skellam::Skellam()
  : DiscreteDistribution()
  , lambda1_(-1.0)
  , lambda2_(-1.0)
  , maximumIteration_(ResourceMap::GetAsUnsignedInteger("DistFunc-MaximumIteration"))
{
  setName("Skellam");
  // We set the dimension of the Skellam distribution
  setDimension( 1 );
  setLambda1Lambda2(1.0, 1.0);
}

/* Parameters constructor */
Skellam::Skellam(const NumericalScalar lambda1,
                 const NumericalScalar lambda2)
  : DiscreteDistribution()
  , lambda1_(-1.0)
  , lambda2_(-1.0)
  , maximumIteration_(ResourceMap::GetAsUnsignedInteger("DistFunc-MaximumIteration"))
{
  setName("Skellam");
  // We set the dimension of the Skellam distribution
  setDimension( 1 );
  // This call set also the range.
  setLambda1Lambda2(lambda1, lambda2);
}

/* Comparison operator */
Bool Skellam::operator ==(const Skellam & other) const
{
  if (this == &other) return true;
  return (lambda1_ == other.lambda1_) && (lambda2_ == other.lambda2_);
}

/* String converter */
String Skellam::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Skellam::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " lambda1=" << lambda1_
      << " lambda2=" << lambda2_;
  return oss;
}

String Skellam::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(lambda1 = " << lambda1_ << ", lambda2 = " << lambda2_ << ")";
  return oss;
}

/* Virtual constructor */
Skellam * Skellam::clone() const
{
  return new Skellam(*this);
}

/* Get one realization of the distribution */
NumericalPoint Skellam::getRealization() const
{
  // Must cast the second Poisson realization to avoid overflow by taking the opposite of an UnsignedInteger
  return NumericalPoint(1, DistFunc::rPoisson(lambda1_) - static_cast<NumericalScalar>(DistFunc::rPoisson(lambda2_)));
}


/* Get the PDF of the distribution */
NumericalScalar Skellam::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar k(point[0]);
  if (std::abs(k - round(k)) > supportEpsilon_) return 0.0;
  if (k < 0) return 2 * DistFunc::dNonCentralChiSquare(2.0 * (1.0 - k), 2.0 * lambda1_, 2.0 * lambda2_, pdfEpsilon_, maximumIteration_);
  return 2 * DistFunc::dNonCentralChiSquare(2.0 * (k + 1.0), 2.0 * lambda2_, 2.0 * lambda1_, pdfEpsilon_, maximumIteration_);
}


/* Get the CDF of the distribution */
NumericalScalar Skellam::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar k(point[0]);
  if (k < 0.0) return DistFunc::pNonCentralChiSquare(-2.0 * k, 2.0 * lambda1_, 2.0 * lambda2_, false, cdfEpsilon_, maximumIteration_);
  return DistFunc::pNonCentralChiSquare(2.0 * (k + 1.0), 2.0 * lambda2_, 2.0 * lambda1_, true, cdfEpsilon_, maximumIteration_);
}

/* Get the PDF gradient of the distribution */
NumericalPoint Skellam::computePDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  throw NotYetImplementedException(HERE) << "In Skellam::computePDFGradient(const NumericalPoint & point) const";
}


/* Get the CDF gradient of the distribution */
NumericalPoint Skellam::computeCDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  throw NotYetImplementedException(HERE) << "In Skellam::computeCDFGradient(const NumericalPoint & point) const";
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex Skellam::computeCharacteristicFunction(const NumericalScalar x) const
{
  return std::exp(computeLogCharacteristicFunction(x));
}

NumericalComplex Skellam::computeLogCharacteristicFunction(const NumericalScalar x) const
{
  return lambda1_ * std::exp(NumericalComplex(0.0, x)) + lambda2_ * std::exp(NumericalComplex(0.0, -x)) - (lambda1_ + lambda2_);
}

/* Get the generating function of the distribution, i.e. psi(z) = E(z^X) */
NumericalComplex Skellam::computeGeneratingFunction(const NumericalComplex & z) const
{
  return std::exp(computeLogGeneratingFunction(z));
}

NumericalComplex Skellam::computeLogGeneratingFunction(const NumericalComplex & z) const
{
  return lambda1_ * z + lambda2_ / z - (lambda1_ + lambda2_);
}

/* Get the quantile of the distribution */
NumericalScalar Skellam::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  return ceil(DistributionImplementation::computeScalarQuantile(prob, tail));
}

/* Compute the mean of the distribution */
void Skellam::computeMean() const
{
  mean_ = NumericalPoint(1, lambda1_ - lambda2_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
NumericalPoint Skellam::getStandardDeviation() const
{
  return NumericalPoint(1, std::sqrt(lambda1_ + lambda2_));
}

/* Get the skewness of the distribution */
NumericalPoint Skellam::getSkewness() const
{
  return NumericalPoint(1, (lambda1_ - lambda2_) * std::pow(lambda1_ + lambda2_, -1.5));
}

/* Get the kurtosis of the distribution */
NumericalPoint Skellam::getKurtosis() const
{
  return NumericalPoint(1, 3.0 + 1.0 / (lambda1_ + lambda2_));
}

/* Compute the covariance of the distribution */
void Skellam::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = lambda1_ + lambda2_;
  isAlreadyComputedCovariance_ = true;
}

/* Get the support of a discrete distribution that intersect a given interval */
NumericalSample Skellam::getSupport(const Interval & interval) const
{
  if (interval.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given interval has a dimension that does not match the distribution dimension.";
  const SignedInteger kMin(static_cast< SignedInteger > (ceil(interval.getLowerBound()[0])));
  const SignedInteger kMax(static_cast< SignedInteger > (floor(interval.getUpperBound()[0])));
  NumericalSample result(0, 1);
  for (SignedInteger k = kMin; k <= kMax; ++k)
    result.add(NumericalPoint(1, k));
  return result;
}

/* Parameters value accessor */
NumericalPoint Skellam::getParameter() const
{
  NumericalPoint point(2);
  point[0] = lambda1_;
  point[1] = lambda2_;
  return point;
}

void Skellam::setParameter(const NumericalPoint & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize(); 
  const NumericalScalar w = getWeight();
  *this = Skellam(parameter[0], parameter[1]);
  setWeight(w);
}

/* Parameters  description accessor */
Description Skellam::getParameterDescription() const
{
  Description description(2);
  description[0] = "lambda1";
  description[1] = "lambda2";
  return description;
}

/* Lambda1/Lambda2 accessor */
void Skellam::setLambda1Lambda2(const NumericalScalar lambda1,
                                const NumericalScalar lambda2)
{
  if (lambda1 <= 0.0) throw InvalidArgumentException(HERE) << "Lambda1 must be positive, here lambda1=" << lambda1;
  if (lambda2 <= 0.0) throw InvalidArgumentException(HERE) << "Lambda2 must be positive, here lambda2=" << lambda2;
  if ((lambda1 != lambda1_) || (lambda2 != lambda2_))
  {
    lambda1_ = lambda1;
    lambda2_ = lambda2;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* Lambda1 accessor */
void Skellam::setLambda1(const NumericalScalar lambda1)
{
  if (lambda1 <= 0.0) throw InvalidArgumentException(HERE) << "Lambda1 must be positive, here lambda1=" << lambda1;
  if (lambda1 != lambda1_)
  {
    lambda1_ = lambda1;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* Lambda1 accessor */
NumericalScalar Skellam::getLambda1() const
{
  return lambda1_;
}

/* Lambda2 accessor */
void Skellam::setLambda2(const NumericalScalar lambda2)
{
  if (lambda2 <= 0.0) throw InvalidArgumentException(HERE) << "Lambda2 must be positive, here lambda2=" << lambda2;
  if (lambda2 != lambda2_)
  {
    lambda2_ = lambda2;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* Lambda2 accessor */
NumericalScalar Skellam::getLambda2() const
{
  return lambda2_;
}

/* Maximum iterations accessor */
void Skellam::setMaximumIteration(const UnsignedInteger maximumIteration)
{
  maximumIteration_ = maximumIteration;
}

UnsignedInteger Skellam::getMaximumIteration() const
{
  return maximumIteration_;
}


/* Method save() stores the object through the StorageManager */
void Skellam::save(Advocate & adv) const
{
  DiscreteDistribution::save(adv);
  adv.saveAttribute( "lambda1_", lambda1_ );
  adv.saveAttribute( "lambda2_", lambda2_ );
  adv.saveAttribute( "maximumIteration_", maximumIteration_ );
}

/* Method load() reloads the object from the StorageManager */
void Skellam::load(Advocate & adv)
{
  DiscreteDistribution::load(adv);
  adv.loadAttribute( "lambda1_", lambda1_ );
  adv.loadAttribute( "lambda2_", lambda2_ );
  adv.loadAttribute( "maximumIteration_", maximumIteration_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
