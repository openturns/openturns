//                                               -*- C++ -*-
/**
 *  @brief Sum of covariance models on the same input/output spaces
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/SumCovarianceModel.hxx"
#include "openturns/ScaledCovarianceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/AbsoluteExponential.hxx"
#include "openturns/OSS.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SumCovarianceModel)

static const Factory<SumCovarianceModel> Factory_SumCovarianceModel;

/* Default constructor */
SumCovarianceModel::SumCovarianceModel(const UnsignedInteger inputDimension)
  : CovarianceModelImplementation(inputDimension)
  , collection_(0)
{
  if (!(inputDimension > 0))
    throw InvalidArgumentException(HERE) << "Error: input dimension must be positive, here inputDimension=" << inputDimension;
  CovarianceModelCollection collection(2, AbsoluteExponential(inputDimension));
  setCollection(collection);
}

/* Collection constructor */
SumCovarianceModel::SumCovarianceModel(const CovarianceModelCollection & collection)
  : CovarianceModelImplementation()
  , collection_(0)
{
  setCollection(collection);
}

/* Collection accessor */
void SumCovarianceModel::setCollection(const CovarianceModelCollection & collection)
{
  const UnsignedInteger size = collection.getSize();
  if (!(size > 0))
    throw InvalidArgumentException(HERE) << "Error: the collection must have a positive size, here size=0";
  const UnsignedInteger inputDimension = collection[0].getInputDimension();
  const UnsignedInteger outputDimension = collection[0].getOutputDimension();
  Bool stationary = collection[0].isStationary();
  Bool diagonal = collection[0].isDiagonal();
  for (UnsignedInteger k = 1; k < size; ++k)
  {
    if (collection[k].getInputDimension() != inputDimension)
      throw InvalidArgumentException(HERE) << "In SumCovarianceModel::setCollection, incompatible input dimension of the element #" << k
                                           << ", input dimension of element=" << collection[k].getInputDimension()
                                           << ", expected input dimension=" << inputDimension;
    if (collection[k].getOutputDimension() != outputDimension)
      throw InvalidArgumentException(HERE) << "In SumCovarianceModel::setCollection, incompatible output dimension of the element #" << k
                                           << ", output dimension of element=" << collection[k].getOutputDimension()
                                           << ", expected output dimension=" << outputDimension;
    if (!collection[k].isStationary()) stationary = false;
    if (!collection[k].isDiagonal()) diagonal = false;
  }
  inputDimension_ = inputDimension;
  outputDimension_ = outputDimension;
  isStationary_ = stationary;
  isDiagonal_ = diagonal;
  // Dummy base fields: the authoritative parameters live in the members.
  // They are kept consistent for persistence compatibility only.
  scale_ = Point(inputDimension_, 1.0);
  amplitude_ = Point(outputDimension_, 1.0);
  nuggetFactor_ = 0.0;
  outputCorrelation_ = CorrelationMatrix();
  updateOutputCovariance();
  collection_ = collection;
  // Active parameter is the shifted union of member active parameters
  activeParameter_ = Indices(0);
  UnsignedInteger offset = 0;
  for (UnsignedInteger k = 0; k < size; ++k)
  {
    const UnsignedInteger localFullSize = collection[k].getFullParameter().getSize();
    const Indices localActive(collection[k].getActiveParameter());
    for (UnsignedInteger j = 0; j < localActive.getSize(); ++j)
      activeParameter_.add(offset + localActive[j]);
    offset += localFullSize;
  }
}

SumCovarianceModel::CovarianceModelCollection SumCovarianceModel::getCollection() const
{
  return collection_;
}

/* Virtual constructor */
SumCovarianceModel * SumCovarianceModel::clone() const
{
  return new SumCovarianceModel(*this);
}

/* Comparison operators */
Bool SumCovarianceModel::operator ==(const SumCovarianceModel & other) const
{
  if (this == &other) return true;
  if (!hasEqualBase(other)) return false;
  if (collection_.getSize() != other.collection_.getSize()) return false;
  for (UnsignedInteger k = 0; k < collection_.getSize(); ++k)
    if (!(collection_[k] == other.collection_[k])) return false;
  return true;
}

Bool SumCovarianceModel::equals(const CovarianceModelImplementation & other) const
{
  const SumCovarianceModel * p_other = dynamic_cast<const SumCovarianceModel *>(&other);
  return p_other && (*this == *p_other);
}

/* Evaluation */
SquareMatrix SumCovarianceModel::operator()(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  SquareMatrix result(collection_[0](s, t));
  for (UnsignedInteger k = 1; k < collection_.getSize(); ++k)
    result = result + collection_[k](s, t);
  return result;
}

