//                                               -*- C++ -*-
/**
 *  @brief Implementation for Sobol indices
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/SobolIndicesImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SobolIndicesImplementation)

static const Factory<SobolIndicesImplementation> Factory_SobolIndicesImplementation;

/* Default constructor */
SobolIndicesImplementation::SobolIndicesImplementation()
  : PersistentObject()
  , referenceMean_()
  , referenceVariance_()
  , iterations_(0) // Number of currently computed iterations
  , modelInputDimension_(0) // nb_parameters
  , modelOutputDimension_(0) // vect_size
  , firstOrderValues_()
  , totalOrderValues_()
{
 // Nothing to do
}

/* Default constructor */
SobolIndicesImplementation::SobolIndicesImplementation(const UnsignedInteger modelInputDimension,
                                                       const UnsignedInteger modelOutputDimension)
  : PersistentObject()
  , referenceMean_(modelOutputDimension)
  , referenceVariance_(modelOutputDimension)
  , iterations_(0) // Number of currently computed iterations
  , modelInputDimension_(modelInputDimension) // nb_parameters
  , modelOutputDimension_(modelOutputDimension) // vect_size
  , firstOrderValues_(modelOutputDimension, modelInputDimension)
  , totalOrderValues_(modelOutputDimension, modelInputDimension)
{
 // Nothing to do
}

/* Virtual constructor */
SobolIndicesImplementation * SobolIndicesImplementation::clone() const
{
  return new SobolIndicesImplementation(*this);
}

void SobolIndicesImplementation::SetModelDimensions(const UnsignedInteger modelInputDimension,
                                                    const UnsignedInteger modelOutputDimension)
{
  modelInputDimension_ = modelInputDimension;
  modelOutputDimension_ = modelOutputDimension;
  referenceVariance_ = Point(modelOutputDimension);
  referenceMean_ = Point(modelOutputDimension);
  firstOrderValues_ = Sample(modelOutputDimension, modelInputDimension);
  totalOrderValues_ = Sample(modelOutputDimension, modelInputDimension);
}

UnsignedInteger SobolIndicesImplementation::GetModelInputDimensions()
{
  return modelInputDimension_;
}

UnsignedInteger SobolIndicesImplementation::GetModelOutputDimensions()
{
  return modelOutputDimension_;
}

void SobolIndicesImplementation::computeIndices(const Sample & inputSample)
{
  // Method is defined in Jansen/Saltelli/Martinez/Mauntz classes
  throw new NotYetImplementedException(HERE);
}

void SobolIndicesImplementation::incrementIndices(const Sample & inputSample)
{
  // Method is defined in Jansen/Saltelli/Martinez/Mauntz classes
  throw new NotYetImplementedException(HERE);
}

Point SobolIndicesImplementation::getMean() const
{
  return referenceMean_;
}

Point SobolIndicesImplementation::getVariance() const
{
  return referenceVariance_;
}

Point SobolIndicesImplementation::getFirstOrderIndices(const UnsignedInteger marginalIndex) const
{
  if (0 == iterations_)
  {
    throw InvalidArgumentException(HERE) << "In SobolIndicesImplementation::getFirstOrderIndices, not computed. Call SobolIndicesImplementation::computeIndices or SobolIndicesImplementation::incrementIndices first.";
  }
  if (marginalIndex >= modelOutputDimension_)
    throw InvalidArgumentException(HERE) << "In SobolIndicesImplementation::getFirstOrderIndices, marginalIndex should be in [0," << modelOutputDimension_ - 1;

  for (UnsignedInteger p = 0; p < modelInputDimension_; ++p)
  {
    if ((firstOrderValues_[marginalIndex][p] > 1.0) || firstOrderValues_[marginalIndex][p] < 0.0)
      LOGWARN(OSS() << "The estimated first order Sobol index (" << p << ") is not in the range [0, 1]. You may increase the sampling size. HERE we have: S_"
              << p << "=" <<  firstOrderValues_ << ", ST_" << p << "=" << totalOrderValues_);
    // Another case : Si > STi
    if (firstOrderValues_[marginalIndex][p] > totalOrderValues_[marginalIndex][p])
      LOGWARN(OSS() << "The estimated first order Sobol index (" << p << ") is greater than its total order index . You may increase the sampling size. HERE we have: S_"
              << p << "=" <<  firstOrderValues_ << ", ST_" << p << "=" << totalOrderValues_);
  }
  return firstOrderValues_[marginalIndex];
}

Point SobolIndicesImplementation::getTotalOrderIndices(const UnsignedInteger marginalIndex) const
{
  if (0 == iterations_)
  {
    throw InvalidArgumentException(HERE) << "In SobolIndicesImplementation::getFirstOrderIndices, not computed. Call SobolIndicesImplementation::computeIndices or SobolIndicesImplementation::incrementIndices first.";
  }
  if (marginalIndex >= modelOutputDimension_)
    throw InvalidArgumentException(HERE) << "In SobolIndicesImplementation::getFirstOrderIndices, marginalIndex should be in [0," << modelOutputDimension_ - 1;

  for (UnsignedInteger p = 0; p < modelInputDimension_; ++p)
  {
    if ((firstOrderValues_[marginalIndex][p] > 1.0) || firstOrderValues_[marginalIndex][p] < 0.0)
      LOGWARN(OSS() << "The estimated first order Sobol index (" << p << ") is not in the range [0, 1]. You may increase the sampling size. HERE we have: S_"
              << p << "=" <<  firstOrderValues_ << ", ST_" << p << "=" << totalOrderValues_);
    // Another case : Si > STi
    if (firstOrderValues_[marginalIndex][p] > totalOrderValues_[marginalIndex][p])
      LOGWARN(OSS() << "The estimated first order Sobol index (" << p << ") is greater than its total order index . You may increase the sampling size. HERE we have: S_"
              << p << "=" <<  firstOrderValues_ << ", ST_" << p << "=" << totalOrderValues_);
  }
  return totalOrderValues_[marginalIndex];
}

