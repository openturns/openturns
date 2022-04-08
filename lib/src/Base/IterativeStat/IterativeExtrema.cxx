//                                               -*- C++ -*-
/**
 *  @brief IterativeExtrema implements an iterative computation of min and max
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
#include "openturns/IterativeExtrema.hxx"
#include "openturns/IterativeAlgorithm.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Point.hxx"
#include "openturns/IterativeExtrema.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IterativeExtrema)

static const Factory<IterativeExtrema> Factory_IterativeExtrema;

/* Constructor */
IterativeExtrema::IterativeExtrema(const UnsignedInteger dimension)
  : IterativeAlgorithmImplementation(dimension)
  , minData_(dimension, 0.0)
  , maxData_(dimension, 0.0)
{
  // Nothing to do
}

/* Virtual constructor */
IterativeExtrema * IterativeExtrema::clone() const
{
  return new IterativeExtrema(*this);
}

/* String converter */
String IterativeExtrema::__repr__() const
{
  OSS oss(true);
  oss << "class=" << IterativeExtrema::GetClassName()
      << " iteration=" << iteration_
      << " dimension=" << dimension_
      << " min=" << minData_.__repr__()
      << " max=" << maxData_.__repr__();
  return oss;
}

/* String converter */
String IterativeExtrema::__str__(const String &) const
{
  OSS oss(false);
  oss << getClassName() << "(";
  String separator("");
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    oss << separator << "(min = " << minData_[i] << ", max = " << maxData_[i] << ")";
    separator = ", ";
  }
  return oss;
}

/* Accessor to the min */
Point IterativeExtrema::getMin() const
{
  return minData_;
}

/* Accessor to the max */
Point IterativeExtrema::getMax() const
{
  return maxData_;
}

/* Increment method from a Point */
void IterativeExtrema::increment(const Point & newData)
{
  if (newData.getSize() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given Point is not compatible with the dimension of the iterative extrema.";
  ++ iteration_;
  if (iteration_ > 1)
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      if (newData[i] > maxData_[i]) maxData_[i] = newData[i];
      if (newData[i] < minData_[i]) minData_[i] = newData[i];
    } // loop over the dimensions
  }
  else
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      maxData_[i] = newData[i];
      minData_[i] = newData[i];
    } // loop over the dimensions
  }
}

/* Increment method from a Sample */
void IterativeExtrema::increment(const Sample & newData)
{
  for (UnsignedInteger j = 0; j < newData.getSize(); ++j) increment(newData[j]);
}

/* Method save() stores the object through the StorageManager */
void IterativeExtrema::save(Advocate & adv) const
{
  IterativeAlgorithmImplementation::save(adv);
  adv.saveAttribute( "minData_", minData_);
  adv.saveAttribute( "maxData_", maxData_);
}


/* Method load() reloads the object from the StorageManager */
void IterativeExtrema::load(Advocate & adv)
{
  IterativeAlgorithmImplementation::load(adv);
  adv.loadAttribute( "minData_", minData_);
  adv.loadAttribute( "maxData_", maxData_);
}

END_NAMESPACE_OPENTURNS
