//                                               -*- C++ -*-
/**
 *  @brief The Rayleigh distribution
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
#include "openturns/Rayleigh.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Rayleigh)

static const Factory<Rayleigh> Factory_Rayleigh;

/* Default constructor */
Rayleigh::Rayleigh()
  : ContinuousDistribution()
  , beta_(1.0)
  , gamma_(0.0)
{
  setName("Rayleigh");
  setDimension(1);
  computeRange();
}

/* Parameters constructor */
Rayleigh::Rayleigh(const Scalar beta,
                   const Scalar gamma)
  : ContinuousDistribution()
  , beta_(0.0)
  , gamma_(gamma)
{
  setName("Rayleigh");
  // This call set also the range
  setBeta(beta);
  setDimension(1);
}

/* Comparison operator */
Bool Rayleigh::operator ==(const Rayleigh & other) const
{
  if (this == &other) return true;
  return (beta_ == other.beta_) && (gamma_ == other.gamma_);
}

Bool Rayleigh::equals(const DistributionImplementation & other) const
{
  const Rayleigh* p_other = dynamic_cast<const Rayleigh*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Rayleigh::__repr__() const
{
  OSS oss;
  oss << "class=" << Rayleigh::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " beta=" << beta_
      << " gamma=" << gamma_;
  return oss;
}

String Rayleigh::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(beta = " << beta_ << ", gamma = " << gamma_ << ")";
  return oss;
}

/* Virtual constructor */
Rayleigh * Rayleigh::clone() const
{
  return new Rayleigh(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void Rayleigh::computeRange()
{
  const Point lowerBound(1, gamma_);
  const Point upperBound(computeUpperBound());
  const Interval::BoolCollection finiteLowerBound(1, true);
  const Interval::BoolCollection finiteUpperBound(1, false);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}


/* Get one realization of the distribution */
Point Rayleigh::getRealization() const
{
  return Point(1, gamma_ + beta_ * std::sqrt(-2.0 * std::log(RandomGenerator::Generate())));
}


/* Get the DDF of the distribution */
Point Rayleigh::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  if (x <= 0.0) return Point(1, 0.0);
  const Scalar y = x / beta_;
  const Scalar beta2 = beta_ * beta_;
  return Point(1, -std::exp(-0.5 * y * y) * (x - beta_) * (x + beta_) / (beta2 * beta2));
}


/* Get the PDF of the distribution */
Scalar Rayleigh::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  if (x <= 0.0) return 0.0;
  const Scalar y = x / (beta_ * beta_);
  return y * std::exp(-0.5 * x * y);
}

Scalar Rayleigh::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  if (x <= 0.0) return SpecFunc::LowestScalar;
  const Scalar y = x / (beta_ * beta_);
  return std::log(y) - 0.5 * x * y;
}


/* Get the CDF of the distribution */
Scalar Rayleigh::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  if (x <= 0.0) return 0.0;
  const Scalar y = x / beta_;
  return 1.0 - std::exp(-0.5 * y * y);
}

/* Compute the entropy of the distribution */
Scalar Rayleigh::computeEntropy() const
{
  return 1.0 + 0.5 * (SpecFunc::EulerConstant - M_LN2) + std::log(beta_);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X))
   phi(x) = (1 - beta * x * exp(-beta^2 * x^2 / 2) * sqrt(pi / 2) * (erfi(beta * x / sqrt(2)) - i)) * exp(i * gamma * x)
   erfi(t) = -i * erf(i * t) = 2 / sqrt(pi) * int(exp(u^2), u=0..t)
   dawson(t) = exp(-t^2) * int(exp(u^2), t=0..t)
   erfi(t) = 2 / sqrt(pi) * exp(t^2) * dawson(t)
   phi(x) = (1 - beta * x * exp(-beta^2 * x^2 / 2) * sqrt(pi / 2) * (2 / sqrt(pi) * exp(beta^2 * x^2 / 2) * dawson(beta * x / sqrt(2)) - i)) * exp(i * gamma * x)
   = (1 - beta * x * (sqrt(2) * dawson(beta * x / sqrt(2)) - i * exp(-beta^2 * x^2 / 2) * sqrt(pi/2))) * exp(i * gamma * x)
   = (1 - t * (2 * dawson(t) - i * exp(-t * t) * sqrt(pi))) * exp(i * gamma * x) with t = beta * x / sqrt(2)
