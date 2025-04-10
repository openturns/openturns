//                                               -*- C++ -*-
/**
 *  @brief Polygon class for polygon plots
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_POLYGON_HXX
#define OPENTURNS_POLYGON_HXX

#include "openturns/DrawableImplementation.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Polygon
 *
 * The class describing a polygon plot
 * Instance of Drawable
 */


class OT_API Polygon
  : public DrawableImplementation
{

  CLASSNAME

public:


  /** Default constructor */
  explicit Polygon(const String & legend = "");

  /** Default constructor */
  explicit Polygon(const Sample & data,
                   const String & legend = "");

  /** Constructor from 2 data sets */
  Polygon(const Sample & dataX,
          const Sample & dataY,
          const String & legend = "");

  /** Constructor from 2 data sets */
  Polygon(const Point & dataX,
          const Point & dataY,
          const String & legend = "");

  /** Constructor with parameters */
  Polygon(const Sample & data,
          const String & color,
          const String & edgeColor,
          const String & legend = "");

  /** String converter */
  String __repr__() const override;

  /** Clone method */
  Polygon * clone() const override;


  /** Color of the edge
   */
  void setEdgeColor(const String & edgeColor);
  String getEdgeColor() const override;


  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() stores the object through the StorageManager */
  void load(Advocate & adv) override;

  /** Builds a polygon which fills the area between two curves */
  static Polygon FillBetween(Sample const& dataX, Sample const& dataY1, Sample const& dataY2);

  /** Builds a polygon which fills the area between two curves */
  static Polygon FillBetween(Point const& dataX, Point const& dataY1, Point const& dataY2);

protected:
  /** Check fo data validity */
  void checkData(const Sample & data) const override;

private:

  /** Color of the edge of the polygon */
  String edgeColor_;

}; /* class Polygon */



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_POLYGON_HXX */
