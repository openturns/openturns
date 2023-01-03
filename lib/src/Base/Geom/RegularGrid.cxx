//                                               -*- C++ -*-
/**
 *  @brief The class RegularGrid implements an equaly spaced set of real values
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

#include "openturns/RegularGrid.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

static const Factory<RegularGrid> Factory_RegularGrid;

CLASSNAMEINIT(RegularGrid)

/* Default constructor */
RegularGrid::RegularGrid()
  : Mesh()
  , start_(0.0)
  , step_(1.0)
  , n_(0)
{
  // Nothing to do
  vertices_.setDescription(Description(1, "t"));
}

/* Parameters constructor */
RegularGrid::RegularGrid(const Scalar start,
                         const Scalar step,
                         const UnsignedInteger n)
  : Mesh()
  , start_(start)
  , step_(step)
  , n_(n)
{
  // Check if there is at least one point
  if (n >= 1)
  {
    // The mesh is the description by extension of the grid
    vertices_ = Sample(n, 1);
    for (UnsignedInteger i = 0; i < n; ++i) vertices_(i, 0) = start_ + i * step_;
    // Here we know that n > 0
    simplices_ = IndicesCollection(n - 1, 2);
    for (UnsignedInteger i = 0; i < n - 1; ++i)
    {
      simplices_(i, 0) = i;
      simplices_(i, 1) = i + 1;
    }
  }
  vertices_.setDescription(Description(1, "t"));
}

RegularGrid::RegularGrid(const Mesh & mesh)
  : Mesh(mesh)
  , start_(0.0)
  , step_(1.0)
  , n_(0)
{
  // Check if the given mesh can be upgraded to a RegularGrid
  if (mesh.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the mesh must be of dimension 1 to be converted into a RegularGrid, here dimension=" << mesh.getDimension();
  if (!mesh.isRegular()) throw InvalidArgumentException(HERE) << "Error: the mesh must be regular to be converted into a RegularGrid.";
  n_ = mesh.getVerticesNumber();
  if (!(n_ > 0)) throw InvalidArgumentException(HERE) << "Error: the mesh must contains at least one vertex to be converted into a RegularGrid.";
  start_ = mesh.getVertices()(0, 0);
  if (n_ > 1) step_ = (mesh.getVertices()(n_ - 1, 0) - start_) / (n_ - 1);
  vertices_.setDescription(Description(1, "t"));
  simplices_ = IndicesCollection(n_ - 1, 2);
  for (UnsignedInteger i = 0; i < n_ - 1; ++i)
  {
    simplices_(i, 0) = i;
    simplices_(i, 1) = i + 1;
  }
}

/* Virtual constructor method */
RegularGrid * RegularGrid::clone() const
{
  return new RegularGrid( *this );
}

Bool RegularGrid::operator == (const RegularGrid & rhs) const
{
  const RegularGrid & lhs = *this;
  return (lhs.start_ == rhs.start_) && (lhs.step_ == rhs.step_) && (lhs.n_ == rhs.n_);
}

Bool RegularGrid::operator != (const RegularGrid & rhs) const
{
  return !operator==(rhs);
}

Scalar RegularGrid::getStart() const
{
  return start_;
}

/* This method computes the timestamp of the very next step past the time series (STL convention) */
Scalar RegularGrid::getEnd() const
{
  return start_ + step_ * n_;
}

Scalar RegularGrid::getStep() const
{
  return step_;
}

UnsignedInteger RegularGrid::getN() const
{
  return n_;
}

Scalar RegularGrid::getValue(const UnsignedInteger i) const
{
  if (!(i < n_)) throw OutOfBoundException(HERE) << "Error: the given index i=" << i << " must be less than the number of ticks n=" << n_;
  return vertices_(i, 0);
}

Point RegularGrid::getValues() const
{
  return vertices_.getImplementation()->getData();
}

/* Tells if it is regular */
Bool RegularGrid::isRegular() const
{
  return true;
}

/* Tells if the given grid follows the current one */
Bool RegularGrid::follows(const RegularGrid & starter) const
{
  const RegularGrid & continuer = *this;
  return (starter.getEnd() == continuer.getStart()) && (starter.getStep()) == continuer.getStep();
}

/*
 * String converter
 */
String RegularGrid::__repr__() const
{
  return OSS(true) << "class=RegularGrid name=" << getName()
         << " start=" << start_
         << " step=" << step_
         << " n=" << n_;
}

String RegularGrid::__str__(const String & ) const
{
  return OSS(false)
         << "RegularGrid(start=" << start_
         << ", step=" << step_
         << ", n=" << n_
         << ")";
}

/* Method save() stores the object through the StorageManager */
void RegularGrid::save(Advocate & adv) const
{
  Mesh::save(adv);
  adv.saveAttribute( "start_", start_);
  adv.saveAttribute( "step_", step_);
  adv.saveAttribute( "n_", n_);
}


/* Method load() reloads the object from the StorageManager */
void RegularGrid::load(Advocate & adv)
{
  Mesh::load(adv);
  adv.loadAttribute( "start_", start_);
  adv.loadAttribute( "step_", step_);
  adv.loadAttribute( "n_", n_);
}

END_NAMESPACE_OPENTURNS
