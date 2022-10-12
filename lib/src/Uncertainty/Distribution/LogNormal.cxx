//                                               -*- C++ -*-
/**
 *  @brief The LogNormal distribution
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/LogNormal.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Normal.hxx"
#include "openturns/HermiteFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LogNormal)

static const Factory<LogNormal> Factory_LogNormal;

/* Default constructor */
LogNormal::LogNormal()
  : ContinuousDistribution()
  , muLog_(0.0)
  , sigmaLog_(0.0)
  , gamma_(0.0)
  , normalizationFactor_(SpecFunc::ISQRT2PI) // 1 / SQRT(2Pi)
  , H_(0)
  , hermiteNodes_(0)
  , hermiteWeights_(0)
{
  setName("LogNormal");
  setDimension(1);
  // The arguments must be different from the initialization values, which is the case as sigmaLog_ is initialized by 0
  setMuLogSigmaLog(0.0, 1.0);
  computeRange();
}


/* Default constructor */
LogNormal::LogNormal(const Scalar muLog,
                     const Scalar sigmaLog,
                     const Scalar gamma)
  : ContinuousDistribution()
  , muLog_(0.0)
  , sigmaLog_(0.0)
  , gamma_(gamma)
  , normalizationFactor_(0.0)
  , H_(0.0)
  , hermiteNodes_(0)
  , hermiteWeights_(0)
{
  setName("LogNormal");
  setMuLogSigmaLog(muLog, sigmaLog);
  normalizationFactor_ = 1.0 / (sigmaLog_ * SpecFunc::SQRT2PI);
  setDimension(1);
}


/* Comparison operator */
Bool LogNormal::operator ==(const LogNormal & other) const
{
  if (this == &other) return true;
  return (muLog_ == other.muLog_) && (sigmaLog_ == other.sigmaLog_) && (gamma_ == other.gamma_);
}

Bool LogNormal::equals(const DistributionImplementation & other) const
{
  const LogNormal* p_other = dynamic_cast<const LogNormal*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String LogNormal::__repr__() const
{
  OSS oss(true);
  oss << "class=" << LogNormal::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " muLog=" << muLog_
      << " sigmaLog=" << sigmaLog_
      << " gamma=" << gamma_;
  return oss;
}

String LogNormal::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(muLog = " << muLog_ << ", sigmaLog = " << sigmaLog_ << ", gamma = " << gamma_ << ")";
  return oss;
}

/* Virtual constructor */
LogNormal * LogNormal::clone() const
{
  return new LogNormal(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void LogNormal::computeRange()
{
  Point lowerBound(1, gamma_);
  const Point upperBound(computeUpperBound());
  const Interval::BoolCollection finiteLowerBound(1, true);
  const Interval::BoolCollection finiteUpperBound(1, false);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Get one realization of the distribution */
Point LogNormal::getRealization() const
{
  return Point(1, gamma_ + std::exp(muLog_ + sigmaLog_ * DistFunc::rNormal()));
}

/* Get the DDF of the distribution */
Point LogNormal::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  // Here we keep the bound within the special case as the distribution is continuous
  if (x <= 0.0) return Point(1, 0.0);
  Scalar v = sigmaLog_ * sigmaLog_;
  return Point(1, (muLog_ - std::log(x) - v) / (v * x) * computePDF(point));
}


/* Get the PDF of the distribution */
Scalar LogNormal::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  // Here we keep the bound within the special case as the distribution is continuous
  if (x <= 0.0) return 0.0;
  Scalar logX = (std::log(x) - muLog_) / sigmaLog_;
  return normalizationFactor_ * std::exp(-0.5 * logX * logX) / x;
}

Scalar LogNormal::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  // Here we keep the bound within the special case as the distribution is continuous
  if (x <= 0.0) return SpecFunc::LowestScalar;
  Scalar logX = (std::log(x) - muLog_) / sigmaLog_;
  return std::log(normalizationFactor_) - 0.5 * logX * logX - std::log(x);
}

/* Get the CDF of the distribution */
Scalar LogNormal::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  // Here we keep the bound within the special case as the distribution is continuous
  if (x <= 0.0) return 0.0;
  Scalar logX = (std::log(x) - muLog_) / sigmaLog_;
  return DistFunc::pNormal(logX);
}

