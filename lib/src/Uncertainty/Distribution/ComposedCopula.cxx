//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all ComposedCopulas
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
#include <cmath>

#include "openturns/ComposedCopula.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/IndependentCopula.hxx"
#include "openturns/MarginalDistribution.hxx"
#include "openturns/RosenblattEvaluation.hxx"
#include "openturns/InverseRosenblattEvaluation.hxx"
#include "openturns/Log.hxx"
#include "openturns/Normal.hxx"
#include "openturns/NormalCopula.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/AggregatedFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ComposedCopula)

static const Factory<ComposedCopula> Factory_ComposedCopula;

/* Default constructor */
ComposedCopula::ComposedCopula()
  : CopulaImplementation()
  , copulaCollection_(0)
  , isIndependent_(false)
{
  setName("ComposedCopula");
  DistributionCollection coll(1, IndependentCopula(2));
  setCopulaCollection(coll);
}

/* Default constructor */
ComposedCopula::ComposedCopula(const DistributionCollection & coll)
  : CopulaImplementation()
  , copulaCollection_()
  , isIndependent_(false)
{
  setName("ComposedCopula");
  // We assign the copula collection through the accessor in order to compute the composed copula dimension
  setCopulaCollection(coll);
}

/* Comparison operator */
Bool ComposedCopula::operator ==(const ComposedCopula & other) const
{
  if (this == &other) return true;
  return copulaCollection_ == other.copulaCollection_;
}

Bool ComposedCopula::equals(const DistributionImplementation & other) const
{
  const ComposedCopula* p_other = dynamic_cast<const ComposedCopula*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String ComposedCopula::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ComposedCopula::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension();
  for (UnsignedInteger i = 0; i < copulaCollection_.getSize(); ++i)
    oss << " copula[" << i << "]=" << copulaCollection_[i];
  return oss;
}

String ComposedCopula::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(";
  String separator("");
  for (UnsignedInteger i = 0; i < copulaCollection_.getSize(); ++i)
  {
    oss << separator << copulaCollection_[i];
    separator = ", ";
  }
  oss << ")";
  return oss;
}

/* Copula collection accessor */
void ComposedCopula::setCopulaCollection(const DistributionCollection & coll)
{
  // Check if the collection is not empty
  const UnsignedInteger size = coll.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Collection of distributions is empty";
  copulaCollection_ = coll;
  Description description(0);
  UnsignedInteger dimension = 0;
  // Compute the dimension, build the description and check the independence
  isIndependent_ = true;
  bool parallel = true;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (!coll[i].isCopula())
      throw InvalidArgumentException(HERE) << "Element " << i << " is not a copula";
    const UnsignedInteger copulaDimension = coll[i].getDimension();
    dimension += copulaDimension;
    const Description copulaDescription(coll[i].getDescription());
    for (UnsignedInteger j = 0; j < copulaDimension; ++j) description.add(copulaDescription[j]);
    isIndependent_ = isIndependent_ && copulaCollection_[i].hasIndependentCopula();
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
ComposedCopula::DistributionCollection ComposedCopula::getCopulaCollection() const
{
  return copulaCollection_;
}

/* Virtual constructor */
ComposedCopula * ComposedCopula::clone() const
{
  return new ComposedCopula(*this);
}

/* Get one realization of the ComposedCopula */
Point ComposedCopula::getRealization() const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger size = copulaCollection_.getSize();
  Point result(dimension);
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Point realization(copulaCollection_[i].getRealization());
    const UnsignedInteger copulaDimension = copulaCollection_[i].getDimension();
    for (UnsignedInteger j = 0; j < copulaDimension; ++j)
    {
      result[index] = realization[j];
      ++index;
    }
  }
  return result;
}

