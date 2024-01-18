//                                               -*- C++ -*-
/**
 *  @brief The MaximumDistribution distribution
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
#include <cstdlib>
#include <cmath>

#include "openturns/MaximumDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/JointDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MaximumDistribution)

static const Factory<MaximumDistribution> Factory_MaximumDistribution;

/* Default constructor */
MaximumDistribution::MaximumDistribution()
  : DistributionImplementation()
  , distribution_()
  , allSame_(true)
  , variablesNumber_(1)
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
  , allSame_(false)
  , variablesNumber_(distribution.getDimension())
{
  setName("MaximumDistribution");
  setDimension(1);
  setDistribution(distribution);
}

/* Parameters constructor */
MaximumDistribution::MaximumDistribution(const DistributionCollection & collection)
  : DistributionImplementation()
  , distribution_()
  , allSame_(true)
  , variablesNumber_(collection.getSize())
{
  if (variablesNumber_ == 0) throw InvalidArgumentException(HERE) << "Error: cannot take the maximum of an empty collection of distributions";
  for (UnsignedInteger i = 0; i < variablesNumber_; ++i)
    if (collection[i].getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot take the maximum of a collection of multivariate distributions, here distribution=" << i << " has dimension=" << collection[i].getDimension();
  setName("MaximumDistribution");
  setDimension(1);
  for (UnsignedInteger i = 0; i < variablesNumber_; ++i)
    if (collection[i] != collection[0])
    {
      allSame_ = false;
      break;
    }
  if (allSame_) setDistribution(collection[0]);
  else setDistribution(JointDistribution(collection));
}

/* Parameters constructor */
MaximumDistribution::MaximumDistribution(const Distribution & distribution,
    const UnsignedInteger variablesNumber)
  : DistributionImplementation()
  , distribution_()
  , allSame_(true)
  , variablesNumber_(variablesNumber)
{
  if (variablesNumber_ == 0) throw InvalidArgumentException(HERE) << "Error: cannot take the maximum of an empty collection of distributions";
  if (distribution.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot take the maximum of a collection of multivariate distributions, here distribution=0 has dimension=" << distribution.getDimension();
  setName("MaximumDistribution");
  setDimension(1);
  setDistribution(distribution);
}

/* Comparison operator */
Bool MaximumDistribution::operator ==(const MaximumDistribution & other) const
{
  if (this == &other) return true;
  return (distribution_ == other.distribution_);
}

Bool MaximumDistribution::equals(const DistributionImplementation & other) const
{
  const MaximumDistribution* p_other = dynamic_cast<const MaximumDistribution*>(&other);
  return p_other && (*this == *p_other);
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

String MaximumDistribution::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(" << getDistribution().__str__() << ")";
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
  if (allSame_) setRange(distribution_.getRange());
  const Point lower(distribution_.getRange().getLowerBound());
  const Point upper(distribution_.getRange().getUpperBound());
  setRange(Interval(*std::max_element(lower.begin(), lower.end()), *std::max_element(upper.begin(), upper.end())));
}

/* Get one realization of the distribution */
Point MaximumDistribution::getRealization() const
{
  if (allSame_) return distribution_.getSample(variablesNumber_).getMax();
  const Point realization(distribution_.getRealization());
  return Point(1, *std::max_element(realization.begin(), realization.end()));
}

/* Get the PDF of the distribution */
Scalar MaximumDistribution::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  if ((point[0] <= getRange().getLowerBound()[0]) || (point[0] >= getRange().getUpperBound()[0])) return 0.0;
  // Special case for identical independent variables
  if (allSame_) return variablesNumber_ * distribution_.computePDF(point) * std::pow(distribution_.computeCDF(point), static_cast<Scalar>(variablesNumber_) - 1.0);
  // General case
  if (!distribution_.hasIndependentCopula()) DistributionImplementation::computePDF(point);
  // Special treatment of the independent copula case
  const UnsignedInteger size = distribution_.getDimension();
  Point marginalCDF(size);
  Scalar product = 1.0;
  DistributionCollection marginals(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    marginals[i] = distribution_.getMarginal(i);
    const Scalar cdf = marginals[i].computeCDF(point);
    if (cdf == 0) return 0.0;
    marginalCDF[i] = cdf;
    product *= cdf;
  }
  Scalar sum = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
    sum += marginals[i].computePDF(point) / marginalCDF[i];
  return sum * product;
}

/* Get the CDF of the distribution */
Scalar MaximumDistribution::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  // Special case for identical independent variables
  if (allSame_) return std::pow(distribution_.computeCDF(point), static_cast<Scalar>(variablesNumber_));
  // General case
  return distribution_.computeCDF(Point(distribution_.getDimension(), point[0]));
}

/* Parameters value and description accessor */
MaximumDistribution::PointWithDescriptionCollection MaximumDistribution::getParametersCollection() const
{
  // This is done on purpose to distinguish the case allSame == True
  if (allSame_) return getDistribution().getParametersCollection();
  return distribution_.getParametersCollection();
}

void MaximumDistribution::setParametersCollection(const PointCollection & parametersCollection)
{
  // This trick is needed n order to cope with the case allSame == True
  if (allSame_)
  {
    Distribution clone(getDistribution());
    clone.setParametersCollection(parametersCollection);
    distribution_ = clone;
  }
  else
    distribution_.setParametersCollection(parametersCollection);
}

/* Distribution accessor */
void MaximumDistribution::setDistribution(const Distribution & distribution)
{
  // Here we suppose that variablesNumber_ has already been initialized with the
  // correct value, ie either the distribution dimension is equal to 1 and
  // variablesNumber_ can take any positive value, or the distribution dimension
  // is greater than 1 and variablesNumber_ is equalt to this dimension
  const UnsignedInteger dimension = distribution.getDimension();
  if ((dimension > 1) && (dimension != variablesNumber_)) throw InvalidArgumentException(HERE) << "Error: the distribution dimension=" << dimension << " does not match the number of variables=" << variablesNumber_;
  distribution_ = distribution;
  allSame_ = (dimension == 1);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  isAlreadyCreatedGeneratingFunction_ = false;
  isParallel_ = distribution_.getImplementation()->isParallel();
  computeRange();
}

Distribution MaximumDistribution::getDistribution() const
{
  if (allSame_) return JointDistribution(DistributionCollection(variablesNumber_, distribution_));
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
  adv.saveAttribute( "allSame_", allSame_ );
  adv.saveAttribute( "variablesNumber_", variablesNumber_ );
}

/* Method load() reloads the object from the StorageManager */
void MaximumDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  Distribution distribution;
  adv.loadAttribute( "distribution_", distribution );
  adv.loadAttribute( "allSame_", allSame_ );
  adv.loadAttribute( "variablesNumber_", variablesNumber_ );
  setDistribution(distribution);
}

END_NAMESPACE_OPENTURNS
