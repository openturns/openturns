//                                               -*- C++ -*-
/**
 *  @brief The class that implements a generic mechanism to extract marginal distributions
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
#include <cmath>

#include "openturns/MarginalDistribution.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MarginalDistribution)

static const Factory<MarginalDistribution> Factory_MarginalDistribution;

/* Default constructor */
MarginalDistribution::MarginalDistribution()
  : DistributionImplementation()
  , distribution_()
  , indices_(1, 0)
{
  setName("MarginalDistribution");
  setDimension(1);
  setDistribution(Uniform());
}

/* Parameters constructor */
MarginalDistribution::MarginalDistribution(const Distribution & distribution,
    const UnsignedInteger & index)
  : DistributionImplementation()
  , distribution_()
  , indices_()
{
  setName("MarginalDistribution");
  setParallel(distribution.getImplementation()->isParallel());
  setDistributionAndIndices(distribution, Indices(1, index));
}

/* Parameters constructor */
MarginalDistribution::MarginalDistribution(const Distribution & distribution,
    const Indices & indices)
  : DistributionImplementation()
  , distribution_()
  , indices_()
{
  setName("MarginalDistribution");
  setParallel(distribution.getImplementation()->isParallel());
  setDistributionAndIndices(distribution, indices);
}

/* Comparison operator */
Bool MarginalDistribution::operator ==(const MarginalDistribution & other) const
{
  if (this == &other) return true;
  return (distribution_ == other.distribution_) && (indices_ == other.indices_);
}

Bool MarginalDistribution::equals(const DistributionImplementation & other) const
{
  const MarginalDistribution* p_other = dynamic_cast<const MarginalDistribution*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String MarginalDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << MarginalDistribution::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " distribution=" << distribution_
      << " indices=" << indices_;
  return oss;
}

String MarginalDistribution::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(distribution="
      << distribution_.__str__()
      << ", indices=" << indices_;
  oss << ")";
  return oss;
}


/* Distribution accessor */
void MarginalDistribution::setDistribution(const Distribution & distribution)
{
  setDistributionAndIndices(distribution, indices_);
}


/* Distribution collection accessor */
Distribution MarginalDistribution::getDistribution() const
{
  return distribution_;
}


/* Indices accessor */
void MarginalDistribution::setIndices(const Indices & indices)
{
  setDistributionAndIndices(distribution_, indices);
}


/* Indices accessor */
Indices MarginalDistribution::getIndices() const
{
  return indices_;
}


