//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all OrdinalSumCopulas
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
#include <cmath>

#include "openturns/OrdinalSumCopula.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/IndependentCopula.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(OrdinalSumCopula);

static const Factory<OrdinalSumCopula> Factory_OrdinalSumCopula;

/* Default constructor */
OrdinalSumCopula::OrdinalSumCopula()
  : CopulaImplementation()
  , copulaCollection_(2, IndependentCopula(2))
  , bounds_(1, 0.5)
  , blockLengths_(2, 0.5)
  , blockDistribution_()
{
  setName("OrdinalSumCopula");
  setDimension(2);
  NumericalSample support(2, 1);
  support[0][0] = 0.0;
  support[1][0] = 1.0;
  blockDistribution_ = UserDefined(support);
  computeRange();
}

/* Default constructor */
OrdinalSumCopula::OrdinalSumCopula(const CopulaCollection & coll,
                                   const NumericalPoint & bounds)
  : CopulaImplementation()
  , copulaCollection_(0)
  , bounds_(0)
  , blockLengths_(0)
  , blockDistribution_()
{
  setName("OrdinalSumCopula");
  // We assign the copula collection through the accessor in order to compute the ordinalSum copula dimension
  setCopulaCollection(coll);
  setBounds(bounds);
}

/* Virtual constructor */
OrdinalSumCopula * OrdinalSumCopula::clone() const
{
  return new OrdinalSumCopula(*this);
}

/* Comparison operator */
Bool OrdinalSumCopula::operator ==(const OrdinalSumCopula & other) const
{
  if (this == &other) return true;
  return (copulaCollection_ == other.copulaCollection_) && (bounds_ == other.bounds_);
}

Bool OrdinalSumCopula::equals(const DistributionImplementation & other) const
{
  const OrdinalSumCopula* p_other = dynamic_cast<const OrdinalSumCopula*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String OrdinalSumCopula::__repr__() const
{
  OSS oss(true);
  oss << "class=" << OrdinalSumCopula::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension();
  for (UnsignedInteger i = 0; i < copulaCollection_.getSize(); ++i)
    oss << " copula[" << i << "]=" << copulaCollection_[i];
  oss << "bounds=" << bounds_;
  return oss;
}

String OrdinalSumCopula::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(";
  const UnsignedInteger size(copulaCollection_.getSize());
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    oss << (i == 0 ? "[" : ", [");
    oss << (i == 0 ? 0.0 : bounds_[i - 1]) << ", ";
    oss << (i == size - 1 ? 1.0 : bounds_[i]) << "], ";
    oss << copulaCollection_[i];
  }
  oss << ")";
  return oss;
}

/* Distribution collection accessor */
void OrdinalSumCopula::setCopulaCollection(const CopulaCollection & coll)
{
  // Check if the collection is not empty
  const UnsignedInteger size = coll.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Collection of distributions is empty";
  copulaCollection_ = coll;
  const UnsignedInteger dimension = coll[0].getDimension();
  // Compute the dimension, build the description and check the independence
  Bool parallel = true;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (coll[i].getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: expected copulas of dimension=" << dimension << " but copula " << i << " has dimension=" << coll[i].getDimension();
    parallel = parallel && coll[i].getImplementation()->isParallel();
  }
  setParallel(parallel);
  isAlreadyComputedCovariance_ = false;
  // One MUST set the dimension BEFORE the description, else an error occurs
  setDimension(dimension);
  setDescription(coll[0].getDescription());
  computeRange();
}


/* Distribution collection accessor */
const OrdinalSumCopula::CopulaCollection & OrdinalSumCopula::getCopulaCollection() const
{
  return copulaCollection_;
}

