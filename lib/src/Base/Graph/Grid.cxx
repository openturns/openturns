//                                               -*- C++ -*-
/**
 *  @brief Grid class for grid plots
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
#include "openturns/Grid.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Grid)

static const Factory<Grid> Factory_Grid;

/* Default constructor */
Grid::Grid()
  : DrawableImplementation(Sample(2, 2), "")
  , bounds_(2)
  , subdivisions_(2, 5)
{
  updateDataFromBounds();
  setColor("black");
  setPointStyle("none");
}

/* Constructor with parameters */
Grid::Grid(const Interval & bounds,
           const Indices & subdivisions,
           const String & legend)
  : DrawableImplementation(Sample(2, 2), legend)
  , bounds_(2)
  , subdivisions_(2, 5)
{
  setBounds(bounds);
  setSubdivisions(subdivisions);
  setColor("black");
  setPointStyle("none");
}

/* String converter */
String Grid::__repr__() const
{
  OSS oss;
  oss << "class=" << Grid::GetClassName()
      << " name=" << getName()
      << " bounds=" << bounds_
      << " subdivisions=" << subdivisions_
      << " derived from " << DrawableImplementation::__repr__();
  return oss;
}

/* Clone method */
Grid * Grid::clone() const
{
  return new Grid(*this);
}

/* Accessor for bounds */
Interval Grid::getBounds() const
{
  return bounds_;
}

void Grid::setBounds(const Interval & bounds)
{
  if (bounds.getDimension() != 2)
    throw InvalidDimensionException(HERE) << "Grid bounds dimension must be 2, got " << bounds.getDimension();
  bounds_ = bounds;
  updateDataFromBounds();
}

/* Accessor for subdivisions */
Indices Grid::getSubdivisions() const
{
  return subdivisions_;
}

void Grid::setSubdivisions(const Indices & subdivisions)
{
  if (subdivisions.getSize() != 2)
    throw InvalidDimensionException(HERE) << "Grid subdivisions size must be 2, got " << subdivisions.getSize();
  for (UnsignedInteger i = 0; i < 2; ++ i)
    if (subdivisions[i] == 0)
      throw InvalidArgumentException(HERE) << "Grid subdivisions must be >= 1 in each direction, got 0 in direction " << i;
  subdivisions_ = subdivisions;
}

/* Accessor for bounding box */
Interval Grid::getBoundingBox() const
{
  return bounds_;
}

/* Check validity of data */
void Grid::checkData(const Sample & data) const
{
  if (data.getDimension() != 2)
  {
    throw InvalidDimensionException(HERE) << "Expected sample of dimension 2: got " << data.getDimension();
  }
}

/* Update data (corners) from bounds, so Graph bbox computation includes Grid */
void Grid::updateDataFromBounds()
{
  Sample data(2, 2);
  data(0, 0) = bounds_.getLowerBound()[0];
  data(0, 1) = bounds_.getLowerBound()[1];
  data(1, 0) = bounds_.getUpperBound()[0];
  data(1, 1) = bounds_.getUpperBound()[1];
  setData(data);
}

/* Method save() stores the object through the StorageManager */
void Grid::save(Advocate & adv) const
{
  DrawableImplementation::save(adv);
  adv.saveAttribute("bounds_", bounds_);
  adv.saveAttribute("subdivisions_", subdivisions_);
}

/* Method load() reloads the object from the StorageManager */
void Grid::load(Advocate & adv)
{
  DrawableImplementation::load(adv);
  adv.loadAttribute("bounds_", bounds_);
  adv.loadAttribute("subdivisions_", subdivisions_);
  // keep data consistent with bounds for old archives
  try
  {
    updateDataFromBounds();
  }
  catch (...)
  {
  }
}

END_NAMESPACE_OPENTURNS
