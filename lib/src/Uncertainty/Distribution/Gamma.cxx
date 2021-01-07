//                                               -*- C++ -*-
/**
 *  @brief The Gamma distribution
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Gamma.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Gamma)

static const Factory<Gamma> Factory_Gamma;

/* Default constructor */
Gamma::Gamma()
  : ContinuousDistribution()
  , k_(1.0)
  , lambda_(1.0)
  , gamma_(0.0)
  , normalizationFactor_(0.0)
{
  setName("Gamma");
  setDimension(1);
  computeRange();
}

/* Parameters constructor */
Gamma::Gamma(const Scalar k,
             const Scalar lambda,
             const Scalar gamma)
  : ContinuousDistribution()
  , k_(0.0)
  , lambda_(0.0)
  , gamma_(gamma)
  , normalizationFactor_(0.0)
{
  setName("Gamma");
  setKLambda(k, lambda);
  setDimension(1);
}

/* Comparison operator */
Bool Gamma::operator ==(const Gamma & other) const
{
  if (this == &other) return true;
  return (k_ == other.k_) && (lambda_ == other.lambda_) && (gamma_ == other.gamma_);
}

Bool Gamma::equals(const DistributionImplementation & other) const
{
  const Gamma* p_other = dynamic_cast<const Gamma*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Gamma::__repr__() const
{
  OSS oss;
  oss << "class=" << Gamma::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " k=" << k_
      << " lambda=" << lambda_
      << " gamma=" << gamma_;
  return oss;
}

String Gamma::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(k = " << k_ << ", lambda = " << lambda_ << ", gamma = " << gamma_ << ")";
  return oss;
}

/* K accessor */
void Gamma::setK(const Scalar k)
{
  if (!(k > 0.0)) throw InvalidArgumentException(HERE) << "K MUST be positive";
  if (k != k_)
  {
    k_ = k;
    computeRange();
    update();
  }
}

Scalar Gamma::getK() const
{
  return k_;
}


/* Lambda accessor */
void Gamma::setLambda(const Scalar lambda)
{
  if (!(lambda > 0.0)) throw InvalidArgumentException(HERE) << "Lambda MUST be positive";
  if (lambda != lambda_)
  {
    lambda_ = lambda;
    computeRange();
    update();
  }
}

Scalar Gamma::getLambda() const
{
  return lambda_;
}

/* K and lambda accessor */
void Gamma::setKLambda(const Scalar k,
                       const Scalar lambda)
{
  if (!(k > 0.0)) throw InvalidArgumentException(HERE) << "K MUST be positive";
  if (!(lambda > 0.0)) throw InvalidArgumentException(HERE) << "Lambda MUST be positive";
  if ((k != k_) || (lambda != lambda_))
  {
    k_ = k;
    lambda_ = lambda;
    computeRange();
    update();
  }
}

/* Gamma accessor */
void Gamma::setGamma(const Scalar gamma)
{
  if (gamma != gamma_)
  {
    gamma_ = gamma;
    isAlreadyComputedMean_ = false;
    // The covariance does not depend on gamma
    computeRange();
  }
}

Scalar Gamma::getGamma() const
{
  return gamma_;
}

/* Virtual constructor */
Gamma * Gamma::clone() const
{
  return new Gamma(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void Gamma::computeRange()
{
  const Point lowerBound(1, gamma_);
  const Point upperBound(computeUpperBound());
  const Interval::BoolCollection finiteLowerBound(1, true);
  const Interval::BoolCollection finiteUpperBound(1, false);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/** Update the derivative attributes */
void Gamma::update()
{
  // For large k we use the following normalization factor:
  // normalizationFactor = log(lambda*k^{k-1}/Gamma(k))
  // which is expanded wrt k
  if (k_ >= 6.9707081224932495879)
  {
    static const Scalar alpha[10] = {0.91893853320467274177, 0.83333333333333333333e-1, -0.27777777777777777778e-2, 0.79365079365079365079e-3, -0.59523809523809523810e-3, 0.84175084175084175084e-3, -0.19175269175269175269e-2, 0.64102564102564102564e-2, -0.29550653594771241830e-1, 0.17964437236883057316};
    const Scalar ik = 1.0 / k_;
    const Scalar ik2 = ik * ik;
    normalizationFactor_ = std::log(lambda_) + k_ - 0.5 * std::log(k_) - (alpha[0] + ik * (alpha[1] + ik2 * (alpha[2] + ik2 * (alpha[3] + ik2 * (alpha[4] + ik2 * (alpha[5] + ik2 * (alpha[6] + ik2 * (alpha[7] + ik2 * (alpha[8] + ik2 * alpha[9])))))))));
  }
  // For small k, the normalization factor is:
  // normalizationFactor = log(lambda/Gamma(k))
  else normalizationFactor_ = std::log(lambda_) - SpecFunc::LnGamma(k_);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
}


/* Get one realization of the distribution */
Point Gamma::getRealization() const
{
  return Point(1, gamma_ + DistFunc::rGamma(k_) / lambda_);
}


/* Get the DDF of the distribution */
Point Gamma::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  if (x <= 0.0) return Point(1, 0.0);
  return Point(1, ((k_ - 1.0) / x - lambda_) * computePDF(point));
}


/* Get the PDF of the distribution */
Scalar Gamma::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return computePDF(point[0]);
}

Scalar Gamma::computePDF(const Scalar u) const
{
  if (u <= gamma_) return 0.0;
  return std::exp(computeLogPDF(u));
}

/* Get the log PDF of the distribution */
Scalar Gamma::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return computeLogPDF(point[0]);
}

