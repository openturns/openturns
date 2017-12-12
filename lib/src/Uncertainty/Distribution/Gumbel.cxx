//                                               -*- C++ -*-
/**
 *  @brief The Gumbel distribution
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/Gumbel.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Gumbel)

static const Factory<Gumbel> Factory_Gumbel;

/* Default constructor */
Gumbel::Gumbel()
  : ContinuousDistribution()
  , alpha_(1.0)
  , beta_(0.0)
{
  setName( "Gumbel" );
  setDimension( 1 );
  computeRange();
}

/* Parameters constructor */
Gumbel::Gumbel(const Scalar alpha,
               const Scalar beta)
  : ContinuousDistribution()
  , alpha_(0.0)
  , beta_(beta)
{
  setName("Gumbel");
  setAlpha(alpha);
  setDimension( 1 );
}

/* Comparison operator */
Bool Gumbel::operator ==(const Gumbel & other) const
{
  if (this == &other) return true;
  return (alpha_ == other.alpha_) && (beta_ == other.beta_);
}

Bool Gumbel::equals(const DistributionImplementation & other) const
{
  const Gumbel* p_other = dynamic_cast<const Gumbel*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Gumbel::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Gumbel::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " alpha=" << alpha_
      << " beta=" << beta_;
  return oss;
}

String Gumbel::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(alpha = " << alpha_ << ", beta = " << beta_ << ")";
  return oss;
}

/* Virtual constructor */
Gumbel * Gumbel::clone() const
{
  return new Gumbel(*this);
}

/* Get one realization of the distribution */
Point Gumbel::getRealization() const
{
  return Point(1, beta_ - std::log(-std::log(RandomGenerator::Generate())) / alpha_);
}



/* Get the DDF of the distribution */
Point Gumbel::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar expX = std::exp(-alpha_ * (point[0] - beta_));
  return Point(1, alpha_ * alpha_ * (expX - 1.0) * expX * std::exp(-expX));
}


/* Get the PDF of the distribution */
Scalar Gumbel::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar expX = std::exp(-alpha_ * (point[0] - beta_));
  return alpha_ * expX * std::exp(-expX);
}

Scalar Gumbel::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar X = -alpha_ * (point[0] - beta_);
  return std::log(alpha_) + X - std::exp(X);
}


/* Get the CDF of the distribution */
Scalar Gumbel::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = -alpha_ * (point[0] - beta_);
  const Scalar expX = std::exp(x);
  return std::exp(-expX);
}

Scalar Gumbel::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = -alpha_ * (point[0] - beta_);
  const Scalar expX = std::exp(x);
  // -2.419227917539996841 = numerical bound for which the approximation has a relative error less than 1e-16
  if (x < -2.419227917539996841)
  {
    Scalar value = expX;
    Scalar coeff = expX;
    for (UnsignedInteger i = 2; i < 10; ++i)
    {
      coeff *= -expX / static_cast<Scalar>(i);
      value += coeff;
    }
    return value;
  }
  return 1.0 - std::exp(-expX);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex Gumbel::computeCharacteristicFunction(const Scalar x) const
{
  return SpecFunc::Gamma(Complex(1.0, -x / alpha_)) * std::exp(Complex(0.0, beta_ * x));
}

Complex Gumbel::computeLogCharacteristicFunction(const Scalar x) const
{
  return std::log(SpecFunc::Gamma(Complex(1.0, -x / alpha_))) + Complex(0.0, beta_ * x);
}

/* Get the PDFGradient of the distribution */
Point Gumbel::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - beta_;
  const Scalar expX = std::exp(-alpha_ * x);
  const Scalar pdf = alpha_ * expX * std::exp(-expX);
  Point pdfGradient(2);
  pdfGradient[0] = (1.0 / alpha_ - x * (1.0 - expX)) * pdf;
  pdfGradient[1] = alpha_ * (1.0 - expX) * pdf;
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
Point Gumbel::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - beta_;
  const Scalar expX = std::exp(-alpha_ * x);
  const Scalar cdf = std::exp(-expX);
  Point cdfGradient(2);
  cdfGradient[0] = x * expX * cdf;
  cdfGradient[1] = -alpha_ * expX * cdf;
  return cdfGradient;
}

/* Get the quantile of the distribution */
Scalar Gumbel::computeScalarQuantile(const Scalar prob,
                                     const Bool tail) const
{
  if (tail) return beta_ - std::log(-log1p(-prob)) / alpha_;
  return beta_ - std::log(-std::log(prob)) / alpha_;
}

/* Compute the mean of the distribution */
void Gumbel::computeMean() const
{
  mean_ = Point(1, beta_ + SpecFunc::EulerConstant / alpha_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point Gumbel::getStandardDeviation() const
{
  return Point(1, SpecFunc::PI_SQRT6 / alpha_);
}

/* Get the skewness of the distribution */
Point Gumbel::getSkewness() const
{
  // 1.139547099404648657492793 = 12 * sqrt(6) * Zeta(3) / Pi^3
  return Point(1, 1.139547099404648657492793);
}

/* Get the kurtosis of the distribution */
Point Gumbel::getKurtosis() const
{
  // 5.4 = 27/5
  return Point(1, 5.4);
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Gumbel::Implementation Gumbel::getStandardRepresentative() const
{
  return new Gumbel(1.0, 0.0);
}

/* Compute the covariance of the distribution */
void Gumbel::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = SpecFunc::PI2_6 / (alpha_ * alpha_);
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value accessor */
Point Gumbel::getParameter() const
{
  Point point(2);
  point[0] = alpha_;
  point[1] = beta_;
  return point;
}

void Gumbel::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 parameters, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = Gumbel(parameter[0], parameter[1]);
  setWeight(w);
}

/* Parameters description accessor */
Description Gumbel::getParameterDescription() const
{
  Description description(2);
  description[0] = "alpha";
  description[1] = "beta";
  return description;
}

/* Alpha accessor */
void Gumbel::setAlpha(const Scalar alpha)
{
  if (!(alpha > 0.0)) throw InvalidArgumentException(HERE) << "Alpha MUST be positive";
  if (alpha != alpha_)
  {
    alpha_ = alpha;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

Scalar Gumbel::getAlpha() const
{
  return alpha_;
}


/* M accessor */
void Gumbel::setBeta(const Scalar beta)
{
  if (beta != beta_)
  {
    beta_ = beta;
    isAlreadyComputedMean_ = false;
    // The covariance does not depend on beta
    computeRange();
  }
}

Scalar Gumbel::getBeta() const
{
  return beta_;
}

/* Method save() stores the object through the StorageManager */
void Gumbel::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "alpha_", alpha_ );
  adv.saveAttribute( "beta_", beta_ );
}

/* Method load() reloads the object from the StorageManager */
void Gumbel::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "alpha_", alpha_ );
  adv.loadAttribute( "beta_", beta_ );
  computeRange();
}



END_NAMESPACE_OPENTURNS
