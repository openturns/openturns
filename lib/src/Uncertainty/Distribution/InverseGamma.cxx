//                                               -*- C++ -*-
/**
 *  @brief The InverseGamma distribution
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
#include "openturns/InverseGamma.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(InverseGamma)

static const Factory<InverseGamma> Factory_InverseGamma;

/* Default constructor */
InverseGamma::InverseGamma()
  : ContinuousDistribution()
  , lambda_(1.0)
  , k_(1.0)
  , normalizationFactor_(0.0)
{
  setName("InverseGamma");
  setDimension(1);
  computeRange();
}

/* Parameters constructor */
InverseGamma::InverseGamma(const Scalar lambda,
                           const Scalar k)
  : ContinuousDistribution()
  , lambda_(0.0)
  , k_(0.0)
  , normalizationFactor_(0.0)
{
  setName("InverseGamma");
  setKLambda(k, lambda);
  setDimension(1);
}

/* Comparison operator */
Bool InverseGamma::operator ==(const InverseGamma & other) const
{
  if (this == &other) return true;
  return (k_ == other.k_) && (lambda_ == other.lambda_);
}

Bool InverseGamma::equals(const DistributionImplementation & other) const
{
  const InverseGamma* p_other = dynamic_cast<const InverseGamma*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String InverseGamma::__repr__() const
{
  OSS oss;
  oss << "class=" << InverseGamma::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " lambda=" << lambda_
      << " k=" << k_;
  return oss;
}

String InverseGamma::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(lambda = " << lambda_ << ", k = " << k_ << ")";
  return oss;
}

/* K accessor */
void InverseGamma::setK(const Scalar k)
{
  if (!(k > 0.0)) throw InvalidArgumentException(HERE) << "K MUST be positive";
  if (k != k_)
  {
    k_ = k;
    computeRange();
    update();
  }
}

Scalar InverseGamma::getK() const
{
  return k_;
}


/* Lambda accessor */
void InverseGamma::setLambda(const Scalar lambda)
{
  if (!(lambda > 0.0)) throw InvalidArgumentException(HERE) << "Lambda MUST be positive";
  if (lambda != lambda_)
  {
    lambda_ = lambda;
    computeRange();
    update();
  }
}

Scalar InverseGamma::getLambda() const
{
  return lambda_;
}

/* K and lambda accessor */
void InverseGamma::setKLambda(const Scalar k,
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

/* Virtual constructor */
InverseGamma * InverseGamma::clone() const
{
  return new InverseGamma(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void InverseGamma::computeRange()
{
  const Point lowerBound(1, 0.0);
  const Point upperBound(computeUpperBound());
  const Interval::BoolCollection finiteLowerBound(1, true);
  const Interval::BoolCollection finiteUpperBound(1, false);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/** Update the derivative attributes */
void InverseGamma::update()
{
  // For large k we use the following normalization factor:
  // normalizationFactor = log(lambda*k^{k-1}/Gamma(k))
  //                     = log(lambda) + (k+1)log(k) - log(Gamma(k))
  // which is expanded wrt k
  if (k_ >= 6.9707081224932495879)
  {
    static const Scalar alpha[10] = {0.91893853320467274177, 0.83333333333333333333e-1, -0.27777777777777777778e-2, 0.79365079365079365079e-3, -0.59523809523809523810e-3, 0.84175084175084175084e-3, -0.19175269175269175269e-2, 0.64102564102564102564e-2, -0.29550653594771241830e-1, 0.17964437236883057316};
    const Scalar ik = 1.0 / k_;
    const Scalar ik2 = ik * ik;
    normalizationFactor_ = std::log(lambda_) + k_ + 1.5 * std::log(k_) - (alpha[0] + ik * (alpha[1] + ik2 * (alpha[2] + ik2 * (alpha[3] + ik2 * (alpha[4] + ik2 * (alpha[5] + ik2 * (alpha[6] + ik2 * (alpha[7] + ik2 * (alpha[8] + ik2 * alpha[9])))))))));
  }
  // For small k, the normalization factor is:
  // normalizationFactor = log(lambda/Gamma(k))
  //                     = log(lambda) - log(Gamma(k))
  else normalizationFactor_ = std::log(lambda_) - SpecFunc::LnGamma(k_);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
}


/* Get one realization of the distribution */
Point InverseGamma::getRealization() const
{
  return Point(1, 1.0 / (lambda_ * DistFunc::rGamma(k_)));
}


/* Get the DDF of the distribution */
Point InverseGamma::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  if (x <= 0.0) return Point(1, 0.0);
  return Point(1, (1.0 / (lambda_ * x) - (k_ + 1.0)) * computePDF(point) / x);
}


/* Get the PDF of the distribution */
Scalar InverseGamma::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  if (point[0] <= 0.0) return 0.0;
  return std::exp(computeLogPDF(point));
}

Scalar InverseGamma::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  // From textbook, we have log(PDF(x)) =  log(lambda)-log(Gamma(k))-(k+1)*log(lambda*x)-1/(lambda*x)
  const Scalar u = lambda_ * point[0];
  if (u <= 0.0) return SpecFunc::LowestScalar;
  // Use asymptotic expansion for large k
  // Here log(PDF(x)) = L - (k-1)*log(k)-(k+1)*log(lambda*x)-1/(lambda*x)
  if (k_ >= 6.9707081224932495879) return normalizationFactor_ - (k_ + 1.0) * std::log(k_ * u) - 1.0 / u;
  return normalizationFactor_ - (k_ + 1.0) * std::log(u) - 1.0 / u;
}

/* Get the CDF of the distribution */
Scalar InverseGamma::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  // No test here as the CDF is continuous for all k_
  if (x <= 0.0) return 0.0;
  return DistFunc::pGamma(k_, 1.0 / (lambda_ * x), true);
}