Scalar Gamma::computeLogPDF(const Scalar u) const
{
  // From textbook, we have log(PDF(u)) =  - lambda * (u - gamma) + (k - 1) * log(u - gamma) + k * log(lambda) - log(Gamma(k))
  const Scalar x = lambda_ * (u - gamma_);
  if (x <= 0.0) return SpecFunc::LowestScalar;
  // Use asymptotic expansion for large k
  // Here log(PDF(u)) = L - lambda * (u - gamma) + (k - 1) * log(lambda * (u - gamma) / k)
  if (k_ >= 6.9707081224932495879) return normalizationFactor_ - x + (k_ - 1.0) * std::log(x / k_);
  return normalizationFactor_ + (k_ - 1) * std::log(x) - x;
}

/* Get the CDF of the distribution */
Scalar Gamma::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return computeCDF(point[0]);
}

Scalar Gamma::computeCDF(const Scalar u) const
{
  const Scalar x = lambda_ * (u - gamma_);
  // No test here as the CDF is continuous for all k_
  if (x <= 0.0) return 0.0;
  return DistFunc::pGamma(k_, x);
}

Scalar Gamma::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return computeComplementaryCDF(point[0]);
}

Scalar Gamma::computeComplementaryCDF(const Scalar u) const
{
  const Scalar x = lambda_ * (u - gamma_);
  // No test here as the CDF is continuous for all k_
  if (x <= 0.0) return 1.0;
  return DistFunc::pGamma(k_, x, true);
}

/* Compute the entropy of the distribution */
Scalar Gamma::computeEntropy() const
{
  return k_ - std::log(lambda_) + SpecFunc::LogGamma(k_) + (1.0 - k_) * SpecFunc::Psi(k_);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex Gamma::computeCharacteristicFunction(const Scalar x) const
{
  return std::exp(Complex(0.0, x * gamma_)) * std::pow(Complex(1.0, -x / lambda_), -k_);
}

Complex Gamma::computeLogCharacteristicFunction(const Scalar x) const
{
  return Complex(0.0, x * gamma_) - k_ * std::log(Complex(1.0, -x / lambda_));
}

/* Get the PDFGradient of the distribution */
Point Gamma::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  Point pdfGradient(3, 0.0);
  const Scalar x = point[0] - gamma_;
  if (x <= 0.0) return pdfGradient;
  const Scalar pdf = computePDF(point);
  pdfGradient[0] = (std::log(x) + std::log(lambda_) - SpecFunc::Psi(k_)) * pdf;
  pdfGradient[1] = (k_ / lambda_ - x) * pdf;
  pdfGradient[2] = ((1.0 - k_) / x + lambda_) * pdf;
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
Point Gamma::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  Point cdfGradient(3, 0.0);
  const Scalar x = point[0] - gamma_;
  if (x <= 0.0) return cdfGradient;
  const Scalar lambdaX = lambda_ * x;
  const Scalar factor = std::exp(k_ * std::log(lambdaX) - SpecFunc::LnGamma(k_) - lambdaX);
  const Scalar eps = std::pow(cdfEpsilon_, 1.0 / 3.0);
  cdfGradient[0] = (DistFunc::pGamma(k_ + eps, lambda_ * x) - DistFunc::pGamma(k_ - eps, lambda_ * x)) / (2.0 * eps);
  cdfGradient[1] = factor / lambda_;
  cdfGradient[2] = -factor / x;
  return cdfGradient;
}

/* Get the quantile of the distribution */
Scalar Gamma::computeScalarQuantile(const Scalar prob,
                                    const Bool tail) const
{
  return gamma_ + DistFunc::qGamma(k_, prob, tail) / lambda_;
}

/* Compute the mean of the distribution */
void Gamma::computeMean() const
{
  mean_ = Point(1, gamma_ + k_ / lambda_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point Gamma::getStandardDeviation() const
{
  return Point(1, std::sqrt(k_) / lambda_);
}

/* Get the skewness of the distribution */
Point Gamma::getSkewness() const
{
  return Point(1, 2.0 / std::sqrt(k_));
}

/* Get the kurtosis of the distribution */
Point Gamma::getKurtosis() const
{
  return Point(1, 3.0 * (k_ + 2.0) / k_);
}

/* Get the moments of the standardized distribution */
Point Gamma::getStandardMoment(const UnsignedInteger n) const
{
  return Point(1, std::exp(SpecFunc::LogGamma(n + k_) - SpecFunc::LogGamma(k_)));
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution Gamma::getStandardRepresentative() const
{
  return new Gamma(k_, 1.0, 0.0);
}

/* Compute the covariance of the distribution */
void Gamma::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = k_ / (lambda_ * lambda_);
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value accessor */
Point Gamma::getParameter() const
{
  Point point(3);
  point[0] = k_;
  point[1] = lambda_;
  point[2] = gamma_;
  return point;
}

void Gamma::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 3) throw InvalidArgumentException(HERE) << "Error: expected 3 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = Gamma(parameter[0], parameter[1], parameter[2]);
  setWeight(w);
}

/* Parameters description accessor */
Description Gamma::getParameterDescription() const
{
  Description description(3);
  description[0] = "k";
  description[1] = "lambda";
  description[2] = "gamma";
  return description;
}

/* Method save() stores the object through the StorageManager */
void Gamma::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "k_", k_ );
  adv.saveAttribute( "lambda_", lambda_ );
  adv.saveAttribute( "gamma_", gamma_ );
  adv.saveAttribute( "normalizationFactor_", normalizationFactor_ );
}

/* Method load() reloads the object from the StorageManager */
void Gamma::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "k_", k_ );
  adv.loadAttribute( "lambda_", lambda_ );
  adv.loadAttribute( "gamma_", gamma_ );
  adv.loadAttribute( "normalizationFactor_", normalizationFactor_ );
  computeRange();
}




END_NAMESPACE_OPENTURNS
