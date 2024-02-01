//                                               -*- C++ -*-
/**
 *  @brief The Geometric distribution
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Geometric.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Geometric)

static const Factory<Geometric> Factory_Geometric;

/* Default constructor */
Geometric::Geometric()
  : DiscreteDistribution()
  , p_(0.5)
{
  setName( "Geometric" );
  setDimension( 1 );
  computeRange();
}

/* Parameters constructor */
Geometric::Geometric(const Scalar p)
  : DiscreteDistribution()
  , p_(0.)
{
  setName( "Geometric" );
  // We set the dimension of the Geometric distribution
  setDimension( 1 );
  // Also sets the range
  setP(p);
}

/* Comparison operator */
Bool Geometric::operator ==(const Geometric & other) const
{
  if (this == &other) return true;
  return p_ == other.p_;
}

Bool Geometric::equals(const DistributionImplementation & other) const
{
  const Geometric* p_other = dynamic_cast<const Geometric*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Geometric::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Geometric::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " p=" << p_;
  return oss;
}

String Geometric::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(p = " << p_ << ")";
  return oss;
}

/* Virtual constructor */
Geometric * Geometric::clone() const
{
  return new Geometric(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void Geometric::computeRange()
{
  const Point lowerBound(1, 0.0);
  const Point upperBound(DistributionImplementation::computeUpperBound());
  const Interval::BoolCollection finiteLowerBound(1, true);
  const Interval::BoolCollection finiteUpperBound(1, false);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Get one realization of the distribution */
Point Geometric::getRealization() const
{
  return Point(1, ceil(std::log(RandomGenerator::Generate()) / log1p(-p_)));
}


/* Get the PDF of the distribution */
Scalar Geometric::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  if ((k < 1.0 - supportEpsilon_) || (std::abs(k - round(k)) > supportEpsilon_)) return 0.0;
  return p_ * std::pow(1.0 - p_, k - 1.0);
}


/* Get the CDF of the distribution */
Scalar Geometric::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  if (k < 1.0) return 0.0;
  return 1.0 - std::pow(1.0 - p_, floor(k));
}

Scalar Geometric::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  if (k < 1.0) return 1.0;
  return std::pow(1.0 - p_, floor(k));
}

/* Get the PDFGradient of the distribution */
Point Geometric::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  if ((k < 1.0 - supportEpsilon_) || (std::abs(k - round(k)) > supportEpsilon_)) return Point(1, 0.0);
  return Point(1, (1.0 - k * p_) * std::pow(1.0 - p_, k - 2.0));
}

/* Get the CDFGradient of the distribution */
Point Geometric::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = floor(point[0]);
  if ( k < 1.0 ) return Point(1, 0.0);
  return Point(1, k * std::pow(1 - p_, k - 1.0));
}

/* Get the quantile of the distribution */
Scalar Geometric::computeScalarQuantile(const Scalar prob,
                                        const Bool tail) const
{
  if (tail) return ceil(std::log(prob) / log1p(-p_));
  return ceil(log1p(-prob) / log1p(-p_));
}

/* Compute the entropy of the distribution */
Scalar Geometric::computeEntropy() const
{
  if (p_ == 0.0) return SpecFunc::MaxScalar;
  if (p_ == 1.0) return 0.0;
  return log1p(-p_) * (1.0 - 1.0 / p_) - std::log(p_);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex Geometric::computeCharacteristicFunction(const Scalar x) const
{
  const Complex value(p_ / (std::exp(Complex(0.0, -x)) - (1.0 - p_)));
  return value;
}

/* Get the generating function of the distribution, i.e. psi(z) = E(z^X) */
Complex Geometric::computeGeneratingFunction(const Complex & z) const
{
  return p_ * z / (1.0 - (1.0 - p_) * z);
}

/* Get the support of a discrete distribution that intersect a given interval */
Sample Geometric::getSupport(const Interval & interval) const
{
  if (interval.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given interval has a dimension that does not match the distribution dimension.";
  const UnsignedInteger kMin = static_cast< UnsignedInteger > (std::max(ceil(interval.getLowerBound()[0]), 0.0));
  const UnsignedInteger kMax = static_cast< UnsignedInteger > (std::min(getRange().getUpperBound()[0], floor(interval.getUpperBound()[0])));
  Sample result(0, 1);
  for (UnsignedInteger k = kMin; k <= kMax; ++k)
    result.add(Point(1, k));
  return result;
}

/* Compute the mean of the distribution */
void Geometric::computeMean() const
{
  mean_ = Point(1, 1 / p_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point Geometric::getStandardDeviation() const
{
  return Point(1, std::sqrt(1.0 - p_) / p_);
}

/* Get the skewness of the distribution */
Point Geometric::getSkewness() const
{
  return Point(1, (2.0 - p_) / std::sqrt(1.0 - p_));
}

/* Get the kurtosis of the distribution */
Point Geometric::getKurtosis() const
{
  return Point(1, 9.0 + p_ * p_ / (1.0 - p_));
}

/* Compute the covariance of the distribution */
void Geometric::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = (1.0 - p_) / (p_ * p_);
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value accessor */
Point Geometric::getParameter() const
{
  return Point(1, p_);
}

void Geometric::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 1) throw InvalidArgumentException(HERE) << "Error: expected 1 value, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = Geometric(parameter[0]);
  setWeight(w);
}

/* Parameters description accessor */
Description Geometric::getParameterDescription() const
{
  return Description(1, "p");
}

/* P accessor */
void Geometric::setP(const Scalar p)
{
  if ( (p <= 0.0) || (p > 1.0) ) throw InvalidArgumentException(HERE) << "Error: p must be in ]0, 1], here p=" << p;
  if (p != p_)
  {
    p_ = p;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* P accessor */
Scalar Geometric::getP() const
{
  return p_;
}

/* Method save() stores the object through the StorageManager */
void Geometric::save(Advocate & adv) const
{
  DiscreteDistribution::save(adv);
  adv.saveAttribute( "p_", p_ );
}

/* Method load() reloads the object from the StorageManager */
void Geometric::load(Advocate & adv)
{
  DiscreteDistribution::load(adv);
  adv.loadAttribute( "p_", p_ );
  computeRange();
}



END_NAMESPACE_OPENTURNS
