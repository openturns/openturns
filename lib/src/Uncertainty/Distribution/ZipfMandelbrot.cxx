//                                               -*- C++ -*-
/**
 *  @brief The ZipfMandelbrot distribution
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
#include "openturns/ZipfMandelbrot.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ZipfMandelbrot)

static const Factory<ZipfMandelbrot> Factory_ZipfMandelbrot;

/* Default constructor */
ZipfMandelbrot::ZipfMandelbrot()
  : DiscreteDistribution()
  , harmonicNumbers_(0)
  , n_(1)
  , q_(0.0)
  , s_(1.0)
{
  setName("ZipfMandelbrot");
  // We set the dimension of the ZipfMandelbrot distribution
  setDimension( 1 );
  computeRange();
  computeHarmonicNumbers();
}

/* Parameters constructor */
ZipfMandelbrot::ZipfMandelbrot(const UnsignedInteger n,
                               const Scalar q,
                               const Scalar s )
  : DiscreteDistribution()
  , harmonicNumbers_(ScalarCollection(0))
  , n_(n)
  , q_(q)
  , s_(s)
{
  setName("ZipfMandelbrot");
  // We set the dimension of the ZipfMandelbrot distribution
  setDimension( 1 );
  computeRange();
  setN(n);
  setQ(q);
  setS(s);
  computeHarmonicNumbers();
}

/* Comparison operator */
Bool ZipfMandelbrot::operator ==(const ZipfMandelbrot & other) const
{
  if (this == &other) return true;
  return (n_ == other.n_) && (q_ == other.q_) && (s_ == other.s_);
}

Bool ZipfMandelbrot::equals(const DistributionImplementation & other) const
{
  const ZipfMandelbrot* p_other = dynamic_cast<const ZipfMandelbrot*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String ZipfMandelbrot::__repr__() const
{
  OSS oss;
  oss << "class=" << ZipfMandelbrot::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " n=" << n_
      << " q=" << q_
      << " s=" << s_;
  return oss;
}

String ZipfMandelbrot::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(n = " << n_ << ", q = " << q_ << ", s = " << s_ << ")";
  return oss;
}

/* Virtual constructor */
ZipfMandelbrot * ZipfMandelbrot::clone() const
{
  return new ZipfMandelbrot(*this);
}

/* Get one realization of the distribution */
Point ZipfMandelbrot::getRealization() const
{
  const Scalar uniformRealization = 1.0 - RandomGenerator::Generate();

  ScalarCollection::const_iterator it(lower_bound(harmonicNumbers_.begin(),
                                      harmonicNumbers_.end(),
                                      uniformRealization * getHarmonicNumbers(n_))
                                     );
  return Point(1, it - harmonicNumbers_.begin() + 1);
}


/* Get the PDF of the distribution */
Scalar ZipfMandelbrot::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];

  if ((k < 1 - supportEpsilon_) || (std::abs(k - round(k)) > supportEpsilon_) || (k > n_ + supportEpsilon_)) return 0.0;
  return 1.0 / (std::pow(round(k) + q_, s_) * getHarmonicNumbers(n_) );
}


/* Get the CDF of the distribution */
Scalar ZipfMandelbrot::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  const Scalar k = point[0];

  if (k < 1 - supportEpsilon_) return 0.0;
  if (k > n_ - 1 + supportEpsilon_) return 1.0;

  Scalar value (getHarmonicNumbers(static_cast<UnsignedInteger>(round(k))) / getHarmonicNumbers(n_));
  return value;
}

/* Compute the entropy of the distribution */
Scalar ZipfMandelbrot::computeEntropy() const
{
  const Scalar hN = getHarmonicNumbers(n_);
  Scalar entropy = 0.0;
  for (UnsignedInteger i = 1; i <= n_; ++i)
    entropy += std::log(i + q_) / std::pow(i + q_, s_);
  entropy *= s_ / hN;
  entropy += std::log(hN);
  return entropy;
}

/* Compute the mean of the distribution */
void ZipfMandelbrot::computeMean() const
{
  Scalar value = 0.0;
  for (UnsignedInteger i = 1; i <= n_; ++i) value += i * std::pow(i + q_, -s_);
  mean_ = Point(1, value / getHarmonicNumbers(n_));
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point ZipfMandelbrot::getStandardDeviation() const
{
  return Point(1, std::sqrt(getCovariance()(0, 0)));
}

/* Get the skewness of the distribution */
Point ZipfMandelbrot::getSkewness() const
{
  Scalar mean = getMean()[0];
  Scalar std = getStandardDeviation()[0];
  Scalar value = 0.0;
  for (UnsignedInteger i = 1; i <= n_; ++i) value += std::pow((i - mean) / std, 3) * std::pow(i + q_, -s_);
  return Point(1, value / getHarmonicNumbers(n_));
}

/* Get the kurtosis of the distribution */
Point ZipfMandelbrot::getKurtosis() const
{
  Scalar mean = getMean()[0];
  Scalar std = getStandardDeviation()[0];
  Scalar value = 0.0;
  for (UnsignedInteger i = 1; i <= n_; ++i) value += std::pow((i - mean) / std, 4) * std::pow(i + q_, -s_);
  return Point(1, value / getHarmonicNumbers(n_));
}

/* Compute the covariance of the distribution */
void ZipfMandelbrot::computeCovariance() const
{
  Scalar mean = getMean()[0];
  Scalar value = 0.0;
  for (UnsignedInteger i = 1; i <= n_; ++i) value += std::pow(i - mean, 2) * std::pow(i + q_, -s_);
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = value / getHarmonicNumbers(n_);
  isAlreadyComputedCovariance_ = true;
}

/* Get the quantile of the distribution */
Scalar ZipfMandelbrot::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  return round(DiscreteDistribution::computeScalarQuantile(prob, tail));
}

