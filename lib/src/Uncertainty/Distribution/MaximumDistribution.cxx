//                                               -*- C++ -*-
/**
 *  @brief The MaximumDistribution distribution
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
#include <cstdlib>
#include <cmath>

#include "openturns/MaximumDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ComposedDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MaximumDistribution);

static const Factory<MaximumDistribution> RegisteredFactory;

/* Default constructor */
MaximumDistribution::MaximumDistribution()
  : DistributionImplementation()
  , distribution_()
{
  setName("MaximumDistribution");
  setDimension(1);
  // Adjust the truncation interval and the distribution range
  setDistribution(Distribution());
}

/* Parameters constructor */
MaximumDistribution::MaximumDistribution(const Distribution & distribution)
  : DistributionImplementation()
  , distribution_()
{
  setName("MaximumDistribution");
  setDimension(1);
  setDistribution(distribution);
}

/* Parameters constructor */
MaximumDistribution::MaximumDistribution(const DistributionCollection & collection)
  : DistributionImplementation()
  , distribution_()
{
  setName("MaximumDistribution");
  setDimension(1);
  setDistribution(ComposedDistribution(collection));
}

/* Comparison operator */
Bool MaximumDistribution::operator ==(const MaximumDistribution & other) const
{
  if (this == &other) return true;
  return (distribution_ == other.distribution_);
}

/* String converter */
String MaximumDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << MaximumDistribution::GetClassName()
      << " name=" << getName()
      << " distribution=" << distribution_;
  return oss;
}

String MaximumDistribution::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(" << distribution_.__str__() << ")";
  return oss;
}

/* Virtual constructor */
MaximumDistribution * MaximumDistribution::clone() const
{
  return new MaximumDistribution(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void MaximumDistribution::computeRange()
{
  const NumericalPoint lower(distribution_.getRange().getLowerBound());
  const NumericalPoint upper(distribution_.getRange().getUpperBound());
  setRange(Interval(*std::max_element(lower.begin(), lower.end()), *std::max_element(upper.begin(), upper.end())));
}

/* Get one realization of the distribution */
NumericalPoint MaximumDistribution::getRealization() const
{
  const NumericalPoint realization(distribution_.getRealization());
  return NumericalPoint(1, *std::max_element(realization.begin(), realization.end()));
}

/* Get the PDF of the distribution */
NumericalScalar MaximumDistribution::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  if ((point[0] <= getRange().getLowerBound()[0]) || (point[0] >= getRange().getUpperBound()[0])) return 0.0;
  if (!distribution_.hasIndependentCopula()) DistributionImplementation::computePDF(point);
  // Special treatment of the independent copula case
  const UnsignedInteger size(distribution_.getDimension());
  NumericalPoint marginalCDF(size);
  NumericalScalar product(1.0);
  DistributionCollection marginals(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    marginals[i] = distribution_.getMarginal(i);
    const NumericalScalar cdf(marginals[i].computeCDF(point));
    if ((cdf == 0) || (cdf == 1.0)) return 0.0;
    marginalCDF[i] = cdf;
    product *= cdf;
  }
  NumericalScalar sum(0.0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalScalar pdfI(marginals[i].computePDF(point));
    if (pdfI > 0.0) sum += pdfI / marginalCDF[i];
  }
  return sum * product;
}

/* Get the CDF of the distribution */
NumericalScalar MaximumDistribution::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  return distribution_.computeCDF(NumericalPoint(distribution_.getDimension(), point[0]));
}

/* Parameters value and description accessor */
MaximumDistribution::NumericalPointWithDescriptionCollection MaximumDistribution::getParametersCollection() const
{
  return distribution_.getParametersCollection();
}

void MaximumDistribution::setParametersCollection(const NumericalPointCollection & parametersCollection)
{
  distribution_.setParametersCollection(parametersCollection);
}

/* Distribution accessor */
void MaximumDistribution::setDistribution(const Distribution & distribution)
{
  distribution_ = distribution;
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  isAlreadyCreatedGeneratingFunction_ = false;
  isParallel_ = distribution_.getImplementation()->isParallel();
  computeRange();
}

Distribution MaximumDistribution::getDistribution() const
{
  return distribution_;
}

Bool MaximumDistribution::isContinuous() const
{
  return distribution_.isContinuous();
}

/* Tell if the distribution is discrete */
Bool MaximumDistribution::isDiscrete() const
{
  return distribution_.isDiscrete();
}

/* Tell if the distribution is integer valued */
Bool MaximumDistribution::isIntegral() const
{
  return distribution_.isIntegral();
}

/* Method save() stores the object through the StorageManager */
void MaximumDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "distribution_", distribution_ );
}

/* Method load() reloads the object from the StorageManager */
void MaximumDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  Distribution distribution;
  adv.loadAttribute( "distribution_", distribution );
  setDistribution(distribution);
}

END_NAMESPACE_OPENTURNS