/* Get the DDF of the ComposedCopula */
Point ComposedCopula::computeDDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  Point DDF(getDimension());
  const UnsignedInteger size = copulaCollection_.getSize();
  Point copulaPDF(size);
  PointCollection copulaDDF(size);
  Scalar productPDF = 1.0;
  UnsignedInteger index = 0;
  // First, compute the several elements
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // If one component is outside of the support, the PDF is null
    if ((point[i] <= 0.0) || (point[i] >= 1.0)) return Point(dimension);
    const Distribution copula(copulaCollection_[i]);
    const UnsignedInteger copulaDimension = copula.getDimension();
    Point component(copulaDimension);
    for (UnsignedInteger j = 0; j < copulaDimension; ++j)
    {
      component[j] = point[index];
      ++index;
    }
    copulaPDF[i] = copula.computePDF(component);
    productPDF *= copulaPDF[i];
    copulaDDF[i] = copula.computeDDF(component);
  }
  // Then, compute the DDF
  index = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Point currentDDF(copulaDDF[i] * (productPDF / copulaPDF[i]));
    const UnsignedInteger currentDimension = currentDDF.getDimension();
    for (UnsignedInteger j = 0; j < currentDimension; ++j)
    {
      DDF[index] = currentDDF[j];
      ++index;
    }
  }
  return DDF;
}

/* Get the PDF of the ComposedCopula */
Scalar ComposedCopula::computePDF(const Point & point) const
{
  /* PDF = PDF_copula1x...xPDF_copula_n */
  const UnsignedInteger dimension = getDimension();
  if (isIndependent_) return IndependentCopula(dimension).computePDF(point);
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  const UnsignedInteger size = copulaCollection_.getSize();
  Scalar productPDF = 1.0;
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // If one component is outside of the support, the PDF is null
    if ((point[i] <= 0.0) || (point[i] >= 1.0)) return 0.0;
    const UnsignedInteger copulaDimension = copulaCollection_[i].getDimension();
    Point component(copulaDimension);
    for (UnsignedInteger j = 0; j < copulaDimension; ++j)
    {
      component[j] = point[index];
      ++index;
    }
    productPDF *= copulaCollection_[i].computePDF(component);
  }
  return productPDF;
}

/* Get the log-PDF of the ComposedCopula */
Scalar ComposedCopula::computeLogPDF(const Point & point) const
{
  /* PDF = PDF_copula1x...xPDF_copula_n */
  const UnsignedInteger dimension = getDimension();
  if (isIndependent_) return IndependentCopula(dimension).computePDF(point);
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  const UnsignedInteger size = copulaCollection_.getSize();
  Scalar sumLogPDF = 0.0;
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // If one component is outside of the support, the PDF is null
    if ((point[i] <= 0.0) || (point[i] >= 1.0)) return -SpecFunc::LogMaxScalar;
    const UnsignedInteger copulaDimension = copulaCollection_[i].getDimension();
    Point component(copulaDimension);
    for (UnsignedInteger j = 0; j < copulaDimension; ++j)
    {
      component[j] = point[index];
      ++index;
    }
    sumLogPDF += copulaCollection_[i].computeLogPDF(component);
  }
  return sumLogPDF;
}

/* Get the CDF of the ComposedCopula */
Scalar ComposedCopula::computeCDF(const Point & point) const
{
  /* CDF = CDF_copula1x...xCDF_copula_n */
  const UnsignedInteger dimension = getDimension();
  if (isIndependent_) return IndependentCopula(dimension).computeCDF(point);
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const UnsignedInteger size = copulaCollection_.getSize();
  Scalar productCDF = 1.0;
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // If one component is at the left of the support of its marginal distribution, the CDF is null
    if (point[i] <= 0.0) return 0.0;
    // If the component is inside of the support, update the CDF value
    const UnsignedInteger copulaDimension = copulaCollection_[i].getDimension();
    Point component(copulaDimension);
    for (UnsignedInteger j = 0; j < copulaDimension; ++j)
    {
      component[j] = point[index];
      ++index;
    }
    const Scalar cdf = copulaCollection_[i].computeCDF(component);
    productCDF *= cdf;
  }
  return productCDF;
}

