//                                               -*- C++ -*-
/**
 *  @brief The UserDefined distribution
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
#include <cstdlib>
#include <cmath>
#include "openturns/UserDefined.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(UserDefined)
static const Factory<UserDefined> Factory_UserDefined;


/* Default constructor */
UserDefined::UserDefined()
  : DiscreteDistribution()
  , points_(1, 1)
  , probabilities_(1, 1.0)
  , cumulativeProbabilities_(1, 1.0)
  , hasUniformWeights_(true)
  , base_(0)
  , alias_(0)
{
  setName("UserDefined");
  setData(Sample(1, 1), Point(1, 1.0));
}

/* Constructor from a sample */
UserDefined::UserDefined(const Sample & sample)
  : DiscreteDistribution()
  , points_(0, 0)
  , probabilities_(0)
  , cumulativeProbabilities_(0)
  , hasUniformWeights_(true)
  , base_(0)
  , alias_(0)
{
  setName("UserDefined");
  const UnsignedInteger size = sample.getSize();
  // We set the dimension of the UserDefined distribution
  // This call set also the range
  setData(sample, Point(size, 1.0 / size));
  if ((getDimension() == 1) || (sample.getSize() <= ResourceMap::GetAsUnsignedInteger("UserDefined-SmallSize"))) compactSupport();
  if(!sample.getDescription().isBlank()) setDescription(sample.getDescription());
}

/* Constructor from a sample and the associated weights */
UserDefined::UserDefined(const Sample & sample,
                         const Point & weights)
  : DiscreteDistribution()
  , points_(0, 0)
  , probabilities_(0)
  , cumulativeProbabilities_(0)
  , hasUniformWeights_(false)
  , base_(0)
  , alias_(0)
{
  setName("UserDefined");
  // We set the dimension of the UserDefined distribution
  // This call set also the range
  setData(sample, weights);
  if ((getDimension() == 1) || (sample.getSize() <= ResourceMap::GetAsUnsignedInteger("UserDefined-SmallSize"))) compactSupport();
  if(!sample.getDescription().isBlank()) setDescription(sample.getDescription());
}

/* Comparison operator */
Bool UserDefined::operator ==(const UserDefined & other) const
{
  if (this == &other) return true;
  return (points_ == other.points_) && (probabilities_ == other.probabilities_);
}

