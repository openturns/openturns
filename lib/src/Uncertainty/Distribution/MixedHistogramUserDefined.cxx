//                                               -*- C++ -*-
/**
 *  @brief The MixedHistogramUserDefined distribution
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
#include "openturns/MixedHistogramUserDefined.hxx"
#include "openturns/ComposedDistribution.hxx"
#include "openturns/Dirac.hxx"
#include "openturns/Histogram.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Tuples.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/RandomGenerator.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MixedHistogramUserDefined);

static const Factory<MixedHistogramUserDefined> Factory_MixedHistogramUserDefined;

/* Default constructor */
MixedHistogramUserDefined::MixedHistogramUserDefined()
  : DistributionImplementation()
  , ticksCollection_(1, Point(1))
  , kind_(1, DISCRETE)
  , probabilityTable_(1, 1.0)
  , discreteIndices_(1, 0)
  , allIndices_(1, 1, Indices(1, 0))
  , normalizedProbabilityTable_(1, 1.0)
{
  setName("MixedHistogramUserDefined");
  setDimension(1);
  computeRange();
}

/* Parameters constructor */
MixedHistogramUserDefined::MixedHistogramUserDefined(const PointCollection & ticksCollection,
    const Indices & kind,
    const Point & probabilityTable)
  : DistributionImplementation()
  , ticksCollection_(ticksCollection)
  , kind_(kind)
  , probabilityTable_(probabilityTable)
{
  setName("MixedHistogramUserDefined");
  const UnsignedInteger dimension = kind.getSize();
  // Check the ticks
  if (ticksCollection.getSize() != dimension) throw InvalidArgumentException(HERE) << "Error: expected a collection of ticks of size=" << dimension << ", got size=" << ticksCollection.getSize();
  // Check the probability table
  // kind[i] == 0 -> the ith marginal is discrete
  // kind[i] == 1 -> the ith marginal is continuous
  UnsignedInteger totalSize = 1;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (kind[i] > 1)
      throw InvalidArgumentException(HERE) << "Kind must be in [[0, 1]]";
    if (!(ticksCollection[i].getSize() >= 1))
      throw InvalidArgumentException(HERE) << "Empty ticks";
    if (ticksCollection[i].getSize() == kind[i])
      throw InvalidArgumentException(HERE) << "Need at least 2 ticks for continuous variable";
    totalSize *= ticksCollection[i].getSize() - kind[i];
  }
  if (probabilityTable.getSize() != totalSize) throw InvalidArgumentException(HERE) << "Error: expected a probability table of size=" << totalSize << ", got size=" << probabilityTable.getSize();

  // cache some data
  Indices discretization(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    // Here, kind[i] == 0 <-> kind[i] is false <-> i is discrete
    discretization[i] = ticksCollection_[i].getSize() - kind_[i];
  allIndices_ = Tuples(discretization).generate();
  for (UnsignedInteger j = 0; j < dimension; ++ j)
    if (kind_[j] == DISCRETE)
      discreteIndices_.add(j);
    else
      continuousIndices_.add(j);
  Scalar weightSum = 0.0;
  for (UnsignedInteger i = 0; i < probabilityTable_.getSize(); ++ i)
    weightSum += probabilityTable_[i];
  normalizedProbabilityTable_ = probabilityTable_ / weightSum;

  setDimension(dimension);
  computeRange();
}

/* Comparison operator */
Bool MixedHistogramUserDefined::operator ==(const MixedHistogramUserDefined & other) const
{
  if (this == &other) return true;
  return (ticksCollection_ == other.ticksCollection_) && (kind_ == other.kind_) && (probabilityTable_ == other.probabilityTable_);
}

Bool MixedHistogramUserDefined::equals(const DistributionImplementation & other) const
{
  const MixedHistogramUserDefined* p_other = dynamic_cast<const MixedHistogramUserDefined*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String MixedHistogramUserDefined::__repr__() const
{
  OSS oss(true);
  oss << "class=" << MixedHistogramUserDefined::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " ticksCollection=" << ticksCollection_
      << " kind=" << kind_
      << " probabilityTable=" << probabilityTable_;
  return oss;
}

String MixedHistogramUserDefined::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(ticksCollection = " << ticksCollection_ << ", kind = " << kind_ << ", probabilityTable = " << probabilityTable_ << ")";
  return oss;
}

