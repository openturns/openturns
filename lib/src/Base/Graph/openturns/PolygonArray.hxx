//                                               -*- C++ -*-
/**
 *  @brief PolygonArray class for array of polygons plots
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
#ifndef OPENTURNS_POLYGONARRAY_HXX
#define OPENTURNS_POLYGONARRAY_HXX

#include "openturns/DrawableImplementation.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Polygon.hxx"
#include "openturns/Collection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PolygonArray
 *
 * The class describing a polygon array plot
 * Instance of Drawable
 */


class OT_API PolygonArray
  : public DrawableImplementation
{

  CLASSNAME

public:

  typedef Collection< Polygon > PolygonCollection;

  /** Default constructor */
  explicit PolygonArray(const String & legend = "");

  /** Constructor based on a collection of polygons */
  explicit PolygonArray(const PolygonCollection & polygons,
                        const String & legend = "");

  /** Construction based on a set of vertices */
  PolygonArray(const Sample & coordinates,
               const UnsignedInteger verticesNumber,
               const Description & palette,
               const String & legend = "");

  /** String converter */
  String __repr__() const override;

  /** Draw method */
  String draw() const override;

  /** Clone method */
  PolygonArray * clone() const override;


  /** Palette of the edge/face of the polygons */
  void setPalette(const Description & palette) override;
  Description getPalette() const override;

  /** Coordinates and vertices number of the vertices of the polygons */
  void setCoordinatesAndVerticesNumber(const Sample & coordinates,
                                       const UnsignedInteger verticesNumber);

  /** Coordinates accessor */
  Sample getCoordinates() const;

  /** Vertices number accessor */
  UnsignedInteger getVerticesNumber() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() stores the object through the StorageManager */
  void load(Advocate & adv) override;

  /** Remove all the temporary files */
  void clean() const override;

protected:
  /** Check fo data validity */
  void checkData(const Sample & data) const override;

private:

  /** Number of vertices per polygon */
  UnsignedInteger verticesNumber_;

  /** List of palette for strip of polygons */
  Description palette_;

  /** Name of the file to store the palette of the polygons */
  mutable String paletteFileName_;

  /** Name of the file to store the coordinates of the polygon */
  mutable String coordinatesFileName_;

}; /* class Polygon */



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_POLYGON_HXX */