void MarginalDistribution::setDistributionAndIndices(const Distribution & distribution,
    const Indices & indices)
{
  if (!indices.check(distribution.getDimension())) throw InvalidArgumentException(HERE) << "Error: the given indices=" << indices << " are not compatible with the given distribution dimension=" << distribution.getDimension();
  distribution_ = distribution;
  isCopula_ = distribution.isCopula();
  indices_ = indices;
  // Set the dimension
  const UnsignedInteger dimension = indices.getSize();
  setDimension(dimension);
  // Compute the range
  // From the underlying distribution
  Point distributionLowerBound(distribution.getRange().getLowerBound());
  lowerBound_ = distributionLowerBound;
  Interval::BoolCollection distributionFiniteLowerBound(distribution.getRange().getFiniteLowerBound());
  Point distributionUpperBound(distribution.getRange().getUpperBound());
  upperBound_ = distributionUpperBound;
  Interval::BoolCollection distributionFiniteUpperBound(distribution.getRange().getFiniteUpperBound());
  // For the marginal distribution
  Point lowerBound(dimension);
  Interval::BoolCollection finiteLowerBound(dimension);
  Point upperBound(dimension);
  Interval::BoolCollection finiteUpperBound(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const UnsignedInteger j = indices[i];
    lowerBound[i] = distributionLowerBound[j];
    finiteLowerBound[i] = distributionFiniteLowerBound[j];
    upperBound[i] = distributionUpperBound[j];
    finiteUpperBound[i] = distributionFiniteUpperBound[j];
  }
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
  // Extract the description
  const Description distributionDescription(distribution_.getDescription());
  Description description(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    description[i] = distributionDescription[indices_[i]];
  setDescription(description);
}


/* Virtual constructor */
MarginalDistribution * MarginalDistribution::clone() const
{
  return new MarginalDistribution(*this);
}

/* Get one realization of the MarginalDistribution */
Point MarginalDistribution::getRealization() const
{
  const UnsignedInteger dimension = getDimension();
  const Point distributionRealization(distribution_.getRealization());
  Point realization(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    realization[i] = distributionRealization[indices_[i]];
  return realization;
}

Sample MarginalDistribution::getSample(const UnsignedInteger size) const
{
  return distribution_.getSample(size).getMarginal(indices_);
}

/* Get the CDF of the MarginalDistribution */
Scalar MarginalDistribution::computeCDF(const Point & point) const
{
  return distribution_.computeCDF(expandPoint(point));
}

Scalar MarginalDistribution::computeSurvivalFunction(const Point & point) const
{
  return distribution_.computeSurvivalFunction(expandPoint(point, false));
}

/* Compute the probability content of an interval */
Scalar MarginalDistribution::computeProbability(const Interval & interval) const
{
  return distribution_.computeProbability(Interval(expandPoint(interval.getLowerBound(), false), expandPoint(interval.getUpperBound())));
}

/* Compute the mean of the distribution. It is cheap if the marginal means are cheap */
void MarginalDistribution::computeMean() const
{
  mean_ = reducePoint(distribution_.getMean());
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point MarginalDistribution::getStandardDeviation() const
{
  return reducePoint(distribution_.getStandardDeviation());
}

/* Compute the covariance of the distribution */
void MarginalDistribution::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  const CovarianceMatrix distributionCovariance(distribution_.getCovariance());
  covariance_ = CovarianceMatrix(dimension);
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    const UnsignedInteger jDistribution = indices_[j];
    for (UnsignedInteger i = j; i < dimension; ++i)
    {
      const UnsignedInteger iDistribution = indices_[i];
      covariance_(i, j) = distributionCovariance(iDistribution, jDistribution);
    }
  }
  isAlreadyComputedCovariance_ = true;
} // computeCovariance

/* Get the skewness of the distribution */
Point MarginalDistribution::getSkewness() const
{
  return reducePoint(distribution_.getSkewness());
}

/* Get the kurtosis of the distribution */
Point MarginalDistribution::getKurtosis() const
{
  return reducePoint(distribution_.getKurtosis());
}

/* Get the Spearman correlation of the distribution */
CorrelationMatrix MarginalDistribution::getSpearmanCorrelation() const
{
  const UnsignedInteger dimension = getDimension();
  const CorrelationMatrix distributionSpearmanCorrelation(distribution_.getSpearmanCorrelation());
  CorrelationMatrix spearmanCorrelation(dimension);
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    const UnsignedInteger jDistribution = indices_[j];
    for (UnsignedInteger i = j; i < dimension; ++i)
    {
      const UnsignedInteger iDistribution = indices_[i];
      spearmanCorrelation(i, j) = distributionSpearmanCorrelation(iDistribution, jDistribution);
    }
  }
  return spearmanCorrelation;
}

/* Get the Spearman correlation of the distribution */
CorrelationMatrix MarginalDistribution::getKendallTau() const
{
  const UnsignedInteger dimension = getDimension();
  const CorrelationMatrix distributionKendallTau(distribution_.getKendallTau());
  CorrelationMatrix kendallTau(dimension);
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    const UnsignedInteger jDistribution = indices_[j];
    for (UnsignedInteger i = j; i < dimension; ++i)
    {
      const UnsignedInteger iDistribution = indices_[i];
      kendallTau(i, j) = distributionKendallTau(iDistribution, jDistribution);
    }
  }
  return kendallTau;
}