/* Virtual constructor */
MixedHistogramUserDefined * MixedHistogramUserDefined::clone() const
{
  return new MixedHistogramUserDefined(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void MixedHistogramUserDefined::computeRange()
{
  const UnsignedInteger dimension = getDimension();
  Point lowerBound(dimension);
  Point upperBound(dimension);
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    const Point ticks(ticksCollection_[j]);
    lowerBound[j] = ticks[0];
    upperBound[j] = ticks[0];
    for (UnsignedInteger k = 1; k < ticks.getSize(); ++ k)
    {
      if (ticks[k] < lowerBound[j])
        lowerBound[j] = ticks[k];
      if (ticks[k] > upperBound[j])
        upperBound[j] = ticks[k];
    }
  }
  setRange(Interval(lowerBound, upperBound));
}


/* Get one realization of the distribution */
Point MixedHistogramUserDefined::getRealization() const
{
  const UnsignedInteger dimension = getDimension();
  if (!base_.getSize())
    (void) DistFunc::rDiscrete(normalizedProbabilityTable_, base_, alias_);
  const UnsignedInteger index = DistFunc::rDiscrete(base_, alias_);
  Point realization(dimension);
  for (UnsignedInteger j = 0; j < discreteIndices_.getSize(); ++j)
  {
    const UnsignedInteger jDiscrete = discreteIndices_[j];
    const UnsignedInteger k = allIndices_(index, jDiscrete);
    const Point ticks(ticksCollection_[jDiscrete]);
    realization[jDiscrete] = ticks[k];
  }
  for (UnsignedInteger j = 0; j < continuousIndices_.getSize(); ++j)
  {
    const UnsignedInteger jContinuous = continuousIndices_[j];
    const UnsignedInteger k = allIndices_(index, jContinuous);
    const Point ticks(ticksCollection_[jContinuous]);
    realization[jContinuous] = ticks[k] + (ticks[k + 1] - ticks[k]) * RandomGenerator::Generate();
  }
  return realization;
}

/* Get a sample of the distribution */
Sample MixedHistogramUserDefined::getSample(const UnsignedInteger size) const
{
  return DistributionImplementation::getSample(size);
}

/* Get the PDF of the distribution */
Scalar MixedHistogramUserDefined::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  // build the list of discrete ticks indices, with early exit if no tick matches
  Indices discreteTicksIndices(discreteIndices_.getSize());
  for (UnsignedInteger j = 0; j < discreteIndices_.getSize(); ++ j)
  {
    const Point ticks(ticksCollection_[discreteIndices_[j]]);
    const UnsignedInteger index = ticks.find(point[discreteIndices_[j]]);
    if (index >= ticks.getSize())
      return 0.0;
    discreteTicksIndices[j] = index;
  }

  // loop over the cpt
  Scalar pdfValue = 0.0;
  const UnsignedInteger totalSize = probabilityTable_.getSize();
  for (UnsignedInteger i = 0; i < totalSize; ++i)
  {
    Bool skip = false;
    // first, loop over the discrete components
    for (UnsignedInteger j = 0; j < discreteIndices_.getSize(); ++j)
    {
      const UnsignedInteger k = allIndices_(i, discreteIndices_[j]);
      if (discreteTicksIndices[j] != k)
      {
        skip = true;
        break;
      }
    }

    // exclude non-matching discrete terms
    if (skip)
      continue;

    skip = false;

    // now compute the pdf over continuous components
    Scalar pdfI = 1.0;
    for (UnsignedInteger j = 0; j < continuousIndices_.getSize(); ++j)
    {
      const UnsignedInteger k = allIndices_(i, continuousIndices_[j]);
      const Point ticks(ticksCollection_[continuousIndices_[j]]);
      const Scalar x = point[continuousIndices_[j]];
      if ((x <= ticks[k]) || (x > ticks[k + 1]))
      {
        skip = true;
        break;
      }
      pdfI *= 1.0 / (ticks[k + 1] - ticks[k]);
    }

    // exclude non-matching continuous terms
    if (skip)
      continue;

    pdfValue += normalizedProbabilityTable_[i] * pdfI;
  }
  return pdfValue;
}