SquareMatrix SumCovarianceModel::operator()(const Point & tau) const
{
  if (!isStationary()) return CovarianceModelImplementation::operator()(tau);
  if (tau.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the lag has dimension=" << tau.getDimension() << ", expected dimension=" << inputDimension_;
  SquareMatrix result(collection_[0](tau));
  for (UnsignedInteger k = 1; k < collection_.getSize(); ++k)
    result = result + collection_[k](tau);
  return result;
}

Scalar SumCovarianceModel::computeAsScalar(const Point & s,
    const Point & t) const
{
  if (outputDimension_ != 1)
    throw InvalidArgumentException(HERE) << "Error: computeAsScalar is only defined for output dimension 1, here output dimension=" << outputDimension_;
  Scalar result = collection_[0].computeAsScalar(s, t);
  for (UnsignedInteger k = 1; k < collection_.getSize(); ++k)
    result += collection_[k].computeAsScalar(s, t);
  return result;
}

Scalar SumCovarianceModel::computeAsScalar(const Point & tau) const
{
  if (outputDimension_ != 1)
    throw InvalidArgumentException(HERE) << "Error: computeAsScalar is only defined for output dimension 1, here output dimension=" << outputDimension_;
  Scalar result = collection_[0].computeAsScalar(tau);
  for (UnsignedInteger k = 1; k < collection_.getSize(); ++k)
    result += collection_[k].computeAsScalar(tau);
  return result;
}

Scalar SumCovarianceModel::computeAsScalar(const Collection<Scalar>::const_iterator & s_begin,
    const Collection<Scalar>::const_iterator & t_begin) const
{
  if (outputDimension_ != 1)
    throw InvalidArgumentException(HERE) << "Error: computeAsScalar is only defined for output dimension 1, here output dimension=" << outputDimension_;
  Scalar result = collection_[0].getImplementation()->computeAsScalar(s_begin, t_begin);
  for (UnsignedInteger k = 1; k < collection_.getSize(); ++k)
    result += collection_[k].getImplementation()->computeAsScalar(s_begin, t_begin);
  return result;
}

Scalar SumCovarianceModel::computeAsScalar(const Scalar s,
    const Scalar t) const
{
  if (inputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", expected input dimension=1.";
  if (outputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has output dimension=" << outputDimension_ << ", expected dimension=1.";
  Scalar result = collection_[0].computeAsScalar(s, t);
  for (UnsignedInteger k = 1; k < collection_.getSize(); ++k)
    result += collection_[k].computeAsScalar(s, t);
  return result;
}

Scalar SumCovarianceModel::computeAsScalar(const Scalar tau) const
{
  if (inputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", expected input dimension=1.";
  if (outputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has output dimension=" << outputDimension_ << ", expected dimension=1.";
  Scalar result = collection_[0].computeAsScalar(tau);
  for (UnsignedInteger k = 1; k < collection_.getSize(); ++k)
    result += collection_[k].computeAsScalar(tau);
  return result;
}

/* Gradient with respect to the input */
Matrix SumCovarianceModel::partialGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  Matrix result(collection_[0].partialGradient(s, t));
  for (UnsignedInteger k = 1; k < collection_.getSize(); ++k)
    result = result + collection_[k].partialGradient(s, t);
  return result;
}

/* Gradient with respect to the parameters: row-wise concatenation of
 * member gradients, rows following the active parameter order */
Matrix SumCovarianceModel::parameterGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  const UnsignedInteger activeSize = activeParameter_.getSize();
  const UnsignedInteger columns = (outputDimension_ == 1) ? 1 : outputDimension_ * outputDimension_;
  Matrix result(activeSize, columns);
  // Offsets of member full parameters within the sum full parameter
  const UnsignedInteger size = collection_.getSize();
  Indices offsets(size + 1);
  offsets[0] = 0;
  for (UnsignedInteger k = 0; k < size; ++k)
    offsets[k + 1] = offsets[k] + collection_[k].getFullParameter().getSize();
  for (UnsignedInteger r = 0; r < activeSize; ++r)
  {
    const UnsignedInteger global = activeParameter_[r];
    UnsignedInteger k = 0;
    while ((k + 1 < size) && (global >= offsets[k + 1])) ++k;
    const UnsignedInteger local = global - offsets[k];
    const Matrix localGradient(collection_[k].parameterGradient(s, t));
    const Indices localActive(collection_[k].getActiveParameter());
    UnsignedInteger localRow = localActive.getSize();
    for (UnsignedInteger j = 0; j < localActive.getSize(); ++j)
      if (localActive[j] == local)
      {
        localRow = j;
        break;
      }
    if (!(localRow < localGradient.getNbRows()))
      throw InternalException(HERE) << "In SumCovarianceModel::parameterGradient: inconsistent active parameters";
    for (UnsignedInteger c = 0; c < columns; ++c)
      result(r, c) = localGradient(localRow, c);
  }
  return result;
}

/* Marginal accessor */
CovarianceModel SumCovarianceModel::getMarginal(const UnsignedInteger index) const
{
  if (!(index < outputDimension_)) throw InvalidArgumentException(HERE) << "Error: index=" << index << " must be less than output dimension=" << outputDimension_;
  CovarianceModelCollection marginals(collection_.getSize());
  for (UnsignedInteger k = 0; k < collection_.getSize(); ++k)
    marginals[k] = collection_[k].getMarginal(index);
  return SumCovarianceModel(marginals);
}

/* Marginal accessor */
CovarianceModel SumCovarianceModel::getMarginal(const Indices & indices) const
{
  if (!indices.check(outputDimension_)) throw InvalidArgumentException(HERE) << "Error: indices=" << indices << " must be less than output dimension=" << outputDimension_;
  CovarianceModelCollection marginals(collection_.getSize());
  for (UnsignedInteger k = 0; k < collection_.getSize(); ++k)
    marginals[k] = collection_[k].getMarginal(indices);
  return SumCovarianceModel(marginals);
}

/* Generic accessors redirect to members or full parameter */
Point SumCovarianceModel::getScale() const
{
  throw NotDefinedException(HERE) << "In SumCovarianceModel::getScale: no unique scale at the sum level, use getCollection() or getFullParameter() instead.";
}

void SumCovarianceModel::setScale(const Point &)
{
  throw NotDefinedException(HERE) << "In SumCovarianceModel::setScale: no unique scale at the sum level, use getCollection() or setFullParameter() instead.";
}

Point SumCovarianceModel::getAmplitude() const
{
  throw NotDefinedException(HERE) << "In SumCovarianceModel::getAmplitude: no unique amplitude at the sum level, use getCollection() or getFullParameter() instead.";
}

void SumCovarianceModel::setAmplitude(const Point &)
{
  throw NotDefinedException(HERE) << "In SumCovarianceModel::setAmplitude: no unique amplitude at the sum level, use getCollection() or setFullParameter() instead.";
}

CorrelationMatrix SumCovarianceModel::getOutputCorrelation() const
{
  throw NotDefinedException(HERE) << "In SumCovarianceModel::getOutputCorrelation: no unique output correlation at the sum level, use getCollection() instead.";
}

void SumCovarianceModel::setOutputCorrelation(const CorrelationMatrix &)
{
  throw NotDefinedException(HERE) << "In SumCovarianceModel::setOutputCorrelation: no unique output correlation at the sum level, use getCollection() instead.";
}

Scalar SumCovarianceModel::getNuggetFactor() const
{
  throw NotDefinedException(HERE) << "In SumCovarianceModel::getNuggetFactor: no unique nugget factor at the sum level, use getCollection() or getFullParameter() instead.";
}

void SumCovarianceModel::setNuggetFactor(const Scalar)
{
  throw NotDefinedException(HERE) << "In SumCovarianceModel::setNuggetFactor: no unique nugget factor at the sum level, use getCollection() or setFullParameter() instead.";
}

/* Flags */
Bool SumCovarianceModel::isStationary() const
{
  return isStationary_;
}

Bool SumCovarianceModel::isDiagonal() const
{
  return isDiagonal_;
}

Bool SumCovarianceModel::isParallel() const
{
  for (UnsignedInteger k = 0; k < collection_.getSize(); ++k)
    if (!collection_[k].getImplementation()->isParallel()) return false;
  return true;
}

/* String converters */
String SumCovarianceModel::__repr__() const
{
  OSS oss;
  oss << "class=" << SumCovarianceModel::GetClassName()
      << " input dimension=" << inputDimension_
      << " output dimension=" << outputDimension_
      << " models=" << collection_;
  return oss;
}

String SumCovarianceModel::__str__(const String &) const
{
  return __repr__();
}

/* Full parameter: concatenation of member full parameters */
Point SumCovarianceModel::getFullParameter() const
{
  Point result(0);
  for (UnsignedInteger k = 0; k < collection_.getSize(); ++k)
  {
    const Point localFull(collection_[k].getFullParameter());
    for (UnsignedInteger j = 0; j < localFull.getSize(); ++j)
      result.add(localFull[j]);
  }
  return result;
}

void SumCovarianceModel::setFullParameter(const Point & parameter)
{
  UnsignedInteger totalSize = 0;
  for (UnsignedInteger k = 0; k < collection_.getSize(); ++k)
    totalSize += collection_[k].getFullParameter().getSize();
  if (parameter.getSize() != totalSize)
    throw InvalidArgumentException(HERE) << "Error: parameter dimension should be " << totalSize << ", got " << parameter.getDimension();
  UnsignedInteger offset = 0;
  for (UnsignedInteger k = 0; k < collection_.getSize(); ++k)
  {
    const UnsignedInteger localSize = collection_[k].getFullParameter().getSize();
    Point localParameter(localSize);
    std::copy(parameter.begin() + offset, parameter.begin() + offset + localSize, localParameter.begin());
    collection_[k].setFullParameter(localParameter);
    offset += localSize;
  }
}

Description SumCovarianceModel::getFullParameterDescription() const
{
  Description description(0);
  for (UnsignedInteger k = 0; k < collection_.getSize(); ++k)
  {
    const Description localDescription(collection_[k].getFullParameterDescription());
    for (UnsignedInteger j = 0; j < localDescription.getSize(); ++j)
      description.add(OSS() << "model_" << k << "_" << localDescription[j]);
  }
  return description;
}

void SumCovarianceModel::setActiveParameter(const Indices & active)
{
  UnsignedInteger totalSize = 0;
  for (UnsignedInteger k = 0; k < collection_.getSize(); ++k)
    totalSize += collection_[k].getFullParameter().getSize();
  Indices sorted(active);
  std::sort(sorted.begin(), sorted.end());
  const Indices::const_iterator last = std::unique(sorted.begin(), sorted.end());
  if (last != sorted.end())
    throw InvalidArgumentException(HERE) << "Active parameter indices must be unique";
  for (UnsignedInteger j = 0; j < active.getSize(); ++j)
    if (!(active[j] < totalSize))
      throw InvalidArgumentException(HERE) << "Active parameter index " << active[j] << " is out of range [0, " << totalSize << ")";
  // Scatter to members
  UnsignedInteger offset = 0;
  for (UnsignedInteger k = 0; k < collection_.getSize(); ++k)
  {
    const UnsignedInteger localSize = collection_[k].getFullParameter().getSize();
    Indices localActive(0);
    for (UnsignedInteger j = 0; j < active.getSize(); ++j)
      if ((active[j] >= offset) && (active[j] < offset + localSize))
        localActive.add(active[j] - offset);
    collection_[k].setActiveParameter(localActive);
    offset += localSize;
  }
  activeParameter_ = active;
}

/* Persistence */
void SumCovarianceModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute("collection_", collection_);
}

void SumCovarianceModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute("collection_", collection_);
}

/* Build the sum of two models with flattening and identical-atom folding */
CovarianceModel SumCovarianceModel::add(const CovarianceModel & left,
                                        const CovarianceModel & right)
{
  CovarianceModelCollection collection(0);
  appendMerged(collection, left);
  appendMerged(collection, right);
  // A single folded atom needs no sum wrapper
  if (collection.getSize() == 1) return collection[0];
  return SumCovarianceModel(collection);
}

/* Append a model, flattening nested sums and folding identical atoms */
void SumCovarianceModel::appendMerged(CovarianceModelCollection & collection,
                                      const CovarianceModel & model)
{
  // Flatten nested sums
  const SumCovarianceModel * p_sum = dynamic_cast<const SumCovarianceModel *>(model.getImplementation().get());
  if (p_sum != nullptr)
  {
    const CovarianceModelCollection members(p_sum->getCollection());
    for (UnsignedInteger k = 0; k < members.getSize(); ++k)
      appendMerged(collection, members[k]);
    return;
  }
  // Unwrap scaled atoms, accumulating nested factors
  CovarianceModel kernel(model);
  Scalar factor = 1.0;
  const ScaledCovarianceModel * p_scaled = dynamic_cast<const ScaledCovarianceModel *>(kernel.getImplementation().get());
  while (p_scaled != nullptr)
  {
    factor *= p_scaled->getScaleFactor();
    kernel = p_scaled->getKernel();
    p_scaled = dynamic_cast<const ScaledCovarianceModel *>(kernel.getImplementation().get());
  }
  // Fold into an identical atom, summing the factors
  for (UnsignedInteger k = 0; k < collection.getSize(); ++k)
  {
    CovarianceModel existingKernel(collection[k]);
    Scalar existingFactor = 1.0;
    const ScaledCovarianceModel * p_existing = dynamic_cast<const ScaledCovarianceModel *>(existingKernel.getImplementation().get());
    while (p_existing != nullptr)
    {
      existingFactor *= p_existing->getScaleFactor();
      existingKernel = p_existing->getKernel();
      p_existing = dynamic_cast<const ScaledCovarianceModel *>(existingKernel.getImplementation().get());
    }
    if (existingKernel == kernel)
    {
      const Scalar mergedFactor = existingFactor + factor;
      if (mergedFactor == 1.0) collection[k] = kernel;
      else collection[k] = ScaledCovarianceModel(kernel, mergedFactor);
      return;
    }
  }
  collection.add(model);
}

END_NAMESPACE_OPENTURNS
