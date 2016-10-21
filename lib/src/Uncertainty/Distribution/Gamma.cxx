//                                               -*- C++ -*-
/**
 *  @brief The Gamma distribution
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
#include "openturns/Gamma.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Gamma);

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
Gamma::Gamma(const NumericalScalar k,
             const NumericalScalar lambda,
             const NumericalScalar gamma)
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

/* Parameters constructor */
Gamma::Gamma(const NumericalScalar arg1,
             const NumericalScalar arg2,
             const NumericalScalar gamma,
             const ParameterSet set)
  : ContinuousDistribution()
  , k_(0.0)
  , lambda_(0.0)
  , gamma_(gamma)
  , normalizationFactor_(0.0)
{
  Log::Warn(OSS() << "Gamma parameter set constructor is deprecated.");
  setName("Gamma");
  switch (set)
  {
    case KLAMBDA:
      // This call set also the range.
      setKLambda(arg1, arg2);
      break;

    case MUSIGMA:
      // This call set also the range.
      setMuSigma(arg1, arg2);
      break;

    default:
      throw InvalidArgumentException(HERE) << "Invalid parameter set argument";

  } /* end switch */
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

String Gamma::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(k = " << k_ << ", lambda = " << lambda_ << ", gamma = " << gamma_ << ")";
  return oss;
}

/* K accessor */
void Gamma::setK(const NumericalScalar k)
{
  if (k <= 0.0) throw InvalidArgumentException(HERE) << "K MUST be positive";
  if (k != k_)
  {
    k_ = k;
    computeRange();
    update();
  }
}

NumericalScalar Gamma::getK() const
{
  return k_;
}


/* Lambda accessor */
void Gamma::setLambda(const NumericalScalar lambda)
{
  if (lambda <= 0.0) throw InvalidArgumentException(HERE) << "Lambda MUST be positive";
  if (lambda != lambda_)
  {
    lambda_ = lambda;
    computeRange();
    update();
  }
}

NumericalScalar Gamma::getLambda() const
{
  return lambda_;
}

/* K and lambda accessor */
void Gamma::setKLambda(const NumericalScalar k,
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

/* Mu accessor */
void Gamma::setMuSigma(const NumericalScalar mu,
                       const NumericalScalar sigma)
{
  if (sigma <= 0.0) throw InvalidArgumentException(HERE) << "Sigma MUST be positive";
  const NumericalScalar eta = (mu - gamma_) / sigma;
  setKLambda(eta * eta, eta / sigma);
}

NumericalScalar Gamma::getMu() const
{
  Log::Warn(OSS() << "Gamma::getMu is deprecated");
  return gamma_ + k_ / lambda_;
}

NumericalScalar Gamma::getSigma() const
{
  Log::Warn(OSS() << "Gamma::getSigma is deprecated");
  return std::sqrt(k_) / lambda_;
}


/* Gamma accessor */
void Gamma::setGamma(const NumericalScalar gamma)
{
  if (gamma != gamma_)
  {
    gamma_ = gamma;
    isAlreadyComputedMean_ = false;
    // The covariance does not depend on gamma
    computeRange();
  }
}

NumericalScalar Gamma::getGamma() const
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
  const NumericalPoint lowerBound(1, gamma_);
  const NumericalPoint upperBound(computeUpperBound());
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
    static const NumericalScalar alpha[10] = {0.91893853320467274177, 0.83333333333333333333e-1, -0.27777777777777777778e-2, 0.79365079365079365079e-3, -0.59523809523809523810e-3, 0.84175084175084175084e-3, -0.19175269175269175269e-2, 0.64102564102564102564e-2, -0.29550653594771241830e-1, 0.17964437236883057316};
    const NumericalScalar ik = 1.0 / k_;
    const NumericalScalar ik2 = ik * ik;
    normalizationFactor_ = std::log(lambda_) + k_ - 0.5 * std::log(k_) - (alpha[0] + ik * (alpha[1] + ik2 * (alpha[2] + ik2 * (alpha[3] + ik2 * (alpha[4] + ik2 * (alpha[5] + ik2 * (alpha[6] + ik2 * (alpha[7] + ik2 * (alpha[8] + ik2 * alpha[9])))))))));
  }
  // For small k, the normalization factor is:
  // normalizationFactor = log(lambda/Gamma(k))
  else normalizationFactor_ = std::log(lambda_) - SpecFunc::LnGamma(k_);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
}


/* Get one realization of the distribution */
NumericalPoint Gamma::getRealization() const
{
  return NumericalPoint(1, gamma_ + DistFunc::rGamma(k_) / lambda_);
}


/* Get the DDF of the distribution */
NumericalPoint Gamma::computeDDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0] - gamma_;
  if (x <= 0.0) return NumericalPoint(1, 0.0);
  return NumericalPoint(1, ((k_ - 1.0) / x - lambda_) * computePDF(point));
}


/* Get the PDF of the distribution */
NumericalScalar Gamma::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0] - gamma_;
  if (x <= 0.0) return 0.0;
  return std::exp(computeLogPDF(point));
}

