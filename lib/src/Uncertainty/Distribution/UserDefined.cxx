//                                               -*- C++ -*-
/**
 *  @brief The UserDefined distribution
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
#include <cstdlib>
#include <cmath>
#include "openturns/UserDefined.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection<UserDefinedPair>);
static const Factory<PersistentCollection<UserDefinedPair> > RegisteredFactory;

CLASSNAMEINIT(UserDefined);
static const Factory<UserDefined> RegisteredFactory_alt2;


/* Default constructor */
UserDefined::UserDefined()
  : DiscreteDistribution()
  , collection_(UserDefinedPairCollection(1))
  , cumulativeProbabilities_(1, 1.0)
  , hasUniformWeights_(true)
{
  setName("UserDefined");
  // Empty range
  setRange(Interval(1.0, 0.0));
}

/* Constructor from UserDefinedPairCollection */
UserDefined::UserDefined(const UserDefinedPairCollection & collection)
  : DiscreteDistribution()
  , collection_(0)
  , cumulativeProbabilities_(0)
  , hasUniformWeights_(false)
{
  setName("UserDefined");
  // We set the dimension of the UserDefined distribution
  // This call set also the range
  setPairCollection( collection );
  if ((getDimension() == 1) || (collection_.getSize() <= ResourceMap::GetAsUnsignedInteger("UserDefined-SmallSize"))) compactSupport();
}

/* Constructor from a sample */
UserDefined::UserDefined(const NumericalSample & sample)
  : DiscreteDistribution()
  , collection_(0)
  , cumulativeProbabilities_(0)
  , hasUniformWeights_(true)
{
  const UnsignedInteger size(sample.getSize());
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a UserDefined distribution based on an empty sample.";
  UserDefinedPairCollection collection(size);
  const NumericalScalar w(1.0 / size);
  for (UnsignedInteger i = 0; i < size; ++i) collection[i] = UserDefinedPair(sample[i], w);
  // We set the dimension of the UserDefined distribution
  // This call set also the range
  setPairCollection( collection );
  if ((getDimension() == 1) || (collection_.getSize() <= ResourceMap::GetAsUnsignedInteger("UserDefined-SmallSize"))) compactSupport();
  setName(sample.getName());
  if(!sample.getDescription().isBlank()) setDescription(sample.getDescription());
}

/* Constructor from a sample and the associated weights */
UserDefined::UserDefined(const NumericalSample & sample,
                         const NumericalPoint & weights)
  : DiscreteDistribution()
  , collection_(0)
  , cumulativeProbabilities_(0)
  , hasUniformWeights_(false)
{
  const UnsignedInteger size(sample.getSize());
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a UserDefined distribution based on an empty sample.";
  if (weights.getDimension() != size) throw InvalidArgumentException(HERE) << "Error: cannot build a UserDefined distribution if the weights don't have the same dimension as the sample size.";
  UserDefinedPairCollection collection(size);
  // Normalize the weights here
  NumericalScalar sumWeights(0.0);
  for (UnsignedInteger i = 0; i < size; ++i) sumWeights += weights[i];
  for (UnsignedInteger i = 0; i < size; ++i) collection[i] = UserDefinedPair(sample[i], weights[i] / sumWeights);
  // We set the dimension of the UserDefined distribution
  // This call set also the range
  setPairCollection( collection );
  if ((getDimension() == 1) || (collection_.getSize() <= ResourceMap::GetAsUnsignedInteger("UserDefined-SmallSize"))) compactSupport();
  setName(sample.getName());
  if(!sample.getDescription().isBlank()) setDescription(sample.getDescription());
}

/* Comparison operator */
Bool UserDefined::operator ==(const UserDefined & other) const
{
  if (this == &other) return true;
  return collection_ == other.collection_;
}

/* String converter */
String UserDefined::__repr__() const
{
  OSS oss;
  oss << "class=" << UserDefined::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " collection=" << collection_;
  return oss;
}