/* Get the i-th marginal distribution */
Distribution MarginalDistribution::getMarginal(const UnsignedInteger i) const
{
  if (i >= getDimension()) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  return getMarginal(Indices(1, i));
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution MarginalDistribution::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  if (dimension == 1) return clone();
  // Build the indices associated to the marginal of the marginal
  const UnsignedInteger outputDimension = indices.getSize();
  Indices marginalIndices(outputDimension);
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
    marginalIndices[i] = indices_[indices[i]];
  return new MarginalDistribution(distribution_, marginalIndices);
}

/* Get the isoprobabilistic transformation */
MarginalDistribution::IsoProbabilisticTransformation MarginalDistribution::getIsoProbabilisticTransformation() const
{
  return distribution_.getIsoProbabilisticTransformation().getMarginal(indices_);
}

/* Get the inverse isoprobabilist transformation */
MarginalDistribution::InverseIsoProbabilisticTransformation MarginalDistribution::getInverseIsoProbabilisticTransformation() const
{
  return distribution_.getInverseIsoProbabilisticTransformation().getMarginal(indices_);
}

/* Get the standard distribution */
Distribution MarginalDistribution::getStandardDistribution() const
{
  return distribution_.getStandardDistribution().getMarginal(indices_).getImplementation();
}

/* Parameters value and description accessor */
MarginalDistribution::PointWithDescriptionCollection MarginalDistribution::getParametersCollection() const
{
  const PointWithDescriptionCollection allParameters(distribution_.getParametersCollection());
  PointWithDescriptionCollection parametersCollection;
  const UnsignedInteger size = indices_.getSize();
  // marginal parameters, can be omitted (BlockIndependentCopula)
  if (allParameters.getSize() == distribution_.getDimension() + 1)
    for (UnsignedInteger i = 0; i < size; ++ i)
      parametersCollection.add(allParameters[indices_[i]]);
  // dependency parameters, mandatory
  if (distribution_.getDimension() > 1)
  {
    parametersCollection.add(allParameters[allParameters.getSize() - 1]);
  }
  return parametersCollection;
} // getParametersCollection

/* Tell if the distribution has independent copula */
Bool MarginalDistribution::hasIndependentCopula() const
{
  return (getDimension() == 1) || distribution_.hasIndependentCopula();
}

/* Tell if the distribution has elliptical copula */
Bool MarginalDistribution::hasEllipticalCopula() const
{
  return (getDimension() == 1) || distribution_.hasEllipticalCopula();
}

/* Check if the distribution is elliptical */
Bool MarginalDistribution::isElliptical() const
{
  return distribution_.isElliptical();
}

/* Check if the distribution is continuous */
Bool MarginalDistribution::isContinuous() const
{
  return distribution_.isContinuous();
}

/* Check if the distribution is discrete */
Bool MarginalDistribution::isDiscrete() const
{
  return distribution_.isDiscrete();
}

/* Tell if the distribution is integer valued */
Bool MarginalDistribution::isIntegral() const
{
  return distribution_.isIntegral();
}

/* Method to expand a given point in the marginal space to a point in the underlying distribution space */
Point MarginalDistribution::expandPoint(const Point & point,
                                        const Bool upper) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << dimension << ", got dimension=" << point.getDimension();
  Point distributionPoint(upper ? upperBound_ : lowerBound_);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    distributionPoint[indices_[i]] = point[i];
  return distributionPoint;
}

/* Method to reduce a given point in the distribution space to a point in the marginal distribution space */
Point MarginalDistribution::reducePoint(const Point & point) const
{
  if (point.getDimension() != distribution_.getDimension()) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << distribution_.getDimension() << ", got dimension=" << point.getDimension();
  const UnsignedInteger dimension = getDimension();
  Point marginalPoint(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    marginalPoint[i] = point[indices_[i]];
  return marginalPoint;
}

/* Method save() stores the object through the StorageManager */
void MarginalDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "distribution_", distribution_ );
  adv.saveAttribute( "indices_", indices_ );
}

/* Method load() reloads the object from the StorageManager */
void MarginalDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  Distribution distribution;
  Indices indices;
  adv.loadAttribute( "distribution_", distribution );
  adv.loadAttribute( "indices_", indices );
  setDistributionAndIndices(distribution, indices);
}

END_NAMESPACE_OPENTURNS
