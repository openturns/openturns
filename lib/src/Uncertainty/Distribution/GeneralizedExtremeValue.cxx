//                                               -*- C++ -*-
/**
 *  @brief The GeneralizedExtremeValue distribution
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
#include "openturns/GeneralizedExtremeValue.hxx"
#include "openturns/Weibull.hxx"
#include "openturns/Frechet.hxx"
#include "openturns/Gumbel.hxx"
#include "openturns/RandomMixture.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GeneralizedExtremeValue);

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
GeneralizedExtremeValue::GeneralizedExtremeValue(const NumericalScalar mu,
						 const NumericalScalar sigma,
						 const NumericalScalar xi)
  : ContinuousDistribution()
{
  setName("GeneralizedExtremeValue");
  setMuSigmaXi(mu, sigma, xi);
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

String GeneralizedExtremeValue::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(mu=" << mu_
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
NumericalScalar GeneralizedExtremeValue::computePDF(const Point & point) const
{
  return actualDistribution_.computePDF(point);
}


/* Get the log-PDF of the distribution */
NumericalScalar GeneralizedExtremeValue::computeLogPDF(const Point & point) const
{
  return actualDistribution_.computeLogPDF(point);
}


/* Get the CDF of the distribution */
NumericalScalar GeneralizedExtremeValue::computeCDF(const Point & point) const
{
  return actualDistribution_.computeCDF(point);
}

NumericalScalar GeneralizedExtremeValue::computeComplementaryCDF(const Point & point) const
{
  return actualDistribution_.computeComplementaryCDF(point);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex GeneralizedExtremeValue::computeCharacteristicFunction(const NumericalScalar x) const
{
  return actualDistribution_.computeCharacteristicFunction(x);
}

NumericalComplex GeneralizedExtremeValue::computeLogCharacteristicFunction(const NumericalScalar x) const
{
  return actualDistribution_.computeLogCharacteristicFunction(x);
}

/* Get the PDFGradient of the distribution */
Point GeneralizedExtremeValue::computePDFGradient(const Point & point) const
{
  return actualDistribution_.computePDFGradient(point);
}

/* Get the CDFGradient of the distribution */
Point GeneralizedExtremeValue::computeCDFGradient(const Point & point) const
{
  return actualDistribution_.computeCDFGradient(point);
}

/* Get the quantile of the distribution */
NumericalScalar GeneralizedExtremeValue::computeScalarQuantile(const NumericalScalar prob,
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
GeneralizedExtremeValue::Implementation GeneralizedExtremeValue::getStandardRepresentative() const
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
void GeneralizedExtremeValue::setMu(const NumericalScalar mu)
{
  setMuSigmaXi(mu, sigma_, xi_);
}

NumericalScalar GeneralizedExtremeValue::getMu() const
{
  return mu_;
}


/* Sigma accessor */
void GeneralizedExtremeValue::setSigma(const NumericalScalar sigma)
{
  setMuSigmaXi(mu_, sigma, xi_);
}

NumericalScalar GeneralizedExtremeValue::getSigma() const
{
  return sigma_;
}

/* Xi accessor */
void GeneralizedExtremeValue::setXi(const NumericalScalar xi)
{
  setMuSigmaXi(mu_, sigma_, xi);
}

NumericalScalar GeneralizedExtremeValue::getXi() const
{
  return xi_;
}

/* All parameters accessor */
void GeneralizedExtremeValue::setMuSigmaXi(const NumericalScalar mu,
					   const NumericalScalar sigma,
					   const NumericalScalar xi)
{
  if (!(sigma > 0.0)) throw InvalidArgumentException(HERE) << "Error: expected a positive value for sigma, here sigma=" << sigma;
  mu_ = mu;
  sigma_ = sigma;
  xi_ = xi;
  // Now build the actual Frechet/Gumbel/Weibull distribution
  const NumericalScalar xiEpsilon = ResourceMap::GetAsNumericalScalar("GeneralizedExtremeValue-XiThreshold");
  // Weibull case
  if (xi_ < -xiEpsilon)
    {
      const NumericalScalar alpha = -sigma / xi;
      const NumericalScalar beta = -1.0 / xi;
      const NumericalScalar gamma = sigma / xi - mu;
      actualDistribution_ = Weibull(alpha, beta, gamma) * (-1.0);
    }
  // Frechet case
  else if (xi_ > xiEpsilon)
    {
      const NumericalScalar alpha = 1.0 / xi;
      const NumericalScalar beta = sigma / xi;
      const NumericalScalar gamma = mu - sigma / xi;
      actualDistribution_ = Frechet(alpha, beta, gamma);
    }
  // Gumbel case
  else
    {
      const NumericalScalar alpha = 1.0 / sigma;
      const NumericalScalar beta = mu;
      actualDistribution_ = Gumbel(alpha, beta);
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
	  mu_ = p_gumbel->getBeta();
	  sigma_ = 1.0 / p_gumbel->getAlpha();
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
  // Try to cast the given distribution into a RandomMixture
  // with a Weibull atom with negative coefficient
  try
    {
      const RandomMixture* p_mixture = dynamic_cast<const RandomMixture*>(distribution.getImplementation().get());
      // If it worked try to catch the atom into a Weibull distribution
      if (p_mixture)
	{
	  // First, the easy checks:
	  // + its diension is 1
	  // + there is only one atom
	  // + its weight is negative
	  if ((p_mixture->getDimension() == 1) &&
	      (p_mixture->getDistributionCollection().getSize() == 1) &&
	      (p_mixture->getWeights()(0, 0) < 0.0))
	    {
	      // Try to catch the unique atom into a Weibull distribution
	      const Weibull* p_weibull = dynamic_cast<const Weibull*>(p_mixture->getDistributionCollection()[0].getImplementation().get());
	      if (p_weibull)
		{
		  const NumericalScalar constant = p_mixture->getConstant()[0];
		  const NumericalScalar weight = p_mixture->getWeights()(0, 0);
		  xi_ = -1.0 / p_weibull->getBeta();
		  sigma_ = -(weight * p_weibull->getAlpha()) * xi_;
		  mu_ = constant + sigma_ / xi_ - p_weibull->getGamma() * weight;
		  actualDistribution_ = RandomMixture(*p_mixture);
		  isAlreadyComputedMean_ = false;
		  isAlreadyComputedCovariance_ = false;
		  return;
		} // p_weibull
	    } // mixture basic check
	} // p_mixture
    }
  catch (...)
    {
      throw InvalidArgumentException(HERE) << "Error: the distribution " << distribution << " cannot be used to define a GeneralizedExtremeValue distribution.";
    }
}

Distribution GeneralizedExtremeValue::getActualDistribution() const
{
  return actualDistribution_;
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

