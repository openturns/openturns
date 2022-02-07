//                                               -*- C++ -*-
/**
 *  @brief Nearest neighbour index search on a 1D Sample
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

#include "openturns/NearestNeighbour1D.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Indices.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Mesh.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NearestNeighbour1D)

static const Factory<NearestNeighbour1D> Factory_NearestNeighbour1D;

/**
 * @class NearestNeighbour1D
 *
 * Find nearest neighbour on a 1D Sample
 */

/* Default constructor */
NearestNeighbour1D::NearestNeighbour1D()
  : NearestNeighbourAlgorithmImplementation()
  , points_()
  , sorted_()
  , originalIndex_()
{
  // Nothing to do
}

/* Parameters constructor */
NearestNeighbour1D::NearestNeighbour1D(const Sample & vertices)
  : NearestNeighbourAlgorithmImplementation()
  , points_()
  , sorted_()
  , originalIndex_()
{
  // Sort points
  setSample(vertices);
}

/* Virtual constructor */
NearestNeighbour1D * NearestNeighbour1D::clone() const
{
  return new NearestNeighbour1D( *this );
}

/* Virtual constructor */
NearestNeighbour1D * NearestNeighbour1D::emptyClone() const
{
  return new NearestNeighbour1D();
}

void NearestNeighbour1D::setSample(const Sample & points)
{
  if (!(points.getSize() == 0 || points.getDimension() == 1)) throw InvalidArgumentException(HERE) << "Error: Sample must be either empty or of dimension 1 but has size " << points.getSize() << " and dimension " << points.getDimension();
  if (points == points_) return;

  points_ = points;
  const UnsignedInteger size(points_.getSize());
  Collection< std::pair<Scalar, UnsignedInteger> > valueAndIndex(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    valueAndIndex[i] = std::pair<Scalar, UnsignedInteger>(points_(i, 0), i);

  // Sort according to values in increasing order
  std::sort(valueAndIndex.begin(), valueAndIndex.end());

  sorted_ = Point(size);
  originalIndex_ = Indices(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    sorted_[i] = valueAndIndex[i].first;
    originalIndex_[i] = valueAndIndex[i].second;
  }
}

/* Sample accessor */
Sample NearestNeighbour1D::getSample() const
{
  return points_;
}

/* String converter */
String NearestNeighbour1D::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " points=" << points_
         << " sorted=" << sorted_;
}

/* String converter */
String NearestNeighbour1D::__str__(const String & ) const
{
  return OSS(false) << "class=" << GetClassName()
         << " points=" << points_
         << " sorted=" << sorted_;
}

/* Get the index of the nearest neighbour of the given point */
UnsignedInteger NearestNeighbour1D::query(const Point & x) const
{
  if (x.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension 1, got a point of dimension " << x.getDimension();
  return queryScalar(x[0]);
}

/* Get the index of the nearest neighbour of the given scalar */
UnsignedInteger NearestNeighbour1D::queryScalar(const Scalar x) const
{
  const UnsignedInteger size = points_.getSize();
  if (x <= sorted_[0])
    return originalIndex_[0];
  else if (x >= sorted_[size - 1])
    return originalIndex_[size - 1];
  // Find iMin such that sorted_[iMin] < x < sorted_[iMin + 1]
  UnsignedInteger iMin = 0;
  UnsignedInteger iMax = size - 1;
  while (iMax > iMin + 1)
  {
    const UnsignedInteger iMiddle = (iMax + iMin) / 2;
    const Scalar value = sorted_[iMiddle];
    if (value < x)
    {
      iMin = iMiddle;
    }
    else if (value > x)
    {
      iMax = iMiddle;
    }
    else
    {
      // Value found
      return originalIndex_[iMiddle];
    }
  }
  if (x - sorted_[iMin] < sorted_[iMin + 1] - x)
  {
    return originalIndex_[iMin];
  }
  else
  {
    return originalIndex_[iMin + 1];
  }
}

/* Get the index of the nearest neighbour of the given scalars */
Indices NearestNeighbour1D::queryScalar(const Point & x) const
{
  const UnsignedInteger size = x.getSize();
  Indices result(size);
  for(UnsignedInteger i = 0; i < size; ++i)
    result[i] = queryScalar(x[i]);
  return result;
}

/* Get the indices of the k nearest neighbours of the given point */
Indices NearestNeighbour1D::queryK(const Point & x, const UnsignedInteger k, const Bool sorted) const
{
  if (x.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension 1, got a point of dimension " << x.getDimension();
  return queryScalarK(x[0], k, sorted);
}

/* Get the indices of the k nearest neighbours of the given scalar */
Indices NearestNeighbour1D::queryScalarK(const Scalar x, const UnsignedInteger k, const Bool sorted) const
{
  const UnsignedInteger size = points_.getSize();
  if (!(k <= size)) throw InvalidArgumentException(HERE) << "Error: cannot return more neighbours (" << k << ") than points (" << size << ") in the sample!";
  Indices result(k);
  // If we need as many neighbours as points in the sample, just return all the possible indices
  if (k == size && !sorted)
  {
    result.fill();
  }
  else
  {
    // Always sort neighbours, this is fast enough anyway
    UnsignedInteger iMin = queryScalar(x);
    // We want index in sorted_, not the original one
    for(UnsignedInteger i = 0; i < size; ++i)
    {
      if (originalIndex_[i] == iMin)
      {
        iMin = i;
        break;
      }
    }
    UnsignedInteger iMax = iMin;
    result[0] = iMin;
    for(UnsignedInteger i = 1; i < k; ++i)
    {
      if (iMin == 0)
      {
        ++iMax;
        result[i] = iMax;
      }
      else if (iMax == size - 1)
      {
        --iMin;
        result[i] = iMin;
      }
      else
      {
        Scalar left = sorted_[iMin - 1];
        Scalar right = sorted_[iMax + 1];
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
    for(UnsignedInteger i = 0; i < k; ++i)
      result[i] = originalIndex_[result[i]];
  }
  return result;
}

/* Method save() stores the object through the StorageManager */
void NearestNeighbour1D::save(Advocate & adv) const
{
  NearestNeighbourAlgorithmImplementation::save(adv);
  adv.saveAttribute("points_", points_);
}


/* Method load() reloads the object from the StorageManager */
void NearestNeighbour1D::load(Advocate & adv)
{
  NearestNeighbourAlgorithmImplementation::load(adv);
  Sample points;
  adv.loadAttribute("points_", points);
  if (points.getSize() > 0) setSample(points);
}

END_NAMESPACE_OPENTURNS
