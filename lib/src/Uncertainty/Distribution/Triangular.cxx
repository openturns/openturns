//                                               -*- C++ -*-
/**
 *  @brief The Triangular distribution
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
#include "openturns/Triangular.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Triangular)

static const Factory<Triangular> Factory_Triangular;

/* Default constructor */
Triangular::Triangular()
  : ContinuousDistribution()
  , a_(-1.0)
  , m_(0.0)
  , b_(1.0)
{
  setName( "Triangular" );
  setDimension( 1 );
  computeRange();
}

/* Parameters constructor */
Triangular::Triangular(const Scalar a,
                       const Scalar m,
                       const Scalar b)
  : ContinuousDistribution()
  , a_(0.0)
  , m_(0.0)
  , b_(0.0)
{
  setName( "Triangular" );
  // This call set also the range
  setAMB(a, m, b);
  setDimension( 1 );
}

/* Comparison operator */
Bool Triangular::operator ==(const Triangular & other) const
{
  if (this == &other) return true;
  return (a_ == other.a_) && (m_ == other.m_) && (b_ == other.b_);
}

Bool Triangular::equals(const DistributionImplementation & other) const
{
  const Triangular* p_other = dynamic_cast<const Triangular*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Triangular::__repr__() const
{
  OSS oss;
  oss << "class=" << Triangular::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " a=" << a_
      << " m=" << m_
      << " b=" << b_;
  return oss;
}

String Triangular::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(a = " << a_ << ", m = " << m_ << ", b = " << b_ << ")";
  return oss;
}

/* Virtual constructor */
Triangular * Triangular::clone() const
{
  return new Triangular(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void Triangular::computeRange()
{
  setRange(Interval(a_, b_));
}

/* Get one realization of the distribution */
Point Triangular::getRealization() const
{
  const Scalar ma = m_ - a_;
  const Scalar ba = b_ - a_;
  const Scalar prob = RandomGenerator::Generate();
  if (ba * prob < ma) return Point(1, a_ + std::sqrt(prob * ba * ma));
  return Point(1, b_ - std::sqrt((1.0 - prob) * ba * (b_ - m_)));
}


/* Get the DDF of the distribution */
Point Triangular::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  if ((x <= a_) || (x > b_)) return Point(1, 0.0);
  const Scalar ddf = 2.0 / (b_ - a_);
  if (x < m_) return Point(1, ddf / (m_ - a_));
  return Point(1, ddf / (m_ - b_));
}


/* Get the PDF of the distribution */
Scalar Triangular::computePDF(const Scalar x) const
{
  if ((x <= a_) || (x > b_)) return 0.0;
  const Scalar pdf = 2.0 / (b_ - a_);
  if (x < m_) return pdf * (x - a_) / (m_ - a_);
  return pdf * (x - b_) / (m_ - b_);
}

Scalar Triangular::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  return computePDF(point[0]);
}


/* Get the CDF of the distribution */
Scalar Triangular::computeCDF(const Scalar x) const
{
  if (x <= a_) return 0.0;
  if (x >= b_) return 1.0;
  const Scalar cdf = 1.0 / (b_ - a_);
  if (x < m_) return (x - a_) * (x - a_) * cdf / (m_ - a_);
  return 1.0 - (x - b_) * (x - b_) * cdf / (b_ - m_);
}

Scalar Triangular::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  return computeCDF(point[0]);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex Triangular::computeCharacteristicFunction(const Scalar x) const
{
  if (std::abs(x) < pdfEpsilon_) return 1.0;
  if (std::abs(x) < 1.0e-8) return Complex(1.0, (a_ + b_ + m_) * x / 3.0);
  const Scalar ba = b_ - a_;
  const Scalar bm = b_ - m_;
  const Scalar ma = m_ - a_;
  const Scalar epsilon = SpecFunc::Precision * ba;
  const Scalar twoOverX2 = 2.0 / (x * x);
  const Complex expIAX = std::exp(Complex(0.0, a_ * x));
  const Complex expIBX = std::exp(Complex(0.0, b_ * x));
  if (ma < epsilon) return twoOverX2 * (expIAX * Complex(1.0 / ba,  x) - expIBX / ba) / ba;
  if (bm < epsilon) return twoOverX2 * (expIBX * Complex(1.0 / ba, -x) - expIAX / ba) / ba;
  return twoOverX2 * (-expIAX / (ba * ma) + std::exp(Complex(0.0, m_ * x)) / (bm * ma) - expIBX / (ba * bm));
}

/* Get the PDFGradient of the distribution */
Point Triangular::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  if ((m_ == a_) || (m_ == b_)) throw NotDefinedException(HERE) << "Error: cannot compute the PDF gradient of a Triangular distribution when m=a or m=b, here m=" << m_ << ", a=" << a_ << " and b=" << b_;
  const Scalar x = point[0];
  if ((x <= a_) || (x > b_)) return Point(1, 0.0);
  Point pdfGradient(3);
  const Scalar ba = b_ - a_;
  const Scalar ma = m_ - a_;
  const Scalar bm = b_ - m_;
  const Scalar bx = b_ - x;
  const Scalar xa = x - a_;
  const Scalar fact = 2.0 / ba;
  if (x < m_)
  {
    pdfGradient[0] = fact * (-a_ * xa + x * ba - m_ * bx) / (ba * ma * ma);
    pdfGradient[1] = -fact * xa / (ma * ma);
    pdfGradient[2] = -fact * xa / (ba * ma);
    return pdfGradient;
  }
  pdfGradient[0] = fact * bx / (bm * ba);
  pdfGradient[1] = fact * bx / (bm * bm);
  pdfGradient[2] = fact * (-b_ * bx - m_ * xa + x * ba) / (ba * bm * bm);
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
Point Triangular::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  if ((m_ == a_) || (m_ == b_)) throw NotDefinedException(HERE) << "Error: cannot compute the CDF gradient of a Triangular distribution when m=a or m=b, here m=" << m_ << ", a=" << a_ << " and b=" << b_;
  const Scalar x = point[0];
  if ((x < a_) || (x > b_)) return Point(1, 0.0);
  Point cdfGradient(3);
  const Scalar ba = b_ - a_;
  const Scalar ma = m_ - a_;
  const Scalar bm = b_ - m_;
  const Scalar bx = b_ - x;
  const Scalar xa = x - a_;
  const Scalar xm = x - m_;
  if (x < m_)
  {
    const Scalar fact = xa / (ba * ma);
    cdfGradient[0] = fact * (xm * ba - bx * ma) / (ma * ba);
    cdfGradient[1] = -fact * xa / ma;
    cdfGradient[2] = -xa / ba;
    return cdfGradient;
  }
  const Scalar fact = bx / (ba * bm);
  cdfGradient[0] = -fact * bx / ba;
  cdfGradient[1] = -fact * bx / bm;
  cdfGradient[2] = -fact * (ba * xm + xa * bm) / (bm * ba);
  return cdfGradient;
}