String UserDefined::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(";
  String separator("");
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
  {
    oss << separator << "{x = " << collection_[i].getX().__str__() << ", p = " << collection_[i].getP() << "}";
    separator = ", ";
  }
  oss << ")";
  return oss;
}

/* Virtual constructor */
UserDefined * UserDefined::clone() const
{
  return new UserDefined(*this);
}

/* Get one realization of the distribution */
NumericalPoint UserDefined::getRealization() const
{
  // Efficient algorithm for uniform weights
  const UnsignedInteger size(collection_.getSize());
  if (hasUniformWeights_)
  {
    const UnsignedInteger j(RandomGenerator::IntegerGenerate(size));
    return collection_[j].getX();
  }
  const NumericalScalar uniformRealization(RandomGenerator::Generate());
  if (uniformRealization <= cumulativeProbabilities_[0]) return collection_[0].getX();
  UnsignedInteger j0(0);
  UnsignedInteger j1(size - 1);
  while (j1 - j0 > 1)
  {
    const UnsignedInteger jm((j0 + j1) / 2);
    if (uniformRealization > cumulativeProbabilities_[jm]) j0 = jm;
    else j1 = jm;
  }
  return collection_[j1].getX();
}

/* Get the PDF of the distribution */
NumericalScalar UserDefined::computePDF(const NumericalPoint & point) const
{
  const UnsignedInteger dimension(getDimension());
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const UnsignedInteger size(collection_.getSize());
  NumericalScalar pdf(0.0);
  // Quick search for 1D case
  const NumericalScalar x(point[0]);
  UnsignedInteger upper(size - 1);
  NumericalScalar xUpper(collection_[upper].getX()[0]);
  if (x > xUpper + supportEpsilon_) return 0.0;
  UnsignedInteger lower(0);
  NumericalScalar xLower(collection_[lower].getX()[0]);
  if (x < xLower - supportEpsilon_) return 0.0;
  // Use bisection search of the correct index
  while (upper - lower > 1)
  {
    // The integer arithmetic ensure that middle will be strictly between lower and upper as far as upper - lower > 1
    const UnsignedInteger middle((upper + lower) / 2);
    const NumericalScalar xMiddle(collection_[middle].getX()[0]);
    if (xMiddle > x + supportEpsilon_)
    {
      upper = middle;
      xUpper = xMiddle;
    }
    else
    {
      lower = middle;
      xLower = xMiddle;
    }
  } // while
  // At this point we have upper == lower or upper == lower + 1, with lower - epsilon <= x < upper + epsilon
  SignedInteger index(upper);
  while ((index < static_cast<SignedInteger>(size)) && (std::abs(x - collection_[index].getX()[0]) <= supportEpsilon_))
  {
    if ((point - collection_[index].getX()).norm() <= supportEpsilon_) pdf += collection_[index].getP();
    ++ index;
  }
  index = upper;
  --index;
  while ((index >= 0) && (std::abs(x - collection_[index].getX()[0]) <= supportEpsilon_))
  {
    if ((point - collection_[index].getX()).norm() <= supportEpsilon_) pdf += collection_[index].getP();
    --index;
  }
  return pdf;
}

/* Get the CDF of the distribution */
NumericalScalar UserDefined::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  NumericalScalar cdf(0.0);
  const UnsignedInteger size(collection_.getSize());
  const UnsignedInteger dimension(getDimension());
  // Quick search for 1D case
  if (dimension == 1)
  {
    const NumericalScalar x(point[0]);
    UnsignedInteger upper(size - 1);
    NumericalScalar xUpper(collection_[upper].getX()[0]);
    if (x > xUpper - supportEpsilon_) return 1.0;
    UnsignedInteger lower(0);
    NumericalScalar xLower(collection_[lower].getX()[0]);
    if (x <= xLower - supportEpsilon_) return 0.0;
    // Use dichotomic search of the correct index
    while (upper - lower > 1)
    {
      // The integer arithmetic insure that middle will be strictly between lower and upper as far as upper - lower > 1
      const UnsignedInteger middle((upper + lower) / 2);
      const NumericalScalar xMiddle(collection_[middle].getX()[0]);
      if (xMiddle > x + supportEpsilon_)
      {
        upper = middle;
        xUpper = xMiddle;
      }
      else
      {
        lower = middle;
        xLower = xMiddle;
      }
    } // while
    // At this point we have upper == lower or upper == lower + 1, with lower - epsilon <= x < upper + epsilon
    // If xLower < x < xUpper, the contribution of lower must be taken into account, else it
    // must be discarded
    if (x <= xUpper - supportEpsilon_) return cumulativeProbabilities_[lower];
    return cumulativeProbabilities_[upper];
  }
  // Dimension > 1
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalPoint x(collection_[i].getX());
    UnsignedInteger j(0);
    while ((j < dimension) && (x[j] <= point[j] + supportEpsilon_)) ++j;
    if (j == dimension) cdf += collection_[i].getP();
  }
  return cdf;
}

