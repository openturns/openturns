//                                               -*- C++ -*-
/**
 *  @brief The Hypergeometric distribution
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
#include "openturns/Hypergeometric.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Hypergeometric)

static const Factory<Hypergeometric> Factory_Hypergeometric;

/* Default constructor */
Hypergeometric::Hypergeometric()
  : DiscreteDistribution()
  , n_(0)
  , k_(0)
  , m_(0)
{
  setName( "Hypergeometric" );
  // We set the dimension of the Hypergeometric distribution
  setDimension( 1 );
  computeProbabilities();
  computeRange();
}

/* Parameters constructor */
Hypergeometric::Hypergeometric(const UnsignedInteger n,
                               const UnsignedInteger k,
                               const UnsignedInteger m)
  : DiscreteDistribution()
  , n_(n)
  , k_(k)
  , m_(m)
{
  setName( "Hypergeometric" );
  // Don't use the accessors to check the parameters to avoid multiple computations
  // of the probabilities
  if (k > n_) throw InvalidArgumentException(HERE) << "Error: expected k=" << k << "to be less or equal to n=" << n_;
  if (m > n_) throw InvalidArgumentException(HERE) << "Error: expected m=" << m << "to be less or equal to n=" << n_;
  computeRange();
  computeProbabilities();
  // We set the dimension of the Hypergeometric distribution
  setDimension( 1 );
}

/* Comparison operator */
Bool Hypergeometric::operator ==(const Hypergeometric & other) const
{
  if (this == &other) return true;
  return (n_ == other.n_) && (k_ == other.k_) && (m_ == other.m_);
}

Bool Hypergeometric::equals(const DistributionImplementation & other) const
{
  const Hypergeometric* p_other = dynamic_cast<const Hypergeometric*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Hypergeometric::__repr__() const
{
  OSS oss;
  oss << "class=" << Hypergeometric::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " n=" << n_
      << " k=" << k_
      << " m=" << m_;
  return oss;
}

String Hypergeometric::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(n = " << n_ << ", k = " << k_ << ", m = " << m_ << ")";
  return oss;
}

/* Virtual constructor */
Hypergeometric * Hypergeometric::clone() const
{
  return new Hypergeometric(*this);
}

/* Get one realization of the distribution */
Point Hypergeometric::getRealization() const
{
  return Point(1, DistFunc::rDiscrete(probabilities_));
}


/* Get the PDF of the distribution */
Scalar Hypergeometric::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  // Check if the given point can be converted into an UnsignedInteger in a reasonable way
  if ((k < -supportEpsilon_) || (std::abs(k - round(k)) > supportEpsilon_)) return 0.0;
  return DistFunc::dHypergeometric(n_, k_, m_, static_cast<UnsignedInteger>(round(k)));
}


Scalar Hypergeometric::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  // Check if the given point can be converted into an UnsignedInteger in a reasonable way
  if ((k < -supportEpsilon_) || (std::abs(k - round(k)) > supportEpsilon_)) return SpecFunc::LowestScalar;
  return DistFunc::logdHypergeometric(n_, k_, m_, static_cast<UnsignedInteger>(round(k)));
}


/* Get the CDF of the distribution */
Scalar Hypergeometric::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  if (k < (k_ + m_ > n_ ? k_ + m_ - n_ : 0.0) - supportEpsilon_) return 0.0;
  if (k > std::min(k_, m_) + supportEpsilon_) return 1.0;
  return DistFunc::pHypergeometric(n_, k_, m_, static_cast<UnsignedInteger>(round(k)), false);
}

Scalar Hypergeometric::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  if (k < (k_ + m_ > n_ ? k_ + m_ - n_ : 0.0) - supportEpsilon_) return 1.0;
  if (k > std::min(k_, m_) + supportEpsilon_) return 0.0;
  return DistFunc::pHypergeometric(n_, k_, m_, static_cast<UnsignedInteger>(round(k)), true);
}

/* Get the PDF gradient of the distribution */
Point Hypergeometric::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  Point pdfGradient(1, 0.0);
  if ((k < (k_ + m_ > n_ ? k_ + m_ - n_ : 0.0) - supportEpsilon_) || (std::abs(k - round(k)) > supportEpsilon_) || (k > std::min(k_, m_) + supportEpsilon_)) return pdfGradient;
  throw NotYetImplementedException(HERE) << "In Hypergeometric::computePDFGradient(const Point & point) const";
}


/* Get the CDF gradient of the distribution */
Point Hypergeometric::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  Point cdfGradient(1, 0.0);
  if ((k < (k_ + m_ > n_ ? k_ + m_ - n_ : 0.0) - supportEpsilon_) || (std::abs(k - round(k)) > supportEpsilon_) || (k > std::min(k_, m_) + supportEpsilon_)) return cdfGradient;
  throw NotYetImplementedException(HERE) << "In Hypergeometric::computeCDFGradient(const Point & point) const";
}

