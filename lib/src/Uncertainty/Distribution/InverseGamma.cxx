//                                               -*- C++ -*-
/**
 *  @brief The InverseGamma distribution
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "InverseGamma.hxx"
#include "RandomGenerator.hxx"
#include "SpecFunc.hxx"
#include "DistFunc.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(InverseGamma);

static const Factory<InverseGamma> RegisteredFactory;

/* Default constructor */
InverseGamma::InverseGamma()
  : ContinuousDistribution()
  , k_(1.0)
  , lambda_(1.0)
  , normalizationFactor_(0.0)
{
  setName("InverseGamma");
  setDimension(1);
  computeRange();
}

/* Parameters constructor */
InverseGamma::InverseGamma(const NumericalScalar k,
                           const NumericalScalar lambda)
  : ContinuousDistribution()
  , k_(0.0)
  , lambda_(0.0)
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

/* String converter */
String InverseGamma::__repr__() const
{
  OSS oss;
  oss << "class=" << InverseGamma::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " k=" << k_
      << " lambda=" << lambda_;
  return oss;
}

String InverseGamma::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(k = " << k_ << ", lambda = " << lambda_ << ")";
  return oss;
}

/* K accessor */
void InverseGamma::setK(const NumericalScalar k)
{
  if (k <= 0.0) throw InvalidArgumentException(HERE) << "K MUST be positive";
  if (k != k_)
  {
    k_ = k;
    computeRange();
    update();
  }
}

NumericalScalar InverseGamma::getK() const
{
  return k_;
}


/* Lambda accessor */
void InverseGamma::setLambda(const NumericalScalar lambda)
{
  if (lambda <= 0.0) throw InvalidArgumentException(HERE) << "Lambda MUST be positive";
  if (lambda != lambda_)
  {
    lambda_ = lambda;
    computeRange();
    update();
  }
}

NumericalScalar InverseGamma::getLambda() const
{
  return lambda_;
}

/* K and lambda accessor */
void InverseGamma::setKLambda(const NumericalScalar k,
                              const NumericalScalar lambda)
{
  if (k <= 0.0) throw InvalidArgumentException(HERE) << "K MUST be positive";
  if (lambda <= 0.0) throw InvalidArgumentException(HERE) << "Lambda MUST be positive";
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
  const NumericalPoint lowerBound(1, 0.0);
  const NumericalPoint upperBound(computeUpperBound());
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
    static const NumericalScalar alpha[10] = {0.91893853320467274177, 0.83333333333333333333e-1, -0.27777777777777777778e-2, 0.79365079365079365079e-3, -0.59523809523809523810e-3, 0.84175084175084175084e-3, -0.19175269175269175269e-2, 0.64102564102564102564e-2, -0.29550653594771241830e-1, 0.17964437236883057316};
    const NumericalScalar ik(1.0 / k_);
    const NumericalScalar ik2(ik * ik);
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
NumericalPoint InverseGamma::getRealization() const
{
  return NumericalPoint(1, 1.0 / (lambda_ * DistFunc::rGamma(k_)));
}


/* Get the DDF of the distribution */
NumericalPoint InverseGamma::computeDDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if (x <= 0.0) return NumericalPoint(1, 0.0);
  return NumericalPoint(1, (1.0 / (lambda_ * x) - (k_ + 1.0)) * computePDF(point) / x);
}


/* Get the PDF of the distribution */
NumericalScalar InverseGamma::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  if (point[0] <= 0.0) return 0.0;
  return std::exp(computeLogPDF(point));
}

NumericalScalar InverseGamma::computeLogPDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  // From textbook, we have log(PDF(x)) =  log(lambda)-log(Gamma(k))-(k+1)*log(lambda*x)-1/(lambda*x)
  const NumericalScalar u(lambda_ * point[0]);
  if (u <= 0.0) return -SpecFunc::MaxNumericalScalar;
  // Use asymptotic expansion for large k
  // Here log(PDF(x)) = L - (k-1)*log(k)-(k+1)*log(lambda*x)-1/(lambda*x)
  if (k_ >= 6.9707081224932495879) return normalizationFactor_ - (k_ + 1.0) * std::log(k_ * u) - 1.0 / u;
  return normalizationFactor_ - (k_ + 1.0) * std::log(u) - 1.0 / u;
}

/* Get the CDF of the distribution */
NumericalScalar InverseGamma::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  // No test here as the CDF is continuous for all k_
  if (x <= 0.0) return 0.0;
  return DistFunc::pGamma(k_, 1.0 / (lambda_ * x), true);
}

NumericalScalar InverseGamma::computeComplementaryCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  // No test here as the CDF is continuous for all k_
  if (x <= 0.0) return 1.0;
  return DistFunc::pGamma(k_, 1.0 / (lambda_ * x));
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex InverseGamma::computeCharacteristicFunction(const NumericalScalar x) const
{
  return DistributionImplementation::computeCharacteristicFunction(x);
}

NumericalComplex InverseGamma::computeLogCharacteristicFunction(const NumericalScalar x) const
{
  return DistributionImplementation::computeLogCharacteristicFunction(x);
}