/* Compute the probability content of an interval */
Scalar ComposedCopula::computeProbability(const Interval & interval) const
{
  const UnsignedInteger dimension = getDimension();
  if (isIndependent_) return IndependentCopula(dimension).computeProbability(interval);
  if (interval.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given interval must have dimension=" << dimension << ", here dimension=" << interval.getDimension();

  // Reduce the given interval to the support of the distribution, which is the nD unit cube
  const Interval intersect(interval.intersect(Interval(dimension)));
  // If the intersection is empty
  if (intersect.isEmpty()) return 0.0;
  const Point lowerIntersect(intersect.getLowerBound());
  const Point upperIntersect(intersect.getUpperBound());
  const UnsignedInteger size = copulaCollection_.getSize();
  Scalar value = 1.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Distribution copula(copulaCollection_[i]);
    const UnsignedInteger copulaDimension = copula.getDimension();
    Point lower(copulaDimension);
    Point upper(copulaDimension);
    for (UnsignedInteger j = 0; j < copulaDimension; ++j)
    {
      lower[j] = lowerIntersect[j];
      upper[j] = upperIntersect[j];
    }
    value *= copula.computeProbability(Interval(lower, upper));
  }
  return value;
}

/* Get the survival function of the distribution */
Scalar ComposedCopula::computeSurvivalFunction(const Point & point) const
{
  /* Survival = Survival_copula1x...xSurvival_copula_n */
  const UnsignedInteger dimension = getDimension();
  if (isIndependent_) return IndependentCopula(dimension).computeSurvivalFunction(point);
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const UnsignedInteger size = copulaCollection_.getSize();
  Scalar productSurvival = 1.0;
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // If one component is at the left of the support of its marginal distribution, the Survival is null
    if (point[i] >= 1.0) return 0.0;
    // If the component is inside of the support, update the Survival value
    const UnsignedInteger copulaDimension = copulaCollection_[i].getDimension();
    Point component(copulaDimension);
    for (UnsignedInteger j = 0; j < copulaDimension; ++j)
    {
      component[j] = point[index];
      ++index;
    }
    const Scalar survival = copulaCollection_[i].computeSurvivalFunction(component);
    productSurvival *= survival;
  }
  return productSurvival;
}

/* Get the Kendall concordance of the distribution */
CorrelationMatrix ComposedCopula::getKendallTau() const
{
  const UnsignedInteger dimension = getDimension();
  CorrelationMatrix tau(dimension);
  if (isIndependent_) return tau;
  const UnsignedInteger size = copulaCollection_.getSize();
  UnsignedInteger globalIndex = 0;
  for (UnsignedInteger n = 0; n < size; ++n)
  {
    const CorrelationMatrix localTau(copulaCollection_[n].getKendallTau());
    const UnsignedInteger localSize = localTau.getDimension();
    for (UnsignedInteger i = 0; i < localSize; ++i)
      for (UnsignedInteger j = 0; j < i; ++j)
        tau(globalIndex + i, globalIndex + j) = localTau(i, j);
    globalIndex += localSize;
  }
  return tau;
}

/* Get the shape matrix of the distribution, ie the correlation matrix
   of its copula if it is elliptical */
CorrelationMatrix ComposedCopula::getShapeMatrix() const
{
  if (!hasEllipticalCopula()) throw NotDefinedException(HERE) << "Error: the shape matrix is defined only for elliptical copulas.";
  const UnsignedInteger size = copulaCollection_.getSize();
  if (size == 1) return copulaCollection_[0].getShapeMatrix();
  const UnsignedInteger dimension = getDimension();
  CorrelationMatrix shapeMatrix(dimension);
  UnsignedInteger shift = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const CorrelationMatrix localShapeMatrix(copulaCollection_[i].getShapeMatrix());
    const UnsignedInteger localDimension = localShapeMatrix.getDimension();
    for (UnsignedInteger j = 0; j < localDimension; ++j)
      for (UnsignedInteger k = 0; k <= j; ++k)
        shapeMatrix(shift + j, shift + k) = localShapeMatrix(j, k);
    shift += localDimension;
  } // i
  return shapeMatrix;
}

/* Get the PDF gradient of the distribution */
Point ComposedCopula::computePDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In ComposedCopula::computePDFGradient(const Point & point) const";
}

