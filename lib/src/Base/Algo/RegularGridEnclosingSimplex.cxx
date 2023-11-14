//                                               -*- C++ -*-
/**
 *  @brief  This class provides a point location algorithm on RegularGrid
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/RegularGridEnclosingSimplex.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/KDTree.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class RegularGridEnclosingSimplex
 */

CLASSNAMEINIT(RegularGridEnclosingSimplex)

static const Factory<RegularGridEnclosingSimplex> Factory_RegularGridEnclosingSimplex;

/* Constructor without parameters */
RegularGridEnclosingSimplex::RegularGridEnclosingSimplex()
  : EnclosingSimplexAlgorithmImplementation()
  , start_(0)
  , N_(0)
  , step_(0)
{
  // Nothing to do
}

/* Parameter constructor */
RegularGridEnclosingSimplex::RegularGridEnclosingSimplex(const RegularGrid & grid)
  : EnclosingSimplexAlgorithmImplementation()
  , start_(grid.getStart())
  , N_(grid.getN())
  , step_(grid.getStep())
{
  // Nothing to do
}

/* Virtual constructor */
RegularGridEnclosingSimplex * RegularGridEnclosingSimplex::clone() const
{
  return new RegularGridEnclosingSimplex(*this);
}

RegularGridEnclosingSimplex * RegularGridEnclosingSimplex::emptyClone() const
{
  return new RegularGridEnclosingSimplex();
}

void RegularGridEnclosingSimplex::setVerticesAndSimplices(const Sample & vertices, const IndicesCollection & simplices)
{
  EnclosingSimplexAlgorithmImplementation::setVerticesAndSimplices(vertices, simplices);
  // Check that sample can be converted to a RegularGrid, and get N, start, step
  RegularGrid newGrid = Mesh(vertices);
  start_ = newGrid.getStart();
  N_ = newGrid.getN();
  step_ = newGrid.getStep();
}

/* Get the index of the enclosing simplex of the given point */
UnsignedInteger RegularGridEnclosingSimplex::query(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=1, got dimension=" << point.getDimension();
  return queryScalar(point[0]);
}

UnsignedInteger RegularGridEnclosingSimplex::queryScalar(const Scalar x) const
{
  const Scalar index = std::floor((x - start_) / step_);
  if (index < 0.0 || index >= Scalar(N_)) return  N_ - 1;
  return static_cast<UnsignedInteger>(index);
}

Indices RegularGridEnclosingSimplex::queryScalar(const Point & values) const
{
  Indices result(values.getSize());
  for(UnsignedInteger i = 0; i < values.getSize(); ++i)
    result[i] = queryScalar(values[i]);
  return result;
}

/* String converter */
String RegularGridEnclosingSimplex::__repr__() const
{
  return OSS(true) << "class=" << RegularGridEnclosingSimplex::GetClassName();
}

/* String converter */
String RegularGridEnclosingSimplex::__str__(const String & ) const
{
  return OSS(false) << "class=" << RegularGridEnclosingSimplex::GetClassName();
}

END_NAMESPACE_OPENTURNS
