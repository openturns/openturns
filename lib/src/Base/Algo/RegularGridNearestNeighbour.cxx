//                                               -*- C++ -*-
/**
 *  @brief Nearest neighbour index search on a RegularGrid
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/RegularGridNearestNeighbour.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Indices.hxx"
#include "openturns/SobolSequence.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(RegularGridNearestNeighbour)

static const Factory<RegularGridNearestNeighbour> Factory_RegularGridNearestNeighbour;

/**
 * @class RegularGridNearestNeighbour
 *
 * Find nearest neighbour on a RegularGrid.
 */

/* Default constructor */
RegularGridNearestNeighbour::RegularGridNearestNeighbour()
  : NearestNeighbourImplementation()
  , grid_()
{
  // Nothing to do
}

/* Parameters constructor */
RegularGridNearestNeighbour::RegularGridNearestNeighbour(const RegularGrid & grid)
  : NearestNeighbourImplementation()
  , grid_(grid)
  , start_(grid.getStart())
  , N_(grid.getN())
  , step_(grid.getStep())
  , values_(grid.getValues())
{
  // Nothing to do
}

/* Virtual constructor */
RegularGridNearestNeighbour * RegularGridNearestNeighbour::clone() const
{
  return new RegularGridNearestNeighbour( *this );
}

/* String converter */
String RegularGridNearestNeighbour::__repr__() const
{
  return OSS() << "class=" << GetClassName()
         << " grid=" << grid_.__repr__();
}

/* Get the index of the nearest neighbour of the given point */
UnsignedInteger RegularGridNearestNeighbour::getNearestNeighbourIndex(const Point & x) const
{
  if (x.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension 1, got a point of dimension " << x.getDimension();
  return getNearestScalarNeighbourIndex(x[0]);
}

/* Get the index of the nearest neighbour of the given scalar */
UnsignedInteger RegularGridNearestNeighbour::getNearestScalarNeighbourIndex(const Scalar & x) const
{
  if (x <= start_) return 0;
  if (x >= start_ + (N_ - 1) * step_) return N_ - 1;
  return static_cast<UnsignedInteger>(round((x - start_) / step_));
}

/* Get the index of the nearest neighbour of the given scalars */
Indices RegularGridNearestNeighbour::getNearestScalarNeighbourIndex(const Point & x) const
{
  const UnsignedInteger size = x.getSize();
  Indices result(size);
  for(UnsignedInteger i = 0; i < size; ++i)
    result[i] = getNearestScalarNeighbourIndex(x[i]);
  return result;
}

/** Get the nearest neighbour of the given point */
Point RegularGridNearestNeighbour::getNearestNeighbour(const Point & x) const
{
  if (x.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension 1, got a point of dimension " << x.getDimension();
  return Point(1, getNearestScalarNeighbour(x[0]));
}

/** Get the nearest neighbour of the given scalar */
Scalar RegularGridNearestNeighbour::getNearestScalarNeighbour(const Scalar & x) const
{
  return values_[getNearestScalarNeighbourIndex(x)];
}

/* Get the nearest neighbour of the given scalars */
Point RegularGridNearestNeighbour::getNearestScalarNeighbour(const Point & x) const
{
  const UnsignedInteger size = x.getSize();
  Point result(size);
  const Indices indices(getNearestScalarNeighbourIndex(x));
  for(UnsignedInteger i = 0; i < size; ++i)
    result[i] = values_[indices[i]];
  return result;
}

/* Get the indices of the k nearest neighbours of the given point */
Indices RegularGridNearestNeighbour::getNearestNeighboursIndices(const Point & x,
                                                                 const UnsignedInteger k,
                                                                 const Bool sorted) const
{
  if (x.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension 1, got a point of dimension " << x.getDimension();
  return getNearestScalarNeighboursIndices(x[0], k, sorted);
}

/* Get the indices of the k nearest neighbours of the given scalar */
Indices RegularGridNearestNeighbour::getNearestScalarNeighboursIndices(const Scalar & x,
                                                                       const UnsignedInteger k,
                                                                       const Bool sorted) const
{
  if (k > N_) throw InvalidArgumentException(HERE) << "Error: cannot return more neighbours than points in the grid!";
  Indices result(k);
  // If we need as many neighbours as points in the sample, just return all the possible indices
  if (k == N_ && !sorted)
  {
    result.fill();
  }
  else
  { // Always sort neighbours, this is fast enough anyway
    UnsignedInteger iMin = getNearestScalarNeighbourIndex(x);
    UnsignedInteger iMax = iMin;
    result[0] = iMin;
    for(UnsignedInteger i = 1; i < k; ++i)
    {
      if (iMin == 0)
      {
        ++iMax;
        result[i] = iMax;
      }
      else if (iMax == N_ - 1)
      {
        --iMin;
        result[i] = iMin;
      }
      else
      {
        Scalar left = values_[iMin - 1];
        Scalar right = values_[iMax + 1];
        if (std::abs(left - x) < std::abs(right - x))
        {
          --iMin;
          result[i] = iMin;
        }
        else
        {
          ++iMax;
          result[i] = iMax;
        }
      }
    }
  }
  return result;
}

/* Get the k nearest neighbours of the given point */
Sample RegularGridNearestNeighbour::getNearestNeighbours(const Point & x,
                                                         const UnsignedInteger k,
                                                         const Bool sorted) const
{
  if (x.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension 1, got a point of dimension " << x.getDimension();
  const Indices indices(getNearestScalarNeighboursIndices(x[0], k, sorted));
  Sample result(indices.getSize(), 1);
  for(UnsignedInteger i = 0; i < indices.getSize(); ++i)
    result(i, 0) = values_[indices[i]];
  return result;
}

/* Get the k nearest neighbours of the given scalar */
Point RegularGridNearestNeighbour::getNearestScalarNeighbours(const Scalar & x,
                                                              const UnsignedInteger k,
                                                              const Bool sorted) const
{
  const Indices indices(getNearestScalarNeighboursIndices(x, k, sorted));
  Point result(indices.getSize());
  for(UnsignedInteger i = 0; i < indices.getSize(); ++i)
    result[i] = values_[indices[i]];
  return result;
}

/* Method save() stores the object through the StorageManager */
void RegularGridNearestNeighbour::save(Advocate & adv) const
{
  NearestNeighbourImplementation::save(adv);
  adv.saveAttribute("grid_", grid_);
}


/* Method load() reloads the object from the StorageManager */
void RegularGridNearestNeighbour::load(Advocate & adv)
{
  NearestNeighbourImplementation::load(adv);
  adv.loadAttribute("grid_", grid_);
  start_ = grid_.getStart();
  N_ = grid_.getN();
  step_ = grid_.getStep();
  values_ = grid_.getValues();
}

END_NAMESPACE_OPENTURNS
