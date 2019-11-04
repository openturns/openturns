//                                               -*- C++ -*-
/**
 *  @brief The GeneralizedExtremeValue distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/GeneralizedExtremeValue.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GeneralizedExtremeValue)

static const Factory<GeneralizedExtremeValue> Factory_GeneralizedExtremeValue;

/* Default constructor */
GeneralizedExtremeValue::GeneralizedExtremeValue()
  : ContinuousDistribution()
  , actualDistribution_(Gumbel(1.0, 0.0))
  , mu_(0.0)
  , sigma_(1.0)
  , xi_(0.0)
{
  setName("GeneralizedExtremeValue");
  setDimension(1);
}

/* Parameters constructor to use when the two bounds are finite */
GeneralizedExtremeValue::GeneralizedExtremeValue(const Scalar mu,
    const Scalar sigma,
    const Scalar xi)
  : ContinuousDistribution()
{
  setName("GeneralizedExtremeValue");
  setMuSigmaXi(mu, sigma, xi);
  setDimension(1);
  computeRange();
}

/* Parameters constructor to use when the two bounds are finite */
GeneralizedExtremeValue::GeneralizedExtremeValue(const Distribution & distribution)
  : ContinuousDistribution()
{
  setName("GeneralizedExtremeValue");
  setActualDistribution(distribution);
  setDimension(1);
  computeRange();
}

/* Comparison operator */
Bool GeneralizedExtremeValue::operator ==(const GeneralizedExtremeValue & other) const
{
  if (this == &other) return true;
  return (mu_ == other.getMu()) && (sigma_ == other.getSigma()) && (xi_ == other.getXi());
}

Bool GeneralizedExtremeValue::equals(const DistributionImplementation & other) const
{
  const GeneralizedExtremeValue* p_other = dynamic_cast<const GeneralizedExtremeValue*>(&other);
  if (p_other) return (*this == *p_other);
  return *actualDistribution_.getImplementation() == other;
}

/* String converter */
String GeneralizedExtremeValue::__repr__() const
{
  OSS oss;
  oss << "class=" << GeneralizedExtremeValue::GetClassName()
      << " name=" << getName()
      << " mu=" << mu_
      << " sigma=" << sigma_
      << " xi=" << xi_
      << " actual distribution=" << actualDistribution_;
  return oss;
}

String GeneralizedExtremeValue::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(mu=" << mu_
      << ", sigma=" << sigma_
      << ", xi=" << xi_
      << ")";
  return oss;
}