Bool UserDefined::equals(const DistributionImplementation & other) const
{
  const UserDefined* p_other = dynamic_cast<const UserDefined*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String UserDefined::__repr__() const
{
  OSS oss;
  oss << "class=" << UserDefined::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " points=" << points_
      << " probabilities=" << probabilities_;
  return oss;
}

String UserDefined::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(";
  String separator("");
  for (UnsignedInteger i = 0; i < points_.getSize(); ++i)
  {
    oss << separator << "{x = " << Point(points_[i]).__str__() << ", p = " << probabilities_[i] << "}";
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
Point UserDefined::getRealization() const
{
  const UnsignedInteger size = points_.getSize();
  UnsignedInteger index = 0;
  // Efficient algorithm for uniform weights
  if (hasUniformWeights_) index = RandomGenerator::IntegerGenerate(size);
  // Alias method for nonuniform weights
  else
  {
    index = base_.getSize() ? DistFunc::rDiscrete(base_, alias_) : DistFunc::rDiscrete(probabilities_, base_, alias_);
  }
  return points_[index];
}

/* Get a sample of the distribution */
Sample UserDefined::getSample(const UnsignedInteger size) const
{
  const UnsignedInteger supportSize = points_.getSize();
  Indices indices;
  // Efficient algorithm for uniform weights
  if (hasUniformWeights_)
  {
    Collection<UnsignedInteger> values(RandomGenerator::IntegerGenerate(size, supportSize));
    indices = Indices(values.begin(), values.end());
  }
  // Alias method for nonuniform weights
  else
  {
    if (!base_.getSize())
      (void) DistFunc::rDiscrete(probabilities_, base_, alias_);
    indices = DistFunc::rDiscrete(base_, alias_, size);
  }
  return points_.select(indices);
}

/* Get the PDF of the distribution */
Scalar UserDefined::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const UnsignedInteger size = points_.getSize();
  Scalar pdf = 0.0;
  // Quick search for 1D case
  const Scalar x = point[0];
  UnsignedInteger upper = size - 1;
  Scalar xUpper = points_(upper, 0);
  if (x > xUpper + supportEpsilon_) return 0.0;
  UnsignedInteger lower = 0;
  Scalar xLower = points_(lower, 0);
  if (x < xLower - supportEpsilon_) return 0.0;
  // Use bisection search of the correct index
  while (upper - lower > 1)
  {
    // The integer arithmetic ensure that middle will be strictly between lower and upper as far as upper - lower > 1
    const UnsignedInteger middle = (upper + lower) / 2;
    const Scalar xMiddle = points_(middle, 0);
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
  SignedInteger index = upper;
  while ((index < static_cast<SignedInteger>(size)) && (std::abs(x - points_(index, 0)) <= supportEpsilon_))
  {
    if ((point - points_[index]).norm() <= supportEpsilon_) pdf += probabilities_[index];
    ++ index;
  }
  index = upper;
  --index;
  while ((index >= 0) && (std::abs(x - points_(index, 0)) <= supportEpsilon_))
  {
    if ((point - points_[index]).norm() <= supportEpsilon_) pdf += probabilities_[index];
    --index;
  }
  return pdf;
}

/* Get the CDF of the distribution */
Scalar UserDefined::computeCDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  Scalar cdf = 0.0;
  const UnsignedInteger size = points_.getSize();
  const UnsignedInteger dimension = getDimension();
  // Quick search for 1D case
  if (dimension == 1)
  {
    const Scalar x = point[0];
    UnsignedInteger upper = size - 1;
    Scalar xUpper = points_(upper, 0);
    if (x > xUpper - supportEpsilon_) return 1.0;
    UnsignedInteger lower = 0;
    Scalar xLower = points_(lower, 0);
    if (x <= xLower - supportEpsilon_) return 0.0;
    // Use dichotomic search of the correct index
    while (upper - lower > 1)
    {
      // The integer arithmetic insure that middle will be strictly between lower and upper as far as upper - lower > 1
      const UnsignedInteger middle = (upper + lower) / 2;
      const Scalar xMiddle = points_(middle, 0);
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
    const Point x(points_[i]);
    UnsignedInteger j = 0;
    while ((j < dimension) && (x[j] <= point[j] + supportEpsilon_)) ++j;
    if (j == dimension) cdf += probabilities_[i];
  }
  return cdf;
}

/* Get the PDF gradient of the distribution */
Point UserDefined::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  const UnsignedInteger size = points_.getSize();
  Point pdfGradient(size, 0.0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if ((point - points_[i]).norm() < supportEpsilon_)
    {
      pdfGradient[i] = 1.0;
      return pdfGradient;
    }
  }
  return pdfGradient;
}


/* Get the CDF gradient of the distribution */
Point UserDefined::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  const UnsignedInteger size = points_.getSize();
  const UnsignedInteger dimension = getDimension();
  Point cdfGradient(size, 0.0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Point x(points_[i]);
    UnsignedInteger j = 0;
    while ((j < dimension) && (point[j] <= x[j])) ++j;
    if (j == dimension) cdfGradient[i] = 1.0;
  }
  return cdfGradient;
}

/* Compute the numerical range of the distribution given the parameters values */
void UserDefined::computeRange()
{
  const UnsignedInteger size = points_.getSize();
  const UnsignedInteger dimension = getDimension();
  // Return an empty interval for the empty collection case
  if (size == 0)
  {
    setRange(Interval(Point(dimension, 1.0), Point(dimension, 0.0)));
    return;
  }
  // The number of points is finite, so are the bounds
  const Interval::BoolCollection finiteLowerBound(dimension, true);
  const Interval::BoolCollection finiteUpperBound(dimension, true);
  Point lowerBound(points_[0]);
  Point upperBound(lowerBound);
  for (UnsignedInteger i = 1; i < size; ++i)
  {
    const Point pt(points_[i]);
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      const Scalar x = pt[j];
      if (x < lowerBound[j]) lowerBound[j] = x;
      if (x > upperBound[j]) upperBound[j] = x;
    }
  }
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Get the support of a discrete distribution that intersect a given interval */
Sample UserDefined::getSupport(const Interval & interval) const
{
  if (interval.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given interval has a dimension that does not match the distribution dimension.";
  Sample result(0, getDimension());
  const UnsignedInteger size = points_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Point x(points_[i]);
    if (interval.contains(x)) result.add(x);
  }
  return result;
}

/* Tell if the distribution is integer valued */
Bool UserDefined::isIntegral() const
{
  if (getDimension() != 1) return false;
  const UnsignedInteger size = points_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar x = points_(i, 0);
    if (std::abs(x - round(x)) >= supportEpsilon_) return false;
  }
  return true;
}