/* Get the CDF of the distribution */
Scalar MixedHistogramUserDefined::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  // build the list of discrete ticks, with early exit if no tick matches
  Indices discreteTicksIndices(discreteIndices_.getSize());
  for (UnsignedInteger j = 0; j < discreteIndices_.getSize(); ++ j)
  {
    const Point ticks(ticksCollection_[discreteIndices_[j]]);
    const Scalar x = point[discreteIndices_[j]];
    UnsignedInteger index = ticks.getSize();
    for (UnsignedInteger i = 0; i < ticks.getSize(); ++i)
      if (ticks[i] <= x)
        index = i;
    if (index >= ticks.getSize())
      return 0.0;
    discreteTicksIndices[j] = index;
  }

  // loop over the cpt
  Scalar cdfValue = 0.0;
  const UnsignedInteger totalSize = probabilityTable_.getSize();
  for (UnsignedInteger i = 0; i < totalSize; ++i)
  {
    Bool skip = false;
    // first, loop over the discrete components
    for (UnsignedInteger j = 0; j < discreteIndices_.getSize(); ++j)
    {
      const UnsignedInteger k = allIndices_(i, discreteIndices_[j]);
      if (k > discreteTicksIndices[j])
      {
        skip = true;
        break;
      }
    }

    // exclude non-matching discrete terms
    if (skip)
      continue;

    skip = false;

    // now compute the cdf over continuous components
    Scalar cdfI = 1.0;
    for (UnsignedInteger j = 0; j < continuousIndices_.getSize(); ++j)
    {
      const UnsignedInteger k = allIndices_(i, continuousIndices_[j]);
      const Point ticks(ticksCollection_[continuousIndices_[j]]);
      const Scalar x = point[continuousIndices_[j]];
      if (x <= ticks[k])
      {
        skip = true;
        break;
      }
      else if (x < ticks[k + 1])
        cdfI *= (x - ticks[k]) / (ticks[k + 1] - ticks[k]);
      //else (x>ticks[k + 1]: nothing to do
    }

    // exclude non-matching continuous terms
    if (skip)
      continue;

    cdfValue += normalizedProbabilityTable_[i] * cdfI;
  }
  return cdfValue;
}

Scalar MixedHistogramUserDefined::computeComplementaryCDF(const Point & point) const
{
  return DistributionImplementation::computeComplementaryCDF(point);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex MixedHistogramUserDefined::computeCharacteristicFunction(const Scalar x) const
{
  return DistributionImplementation::computeCharacteristicFunction(x);
}

/* Get the quantile of the distribution */
Point MixedHistogramUserDefined::computeQuantile(const Scalar prob,
    const Bool tail) const
{
  return DistributionImplementation::computeQuantile(prob, tail);
}

/* Get the i-th marginal distribution */
Distribution MixedHistogramUserDefined::getMarginal(const UnsignedInteger index) const
{
  const UnsignedInteger dimension = getDimension();
  if (index >= dimension) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  if (dimension == 1) return clone();

  // contract probability table
  const Point ticks(ticksCollection_[index]);
  const UnsignedInteger size = ticks.getSize();
  Point marginalProbabilityTable((kind_[index] == DISCRETE) ? size : size - 1);
  const UnsignedInteger totalSize = probabilityTable_.getSize();
  for (UnsignedInteger i = 0; i < totalSize; ++i)
  {
    const UnsignedInteger k = allIndices_(i, index);
    marginalProbabilityTable[k] += probabilityTable_[i];
  }

  Distribution marginal;
  if (kind_[index] == DISCRETE)
  {
    SampleImplementation support(size, 1);
    support.setData(ticks);
    marginal = UserDefined(support, marginalProbabilityTable);
  }
  else
  {
    marginal = Histogram(ticks, marginalProbabilityTable);
  }
  marginal.setDescription(Description(1, getDescription()[index]));
  return marginal;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution MixedHistogramUserDefined::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";

  Indices full(dimension);
  full.fill();
  if (indices == full) return clone();

  // contract probability table
  Indices marginalKind;
  PointCollection marginalTicksCollection;
  UnsignedInteger marginalTotalSize = 1;
  Indices discretization;
  for (UnsignedInteger j = 0; j < indices.getSize(); ++j)
  {
    const UnsignedInteger index = indices[j];
    marginalKind.add(kind_[index]);
    marginalTicksCollection.add(ticksCollection_[index]);
    const UnsignedInteger size = ticksCollection_[index].getSize();
    discretization.add((kind_[index] == DISCRETE) ? size : size - 1);
    marginalTotalSize *= discretization[j];
  }
  IndicesCollection marginalAllIndices(Tuples(discretization).generate());
  Point marginalProbabilityTable(marginalTotalSize);
  const UnsignedInteger totalSize = probabilityTable_.getSize();

  // compute the base of discretization to quickly retrieve the global marginal index
  Indices productDiscretization(indices.getSize(), 1);
  for (UnsignedInteger j = 1; j < indices.getSize(); ++j)
  {
    productDiscretization[j] = productDiscretization[j - 1] * discretization[j - 1];
  }

  for (UnsignedInteger i = 0; i < totalSize; ++i)
  {
    for (UnsignedInteger j = 0; j < indices.getSize(); ++j)
    {
      // find the global marginal index
      UnsignedInteger marginalProbabilityTableIndex = 0;
      for (UnsignedInteger k = 0; k < indices.getSize(); ++k)
      {
        marginalProbabilityTableIndex += allIndices_(i, indices[k]) * productDiscretization[k];
      }
      marginalProbabilityTable[marginalProbabilityTableIndex] += probabilityTable_[i];
    }
  }

  MixedHistogramUserDefined marginal(marginalTicksCollection, marginalKind, marginalProbabilityTable);
  marginal.setDescription(getDescription().select(indices));
  return marginal;
} // getMarginal(Indices)

/* Check if the distribution is continuous */
Bool MixedHistogramUserDefined::isContinuous() const
{
  const UnsignedInteger size = kind_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
    if (kind_[i] == DISCRETE) return false;
  return true;
}

/* Check if the distribution is discrete */
Bool MixedHistogramUserDefined::isDiscrete() const
{
  const UnsignedInteger size = kind_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
    if (kind_[i] == CONTINUOUS) return false;
  return true;
}

/* Check if the distribution is integral */
Bool MixedHistogramUserDefined::isIntegral() const
{
  const Scalar epsilon = ResourceMap::GetAsScalar("DiscreteDistribution-SupportEpsilon");
  const UnsignedInteger size = kind_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (kind_[i] == CONTINUOUS) return false;
    const UnsignedInteger supportSize = ticksCollection_[i].getSize();
    for (UnsignedInteger j = 0; j < supportSize; ++j)
    {
      const Scalar x = ticksCollection_[i][j];
      if (std::abs(x - floor(x + 0.5)) > epsilon) return false;
    }
  } // i
  return true;
}


