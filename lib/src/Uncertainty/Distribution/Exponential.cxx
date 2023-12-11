//                                               -*- C++ -*-
/**
 *  @brief The Exponential distribution
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
#include "openturns/Exponential.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Gamma.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Exponential)

static const Factory<Exponential> Factory_Exponential;

/* Default constructor */
Exponential::Exponential()
  : ContinuousDistribution()
  , lambda_(1.0)
  , gamma_(0.0)
{
  setName( "Exponential" );
  setDimension( 1 );
  computeRange();
}

/* Parameters constructor */
Exponential::Exponential(const Scalar lambda,
                         const Scalar gamma)
  : ContinuousDistribution()
  , lambda_(0.0)
  , gamma_(gamma)
{
  setName( "Exponential" );
  // We set the dimension of the Exponential distribution
  setDimension( 1 );
  // This call check lambda and set also the range.
  setLambda(lambda);
}

/* Comparison operator */
Bool Exponential::operator ==(const Exponential & other) const
{
  if (this == &other) return true;
  return (lambda_ == other.lambda_) && (gamma_ == other.gamma_);
}

Bool Exponential::equals(const DistributionImplementation & other) const
{
  const Exponential* p_other = dynamic_cast<const Exponential*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Exponential::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Exponential::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " lambda=" << lambda_
      << " gamma=" << gamma_;
  return oss;
}

String Exponential::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(lambda = " << lambda_ << ", gamma = " << gamma_ << ")";
  return oss;
}

/* Virtual constructor */
Exponential * Exponential::clone() const
{
  return new Exponential(*this);
}

/* Get one realization of the distribution */
Point Exponential::getRealization() const
{
  return Point(1, gamma_ - std::log(RandomGenerator::Generate()) / lambda_);
}


/* Get the DDF of the distribution */
Point Exponential::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  if (point[0] < gamma_) return Point(1, 0.0);
  return Point(1, -lambda_ * computePDF(point));
}


/* Get the PDF of the distribution */
Scalar Exponential::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return computePDF(point[0]);
}

Scalar Exponential::computePDF(const Scalar u) const
{
  const Scalar x = u - gamma_;
  if (x < 0.0) return 0.0;
  return lambda_ * std::exp(-lambda_ * x);
}

Scalar Exponential::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return computeLogPDF(point[0]);
}

Scalar Exponential::computeLogPDF(const Scalar u) const
{
  const Scalar x = u - gamma_;
  if (x < 0.0) return SpecFunc::LowestScalar;
  return std::log(lambda_) - lambda_ * x;
}

/* Get the CDF of the distribution */
Scalar Exponential::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return computeCDF(point[0]);
}

Scalar Exponential::computeCDF(const Scalar u) const
{
  const Scalar x = u - gamma_;
  if (x <= 0.0) return 0.0;
  return -expm1(-lambda_ * x);
}

/* Get the complementary CDF of the distribution */
Scalar Exponential::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return computeComplementaryCDF(point[0]);
}

/* Get the complementary CDF of the distribution */
Scalar Exponential::computeComplementaryCDF(const Scalar u) const
{
  const Scalar x = u - gamma_;
  if (x < 0.0) return 1.0;
  return std::exp(-lambda_ * x);
}

/* Compute the entropy of the distribution */
Scalar Exponential::computeEntropy() const
{
  return 1.0 - std::log(lambda_);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex Exponential::computeCharacteristicFunction(const Scalar x) const
{
  return std::exp(Complex(0.0, x * gamma_)) / Complex(1.0, -x / lambda_);
}

Complex Exponential::computeLogCharacteristicFunction(const Scalar x) const
{
  return Complex(0.0, x * gamma_) - std::log(Complex(1.0, - x / lambda_));
}

/* Get the PDFGradient of the distribution */
Point Exponential::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  Point pdfGradient(2, 0.0);
  if (x < 0.0) return pdfGradient;
  const Scalar expX = std::exp(-lambda_ * x);
  pdfGradient[0] = (1.0 - lambda_ * x) * expX;
  pdfGradient[1] = lambda_ * lambda_ * expX;
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
Point Exponential::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  Point cdfGradient(2, 0.0);
  if (x < 0.0) return cdfGradient;
  const Scalar expX = std::exp(-lambda_ * x);
  cdfGradient[0] = x * expX;
  cdfGradient[1] = -lambda_ * expX;
  return cdfGradient;
}

/* Get the quantile of the distribution */
Scalar Exponential::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  if (tail) return gamma_ - std::log(prob) / lambda_;
  return gamma_ - log1p(-prob) / lambda_;
}

Scalar Exponential::computeProbability(const Interval & interval) const
{
  if (interval.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "computeProbability expected an interval of dimension=" << dimension_ << ", got dimension=" << interval.getDimension();
  return computeProbabilityGeneral1D(interval.getLowerBound()[0], interval.getUpperBound()[0]);
}

/* Compute the mean of the distribution */
void Exponential::computeMean() const
{
  mean_ = Point(1, gamma_ + 1.0 / lambda_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point Exponential::getStandardDeviation() const
{
  return Point(1, 1.0 / lambda_);
}

/* Get the skewness of the distribution */
Point Exponential::getSkewness() const
{
  return Point(1, 2.0);
}

/* Get the kurtosis of the distribution */
Point Exponential::getKurtosis() const
{
  return Point(1, 9.0);
}

/* Compute the covariance of the distribution */
void Exponential::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = 1.0 / (lambda_ * lambda_);
  isAlreadyComputedCovariance_ = true;
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution Exponential::getStandardRepresentative() const
{
  Gamma standard(1.0, 1.0, 0.0);
  standard.setDescription(getDescription());
  return standard;
}

/* Parameters value accessor */
Point Exponential::getParameter() const
{
  Point point(2);
  point[0] = lambda_;
  point[1] = gamma_;
  return point;
}

void Exponential::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = Exponential(parameter[0], parameter[1]);
  setWeight(w);
}

/* Parameters description accessor */
Description Exponential::getParameterDescription() const
{
  Description description(2);
  description[0] = "lambda";
  description[1] = "gamma";
  return description;
}

/* Lambda accessor */
void Exponential::setLambda(const Scalar lambda)
{
  if (!(lambda > 0.0)) throw InvalidArgumentException(HERE) << "Lambda MUST be positive";
  if (lambda != lambda_)
  {
    lambda_ = lambda;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* Lambda accessor */
Scalar Exponential::getLambda() const
{
  return lambda_;
}


/* Gamma accessor */
void Exponential::setGamma(const Scalar gamma)
{
  if (gamma != gamma_)
  {
    gamma_ = gamma;
    isAlreadyComputedMean_ = false;
    // The covariance does not depend on gamma
    computeRange();
  }
}

/* Gamma accessor */
Scalar Exponential::getGamma() const
{
  return gamma_;
}

/* Compute the numerical range of the distribution given the parameters values */
void Exponential::computeRange()
{
  setRange(Interval(Point(1, gamma_), Point(1, computeScalarQuantile(cdfEpsilon_, true)), Interval::BoolCollection(1, true), Interval::BoolCollection(1, false)));
}

/* Method save() stores the object through the StorageManager */
void Exponential::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "lambda_", lambda_ );
  adv.saveAttribute( "gamma_", gamma_ );
}

/* Method load() reloads the object from the StorageManager */
void Exponential::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "lambda_", lambda_ );
  adv.loadAttribute( "gamma_", gamma_ );
  computeRange();
}




END_NAMESPACE_OPENTURNS