/* Compute the mean of the distribution */
void UserDefined::computeMean() const
{
  const UnsignedInteger size = points_.getSize();
  Point mean(getDimension());
  for (UnsignedInteger i = 0; i < size; ++i) mean += probabilities_[i] * points_[i];
  mean_ = mean;
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void UserDefined::computeCovariance() const
{
  const UnsignedInteger size = points_.getSize();
  const UnsignedInteger dimension = getDimension();
  covariance_ = CovarianceMatrix(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) covariance_(i, i) = 0.0;
  const Point mean(getMean());
  for (UnsignedInteger k = 0; k < size; ++k)
  {
    const Point xK(points_[k] - mean);
    const Scalar pK = probabilities_[k];
    for (UnsignedInteger i = 0; i < dimension; ++i)
      for (UnsignedInteger j = 0; j <= i; ++j)
        covariance_(i, j) += pK * xK[i] * xK[j];
  }
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value and description accessor */
UserDefined::PointWithDescriptionCollection UserDefined::getParametersCollection() const
{
  const UnsignedInteger dimension = getDimension();
  PointWithDescriptionCollection parameters(dimension + 1);
  const UnsignedInteger size = points_.getSize();
  // Loop over the dimension to extract the marginal coordinates of the support
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    PointWithDescription point(size);
    Description description(size);
    for (UnsignedInteger j = 0; j < size; ++j)
    {
      point[j] = points_(j, i);
      OSS oss;
      oss << "X^" << i << "_" << j;
      description[j] = oss;
    }
    point.setDescription(description);
    parameters[i] = point;
  }
  // Loop over the size to extract the probabilities, seen as the dependence parameters
  PointWithDescription point(size);
  Description description(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    point[i] = probabilities_[i];
    OSS oss;
    oss << "probabilities_" << i;
    description[i] = oss;
  }
  point.setDescription(description);
  point.setName(getDescription()[0]);
  parameters[dimension] = point;
  return parameters;
}

/* Parameters value accessor */
Point UserDefined::getParameter() const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger size = points_.getSize();
  Point point((dimension + 1) * size);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    for (UnsignedInteger j = 0; j < size; ++ j)
    {
      point[i * size + j] = points_(j, i);
    }
  }
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    point[dimension * size + i] = probabilities_[i];
  }
  return point;
}

/* Parameters description accessor */
Description UserDefined::getParameterDescription() const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger size = points_.getSize();
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
    description[dimension * size + i] = (OSS() << "probabilities_" << i);
  }
  return description;
}

void UserDefined::setParameter(const Point & parameter)
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger size = points_.getSize();
  if (parameter.getSize() != (dimension + 1) * size)
    throw InvalidArgumentException(HERE) << "Expected " << (dimension + 1) * size << " parameters";
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    for (UnsignedInteger j = 0; j < size; ++ j)
    {
      points_(j, i) = parameter[i * size + j];
    }
  }
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    probabilities_[i] = parameter[dimension * size + i];
  }
  setData(points_, probabilities_);
}

/* Get the i-th marginal distribution */
Distribution UserDefined::getMarginal(const UnsignedInteger i) const
{
  const UnsignedInteger dimension = getDimension();
  if (i >= dimension) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // General case
  UserDefined::Implementation marginal(new UserDefined(points_.getMarginal(i), probabilities_));
  marginal->setDescription(Description(1, getDescription()[i]));
  return marginal;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution UserDefined::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // General case
  const UnsignedInteger outputDimension = indices.getSize();
  Description description(getDescription());
  Description marginalDescription(outputDimension);
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
  {
    const UnsignedInteger index_i = indices[i];
    marginalDescription[i] = description[index_i];
  }
  UserDefined::Implementation marginal(new UserDefined(points_.getMarginal(indices), probabilities_));
  marginal->setDescription(marginalDescription);
  return marginal;
} // getMarginal(Indices)

