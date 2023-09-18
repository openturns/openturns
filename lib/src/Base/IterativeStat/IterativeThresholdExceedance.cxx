//                                               -*- C++ -*-
/**
 *  @brief IterativeThresholdExceedance
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/IterativeThresholdExceedance.hxx"
#include "openturns/IterativeAlgorithm.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Point.hxx"
#include "openturns/IterativeThresholdExceedance.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IterativeThresholdExceedance)

static const Factory<IterativeThresholdExceedance> Factory_IterativeThresholdExceedance;

/* Default constructor */
IterativeThresholdExceedance::IterativeThresholdExceedance(const UnsignedInteger dimension,
                                                           const Scalar threshold)
  : IterativeAlgorithmImplementation(dimension)
  , operator_(Greater())
  , thresholdValue_(threshold)
  , data_(dimension_, 0.0)
{
  LOGWARN(OSS() << "IterativeThresholdExceedance(dimension, threshold) is deprecated in favor of IterativeThresholdExceedance(dimension, ComparisonOperator, threshold)");
}

/* Default constructor */
IterativeThresholdExceedance::IterativeThresholdExceedance(const UnsignedInteger dimension,
                                                           const ComparisonOperator & op,
                                                           const Scalar threshold)
  : IterativeAlgorithmImplementation(dimension)
  , operator_(op)
  , thresholdValue_(threshold)
  , data_(dimension_, 0.0)
{
  // Nothing to do
}

/* Virtual constructor */
IterativeThresholdExceedance * IterativeThresholdExceedance::clone() const
{
  return new IterativeThresholdExceedance(*this);
}

/* String converter */
String IterativeThresholdExceedance::__repr__() const
{
  OSS oss(true);
  oss << "class=" << IterativeThresholdExceedance::GetClassName()
      << " iteration=" << iteration_
      << " dimension=" << dimension_
      << " threshold value=" << thresholdValue_
      << " values=" << data_.__repr__();
  return oss;
}

/* String converter */
String IterativeThresholdExceedance::__str__(const String & offset) const
{
  return data_.__str__(offset);
}

/* Get the threshold value used */
Scalar IterativeThresholdExceedance::getThresholdValue() const
{
  return thresholdValue_;
}

/* Get the values exceeding the specified threshold */
Point IterativeThresholdExceedance::getThresholdExceedance() const
{
  return data_;
}

/* Increment the internal data with a Point */
void IterativeThresholdExceedance::increment(const Point & newData)
{
  if (newData.getSize() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given Point is not compatible with the dimension of the iterative threshold exceedance.";
  ++ iteration_;
  for (UnsignedInteger i = 0; i < dimension_; ++i)
    if (operator_(newData[i], thresholdValue_)) data_[i] += 1.0;
}

/* Increment the internal data with a Sample */
void IterativeThresholdExceedance::increment(const Sample & newData)
{
  if (newData.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given Sample is not compatible with the dimension of the iterative threshold exceedance.";

  for (UnsignedInteger j = 0; j < newData.getSize(); ++j) increment(newData[j]);
}

/* Method save() stores the object through the StorageManager */
void IterativeThresholdExceedance::save(Advocate & adv) const
{
  IterativeAlgorithmImplementation::save(adv);
  adv.saveAttribute( "operator_", operator_);
  adv.saveAttribute( "thresholdValue_", thresholdValue_);
  adv.saveAttribute( "data_", data_);
}


/* Method load() reloads the object from the StorageManager */
void IterativeThresholdExceedance::load(Advocate & adv)
{
  IterativeAlgorithmImplementation::load(adv);
  if (adv.hasAttribute("operator_"))
    adv.loadAttribute( "operator_", operator_);
  else
    operator_ = Greater();// OT<1.21
  adv.loadAttribute( "thresholdValue_", thresholdValue_);
  adv.loadAttribute( "data_", data_);
}

END_NAMESPACE_OPENTURNS