/* Compute the mean of the distribution */
void Hypergeometric::computeMean() const
{
  mean_ = Point(1, m_ * k_ / (1.0 * n_));
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point Hypergeometric::getStandardDeviation() const
{
  if (n_ <= 1) throw NotDefinedException(HERE) << "Error: the standard deviation is not defined for the Hypergeometric distribution when n is less than or equal to 1.";
  return Point(1, std::sqrt(m_ * k_ / (1.0 * n_) * (n_ - k_) / (1.0 * n_) * (n_ - m_) / (n_ - 1.0)));
}

/* Get the skewness of the distribution */
Point Hypergeometric::getSkewness() const
{
  if (n_ <= 2) throw NotDefinedException(HERE) << "Error: the skewness is not defined for the Hypergeometric distribution when n is less than or equal to 2.";
  if (n_ == k_) throw NotDefinedException(HERE) << "Error: the skewness is not defined for the Hypergeometric distribution when n is equal to k.";
  if (n_ == m_) throw NotDefinedException(HERE) << "Error: the skewness is not defined for the Hypergeometric distribution when n is equal to m.";
  return Point(1, (n_ - 2.0 * k_) / (n_ - 2.0) * (n_ - 2.0 * m_) * std::sqrt((n_ - 1.0) / (m_ * k_ * (n_ - k_) * (n_ - m_))));
}

/* Get the kurtosis of the distribution */
Point Hypergeometric::getKurtosis() const
{
  if (n_ <= 3) throw NotDefinedException(HERE) << "Error: the kurtosis is not defined for the Hypergeometric distribution when n is less than or equal to 3.";
  if (n_ == k_) throw NotDefinedException(HERE) << "Error: the kurtosis is not defined for the Hypergeometric distribution when n is equal to k.";
  if (n_ == m_) throw NotDefinedException(HERE) << "Error: the kurtosis is not defined for the Hypergeometric distribution when n is equal to m.";
  return Point(1, 3.0 + (n_ * n_ * (n_ - 1.0) * (n_ * (n_ + 1.0) - 6.0 * k_ * (n_ - k_) - 6.0 * m_ * (n_ - m_)) + 6.0 * m_ * k_ * (n_ - k_) * (n_ - m_) * (5.0 * n_ - 6.0)) / (m_ * k_ * (n_ - k_) * (n_ - m_) * (n_ - 2.0) * (n_ - 3.0)));
}

/* Compute the covariance of the distribution */
void Hypergeometric::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = m_ * k_ / (1.0 * n_) * (n_ - k_) / (1.0 * n_) * (n_ - m_) / (n_ - 1.0);
  isAlreadyComputedCovariance_ = true;
}

/* Get the support of a discrete distribution that intersect a given interval */
Sample Hypergeometric::getSupport(const Interval & interval) const
{
  if (interval.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given interval has a dimension that does not match the distribution dimension.";
  const SignedInteger kMin = static_cast< SignedInteger > (std::max(static_cast<Scalar>(m_ + k_ > n_ ? m_ + k_ - n_ : 0), ceil(interval.getLowerBound()[0])));
  const SignedInteger kMax = static_cast< SignedInteger > (std::min(static_cast<Scalar>(std::min(k_, m_)), floor(interval.getUpperBound()[0])));
  Sample result(0, 1);
  for (SignedInteger k = kMin; k <= kMax; ++ k)
    result.add(Point(1, k));
  return result;
}

/* Get the discrete probability levels */
Point Hypergeometric::getProbabilities() const
{
  return probabilities_;
}

/* Parameters value accessor */
Point Hypergeometric::getParameter() const
{
  Point point(3);
  point[0] = n_;
  point[1] = k_;
  point[2] = m_;
  return point;
}

void Hypergeometric::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 3) throw InvalidArgumentException(HERE) << "Error: expected 3 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = Hypergeometric(static_cast<UnsignedInteger>(parameter[0]), static_cast<UnsignedInteger>(parameter[1]), static_cast<UnsignedInteger>(parameter[2]));
  setWeight(w);
}

/* Parameters description accessor */
Description Hypergeometric::getParameterDescription() const
{
  Description description(3);
  description[0] = "n";
  description[1] = "k";
  description[2] = "m";
  return description;
}

/* Check if the distribution is elliptical */
Bool Hypergeometric::isElliptical() const
{
  return false;
}

/* N accessor */
void Hypergeometric::setN(const UnsignedInteger n)
{
  if (n == 0) throw InvalidArgumentException(HERE) << "N must be a positive integer.";
  if (n != n_)
  {
    n_ = n;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
    computeProbabilities();
  }
}

/* N accessor */
UnsignedInteger Hypergeometric::getN() const
{
  return n_;
}

/* K accessor */
void Hypergeometric::setK(const UnsignedInteger k)
{
  if (k != k_)
  {
    if (k > n_) throw InvalidArgumentException(HERE) << "Error: expected k=" << k << "to be less or equal to n=" << n_;
    k_ = k;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
    computeProbabilities();
  }
}

/* K accessor */
UnsignedInteger Hypergeometric::getK() const
{
  return k_;
}

