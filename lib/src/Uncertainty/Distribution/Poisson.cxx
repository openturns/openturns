//                                               -*- C++ -*-
/**
 *  @brief The Poisson distribution
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
#include "openturns/Poisson.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Poisson);

static const Factory<Poisson> Factory_Poisson;

/* Default constructor */
Poisson::Poisson()
  : DiscreteDistribution()
  , lambda_(1.0)
{
  setName( "Poisson" );
  // We set the dimension of the Poisson distribution
  setDimension( 1 );
  computeRange();
}

/* Parameters constructor */
Poisson::Poisson(const NumericalScalar lambda)
  : DiscreteDistribution()
  , lambda_(0.0)
{
  setName( "Poisson" );
  // We set the dimension of the Poisson distribution
  setDimension( 1 );
  // This call set also the range.
  setLambda(lambda);
}

/* Comparison operator */
Bool Poisson::operator ==(const Poisson & other) const
{
  if (this == &other) return true;
  return (lambda_ == other.lambda_);
}

Bool Poisson::equals(const DistributionImplementation & other) const
{
  const Poisson* p_other = dynamic_cast<const Poisson*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Poisson::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Poisson::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " lambda=" << lambda_;
  return oss;
}

String Poisson::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(lambda = " << lambda_ << ")";
  return oss;
}

/* Virtual constructor */
Poisson * Poisson::clone() const
{
  return new Poisson(*this);
}

/* Get one realization of the distribution */
NumericalPoint Poisson::getRealization() const
{
  return NumericalPoint(1, DistFunc::rPoisson(lambda_));
}


/* Get the PDF of the distribution */
NumericalScalar Poisson::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar k(point[0]);
  if ((k < -supportEpsilon_) || (std::abs(k - round(k)) > supportEpsilon_)) return 0.0;
  return std::exp(k * std::log(lambda_) - lambda_ - SpecFunc::LnGamma(k + 1.0));
}


/* Get the CDF of the distribution */
NumericalScalar Poisson::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar k(point[0]);
  if (k < -supportEpsilon_) return 0.0;
  return DistFunc::pGamma(floor(k) + 1.0, lambda_, true);
}

NumericalScalar Poisson::computeComplementaryCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar k(point[0]);
  if (k < -supportEpsilon_) return 1.0;
  return DistFunc::pGamma(floor(k) + 1.0, lambda_);
}

/* Get the PDF gradient of the distribution */
NumericalPoint Poisson::computePDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar k(point[0]);
  NumericalPoint pdfGradient(1, 0.0);
  if ((k < -supportEpsilon_) || (std::abs(k - round(k)) > supportEpsilon_)) return pdfGradient;
  return NumericalPoint(1, (k - lambda_) * std::exp((k - 1.0) * std::log(lambda_) - lambda_ - SpecFunc::LnGamma(k + 1.0)));
}


/* Get the CDF gradient of the distribution */
NumericalPoint Poisson::computeCDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar k(point[0]);
  if (k < -supportEpsilon_) return NumericalPoint(1, 0.0);
  return NumericalPoint(1, -std::exp(floor(k) * std::log(lambda_) - lambda_ - SpecFunc::LnGamma(floor(k) + 1.0)));
}

/* Get the quantile of the distribution */
NumericalScalar Poisson::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  return DistFunc::qPoisson(lambda_, prob, tail);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex Poisson::computeCharacteristicFunction(const NumericalScalar x) const
{
  return std::exp(computeLogCharacteristicFunction(x));
}

NumericalComplex Poisson::computeLogCharacteristicFunction(const NumericalScalar x) const
{
  return lambda_ * (std::exp(NumericalComplex(0.0, x)) - 1.0);
}

/* Get the generating function of the distribution, i.e. psi(z) = E(z^X) */
NumericalComplex Poisson::computeGeneratingFunction(const NumericalComplex & z) const
{
  return std::exp(computeLogGeneratingFunction(z));
}

NumericalComplex Poisson::computeLogGeneratingFunction(const NumericalComplex & z) const
{
  return lambda_ * (z - 1.0);
}

/* Compute the mean of the distribution */
void Poisson::computeMean() const
{
  mean_ = NumericalPoint(1, lambda_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
NumericalPoint Poisson::getStandardDeviation() const
{
  return NumericalPoint(1, std::sqrt(lambda_));
}

/* Get the skewness of the distribution */
NumericalPoint Poisson::getSkewness() const
{
  return NumericalPoint(1, 1.0 / std::sqrt(lambda_));
}

/* Get the kurtosis of the distribution */
NumericalPoint Poisson::getKurtosis() const
{
  return NumericalPoint(1, 3.0 + 1.0 / lambda_);
}

/* Compute the covariance of the distribution */
void Poisson::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = lambda_;
  isAlreadyComputedCovariance_ = true;
}

/* Get the support of a discrete distribution that intersect a given interval */
NumericalSample Poisson::getSupport(const Interval & interval) const
{
  if (interval.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given interval has a dimension that does not match the distribution dimension.";
  const UnsignedInteger kMin(static_cast< UnsignedInteger > (std::max(ceil(interval.getLowerBound()[0]), 0.0)));
  const UnsignedInteger kMax(static_cast< UnsignedInteger > (floor(interval.getUpperBound()[0])));
  NumericalSample result(0, 1);
  for (UnsignedInteger k = kMin; k <= kMax; ++k)
    result.add(NumericalPoint(1, k));
  return result;
}

/* Parameters value accessor */
NumericalPoint Poisson::getParameter() const
{
  return NumericalPoint(1, lambda_);
}

void Poisson::setParameter(const NumericalPoint & parameter)
{
  if (parameter.getSize() != 1) throw InvalidArgumentException(HERE) << "Error: expected 1 value, got " << parameter.getSize(); 
  const NumericalScalar w = getWeight();
  *this = Poisson(parameter[0]);
  setWeight(w);
}

/* Parameters description accessor */
Description Poisson::getParameterDescription() const
{
  return Description(1, "lambda");
}

/* Lambda accessor */
void Poisson::setLambda(const NumericalScalar lambda)
{
  if (lambda <= 0.0) throw InvalidArgumentException(HERE) << "Lambda must be positive, here lambda=" << lambda;
  if (lambda != lambda_)
  {
    lambda_ = lambda;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* Lambda accessor */
NumericalScalar Poisson::getLambda() const
{
  return lambda_;
}


/* Method save() stores the object through the StorageManager */
void Poisson::save(Advocate & adv) const
{
  DiscreteDistribution::save(adv);
  adv.saveAttribute( "lambda_", lambda_ );
}

/* Method load() reloads the object from the StorageManager */
void Poisson::load(Advocate & adv)
{
  DiscreteDistribution::load(adv);
  adv.loadAttribute( "lambda_", lambda_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