NumericalScalar Gamma::computeLogPDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  // From textbook, we have log(PDF(x)) =  - lambda * (x - gamma) + (k - 1) * log(x - gamma) + k * log(lambda) - log(Gamma(k))
  const NumericalScalar u = lambda_ * (point[0] - gamma_);
  if (u <= 0.0) return SpecFunc::LogMinNumericalScalar;
  // Use asymptotic expansion for large k
  // Here log(PDF(x)) = L - lambda * (x - gamma) + (k - 1) * log(lambda * (x - gamma) / k)
  if (k_ >= 6.9707081224932495879) return normalizationFactor_ - u + (k_ - 1.0) * std::log(u / k_);
  return normalizationFactor_ + (k_ - 1) * std::log(u) - u;
}

/* Get the CDF of the distribution */
NumericalScalar Gamma::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0] - gamma_;
  // No test here as the CDF is continuous for all k_
  if (x <= 0.0) return 0.0;
  return DistFunc::pGamma(k_, lambda_ * x);
}

NumericalScalar Gamma::computeComplementaryCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0] - gamma_;
  // No test here as the CDF is continuous for all k_
  if (x <= 0.0) return 1.0;
  return DistFunc::pGamma(k_, lambda_ * x, true);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex Gamma::computeCharacteristicFunction(const NumericalScalar x) const
{
  return std::exp(NumericalComplex(0.0, x * gamma_)) * std::pow(NumericalComplex(1.0, -x / lambda_), -k_);
}

NumericalComplex Gamma::computeLogCharacteristicFunction(const NumericalScalar x) const
{
  return NumericalComplex(0.0, x * gamma_) - k_ * std::log(NumericalComplex(1.0, -x / lambda_));
}

/* Get the PDFGradient of the distribution */
NumericalPoint Gamma::computePDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  NumericalPoint pdfGradient(3, 0.0);
  const NumericalScalar x = point[0] - gamma_;
  if (x <= 0.0) return pdfGradient;
  const NumericalScalar pdf = computePDF(point);
  pdfGradient[0] = (std::log(x) + std::log(lambda_) - SpecFunc::Psi(k_)) * pdf;
  pdfGradient[1] = (k_ / lambda_ - x) * pdf;
  pdfGradient[2] = ((1.0 - k_) / x + lambda_) * pdf;
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
NumericalPoint Gamma::computeCDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  NumericalPoint cdfGradient(3, 0.0);
  const NumericalScalar x = point[0] - gamma_;
  if (x <= 0.0) return cdfGradient;
  const NumericalScalar lambdaX = lambda_ * x;
  const NumericalScalar factor = std::exp(k_ * std::log(lambdaX) - SpecFunc::LnGamma(k_) - lambdaX);
  const NumericalScalar eps = std::pow(cdfEpsilon_, 1.0 / 3.0);
  cdfGradient[0] = (DistFunc::pGamma(k_ + eps, lambda_ * x) - DistFunc::pGamma(k_ - eps, lambda_ * x)) / (2.0 * eps);
  cdfGradient[1] = factor / lambda_;
  cdfGradient[2] = -factor / x;
  return cdfGradient;
}

/* Get the quantile of the distribution */
NumericalScalar Gamma::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  return gamma_ + DistFunc::qGamma(k_, prob, tail) / lambda_;
}

/* Compute the mean of the distribution */
void Gamma::computeMean() const
{
  mean_ = NumericalPoint(1, gamma_ + k_ / lambda_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
NumericalPoint Gamma::getStandardDeviation() const
{
  return NumericalPoint(1, std::sqrt(k_) / lambda_);
}

/* Get the skewness of the distribution */
NumericalPoint Gamma::getSkewness() const
{
  return NumericalPoint(1, 2.0 / std::sqrt(k_));
}

/* Get the kurtosis of the distribution */
NumericalPoint Gamma::getKurtosis() const
{
  return NumericalPoint(1, 3.0 * (k_ + 2.0) / k_);
}

/* Get the moments of the standardized distribution */
NumericalPoint Gamma::getStandardMoment(const UnsignedInteger n) const
{
  return NumericalPoint(1, std::exp(SpecFunc::LogGamma(n + k_) - SpecFunc::LogGamma(k_)));
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Gamma::Implementation Gamma::getStandardRepresentative() const
{
  return Gamma(k_, 1.0, 0.0).clone();
}

/* Compute the covariance of the distribution */
void Gamma::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = k_ / (lambda_ * lambda_);
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value accessor */
NumericalPoint Gamma::getParameter() const
{
  NumericalPoint point(3);
  point[0] = k_;
  point[1] = lambda_;
  point[2] = gamma_;
  return point;
}

void Gamma::setParameter(const NumericalPoint & parameter)
{
  if (parameter.getSize() != 3) throw InvalidArgumentException(HERE) << "Error: expected 3 values, got " << parameter.getSize();
  const NumericalScalar w = getWeight();
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
