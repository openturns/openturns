//                                               -*- C++ -*-
/**
 *  @brief The Bernoulli distribution
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
#include "openturns/Bernoulli.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Bernoulli)

static const Factory<Bernoulli> Factory_Bernoulli;

/* Default constructor */
Bernoulli::Bernoulli()
  : DiscreteDistribution(),
    p_(0.5)
{
  setName( "Bernoulli" );
  // We set the dimension of the Bernoulli distribution
  setDimension( 1 );
  computeRange();
}

/* Parameters constructor */
Bernoulli::Bernoulli(const Scalar p)
  : DiscreteDistribution(),
    p_(-1.0)
{
  setName( "Bernoulli" );
  // This call sets also the range
  setP(p);
  // We set the dimension of the Bernoulli distribution
  setDimension( 1 );
}

/* Comparison operator */
Bool Bernoulli::operator ==(const Bernoulli & other) const
{
  if (this == &other) return true;
  return p_ == other.p_;
}

Bool Bernoulli::equals(const DistributionImplementation & other) const
{
  const Bernoulli* p_other = dynamic_cast<const Bernoulli*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Bernoulli::__repr__() const
{
  OSS oss;
  oss << "class=" << Bernoulli::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " p=" << p_;
  return oss;
}

String Bernoulli::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(p = " << p_ << ")";
  return oss;
}

/* Virtual constructor */
Bernoulli * Bernoulli::clone() const
{
  return new Bernoulli(*this);
}

/* Get one realization of the distribution */
Point Bernoulli::getRealization() const
{
  return Point(1, (RandomGenerator::Generate() < p_ ? 1.0 : 0.0));
}


/* Get the PDF of the distribution */
Scalar Bernoulli::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  if (std::abs(k) < supportEpsilon_) return 1.0 - p_;
  if (std::abs(k - 1.0) < supportEpsilon_) return p_;
  return 0.0;
}


/* Get the CDF of the distribution */
Scalar Bernoulli::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  // k < 0.0
  if (k < -supportEpsilon_) return 0.0;
  // k >= 1.0
  if (k > 1.0 - supportEpsilon_) return 1.0;
  // k > 0.0 && k < 1.0
  return 1.0 - p_;
}

/* Get the PDF gradient of the distribution */
Point Bernoulli::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  Point pdfGradient(1, 0.0);
  if ((k < -supportEpsilon_) || (std::abs(k - round(k)) > supportEpsilon_)) return pdfGradient;
  throw NotYetImplementedException(HERE) << "In Bernoulli::computePDFGradient(const Point & point) const";
}


/* Get the CDF gradient of the distribution */
Point Bernoulli::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  if (k < -supportEpsilon_) return Point(1, 0.0);
  throw NotYetImplementedException(HERE) << "In Bernoulli::computeCDFGradient(const Point & point) const";
}

/* Get the quantile of the distribution */
Scalar Bernoulli::computeScalarQuantile(const Scalar prob,
                                        const Bool tail) const
{
  if (prob < 1.0 - p_) return (tail ? 1.0 : 0.0);
  return (tail ? 0.0 : 1.0);
}

/* Compute the entropy of the distribution */
Scalar Bernoulli::computeEntropy() const
{
  if (p_ == 0.0 || p_ == 1.0) return 0.0;
  return -p_ * std::log(p_) - (1.0 - p_) * log1p(-p_);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex Bernoulli::computeCharacteristicFunction(const Scalar x) const
{
  const Complex value(1.0 - p_ + p_ * std::exp(Complex(0.0, x)));
  return value;
}

/* Get the generating function of the distribution, i.e. psi(z) = E(z^X) */
Complex Bernoulli::computeGeneratingFunction(const Complex & z) const
{
  const Complex value(1.0 - p_ + p_ * z);
  return value;
}

/* Compute the mean of the distribution */
void Bernoulli::computeMean() const
{
  mean_ = Point(1, p_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point Bernoulli::getStandardDeviation() const
{
  return Point(1, std::sqrt(p_ * (1.0 - p_)));
}

/* Get the skewness of the distribution */
Point Bernoulli::getSkewness() const
{
  if ((p_ == 0.0) || (p_ == 1.0)) throw NotDefinedException(HERE) << "Error: the skewness is not defined for the Bernoulli distribution when p is zero or one.";
  return Point(1, (1.0 - 2.0 * p_) / std::sqrt(p_ * (1.0 - p_)));
}

/* Get the kurtosis of the distribution */
Point Bernoulli::getKurtosis() const
{
  if ((p_ == 0.0) || (p_ == 1.0)) throw NotDefinedException(HERE) << "Error: the kurtosis is not defined for the Bernoulli distribution when p is zero or one.";
  return Point(1, 3.0 + (6.0 * p_ * (1.0 - p_) + 1.0) / (p_ * (1.0 - p_)));
}

/* Compute the covariance of the distribution */
void Bernoulli::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = p_ * (1.0 - p_);
  isAlreadyComputedCovariance_ = true;
}

/* Get the support of a discrete distribution that intersect a given interval */
Sample Bernoulli::getSupport(const Interval & interval) const
{
  if (interval.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given interval has a dimension that does not match the distribution dimension.";
  const UnsignedInteger kMin = static_cast< UnsignedInteger > (std::max(ceil(interval.getLowerBound()[0]), 0.0));
  const UnsignedInteger kMax = static_cast< UnsignedInteger > (std::min(floor(interval.getUpperBound()[0]), 1.0));
  Sample result(0, 1);
  for (UnsignedInteger k = kMin; k <= kMax; ++k) result.add(Point(1, k));
  return result;
}

/* Parameters value and description accessor */
Point Bernoulli::getParameter() const
{
  return Point(1, p_);
}

void Bernoulli::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 1) throw InvalidArgumentException(HERE) << "Error: expected 1 value, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = Bernoulli(parameter[0]);
  setWeight(w);
}

Description Bernoulli::getParameterDescription() const
{
  return Description(1, "p");
}

/* Check if the distribution is elliptical */
Bool Bernoulli::isElliptical() const
{
  return p_ == 0.5;
}

/* P accessor */
void Bernoulli::setP(const Scalar p)
{
  if ((p < 0.0) || (p > 1.0)) throw InvalidArgumentException(HERE) << "P must be in [0, 1], here p=" << p;
  if (p != p_)
  {
    p_ = p;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* P accessor */
Scalar Bernoulli::getP() const
{
  return p_;
}


/* Compute the numerical range of the distribution given the parameters values */
void Bernoulli::computeRange()
{
  // Degenerate cases
  if (p_ == 0.0) setRange(Interval(0.0, 0.0));
  if (p_ == 1.0) setRange(Interval(1.0, 1.0));
  // Usual case
  setRange(Interval(0.0, 1.0));
}

/* Method save() stores the object through the StorageManager */
void Bernoulli::save(Advocate & adv) const
{
  DiscreteDistribution::save(adv);
  adv.saveAttribute( "p_", p_ );
}

/* Method load() reloads the object from the StorageManager */
void Bernoulli::load(Advocate & adv)
{
  DiscreteDistribution::load(adv);
  adv.loadAttribute( "p_", p_ );
}

END_NAMESPACE_OPENTURNS