Interval SobolIndicesImplementation::getFirstOrderIndicesInterval() const
{
  // Method is defined in Jansen/Saltelli/Martinez/Mauntz classes
  throw new NotYetImplementedException(HERE);
}

Interval SobolIndicesImplementation::getTotalOrderIndicesInterval() const
{
  // Method is defined in Jansen/Saltelli/Martinez/Mauntz classes
  throw new NotYetImplementedException(HERE);
}

Point SobolIndicesImplementation::getAggregatedFirstOrderIndices() const
{
  Point aggregatedFirstOrderIndices(modelInputDimension_);
  for (UnsignedInteger i = 0; i < modelInputDimension_; i++)
  {
    for (UnsignedInteger j = 0; j < modelOutputDimension_; j++)
    {
      aggregatedFirstOrderIndices[i] += firstOrderValues_(j, i);
    }
  }

  return aggregatedFirstOrderIndices /= modelOutputDimension_;
}

Point SobolIndicesImplementation::getAggregatedTotalOrderIndices() const
{
  Point aggregatedTotalOrderIndices(modelInputDimension_);
  for (UnsignedInteger i = 0; i < modelInputDimension_; i++)
  {
    for (UnsignedInteger j = 0; j < modelOutputDimension_; j++)
    {
      aggregatedTotalOrderIndices[i] += totalOrderValues_(j, i);
    }
  }

  return aggregatedTotalOrderIndices /= modelOutputDimension_;
}

// Multiplication and sum of two Samples
// TODO Write method in Sample ?
Point SobolIndicesImplementation::computeSumDotSamples(const Sample & x,
    const Sample & y) const
{
  // Internal method
  // Suppose that samples have the same size, same dimension
  const UnsignedInteger dimension = x.getDimension();
  const UnsignedInteger size = x.getSize();

  const Scalar * xptr(&x(0, 0));
  const Scalar * yptr(&y(0, 0));


  Point value(dimension, 0.0);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < dimension; ++j, ++xptr, ++yptr)
      value[j] += (*xptr) * (*yptr);
  return value;
}

// Multiplication and sum of two Samples, contained in the sampe sample
Point SobolIndicesImplementation::computeSumDotSamples(const Sample & sample,
    const UnsignedInteger size,
    const UnsignedInteger indexX,
    const UnsignedInteger indexY) const
{
  // Internal method
  // Suppose that samples have the same size, same dimension
  const UnsignedInteger dimension = sample.getDimension();

  const Scalar * xptr(&sample(indexX, 0));
  const Scalar * yptr(&sample(indexY, 0));


  Point value(dimension, 0.0);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < dimension; ++j, ++xptr, ++yptr)
      value[j] += (*xptr) * (*yptr);
  return value;
}

/* Multiplication of two sub-samples */
Sample SobolIndicesImplementation::ComputeProdSample(const Sample & sample,
    const UnsignedInteger marginalIndex,
    const UnsignedInteger size,
    const UnsignedInteger indexX,
    const UnsignedInteger indexY)
{
  Sample prod(size, 1);
  for (UnsignedInteger i = 0; i < size; ++ i)
    prod(i, 0) = sample(i + indexX, marginalIndex) * sample(i + indexY, marginalIndex);
  return prod;
}

/* String converter */
String SobolIndicesImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName();
  return oss;
}

/* String converter */
String SobolIndicesImplementation::__str__(const String & ) const
{
  return __repr__();
}

/* Method save() stores the object through the StorageManager */
void SobolIndicesImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "referenceMean_",  referenceMean_);
  adv.saveAttribute( "referenceVariance_",  referenceVariance_);
  adv.saveAttribute( "iterations_", iterations_);
  adv.saveAttribute( "modelInputDimension_", modelInputDimension_);
  adv.saveAttribute( "modelOutputDimension_", modelOutputDimension_);
  adv.saveAttribute( "firstOrderValues_", firstOrderValues_);
  adv.saveAttribute( "totalOrderValues_", totalOrderValues_);
}

/* Method load() reloads the object from the StorageManager */
void SobolIndicesImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "referenceMean_",  referenceMean_);
  adv.loadAttribute( "referenceVariance_",  referenceVariance_);
  adv.loadAttribute( "iterations_", iterations_);
  adv.saveAttribute( "modelInputDimension_", modelInputDimension_);
  adv.saveAttribute( "modelOutputDimension_", modelOutputDimension_);
  adv.saveAttribute( "firstOrderValues_", firstOrderValues_);
  adv.saveAttribute( "totalOrderValues_", totalOrderValues_);
}

END_NAMESPACE_OPENTURNS


