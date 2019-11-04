//                                               -*- C++ -*-
/**
 *  @brief Pie class for piechart plots
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_PIE_HXX
#define OPENTURNS_PIE_HXX

#include "openturns/DrawableImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Pie
 *
 * The class describing a pie chart
 * Instance of Drawable
 */


class OT_API Pie : public DrawableImplementation
{

  CLASSNAME

public:
  /** Default constructor */
  Pie();

  /** Default constructor */
  explicit Pie(const Point & data);

  /** Constructor with parameters */
  Pie(const Point & data,
      const Description & labels);

  /** Constructor with parameters */
  Pie(const Point & data,
      const Description & labels,
      const Point & center,
      const Scalar & radius,
      const Description & palette);

  /** String converter */
  String __repr__() const;

  /** Accessor for center */
  Point getCenter() const;
  void setCenter(const Point & center);

  /** Accessor for radius */
  Scalar getRadius() const;
  void setRadius(const Scalar radius);

  /** Accessor for labels */
  Description getLabels() const;
  void setLabels(const Description & labels);

  /** Accessor for color palette */
  Description getPalette() const;
  void setPalette(const Description & palette);

  /** Accessor for boundingbox */
  Interval getBoundingBox() const;

  /** Draw method */
  String draw() const;

  /** Clone method */
  virtual Pie * clone() const;

  /** Build default palette */
  void buildDefaultPalette();

  /** Build default labels */
  void buildDefaultLabels();

  /** Check for color palette validity */
  static Bool IsValidColorPalette(const Description & palette);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() stores the object through the StorageManager */
  void load(Advocate & adv);

protected:
  /** Check for data validity */
  virtual void checkData(const Point & data) const;

private:

  /** Collection of Strings representing the color palettes */
  Description palette_;

  /** Radius of the Pie */
  Scalar radius_;

  /** Centre of the Pie */
  Point center_;

  /** Labels of the pie sectors */
  Description labels_;

}; /* class Pie */



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PIE_HXX */