/* Get the PDFGradient of the distribution */
NumericalPoint InverseGamma::computePDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  NumericalPoint pdfGradient(2);
  const NumericalScalar x(point[0]);
  if (x <= 0.0) return pdfGradient;
  const NumericalScalar pdf(computePDF(point));
  pdfGradient[0] = -(std::log(lambda_) + std::log(x) + SpecFunc::DiGamma(k_)) * pdf;
  pdfGradient[1] = (1.0 / (lambda_ * x) - k_) * pdf / lambda_;
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
NumericalPoint InverseGamma::computeCDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  NumericalPoint cdfGradient(2, 0.0);
  const NumericalScalar x(point[0]);
  if (x <= 0.0) return cdfGradient;
  const NumericalScalar lambdaXInverse(1.0 / (lambda_ * x));
  const NumericalScalar pdf(computePDF(x));
  const NumericalScalar eps(std::pow(cdfEpsilon_, 1.0 / 3.0));
  cdfGradient[0] = (DistFunc::pGamma(k_ + eps, lambdaXInverse, true) - DistFunc::pGamma(k_ - eps, lambdaXInverse, true)) / (2.0 * eps);
  cdfGradient[1] = pdf * x / lambda_;
  return cdfGradient;
}

/* Get the quantile of the distribution */
NumericalScalar InverseGamma::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  return 1.0 / (lambda_ * DistFunc::qGamma(k_, prob, !tail));
}

/* Compute the mean of the distribution */
void InverseGamma::computeMean() const
{
  if (k_ <= 1.0) throw NotDefinedException(HERE) << "InverseGamma mean is defined only for k > 1, here k=" << k_;
  mean_ = NumericalPoint(1, 1.0 / (lambda_ * (k_ - 1.0)));
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
NumericalPoint InverseGamma::getStandardDeviation() const
{
  if (k_ <= 2.0) throw NotDefinedException(HERE) << "InverseGamma standard deviation is defined only for k > 2, here k=" << k_;
  return NumericalPoint(1, std::sqrt(getCovariance()(0, 0)));
}

/* Get the skewness of the distribution */
NumericalPoint InverseGamma::getSkewness() const
{
  if (k_ <= 3.0) throw NotDefinedException(HERE) << "InverseGamma skewness is defined only for k > 3, here k=" << k_;
  return NumericalPoint(1, 4.0 * std::sqrt(k_ - 2.0) / (k_ - 3.0));
}

/* Get the kurtosis of the distribution */
NumericalPoint InverseGamma::getKurtosis() const
{
  if (k_ <= 4.0) throw NotDefinedException(HERE) << "InverseGamma kurtosis is defined only for k > 4, here k=" << k_;
  return NumericalPoint(1, 3.0 * (k_ * (k_ + 3.0) - 10.0) / ((k_ - 3.0) * (k_ - 4.0)));
}

/* Get the moments of the standardized distribution */
NumericalPoint InverseGamma::getStandardMoment(const UnsignedInteger n) const
{
  if (k_ <= n) throw NotDefinedException(HERE) << "InverseGamma standard moment of order " << n << " is defined only for k > " << n << ", here k=" << k_;
  return NumericalPoint(1, std::exp(SpecFunc::LogGamma(k_ - n) - SpecFunc::LogGamma(k_)));
}

/* Get the standard representative in the parametric family, associated with the standard moments */
InverseGamma::Implementation InverseGamma::getStandardRepresentative() const
{
  return InverseGamma(k_, 1.0).clone();
}

/* Compute the covariance of the distribution */
void InverseGamma::computeCovariance() const
{
  if (k_ <= 2.0) throw NotDefinedException(HERE) << "InverseGamma covariance is defined only for k > 2, here k=" << k_;
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = 1.0 / (lambda_ * lambda_ * (k_ - 1.0) * (k_ - 1.0) * (k_ - 2.0));
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value and description accessor */
InverseGamma::NumericalPointWithDescriptionCollection InverseGamma::getParametersCollection() const
{
  NumericalPointWithDescriptionCollection parameters(1);
  NumericalPointWithDescription point(2);
  Description description(point.getDimension());
  point[0] = k_;
  point[1] = lambda_;
  description[0] = "k";
  description[1] = "lambda";
  point.setDescription(description);
  point.setName(getDescription()[0]);
  parameters[0] = point;
  return parameters;
}

void InverseGamma::setParameters(const NumericalPoint & parameters)
{
  if (parameters.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 parameters, got " << parameters.getSize();

  const NumericalScalar w(getWeight());
  *this = InverseGamma(parameters[0], parameters[1]);
  setWeight(w);
}

/* Method save() stores the object through the StorageManager */
void InverseGamma::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "k_", k_ );
  adv.saveAttribute( "lambda_", lambda_ );
  adv.saveAttribute( "normalizationFactor_", normalizationFactor_ );
}

/* Method load() reloads the object from the StorageManager */
void InverseGamma::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "k_", k_ );
  adv.loadAttribute( "lambda_", lambda_ );
  adv.loadAttribute( "normalizationFactor_", normalizationFactor_ );
  computeRange();
}




END_NAMESPACE_OPENTURNS