Scalar InverseGamma::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  // No test here as the CDF is continuous for all k_
  if (x <= 0.0) return 1.0;
  return DistFunc::pGamma(k_, 1.0 / (lambda_ * x));
}

/* Compute the entropy of the distribution */
Scalar InverseGamma::computeEntropy() const
{
  return SpecFunc::LogGamma(k_) - (k_ + 1.0) * SpecFunc::Psi(k_) + k_ - std::log(lambda_);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex InverseGamma::computeCharacteristicFunction(const Scalar x) const
{
  return DistributionImplementation::computeCharacteristicFunction(x);
}

Complex InverseGamma::computeLogCharacteristicFunction(const Scalar x) const
{
  return DistributionImplementation::computeLogCharacteristicFunction(x);
}

/* Get the PDFGradient of the distribution */
Point InverseGamma::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  Point pdfGradient(2);
  const Scalar x = point[0];
  if (x <= 0.0) return pdfGradient;
  const Scalar pdf = computePDF(point);
  pdfGradient[0] = (1.0 / (lambda_ * x) - k_) * pdf / lambda_;
  pdfGradient[1] = -(std::log(lambda_) + std::log(x) + SpecFunc::DiGamma(k_)) * pdf;
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
Point InverseGamma::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  Point cdfGradient(2, 0.0);
  const Scalar x = point[0];
  if (x <= 0.0) return cdfGradient;
  const Scalar lambdaXInverse = 1.0 / (lambda_ * x);
  const Scalar pdf = computePDF(x);
  const Scalar eps = std::pow(cdfEpsilon_, 1.0 / 3.0);
  cdfGradient[0] = pdf * x / lambda_;
  cdfGradient[1] = (DistFunc::pGamma(k_ + eps, lambdaXInverse, true) - DistFunc::pGamma(k_ - eps, lambdaXInverse, true)) / (2.0 * eps);
  return cdfGradient;
}

/* Get the quantile of the distribution */
Scalar InverseGamma::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  return 1.0 / (lambda_ * DistFunc::qGamma(k_, prob, !tail));
}

/* Compute the mean of the distribution */
void InverseGamma::computeMean() const
{
  if (!(k_ > 1.0)) throw NotDefinedException(HERE) << "InverseGamma mean is defined only for k > 1, here k=" << k_;
  mean_ = Point(1, 1.0 / (lambda_ * (k_ - 1.0)));
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point InverseGamma::getStandardDeviation() const
{
  if (!(k_ > 2.0)) throw NotDefinedException(HERE) << "InverseGamma standard deviation is defined only for k > 2, here k=" << k_;
  return Point(1, std::sqrt(getCovariance()(0, 0)));
}

/* Get the skewness of the distribution */
Point InverseGamma::getSkewness() const
{
  if (!(k_ > 3.0)) throw NotDefinedException(HERE) << "InverseGamma skewness is defined only for k > 3, here k=" << k_;
  return Point(1, 4.0 * std::sqrt(k_ - 2.0) / (k_ - 3.0));
}

/* Get the kurtosis of the distribution */
Point InverseGamma::getKurtosis() const
{
  if (!(k_ > 4.0)) throw NotDefinedException(HERE) << "InverseGamma kurtosis is defined only for k > 4, here k=" << k_;
  return Point(1, 3.0 * (k_ * (k_ + 3.0) - 10.0) / ((k_ - 3.0) * (k_ - 4.0)));
}

/* Get the moments of the standardized distribution */
Point InverseGamma::getStandardMoment(const UnsignedInteger n) const
{
  if (k_ <= n) throw NotDefinedException(HERE) << "InverseGamma standard moment of order " << n << " is defined only for k > " << n << ", here k=" << k_;
  return Point(1, std::exp(SpecFunc::LogGamma(k_ - n) - SpecFunc::LogGamma(k_)));
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution InverseGamma::getStandardRepresentative() const
{
  return new InverseGamma(k_, 1.0);
}

/* Compute the covariance of the distribution */
void InverseGamma::computeCovariance() const
{
  if (!(k_ > 2.0)) throw NotDefinedException(HERE) << "InverseGamma covariance is defined only for k > 2, here k=" << k_;
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = 1.0 / (lambda_ * lambda_ * (k_ - 1.0) * (k_ - 1.0) * (k_ - 2.0));
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value and description accessor */
Point InverseGamma::getParameter() const
{
  Point point(2);
  point[0] = lambda_;
  point[1] = k_;
  return point;
}

void InverseGamma::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 parameters, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = InverseGamma(parameter[0], parameter[1]);
  setWeight(w);
}

/* Parameters description accessor */
Description InverseGamma::getParameterDescription() const
{
  Description description(2);
  description[0] = "lambda";
  description[1] = "k";
  return description;
}

/* Method save() stores the object through the StorageManager */
void InverseGamma::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "lambda_", lambda_ );
  adv.saveAttribute( "k_", k_ );
  adv.saveAttribute( "normalizationFactor_", normalizationFactor_ );
}

/* Method load() reloads the object from the StorageManager */
void InverseGamma::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "lambda_", lambda_ );
  adv.loadAttribute( "k_", k_ );
  adv.loadAttribute( "normalizationFactor_", normalizationFactor_ );
  computeRange();
}




END_NAMESPACE_OPENTURNS
