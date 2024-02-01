//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all BlockIndependentDistribution
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

#include "openturns/BlockIndependentDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MarginalDistribution.hxx"
#include "openturns/RosenblattEvaluation.hxx"
#include "openturns/InverseRosenblattEvaluation.hxx"
#include "openturns/Log.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/AggregatedFunction.hxx"
#include "openturns/Uniform.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BlockIndependentDistribution)

static const Factory<BlockIndependentDistribution> Factory_BlockIndependentDistribution;

/* Default constructor */
BlockIndependentDistribution::BlockIndependentDistribution()
  : DistributionImplementation()
  , distributionCollection_(0)
{
  setName("BlockIndependentDistribution");
  const DistributionCollection coll(1, Uniform());
  setDistributionCollection(coll);
}

/* Default constructor */
BlockIndependentDistribution::BlockIndependentDistribution(const DistributionCollection & coll)
  : DistributionImplementation()
  , distributionCollection_()
{
  setName("BlockIndependentDistribution");
  // We assign the distribution collection through the accessor in order to compute the block composed distribution dimension
  setDistributionCollection(coll);
}

/* Comparison operator */
Bool BlockIndependentDistribution::operator ==(const BlockIndependentDistribution & other) const
{
  if (this == &other) return true;
  return distributionCollection_ == other.distributionCollection_;
}

Bool BlockIndependentDistribution::equals(const DistributionImplementation & other) const
{
  const BlockIndependentDistribution* p_other = dynamic_cast<const BlockIndependentDistribution*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String BlockIndependentDistribution::__repr__() const
{
  OSS oss(true);
  oss << "class=" << BlockIndependentDistribution::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension();
  for (UnsignedInteger i = 0; i < distributionCollection_.getSize(); ++i)
    oss << " distribution[" << i << "]=" << distributionCollection_[i];
  return oss;
}

String BlockIndependentDistribution::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(";
  String separator("");
  for (UnsignedInteger i = 0; i < distributionCollection_.getSize(); ++i)
  {
    oss << separator << distributionCollection_[i];
    separator = ", ";
  }
  oss << ")";
  return oss;
}

/* Distribution collection accessor */
void BlockIndependentDistribution::setDistributionCollection(const DistributionCollection & coll)
{
  // Check if the collection is not empty
  const UnsignedInteger size = coll.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Collection of distributions is empty";
  distributionCollection_ = coll;
  Description description(0);
  UnsignedInteger dimension = 0;
  // Compute the dimension, build the description and check the independence
  Bool parallel = true;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger distributionDimension = coll[i].getDimension();
    dimension += distributionDimension;
    const Description distributionCollection(coll[i].getDescription());
    for (UnsignedInteger j = 0; j < distributionDimension; ++j) description.add(distributionCollection[j]);
    parallel = parallel && coll[i].getImplementation()->isParallel();
  }
  setParallel(parallel);
  isAlreadyComputedCovariance_ = false;
  // One MUST set the dimension BEFORE the description, else an error occurs
  setDimension(dimension);

  // avoid description warning with identical entries
  Description test(description);
  Description::const_iterator it = std::unique(test.begin(), test.end());
  if (it != test.end())
  {
    description = Description::BuildDefault(dimension_, "X");
  }
  setDescription(description);

  computeRange();
}


/* Distribution collection accessor */
BlockIndependentDistribution::DistributionCollection BlockIndependentDistribution::getDistributionCollection() const
{
  return distributionCollection_;
}

/* Virtual constructor */
BlockIndependentDistribution * BlockIndependentDistribution::clone() const
{
  return new BlockIndependentDistribution(*this);
}

/* Get one realization of the BlockIndependentDistribution */
Point BlockIndependentDistribution::getRealization() const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger size = distributionCollection_.getSize();
  Point result(dimension);
  UnsignedInteger shift = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger distributionDimension = distributionCollection_[i].getDimension();
    const Point realization(distributionCollection_[i].getRealization());
    std::copy(realization.begin(), realization.end(), result.begin() + shift);
    shift += distributionDimension;
  }
  return result;
}