/* Compute the mean of the distribution */
void MixedHistogramUserDefined::computeMean() const
{
  const UnsignedInteger dimension = getDimension();
  mean_ = Point(dimension);
  const UnsignedInteger totalSize = probabilityTable_.getSize();
  for (UnsignedInteger i = 0; i < totalSize; ++i)
  {
    Point meani(dimension);
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      const UnsignedInteger k = allIndices_(i, j);
      const Point ticks(ticksCollection_[j]);
      if (kind_[j] == DISCRETE)
        meani[j] = ticks[k];
      else
        meani[j] = 0.5 * (ticks[k] + ticks[k + 1]);
    }
    mean_ += meani * normalizedProbabilityTable_[i];
  }
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point MixedHistogramUserDefined::getStandardDeviation() const
{
  const UnsignedInteger dimension = getDimension();
  Point standardDeviation(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    standardDeviation[i] = getMarginal(i).getStandardDeviation()[0];
  return standardDeviation;
}

/* Get the skewness of the distribution */
Point MixedHistogramUserDefined::getSkewness() const
{
  const UnsignedInteger dimension = getDimension();
  Point skewness(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    skewness[i] = getMarginal(i).getSkewness()[0];
  return skewness;
}

/* Get the kurtosis of the distribution */
Point MixedHistogramUserDefined::getKurtosis() const
{
  const UnsignedInteger dimension = getDimension();
  Point kurtosis(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    kurtosis[i] = getMarginal(i).getKurtosis()[0];
  return kurtosis;
}

/* Compute the covariance of the distribution */
void MixedHistogramUserDefined::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  covariance_ = CovarianceMatrix(dimension);
  for (UnsignedInteger j = 0; j < dimension; ++j)
    covariance_(j, j) = 0.0;
  // First, compute E(X.X^t)
  const UnsignedInteger totalSize = probabilityTable_.getSize();
  for (UnsignedInteger i = 0; i < totalSize; ++i)
  {
    Point meanI(dimension);
    Point varianceI(dimension);
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      const UnsignedInteger k = allIndices_(i, j);
      const Point ticks(ticksCollection_[j]);
      if (kind_[j] == DISCRETE)
        meanI[j] = ticks[k];
      else
      {
        meanI[j] = 0.5 * (ticks[k] + ticks[k + 1]);
        const Scalar eta = ticks[k + 1] - ticks[k];
        varianceI[j] = eta * eta / 12.0;
      }
    }
    for(UnsignedInteger row = 0; row < dimension; ++row)
      for(UnsignedInteger column = 0; column <= row; ++column)
        covariance_(row, column) += normalizedProbabilityTable_[i] * (((row == column) ? varianceI[row] : 0.0) + meanI[row] * meanI[column]);
  }
  // Then, subtract E(X).E(X)^t
  const Point mean(getMean());
  for(UnsignedInteger row = 0; row < dimension; ++row)
    for(UnsignedInteger column = 0; column <= row; ++column)
      covariance_(row, column) -= mean[row] * mean[column];
  isAlreadyComputedCovariance_ = true;
}

/* Get the moments of the standardized distribution */
Point MixedHistogramUserDefined::getStandardMoment(const UnsignedInteger n) const
{
  const UnsignedInteger dimension = getDimension();
  Point standardMoment(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    standardMoment[i] = getMarginal(i).getStandardMoment(n)[0];
  return standardMoment;
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution MixedHistogramUserDefined::getStandardRepresentative() const
{
  return clone();
}

/* Ticks collection accessor */
void MixedHistogramUserDefined::setTicksCollection(const Collection<Point> & ticksCollection)
{
  *this = MixedHistogramUserDefined(ticksCollection, kind_, probabilityTable_);
  computeRange();
}

Collection<Point> MixedHistogramUserDefined::getTicksCollection() const
{
  return ticksCollection_;
}

/* Kind accessor */
void MixedHistogramUserDefined::setKind(const Indices & kind)
{
  *this = MixedHistogramUserDefined(ticksCollection_, kind, probabilityTable_);
  computeRange();
}

Indices MixedHistogramUserDefined::getKind() const
{
  return kind_;
}

/* Probability table accessor */
void MixedHistogramUserDefined::setProbabilityTable(const Point & probabilityTable)
{
  *this = MixedHistogramUserDefined(ticksCollection_, kind_, probabilityTable);
  computeRange();
}

Point MixedHistogramUserDefined::getProbabilityTable() const
{
  return probabilityTable_;
}

/* Conversion as a Mixture */
Mixture MixedHistogramUserDefined::asMixture() const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger totalSize = probabilityTable_.getSize();
  Mixture mixture;
  // Special case: dimension 1
  if (dimension == 1)
  {
    const Point ticks(ticksCollection_[0]);
    if (kind_[0] == DISCRETE)
    {
      const UnsignedInteger size = ticks.getSize();
      SampleImplementation support(size, 1);
      support.setData(ticks);
      mixture = Mixture(Mixture::DistributionCollection(1, UserDefined(support, probabilityTable_)));
    }
    // Continuous
    else
      mixture = Mixture(Mixture::DistributionCollection(1, Histogram(ticks, probabilityTable_)));
  } // dimension == 1
  else
  {
    Bool allDiscrete = (discreteIndices_.getSize() == dimension);
    // Multivariate discrete
    if (allDiscrete)
    {
      Sample support(totalSize, dimension);
      for (UnsignedInteger i = 0; i < totalSize; ++i)
      {
        for (UnsignedInteger j = 0; j < dimension; ++j)
          support(i, j) = ticksCollection_[j][allIndices_(i, j)];
      }
      mixture = Mixture(Mixture::DistributionCollection(1, UserDefined(support, probabilityTable_)));
    } // allDiscrete
    else
    {
      Mixture::DistributionCollection atoms(totalSize);
      for (UnsignedInteger i = 0; i < totalSize; ++i)
      {
        Mixture::DistributionCollection subAtoms(dimension);
        for (UnsignedInteger j = 0; j < dimension; ++j)
        {
          const UnsignedInteger k = allIndices_(i, j);
          const Point ticks = ticksCollection_[j];
          if (kind_[j] == DISCRETE)
            subAtoms[j] = Dirac(Point(1, ticks[k]));
          // Continuous
          else
            subAtoms[j] = Uniform(ticks[k], ticks[k + 1]);
        } // j
        atoms[i] = ComposedDistribution(subAtoms);
      } // i
      mixture = Mixture(atoms, probabilityTable_);
    } // At least one continuous
  } // dimension > 1
  mixture.setDescription(getDescription());
  return mixture;
}

void MixedHistogramUserDefined::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "ticksCollection_", ticksCollection_ );
  adv.saveAttribute( "kind_", kind_ );
  adv.saveAttribute( "probabilityTable_", probabilityTable_ );
}

/* Method load() reloads the object from the StorageManager */
void MixedHistogramUserDefined::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "ticksCollection_", ticksCollection_ );
  adv.loadAttribute( "kind_", kind_ );
  adv.loadAttribute( "probabilityTable_", probabilityTable_ );
  computeRange();
}

/* Description accessor */
void MixedHistogramUserDefined::setDescription(const Description & description)
{
  DistributionImplementation::setDescription(description);
}

END_NAMESPACE_OPENTURNS