/* Get the PDF gradient of the distribution */
NumericalPoint UserDefined::computePDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  const UnsignedInteger size(collection_.getSize());
  NumericalPoint pdfGradient(size, 0.0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if ((point - collection_[i].getX()).norm() < supportEpsilon_)
    {
      pdfGradient[i] = 1.0;
      return pdfGradient;
    }
  }
  return pdfGradient;
}


/* Get the CDF gradient of the distribution */
NumericalPoint UserDefined::computeCDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  const UnsignedInteger size(collection_.getSize());
  const UnsignedInteger dimension(getDimension());
  NumericalPoint cdfGradient(size, 0.0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalPoint x(collection_[i].getX());
    UnsignedInteger j(0);
    while ((j < dimension) && (point[j] <= x[j])) ++j;
    if (j == dimension) cdfGradient[i] = 1.0;
  }
  return cdfGradient;
}

/* Compute the numerical range of the distribution given the parameters values */
void UserDefined::computeRange()
{
  const UnsignedInteger size(collection_.getSize());
  const UnsignedInteger dimension(getDimension());
  // Return an empty interval for the empty collection case
  if (size == 0)
  {
    setRange(Interval(NumericalPoint(dimension, 1.0), NumericalPoint(dimension, 0.0)));
    return;
  }
  // The number of points is finite, so are the bounds
  const Interval::BoolCollection finiteLowerBound(dimension, true);
  const Interval::BoolCollection finiteUpperBound(dimension, true);
  NumericalPoint lowerBound(collection_[0].getX());
  NumericalPoint upperBound(lowerBound);
  for (UnsignedInteger i = 1; i < size; ++i)
  {
    const NumericalPoint pt(collection_[i].getX());
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      const NumericalScalar x(pt[j]);
      if (x < lowerBound[j]) lowerBound[j] = x;
      if (x > upperBound[j]) upperBound[j] = x;
    }
  }
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Get the support of a discrete distribution that intersect a given interval */
NumericalSample UserDefined::getSupport(const Interval & interval) const
{
  if (interval.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given interval has a dimension that does not match the distribution dimension.";
  NumericalSample result(0, getDimension());
  const UnsignedInteger size(collection_.getSize());
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalPoint x(collection_[i].getX());
    if (interval.contains(x)) result.add(x);
  }
  return result;
}

/* Tell if the distribution is integer valued */
Bool UserDefined::isIntegral() const
{
  if (getDimension() != 1) return false;
  const UnsignedInteger size(collection_.getSize());
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalScalar x(collection_[i].getX()[0]);
    if (std::abs(x - round(x)) >= supportEpsilon_) return false;
  }
  return true;
}

