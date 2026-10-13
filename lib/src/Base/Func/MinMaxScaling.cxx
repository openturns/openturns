//                                               -*- C++ -*-
/**
 *  @brief Min-max scaling
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
#include "openturns/MinMaxScaling.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include <cmath>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MinMaxScaling)

static const Factory<MinMaxScaling> Factory_MinMaxScaling;

/* Default constructor */
MinMaxScaling::MinMaxScaling()
  : PersistentObject()
  , minimum_(1, 0.0)
  , maximum_(1, 1.0)
{
  // Nothing to do
}

/* Parameter constructor */
MinMaxScaling::MinMaxScaling(const Point & minimum,
                             const Point & maximum)
  : PersistentObject()
  , minimum_(minimum)
  , maximum_(maximum)
{
  if (minimum.getDimension() != maximum.getDimension()) throw InvalidArgumentException(HERE) << "Error: minimum dimension (" << minimum.getDimension() << ") does not match maximum dimension (" << maximum.getDimension() << ")";
  if (!(minimum.getDimension() > 0)) throw InvalidArgumentException(HERE) << "Error: minimum and maximum must be non-empty";
  for (UnsignedInteger i = 0; i < minimum.getDimension(); ++i)
    if (!(maximum[i] > minimum[i])) throw InvalidArgumentException(HERE) << "Error: maximum must be strictly greater than minimum, got minimum[" << i << "]=" << minimum[i] << " and maximum[" << i << "]=" << maximum[i];
  for (UnsignedInteger i = 0; i < minimum.getDimension(); ++i)
    if (!std::isfinite(maximum[i] - minimum[i]) || !std::isfinite(1.0 / (maximum[i] - minimum[i]))) throw InvalidArgumentException(HERE) << "Error: maximum and minimum must have a finite difference with a finite reciprocal, got minimum[" << i << "]=" << minimum[i] << " and maximum[" << i << "]=" << maximum[i];
}

/* Virtual constructor */
MinMaxScaling * MinMaxScaling::clone() const
{
  return new MinMaxScaling(*this);
}

/* Minimum accessor */
Point MinMaxScaling::getMinimum() const
{
  return minimum_;
}

/* Maximum accessor */
Point MinMaxScaling::getMaximum() const
{
  return maximum_;
}

/* Forward scaling accessor */
Function MinMaxScaling::getTransform() const
{
  const UnsignedInteger dimension = minimum_.getDimension();
  Matrix linear(dimension, dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) linear(i, i) = 1.0 / (maximum_[i] - minimum_[i]);
  return LinearFunction(minimum_, Point(dimension, 0.0), linear);
}

/* Inverse scaling accessor */
Function MinMaxScaling::getInverseTransform() const
{
  const UnsignedInteger dimension = minimum_.getDimension();
  Matrix linear(dimension, dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) linear(i, i) = maximum_[i] - minimum_[i];
  return LinearFunction(Point(dimension, 0.0), minimum_, linear);
}

/* String converter */
String MinMaxScaling::__repr__() const
{
  OSS oss(true);
  oss << "class=" << MinMaxScaling::GetClassName()
      << " name=" << getName();
  return oss;
}

/* String converter */
String MinMaxScaling::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << "MinMaxScaling(minimum=" << minimum_.__str__()
      << ", maximum=" << maximum_.__str__() << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void MinMaxScaling::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("minimum_", minimum_);
  adv.saveAttribute("maximum_", maximum_);
}

/* Method load() reloads the object from the StorageManager */
void MinMaxScaling::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("minimum_", minimum_);
  adv.loadAttribute("maximum_", maximum_);
}

END_NAMESPACE_OPENTURNS
