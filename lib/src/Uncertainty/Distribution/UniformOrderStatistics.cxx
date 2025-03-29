//                                               -*- C++ -*-
/**
 *  @brief The UniformOrderStatistics distribution
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Indices.hxx"
#include "openturns/UniformOrderStatistics.hxx"
#include "openturns/MarginalUniformOrderStatistics.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Beta.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(UniformOrderStatistics)

static const Factory<UniformOrderStatistics> Factory_UniformOrderStatistics;


/* Parameters constructor */
UniformOrderStatistics::UniformOrderStatistics(const UnsignedInteger dimension)
  : DistributionImplementation()
{
  setName("UniformOrderStatistics");
  setDimension(dimension);
  computeRange();
  logNormalization_ = SpecFunc::LogGamma(dimension + 1);
  vertices_ = Sample(dimension + 1, dimension);
  for (UnsignedInteger i = 0; i <= dimension; ++i)
    for (UnsignedInteger j = i; j < dimension; ++j)
      vertices_(i, j) = 1.0;
  IndicesCollection simplices(1, dimension + 1);
  for (UnsignedInteger i = 0; i <= dimension; ++i)
    simplices(0, i) = i;
  simplex_ = MeshDomain(Mesh(vertices_, simplices));
}

/* Comparison operator */
Bool UniformOrderStatistics::operator ==(const UniformOrderStatistics & other) const
{
  if (this == &other) return true;
  return vertices_ == other.vertices_;
}

Bool UniformOrderStatistics::equals(const DistributionImplementation & other) const
{
  const UniformOrderStatistics* p_other = dynamic_cast<const UniformOrderStatistics*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String UniformOrderStatistics::__repr__() const
{
  OSS oss(true);
  oss << "class=" << UniformOrderStatistics::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " simplex=" << simplex_;
  return oss;
}

String UniformOrderStatistics::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(dimension = " << dimension_ << ")";
  return oss;
}

/* Virtual constructor */
UniformOrderStatistics * UniformOrderStatistics::clone() const
{
  return new UniformOrderStatistics(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void UniformOrderStatistics::computeRange()
{
  setRange(Interval(dimension_));
}

/* Get one realization of the distribution. */
Point UniformOrderStatistics::getRealization() const
{
  return DistFunc::rUniformSimplex(vertices_);
}

/* Get the PDF of the distribution */
Scalar UniformOrderStatistics::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  if (!simplex_.contains(point))
    return 0.0;
  return std::exp(logNormalization_);
}

Scalar UniformOrderStatistics::computeLogPDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  if (!simplex_.contains(point))
    return SpecFunc::LowestScalar;
  return logNormalization_;
}

namespace
{
// The free functions are embedded into an anonymous namespace and are prefixed by the class name,
// see Mesh.cxx
Scalar UniformOverMesh_exploreTree(const UnsignedInteger k,
                                   const UnsignedInteger lower,
                                   const UnsignedInteger upper,
                                   const UnsignedInteger count,
                                   const Point & xReduced)
{
  // Upper branch of the tree
  const UnsignedInteger lower1 = lower;
  const UnsignedInteger upper1 = upper;
  // The value lower1 == dimension is a guard, telling us that the lower bound is 0 and not a component of xReduced
  const Scalar a1 = (lower1 < xReduced.getDimension()) ? xReduced[lower1] : 0.0;
  const Scalar b1 = xReduced[upper1];
  const UnsignedInteger count1 = count + 1;
  const Scalar f1 = (b1 - a1) / count1;
  // Lower branch of the tree
  const UnsignedInteger lower2 = upper;
  const UnsignedInteger upper2 = k;
  const Scalar a2 = xReduced[lower2];
  const Scalar b2 = xReduced[upper2];
  const UnsignedInteger count2 = 1;
  const Scalar f2 = b2 - a2;
  if (k == xReduced.getDimension() - 1)
    return (k + 1) * (f1 + f2);
  Scalar value = 0.0;
  // This test allows one to cut upper parts of the tree
  if (f1 > 0.0)
    value += f1 * UniformOverMesh_exploreTree(k + 1, lower1, upper1, count1, xReduced);
  // This test allows one to cut lower parts of the tree
  if (f2 > 0.0)
    value += f2 * UniformOverMesh_exploreTree(k + 1, upper, k, count2, xReduced);
  return value * (k + 1);
}
} // Anonymous namespace