/* Check if the distribution is elliptical */
Bool ZipfMandelbrot::isElliptical() const
{
  return n_ == 1;
}

/* Q accessor */
Scalar ZipfMandelbrot::getQ() const
{
  return q_;
}

void ZipfMandelbrot::setQ(const Scalar q)
{
  if (q < 0) throw InvalidArgumentException(HERE) << "q must be >= 0";
  if (q != q_)
  {
    q_ = q;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    isAlreadyCreatedGeneratingFunction_ = false;
  }
}

/* S accessor */
Scalar ZipfMandelbrot::getS() const
{
  return s_;
}

void ZipfMandelbrot::setS(const Scalar s)
{
  if (s <= 0) throw InvalidArgumentException(HERE) << "s must be > 0";
  if (s != s_)
  {
    s_ = s;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    isAlreadyCreatedGeneratingFunction_ = false;
  }
}

/* N accessor */
UnsignedInteger ZipfMandelbrot::getN() const
{
  return n_;
}

void ZipfMandelbrot::setN(const UnsignedInteger n)
{
  if (n == 0) throw InvalidArgumentException(HERE) << "N must be > 0";
  if (n != n_)
  {
    n_ = n;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    isAlreadyCreatedGeneratingFunction_ = false;
    computeRange();
  }
}


/* Compute the numerical range of the distribution given the parameters values */
void ZipfMandelbrot::computeRange()
{
  const Point lowerBound(1, 1.0);
  const Point upperBound(1, n_);
  const Interval::BoolCollection finiteLowerBound(1, true);
  const Interval::BoolCollection finiteUpperBound(1, true);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Get the support of a discrete distribution that intersect a given interval */
Sample ZipfMandelbrot::getSupport(const Interval & interval) const
{
  if (interval.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given interval has a dimension that does not match the distribution dimension.";
  const UnsignedInteger kMin = static_cast< UnsignedInteger > (std::max(ceil(interval.getLowerBound()[0]), 1.0));
  const UnsignedInteger kMax = static_cast< UnsignedInteger > (std::min(floor(interval.getUpperBound()[0]), Scalar(n_)));
  Sample result(0, 1);
  for (UnsignedInteger k = kMin; k <= kMax; ++k) result.add(Point(1, k));
  return result;
}

/* Parameters value accessor */
Point ZipfMandelbrot::getParameter() const
{
  Point point(3);
  point[0] = n_;
  point[1] = q_;
  point[2] = s_;
  return point;
}

void ZipfMandelbrot::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 3) throw InvalidArgumentException(HERE) << "Error: expected 3 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = ZipfMandelbrot(static_cast< UnsignedInteger >(round(parameter[0])), parameter[1], parameter[2]);
  setWeight(w);
}

/* Parametersdescription accessor */
Description ZipfMandelbrot::getParameterDescription() const
{
  Description description(3);
  description[0] = "n";
  description[1] = "q";
  description[2] = "s";
  return description;
}

/* Method save() stores the object through the StorageManager */
void ZipfMandelbrot::save(Advocate & adv) const
{
  DiscreteDistribution::save(adv);
  adv.saveAttribute( "n_", n_ );
  adv.saveAttribute( "q_", q_ );
  adv.saveAttribute( "s_", s_ );
}

/* Method load() reloads the object from the StorageManager */
void ZipfMandelbrot::load(Advocate & adv)
{
  DiscreteDistribution::load(adv);
  adv.loadAttribute( "n_", n_ );
  adv.loadAttribute( "q_", q_ );
  adv.loadAttribute( "s_", s_ );
  computeHarmonicNumbers();
  computeRange();
}


/* Method getHarmonicNumbers returns the k-th harmonic number for the current distribution */
Scalar ZipfMandelbrot::getHarmonicNumbers(const UnsignedInteger k ) const
{
  if (k < 1) throw InvalidArgumentException(HERE) << "k must be >= 1" ;
  if (k > n_) throw InvalidArgumentException(HERE) << "k must be <= N";

  return harmonicNumbers_[k - 1];
}

/* Method  computeHarmonicNumbers computes and stores the  of the k harmonic numbers for
   k = 1..n
   harmonicNumbers_[i] = \sum_{l=1}^i 1./( (i+q)**s )
*/
void ZipfMandelbrot::computeHarmonicNumbers()
{
  harmonicNumbers_ = ScalarCollection(n_);
  harmonicNumbers_[0] = std::pow(1.0 + q_, -s_);
  for (UnsignedInteger i = 2; i <= n_; ++i)
  {
    const Scalar hiqs = std::pow(i + q_, -s_);
    harmonicNumbers_[i - 1] = harmonicNumbers_[i - 2] + hiqs;
  }
}


END_NAMESPACE_OPENTURNS
