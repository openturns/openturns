//                                               -*- C++ -*-
/**
 *  @brief The Triangular distribution
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
#include "openturns/Triangular.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Triangular);

static const Factory<Triangular> RegisteredFactory;

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
Triangular::Triangular(const NumericalScalar a,
                       const NumericalScalar m,
                       const NumericalScalar b)
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

String Triangular::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(a = " << a_ << ", m = " << m_ << ", b = " << b_ << ")";
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
NumericalPoint Triangular::getRealization() const
{
  const NumericalScalar ma(m_ - a_);
  const NumericalScalar ba(b_ - a_);
  const NumericalScalar prob(RandomGenerator::Generate());
  if (ba * prob < ma) return NumericalPoint(1, a_ + std::sqrt(prob * ba * ma));
  return NumericalPoint(1, b_ - std::sqrt((1.0 - prob) * ba * (b_ - m_)));
}


/* Get the DDF of the distribution */
NumericalPoint Triangular::computeDDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if ((x <= a_) || (x > b_)) return NumericalPoint(1, 0.0);
  const NumericalScalar ddf(2.0 / (b_ - a_));
  if (x < m_) return NumericalPoint(1, ddf / (m_ - a_));
  return NumericalPoint(1, ddf / (m_ - b_));
}


/* Get the PDF of the distribution */
NumericalScalar Triangular::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if ((x <= a_) || (x > b_)) return 0.0;
  const NumericalScalar pdf(2.0 / (b_ - a_));
  if (x < m_) return pdf * (x - a_) / (m_ - a_);
  return pdf * (x - b_) / (m_ - b_);
}


/* Get the CDF of the distribution */
NumericalScalar Triangular::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if (x <= a_) return 0.0;
  if (x >= b_) return 1.0;
  const NumericalScalar cdf(1.0 / (b_ - a_));
  if (x < m_) return (x - a_) * (x - a_) * cdf / (m_ - a_);
  return 1.0 - (x - b_) * (x - b_) * cdf / (b_ - m_);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex Triangular::computeCharacteristicFunction(const NumericalScalar x) const
{
  if (std::abs(x) < 1.0e-8) return NumericalComplex(1.0, (a_ + b_ + m_) * x / 3.0);
  const NumericalScalar ba(b_ - a_);
  const NumericalScalar bm(b_ - m_);
  const NumericalScalar ma(m_ - a_);
  return 2.0 / (x * x) * (-std::exp(NumericalComplex(0.0, a_ * x)) / (ba * ma) + std::exp(NumericalComplex(0.0, m_ * x)) / (bm * ma) - std::exp(NumericalComplex(0.0, b_ * x)) / (ba * bm));
}

NumericalComplex Triangular::computeLogCharacteristicFunction(const NumericalScalar x) const
{
  if (std::abs(x) < pdfEpsilon_) return 0.0;
  const NumericalScalar ba(b_ - a_);
  const NumericalScalar bm(b_ - m_);
  const NumericalScalar ma(m_ - a_);
  return  M_LN2 - 2.0 * std::log(std::abs(x)) + std::log(-std::exp(NumericalComplex(0.0, a_ * x)) / (ba * ma) + std::exp(NumericalComplex(0.0, m_ * x)) / (bm * ma) - std::exp(NumericalComplex(0.0, b_ * x)) / (ba * bm));
}

/* Get the PDFGradient of the distribution */
NumericalPoint Triangular::computePDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if ((x <= a_) || (x > b_)) return NumericalPoint(1, 0.0);
  NumericalPoint pdfGradient(3);
  const NumericalScalar ba(b_ - a_);
  const NumericalScalar ma(m_ - a_);
  const NumericalScalar bm(b_ - m_);
  const NumericalScalar bx(b_ - x);
  const NumericalScalar xa(x - a_);
  const NumericalScalar fact(2.0 / ba);
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
NumericalPoint Triangular::computeCDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if ((x < a_) || (x > b_)) return NumericalPoint(1, 0.0);
  NumericalPoint cdfGradient(3);
  const NumericalScalar ba(b_ - a_);
  const NumericalScalar ma(m_ - a_);
  const NumericalScalar bm(b_ - m_);
  const NumericalScalar bx(b_ - x);
  const NumericalScalar xa(x - a_);
  const NumericalScalar xm(x - m_);
  if (x < m_)
  {
    const NumericalScalar fact(xa / (ba * ma));
    cdfGradient[0] = fact * (xm * ba - bx * ma) / (ma * ba);
    cdfGradient[1] = -fact * xa / ma;
    cdfGradient[2] = -xa / ba;
    return cdfGradient;
  }
  const NumericalScalar fact(bx / (ba * bm));
  cdfGradient[0] = -fact * bx / ba;
  cdfGradient[1] = -fact * bx / bm;
  cdfGradient[2] = -fact * (ba * xm + xa * bm) / (bm * ba);
  return cdfGradient;
}