/* Get the CDF gradient of the distribution */
Point ComposedCopula::computeCDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In ComposedCopula::computeCDFGradient(const Point & point) const";
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar ComposedCopula::computeConditionalPDF(const Scalar x, const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return ((x >= 0.0 && x < 1.0) ? 1.0 : 0.0);
  // General case
  UnsignedInteger copulaIndex = 0;
  UnsignedInteger partialDimension = copulaCollection_[copulaIndex].getDimension();
  while (partialDimension < conditioningDimension)
  {
    ++copulaIndex;
    partialDimension += copulaCollection_[copulaIndex].getDimension();
  }
  // Extract the relevant part of the conditioning
  const UnsignedInteger conditioningSize = partialDimension - conditioningDimension;
  Point conditioningVector(conditioningSize);
  for (UnsignedInteger i = 0; i < conditioningSize; ++i) conditioningVector[i] = y[conditioningDimension - conditioningSize + i];
  return copulaCollection_[copulaIndex].computeConditionalPDF(x, conditioningVector);
}

Point ComposedCopula::computeSequentialConditionalPDF(const Point & x) const
{
  if (x.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute sequential conditional PDF with an argument of dimension=" << x.getDimension() << " different from distribution dimension=" << dimension_;
  Point result(dimension_);
  if (hasIndependentCopula())
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      result[i] = ((x[i] >= 0.0 && x[i] < 1.0) ? 1.0 : 0.0);
  else
  {
    const UnsignedInteger size = copulaCollection_.getSize();
    UnsignedInteger start = 0;
    UnsignedInteger stop = 0;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const UnsignedInteger localDimension = copulaCollection_[i].getDimension();
      Point localX(localDimension);
      stop += localDimension;
      std::copy(x.begin() + start, x.begin() + stop, localX.begin());
      const Point localResult(copulaCollection_[i].computeSequentialConditionalPDF(localX));
      std::copy(localResult.begin(), localResult.end(), result.begin() + start);
      start = stop;
    } // i
  } // else
  return result;
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar ComposedCopula::computeConditionalCDF(const Scalar x, const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return std::min(1.0, std::max(0.0, x));
  // General case
  UnsignedInteger copulaIndex = 0;
  UnsignedInteger partialDimension = copulaCollection_[copulaIndex].getDimension();
  while (partialDimension < conditioningDimension)
  {
    ++copulaIndex;
    partialDimension += copulaCollection_[copulaIndex].getDimension();
  }
  // Extract the relevant part of the conditioning
  const UnsignedInteger conditioningSize = partialDimension - conditioningDimension;
  Point conditioningVector(conditioningSize);
  for (UnsignedInteger i = 0; i < conditioningSize; ++i) conditioningVector[i] = y[conditioningDimension - conditioningSize + i];
  return copulaCollection_[copulaIndex].computeConditionalCDF(x, conditioningVector);
}

Point ComposedCopula::computeSequentialConditionalCDF(const Point & x) const
{
  if (x.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute sequential conditional CDF with an argument of dimension=" << x.getDimension() << " different from distribution dimension=" << dimension_;
  Point result(dimension_);
  if (hasIndependentCopula())
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      result[i] = (x[i] < 0.0 ? 0.0 : x[i] > 1.0 ? 1.0 : x[i]);
  else
  {
    const UnsignedInteger size = copulaCollection_.getSize();
    UnsignedInteger start = 0;
    UnsignedInteger stop = 0;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const UnsignedInteger localDimension = copulaCollection_[i].getDimension();
      Point localX(localDimension);
      stop += localDimension;
      std::copy(x.begin() + start, x.begin() + stop, localX.begin());
      const Point localResult(copulaCollection_[i].computeSequentialConditionalCDF(localX));
      std::copy(localResult.begin(), localResult.end(), result.begin() + start);
      start = stop;
    } // i
  } // else
  return result;
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
Scalar ComposedCopula::computeConditionalQuantile(const Scalar q, const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension == 0) return q;
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if ((q < 0.0) || (q > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
  if (q == 0.0) return 0.0;
  if (q == 1.0) return 1.0;
  if (conditioningDimension == 0) return q;
  // General case
  UnsignedInteger copulaIndex = 0;
  UnsignedInteger partialDimension = copulaCollection_[copulaIndex].getDimension();
  while (partialDimension < conditioningDimension)
  {
    ++copulaIndex;
    partialDimension += copulaCollection_[copulaIndex].getDimension();
  }
  // Extract the relevant part of the conditioning
  const UnsignedInteger conditioningSize = partialDimension - conditioningDimension;
  Point conditioningVector(conditioningSize);
  for (UnsignedInteger i = 0; i < conditioningSize; ++i)
  {
    conditioningVector[i] = y[conditioningDimension - conditioningSize + i];
  }
  return copulaCollection_[copulaIndex].computeConditionalQuantile(q, conditioningVector);
}

Point ComposedCopula::computeSequentialConditionalQuantile(const Point & q) const
{
  if (q.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute sequential conditional quantile with an argument of dimension=" << q.getDimension() << " different from distribution dimension=" << dimension_;
  Point result(dimension_);
  if (hasIndependentCopula())
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      result[i] = (q[i] < 0.0 ? 0.0 : q[i] > 1.0 ? 1.0 : q[i]);
  else
  {
    const UnsignedInteger size = copulaCollection_.getSize();
    UnsignedInteger start = 0;
    UnsignedInteger stop = 0;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const UnsignedInteger localDimension = copulaCollection_[i].getDimension();
      Point localQ(localDimension);
      stop += localDimension;
      std::copy(q.begin() + start, q.begin() + stop, localQ.begin());
      const Point localResult(copulaCollection_[i].computeSequentialConditionalQuantile(localQ));
      std::copy(localResult.begin(), localResult.end(), result.begin() + start);
      start = stop;
    } // i
  } // else
  return result;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions
*/
Distribution ComposedCopula::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  DistributionCollection marginalCopulas;
  const UnsignedInteger indicesSize = indices.getSize();
  const UnsignedInteger size = copulaCollection_.getSize();

  // helps reverse the copula indices
  Indices copulaCumulatedDimension(size + 1);
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    copulaCumulatedDimension[i + 1] = copulaCumulatedDimension[i] + copulaCollection_[i].getDimension();
  }

  Indices copulaOrder;
  for (UnsignedInteger currentPosition = 0; currentPosition < indicesSize; ++ currentPosition)
  {
    UnsignedInteger currentIndex = indices[currentPosition];

    // find the index of the current copula
    UnsignedInteger copulaIndex = 0;
    while (currentIndex >= copulaCumulatedDimension[copulaIndex + 1])
    {
      ++ copulaIndex;
    }

    // we already marked that copula, so we have non-contiguous blocs
    if (copulaOrder.contains(copulaIndex))
      return new MarginalDistribution(*this, indices);

    // mark this copula
    copulaOrder.add(copulaIndex);

    // bounds of the current copula
    const UnsignedInteger lowerIndex = copulaCumulatedDimension[copulaIndex];
    const UnsignedInteger upperIndex = copulaCumulatedDimension[copulaIndex + 1];

    // store the current index wrt that copula
    Indices copulaIndices(1, currentIndex - lowerIndex);

    // store the next indices that also belong to that copula
    while (currentPosition + 1 < indicesSize)
    {
      currentIndex = indices[currentPosition + 1];
      if ((currentIndex >= lowerIndex) && (currentIndex < upperIndex))
        copulaIndices.add(currentIndex - lowerIndex);
      else
        break;
      ++ currentPosition;
    }
    marginalCopulas.add(copulaCollection_[copulaIndex].getMarginal(copulaIndices));
  }
  return new ComposedCopula(marginalCopulas);
}

/* Parameters value and description accessor */
ComposedCopula::PointWithDescriptionCollection ComposedCopula::getParametersCollection() const
{
  PointWithDescriptionCollection parameters(1);
  // No marginal parameter
  // Put the dependence parameters
  PointWithDescription point(0);
  Description description(0);
  const UnsignedInteger size = copulaCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // All distributions, including copulas, must output a collection of Point of size at least 1,
    // even if the Point are empty
    const PointWithDescription copulaParameters(copulaCollection_[i].getParametersCollection()[0]);
    const Description parametersDescription(copulaParameters.getDescription());
    const String copulaName(copulaCollection_[i].getName());
    const UnsignedInteger parameterDimension = copulaParameters.getDimension();
    for (UnsignedInteger j = 0; j < parameterDimension; ++j)
    {
      point.add(copulaParameters[j]);
      description.add(OSS() << "copula_" << i << "_" << parametersDescription[j]);
    }
  }
  point.setDescription(description);
  point.setName(getName());
  parameters[0] = point;
  return parameters;
} // getParametersCollection

void ComposedCopula::setParametersCollection(const PointCollection & parametersCollection)
{
  // Check if the given parameters are ok
  if (parametersCollection.getSize() != 1) throw InvalidArgumentException(HERE) << "Error: the given collection has a size=" << parametersCollection.getSize() << " but should be of size=1";
  // Dependence parameters
  const Point parameters(parametersCollection[0]);
  const UnsignedInteger parametersDimension = parameters.getDimension();
  // Index within the given parametersCollection
  UnsignedInteger globalIndex = 0;
  const UnsignedInteger size = copulaCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // All distributions, including copulas, must output a collection of Point of size at least 1,
    // even if the Point are empty
    const UnsignedInteger atomParametersDimension = copulaCollection_[i].getParametersCollection()[0].getDimension();
    // ith copula parameters
    Point point(atomParametersDimension);
    for (UnsignedInteger j = 0; j < atomParametersDimension; ++j)
    {
      if (globalIndex >= parametersDimension) throw InvalidArgumentException(HERE) << "Error: there are too few dependence parameters";
      point[j] = parameters[globalIndex];
      ++globalIndex;
    } // atom parameters
    copulaCollection_[i].setParametersCollection(PointCollection(1, point));
  } // atoms
  if (globalIndex != parametersDimension) throw InvalidArgumentException(HERE) << "Error: there are too many dependence parameters, expected " << globalIndex << " parameters and got " << parametersDimension;
}

