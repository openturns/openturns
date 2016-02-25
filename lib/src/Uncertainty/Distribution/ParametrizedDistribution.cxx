//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for Parametrized distributions
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

#include "openturns/ParametrizedDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ParametrizedDistribution);

static const Factory<ParametrizedDistribution> RegisteredFactory;


/* Default constructor */
ParametrizedDistribution::ParametrizedDistribution()
  : DistributionImplementation()
{
  // Nothing to do
}


/* Parameter constructor */
ParametrizedDistribution::ParametrizedDistribution(const DistributionParameters & distParam)
  : DistributionImplementation()
  , distributionParameters_(distParam)
  , distribution_(distParam.getDistribution())
{
  // Nothing to do
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

/* Get the DDF of the distribution */
NumericalPoint ParametrizedDistribution::computeDDF(const NumericalPoint & point) const
{
  return distribution_.computeDDF(point);
}


/* Get the PDF of the distribution */
NumericalScalar ParametrizedDistribution::computePDF(const NumericalPoint & point) const
{
  return distribution_.computePDF(point);
}


NumericalScalar ParametrizedDistribution::computeLogPDF(const NumericalPoint & point) const
{
  return distribution_.computeLogPDF(point);
}


/* Get the CDF of the distribution */
NumericalScalar ParametrizedDistribution::computeCDF(const NumericalPoint & point) const
{
  return distribution_.computeCDF(point);
}


NumericalScalar ParametrizedDistribution::computeComplementaryCDF(const NumericalPoint & point) const
{
  return distribution_.computeComplementaryCDF(point);
}


NumericalComplex ParametrizedDistribution::computeCharacteristicFunction(const NumericalScalar x) const
{
  return distribution_.computeCharacteristicFunction(x);
}


NumericalComplex ParametrizedDistribution::computeLogCharacteristicFunction(const NumericalScalar x) const
{
  return distribution_.computeLogCharacteristicFunction(x);
}


/* Generic implementation of the quantile computation */
NumericalPoint ParametrizedDistribution::computeQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  return distribution_.computeQuantile(prob, tail);
}

/* Get the PDFGradient of the distribution */
NumericalPoint ParametrizedDistribution::computePDFGradient(const NumericalPoint & point) const
{
  Matrix gradient(distributionParameters_.gradient());
  return gradient * distribution_.computePDFGradient(point);
}


/* Get the CDFGradient of the distribution */
NumericalPoint ParametrizedDistribution::computeCDFGradient(const NumericalPoint & point) const
{
  Matrix gradient(distributionParameters_.gradient());
  return gradient * distribution_.computeCDFGradient(point);
}


NumericalPoint ParametrizedDistribution::getParameter() const
{
  return distributionParameters_.inverse(distribution_.getParameter());
}

void ParametrizedDistribution::setParameter(const NumericalPoint & parameter)
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
NumericalScalar ParametrizedDistribution::getRoughness() const
{
  return distribution_.getRoughness();
}

/* Get the mean of the distribution */
NumericalPoint ParametrizedDistribution::getMean() const
{
  return distribution_.getMean();
}

/* Get the standard deviation of the distribution */
NumericalPoint ParametrizedDistribution::getStandardDeviation() const
{
  return distribution_.getStandardDeviation();
}

/* Get the skewness of the distribution */
NumericalPoint ParametrizedDistribution::getSkewness() const
{
  return distribution_.getSkewness();
}

/* Get the kurtosis of the distribution */
NumericalPoint ParametrizedDistribution::getKurtosis() const
{
  return distribution_.getKurtosis();
}

/* Get the raw moments of the standardized distribution */
NumericalPoint ParametrizedDistribution::getStandardMoment(const UnsignedInteger n) const
{
  return distribution_.getStandardMoment(n);
}

/* Get the standard representative in the parametric family, associated with the standard moments */
ParametrizedDistribution::Implementation ParametrizedDistribution::getStandardRepresentative() const
{
  return distribution_.getStandardRepresentative().getImplementation()->clone();
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