//                                               -*- C++ -*-
/**
 *  @brief Drawable implements graphic devices for plotting through R
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
#ifndef OPENTURNS_DRAWABLE_HXX
#define OPENTURNS_DRAWABLE_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/DrawableImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS
/**
 * @class Drawable
 *
 * Drawable is an interface to implement graphics
 */

class OT_API Drawable :
  public TypedInterfaceObject<DrawableImplementation>
{
  CLASSNAME;

public:

  /** Default constructor */
  Drawable();

  /** Default constructor */
  Drawable(const DrawableImplementation & implementation);


  /** String converter */
  String __repr__() const;

  /* Here is the interface */

  /** Accessor for bounding box of the drawable */
  NumericalPoint getBoundingBox() const;

  /** Legend accessor */
  virtual String getLegend() const;
  virtual void setLegend(const String & legend);

  /** Accessor for legend line style */
  String getLineStyle() const;
  void setLineStyle(const String & lineStyle);

  /** Accessor for legend point style */
  String getPointStyle() const;
  void setPointStyle(const String & pointStyle);

  /** Accessor for legend fill style */
  String getFillStyle() const;
  void setFillStyle(const String & fillStyle);

  /** Line width accessor */
  UnsignedInteger getLineWidth() const;
  void setLineWidth(const UnsignedInteger lineWidth);

  /** Accessor for color */
  String getColor() const;
  String getColorCode() const;
  void setColor(const String & color);
  String getEdgeColor() const;

  /** Accessor for pattern */
  String getPattern() const;
  void setPattern(const String style);

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

  /** Accessor for origin */
  NumericalScalar getOrigin() const;
  void setOrigin(const NumericalScalar origin);

  /** Accessor for first coordinate */
  NumericalSample getX() const;
  void setX(const NumericalSample & x);

  /** Accessor for second coordinate */
  NumericalSample getY() const;
  void setY(const NumericalSample & y);

  /** Accessor for levels */
  NumericalPoint getLevels() const;
  void setLevels(const NumericalPoint & levels);

  /** Accessor for drawLabels */
  Bool getDrawLabels() const;
  void setDrawLabels(const Bool & drawLabels);

  /** Accessor for textAnnotations */
  Description getTextAnnotations() const;
  void setTextAnnotations(const Description & textAnnotations);

  /** Accessor for textPositions */
  Description getTextPositions() const;
  void setTextPositions(const Description & textPositions);

  /** Accessor for data */
  NumericalSample getData() const;

  /** Generate R commands for plotting the graphic */
  String draw() const;

  /** Get R point code from key */
  UnsignedInteger getPointCode(const String key) const;

  /** Clean all the temporary data created by draw() method */
  void clean() const;

  /** Give the colors name */
  static Description GetValidColors();

  /** Give the line style names */
  static Description GetValidLineStyles();

  /** Give the fill style names */
  static Description GetValidFillStyles();

  /** Give the point style names */
  static Description GetValidPointStyles();

  /** Convert a color name into a valid hexadecimal code */
  static String ConvertFromName(const String & name);

  /** Convert an hexadecimal code into an RGB triplet */
  static Indices ConvertToRGB(const String & key);

  /** Convert an hexadecimal code into an RGBA quadruplet */
  static Indices ConvertToRGBA(const String & key);

  /** Convert an RGB triplet to a valid hexadecimal code */
  static String ConvertFromRGB(const UnsignedInteger red,
                               const UnsignedInteger green,
                               const UnsignedInteger blue);

  /** Convert an RGBA quadruplet to a valid hexadecimal code */
  static String ConvertFromRGBA(const UnsignedInteger red,
                                const UnsignedInteger green,
                                const UnsignedInteger blue,
                                const UnsignedInteger alpha);

  /** Convert an RGB triplet to a valid hexadecimal code */
  static String ConvertFromRGB(const NumericalScalar red,
                               const NumericalScalar green,
                               const NumericalScalar blue);

  /** Convert an RGBA quadruplet to a valid hexadecimal code */
  static String ConvertFromRGBA(const NumericalScalar red,
                                const NumericalScalar green,
                                const NumericalScalar blue,
                                const NumericalScalar alpha);

  /** Convert an HSV triplet into an RGB triplet */
  static NumericalPoint ConvertFromHSVIntoRGB(const NumericalScalar hue,
      const NumericalScalar saturation,
      const NumericalScalar value);

  /** Convert an HSV triplet to a valid hexadecimal code */
  static String ConvertFromHSV(const NumericalScalar hue,
                               const NumericalScalar saturation,
                               const NumericalScalar value);

  /** Convert an HSVA quadruplet to a valid hexadecimal code */
  static String ConvertFromHSVA(const NumericalScalar hue,
                                const NumericalScalar saturation,
                                const NumericalScalar value,
                                const NumericalScalar alpha);

  /** Build default palette */
  static Description BuildDefaultPalette(const UnsignedInteger size);

}; /* class Drawable */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DRAWABLE_HXX */