/* Compute the mean of the distribution */
void UserDefined::computeMean() const
{
  const UnsignedInteger size(collection_.getSize());
  NumericalPoint mean(getDimension());
  for (UnsignedInteger i = 0; i < size; ++i) mean += collection_[i].getP() * collection_[i].getX();
  mean_ = mean;
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void UserDefined::computeCovariance() const
{
  const UnsignedInteger size(collection_.getSize());
  const UnsignedInteger dimension(getDimension());
  covariance_ = CovarianceMatrix(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) covariance_(i, i) = 0.0;
  const NumericalPoint mean(getMean());
  for (UnsignedInteger k = 0; k < size; ++k)
  {
    const NumericalPoint xK(collection_[k].getX() - mean);
    const NumericalScalar pK(collection_[k].getP());
    for (UnsignedInteger i = 0; i < dimension; ++i)
      for (UnsignedInteger j = 0; j <= i; ++j)
        covariance_(i, j) += pK * xK[i] * xK[j];
  }
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value and description accessor */
UserDefined::NumericalPointWithDescriptionCollection UserDefined::getParametersCollection() const
{
  const UnsignedInteger dimension(getDimension());
  NumericalPointWithDescriptionCollection parameters(dimension + 1);
  const UnsignedInteger size(collection_.getSize());
  // Loop over the dimension to extract the marginal coordinates of the support
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    NumericalPointWithDescription point(size);
    Description description(size);
    for (UnsignedInteger j = 0; j < size; ++j)
    {
      point[j] = collection_[j].getX()[i];
      OSS oss;
      oss << "X^" << i << "_" << j;
      description[j] = oss;
    }
    point.setDescription(description);
    parameters[i] = point;
  }
  // Loop over the size to extract the probabilities, seen as the dependence parameters
  NumericalPointWithDescription point(size);
  Description description(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    point[i] = collection_[i].getP();
    OSS oss;
    oss << "p_" << i;
    description[i] = oss;
  }
  point.setDescription(description);
  point.setName(getDescription()[0]);
  parameters[dimension] = point;
  return parameters;
}

/* Parameters value accessor */
NumericalPoint UserDefined::getParameter() const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger size = collection_.getSize();
  NumericalPoint point((dimension + 1) * size);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    for (UnsignedInteger j = 0; j < size; ++ j)
    {
      point[i * size + j] = collection_[j].getX()[i];
    }
  }
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    point[dimension * size + i] = collection_[i].getP();
  }
  return point;
}

/* Parameters description accessor */
Description UserDefined::getParameterDescription() const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger size = collection_.getSize();
  Description description((dimension + 1) * size);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    for (UnsignedInteger j = 0; j < size; ++ j)
    {
      description[i * size + j] = (OSS() << "X^" << i << "_" << j);
    }
  }
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    description[dimension * size + i] = (OSS() << "p_" << i);
  }
  return description;
}