/* Virtual constructor */
GeneralizedExtremeValue * GeneralizedExtremeValue::clone() const
{
  return new GeneralizedExtremeValue(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void GeneralizedExtremeValue::computeRange()
{
  setRange(actualDistribution_.getRange());
}

/* Get one realization of the distribution */
Point GeneralizedExtremeValue::getRealization() const
{
  return actualDistribution_.getRealization();
}

/* Get the DDF of the distribution */
Point GeneralizedExtremeValue::computeDDF(const Point & point) const
{
  return actualDistribution_.computeDDF(point);
}


/* Get the PDF of the distribution */
Scalar GeneralizedExtremeValue::computePDF(const Point & point) const
{
  return actualDistribution_.computePDF(point);
}


/* Get the log-PDF of the distribution */
Scalar GeneralizedExtremeValue::computeLogPDF(const Point & point) const
{
  return actualDistribution_.computeLogPDF(point);
}


/* Get the CDF of the distribution */
Scalar GeneralizedExtremeValue::computeCDF(const Point & point) const
{
  return actualDistribution_.computeCDF(point);
}

Scalar GeneralizedExtremeValue::computeComplementaryCDF(const Point & point) const
{
  return actualDistribution_.computeComplementaryCDF(point);
}

/* Compute the entropy of the distribution */
Scalar GeneralizedExtremeValue::computeEntropy() const
{
  return std::log(sigma_) + SpecFunc::EulerConstant * (1.0 + xi_) + 1.0;
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex GeneralizedExtremeValue::computeCharacteristicFunction(const Scalar x) const
{
  return actualDistribution_.computeCharacteristicFunction(x);
}

Complex GeneralizedExtremeValue::computeLogCharacteristicFunction(const Scalar x) const
{
  return actualDistribution_.computeLogCharacteristicFunction(x);
}

/* Get the PDFGradient of the distribution */
Point GeneralizedExtremeValue::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  Point result(3);
  // From Maple...
  if (xi_ == 0.0)
  {
    const Scalar t2 = 1.0 / sigma_;
    const Scalar t4 = std::exp(t2 * (mu_ + x));
    const Scalar t7 = std::exp(2.0 * mu_ * t2);
    const Scalar t11 = std::exp(t2 * (-x + mu_));
    const Scalar t16 = std::exp(-t2 * (t11 * sigma_ + 2.0 * x));
    const Scalar t18 = sigma_ * sigma_;
    const Scalar t21 = mu_ * t4;
    const Scalar t23 = x * t4;
    const Scalar t24 = mu_ * t7;
    const Scalar t29 = 1.0 / (t18 * sigma_);
    const Scalar t31 = mu_ * mu_;
    const Scalar t39 = x * x;
    result[0] = t16 * (t4 - t7) / t18;
    result[1] = -t29 * t16 * (t4 * sigma_ + t7 * x + t21 - t23 - t24);
    result[2] = 0.5 * t29 * t16 * (-2.0 * t21 * x + 2.0 * t21 * sigma_ - 2.0 * t23 * sigma_ + 2.0 * t24 * x + t31 * t4 - t31 * t7 + t39 * t4 - t39 * t7);
  } // xi_ == 0.0
  // From Maple...
  else
  {
    const Scalar t1 = 1 / xi_;
    const Scalar t2 = std::pow(sigma_, t1);
    const Scalar t3 = x - mu_;
    const Scalar t4 = xi_ * t3;
    const Scalar t5 = t4 + sigma_;
    const Scalar t6 = std::pow(t5, -t1);
    const Scalar t8 = std::exp(-t6 * t2);
    const Scalar t9 = 1.0 + xi_;
    const Scalar t14 = std::pow(t5, (t1 * (-1.0 - 2.0 * xi_)));
    const Scalar t17 = std::pow(sigma_, 2.0 * t1);
    const Scalar t19 = std::pow(t5, -2.0 * t1 * t9);
    const Scalar t25 = std::pow(sigma_, t1 * (2.0 - xi_));
    const Scalar t29 = std::pow(t5, -t1 * t9);
    const Scalar t35 = std::pow(sigma_, t1 * (1.0 - xi_));
    const Scalar t40 = std::pow(sigma_, t1 * t9);
    const Scalar t42 = t2 * t4 + t40;
    const Scalar t43 = std::log(t5);
    const Scalar t45 = std::log(sigma_);
    const Scalar t49 = t3 * (t45 + 1.0);
    const Scalar t52 = t5 * t5;
    const Scalar t55 = 2.0 + xi_;
    const Scalar t57 = std::pow(sigma_, t1 * t55);
    const Scalar t67 = std::pow(t5, -t1 * t55);
    const Scalar t69 = xi_ * xi_;
    result[0] = (t14 * t2 * t9 - t17 * t19) * t8;
    result[1] = t8 * (-t19 * t3 * t25 + t35 * (t14 * t3 * t9 - t29));
    result[2] = t8 * (t29 * t52 * (-t2 * t49 * xi_ - t40 * t45 + t42 * t43) - t67 * (t43 * (t17 * t4 + t57) - t57 * t45 - t49 * t17 * xi_) * t52 - t3 * t42 * t69 * t6) / (t69 * t52 * t5);
  } // xi_ != 0.0
  return result;
}

/* Get the CDFGradient of the distribution */
Point GeneralizedExtremeValue::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  Point result(3);
  // From Maple...
  if (xi_ == 0.0)
  {
    const Scalar t1 = 1.0 / sigma_;
    const Scalar t2 = -x + mu_;
    const Scalar t4 = std::exp(t2 * t1);
    const Scalar t8 = std::exp(-t1 * (t4 * sigma_ - mu_ + x));
    const Scalar t11 = sigma_ * sigma_;
    const Scalar t12 = 1.0 / t11;
    const Scalar t14 = mu_ * mu_;
    const Scalar t17 = x * x;
    result[0] = -t8 * t1;
    result[1] = t12 * t2 * t8;
    result[2] = -0.5 * t12 * (-2.0 * mu_ * x + t14 + t17) * t8;
  } // xi_ == 0.0
  // From Maple...
  else
  {
    const Scalar t1 = 1.0 / xi_;
    const Scalar t2 = std::pow(sigma_, t1);
    const Scalar t3 = x - mu_;
    const Scalar t4 = t3 * xi_;
    const Scalar t5 = t4 + sigma_;
    const Scalar t8 = std::pow(t5, t1 * (-1.0 - xi_));
    const Scalar t10 = std::pow(t5, -t1);
    const Scalar t12 = std::exp(-t10 * t2);
    const Scalar t16 = std::pow(sigma_, t1 * (1.0 - xi_));
    const Scalar t23 = std::log(t5);
    const Scalar t25 = std::log(sigma_);
    const Scalar t29 = xi_ * xi_;
    result[0] = -t12 * t2 * t8;
    result[1] = -t12 * t3 * t16 * t8;
    result[2] = (t23 * (-t3 * xi_ - sigma_) + t25 * t5 + t4) * t2 * t8 * t12 / t29;
  } // xi_ != 0.0
  return result;
}

/* Get the quantile of the distribution */
Scalar GeneralizedExtremeValue::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  return actualDistribution_.computeQuantile(prob, tail)[0];
}