/* Get the CDF of the distribution */
Scalar UniformOrderStatistics::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  // Special case in dimension 1
  if (dimension == 1)
  {
    if (point[0] <= 0.0) return 0.0;
    if (point[0] >= 1.0) return 1.0;
    return point[0];
  }

  // First, reduction of the argument
  Point xReduced(dimension_);
  xReduced[dimension_ - 1] = std::min(point[dimension_ - 1], 1.0);
  if (xReduced[dimension_ - 1] <= 0.0)
    return 0.0;
  // Remove the parts of the interval containing no mass
  for (UnsignedInteger i = dimension_ - 1; i > 0; --i)
  {
    xReduced[i - 1] = std::min(point[i - 1], xReduced[i]);
    // If one of the reduced components is not positive, the CDF in zero
    if (xReduced[i - 1] <= 0.0)
      return 0.0;
    // If all the components are equal to one, which is equivalent to the
    // last reduced component being equal to one, then the CDF is one
    if (xReduced[0] == 1.0)
      return 1.0;
  }
  // Explore the tree describing the domain of integration
  // k = 1
  // lower = dimension_ (guard value to tell that the current interval is [0, xReduced[0]])
  // upper = 0
  // count = 1
  return xReduced[0] * UniformOverMesh_exploreTree(1, dimension_, 0, 1, xReduced);
}

