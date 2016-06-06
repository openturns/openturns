//                                               -*- C++ -*-
/**
 *  @brief The Burr distribution
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published bydistributiolib
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
#include "openturns/Burr.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Burr);

static const Factory<Burr> Factory_Burr;

/* Default constructor */
Burr::Burr()
  : ContinuousDistribution()
  , c_(1.0)
  , k_(1.0)
{
  setName( "Burr" );
  setDimension( 1 );
  computeRange();
}

/* Parameters constructor */
Burr::Burr(const NumericalScalar c,
           const NumericalScalar k)
  : ContinuousDistribution()
  , c_(c)
  , k_(0.)
{
  setName( "Burr" );
  // We set the dimension of the Burr distribution
  setDimension( 1 );
  setC(c);
  // calls computeRange
  setK(k);
}

/* Comparison operator */
Bool Burr::operator ==(const Burr & other) const
{
  if (this == &other) return true;
  return (c_ == other.c_) && (k_ == other.k_);
}

/* String converter */
String Burr::__repr__() const
{
  OSS oss;
  oss << "class=" << Burr::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " c=" << c_
      << " k=" << k_;
  return oss;
}

String Burr::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(c = " << c_ << ", k = " << k_ << ")";
  return oss;
}

/* Virtual constructor */
Burr * Burr::clone() const
{
  return new Burr(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void Burr::computeRange()
{
  const NumericalPoint lowerBound(1, 0.0);
  const NumericalPoint upperBound(computeUpperBound());
  const Interval::BoolCollection finiteLowerBound(1, true);
  const Interval::BoolCollection finiteUpperBound(1, false);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Get one realization of the distribution */
NumericalPoint Burr::getRealization() const
{
  return NumericalPoint(1, computeScalarQuantile(RandomGenerator::Generate()));
}


/* Get the DDF of the distribution */
NumericalPoint Burr::computeDDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if (x <= 0.0) return NumericalPoint(1, 0.0);
  const NumericalScalar xC(std::pow(x, c_));
  return NumericalPoint(1, -(xC * (c_ * k_ + 1.0) + 1.0 - c_) * computePDF(x) / (x * (1.0 + xC)));
}


/* Get the PDF of the distribution */
NumericalScalar Burr::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if (x <= 0.0) return 0.0;
  return std::exp(computeLogPDF(point));
}

NumericalScalar Burr::computeLogPDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if (x <= 0.0) return -SpecFunc::MaxNumericalScalar;
  const NumericalScalar logX(std::log(x));
  return std::log(c_ * k_) + (c_ - 1.0) * logX - (k_ + 1.0) * log1p(std::exp(c_ * logX));
}

/* Get the CDF of the distribution */
NumericalScalar Burr::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if (x <= 0.0) return 0.0;
  return -expm1(-k_ * log1p(std::exp(c_ * std::log(x))));
}

/* Get the PDFGradient of the distribution */
NumericalPoint Burr::computePDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  NumericalPoint pdfGradient(2, 0.0);
  if (x <= 0.0) return pdfGradient;
  const NumericalScalar pdf(computePDF(point));
  const NumericalScalar logX(std::log(x));
  pdfGradient[0] = ((1.0 - (1.0 + k_) / (1.0 + std::exp(-c_ * logX))) * logX + 1.0 / c_) * pdf;
  pdfGradient[1] = (1.0 / k_ - log1p(std::pow(x, c_))) * pdf;
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
NumericalPoint Burr::computeCDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  NumericalPoint cdfGradient(2, 0.0);
  if (x <= 0.0) return cdfGradient;
  const NumericalScalar cdfC(computeComplementaryCDF(point));
  const NumericalScalar xC(std::pow(x, c_));
  cdfGradient[0] = k_ * xC * std::log(x) * cdfC / (1.0 + xC);
  cdfGradient[1] = log1p(xC) * cdfC;
  return cdfGradient;
}

/* Get the quantile of the distribution */
NumericalScalar Burr::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  if (tail) return std::exp(std::log(expm1(-std::log(prob) / k_)) / c_);
  return std::exp(std::log(expm1(-log1p(-prob) / k_)) / c_);
}

/* Compute the mean of the distribution */
void Burr::computeMean() const
{
  mean_ = getStandardMoment(1);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
NumericalPoint Burr::getStandardDeviation() const
{
  return NumericalPoint(1, std::sqrt(getCovariance()(0, 0)));
}

/* Get the skewness of the distribution */
NumericalPoint Burr::getSkewness() const
{
  const NumericalScalar mu(getMean()[0]);
  const NumericalScalar sigma(getStandardDeviation()[0]);
  return NumericalPoint(1, (getStandardMoment(3)[0] - 3.0 * mu * sigma * sigma - mu * mu * mu) / (sigma * sigma * sigma));
}

/* Get the kurtosis of the distribution */
NumericalPoint Burr::getKurtosis() const
{
  const NumericalScalar mu(getMean()[0]);
  const NumericalScalar sigma(getStandardDeviation()[0]);
  return NumericalPoint(1, (getStandardMoment(4)[0] - 4.0 * mu * getStandardMoment(3)[0] + 6.0 * sigma * sigma * mu * mu + 3.0 * mu * mu * mu * mu) / (sigma * sigma * sigma * sigma));
}

/* Compute the covariance of the distribution */
void Burr::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = getStandardMoment(2)[0] - std::pow(getMean()[0], 2);
  isAlreadyComputedCovariance_ = true;
}

/* Get the moments of the standardized distribution */
NumericalPoint Burr::getStandardMoment(const UnsignedInteger n) const
{
  return NumericalPoint(1, std::exp(SpecFunc::LogGamma(k_ - n / c_) + SpecFunc::LogGamma(n / c_ + 1.0) - SpecFunc::LogGamma(k_)));
}

/* Parameters value accessor */
NumericalPoint Burr::getParameter() const
{
  NumericalPoint point(2);
  point[0] = c_;
  point[1] = k_;
  return point;
}

void Burr::setParameter(const NumericalPoint & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize(); 
  const NumericalScalar w = getWeight();
  *this = Burr(parameter[0], parameter[1]);
  setWeight(w);
}

/* Parameters description accessor */
Description Burr::getParameterDescription() const
{
  Description description(2);
  description[0] = "c";
  description[1] = "k";
  return description;
}

/* C accessor */
void Burr::setC(const NumericalScalar c)
{
  if (c <= 0.0) throw InvalidArgumentException(HERE) << "C MUST be positive";
  if (c_ != c)
  {
    c_ = c;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* C accessor */
NumericalScalar Burr::getC() const
{
  return c_;
}


/* K accessor */
void Burr::setK(const NumericalScalar k)
{
  if (k <= 0.0) throw InvalidArgumentException(HERE) << "K MUST be positive";
  if (k_ != k)
  {
    k_ = k;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* K accessor */
NumericalScalar Burr::getK() const
{
  return k_;
}

/* Method save() stores the object through the StorageManager */
void Burr::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "c_", c_ );
  adv.saveAttribute( "k_", k_ );
}

/* Method load() reloads the object from the StorageManager */
void Burr::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "c_", c_ );
  adv.loadAttribute( "k_", k_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
