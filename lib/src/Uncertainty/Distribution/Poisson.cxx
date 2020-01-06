//                                               -*- C++ -*-
/**
 *  @brief The Poisson distribution
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Poisson.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Poisson)

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
Poisson::Poisson(const Scalar lambda)
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

String Poisson::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(lambda = " << lambda_ << ")";
  return oss;
}

/* Virtual constructor */
Poisson * Poisson::clone() const
{
  return new Poisson(*this);
}

/* Get one realization of the distribution */
Point Poisson::getRealization() const
{
  return Point(1, DistFunc::rPoisson(lambda_));
}


/* Get the PDF of the distribution */
Scalar Poisson::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  if ((k < -supportEpsilon_) || (std::abs(k - round(k)) > supportEpsilon_)) return 0.0;
  return DistFunc::dPoisson(lambda_, static_cast<UnsignedInteger>(round(k)));
}


Scalar Poisson::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  if ((k < -supportEpsilon_) || (std::abs(k - round(k)) > supportEpsilon_)) return -SpecFunc::LogMaxScalar;
  return DistFunc::logdPoisson(lambda_, static_cast<UnsignedInteger>(round(k)));
}


/* Get the CDF of the distribution */
Scalar Poisson::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  if (k < -supportEpsilon_) return 0.0;
  return DistFunc::pGamma(floor(k) + 1.0, lambda_, true);
}

Scalar Poisson::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  if (k < -supportEpsilon_) return 1.0;
  return DistFunc::pGamma(floor(k) + 1.0, lambda_);
}

/* Get the PDF gradient of the distribution */
Point Poisson::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  Point pdfGradient(1, 0.0);
  if ((k < -supportEpsilon_) || (std::abs(k - round(k)) > supportEpsilon_)) return pdfGradient;
  return Point(1, (k - lambda_) * std::exp((k - 1.0) * std::log(lambda_) - lambda_ - SpecFunc::LnGamma(k + 1.0)));
}


/* Get the CDF gradient of the distribution */
Point Poisson::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  if (k < -supportEpsilon_) return Point(1, 0.0);
  return Point(1, -std::exp(floor(k) * std::log(lambda_) - lambda_ - SpecFunc::LnGamma(floor(k) + 1.0)));
}

/* Get the quantile of the distribution */
Scalar Poisson::computeScalarQuantile(const Scalar prob,
                                      const Bool tail) const
{
  return DistFunc::qPoisson(lambda_, prob, tail);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex Poisson::computeCharacteristicFunction(const Scalar x) const
{
  return std::exp(computeLogCharacteristicFunction(x));
}

Complex Poisson::computeLogCharacteristicFunction(const Scalar x) const
{
  return lambda_ * (std::exp(Complex(0.0, x)) - 1.0);
}

/* Get the generating function of the distribution, i.e. psi(z) = E(z^X) */
Complex Poisson::computeGeneratingFunction(const Complex & z) const
{
  return std::exp(computeLogGeneratingFunction(z));
}

Complex Poisson::computeLogGeneratingFunction(const Complex & z) const
{
  return lambda_ * (z - 1.0);
}

/* Compute the mean of the distribution */
void Poisson::computeMean() const
{
  mean_ = Point(1, lambda_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point Poisson::getStandardDeviation() const
{
  return Point(1, std::sqrt(lambda_));
}

/* Get the skewness of the distribution */
Point Poisson::getSkewness() const
{
  return Point(1, 1.0 / std::sqrt(lambda_));
}

/* Get the kurtosis of the distribution */
Point Poisson::getKurtosis() const
{
  return Point(1, 3.0 + 1.0 / lambda_);
}

/* Compute the covariance of the distribution */
void Poisson::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = lambda_;
  isAlreadyComputedCovariance_ = true;
}

/* Get the support of a discrete distribution that intersect a given interval */
Sample Poisson::getSupport(const Interval & interval) const
{
  if (interval.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given interval has a dimension that does not match the distribution dimension.";
  const UnsignedInteger kMin = static_cast< UnsignedInteger > (std::max(ceil(interval.getLowerBound()[0]), 0.0));
  const UnsignedInteger kMax = static_cast< UnsignedInteger > (floor(interval.getUpperBound()[0]));
  Sample result(0, 1);
  for (UnsignedInteger k = kMin; k <= kMax; ++k)
    result.add(Point(1, k));
  return result;
}

/* Parameters value accessor */
Point Poisson::getParameter() const
{
  return Point(1, lambda_);
}

void Poisson::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 1) throw InvalidArgumentException(HERE) << "Error: expected 1 value, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = Poisson(parameter[0]);
  setWeight(w);
}

/* Parameters description accessor */
Description Poisson::getParameterDescription() const
{
  return Description(1, "lambda");
}

/* Lambda accessor */
void Poisson::setLambda(const Scalar lambda)
{
  if (!(lambda > 0.0)) throw InvalidArgumentException(HERE) << "Lambda must be positive, here lambda=" << lambda;
  if (lambda != lambda_)
  {
    lambda_ = lambda;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* Lambda accessor */
Scalar Poisson::getLambda() const
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