Scalar LogNormal::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  // Here we keep the bound within the special case as the distribution is continuous
  if (x <= 0.0) return 1.0;
  Scalar logX = (std::log(x) - muLog_) / sigmaLog_;
  return DistFunc::pNormal(logX, true);
}


/* Compute the entropy of the distribution */
Scalar LogNormal::computeEntropy() const
{
  return 0.5 + muLog_ - std::log(normalizationFactor_);
}

/* Compute the integrand that is involved in the computation of the characteristic function */
Complex LogNormal::characteristicIntegrand(const Scalar eta,
    const Scalar sStar) const
{
  return std::exp(Complex(-sStar * (eta - std::exp(sigmaLog_ * eta) / sigmaLog_), -M_PI * eta / (2.0 * sigmaLog_)));
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X))
 * Algorithm adapted from:
 * John A. Gubner, "A New Formula for Lognormal Characteristic Functions",
 * IEEE transactions on vehicular technology, vol. 55, no. 5, September 2006.
 */
Complex LogNormal::computeCharacteristicFunction(const Scalar x) const
{
  // Quick return for null argument
  if (x == 0.0) return 1.0;
  return std::exp(computeLogCharacteristicFunction(x));
}

Complex LogNormal::computeLogCharacteristicFunction(const Scalar x) const
{
  // Quick return for null argument
  if (x == 0.0) return 0.0;
  // Compute the characteristic function for the positive arguments
  const Scalar nu = std::abs(x) * std::exp(muLog_);
  const Scalar sigma2 = sigmaLog_ * sigmaLog_;
  Complex logCFValue(0.0);
  // Quick return for small argument
  if (nu < 0.001 * std::exp(-1.5 * sigma2))
  {
    const Scalar nu2 = nu * nu;
    logCFValue = std::log(Complex(1.0 - 0.5 * nu2 * std::exp(2.0 * sigma2), nu * std::exp(0.5 * sigma2) * (1.0 - nu2 * std::exp(4.0 * sigma2) / 6.0)));
  }
  else
  {
    UnsignedInteger integrationNodesNumber = ResourceMap::GetAsUnsignedInteger("LogNormal-CharacteristicFunctionIntegrationNodes");
    // Check if the integration nodes have to be recomputed
    if (integrationNodesNumber != hermiteNodes_.getSize()) hermiteNodes_ = HermiteFactory().getNodesAndWeights(integrationNodesNumber, hermiteWeights_);
    // Hermite integration of the Fourier transform for small sigma
    if (sigmaLog_ <= ResourceMap::GetAsScalar("LogNormal-CharacteristicFunctionSmallSigmaThreshold"))
    {
      // Adjust the integration nodes number
      // nu*exp(sigma*x_n) - nu * exp(sigma*x_{n-1}) < 2*\pi/8
      // nu*exp(sigma*x_n)(1 - exp(sigma*(x_{n-1}-x_n))) < 2*\pi/8
      // 1 - exp(sigma*(x_{n-1}-x_n)) < 2*\pi/(8*nu*exp(sigma*x_n))
      // sigma << 1
      // sigma(x_n-x_{n-1}) < 2*\pi/(8*nu)
      // const Scalar maxPulsation(exp(sigmaLog_ * nodes[integrationNodesNumber - 1]));
      // const UnsignedInteger minimumIntegrationNodesNumber(static_cast<UnsignedInteger>(8 * 2 * M_PI * nu));
      // integrationNodesNumber = std::max(integrationNodesNumber, static_cast<UnsignedInteger>(8 * 2 * M_PI * nu));
      Complex value(0.0);
      for (UnsignedInteger i = 0; i < integrationNodesNumber; ++i) value += hermiteWeights_[i] * std::exp(Complex(0.0, nu * std::exp(sigmaLog_ * hermiteNodes_[i])));
      logCFValue = std::log(value);
    } // Small sigma
    else
    {
      // Hermite integration centered on the maximal amplitude
      // Compute the characteristic function for the positive arguments
      const Scalar sStar = -SpecFunc::LambertW(sigmaLog_ * sigmaLog_ * nu) / sigmaLog_;
      Complex value(0.0);
      for (UnsignedInteger i = 0; i < integrationNodesNumber; ++i) value += hermiteWeights_[i] * characteristicIntegrand(hermiteNodes_[i], sStar);
      logCFValue = std::log(value) + H_ - Complex(sStar * sStar / 2.0, M_PI * sStar / (2.0 * sigmaLog_));
    } // Large sigma
  } // Large argument
  logCFValue += Complex(0.0, x * gamma_);
  // Use symmetry for negative arguments
  if (x < 0.0) logCFValue = conj(logCFValue);
  return logCFValue;
}