/* Get the DDF of the BlockIndependentDistribution */
Point BlockIndependentDistribution::computeDDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger size = distributionCollection_.getSize();
  // Will store DDF divided by PDF
  Point unscaledDDF(dimension);
  UnsignedInteger shift = 0;
  Scalar pdf = 1.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger distributionDimension = distributionCollection_[i].getDimension();
    Point pointMarg(distributionDimension);
    // Copy the relevant part of the input point to compute the DDF of the block
    std::copy(point.begin() + shift, point.begin() + shift + distributionDimension, pointMarg.begin());
    const Scalar pdfBlock = distributionCollection_[i].computePDF(pointMarg);
    // If the PDF of one block is zero, then the DDF is zero
    if (pdfBlock == 0) return Point(dimension);
    // Aggregate the PDF
    pdf *= pdfBlock;
    const Point scaledDDFBlock(distributionCollection_[i].computeDDF(pointMarg) / pdfBlock);
    std::copy(scaledDDFBlock.begin(), scaledDDFBlock.end(), unscaledDDF.begin() + shift);
    shift += distributionDimension;
  }
  return unscaledDDF * pdf;
}

/* Get the PDF of the BlockIndependentDistribution */
Scalar BlockIndependentDistribution::computePDF(const Point & point) const
{
  /* PDF = PDF_1x...xPDF_n */
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  const UnsignedInteger size = distributionCollection_.getSize();
  Scalar productPDF = 1.0;
  UnsignedInteger shift = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger distributionDimension = distributionCollection_[i].getDimension();
    Point pointMarg(distributionDimension);
    // Copy the relevant part of the input point to compute the DDF of the block
    std::copy(point.begin() + shift, point.begin() + shift + distributionDimension, pointMarg.begin());
    const Scalar blockPDF = distributionCollection_[i].computePDF(pointMarg);
    if (blockPDF == 0.0) return 0.0;
    productPDF *= blockPDF;
    shift += distributionDimension;
  }
  return productPDF;
}

/* Get the log-PDF of the BlockIndependentDistribution */
Scalar BlockIndependentDistribution::computeLogPDF(const Point & point) const
{
  /* PDF = PDF_1x...xPDF_n */
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  const UnsignedInteger size = distributionCollection_.getSize();
  Scalar sumLogPDF = 0.0;
  UnsignedInteger shift = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger distributionDimension = distributionCollection_[i].getDimension();
    Point pointMarg(distributionDimension);
    // Copy the relevant part of the input point to compute the DDF of the block
    std::copy(point.begin() + shift, point.begin() + shift + distributionDimension, pointMarg.begin());
    const Scalar blockLogPDF = distributionCollection_[i].computeLogPDF(pointMarg);
    sumLogPDF += blockLogPDF;
    shift += distributionDimension;
  }
  return sumLogPDF;
}

/* Get the CDF of the BlockIndependentDistribution */
Scalar BlockIndependentDistribution::computeCDF(const Point & point) const
{
  /* CDF = CDF_1x...xCDF_n */
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  const UnsignedInteger size = distributionCollection_.getSize();
  Scalar productCDF = 1.0;
  UnsignedInteger shift = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger distributionDimension = distributionCollection_[i].getDimension();
    Point pointMarg(distributionDimension);
    // Copy the relevant part of the input point to compute the DDF of the block
    std::copy(point.begin() + shift, point.begin() + shift + distributionDimension, pointMarg.begin());
    productCDF *= distributionCollection_[i].computeCDF(pointMarg);
    if (productCDF == 0.0) return 0.0;
    shift += distributionDimension;
  }
  return productCDF;
}