/* Bounds accessor */
void OrdinalSumCopula::setBounds(const NumericalPoint & bounds)
{
  const UnsignedInteger size(bounds.getSize());
  if (size != copulaCollection_.getSize() - 1) throw InvalidArgumentException(HERE) << "Error: expected " << copulaCollection_.getSize() - 1 << " bounds, got " << size;
  // Check that the bounds:
  // + are in [0, 1]
  // + are in increasing order
  // Build:
  // + the block lengths
  // + the block distribution
  // Here are the parameters of the block distribution
  bounds_ = NumericalPoint(0);
  NumericalSample support(0, 1);
  blockLengths_ = NumericalPoint(0);
  NumericalScalar lastBound(0.0);
  CopulaCollection coll(0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalScalar currentBound(bounds[i]);
    if (currentBound < lastBound) throw InvalidArgumentException(HERE) << "Error: bound[" << i << "]=" << currentBound << " and should be greater than " << lastBound;
    if (currentBound > 1.0) throw InvalidArgumentException(HERE) << "Error: bound[" << i << "]=" << currentBound << " and should be less than 1";
    const NumericalScalar length(currentBound - lastBound);
    if (length > 0.0)
    {
      support.add(NumericalPoint(1, blockLengths_.getSize()));
      blockLengths_.add(length);
      coll.add(copulaCollection_[i]);
      bounds_.add(currentBound);
    }
    else LOGWARN(OSS(false) << "The length of block " << i << " is zero, the corresponding copula " << copulaCollection_[i] << " is removed from the collection");
    lastBound = currentBound;
  }
  if (lastBound < 1.0)
  {
    support.add(NumericalPoint(1, blockLengths_.getSize()));
    blockLengths_.add(1.0 - lastBound);
    coll.add(copulaCollection_[size]);
  }
  else LOGWARN(OSS(false) << "The length of block " << size << " is zero, the corresponding copula " << copulaCollection_[size] << " is removed from the collection");
  blockDistribution_ = UserDefined(support, blockLengths_);
  if (coll.getSize() < copulaCollection_.getSize())
    copulaCollection_ = coll;
}

const NumericalPoint & OrdinalSumCopula::getBounds() const
{
  return bounds_;
}

/* Get one realization of the OrdinalSumCopula */
NumericalPoint OrdinalSumCopula::getRealization() const
{
  // If there is only one copula
  if (copulaCollection_.getSize() == 1) return copulaCollection_[0].getRealization();
  const UnsignedInteger index(static_cast<UnsignedInteger>(round(blockDistribution_.getRealization()[0])));
  NumericalPoint result(copulaCollection_[index].getRealization() * blockLengths_[index]);
  if (index > 0) result += NumericalPoint(getDimension(), bounds_[index - 1]);
  return result;
}

/* Find the block number of a given real wrt the bounds. Returns -1 if negative and -2 if greater than 1 */
SignedInteger OrdinalSumCopula::findBlock(const NumericalScalar x) const
{
  if (x < 0.0) return -1;
  if (x >= 1.0) return -2;
  const UnsignedInteger size(bounds_.getSize());
  // If no bounds, x must be in block 0
  if (size == 0) return 0;
  // If less than first bound, x must be in block 0
  if (x < bounds_[0]) return 0;
  // If x >= xMax, index = size
  if (x >= bounds_[size - 1]) return size;
  // Else, x is between bounds_[0] and bounds_[size-1]. Find the index by bisection.
  SignedInteger iLeft(0);
  SignedInteger iRight(size - 1);
  while (iRight - iLeft > 1)
  {
    const SignedInteger iMiddle((iLeft + iRight) / 2);
    if (x < bounds_[iMiddle]) iRight = iMiddle;
    else iLeft = iMiddle;
  }
  return iRight;
}

/* Check if the given point is in the given block */
Bool OrdinalSumCopula::isInBlock(const NumericalPoint & point,
                                 const UnsignedInteger index) const
{
  const UnsignedInteger dimension(getDimension());
  if (index == 0) return Interval(NumericalPoint(dimension, 0.0), NumericalPoint(dimension, bounds_[index])).contains(point);
  if (index < bounds_.getSize()) return Interval(NumericalPoint(dimension, bounds_[index - 1]), NumericalPoint(dimension, bounds_[index])).contains(point);
  return Interval(NumericalPoint(dimension, bounds_[index - 1]), NumericalPoint(dimension, 1.0)).contains(point);
}


