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
#ifndef OPENTURNS_GRID_HXX
#define OPENTURNS_GRID_HXX

#include "openturns/DrawableImplementation.hxx"
#include "openturns/Interval.hxx"
#include "openturns/Indices.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Grid
 *
 * The class describing a grid plot
 * Instance of Drawable
 */

class OT_API Grid : public DrawableImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  Grid();

  /** Constructor with parameters */
  Grid(const Interval & bounds,
       const Indices & subdivisions,
       const String & legend = "");

  /** String converter */
  String __repr__() const override;

  /** Clone method */
  Grid * clone() const override;

  /** Accessor for bounds */
  Interval getBounds() const override;
  void setBounds(const Interval & bounds) override;

  /** Accessor for subdivisions */
  Indices getSubdivisions() const override;
  void setSubdivisions(const Indices & subdivisions) override;

  /** Accessor for bounding box */
  Interval getBoundingBox() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Check for data validity */
  void checkData(const Sample & data) const override;

private:
  /** Update data (corners) from bounds */
  void updateDataFromBounds();

  /** Bounds of the grid, dimension 2 */
  Interval bounds_;

  /** Number of subdivisions in each direction, size 2 */
  Indices subdivisions_;

}; /* class Grid */

END_NAMESPACE_OPENTURNS
#endif /* OPENTURNS_GRID_HXX */