/* Compute the probability content of an interval */
Scalar BlockIndependentDistribution::computeProbability(const Interval & interval) const
{
  const UnsignedInteger dimension = getDimension();
  if (interval.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given interval must have dimension=" << dimension << ", here dimension=" << interval.getDimension();
  const UnsignedInteger size = distributionCollection_.getSize();
  const Point lowerBound(interval.getLowerBound());
  const Point upperBound(interval.getUpperBound());
  Scalar productProbability = 1.0;
  UnsignedInteger shift = 0;

  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger distributionDimension = distributionCollection_[i].getDimension();
    Point lowerBoundMarg(distributionDimension);
    // Copy the relevant part of the lower bound to compute the DDF of the block
    std::copy(lowerBound.begin() + shift, lowerBound.begin() + shift + distributionDimension, lowerBoundMarg.begin());
    Point upperBoundMarg(distributionDimension);
    // Copy the relevant part of the upper bound to compute the DDF of the block
    std::copy(upperBound.begin() + shift, upperBound.begin() + shift + distributionDimension, upperBoundMarg.begin());
    productProbability *= distributionCollection_[i].computeProbability(Interval(lowerBoundMarg, upperBoundMarg));
    if (productProbability == 0.0) return 0.0;
    shift += distributionDimension;
  }
  return productProbability;
}

/* Get the survival function of the distribution */
Scalar BlockIndependentDistribution::computeSurvivalFunction(const Point & point) const
{
  /* CDF = CDF_1x...xCDF_n */
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  const UnsignedInteger size = distributionCollection_.getSize();
  Scalar productSurvival = 1.0;
  UnsignedInteger shift = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger distributionDimension = distributionCollection_[i].getDimension();
    Point pointMarg(distributionDimension);
    // Copy the relevant part of the input point to compute the DDF of the block
    std::copy(point.begin() + shift, point.begin() + shift + distributionDimension, pointMarg.begin());
    const Scalar survivalBlock = distributionCollection_[i].computeSurvivalFunction(pointMarg);
    productSurvival *= survivalBlock;
    if (productSurvival == 0.0) return 0.0;
    shift += distributionDimension;
  }
  return productSurvival;
}

/* Get the Spearman correlation of the distribution */
CorrelationMatrix BlockIndependentDistribution::getSpearmanCorrelation() const
{
  const UnsignedInteger dimension = getDimension();
  CorrelationMatrix spearman(dimension);
  const UnsignedInteger size = distributionCollection_.getSize();
  UnsignedInteger globalIndex = 0;
  for (UnsignedInteger n = 0; n < size; ++n)
  {
    const CorrelationMatrix localSpearman(distributionCollection_[n].getSpearmanCorrelation());
    const UnsignedInteger localSize = localSpearman.getDimension();
    for (UnsignedInteger i = 0; i < localSize; ++i)
      for (UnsignedInteger j = 0; j < i; ++j)
        spearman(globalIndex + i, globalIndex + j) = localSpearman(i, j);
    globalIndex += localSize;
  }
  return spearman;
}

/* Get the Kendall concordance of the distribution */
CorrelationMatrix BlockIndependentDistribution::getKendallTau() const
{
  const UnsignedInteger dimension = getDimension();
  CorrelationMatrix tau(dimension);
  const UnsignedInteger size = distributionCollection_.getSize();
  UnsignedInteger globalIndex = 0;
  for (UnsignedInteger n = 0; n < size; ++n)
  {
    const CorrelationMatrix localTau(distributionCollection_[n].getKendallTau());
    const UnsignedInteger localSize = localTau.getDimension();
    for (UnsignedInteger i = 0; i < localSize; ++i)
      for (UnsignedInteger j = 0; j < i; ++j)
        tau(globalIndex + i, globalIndex + j) = localTau(i, j);
    globalIndex += localSize;
  }
  return tau;
}

