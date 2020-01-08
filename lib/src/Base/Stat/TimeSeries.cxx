//                                               -*- C++ -*-
/**
 *  @brief The class TimeSeries implements values indexed by time
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/TimeSeries.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TimeSeries)

static const Factory<TimeSeries> Factory_TimeSeries;

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
                       const Sample & sample)
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
TimeSeries & TimeSeries::add(const Point & point)
{
  return add(Sample(1, point));
}


/* Appends a sample to the collection */
TimeSeries & TimeSeries::add(const Sample & sample)
{
  if ((n_ > 0) && (sample.getDimension() != getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: expected a sample of dimension=" << getOutputDimension() << ", got dimension=" << sample.getDimension();
  if (sample.getDimension() == 0) throw InvalidArgumentException(HERE) << "Error: expected a sample of dimension greater than 0";
  const UnsignedInteger size = sample.getSize();
  if (size == 0) return *this;
  // Update the vertices
  Sample vertices(mesh_.getVertices());
  for (UnsignedInteger i = 0; i < size; ++i) vertices.add(Point(1, start_ + timeStep_ * (n_ + i)));
  // Update the values
  values_.add(sample);
  // Update the simplices
  // If there is currently no point in the TimeSeries the new points create (size-1) elements
  UnsignedInteger iStart = 0;
  if (n_ == 0) iStart = 1;
  const UnsignedInteger nrSimplices = mesh_.getSimplicesNumber();
  IndicesCollection simplices(mesh_.getSimplices());
  Indices values;
  if (nrSimplices > 0) values.add(Collection<UnsignedInteger>(simplices.cbegin_at(0), simplices.cend_at(nrSimplices - 1)));
  for (UnsignedInteger i = iStart; i < size; ++i)
  {
    // Cannot underflow because either n_>0 or i>0
    values.add((n_ + i) - 1);
    values.add(n_ + i);
  }
  mesh_ = Mesh(vertices, IndicesCollection(nrSimplices + size - iStart, 2, values));
  n_ += size;
  return *this;
}

/* Append another time series to the collection. The time grids must match (one follows the other) */
TimeSeries & TimeSeries::add(const TimeSeries & continuer)
{
  Sample vertices(mesh_.getVertices());
  if ((timeStep_ != continuer.timeStep_) || (start_ + n_ * timeStep_ != continuer.start_)) LOGWARN(OSS() << "The continuer does not have a compatible time grid. Using the values only.");
  return add(continuer.getValues());
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
