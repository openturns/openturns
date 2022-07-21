//                                               -*- C++ -*-
/**
 *  @brief
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
#include "openturns/SamplingStrategyImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SamplingStrategyImplementation
 */

CLASSNAMEINIT(SamplingStrategyImplementation)

static const Factory<SamplingStrategyImplementation> Factory_SamplingStrategyImplementation;

/* Constructor with parameters */
SamplingStrategyImplementation::SamplingStrategyImplementation(const UnsignedInteger dimension)
  : PersistentObject()
  , dimension_(dimension)
{
  // Nothing to do
}

/* Virtual constructor */
SamplingStrategyImplementation * SamplingStrategyImplementation::clone() const
{
  return new SamplingStrategyImplementation(*this);
}

/* Generate a set of directions */
Sample SamplingStrategyImplementation::generate() const
{
  throw NotYetImplementedException(HERE) << "In SamplingStrategyImplementation::generate() const";
}

/* Generate a uniform random unit vector */
Point SamplingStrategyImplementation::getUniformUnitVectorRealization(const UnsignedInteger dimension) const
{
  if (dimension == 0)
    throw InvalidArgumentException(HERE) << "Cannot run a 0-d SamplingStrategy.";
  Point direction(dimension);
  Scalar norm = 0.0;
  do
  {
    for (UnsignedInteger i = 0; i < dimension; ++i) direction[i] = DistFunc::rNormal();
    norm = direction.norm();
  }
  while (norm == 0.0);
  return (1.0 / norm) * direction;
}

/* Generate a uniform random unit vector */
Point SamplingStrategyImplementation::getUniformUnitVectorRealization() const
{
  return getUniformUnitVectorRealization(dimension_);
}

/* Dimension accessor */
void SamplingStrategyImplementation::setDimension(const UnsignedInteger dimension)
{
  dimension_ = dimension;
}

UnsignedInteger SamplingStrategyImplementation::getDimension() const
{
  return dimension_;
}

/* String converter */
String SamplingStrategyImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << SamplingStrategyImplementation::GetClassName()
      << " dimension=" << dimension_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SamplingStrategyImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("dimension_", dimension_);
}

/* Method load() reloads the object from the StorageManager */
void SamplingStrategyImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("dimension_", dimension_);
}

END_NAMESPACE_OPENTURNS
