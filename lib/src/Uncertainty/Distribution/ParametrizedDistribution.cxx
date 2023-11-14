//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for Parametrized distributions
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

#include "openturns/ParametrizedDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ParametrizedDistribution)

static const Factory<ParametrizedDistribution> Factory_ParametrizedDistribution;


/* Default constructor */
ParametrizedDistribution::ParametrizedDistribution()
  : DistributionImplementation()
{
  // Update the range
  computeRange();
}


/* Parameter constructor */
ParametrizedDistribution::ParametrizedDistribution(const DistributionParameters & distParam)
  : DistributionImplementation()
  , distributionParameters_(distParam)
  , distribution_(distParam.getDistribution())
{
  // Update the range
  computeRange();
}


/* Virtual constructor */
ParametrizedDistribution * ParametrizedDistribution::clone() const
{
  return new ParametrizedDistribution(*this);
}


/* Comparison operator */
Bool ParametrizedDistribution::operator ==(const ParametrizedDistribution & other) const
{
  return (this == &other);
}

Bool ParametrizedDistribution::equals(const DistributionImplementation & other) const
{
  const ParametrizedDistribution* p_other = dynamic_cast<const ParametrizedDistribution*>(&other);
  return p_other && (*this == *p_other);
}

/* Get the DDF of the distribution */
Point ParametrizedDistribution::computeDDF(const Point & point) const
{
  return distribution_.computeDDF(point);
}


/* Get the PDF of the distribution */
Scalar ParametrizedDistribution::computePDF(const Point & point) const
{
  return distribution_.computePDF(point);
}


Scalar ParametrizedDistribution::computeLogPDF(const Point & point) const
{
  return distribution_.computeLogPDF(point);
}


/* Get the CDF of the distribution */
Scalar ParametrizedDistribution::computeCDF(const Point & point) const
{
  return distribution_.computeCDF(point);
}


Scalar ParametrizedDistribution::computeComplementaryCDF(const Point & point) const
{
  return distribution_.computeComplementaryCDF(point);
}


Complex ParametrizedDistribution::computeCharacteristicFunction(const Scalar x) const
{
  return distribution_.computeCharacteristicFunction(x);
}


Complex ParametrizedDistribution::computeLogCharacteristicFunction(const Scalar x) const
{
  return distribution_.computeLogCharacteristicFunction(x);
}


/* Generic implementation of the quantile computation */
Point ParametrizedDistribution::computeQuantile(const Scalar prob,
    const Bool tail) const
{
  return distribution_.computeQuantile(prob, tail);
}

/* Get the product minimum volume interval containing a given probability of the distribution */
Interval ParametrizedDistribution::computeMinimumVolumeIntervalWithMarginalProbability(const Scalar prob, Scalar & marginalProb) const
{
  return distribution_.computeMinimumVolumeIntervalWithMarginalProbability(prob, marginalProb);
}

/* Get the product bilateral confidence interval containing a given probability of the distribution */
Interval ParametrizedDistribution::computeBilateralConfidenceIntervalWithMarginalProbability(const Scalar prob, Scalar & marginalProb) const
{
  return distribution_.computeBilateralConfidenceIntervalWithMarginalProbability(prob, marginalProb);
}

/* Get the product unilateral confidence interval containing a given probability of the distribution */
Interval ParametrizedDistribution::computeUnilateralConfidenceIntervalWithMarginalProbability(const Scalar prob, const Bool tail, Scalar & marginalProb) const
{
  return distribution_.computeUnilateralConfidenceIntervalWithMarginalProbability(prob, tail, marginalProb);
}

/* Get the minimum volume level set containing a given probability of the distribution */
LevelSet ParametrizedDistribution::computeMinimumVolumeLevelSetWithThreshold(const Scalar prob, Scalar & threshold) const
{
  return distribution_.computeMinimumVolumeLevelSetWithThreshold(prob, threshold);
}

/* Get the PDFGradient of the distribution */
Point ParametrizedDistribution::computePDFGradient(const Point & point) const
{
  Matrix gradient(distributionParameters_.gradient());
  return gradient * distribution_.computePDFGradient(point);
}


/* Get the CDFGradient of the distribution */
Point ParametrizedDistribution::computeCDFGradient(const Point & point) const
{
  Matrix gradient(distributionParameters_.gradient());
  return gradient * distribution_.computeCDFGradient(point);
}


Point ParametrizedDistribution::getParameter() const
{
  return distributionParameters_.inverse(distribution_.getParameter());
}

void ParametrizedDistribution::setParameter(const Point & parameter)
{
  distribution_.setParameter(distributionParameters_(parameter));
}

Description ParametrizedDistribution::getParameterDescription() const
{
  return distributionParameters_.getDescription();
}

/* Check if the distribution is elliptical */
Bool ParametrizedDistribution::isElliptical() const
{
  return distribution_.isElliptical();
}


/* Check if the distribution is continuous */
Bool ParametrizedDistribution::isContinuous() const
{
  return distribution_.isContinuous();
}


/* String converter */
String ParametrizedDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << ParametrizedDistribution::GetClassName()
      << " parameters=" << distributionParameters_
      << " distribution=" << distribution_;
  return oss;
}


/* Get the roughness, i.e. the L2-norm of the PDF */
Scalar ParametrizedDistribution::getRoughness() const
{
  return distribution_.getRoughness();
}

/* Get the mean of the distribution */
Point ParametrizedDistribution::getMean() const
{
  return distribution_.getMean();
}

/* Get the standard deviation of the distribution */
Point ParametrizedDistribution::getStandardDeviation() const
{
  return distribution_.getStandardDeviation();
}

/* Get the skewness of the distribution */
Point ParametrizedDistribution::getSkewness() const
{
  return distribution_.getSkewness();
}

/* Get the kurtosis of the distribution */
Point ParametrizedDistribution::getKurtosis() const
{
  return distribution_.getKurtosis();
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution ParametrizedDistribution::getStandardRepresentative() const
{
  return distribution_.getStandardRepresentative().getImplementation()->clone();
}

void ParametrizedDistribution::computeRange()
{
  range_ = distribution_.getRange();
}

/* Method save() stores the object through the StorageManager */
void ParametrizedDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute("distributionParameters_", distributionParameters_);
}


/* Method load() reloads the object from the StorageManager */
void ParametrizedDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute("distributionParameters_", distributionParameters_);
  distribution_ = distributionParameters_.getDistribution();
}


END_NAMESPACE_OPENTURNS