/* Compute the conditioning dimension of the active block */
UnsignedInteger BlockIndependentDistribution::computePartialDimension(const UnsignedInteger conditioningDimension,
    UnsignedInteger & distributionIndex) const
{
  distributionIndex = 0;
  UnsignedInteger partialDimension = distributionCollection_[distributionIndex].getDimension();
  while (partialDimension < conditioningDimension)
  {
    ++distributionIndex;
    partialDimension += distributionCollection_[distributionIndex].getDimension();
  }
  return partialDimension;
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar BlockIndependentDistribution::computeConditionalPDF(const Scalar x, const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return getMarginal(y.getDimension()).computePDF(x);
  // General case
  UnsignedInteger distributionIndex;
  const UnsignedInteger partialDimension = computePartialDimension(conditioningDimension, distributionIndex);
  // Extract the relevant part of the conditioning
  const UnsignedInteger conditioningSize = partialDimension - conditioningDimension;
  Point conditioningVector(conditioningSize);
  for (UnsignedInteger i = 0; i < conditioningSize; ++i) conditioningVector[i] = y[conditioningDimension - conditioningSize + i];
  return distributionCollection_[distributionIndex].computeConditionalPDF(x, conditioningVector);
}

Point BlockIndependentDistribution::computeSequentialConditionalPDF(const Point & x) const
{
  if (x.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute sequential conditional PDF with an argument of dimension=" << x.getDimension() << " different from distribution dimension=" << dimension_;
  Point result(dimension_);
  if (hasIndependentCopula())
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      result[i] = getMarginal(i).computePDF(x[i]);
  else
  {
    const UnsignedInteger size = distributionCollection_.getSize();
    UnsignedInteger start = 0;
    UnsignedInteger stop = 0;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const UnsignedInteger localDimension = distributionCollection_[i].getDimension();
      Point localX(localDimension);
      stop += localDimension;
      std::copy(x.begin() + start, x.begin() + stop, localX.begin());
      const Point localResult(distributionCollection_[i].computeSequentialConditionalPDF(localX));
      std::copy(localResult.begin(), localResult.end(), result.begin() + start);
      start = stop;
    } // i
  } // else
  return result;
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar BlockIndependentDistribution::computeConditionalCDF(const Scalar x, const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return getMarginal(y.getDimension()).computeCDF(x);
  // General case
  UnsignedInteger distributionIndex;
  const UnsignedInteger partialDimension = computePartialDimension(conditioningDimension, distributionIndex);
  // Extract the relevant part of the conditioning
  const UnsignedInteger conditioningSize = partialDimension - conditioningDimension;
  Point conditioningVector(conditioningSize);
  for (UnsignedInteger i = 0; i < conditioningSize; ++i) conditioningVector[i] = y[conditioningDimension - conditioningSize + i];
  return distributionCollection_[distributionIndex].computeConditionalCDF(x, conditioningVector);
}

Point BlockIndependentDistribution::computeSequentialConditionalCDF(const Point & x) const
{
  if (x.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute sequential conditional CDF with an argument of dimension=" << x.getDimension() << " different from distribution dimension=" << dimension_;
  Point result(dimension_);
  if (hasIndependentCopula())
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      result[i] = getMarginal(i).computeCDF(x[i]);
  else
  {
    const UnsignedInteger size = distributionCollection_.getSize();
    UnsignedInteger start = 0;
    UnsignedInteger stop = 0;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const UnsignedInteger localDimension = distributionCollection_[i].getDimension();
      Point localX(localDimension);
      stop += localDimension;
      std::copy(x.begin() + start, x.begin() + stop, localX.begin());
      const Point localResult(distributionCollection_[i].computeSequentialConditionalCDF(localX));
      std::copy(localResult.begin(), localResult.end(), result.begin() + start);
      start = stop;
    } // i
  } // else
  return result;
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
Scalar BlockIndependentDistribution::computeConditionalQuantile(const Scalar q, const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension == 0) return q;
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if ((q < 0.0) || (q > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
  if (q == 0.0) return getRange().getLowerBound()[y.getDimension()];
  if (q == 1.0) return getRange().getUpperBound()[y.getDimension()];
  if (conditioningDimension == 0) return getMarginal(0).computeQuantile(q)[0];
  // General case
  UnsignedInteger distributionIndex;
  const UnsignedInteger partialDimension = computePartialDimension(conditioningDimension, distributionIndex);
  // Extract the relevant part of the conditioning
  const UnsignedInteger conditioningSize = partialDimension - conditioningDimension;
  Point conditioningVector(conditioningSize);
  for (UnsignedInteger i = 0; i < conditioningSize; ++i)
  {
    conditioningVector[i] = y[conditioningDimension - conditioningSize + i];
  }
  return distributionCollection_[distributionIndex].computeConditionalQuantile(q, conditioningVector);
}

Point BlockIndependentDistribution::computeSequentialConditionalQuantile(const Point & q) const
{
  if (q.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute sequential conditional quantile with an argument of dimension=" << q.getDimension() << " different from distribution dimension=" << dimension_;
  Point result(dimension_);
  if (hasIndependentCopula())
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      result[i] = getMarginal(i).computeQuantile(q[i])[0];
  else
  {
    const UnsignedInteger size = distributionCollection_.getSize();
    UnsignedInteger start = 0;
    UnsignedInteger stop = 0;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const UnsignedInteger localDimension = distributionCollection_[i].getDimension();
      Point localQ(localDimension);
      stop += localDimension;
      std::copy(q.begin() + start, q.begin() + stop, localQ.begin());
      const Point localResult(distributionCollection_[i].computeSequentialConditionalQuantile(localQ));
      std::copy(localResult.begin(), localResult.end(), result.begin() + start);
      start = stop;
    } // i
  } // else
  return result;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions
*/
Distribution BlockIndependentDistribution::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  DistributionCollection marginalDistributions;
  const UnsignedInteger indicesSize = indices.getSize();
  const UnsignedInteger size = distributionCollection_.getSize();

  // helps reverse the distribution indices
  Indices distributionCumulatedDimension(size + 1);
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    distributionCumulatedDimension[i + 1] = distributionCumulatedDimension[i] + distributionCollection_[i].getDimension();
  }

  Indices distributionOrder;
  for (UnsignedInteger currentPosition = 0; currentPosition < indicesSize; ++ currentPosition)
  {
    UnsignedInteger currentIndex = indices[currentPosition];

    // find the index of the current distribution
    UnsignedInteger distributionIndex = 0;
    while (currentIndex >= distributionCumulatedDimension[distributionIndex + 1])
    {
      ++ distributionIndex;
    }

    // we already marked that distribution, so we have non-contiguous blocs
    if (distributionOrder.contains(distributionIndex))
      return new MarginalDistribution(*this, indices);

    // mark this distribution
    distributionOrder.add(distributionIndex);

    // bounds of the current distribution
    const UnsignedInteger lowerIndex = distributionCumulatedDimension[distributionIndex];
    const UnsignedInteger upperIndex = distributionCumulatedDimension[distributionIndex + 1];

    // store the current index wrt that distribution
    Indices distributionIndices(1, currentIndex - lowerIndex);

    // store the next indices that also belong to that distribution
    while (currentPosition + 1 < indicesSize)
    {
      currentIndex = indices[currentPosition + 1];
      if ((currentIndex >= lowerIndex) && (currentIndex < upperIndex))
        distributionIndices.add(currentIndex - lowerIndex);
      else
        break;
      ++ currentPosition;
    }
    marginalDistributions.add(distributionCollection_[distributionIndex].getMarginal(distributionIndices));
  }
  if (indicesSize == 1)
    return marginalDistributions[0];
  return new BlockIndependentDistribution(marginalDistributions);
}

/* Tell if the distribution has elliptical copula */
Bool BlockIndependentDistribution::hasEllipticalCopula() const
{
  // The copula is elliptical either because it is based on a unique elliptical copula or it is composed of normal copulas
  const UnsignedInteger size = distributionCollection_.getSize();
  // If there is only one copula in the collection, check if it has an elliptical copula
  if (size == 1) return distributionCollection_[0].hasEllipticalCopula();
  // Else, check if all the distributions have normal copulas <==> elliptical and independent standard representative. We cannot check it in the most general case (eg a SklarCopula built upon a normal distribution).
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (!distributionCollection_[i].hasEllipticalCopula()) return false;
    if (!distributionCollection_[i].getStandardRepresentative().hasIndependentCopula()) return false;
  }
  return true;
}

/* Check if the distribution is a copula */
Bool BlockIndependentDistribution::isCopula() const
{
  for (UnsignedInteger i = 0; i < distributionCollection_.getSize(); ++i)
    if (!distributionCollection_[i].isCopula()) return false;
  return true;
}

/* Check if the distribution is elliptical */
Bool BlockIndependentDistribution::isElliptical() const
{
  for (UnsignedInteger i = 0; i < distributionCollection_.getSize(); ++i)
    if (!distributionCollection_[i].isElliptical() || !distributionCollection_[i].getStandardDistribution().hasIndependentCopula()) return false;
  return true;
}

/* Check if the distribution is constinuous */
Bool BlockIndependentDistribution::isContinuous() const
{
  for (UnsignedInteger i = 0; i < distributionCollection_.getSize(); ++i)
    if (!distributionCollection_[i].isContinuous()) return false;
  return true;
}

/* Check if the distribution is discrete */
Bool BlockIndependentDistribution::isDiscrete() const
{
  for (UnsignedInteger i = 0; i < distributionCollection_.getSize(); ++i)
    if (!distributionCollection_[i].isDiscrete()) return false;
  return true;
}

/* Check if the distribution is integer valued */
Bool BlockIndependentDistribution::isIntegral() const
{
  for (UnsignedInteger i = 0; i < distributionCollection_.getSize(); ++i)
    if (!distributionCollection_[i].isIntegral()) return false;
  return true;
}

/* Tell if the distribution has independent copula */
Bool BlockIndependentDistribution::hasIndependentCopula() const
{
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
    if (!distributionCollection_[i].hasIndependentCopula()) return false;
  return true;
}

/* Get the isoprobabilist transformation */
BlockIndependentDistribution::IsoProbabilisticTransformation BlockIndependentDistribution::getIsoProbabilisticTransformation() const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger size = distributionCollection_.getSize();
  // Special case: only one distribution in the collection
  if (size == 1) return distributionCollection_[0].getIsoProbabilisticTransformation();
  // General case: return the transformation as an aggregated function of pre/post processed isoprobabilistic transformations built using the elementary distributions. These transformations are the isoprobabilistic transformations of the elementary distributions if they lead to the standard Normal space, else a Rosenblatt transformation is substituted
  const Description allVariables(Description::BuildDefault(dimension, "x"));
  UnsignedInteger shift = 0;
  Collection< Function > atomTransformations(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // First, build the mapping between the full components and the components of the current distribution
    const UnsignedInteger atomDimension = distributionCollection_[i].getDimension();
    Description atomVariables(atomDimension);
    for (UnsignedInteger j = 0; j < atomDimension; ++j) atomVariables[j] = allVariables[shift + j];
    const SymbolicFunction projection(allVariables, atomVariables);
    // Second, check if the isoprobabilistic transformation associated with the current distribution maps to a Normal standard distribution
    if (distributionCollection_[i].getStandardDistribution().hasIndependentCopula()) atomTransformations[i] = ComposedFunction(distributionCollection_[i].getIsoProbabilisticTransformation(), projection);
    else atomTransformations[i] = ComposedFunction(RosenblattEvaluation(distributionCollection_[i]), projection);
    shift += atomDimension;
  }
  return AggregatedFunction(atomTransformations);
}

/* Get the inverse isoprobabilist transformation */
BlockIndependentDistribution::InverseIsoProbabilisticTransformation BlockIndependentDistribution::getInverseIsoProbabilisticTransformation() const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger size = distributionCollection_.getSize();
  // Special case: only one distribution in the collection
  if (size == 1) return distributionCollection_[0].getInverseIsoProbabilisticTransformation();
  // General case: return the transformation as an aggregated function of pre/post processed isoprobabilistic transformations built using the elementary distributions. These transformations are the isoprobabilistic transformations of the elementary distributions if they lead to the standard Normal space, else a Rosenblatt transformation is substituted
  const Description allVariables(Description::BuildDefault(dimension, "x"));
  UnsignedInteger shift = 0;
  Collection< Function > atomTransformations(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // First, build the mapping between the full components and the components of the current distribution
    const UnsignedInteger atomDimension = distributionCollection_[i].getDimension();
    Description atomVariables(atomDimension);
    for (UnsignedInteger j = 0; j < atomDimension; ++j) atomVariables[j] = allVariables[shift + j];
    const SymbolicFunction projection(allVariables, atomVariables);
    // Second, check if the isoprobabilistic transformation associated with the current distribution maps to a Normal standard distribution
    if (distributionCollection_[i].getStandardDistribution().hasIndependentCopula()) atomTransformations[i] = ComposedFunction(distributionCollection_[i].getInverseIsoProbabilisticTransformation(), projection);
    else atomTransformations[i] = ComposedFunction(InverseRosenblattEvaluation(distributionCollection_[i]), projection);
    shift += atomDimension;
  }
  return AggregatedFunction(atomTransformations);
}

