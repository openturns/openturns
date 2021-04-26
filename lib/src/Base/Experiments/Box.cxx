//                                               -*- C++ -*-
/**
 *  @brief Implementation of the box experiment plane
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OTprivate.hxx"
#include "openturns/Box.hxx"
#include "openturns/Tuples.hxx"
#include "openturns/SpecFunc.hxx" // for boost.math.round
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Box)

static const Factory<Box> Factory_Box;

/* Default constructor */
Box::Box()
  : StratifiedExperiment()
{
  // Nothing to do
}

/* Constructor with parameters */
Box::Box(const Point & levels)
  : StratifiedExperiment(Point(levels.getDimension(), 0.0), levels)
  , bounds_(levels_.getDimension())
{
  // Check if there is the same number of levels than the dimension of the experiment plane
  if (!(levels.getDimension() > 0)) throw InvalidArgumentException(HERE) << "Error: the levels dimension is " << levels.getDimension() << ", it should be > 0.";
  setLevels(levels);
}

/* Constructor with parameters */
Box::Box(const Indices & levels)
  : StratifiedExperiment(Point(levels.getSize(), 0.0), Point(levels.getSize(), 0.0))
  , bounds_(levels_.getDimension())
{
  // Check if there is the same number of levels than the dimension of the experiment plane
  const UnsignedInteger size = levels.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: the levels dimension is " << size << ", it should be > 0.";
  setLevels(Collection<Scalar>(levels.begin(), levels.end()));
}

Box::Box(const Indices & levels,
         const Interval & bounds)
  : StratifiedExperiment(Point(levels.getSize(), 0.0), Point(levels.getSize(), 0.0))
  , bounds_(bounds)
{
  // Check if there is the same number of levels than the dimension of the experiment plane
  const UnsignedInteger size = levels.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: the levels dimension is " << size << ", it should be > 0.";
  setLevels(Collection<Scalar>(levels.begin(), levels.end()));
  if (bounds.getDimension() != size) throw InvalidArgumentException(HERE) << "Error: the bounds dimension (" << bounds.getDimension() << ") should be equal to the levels dimension (" << size << ").";
}


/* Virtual constructor */
Box * Box::clone() const
{
  return new Box(*this);
}

/* Experiment plane generation
   The box [0, 1]^n is uniformly sampled in each dimension
   levels counts the number of interior points in each dimension */
Sample Box::generate() const
{
  const UnsignedInteger dimension = levels_.getDimension();
  Indices bounds(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) bounds[i] = static_cast< UnsignedInteger > (round(levels_[i] + 2.0));
  IndicesCollection tuples(Tuples(bounds).generate());
  const UnsignedInteger size = tuples.getSize();
  Sample boxPlane(size, dimension);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < dimension; ++j)
      boxPlane(i, j) = tuples(i, j) / (levels_[j] + 1.0);

  // scale sample
  if (bounds_ != Interval(dimension))
  {
    const Point lowerBound(bounds_.getLowerBound());
    const Point upperBound(bounds_.getUpperBound());
    const Point delta(upperBound - lowerBound);
    boxPlane *= delta;
    boxPlane += lowerBound;
  }

  return boxPlane;
} // generate()

/* String converter */
String Box::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " levels=" << levels_
      << " bounds=" << bounds_;
  return oss;
}

/** Specific levels accessor */
void Box::setLevels(const Point & levels)
{
  UnsignedInteger dimension = center_.getDimension();
  UnsignedInteger size = levels.getDimension();
  if (size != dimension) throw InvalidArgumentException(HERE) << "Error: levels dimension must equal center dimension for the Box design of experiment, here levels dimension=" << size << " and center dimension=" << dimension;
  for (UnsignedInteger i = 0; i < dimension; ++i) if (!(levels[i] >= 0.0)) throw InvalidArgumentException(HERE) << "Error: levels values must be greater or equal to 0 for the Box design of experiment, but levels[" << i << "]=" << levels[i];
  StratifiedExperiment::setLevels(levels);
}

/* Method save() stores the object through the StorageManager */
void Box::save(Advocate & adv) const
{
  StratifiedExperiment::save(adv);
  adv.saveAttribute("bounds_", bounds_);
}

/* Method load() reloads the object from the StorageManager */
void Box::load(Advocate & adv)
{
  StratifiedExperiment::load(adv);
  adv.loadAttribute("bounds_", bounds_);
}

END_NAMESPACE_OPENTURNS