/* Get the PDFGradient of the distribution */
Point LogNormal::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  Point pdfGradient(3, 0.0);
  // Here we keep the bound within the special case as the distribution is continuous
  if (x <= 0.0) return pdfGradient;
  Scalar logX = (std::log(x) - muLog_) / sigmaLog_;
  Scalar pdf = normalizationFactor_ * std::exp(-0.5 * logX * logX) / x;
  pdfGradient[0] = pdf * logX / sigmaLog_;
  pdfGradient[1] = pdf * (logX - 1.0) * (logX + 1.0) / sigmaLog_;
  pdfGradient[2] = pdf * (1.0 + logX / sigmaLog_) / x;
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
Point LogNormal::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  Point cdfGradient(3, 0.0);
  // Here we keep the bound within the special case as the distribution is continuous
  if (x <= 0.0) return cdfGradient;
  Scalar logX = (std::log(x) - muLog_) / sigmaLog_;
  Scalar pdf = normalizationFactor_ * std::exp(-0.5 * logX * logX) / x;
  cdfGradient[0] = -x * pdf;
  cdfGradient[1] = -logX * x * pdf;
  cdfGradient[2] = -pdf;
  return cdfGradient;
}

/* Get the quantile of the distribution */
Scalar LogNormal::computeScalarQuantile(const Scalar prob,
                                        const Bool tail) const
{
  return gamma_ + std::exp(muLog_ + sigmaLog_ * DistFunc::qNormal(prob, tail));
}

/* Compute the mean of the distribution */
void LogNormal::computeMean() const
{
  mean_ = Point(1, gamma_ + std::exp(muLog_ + 0.5 * sigmaLog_ * sigmaLog_));
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point LogNormal::getStandardDeviation() const
{
  Scalar expSigmaLog2 = std::exp(sigmaLog_ * sigmaLog_);
  return Point(1, std::exp(muLog_) * std::sqrt(expSigmaLog2 * (expSigmaLog2 - 1.0)));
}

/* Get the skewness of the distribution */
Point LogNormal::getSkewness() const
{
  Scalar expSigmaLog2 = std::exp(sigmaLog_ * sigmaLog_);
  return Point(1, (expSigmaLog2 + 2.0) * std::sqrt(expSigmaLog2 - 1.0));
}

/* Get the kurtosis of the distribution */
Point LogNormal::getKurtosis() const
{
  Scalar expSigmaLog2 = std::exp(sigmaLog_ * sigmaLog_);
  return Point(1, -3.0 + expSigmaLog2 * expSigmaLog2 * (3.0 + expSigmaLog2 * (2.0 + expSigmaLog2)));
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution LogNormal::getStandardRepresentative() const
{
  return new LogNormal(muLog_, sigmaLog_, 0.0);
}

/* Compute the covariance of the distribution */
void LogNormal::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  const Scalar expSigmaLog2 = std::exp(sigmaLog_ * sigmaLog_);
  covariance_(0, 0) = expSigmaLog2 * std::exp(2.0 * muLog_) * (expSigmaLog2 - 1.0);
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value accessor */
Point LogNormal::getParameter() const
{
  Point point(3);
  point[0] = muLog_;
  point[1] = sigmaLog_;
  point[2] = gamma_;
  return point;
}

void LogNormal::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 3) throw InvalidArgumentException(HERE) << "Error: expected 3 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = LogNormal(parameter[0], parameter[1], parameter[2]);
  setWeight(w);
}

/* Parameters description accessor */
Description LogNormal::getParameterDescription() const
{
  Description description(3);
  description[0] = "muLog";
  description[1] = "sigmaLog";
  description[2] = "gamma";
  return description;
}