/* Get the shifted moments of the distribution */
Point BlockIndependentDistribution::getShiftedMoment(const UnsignedInteger n, const Point & shift) const
{
  Point shiftedMoment;
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < distributionCollection_.getSize(); ++ i)
  {
    const UnsignedInteger localDimension = distributionCollection_[i].getDimension();
    Point localShift(localDimension);
    std::copy(shift.begin() + index, shift.begin() + index + localDimension, localShift.begin());
    shiftedMoment.add(distributionCollection_[i].getShiftedMoment(n, localShift));
    index += localDimension;
  }
  return shiftedMoment;
}

/* Compute the covariance of the distribution */
void BlockIndependentDistribution::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  covariance_ = CovarianceMatrix(dimension);
  UnsignedInteger shift = 0;
  for (UnsignedInteger i = 0; i < distributionCollection_.getSize(); ++i)
  {
    const CovarianceMatrix localCovariance(distributionCollection_[i].getCovariance());
    const UnsignedInteger localDimension = localCovariance.getDimension();
    for (UnsignedInteger j = 0; j < localDimension; ++j)
      for (UnsignedInteger k = 0; k <= j; ++k)
        covariance_(shift + j, shift + k) = localCovariance(j, k);
    shift += localDimension;
  } // i
  isAlreadyComputedCovariance_ = true;
}

