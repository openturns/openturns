//                                               -*- C++ -*-
/**
 *  @brief Pie class for piechart plots
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_PIE_HXX
#define OPENTURNS_PIE_HXX

#include "DrawableImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Pie
 *
 * The class describing a pie chart
 * Instance of Drawable
 */


class OT_API Pie : public DrawableImplementation
{

  CLASSNAME;

public:

  /** Default constructor */
  explicit Pie(const NumericalPoint & data);

  /** Constructor with parameters */
  Pie(const NumericalPoint & data,
      const Description & labels);

  /** Constructor with parameters */
  Pie(const NumericalPoint & data,
      const Description & labels,
      const NumericalPoint & center,
      const NumericalScalar & radius,
      const Description & palette);

  /** String converter */
  String __repr__() const;

  /** Accessor for center */
  NumericalPoint getCenter() const;
  void setCenter(const NumericalPoint & center);

  /** Accessor for radius */
  NumericalScalar getRadius() const;
  void setRadius(const NumericalScalar radius);

  /** Accessor for labels */
  Description getLabels() const;
  void setLabels(const Description & labels);

  /** Accessor for color palette */
  Description getPalette() const;
  void setPalette(const Description & palette);

  /** Accessor for boundingbox */
  BoundingBox getBoundingBox() const;

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
  virtual void checkData(const NumericalPoint & data) const;

private:

  Pie() {};
  friend class Factory<Pie>;

  /** Collection of Strings representing the color palettes */
  Description palette_;

  /** Radius of the Pie */
  NumericalScalar radius_;

  /** Centre of the Pie */
  NumericalPoint center_;

  /** Labels of the pie sectors */
  Description labels_;

}; /* class Pie */



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PIE_HXX */
