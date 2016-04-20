//                                               -*- C++ -*-
/**
 *  @brief The class TimeSeries implements values indexed by time
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
#include <limits>        // std::numeric_limits
#include <map>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include <cstdio>        // std::fopen, std::errno
#include <cstring>       // std::strerror

#include "openturns/OTconfig.hxx"
#include "openturns/TimeSeries.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/Exception.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Path.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Os.hxx"
#include "openturns/TBB.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TimeSeries);

/* Default constructor */
TimeSeries::TimeSeries()
  : FieldImplementation(RegularGrid(0.0, 1.0, 0), 1)
  , start_(0.0)
  , timeStep_(1.0)
  , n_(0)
{
  description_[0] = "t";
}

/* Standard constructor */
TimeSeries::TimeSeries(const UnsignedInteger n,
                       const UnsignedInteger dim)
  : FieldImplementation(RegularGrid(0.0, 1.0, n), dim)
  , start_(0.0)
  , timeStep_(1.0)
  , n_(n)
{
  description_[0] = "t";
}

/* Constructor from a TimeGrid and a dimension */
TimeSeries::TimeSeries(const RegularGrid & tg,
                       const UnsignedInteger dim)
  : FieldImplementation(tg, dim)
  , start_( tg.getStart() )
  , timeStep_( tg.getStep() )
  , n_( tg.getN() )
{
  description_[0] = "t";
}

/* Constructor from a TimeGrid and a sample */
TimeSeries::TimeSeries(const RegularGrid & tg,
                       const NumericalSample & sample)
  : FieldImplementation(tg, sample)
  , start_( tg.getStart() )
  , timeStep_( tg.getStep() )
  , n_( tg.getN() )
{
  if (n_ != sample.getSize()) throw InvalidArgumentException(HERE) << "Can't create a TimeSeries with a TimeGrid with " << tg.getN()
        << " steps and a sample of size " << sample.getSize();
  description_[0] = "t";
}

/* Constructor from a Field */
TimeSeries::TimeSeries(const Field & field)
  : FieldImplementation(*field.getImplementation())
  , start_( 0 )
  , timeStep_( 0 )
  , n_( 0 )
{
  const RegularGrid timeGrid(field.getMesh());
  start_ = timeGrid.getStart();
  timeStep_ = timeGrid.getStep();
  n_ = timeGrid.getN();
  description_[0] = "t";
}

/* Virtual constructor */
TimeSeries * TimeSeries::clone() const
{
  return new TimeSeries(*this);
}

/* Comparison function */
Bool TimeSeries::operator ==(const TimeSeries & other) const
{
  if (this == &other) return true;
  return (mesh_ == other.mesh_) && (values_ == other.values_);
}

/* String converter */
String TimeSeries::__repr__() const
{
  OSS oss(true);
  oss << "class=" << TimeSeries::GetClassName()
      << " name=" << getName()
      << " derived from=" << FieldImplementation::__repr__()
      << " start=" << start_
      << " timeStep=" << timeStep_
      << " n=" << n_;
  return oss;
}

String TimeSeries::__str__(const String & offset) const
{
  return FieldImplementation::__str__(offset);
}


/* Appends an element to the collection */
TimeSeries & TimeSeries::add(const NumericalPoint & point)
{
  return add(NumericalSample(1, point));
}


/* Appends a sample to the collection */
TimeSeries & TimeSeries::add(const NumericalSample & sample)
{
  if ((n_ > 0) && (sample.getDimension() != getDimension())) throw InvalidArgumentException(HERE) << "Error: expected a sample of dimension=" << getDimension() << ", got dimension=" << sample.getDimension();
  if (sample.getDimension() == 0) throw InvalidArgumentException(HERE) << "Error: expected a sample of dimension greater than 0";
  const UnsignedInteger size = sample.getSize();
  if (size == 0) return *this;
  // Update the vertices
  NumericalSample vertices(mesh_.getVertices());
  for (UnsignedInteger i = 0; i < size; ++i) vertices.add(NumericalPoint(1, start_ + timeStep_ * (n_ + i)));
  // Update the values
  values_.add(sample);
  // Update the simplices
  // If there is currently no point in the TimeSeries the new points create (size-1) elements
  UnsignedInteger iStart = 0;
  if (n_ == 0) iStart = 1;
  Collection<Indices> simplices(mesh_.getSimplices());
  for (UnsignedInteger i = iStart; i < size; ++i)
  {
    Indices element(2);
    element[1] = n_ + i;
    // Cannot underflow because either n_>0 or i>0
    element[0] = (n_ + i) - 1;
    simplices.add(element);
  }
  mesh_ = Mesh(vertices, simplices);
  n_ += size;
  return *this;
}

/* Append another time series to the collection. The time grids must match (one follows the other) */
TimeSeries & TimeSeries::add(const TimeSeries & continuer)
{
  NumericalSample vertices(mesh_.getVertices());
  if ((timeStep_ != continuer.timeStep_) || (start_ + n_ * timeStep_ != continuer.start_)) LOGWARN(OSS() << "The continuer does not have a compatible time grid. Using the values only.");
  return add(continuer.getSample());
}

/* Method save() stores the object through the StorageManager */
void TimeSeries::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "start_", start_);
  adv.saveAttribute( "timeStep_", timeStep_);
  adv.saveAttribute( "n_", n_);
}


/* Method load() reloads the object from the StorageManager */
void TimeSeries::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "start_", start_);
  adv.loadAttribute( "timeStep_", timeStep_);
  adv.loadAttribute( "n_", n_);
}

END_NAMESPACE_OPENTURNS