/* Compute the entropy of the distribution */
Scalar BlockIndependentDistribution::computeEntropy() const
{
  Scalar entropy = 0.0;
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
    entropy += distributionCollection_[i].computeEntropy();
  return entropy;
}

Point BlockIndependentDistribution::getParameter() const
{
  const UnsignedInteger size = distributionCollection_.getSize();
  Point point;
  for (UnsignedInteger i = 0; i < size; ++i)
    point.add(distributionCollection_[i].getParameter());
  return point;
}

void BlockIndependentDistribution::setParameter(const Point & parameter)
{
  const UnsignedInteger size = distributionCollection_.getSize();
  UnsignedInteger globalIndex = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger parametersSize = distributionCollection_[i].getParameterDimension();
    if (globalIndex + parametersSize > parameter.getSize()) throw InvalidArgumentException(HERE) << "Not enough values (" << parameter.getSize() << "), needed " << globalIndex + parametersSize << " for block " << i;
    Point newParameters(parametersSize);
    std::copy(parameter.begin() + globalIndex, parameter.begin() + globalIndex + parametersSize, newParameters.begin());
    distributionCollection_[i].setParameter(newParameters);
    globalIndex += parametersSize;
  }
}

void BlockIndependentDistribution::computeRange()
{
  // aggregate ranges
  Interval::BoolCollection finiteLowerBound;
  Interval::BoolCollection finiteUpperBound;
  Point lowerBound;
  Point upperBound;
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const Interval range(distributionCollection_[i].getRange());
    lowerBound.add(range.getLowerBound());
    upperBound.add(range.getUpperBound());
    finiteLowerBound.add(range.getFiniteLowerBound());
    finiteUpperBound.add(range.getFiniteUpperBound());
  }
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Method save() stores the object through the StorageManager */
void BlockIndependentDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "distributionCollection_", distributionCollection_ );
}

/* Method load() reloads the object from the StorageManager */
void BlockIndependentDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "distributionCollection_", distributionCollection_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