/* M accessor */
void Hypergeometric::setM(const UnsignedInteger m)
{
  if (m != m_)
  {
    if (m > n_) throw InvalidArgumentException(HERE) << "Error: expected m=" << m << "to be less or equal to n=" << n_;
    m_ = m;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
    computeProbabilities();
  }
}

/* K accessor */
UnsignedInteger Hypergeometric::getM() const
{
  return m_;
}

/* Compute the numerical range of the distribution given the parameters values */
void Hypergeometric::computeRange()
{
  if (k_ + m_ > n_) setRange(Interval(k_ + m_ - n_, std::min(k_, m_)));
  setRange(Interval(0.0, std::min(k_, m_)));
}

/* Compute the probability table of the distribution */
void Hypergeometric::computeProbabilities()
{
  // Generate the probability table
  probabilities_ = Point(n_ + 1);
  UnsignedInteger xMin = (k_ + m_ >= n_ ? k_ + m_ - n_ : 0);
  UnsignedInteger xMax = std::min(m_, k_);
  UnsignedInteger xMode = ((k_ + 1) * (m_ + 1)) / (n_ + 2);
  Scalar p = DistFunc::dHypergeometric(n_, k_, m_, xMode);
  probabilities_[xMode] = p;
  for (UnsignedInteger x = xMode + 1; x <= xMax; ++x)
  {
    p *= ((1.0 + k_ - x) / x) * ((1.0 + m_ - x) / (n_ + x - m_ - k_));
    probabilities_[x] = p;
  }
  p = probabilities_[xMode];
  for (UnsignedInteger x = xMode; x > xMin; --x)
  {
    p *= (x / (k_ - x + 1.0)) * ((n_ + x - m_ - k_) / (m_ - x + 1.0));
    probabilities_[x - 1] = p;
  }
}

/* Get the quantile of the distribution */
Scalar Hypergeometric::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  LOGDEBUG(OSS() << "in Hypergeometric::computeScalarQuantile, prob=" << prob << ", tail=" << tail);
  const Scalar a = getRange().getLowerBound()[0];
  const Scalar b = getRange().getUpperBound()[0];
  if (a == b)
    return a;
  if (prob <= 0.0) return (tail ? b : a);
  if (prob >= 1.0) return (tail ? a : b);
  Scalar quantile = (a + b) / 2;
  if (n_ > 3)
  {
    // Initialization by the Cornish-Fisher expansion
    Scalar qNorm = DistFunc::qNormal(prob, tail);
    Scalar gamma1 = getSkewness()[0];
    Scalar gamma2 = getKurtosis()[0] - 3.0;
    quantile = round(getMean()[0] + getStandardDeviation()[0] * (qNorm + (qNorm * qNorm - 1.0) * gamma1 / 6.0 + qNorm * (qNorm * qNorm - 3.0) * gamma2 / 24.0 - qNorm * (2.0 * qNorm * qNorm - 5.0) * gamma1 * gamma1 / 36.0));
  }
  if (quantile < a) quantile = a;
  if (quantile > b) quantile = b;
  // CDF of the guess
  Scalar cdf = tail ? computeComplementaryCDF(quantile) : computeCDF(quantile);
  LOGDEBUG(OSS() << "in Hypergeometric::computeScalarQuantile, Cornish-Fisher estimate=" << quantile << ", cdf=" << cdf);
  Scalar oldCDF = cdf;
  const Scalar step = tail ? -1.0 : 1.0;
  // Do we have to
  while (cdf >= prob)
  {
    quantile -= step;
    oldCDF = cdf;
    cdf = tail ? computeComplementaryCDF(quantile) : computeCDF(quantile);
    LOGDEBUG(OSS() << "in Hypergeometric::computeScalarQuantile, backward search, quantile=" << quantile << ", cdf=" << cdf);
  }
  // If
  if (cdf < oldCDF)
  {
    quantile += step;
    LOGDEBUG(OSS() << "in Hypergeometric::computeScalarQuantile, final quantile=" << quantile);
    return quantile;
  }
  while (cdf < prob)
  {
    quantile += step;
    cdf = tail ? computeComplementaryCDF(quantile) : computeCDF(quantile);
    LOGDEBUG(OSS() << "in Hypergeometric::computeScalarQuantile, forward search, quantile=" << quantile << ", cdf=" << cdf);
  }
  LOGDEBUG(OSS() << "in Hypergeometric::computeScalarQuantile, final quantile=" << quantile);
  return quantile;
}

/* Method save() stores the object through the StorageManager */
void Hypergeometric::save(Advocate & adv) const
{
  DiscreteDistribution::save(adv);
  adv.saveAttribute( "n_", n_ );
  adv.saveAttribute( "k_", k_ );
  adv.saveAttribute( "m_", m_ );
}

/* Method load() reloads the object from the StorageManager */
void Hypergeometric::load(Advocate & adv)
{
  DiscreteDistribution::load(adv);
  adv.loadAttribute( "n_", n_ );
  adv.loadAttribute( "k_", k_ );
  adv.loadAttribute( "m_", m_ );
  computeRange();
  computeProbabilities();
}

END_NAMESPACE_OPENTURNS