/* Get the i-th marginal distribution */
UserDefined::Implementation UserDefined::getMarginal(const UnsignedInteger i) const
{
  const UnsignedInteger dimension(getDimension());
  if (i >= dimension) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // General case
  const UnsignedInteger size(collection_.getSize());
  UserDefinedPairCollection coll(size);
  for (UnsignedInteger j = 0; j < size; ++j)
  {
    coll[j].setX(NumericalPoint(1, collection_[j].getX()[i]));
    coll[j].setP(collection_[j].getP());
  }
  return new UserDefined(coll);
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
UserDefined::Implementation UserDefined::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension(getDimension());
  if (!indices.check(dimension - 1)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and  must be different";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // General case
  const UnsignedInteger outputDimension(indices.getSize());
  const UnsignedInteger size(collection_.getSize());
  UserDefinedPairCollection coll(size);
  for (UnsignedInteger j = 0; j < size; ++j)
  {
    NumericalPoint x(outputDimension);
    NumericalPoint xIni(collection_[j].getX());
    for (UnsignedInteger k = 0; k < outputDimension; ++k) x[k] = xIni[indices[k]];
    coll[j].setX(x);
    coll[j].setP(collection_[j].getP());
  }
  return new UserDefined(coll);
} // getMarginal(Indices)

/* Interface specific to UserDefined */

/* Pair collection accessor */
void UserDefined::setPairCollection(const UserDefinedPairCollection & collection)
{
  const UnsignedInteger size(collection.getSize());
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: the collection is empty";
  hasUniformWeights_ = true;
  const UnsignedInteger dimension(collection[0].getX().getDimension());
  if (dimension == 0) throw InvalidArgumentException(HERE) << "Error: the points in the collection must have a dimension > 0";
  // Check if all the given probabilities are >= 0
  // Check if all the points have the same dimension
  for (UnsignedInteger i = 1; i < size; ++i) if (collection[i].getX().getDimension() != dimension) throw InvalidArgumentException(HERE) << "UserDefined distribution must have all its point with the same dimension, which is not the case here collection=" << collection;
  setDimension(dimension);
  // First, sort the collection such that the sample made with the first component is in ascending order
  NumericalSample weightedData(size, dimension + 1);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalPoint x(collection[i].getX());
    for (UnsignedInteger j = 0; j < dimension; ++j) weightedData[i][j] = x[j];
    weightedData[i][dimension] = collection[i].getP();
  }
  // Sort the pairs
  weightedData = weightedData.sortAccordingToAComponent(0);
  // Check the probabilities and normalize them
  const NumericalScalar firstProbability(weightedData[0][dimension]);
  NumericalScalar sum(0.0);
  cumulativeProbabilities_ = NumericalPoint(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalScalar p(weightedData[i][dimension]);
    if (p < 0.0) throw InvalidArgumentException(HERE) << "UserDefined distribution must have positive probabilities, which is not the case here collection=" << collection;
    sum += p;
    cumulativeProbabilities_[i] = sum;
    hasUniformWeights_ = hasUniformWeights_ && (std::abs(p - firstProbability) < pdfEpsilon_);
  }
  if (sum < pdfEpsilon_) throw InvalidArgumentException(HERE) << "Error: the sum of probabilities is zero.";
  // Normalize the probabilities
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    weightedData[i][dimension] /= sum;
    cumulativeProbabilities_[i] /= sum;
  }
  collection_ = UserDefinedPairCollection(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    NumericalPoint x(dimension);
    for (UnsignedInteger j = 0; j < dimension; ++j) x[j] = weightedData[i][j];
    collection_[i] = UserDefinedPair(x, std::max(0.0, std::min(1.0, weightedData[i][dimension])));
  }
  // We augment slightly the last cumulative probability, which should be equal to 1.0 but we enforce a value > 1.0. It stabilizes the sampling procedures without affecting their correctness (i.e. the algoritms are exact, not approximative)
  cumulativeProbabilities_[size - 1] = 1.0 + 2.0 * supportEpsilon_;
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  isAlreadyCreatedGeneratingFunction_ = false;
  computeRange();
}

UserDefined::UserDefinedPairCollection UserDefined::getPairCollection() const
{
  return collection_;
}

/* Quantile computation for dimension=1 */
NumericalScalar UserDefined::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  UnsignedInteger index(0);
  const NumericalScalar p(tail ? 1 - prob : prob);
  while (cumulativeProbabilities_[index] < p) ++index;
  return collection_[index].getX()[0];
}