*/
Complex Rayleigh::computeCharacteristicFunction(const Scalar x) const
{
  const Scalar t = beta_ * x / std::sqrt(2.0);
  return Complex(1 - 2 * t * SpecFunc::Dawson(t), t * std::exp(-t * t) * std::sqrt(M_PI)) * std::exp(Complex(0.0, x * gamma_));
}

/* Get the PDFGradient of the distribution */
Point Rayleigh::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  Point pdfGradient(2, 0.0);
  if (x <= 0.0) return pdfGradient;
  const Scalar beta2 = beta_ * beta_;
  const Scalar factor1 = computePDF(point) / beta2;
  const Scalar factor2 = (x - beta_) * (x + beta_);
  pdfGradient[0] = factor1 * (factor2 - beta2) / beta_;
  pdfGradient[1] = factor1 *  factor2 / x;
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
Point Rayleigh::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  Point cdfGradient(2, 0.0);
  const Scalar pdf = computePDF(point);
  if (x <= 0.0) return cdfGradient;
  cdfGradient[0] = -x * pdf / beta_;
  cdfGradient[1] = -pdf;
  return cdfGradient;
}

/* Get the quantile of the distribution */
Scalar Rayleigh::computeScalarQuantile(const Scalar prob,
                                       const Bool tail) const
{
  if (tail) return gamma_ + beta_ * std::sqrt(-2.0 * std::log(prob));
  return gamma_ + beta_ * std::sqrt(-2.0 * log1p(-prob));
}

/* Compute the mean of the distribution */
void Rayleigh::computeMean() const
{
  // 1.253314137315500251207882 = sqrt(pi/2)
  mean_ = Point(1, gamma_ + 1.253314137315500251207882 * beta_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point Rayleigh::getStandardDeviation() const
{
  // 0.6551363775620335530939357 = sqrt(2 - pi / 2)
  return Point(1, 0.6551363775620335530939357 * beta_);
}

/* Get the skewness of the distribution */
Point Rayleigh::getSkewness() const
{
  // 0.631110657818937138918970 = 2 * sqrt(pi / (4 - pi)) * (pi - 3) / (4 - pi)
  return Point(1, 0.631110657818937138918970);
}

/* Get the kurtosis of the distribution */
Point Rayleigh::getKurtosis() const
{
  // 3.245089300687638062848667 = 3 - (6 * pi^2 - 24 * pi + 16) / (4 - pi)^2
  return Point(1, 3.245089300687638062848667);
}

/* Get the moments of the distribution */
Point Rayleigh::getStandardMoment(const UnsignedInteger n) const
{
  return Point(1, std::exp(0.5 * n * M_LN2 + SpecFunc::LnGamma(1.0 + 0.5 * n)));
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution Rayleigh::getStandardRepresentative() const
{
  return new Rayleigh(1.0, 0.0);
}

/* Compute the covariance of the distribution */
void Rayleigh::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  // 0.429203673205103380768678 = (4 - pi) / 2
  covariance_(0, 0) = 0.429203673205103380768678 * beta_ * beta_;
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value accessor */
Point Rayleigh::getParameter() const
{
  return {beta_, gamma_};
}

void Rayleigh::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = Rayleigh(parameter[0], parameter[1]);
  setWeight(w);
}

/* Parameters description accessor */
Description Rayleigh::getParameterDescription() const
{
  return {"beta", "gamma"};
}

/* Sigma accessor */
void Rayleigh::setBeta(const Scalar beta)
{
  if (!(beta > 0.0)) throw InvalidArgumentException(HERE) << "Beta MUST be positive";
  if (beta != beta_)
  {
    beta_ = beta;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

Scalar Rayleigh::getBeta() const
{
  return beta_;
}

/* Gamma accessor */
void Rayleigh::setGamma(const Scalar gamma)
{
  if (gamma != gamma_)
  {
    gamma_ = gamma;
    isAlreadyComputedMean_ = false;
    // The covariance does not depend on gamma
    computeRange();
  }
}

Scalar Rayleigh::getGamma() const
{
  return gamma_;
}

/* Method save() stores the object through the StorageManager */
void Rayleigh::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "beta_", beta_ );
  adv.saveAttribute( "gamma_", gamma_ );
}

/* Method load() reloads the object from the StorageManager */
void Rayleigh::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  if (adv.hasAttribute("sigma_")) // old parameter
    adv.loadAttribute( "sigma_", beta_ );
  else
    adv.loadAttribute( "beta_", beta_ );
  adv.loadAttribute( "gamma_", gamma_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
