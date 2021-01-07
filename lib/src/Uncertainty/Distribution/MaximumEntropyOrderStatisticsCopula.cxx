//                                               -*- C++ -*-
/**
 *  @brief The maximum entropy order statistics copula
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/MaximumEntropyOrderStatisticsCopula.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MaximumEntropyOrderStatisticsCopula)

static const Factory<MaximumEntropyOrderStatisticsCopula> Factory_MaximumEntropyOrderStatisticsCopula;

/* Default constructor */
MaximumEntropyOrderStatisticsCopula::MaximumEntropyOrderStatisticsCopula()
  : SklarCopula(MaximumEntropyOrderStatisticsDistribution())
  , distribution_()
{
  setName("MaximumEntropyOrderStatisticsCopula");
  setDimension(distribution_.getDimension());
  isParallel_ = distribution_.isParallel();
}

/* Parameters constructor */
MaximumEntropyOrderStatisticsCopula::MaximumEntropyOrderStatisticsCopula(const DistributionCollection & coll)
  : SklarCopula(MaximumEntropyOrderStatisticsDistribution(coll))
  , distribution_(coll)
{
  setName("MaximumEntropyOrderStatisticsCopula");
  setDimension(distribution_.getDimension());
  computeRange();
  isParallel_ = distribution_.isParallel();
}

/* Constructor with no check of the parameters, to speed-up margina creations */
MaximumEntropyOrderStatisticsCopula::MaximumEntropyOrderStatisticsCopula(const MaximumEntropyOrderStatisticsDistribution & distribution)
  : SklarCopula(distribution)
  , distribution_(distribution)
{
  setName("MaximumEntropyOrderStatisticsCopula");
  setDimension(distribution_.getDimension());
  computeRange();
  isParallel_ = distribution_.isParallel();
}

/* Comparison operator */
Bool MaximumEntropyOrderStatisticsCopula::operator ==(const MaximumEntropyOrderStatisticsCopula & other) const
{
  if (this == &other) return true;
  return (distribution_ == other.distribution_);
}

Bool MaximumEntropyOrderStatisticsCopula::equals(const DistributionImplementation & other) const
{
  const MaximumEntropyOrderStatisticsCopula* p_other = dynamic_cast<const MaximumEntropyOrderStatisticsCopula*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String MaximumEntropyOrderStatisticsCopula::__repr__() const
{
  OSS oss(true);
  oss << "class=" << MaximumEntropyOrderStatisticsCopula::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " distribution=" << distribution_;
  return oss;
}

String MaximumEntropyOrderStatisticsCopula::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(collection = " << distribution_.distributionCollection_ << ")";
  return oss;
}

/* Virtual constructor */
MaximumEntropyOrderStatisticsCopula * MaximumEntropyOrderStatisticsCopula::clone() const
{
  return new MaximumEntropyOrderStatisticsCopula(*this);
}

/* Get the kth approximation */
PiecewiseHermiteEvaluation MaximumEntropyOrderStatisticsCopula::getApproximation(const UnsignedInteger k) const
{
  return distribution_.getApproximation(k);
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution MaximumEntropyOrderStatisticsCopula::getMarginal(const Indices & indices) const
{
  const UnsignedInteger size = indices.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "indices is empty";
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  if (dimension == 1) return clone();
  if (size == 1)
  {
    Implementation marginal(getMarginal(indices[0]).getImplementation());
    marginal->setDescription(Description(1, getDescription()[indices[0]]));
    return marginal;
  }
  Description marginalDescription(0);
  const Description description(getDescription());
  MaximumEntropyOrderStatisticsCopula::Implementation marginal(new MaximumEntropyOrderStatisticsCopula(distribution_.getMarginalAsMaximumEntropyOrderStatisticsDistribution(indices)));
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger j = indices[i];
    marginalDescription.add(description[j]);
  }
  marginal->setDescription(marginalDescription);
  return marginal;
}


/* Distribution collection accessor */
void MaximumEntropyOrderStatisticsCopula::setDistributionCollection(const DistributionCollection & coll)
{
  distribution_.setDistributionCollection(coll);
}


/* Parameters value and description accessor */
MaximumEntropyOrderStatisticsCopula::PointWithDescriptionCollection MaximumEntropyOrderStatisticsCopula::getParametersCollection() const
{
  const UnsignedInteger dimension = getDimension();
  PointWithDescriptionCollection parameters(dimension);
  const Description description(getDescription());
  // First put the marginal parameters
  for (UnsignedInteger marginalIndex = 0; marginalIndex < dimension; ++marginalIndex)
  {
    // Each marginal distribution must output a collection of parameters of size 1, even if it contains an empty Point
    const PointWithDescriptionCollection marginalParameters(distribution_.distributionCollection_[marginalIndex].getParametersCollection());
    PointWithDescription point(marginalParameters[0]);
    Description marginalParametersDescription(point.getDescription());
    // Here we must add a unique prefix to the marginal parameters description in order to deambiguate the parameters of different marginals sharing the same description
    for (UnsignedInteger i = 0; i < point.getDimension(); ++i) marginalParametersDescription[i] = (OSS() << marginalParametersDescription[i] << "_marginal_" << marginalIndex);
    point.setDescription(marginalParametersDescription);
    point.setName(description[marginalIndex]);
    parameters[marginalIndex] = point;
  } // marginalIndex
  return parameters;
} // getParametersCollection


void MaximumEntropyOrderStatisticsCopula::setParametersCollection(const PointCollection& parametersCollection)
{
  distribution_.setParametersCollection(parametersCollection);
}


MaximumEntropyOrderStatisticsCopula::DistributionCollection MaximumEntropyOrderStatisticsCopula::getDistributionCollection() const
{
  return distribution_.distributionCollection_;
}

/* Method save() stores the object through the StorageManager */
void MaximumEntropyOrderStatisticsCopula::save(Advocate & adv) const
{
  SklarCopula::save(adv);
  adv.saveAttribute("distribution_", distribution_);
}

/* Method load() reloads the object from the StorageManager */
void MaximumEntropyOrderStatisticsCopula::load(Advocate & adv)
{
  SklarCopula::load(adv);
  adv.loadAttribute("distribution_", distribution_);
}


END_NAMESPACE_OPENTURNS