/* Tell if the distribution has elliptical copula */
Bool ComposedCopula::hasEllipticalCopula() const
{
  // The copula is elliptical either because it is based on a unique elliptical copula or it is composed of normal copulas
  const UnsignedInteger size = copulaCollection_.getSize();
  // If there is only one copula in the collection, check if it has an elliptical copula
  if (size == 1) return copulaCollection_[0].hasEllipticalCopula();
  // Else, check if all the copulas are normal copulas. We cannot check it in the most general case (eg a SklarCopula built upon a normal distribution).
  const String normalCopulaName(NormalCopula::GetClassName());
  for (UnsignedInteger i = 0; i < size; ++i)
    if (copulaCollection_[i].getImplementation()->getClassName() != normalCopulaName) return false;
  return true;
}

/* Tell if the distribution has independent copula */
Bool ComposedCopula::hasIndependentCopula() const
{
  return isIndependent_;
}

/* Get the isoprobabilist transformation */
ComposedCopula::IsoProbabilisticTransformation ComposedCopula::getIsoProbabilisticTransformation() const
{
  // First special case: the copula is independent
  const UnsignedInteger dimension = getDimension();
  if (isIndependent_) return IndependentCopula(dimension).getIsoProbabilisticTransformation();
  const UnsignedInteger size = copulaCollection_.getSize();
  // Second special case: only one copula in the collection
  if (size == 1) return copulaCollection_[0].getIsoProbabilisticTransformation();
  // General case: return the transformation as an aggregated function of pre/post processed isoprobabilistic transformations built using the elementary copulas. These transformations are the isoprobabilistic transformations of the elementary copulas if they lead to the standard Normal space, else a Rosenblatt transformation is substituted
  const Description allVariables(Description::BuildDefault(dimension, "x"));
  UnsignedInteger shift = 0;
  Collection< Function > atomTransformations(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // First, build the mapping between the full components and the components of the current copula
    const UnsignedInteger atomDimension = copulaCollection_[i].getDimension();
    Description atomVariables(atomDimension);
    for (UnsignedInteger j = 0; j < atomDimension; ++j) atomVariables[j] = allVariables[shift + j];
    const SymbolicFunction projection(allVariables, atomVariables);
    // Second, check if the isoprobabilistic transformation associated with the current copula maps to a Normal standard distribution
    if (copulaCollection_[i].getStandardDistribution().hasIndependentCopula()) atomTransformations[i] = ComposedFunction(copulaCollection_[i].getIsoProbabilisticTransformation(), projection);
    else atomTransformations[i] = ComposedFunction(RosenblattEvaluation(copulaCollection_[i]), projection);
    shift += atomDimension;
  }
  return AggregatedFunction(atomTransformations);
}