/* Merge the identical points of the support */
void UserDefined::compactSupport(const NumericalScalar epsilon)
{
  // No compaction if epsilon is negative
  if (epsilon < 0.0) return;
  const UnsignedInteger size(collection_.getSize());
  if (size == 0) return;
  const UnsignedInteger dimension(getDimension());
  UserDefinedPairCollection compactCollection(0);
  if (dimension > 1)
  {
    // Build a hash table with rounded components
    const UnsignedInteger hashSize(511);
    Collection<Indices> indices(hashSize);
    Collection<Indices> residuals(hashSize);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const NumericalPoint x(collection_[i].getX());
      NumericalScalar roundedComponent(x[0]);
      if (epsilon > 0.0) roundedComponent = epsilon * round(roundedComponent / epsilon);
      UnsignedInteger index(*reinterpret_cast<Unsigned64BitsInteger*>(&roundedComponent));
      for (UnsignedInteger j = 1; j < dimension; ++j)
      {
        roundedComponent = x[j];
        if (epsilon > 0.0) roundedComponent = epsilon * round(roundedComponent / epsilon);
        // XOR based hash function on the binary representation of the floating point coordinates
        index ^= *reinterpret_cast<Unsigned64BitsInteger*>(&roundedComponent);
      }
      const UnsignedInteger hash(index % hashSize);
      const UnsignedInteger quotient(index / hashSize);
      indices[hash].add(i);
      residuals[hash].add(quotient);
    }
    // Now, we just have to inspect the points with the same hash
    for (UnsignedInteger i = 0; i < hashSize; ++i)
    {
      const Indices bucket(indices[i]);
      const Indices keys(residuals[i]);
      const UnsignedInteger bucketSize(bucket.getSize());
      if (bucketSize == 0) continue;
      if (bucketSize == 1) compactCollection.add(collection_[bucket[0]]);
      else
      {
        // New weights for the points in the bucket
        NumericalPoint weights(bucketSize);
        // Here we have to detect and merge all the duplicates
        // The ith atom is removed if flagToRemove[i] > 0
        Indices flagToRemove(bucketSize, 0);
        for (UnsignedInteger j = 0; j < bucketSize; ++j)
        {
          const UnsignedInteger currentIndex(bucket[j]);
          const UnsignedInteger currentKey(keys[j]);
          weights[j] = collection_[currentIndex].getP();
          const NumericalPoint current(collection_[currentIndex].getX());
          if (flagToRemove[j] == 0)
          {
            for (UnsignedInteger k = j + 1; k < bucketSize; ++k)
            {
              const UnsignedInteger candidateIndex(bucket[k]);
              const UnsignedInteger candidateKey(keys[k]);
              const NumericalPoint candidate(collection_[candidateIndex].getX());
              if ((currentKey == candidateKey) && (current - candidate).norm() <= epsilon)
              {
                flagToRemove[k] = 1;
                weights[j] += collection_[candidateIndex].getP();
              }
            } // k
          } // flagToRemove
        } // j
        // We keep all the points that must not be removed
        for (UnsignedInteger j = 0; j < bucketSize; ++j)
        {
          if (flagToRemove[j] == 0) compactCollection.add(UserDefinedPair(collection_[bucket[j]].getX(), weights[j]));
        }
      } // bucketSize > 1
    } // Loop over the hash table
    setPairCollection(compactCollection);
    return;
  }
  NumericalScalar lastLocation(collection_[0].getX()[0]);
  NumericalScalar lastWeight(collection_[0].getP());
  for (UnsignedInteger i = 1; i < size; ++i)
  {
    const NumericalScalar currentLocation(collection_[i].getX()[0]);
    const NumericalScalar currentWeight(collection_[i].getP());
    // The current point must be merged
    if (std::abs(currentLocation - lastLocation) <= epsilon) lastWeight += collection_[i].getP();
    else
    {
      compactCollection.add(UserDefinedPair(NumericalPoint(1, lastLocation), std::max(0.0, std::min(1.0, lastWeight))));
      lastLocation = currentLocation;
      lastWeight = currentWeight;
    }
  }
  compactCollection.add(UserDefinedPair(NumericalPoint(1, lastLocation), std::max(0.0, std::min(1.0, lastWeight))));
  setPairCollection(compactCollection);
}

/* Tell if the distribution has an elliptical copula */
Bool UserDefined::hasEllipticalCopula() const
{
  return collection_.getSize() == 1;
}


/* Tell if the distribution has independent copula */
Bool UserDefined::hasIndependentCopula() const
{
  return collection_.getSize() == 1;
}


/* Method save() stores the object through the StorageManager */
void UserDefined::save(Advocate & adv) const
{
  DiscreteDistribution::save(adv);
  adv.saveAttribute( "collection_", collection_ );
  adv.saveAttribute( "cumulativeProbabilities_", cumulativeProbabilities_ );
  adv.saveAttribute( "hasUniformWeights_", hasUniformWeights_ );
}

/* Method load() reloads the object from the StorageManager */
void UserDefined::load(Advocate & adv)
{
  DiscreteDistribution::load(adv);
  adv.loadAttribute( "collection_", collection_ );
  adv.loadAttribute( "cumulativeProbabilities_", cumulativeProbabilities_ );
  adv.loadAttribute( "hasUniformWeights_", hasUniformWeights_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