/* Interface specific to UserDefined */

void UserDefined::setData(const Sample & sample,
                          const Point & weights)
{
  const UnsignedInteger size = sample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: the collection is empty";
  if (weights.getDimension() != size) throw InvalidArgumentException(HERE) << "Error: cannot build a UserDefined distribution if the weights don't have the same dimension as the sample size.";
  hasUniformWeights_ = true;
  const UnsignedInteger dimension = sample[0].getDimension();
  if (dimension == 0) throw InvalidArgumentException(HERE) << "Error: the points in the collection must have a dimension > 0";
  // Check if all the given probabilities are >= 0
  // Check if all the points have the same dimension
  for (UnsignedInteger i = 1; i < size; ++i) if (sample[i].getDimension() != dimension) throw InvalidArgumentException(HERE) << "UserDefined distribution must have all its point with the same dimension";
  setDimension(dimension);
  // First, sort the collection such that the sample made with the first component is in ascending order
  Sample weightedData(size, dimension + 1);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Point x(sample[i]);
    for (UnsignedInteger j = 0; j < dimension; ++j) weightedData(i, j) = x[j];
    weightedData(i, dimension) = weights[i];
  }
  // Sort the pairs
  weightedData = weightedData.sortAccordingToAComponent(0);
  // Check the probabilities and normalize them
  const Scalar firstProbability = weightedData(0, dimension);
  Scalar sum = 0.0;
  cumulativeProbabilities_ = Point(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar p = weightedData(i, dimension);
    if (!(p >= 0.0)) throw InvalidArgumentException(HERE) << "UserDefined distribution must have positive probabilities";
    sum += p;
    cumulativeProbabilities_[i] = sum;
    hasUniformWeights_ = hasUniformWeights_ && (std::abs(p - firstProbability) < pdfEpsilon_);
  }
  if (sum <= 0.0) throw InvalidArgumentException(HERE) << "Error: the sum of probabilities is zero.";
  // Normalize the probabilities
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    weightedData(i, dimension) /= sum;
    cumulativeProbabilities_[i] /= sum;
  }
  points_ = Sample(size, dimension);
  probabilities_ = Point(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Point x(dimension);
    for (UnsignedInteger j = 0; j < dimension; ++j) x[j] = weightedData(i, j);
    points_[i] = x;
    probabilities_[i] = std::max(0.0, std::min(1.0, weightedData(i, dimension)));
  }
  // We augment slightly the last cumulative probability, which should be equal to 1.0 but we enforce a value > 1.0.
  cumulativeProbabilities_[size - 1] = 1.0 + 2.0 * supportEpsilon_;
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  isAlreadyCreatedGeneratingFunction_ = false;
  computeRange();
  // trigger reset of alias method
  base_.clear();
  alias_.clear();
}


Sample UserDefined::getX() const
{
  return points_;
}


Point UserDefined::getP() const
{
  return probabilities_;
}


/* Quantile computation for dimension=1 */
Scalar UserDefined::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "Error: the method computeScalarQuantile is only defined for 1D distributions";
  UnsignedInteger index = 0;
  const Scalar p = tail ? 1 - prob : prob;
  while (cumulativeProbabilities_[index] < p) ++index;
  return points_(index, 0);
}