/* Get the quantile of the distribution */
NumericalScalar Triangular::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  const NumericalScalar ma(m_ - a_);
  const NumericalScalar ba(b_ - a_);
  const NumericalScalar bm(b_ - m_);
  if (tail)
  {
    if (bm < prob * ba) return a_ + std::sqrt((1.0 - prob) * ba * ma);
    return b_ - std::sqrt(prob * ba * (b_ - m_));
  }
  if (ba * prob < ma) return a_ + std::sqrt(prob * ba * ma);
  return b_ - std::sqrt((1.0 - prob) * ba * bm);
}

/* Get the roughness, i.e. the L2-norm of the PDF */
NumericalScalar Triangular::getRoughness() const
{
  // 1.333333333333333333333333 = 4/3
  return 1.333333333333333333333333 / (b_ - a_);
}

/* Compute the mean of the distribution */
void Triangular::computeMean() const
{
  mean_ = NumericalPoint(1, (a_ + m_ + b_) / 3.0);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
NumericalPoint Triangular::getStandardDeviation() const
{
  const NumericalScalar ma(m_ - a_);
  const NumericalScalar bm(b_ - m_);
  return NumericalPoint(1, std::sqrt((bm * bm + bm * ma + ma * ma) / 18.0));
}

/* Get the skewness of the distribution */
NumericalPoint Triangular::getSkewness() const
{
  const NumericalScalar ma(m_ - a_);
  const NumericalScalar bm(b_ - m_);
  const NumericalScalar ba(b_ - a_);
  const NumericalScalar den(std::pow(bm * bm + bm * ma + ma * ma, 1.5));
  NumericalScalar num((ba + ma) * (bm - ma) * (bm + ba));
  // 0.2828427124746190097603378 = sqrt(2) / 5
  return NumericalPoint(1, 0.2828427124746190097603378 * num / den);
}

/* Get the kurtosis of the distribution */
NumericalPoint Triangular::getKurtosis() const
{
  // 2.4 = 12/5
  return NumericalPoint(1, 2.4);
}

/* Compute the covariance of the distribution */
void Triangular::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  const NumericalScalar ma(m_ - a_);
  const NumericalScalar bm(b_ - m_);
  covariance_(0, 0) = (bm * bm + bm * ma + ma * ma) / 18.0;
  isAlreadyComputedCovariance_ = true;
}

/* Get the moments of the standardized distribution */
NumericalPoint Triangular::getStandardMoment(const UnsignedInteger n) const
{
  const NumericalScalar mu(((m_ - a_) + (m_ - b_)) / (b_ - a_));
  // Even order
  if (n % 2 == 0)
  {
    // Vertical part?
    if (1.0 - std::abs(mu) < ResourceMap::GetAsNumericalScalar("DistributionImplementation-DefaultPDFEpsilon")) return NumericalPoint(1, 1.0 / (n + 1.0));
    // Usual case
    return NumericalPoint(1, 2.0 * (1.0 - std::pow(mu, n + 2.0)) / ((n + 1.0) * (n + 2.0) * (1.0 - mu) * (1.0 + mu)));
  }
  // Odd order
  // Vertical part?
  if (1.0 - std::abs(mu) < ResourceMap::GetAsNumericalScalar("DistributionImplementation-DefaultPDFEpsilon")) return NumericalPoint(1, 1.0 / (n + 2.0));
  return NumericalPoint(1, 2.0 * mu * (1.0 - std::pow(mu, n + 1.0)) / ((n + 1.0) * (n + 2.0) * (1.0 - mu) * (1.0 + mu)));
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Triangular::Implementation Triangular::getStandardRepresentative() const
{
  return Triangular(-1.0, ((m_ - a_) + (m_ - b_)) / (b_ - a_), 1.0).clone();
}

/* Parameters value accessor */
NumericalPoint Triangular::getParameter() const
{
  NumericalPoint point(3);
  point[0] = a_;
  point[1] = m_;
  point[2] = b_;
  return point;
}

void Triangular::setParameter(const NumericalPoint & parameter)
{
  if (parameter.getSize() != 3) throw InvalidArgumentException(HERE) << "Error: expected 3 values, got " << parameter.getSize(); 
  const NumericalScalar w = getWeight();
  *this = Triangular(parameter[0], parameter[1], parameter[2]);
  setWeight(w);
}


/* Parameters description accessor */
Description Triangular::getParameterDescription() const
{
  Description description(3);
  description[0] = "a";
  description[1] = "m";
  description[2] = "b";
  return description;
}

/* Interface specific to Triangular */

/* A accessor */
void Triangular::setAMB(const NumericalScalar a,
                        const NumericalScalar m,
                        const NumericalScalar b)
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

NumericalScalar Triangular::getA() const
{
  return a_;
}


/* M accessor */
NumericalScalar Triangular::getM() const
{
  return m_;
}


/* B accessor */
NumericalScalar Triangular::getB() const
{
  return b_;
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