/* Get the DDF of the OrdinalSumCopula */
NumericalPoint OrdinalSumCopula::computeDDF(const NumericalPoint & point) const
{
  const UnsignedInteger dimension(getDimension());
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  // If there is only one copula
  if (copulaCollection_.getSize() == 1) return copulaCollection_[0].computeDDF(point);
  // Compute the candidate block index based on the first coordinate
  const SignedInteger index(findBlock(point[0]));
  // There is no candidate
  if (index < 0) return NumericalPoint(dimension, 0.0);
  // The point is in the candidate
  if (isInBlock(point, index)) return copulaCollection_[index].computeDDF(point);
  // The point is not in the candidate
  return NumericalPoint(dimension, 0.0);
}

/* Get the PDF of the OrdinalSumCopula */
NumericalScalar OrdinalSumCopula::computePDF(const NumericalPoint & point) const
{
  LOGDEBUG(OSS() << "In OrdinalSumCopula::computePDF, point=" << point);
  // If there is only one copula
  if (copulaCollection_.getSize() == 1) return copulaCollection_[0].computePDF(point);
  const UnsignedInteger dimension(getDimension());
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  // Compute the candidate block index based on the first coordinate
  const SignedInteger index(findBlock(point[0]));
  NumericalScalar pdf(0.0);
  LOGDEBUG(OSS() << "index=" << index);
  // First, the special cases
  if (index < 0)
  {
    LOGDEBUG("OUT OF RANGE");
    return pdf;
  }
  // The point is not in the candidate, the ordinal sum is the min copula
  if (!isInBlock(point, index))
  {
    LOGDEBUG("OUT OF BLOCKS");
    return pdf;
  }
  // The point is in the candidate, compute the value of the corresponding copula
  if (index == 0)
  {
    pdf = std::pow(blockLengths_[0], 1.0 - dimension) * copulaCollection_[0].computePDF(point / blockLengths_[0]);
    LOGDEBUG(OSS() << "In block " << index << ", pdf=" << pdf);
    return pdf;
  }
  pdf = std::pow(blockLengths_[index], 1.0 - dimension) * copulaCollection_[index].computePDF((point - NumericalPoint(dimension, bounds_[index - 1])) / blockLengths_[index]);
  LOGDEBUG(OSS() << "In block " << index << ", pdf=" << pdf);
  return pdf;
}

/* Get the CDF of the OrdinalSumCopula */
NumericalScalar OrdinalSumCopula::computeCDF(const NumericalPoint & point) const
{
  // If there is only one copula
  if (copulaCollection_.getSize() == 1) return copulaCollection_[0].computeCDF(point);
  LOGDEBUG(OSS() << "In OrdinalSumCopula::computeCDF, point=" << point);
  const UnsignedInteger dimension(getDimension());
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  // Compute the candidate block index based on the first coordinate
  const SignedInteger index(findBlock(point[0]));
  LOGDEBUG(OSS() << "index=" << index);
  // First, the special cases
  // The first component is negative, CDF==0
  if (index == -1) return 0.0;
  // The first component is greater than 1, the ordinal sum is the min copula
  if (index == -2) return std::max(0.0, std::min(1.0, *std::min_element(point.begin(), point.end())));
  // The point is not in the candidate, the ordinal sum is the min copula
  // We separate this case from the previous one because the test is expansive
  if (!isInBlock(point, index)) return std::max(0.0, std::min(1.0, *std::min_element(point.begin(), point.end())));
  // The point is in the candidate, compute the value of the corresponding copula
  if (index == 0) return blockLengths_[0] * copulaCollection_[0].computeCDF(point / blockLengths_[0]);
  return bounds_[index - 1] + blockLengths_[index] * copulaCollection_[index].computeCDF((point - NumericalPoint(dimension, bounds_[index - 1])) / blockLengths_[index]);
}