/* Get the probability content of an interval */
Scalar UniformOrderStatistics::computeProbability(const Interval & interval) const
{
  // Here we force the use of Poincare's summation formula
  return DistributionImplementation::computeProbabilityGeneral(interval);
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar UniformOrderStatistics::computeConditionalPDF(const Scalar x,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  if (x < 0.0) return 0.0;
  if (x >= 1.0) return 0.0;
  if (conditioningDimension == 0) return std::pow(1.0 - x, dimension_ - 1.0) / dimension_;
  // The conditioning values must be in nondecreasing order
  if (!y.isNonDecreasing()) return 0.0;
  const UnsignedInteger k = y.getDimension();
  const Scalar xKm1 = y[k - 1];
  if (x <= xKm1) return 0.0;
  return (dimension_ - k) * std::pow((1.0 - x) / (1.0 - xKm1), dimension_ - k - 1.0) / (1.0 - xKm1);
}

Point UniformOrderStatistics::computeSequentialConditionalPDF(const Point & x) const
{
  if (x.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute a sequential conditional PDF at a point of dimension=" << x.getDimension() << " not equal to the distribution dimension=" << dimension_;
  Point result(dimension_);
  if (!(x[0] >= 0.0) && (x[0] < 1)) return result;
  result[0] = std::pow(1.0 - x[0], dimension_ - 1.0) / dimension_;
  for (UnsignedInteger k = 1; k < dimension_; ++k)
  {
    // If at one step the components of x are not in nondecreasing order, all the subsequent conditional PDF
    // will be zero
    const Scalar xKm1 = x[k - 1];
    if ((x[k] < xKm1) || (x[k] >= 1.0)) return result;
    result[k] = (dimension_ - k) * std::pow((1.0 - x[k]) / (1.0 - xKm1), dimension_ - k - 1.0) / (1.0 - xKm1);
  }
  return result;
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar UniformOrderStatistics::computeConditionalCDF(const Scalar x,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  if (x < 0.0) return 0.0;
  if (conditioningDimension == 0) return (x >= 1.0 ? 1.0 : 1.0 - std::pow(1.0 - x, dimension_));
  // The conditioning values must be in nondecreasing order
  if (!y.isNonDecreasing()) return 0.0;
  const UnsignedInteger k = y.getDimension();
  const Scalar xKm1 = y[k - 1];
  if (x <= xKm1) return 0.0;
  if (x >= 1.0) return 1.0;
  return 1.0 - std::pow((1.0 - x) / (1.0 - xKm1), dimension_ - k);
}

Point UniformOrderStatistics::computeSequentialConditionalCDF(const Point & x) const
{
  if (x.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute a sequential conditional CDF at a point of dimension=" << x.getDimension() << " not equal to the distribution dimension=" << dimension_;
  Point result(dimension_);
  if (x[0] <= 0) return result;
  result[0] = 1.0 - std::pow(1.0 - x[0], dimension_);
  for (UnsignedInteger k = 1; k < dimension_; ++k)
  {
    // If at one step the components of x are not in nondecreasing order, all the subsequent conditional PDF
    // will be zero
    const Scalar xKm1 = x[k - 1];
    if ((x[k] < xKm1)) return result;
    if (x[k] >= 1.0) result[k] = 1.0;
    result[k] = 1.0 - std::pow((1.0 - x[k]) / (1.0 - xKm1), dimension_ - k);
  }
  return result;
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
Scalar UniformOrderStatistics::computeConditionalQuantile(const Scalar q,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if ((q < 0.0) || (q > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
  if (conditioningDimension == 0) return 1.0 - std::pow(1.0 - q, 1.0 / dimension_);
  // The conditioning values must be in nondecreasing order
  if (!y.isNonDecreasing()) return 0.0;
  const UnsignedInteger k = y.getDimension();
  const Scalar xKm1 = y[k - 1];
  return 1.0 - (1.0 - xKm1) * std::pow(1.0 - q, 1.0 / (dimension_ - k));
}

Point UniformOrderStatistics::computeSequentialConditionalQuantile(const Point & q) const
{
  if (q.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute a sequential conditional PDF at a quantile level vector of dimension=" << q.getDimension() << " not equal to the distribution dimension=" << dimension_;
  if ((q[0] < 0.0) || (q[0] > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
  Point result(dimension_);
  result[0] = 1.0 - std::pow(1.0 - q[0], 1.0 / dimension_);
  for (UnsignedInteger k = 1; k < dimension_; ++k)
  {
    if ((q[k] < 0.0) || (q[k] > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
    result[k] = 1.0 - (1.0 - result[k - 1]) * std::pow(1.0 - q[k], 1.0 / (dimension_ - k));
  }
  return result;
}

/* Compute the mean of the distribution */
void UniformOrderStatistics::computeMean() const
{
  mean_ = Point(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
    mean_[i] =  (i + 1.0) / (dimension_ + 1.0);
  isAlreadyComputedMean_ = true;
}

/* Compute the entropy of the distribution */
Scalar UniformOrderStatistics::computeEntropy() const
{
  return -SpecFunc::LogGamma(dimension_ + 1.0);
}

/* Get the standard deviation of the distribution */
Point UniformOrderStatistics::getStandardDeviation() const
{
  Point sigma(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
    sigma[i] = 1.0 / (dimension_ + 1.0) * std::sqrt((i + 1.0) * (dimension_ - i) / (dimension_ + 2.0));
  return sigma;
}

/* Get the skewness of the distribution */
Point UniformOrderStatistics::getSkewness() const
{
  Point skewness(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
    skewness[i] = 2.0 * (dimension_ - 1.0 - 2.0 * i) / (dimension_ + 3.0) * std::sqrt((dimension_ + 2.0) / ((i + 1.0) * (dimension_ - i)));
  return skewness;
}

/* Get the kurtosis of the distribution */
Point UniformOrderStatistics::getKurtosis() const
{
  Point kurtosis(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
    kurtosis[i] = 3.0 * (dimension_ + 2.0) * (2.0 * (dimension_ + 1.0) * (dimension_ + 1.0) + (i + 1.0) * ((dimension_ + 1.0) - 6.0) * (dimension_ - i)) / ((i + 1.0) * (dimension_ - i) * (3.0 + (dimension_ + 1.0)) * (2.0 + (dimension_ + 1.0)));
  return kurtosis;
}

/* Compute the covariance of the distribution */
void UniformOrderStatistics::computeCovariance() const
{
  covariance_ = CovarianceMatrix(dimension_);
  for (UnsignedInteger j = 0; j < dimension_; ++j)
    for (UnsignedInteger i = 0; i <= j; ++i)
      covariance_(i, j) = (i + 1.0) * (dimension_ - j) / ((dimension_ + 1.0) * (dimension_ + 1.0) * (dimension_ + 2.0));
  isAlreadyComputedCovariance_ = true;
}

/* Get the quantile of the distribution */
Scalar UniformOrderStatistics::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  if (!((prob >= 0.0) && (prob <= 1.0)))
    throw InvalidArgumentException(HERE) << "computeScalarQuantile expected prob to belong to [0,1], but is " << prob;
  if (tail) return 1.0 - prob;
  return prob;
}

/* Get the i-th marginal distribution */
Distribution UniformOrderStatistics::getMarginal(const UnsignedInteger i) const
{
  if (i >= dimension_) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  return getMarginal(Indices(1, i));
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution UniformOrderStatistics::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  if (dimension == 1) return clone();
  if (indices.getSize() == 1)
  {
    const UnsignedInteger i = indices[0];
    Beta marginal(i + 1.0, dimension_ - i, 0.0, 1.0);
    marginal.setDescription(Description(1, getDescription()[i]));
    return marginal;
  }
  // The MarginalUniformOrderStatistics needs increasing indices
  if (indices.isStrictlyIncreasing())
    return MarginalUniformOrderStatistics(dimension_, indices);
  else
    return DistributionImplementation::getMarginal(indices);
} // getMarginal(Indices)

/* Tell if the distribution has independent marginals */
Bool UniformOrderStatistics::hasIndependentCopula() const
{
  return getDimension() == 1;
}

/* Tell if the distribution has an elliptical copula */
Bool UniformOrderStatistics::hasEllipticalCopula() const
{
  return hasIndependentCopula();
}

/* Get the Spearman correlation of the distribution */
CorrelationMatrix UniformOrderStatistics::getSpearmanCorrelation() const
{
  return DistributionImplementation::getSpearmanCorrelation();
}

/* Get the Kendall concordance of the distribution */
CorrelationMatrix UniformOrderStatistics::getKendallTau() const
{
  return DistributionImplementation::getKendallTau();
}

Point UniformOrderStatistics::getParameter() const
{
  return Point();
}

Description UniformOrderStatistics::getParameterDescription() const
{
  return Description();
}

/* Method save() stores the object through the StorageManager */
void UniformOrderStatistics::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "logNormalization_", logNormalization_ );
  adv.saveAttribute( "vertices_", vertices_ );
  adv.saveAttribute( "simplex_", simplex_ );
}

/* Method load() reloads the object from the StorageManager */
void UniformOrderStatistics::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "logNormalization_", logNormalization_ );
  adv.loadAttribute( "vertices_", vertices_ );
  adv.loadAttribute( "simplex_", simplex_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
