//                                               -*- C++ -*-
/**
 *  @brief The Laplace distribution
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
#include "openturns/Laplace.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Laplace)

static const Factory<Laplace> Factory_Laplace;

/* Default constructor */
Laplace::Laplace()
  : ContinuousDistribution()
  , mu_(0.0)
  , lambda_(1.0)
{
  setName( "Laplace" );
  setDimension( 1 );
  computeRange();
}

/* Parameters constructor */
Laplace::Laplace(const Scalar mu,
                 const Scalar lambda)
  : ContinuousDistribution()
  , mu_(mu)
  , lambda_(lambda)
{
  setName( "Laplace" );
  // We set the dimension of the Laplace distribution
  setDimension( 1 );
  // To check that lambda > 0
  setLambda(lambda);
  // This call is needed as lambda_ == lambda and setLambda will not compute the range
  computeRange();
}

/* Comparison operator */
Bool Laplace::operator ==(const Laplace & other) const
{
  if (this == &other) return true;
  return (lambda_ == other.lambda_) && (mu_ == other.mu_);
}

Bool Laplace::equals(const DistributionImplementation & other) const
{
  const Laplace* p_other = dynamic_cast<const Laplace*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Laplace::__repr__() const
{
  OSS oss;
  oss << "class=" << Laplace::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " mu=" << mu_
      << " lambda=" << lambda_;
  return oss;
}

String Laplace::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(mu = " << mu_ << ", lambda = " << lambda_ << ")";
  return oss;
}

/* Virtual constructor */
Laplace * Laplace::clone() const
{
  return new Laplace(*this);
}

/* Get one realization of the distribution */
Point Laplace::getRealization() const
{
  const Scalar d = RandomGenerator::Generate() - 0.5;
  if (d < 0.0) return Point(1, mu_ + log1p(2.0 * d) / lambda_);
  return Point(1, mu_ - log1p(-2.0 * d) / lambda_);
}


/* Get the DDF of the distribution */
Point Laplace::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar value = computePDF(point) * lambda_;
  return (point[0] < mu_ ? Point(1, value) : Point(1, -value));
}


/* Get the PDF of the distribution */
Scalar Laplace::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return 0.5 * lambda_ * std::exp(-lambda_ * std::abs(point[0] - mu_));
}

Scalar Laplace::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return std::log(0.5 * lambda_) - lambda_ * std::abs(point[0] - mu_);
}

/* Get the CDF of the distribution */
Scalar Laplace::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar u = lambda_ * (point[0] - mu_);
  if (u < 0.0) return 0.5 * std::exp(u);
  return 1.0 - 0.5 * std::exp(-u);
}

/* Get the complementary CDF of the distribution */
Scalar Laplace::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar u = lambda_ * (point[0] - mu_);
  if (u < 0.0) return 1.0 - 0.5 * std::exp(u);
  return 0.5 * std::exp(-u);
}

/* Compute the entropy of the distribution */
Scalar Laplace::computeEntropy() const
{
  return 1.0 + M_LN2 - std::log(lambda_);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex Laplace::computeCharacteristicFunction(const Scalar x) const
{
  const Scalar t = x / lambda_;
  return std::exp(Complex(0.0, mu_ * x)) / (1.0 + t * t);
}

Complex Laplace::computeLogCharacteristicFunction(const Scalar x) const
{
  const Scalar t = x / lambda_;
  return Complex(0.0, mu_ * x) - log1p(t * t);
}

/* Get the PDFGradient of the distribution */
Point Laplace::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  Point pdfGradient(2, 0.0);
  const Scalar factor = std::abs(point[0] - mu_) * lambda_;
  const Scalar expFactor = std::exp(-factor);
  pdfGradient[0] = (point[0] > mu_ ? 0.5 * lambda_ * lambda_ * expFactor : -0.5 * lambda_ * lambda_ * expFactor);
  pdfGradient[1] = 0.5 * expFactor * (1.0 - factor);
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
Point Laplace::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  Point cdfGradient(2, 0.0);
  const Scalar factor = std::abs(point[0] - mu_) * lambda_;
  const Scalar expFactor = std::exp(-factor);
  cdfGradient[0] = -0.5 * lambda_ * expFactor;
  cdfGradient[1] = 0.5 * factor / lambda_ * expFactor;
  return cdfGradient;
}

/* Get the quantile of the distribution */
Scalar Laplace::computeScalarQuantile(const Scalar prob,
                                      const Bool tail) const
{
  const Scalar d = tail ? 0.5 - prob : prob - 0.5;
  if (d < 0.0) return mu_ + log1p(2.0 * d) / lambda_;
  return mu_ - log1p(-2.0 * d) / lambda_;
}

/* Compute the mean of the distribution */
void Laplace::computeMean() const
{
  mean_ = Point(1, mu_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point Laplace::getStandardDeviation() const
{
  return Point(1, std::sqrt(2.0) / lambda_);
}

/* Get the skewness of the distribution */
Point Laplace::getSkewness() const
{
  return Point(1, 0.0);
}

/* Get the kurtosis of the distribution */
Point Laplace::getKurtosis() const
{
  return Point(1, 6.0);
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution Laplace::getStandardRepresentative() const
{
  Laplace standard(0.0, 1.0);
  standard.setDescription(getDescription());
  return standard;
}

/* Compute the covariance of the distribution */
void Laplace::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = 2.0 / (lambda_ * lambda_);
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value and description accessor */
Point Laplace::getParameter() const
{
  Point point(2);
  point[0] = mu_;
  point[1] = lambda_;
  return point;
}

void Laplace::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = Laplace(parameter[0], parameter[1]);
  setWeight(w);
}

/* Parameters description accessor */
Description Laplace::getParameterDescription() const
{
  Description description(2);
  description[0] = "mu";
  description[1] = "lambda";
  return description;
}

/* Check if the distribution is elliptical */
Bool Laplace::isElliptical() const
{
  return true;
}

/* Mu accessor */
void Laplace::setMu(const Scalar mu)
{
  if (mu != mu_)
  {
    mu_ = mu;
    isAlreadyComputedMean_ = false;
    // The covariance does not depend on mu
    computeRange();
  }
}

/* Mu accessor */
Scalar Laplace::getMu() const
{
  return mu_;
}


/* Lambda accessor */
void Laplace::setLambda(const Scalar lambda)
{
  if (!(lambda > 0.0)) throw InvalidArgumentException(HERE) << "Lambda MUST be positive";
  if (lambda != lambda_)
  {
    lambda_ = lambda;
    isAlreadyComputedCovariance_ = false;
    // The mean does not depend on lambda
    computeRange();
  }
}

/* Lambda accessor */
Scalar Laplace::getLambda() const
{
  return lambda_;
}

/* Get the PDF singularities inside of the range - 1D only */
Point Laplace::getSingularities() const
{
  return Point(1, mu_);
}

/* Method save() stores the object through the StorageManager */
void Laplace::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "mu_", mu_ );
  adv.saveAttribute( "lambda_", lambda_ );
}

/* Method load() reloads the object from the StorageManager */
void Laplace::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "mu_", mu_ );
  adv.loadAttribute( "lambda_", lambda_ );
  computeRange();
}




END_NAMESPACE_OPENTURNS