/* Compute the probability content of an interval */
NumericalScalar OrdinalSumCopula::computeProbability(const Interval & interval) const
{
  const UnsignedInteger dimension(getDimension());
  if (interval.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given interval must have dimension=" << dimension << ", here dimension=" << interval.getDimension();

  // Compute the contribution of the first block
  NumericalPoint xMin(dimension, 0.0);
  NumericalPoint xMax(dimension, bounds_[0]);
  NumericalScalar probability(copulaCollection_[0].computeProbability(interval.intersect(Interval(xMin, xMax)) * (1.0 / blockLengths_[0])));
  // Sum the contribution of all the intermediate blocks
  xMin = xMax;
  const UnsignedInteger size(bounds_.getSize());
  for (UnsignedInteger i = 1; i < size; ++i)
  {
    xMax = NumericalPoint(dimension, bounds_[i]);
    probability += copulaCollection_[i].computeProbability(interval.intersect(Interval(xMin, xMax) - Interval(xMin, xMin)) * (1.0 / blockLengths_[i]));
    xMin = xMax;
  }
  // And the contribution of the last block
  xMax = NumericalPoint(dimension, 1.0);
  probability += copulaCollection_[size].computeProbability(interval.intersect(Interval(xMin, xMax) - Interval(xMin, xMin)) * (1.0 / blockLengths_[size]));

  return probability;
}

/* Compute the covariance matrix of the distribution */
void OrdinalSumCopula::computeCovariance() const
{
  MatrixImplementation covariance(*copulaCollection_[0].getCovariance().getImplementation());
  const UnsignedInteger size(copulaCollection_.getSize());
  if (size == 1)
  {
    covariance_ = covariance;
    isAlreadyComputedCovariance_ = true;
    return;
  }
  const UnsignedInteger dimension(getDimension());
  const NumericalPoint data(dimension * dimension, 0.25);
  const MatrixImplementation mask(dimension, dimension, data.begin(), data.end());
  NumericalScalar lastAi(bounds_[0]);
  for (UnsignedInteger i = 1; i < size - 1; ++i)
  {
    const NumericalScalar aI(bounds_[i]);
    const NumericalScalar theta(lastAi / aI);
    covariance = covariance * std::pow(theta, 3.0) + (*copulaCollection_[i].getCovariance().getImplementation()) * std::pow(1.0 - theta, 3.0) + mask * theta * (1.0 - theta);
    lastAi = aI;
  }
  const NumericalScalar theta(lastAi);
  covariance = covariance * std::pow(theta, 3.0) + (*copulaCollection_[size - 1].getCovariance().getImplementation()) * std::pow(1.0 - theta, 3.0) + mask * theta * (1.0 - theta);
  covariance_ = covariance;
  isAlreadyComputedCovariance_ = true;
}

/* Get the Kendall concordance of the distribution */
CorrelationMatrix OrdinalSumCopula::getKendallTau() const
{
  MatrixImplementation tauKendall(*copulaCollection_[0].getKendallTau().getImplementation());
  const UnsignedInteger size(copulaCollection_.getSize());
  if (size == 1) return tauKendall;
  const UnsignedInteger dimension(getDimension());
  const NumericalPoint data(dimension * dimension, 1.0);
  const MatrixImplementation mask(dimension, dimension, data.begin(), data.end());
  NumericalScalar lastAi(bounds_[0]);
  for (UnsignedInteger i = 1; i < size - 1; ++i)
  {
    const NumericalScalar aI(bounds_[i]);
    const NumericalScalar theta(lastAi / aI);
    tauKendall = tauKendall * std::pow(theta, 2.0) + (*copulaCollection_[i].getKendallTau().getImplementation()) * std::pow(1.0 - theta, 2.0) + mask * (2.0 * theta * (1.0 - theta));
    lastAi = aI;
  }
  const NumericalScalar theta(lastAi);
  tauKendall = tauKendall * std::pow(theta, 2.0) + (*copulaCollection_[size - 1].getKendallTau().getImplementation()) * std::pow(1.0 - theta, 2.0) + mask * (2.0 * theta * (1.0 - theta));
  return CorrelationMatrix(tauKendall);
}

/* Get the PDF gradient of the distribution */
NumericalPoint OrdinalSumCopula::computePDFGradient(const NumericalPoint & point) const
{
  const UnsignedInteger dimension(getDimension());
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In OrdinalSumCopula::computePDFGradient(const NumericalPoint & point) const";
}

/* Get the CDF gradient of the distribution */
NumericalPoint OrdinalSumCopula::computeCDFGradient(const NumericalPoint & point) const
{
  const UnsignedInteger dimension(getDimension());
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In OrdinalSumCopula::computeCDFGradient(const NumericalPoint & point) const";
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
NumericalScalar OrdinalSumCopula::computeConditionalPDF(const NumericalScalar x, const NumericalPoint & y) const
{
  throw NotYetImplementedException(HERE) << "In OrdinalSumCopula::computeConditionalPDF(const NumericalScalar x, const NumericalPoint & y) const";
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
NumericalScalar OrdinalSumCopula::computeConditionalCDF(const NumericalScalar x, const NumericalPoint & y) const
{
  throw NotYetImplementedException(HERE) << "In OrdinalSumCopula::computeConditionalCDF(const NumericalScalar x, const NumericalPoint & y) const";
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
NumericalScalar OrdinalSumCopula::computeConditionalQuantile(const NumericalScalar q, const NumericalPoint & y) const
{
  throw NotYetImplementedException(HERE) << "In OrdinalSumCopula::computeConditionalQuantile(const NumericalScalar q, const NumericalPoint & y) const";
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
OrdinalSumCopula::Implementation OrdinalSumCopula::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension(getDimension());
  if (!indices.check(dimension - 1)) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal distribution must be in the range [0, dim-1] and  must be different";
  const UnsignedInteger size(copulaCollection_.getSize());
  CopulaCollection coll(size);
  for (UnsignedInteger i = 0; i < size; ++i) coll[i] = copulaCollection_[i].getMarginal(indices);
  return new OrdinalSumCopula(coll, bounds_);
}

/* Parameters value and description accessor */
OrdinalSumCopula::NumericalPointWithDescriptionCollection OrdinalSumCopula::getParametersCollection() const
{
  NumericalPointWithDescriptionCollection parameters(1);
  // No marginal parameter
  // Put the dependence parameters
  NumericalPointWithDescription point(0);
  Description description(0);
  const UnsignedInteger size(copulaCollection_.getSize());
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // All distributions, including copulas, must output a collection of NumericalPoint of size at least 1,
    // even if the NumericalPoint are empty
    const NumericalPointWithDescription copulaParameters(copulaCollection_[i].getParametersCollection()[0]);
    const Description parametersDescription(copulaParameters.getDescription());
    const String copulaName(copulaCollection_[i].getName());
    const UnsignedInteger parameterDimension(copulaParameters.getDimension());
    for (UnsignedInteger j = 0; j < parameterDimension; ++j)
    {
      point.add(copulaParameters[j]);
      description.add(OSS() << parametersDescription[j] << "_copula_" << i);
    }
  }
  point.setDescription(description);
  point.setName(getName());
  parameters[0] = point;
  return parameters;
} // getParametersCollection

void OrdinalSumCopula::setParametersCollection(const NumericalPointCollection & parametersCollection)
{
  // Check if the given parameters are ok
  if (parametersCollection.getSize() != 1) throw InvalidArgumentException(HERE) << "Error: the given collection has a size=" << parametersCollection.getSize() << " but should be of size=1";
  // Dependence parameters
  const NumericalPoint parameters(parametersCollection[0]);
  const UnsignedInteger parametersDimension(parameters.getDimension());
  // Index within the given parametersCollection
  UnsignedInteger globalIndex(0);
  const UnsignedInteger size(copulaCollection_.getSize());
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // All distributions, including copulas, must output a collection of NumericalPoint of size at least 1,
    // even if the NumericalPoint are empty
    const UnsignedInteger atomParametersDimension(copulaCollection_[i].getParametersCollection()[0].getDimension());
    // ith copula parameters
    NumericalPoint point(atomParametersDimension);
    for (UnsignedInteger j = 0; j < atomParametersDimension; ++j)
    {
      if (globalIndex >= parametersDimension) throw InvalidArgumentException(HERE) << "Error: there are too few dependence parameters";
      point[j] = parameters[globalIndex];
      ++globalIndex;
    } // atom parameters
    copulaCollection_[i].setParametersCollection(NumericalPointCollection(1, point));
  } // atoms
  if (globalIndex != parametersDimension) throw InvalidArgumentException(HERE) << "Error: there are too many dependence parameters, expected " << globalIndex << " parameters and got " << parametersDimension;
}

NumericalPoint OrdinalSumCopula::getParameter() const
{
  NumericalPoint point;
  const UnsignedInteger size = copulaCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    point.add(copulaCollection_[i].getParameter());
  }
  return point;
}

void OrdinalSumCopula::setParameter(const NumericalPoint & parameter)
{
  UnsignedInteger globalIndex = 0;
  const UnsignedInteger size = copulaCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    // All distributions, including copulas, must output a collection of NumericalPoint of size at least 1,
    // even if the NumericalPoint are empty
    const UnsignedInteger atomParametersDimension = copulaCollection_[i].getParameterDimension();
    if (globalIndex + atomParametersDimension > parameter.getSize()) throw InvalidArgumentException(HERE) << "Error: there are too few dependence parameters";
    // ith copula parameters
    NumericalPoint newParameter(atomParametersDimension);
    std::copy(parameter.begin() + globalIndex, parameter.begin() + globalIndex + atomParametersDimension, newParameter.begin());
    copulaCollection_[i].setParameter(newParameter);
  } // atoms
}