/* Compute the mean of the distribution */
void GeneralizedExtremeValue::computeMean() const
{
  mean_ = actualDistribution_.getMean();
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point GeneralizedExtremeValue::getStandardDeviation() const
{
  return actualDistribution_.getStandardDeviation();
}

/* Get the skewness of the distribution */
Point GeneralizedExtremeValue::getSkewness() const
{
  return actualDistribution_.getSkewness();
}

/* Get the kurtosis of the distribution */
Point GeneralizedExtremeValue::getKurtosis() const
{
  return actualDistribution_.getKurtosis();
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution GeneralizedExtremeValue::getStandardRepresentative() const
{
  return actualDistribution_.getImplementation()->getStandardRepresentative();
}

/* Compute the covariance of the distribution */
void GeneralizedExtremeValue::computeCovariance() const
{
  covariance_ = actualDistribution_.getCovariance();
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value accessor */
Point GeneralizedExtremeValue::getParameter() const
{
  Point point(3);
  point[0] = mu_;
  point[1] = sigma_;
  point[2] = xi_;
  return point;
}

void GeneralizedExtremeValue::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 3) throw InvalidArgumentException(HERE) << "Error: expected 3 values, got " << parameter.getSize();
  setMuSigmaXi(parameter[0], parameter[1], parameter[2]);
}

/* Parameters description accessor */
Description GeneralizedExtremeValue::getParameterDescription() const
{
  Description description(3);
  description[0] = "mu";
  description[1] = "sigma";
  description[2] = "xi";
  return description;
}

/* Mu accessor */
void GeneralizedExtremeValue::setMu(const Scalar mu)
{
  setMuSigmaXi(mu, sigma_, xi_);
}

Scalar GeneralizedExtremeValue::getMu() const
{
  return mu_;
}


/* Sigma accessor */
void GeneralizedExtremeValue::setSigma(const Scalar sigma)
{
  setMuSigmaXi(mu_, sigma, xi_);
}

Scalar GeneralizedExtremeValue::getSigma() const
{
  return sigma_;
}

/* Xi accessor */
void GeneralizedExtremeValue::setXi(const Scalar xi)
{
  setMuSigmaXi(mu_, sigma_, xi);
}

Scalar GeneralizedExtremeValue::getXi() const
{
  return xi_;
}

/* All parameters accessor */
void GeneralizedExtremeValue::setMuSigmaXi(const Scalar mu,
    const Scalar sigma,
    const Scalar xi)
{
  if (!(sigma > 0.0)) throw InvalidArgumentException(HERE) << "Error: expected a positive value for sigma, here sigma=" << sigma;
  mu_ = mu;
  sigma_ = sigma;
  xi_ = xi;
  // Now build the actual Frechet/Gumbel/WeibullMax distribution
  const Scalar xiEpsilon = ResourceMap::GetAsScalar("GeneralizedExtremeValue-XiThreshold");
  // WeibullMax case
  if (xi_ < -xiEpsilon)
  {
    const Scalar beta = -sigma / xi;
    const Scalar alpha = -1.0 / xi;
    const Scalar gamma = mu - sigma / xi;
    actualDistribution_ = WeibullMax(beta, alpha, gamma);
  }
  // Frechet case
  else if (xi_ > xiEpsilon)
  {
    const Scalar beta = sigma / xi;
    const Scalar alpha = 1.0 / xi;
    const Scalar gamma = mu - sigma / xi;
    actualDistribution_ = Frechet(beta, alpha, gamma);
  }
  // Gumbel case
  else
  {
    const Scalar beta = sigma;
    const Scalar gamma = mu;
    actualDistribution_ = Gumbel(beta, gamma);
  }
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
}

/* Actual distribution accessor */
void GeneralizedExtremeValue::setActualDistribution(const Distribution & distribution)
{
  // Try to cast the given distribution into a Gumbel distribution
  try
  {
    const Gumbel* p_gumbel = dynamic_cast<const Gumbel*>(distribution.getImplementation().get());
    // If it worked create the actual distribution
    if (p_gumbel)
    {
      mu_ = p_gumbel->getGamma();
      sigma_ = p_gumbel->getBeta();
      xi_ = 0.0;
      actualDistribution_ = Gumbel(*p_gumbel);
      isAlreadyComputedMean_ = false;
      isAlreadyComputedCovariance_ = false;
      return;
    } // p_gumbel
  }
  catch (...)
  {
    // Nothing to do
  }
  // Try to cast the given distribution into a Frechet distribution
  try
  {
    const Frechet* p_frechet = dynamic_cast<const Frechet*>(distribution.getImplementation().get());
    // If it worked create the actual distribution
    if (p_frechet)
    {
      xi_ = 1.0 / p_frechet->getAlpha();
      sigma_ = p_frechet->getBeta() * xi_;
      mu_ = p_frechet->getGamma() + p_frechet->getBeta();
      actualDistribution_ = Frechet(*p_frechet);
      isAlreadyComputedMean_ = false;
      isAlreadyComputedCovariance_ = false;
      return;
    } // p_frechet
  }
  catch (...)
  {
    // Nothing to do
  }
  // Try to cast the given distribution into a WeibullMax
  try
  {
    const WeibullMax* p_weibull = dynamic_cast<const WeibullMax*>(distribution.getImplementation().get());
    if (p_weibull)
    {
      xi_ = -1.0 / p_weibull->getAlpha();
      sigma_ = -p_weibull->getBeta() * xi_;
      mu_ = p_weibull->getGamma() - p_weibull->getBeta();
      actualDistribution_ = WeibullMax(*p_weibull);
      isAlreadyComputedMean_ = false;
      isAlreadyComputedCovariance_ = false;
      return;
    } // p_weibull
  }
  catch (...)
  {
    // Nothing to do
  }
  throw InvalidArgumentException(HERE) << "Error: the distribution " << distribution << " cannot be used to define a GeneralizedExtremeValue distribution.";
}

Distribution GeneralizedExtremeValue::getActualDistribution() const
{
  return actualDistribution_;
}

/* Actual distribution converter */
Frechet GeneralizedExtremeValue::asFrechet() const
{
  const Frechet* p_frechet = dynamic_cast<const Frechet*>(actualDistribution_.getImplementation().get());
  if (p_frechet)
    return Frechet(*p_frechet);
  else
    throw InvalidArgumentException(HERE) << "Cannot convert to Frechet";
}

WeibullMax GeneralizedExtremeValue::asWeibullMax() const
{
  const WeibullMax* p_weibull = dynamic_cast<const WeibullMax*>(actualDistribution_.getImplementation().get());
  if (p_weibull)
    return WeibullMax(*p_weibull);
  else
    throw InvalidArgumentException(HERE) << "Cannot convert to WeibullMax";
}

Gumbel GeneralizedExtremeValue::asGumbel() const
{
  const Gumbel* p_gumbel = dynamic_cast<const Gumbel*>(actualDistribution_.getImplementation().get());
  if (p_gumbel)
    return Gumbel(*p_gumbel);
  else
    throw InvalidArgumentException(HERE) << "Cannot convert to Gumbel";
}

/* Method save() stores the object through the StorageManager */
void GeneralizedExtremeValue::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "mu_", mu_ );
  adv.saveAttribute( "sigma_", sigma_ );
  adv.saveAttribute( "xi_", xi_ );
  adv.saveAttribute( "actualDistribution_", actualDistribution_ );
}

/* Method load() reloads the object from the StorageManager */
void GeneralizedExtremeValue::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "mu_", mu_ );
  adv.loadAttribute( "sigma_", sigma_ );
  adv.loadAttribute( "xi_", xi_ );
  adv.loadAttribute( "actualDistribution_", actualDistribution_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