/* Interface specific to LogNormal */

/* MuLogSigmaLog accessor */
void LogNormal::setMuLogSigmaLog(const Scalar muLog,
                                 const Scalar sigmaLog)
{
  if (!(sigmaLog > 0.0)) throw InvalidArgumentException(HERE) << "SigmaLog MUST be positive, here sigmaLog=" << sigmaLog;
  if ((muLog != muLog_) || (sigmaLog != sigmaLog_))
  {
    muLog_ = muLog;
    sigmaLog_ = sigmaLog;
    // Check if the parameters values are not crazy
    static const Interval range(Normal().getRange());
    const Scalar rMin = muLog_ + range.getUpperBound()[0] * sigmaLog_;
    const Scalar rMax = muLog_ + range.getLowerBound()[0] * sigmaLog_;
    if ((rMin >= SpecFunc::LogMaxScalar) ||
        (rMax <= SpecFunc::LogMinScalar)) throw InvalidArgumentException(HERE) << "MuLog=" << muLog_ << " and SigmaLog=" << sigmaLog_ << " lead to a LogNormal distribution with a too much wide range";
    H_ = M_PI * M_PI / (8.0 * sigmaLog_ * sigmaLog_);
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* MuLog accessor */
void LogNormal::setMuLog(const Scalar muLog)
{
  if (muLog != muLog_)
  {
    muLog_ = muLog;
    // Check if the parameters values are not crazy
    static const Interval range(Normal().getRange());
    const Scalar rMin = muLog_ + range.getUpperBound()[0] * sigmaLog_;
    const Scalar rMax = muLog_ + range.getLowerBound()[0] * sigmaLog_;
    if ((rMin >= SpecFunc::LogMaxScalar) ||
        (rMax <= SpecFunc::LogMinScalar)) throw InvalidArgumentException(HERE) << "MuLog and SigmaLog lead to a LogNormal distribution with a too much wide range";
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

Scalar LogNormal::getMuLog() const
{
  return muLog_;
}


/* SigmaLog accessor */
void LogNormal::setSigmaLog(const Scalar sigmaLog)
{
  if (!(sigmaLog > 0.0)) throw InvalidArgumentException(HERE) << "SigmaLog MUST be positive, here sigmaLog=" << sigmaLog;
  if (sigmaLog != sigmaLog_)
  {
    sigmaLog_ = sigmaLog;
    // Check if the parameters values are not crazy
    static const Interval range(Normal().getRange());
    const Scalar rMin = muLog_ + range.getUpperBound()[0] * sigmaLog_;
    const Scalar rMax = muLog_ + range.getLowerBound()[0] * sigmaLog_;
    if ((rMin >= SpecFunc::LogMaxScalar) ||
        (rMax <= SpecFunc::LogMinScalar)) throw InvalidArgumentException(HERE) << "MuLog and SigmaLog lead to a LogNormal distribution with a too much wide range";
    H_ = std::pow(M_PI / (2.0 * sigmaLog_), 2.0) / 2.0 - 0.5 * std::log(2.0 * M_PI);
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

Scalar LogNormal::getSigmaLog() const
{
  return sigmaLog_;
}

/* Gamma accessor */
void LogNormal::setGamma(const Scalar gamma)
{
  if (gamma != gamma_)
  {
    gamma_ = gamma;
    isAlreadyComputedMean_ = false;
    // The covariance does not depends on gamma
    computeRange();
  }
}

Scalar LogNormal::getGamma() const
{
  return gamma_;
}

/* Method save() stores the object through the StorageManager */
void LogNormal::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "muLog_", muLog_ );
  adv.saveAttribute( "sigmaLog_", sigmaLog_ );
  adv.saveAttribute( "gamma_", gamma_ );
  adv.saveAttribute( "normalizationFactor_", normalizationFactor_ );
  adv.saveAttribute( "H_", H_ );
}

/* Method load() reloads the object from the StorageManager */
void LogNormal::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "muLog_", muLog_ );
  adv.loadAttribute( "sigmaLog_", sigmaLog_ );
  adv.loadAttribute( "gamma_", gamma_ );
  adv.loadAttribute( "normalizationFactor_", normalizationFactor_ );
  adv.loadAttribute( "H_", H_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