Description OrdinalSumCopula::getParameterDescription() const
{
  Description description;
  const UnsignedInteger size = copulaCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const Description parameterDescription(copulaCollection_[i].getParameterDescription());
    const UnsignedInteger parameterDimension(parameterDescription.getSize());
    for (UnsignedInteger j = 0; j < parameterDimension; ++j)
    {
      description.add(OSS() << parameterDescription[j] << "_copula_" << i);
    }
  }
  return description;
}

/* Tell if the distribution has elliptical copula */
Bool OrdinalSumCopula::hasEllipticalCopula() const
{
  return (copulaCollection_.getSize() == 1) && copulaCollection_[0].hasEllipticalCopula();
}

/* Tell if the distribution has independent copula */
Bool OrdinalSumCopula::hasIndependentCopula() const
{
  return (copulaCollection_.getSize() == 1) && copulaCollection_[0].hasIndependentCopula();
}

/* Get the isoprobabilist transformation */
OrdinalSumCopula::IsoProbabilisticTransformation OrdinalSumCopula::getIsoProbabilisticTransformation() const
{
  throw NotYetImplementedException(HERE) << "In OrdinalSumCopula::getIsoProbabilisticTransformation() const";
}

/* Get the inverse isoprobabilist transformation */
OrdinalSumCopula::InverseIsoProbabilisticTransformation OrdinalSumCopula::getInverseIsoProbabilisticTransformation() const
{
  throw NotYetImplementedException(HERE) << "In OrdinalSumCopula::getInverseIsoProbabilisticTransformation() const";
}

/* Method save() stores the object through the StorageManager */
void OrdinalSumCopula::save(Advocate & adv) const
{
  CopulaImplementation::save(adv);
  adv.saveAttribute( "copulaCollection_", copulaCollection_ );
  adv.saveAttribute( "bounds_", bounds_ );
  adv.saveAttribute( "blockLengths_", blockLengths_ );
  adv.saveAttribute( "blockDistribution_", blockDistribution_ );
}

/* Method load() reloads the object from the StorageManager */
void OrdinalSumCopula::load(Advocate & adv)
{
  CopulaImplementation::load(adv);
  adv.loadAttribute( "copulaCollection_", copulaCollection_ );
  adv.loadAttribute( "bounds_", bounds_ );
  adv.loadAttribute( "blockLengths_", blockLengths_ );
  adv.loadAttribute( "blockDistribution_", blockDistribution_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