/* Get the quantile of the distribution */
Scalar Triangular::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  const Scalar ma = m_ - a_;
  const Scalar ba = b_ - a_;
  const Scalar bm = b_ - m_;
  if (tail)
  {
    if (bm < prob * ba) return a_ + std::sqrt((1.0 - prob) * ba * ma);
    return b_ - std::sqrt(prob * ba * (b_ - m_));
  }
  if (ba * prob < ma) return a_ + std::sqrt(prob * ba * ma);
  return b_ - std::sqrt((1.0 - prob) * ba * bm);
}

/* Compute the entropy of the distribution */
Scalar Triangular::computeEntropy() const
{
  return 0.5 - std::log(2.0 / (b_ - a_));
}

/* Get the roughness, i.e. the L2-norm of the PDF */
Scalar Triangular::getRoughness() const
{
  // 1.333333333333333333333333 = 4/3
  return 1.333333333333333333333333 / (b_ - a_);
}

/* Compute the mean of the distribution */
void Triangular::computeMean() const
{
  mean_ = Point(1, (a_ + m_ + b_) / 3.0);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point Triangular::getStandardDeviation() const
{
  const Scalar ma = m_ - a_;
  const Scalar bm = b_ - m_;
  return Point(1, std::sqrt((bm * bm + bm * ma + ma * ma) / 18.0));
}

/* Get the skewness of the distribution */
Point Triangular::getSkewness() const
{
  const Scalar ma = m_ - a_;
  const Scalar bm = b_ - m_;
  const Scalar ba = b_ - a_;
  const Scalar den = std::pow(bm * bm + bm * ma + ma * ma, 1.5);
  Scalar num = (ba + ma) * (bm - ma) * (bm + ba);
  // 0.2828427124746190097603378 = sqrt(2) / 5
  return Point(1, 0.2828427124746190097603378 * num / den);
}

/* Get the kurtosis of the distribution */
Point Triangular::getKurtosis() const
{
  // 2.4 = 12/5
  return Point(1, 2.4);
}

/* Compute the covariance of the distribution */
void Triangular::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  const Scalar ma = m_ - a_;
  const Scalar bm = b_ - m_;
  covariance_(0, 0) = (bm * bm + bm * ma + ma * ma) / 18.0;
  isAlreadyComputedCovariance_ = true;
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution Triangular::getStandardRepresentative() const
{
  Triangular standard(-1.0, ((m_ - a_) + (m_ - b_)) / (b_ - a_), 1.0);
  standard.setDescription(getDescription());
  return standard;
}

/* Parameters value accessor */
Point Triangular::getParameter() const
{
  return {a_, m_, b_};
}

void Triangular::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 3) throw InvalidArgumentException(HERE) << "Error: expected 3 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = Triangular(parameter[0], parameter[1], parameter[2]);
  setWeight(w);
}


/* Parameters description accessor */
Description Triangular::getParameterDescription() const
{
  return {"a", "m", "b"};
}

/* Check if the distribution is elliptical */
Bool Triangular::isElliptical() const
{
  return std::abs(m_ - 0.5 * (a_ + b_)) < ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon");
}

/* Interface specific to Triangular */

/* A accessor */
void Triangular::setAMB(const Scalar a,
                        const Scalar m,
                        const Scalar b)
{
  if (a >= b) throw InvalidArgumentException(HERE) << "A MUST be less than b";
  if ((m < a) || (m > b)) throw InvalidArgumentException(HERE) << "M MUST be enclosed between a and b";
  if ((a != a_) || (m != m_) || (b != b_))
  {
    a_ = a;
    m_ = m;
    b_ = b;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

Scalar Triangular::getA() const
{
  return a_;
}


/* M accessor */
Scalar Triangular::getM() const
{
  return m_;
}


/* B accessor */
Scalar Triangular::getB() const
{
  return b_;
}


/* Get the PDF singularities inside of the range - 1D only */
Point Triangular::getSingularities() const
{
  if (m_ == a_) return Point(0);
  if (m_ == b_) return Point(0);
  return Point(1, m_);
}

/* Method save() stores the object through the StorageManager */
void Triangular::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "a_", a_ );
  adv.saveAttribute( "m_", m_ );
  adv.saveAttribute( "b_", b_ );
}

/* Method load() reloads the object from the StorageManager */
void Triangular::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "a_", a_ );
  adv.loadAttribute( "m_", m_ );
  adv.loadAttribute( "b_", b_ );
  computeRange();
}


END_NAMESPACE_OPENTURNS