/* Merge the identical points of the support */
void UserDefined::compactSupport(const Scalar epsilon)
{
  // No compaction if epsilon is negative
  if (epsilon < 0.0) return;
  const UnsignedInteger size = points_.getSize();
  if (size == 0) return;
  const UnsignedInteger dimension = getDimension();
  Sample compactX(0, dimension);
  Point compactP(0);
  if (dimension > 1)
  {
    // Build a hash table with rounded components
    const UnsignedInteger hashSize = 511;
    Collection<Indices> indices(hashSize);
    Collection<Indices> residuals(hashSize);
    union
    {
      Scalar *s;
      Unsigned64BitsInteger *u64;
    } typepun;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Point x(points_[i]);
      Scalar roundedComponent = x[0];
      if (epsilon > 0.0) roundedComponent = epsilon * round(roundedComponent / epsilon);
      typepun.s = &roundedComponent;
      UnsignedInteger index = *typepun.u64;
      for (UnsignedInteger j = 1; j < dimension; ++j)
      {
        roundedComponent = x[j];
        if (epsilon > 0.0) roundedComponent = epsilon * round(roundedComponent / epsilon);
        // XOR based hash function on the binary representation of the floating point coordinates
        typepun.s = &roundedComponent;
        index ^= *typepun.u64;
      }
      const UnsignedInteger hash = index % hashSize;
      const UnsignedInteger quotient = index / hashSize;
      indices[hash].add(i);
      residuals[hash].add(quotient);
    }
    // Now, we just have to inspect the points with the same hash
    for (UnsignedInteger i = 0; i < hashSize; ++i)
    {
      const Indices bucket(indices[i]);
      const Indices keys(residuals[i]);
      const UnsignedInteger bucketSize = bucket.getSize();
      if (bucketSize == 0) continue;
      if (bucketSize == 1)
      {
        compactX.add(points_[bucket[0]]);
        compactP.add(probabilities_[bucket[0]]);
      }
      else
      {
        // New weights for the points in the bucket
        Point weights(bucketSize);
        // Here we have to detect and merge all the duplicates
        // The ith atom is removed if flagToRemove[i] > 0
        Indices flagToRemove(bucketSize, 0);
        for (UnsignedInteger j = 0; j < bucketSize; ++j)
        {
          const UnsignedInteger currentIndex = bucket[j];
          const UnsignedInteger currentKey = keys[j];
          weights[j] = probabilities_[currentIndex];
          const Point current(points_[currentIndex]);
          if (flagToRemove[j] == 0)
          {
            for (UnsignedInteger k = j + 1; k < bucketSize; ++k)
            {
              const UnsignedInteger candidateIndex = bucket[k];
              const UnsignedInteger candidateKey = keys[k];
              const Point candidate(points_[candidateIndex]);
              if ((currentKey == candidateKey) && (current - candidate).norm() <= epsilon)
              {
                flagToRemove[k] = 1;
                weights[j] += probabilities_[candidateIndex];
              }
            } // k
          } // flagToRemove
        } // j
        // We keep all the points that must not be removed
        for (UnsignedInteger j = 0; j < bucketSize; ++j)
        {
          if (flagToRemove[j] == 0)
          {
            compactX.add(points_[bucket[j]]);
            compactP.add(weights[j]);
          }
        }
      } // bucketSize > 1
    } // Loop over the hash table
    setData(compactX, compactP);
    return;
  }
  Scalar lastLocation = points_(0, 0);
  Scalar lastWeight = probabilities_[0];
  for (UnsignedInteger i = 1; i < size; ++i)
  {
    const Scalar currentLocation = points_(i, 0);
    const Scalar currentWeight = probabilities_[i];
    // The current point must be merged
    if (std::abs(currentLocation - lastLocation) <= epsilon) lastWeight += probabilities_[i];
    else
    {
      compactX.add(Point(1, lastLocation));
      compactP.add(std::max(0.0, std::min(1.0, lastWeight)));
      lastLocation = currentLocation;
      lastWeight = currentWeight;
    }
  }
  compactX.add(Point(1, lastLocation));
  compactP.add(std::max(0.0, std::min(1.0, lastWeight)));
  setData(compactX, compactP);
}

/* Tell if the distribution has an elliptical copula */
Bool UserDefined::hasEllipticalCopula() const
{
  return points_.getSize() == 1;
}


/* Tell if the distribution has independent copula */
Bool UserDefined::hasIndependentCopula() const
{
  return points_.getSize() == 1;
}


/* Method save() stores the object through the StorageManager */
void UserDefined::save(Advocate & adv) const
{
  DiscreteDistribution::save(adv);
  adv.saveAttribute( "points_", points_ );
  adv.saveAttribute( "probabilities_", probabilities_ );
  adv.saveAttribute( "cumulativeProbabilities_", cumulativeProbabilities_ );
  adv.saveAttribute( "hasUniformWeights_", hasUniformWeights_ );
}

/* Method load() reloads the object from the StorageManager */
void UserDefined::load(Advocate & adv)
{
  DiscreteDistribution::load(adv);
  adv.loadAttribute( "points_", points_ );
  adv.loadAttribute( "probabilities_", probabilities_ );
  adv.loadAttribute( "cumulativeProbabilities_", cumulativeProbabilities_ );
  adv.loadAttribute( "hasUniformWeights_", hasUniformWeights_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