/* Get the inverse isoprobabilist transformation */
ComposedCopula::InverseIsoProbabilisticTransformation ComposedCopula::getInverseIsoProbabilisticTransformation() const
{
  // First special case: the copula is independent
  const UnsignedInteger dimension = getDimension();
  if (isIndependent_) return IndependentCopula(dimension).getInverseIsoProbabilisticTransformation();
  const UnsignedInteger size = copulaCollection_.getSize();
  // Special case: only one copula in the collection
  if (size == 1) return copulaCollection_[0].getInverseIsoProbabilisticTransformation();
  // General case: return the transformation as an aggregated function of pre/post processed isoprobabilistic transformations built using the elementary copulas. These transformations are the isoprobabilistic transformations of the elementary copulas if they lead to the standard Normal space, else a Rosenblatt transformation is substituted
  const Description allVariables(Description::BuildDefault(dimension, "x"));
  UnsignedInteger shift = 0;
  Collection< Function > atomTransformations(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // First, build the mapping between the full components and the components of the current copula
    const UnsignedInteger atomDimension = copulaCollection_[i].getDimension();
    Description atomVariables(atomDimension);
    for (UnsignedInteger j = 0; j < atomDimension; ++j) atomVariables[j] = allVariables[shift + j];
    const SymbolicFunction projection(allVariables, atomVariables);
    // Second, check if the isoprobabilistic transformation associated with the current copula maps to a Normal standard distribution
    if (copulaCollection_[i].getStandardDistribution().hasIndependentCopula()) atomTransformations[i] = ComposedFunction(copulaCollection_[i].getInverseIsoProbabilisticTransformation(), projection);
    else atomTransformations[i] = ComposedFunction(InverseRosenblattEvaluation(copulaCollection_[i]), projection);
    shift += atomDimension;
  }
  return AggregatedFunction(atomTransformations);
}

/* Compute the covariance of the distribution */
void ComposedCopula::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  covariance_ = CovarianceMatrix(dimension);
  UnsignedInteger shift = 0;
  for (UnsignedInteger i = 0; i < copulaCollection_.getSize(); ++i)
  {
    const CovarianceMatrix localCovariance(copulaCollection_[i].getCovariance());
    const UnsignedInteger localDimension = localCovariance.getDimension();
    for (UnsignedInteger j = 0; j < localDimension; ++j)
      for (UnsignedInteger k = 0; k <= j; ++k)
        covariance_(shift + j, shift + k) = localCovariance(j, k);
    shift += localDimension;
  } // i
  isAlreadyComputedCovariance_ = true;
}

/* Compute the entropy of the distribution */
Scalar ComposedCopula::computeEntropy() const
{
  Scalar entropy = 0.0;
  if (isIndependent_) return entropy;
  const UnsignedInteger size = copulaCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
    entropy += copulaCollection_[i].computeEntropy();
  return entropy;
}

/* Method save() stores the object through the StorageManager */
void ComposedCopula::save(Advocate & adv) const
{
  CopulaImplementation::save(adv);
  adv.saveAttribute( "copulaCollection_", copulaCollection_ );
  adv.saveAttribute( "isIndependent_", isIndependent_ );
}

/* Method load() reloads the object from the StorageManager */
void ComposedCopula::load(Advocate & adv)
{
  CopulaImplementation::load(adv);
  adv.loadAttribute( "copulaCollection_", copulaCollection_ );
  adv.loadAttribute( "isIndependent_", isIndependent_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
