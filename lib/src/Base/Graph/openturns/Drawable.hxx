//                                               -*- C++ -*-
/**
 *  @brief Drawable implements graphic devices for plotting through R
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
  CLASSNAME

public:

  /** Default constructor */
  Drawable();

  /** Default constructor */
  Drawable(const DrawableImplementation & implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  Drawable(DrawableImplementation * p_implementation);
#endif

  /** String converter */
  String __repr__() const override;

  /* Here is the interface */

  /** Accessor for bounding box of the drawable */
  Interval getBoundingBox() const;

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
  Scalar getLineWidth() const;
  void setLineWidth(const Scalar lineWidth);

  /** Accessor for color */
  String getColor() const;
  String getColorCode() const;
  void setColor(const String & color);
  String getEdgeColor() const;

  /** Accessor for pattern */
  String getPattern() const;
  void setPattern(const String style);

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
  Sample getPaletteAsNormalizedRGBA() const;

  /** Accessor for origin */
  Scalar getOrigin() const;
  void setOrigin(const Scalar origin);

  /** Accessor for first coordinate */
  Sample getX() const;
  void setX(const Sample & x);

  /** Accessor for second coordinate */
  Sample getY() const;
  void setY(const Sample & y);

  /** Accessor for levels */
  Point getLevels() const;
  void setLevels(const Point & levels);

  /** Accessor for drawLabels */
  Bool getDrawLabels() const;
  void setDrawLabels(const Bool & drawLabels);

  /** Accessor for textAnnotations */
  Description getTextAnnotations() const;
  void setTextAnnotations(const Description & textAnnotations);

  /** Accessor for textPositions */
  Description getTextPositions() const;
  void setTextPositions(const Description & textPositions);

  /** Accessor for the text size */
  Scalar getTextSize() const;
  void setTextSize(const Scalar size);

  /** Accessor for data */
  Sample getData() const;

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
  static String ConvertFromRGB(const Scalar red,
                               const Scalar green,
                               const Scalar blue);

  /** Convert an RGBA quadruplet to a valid hexadecimal code */
  static String ConvertFromRGBA(const Scalar red,
                                const Scalar green,
                                const Scalar blue,
                                const Scalar alpha);

  /** Convert an HSV triplet into an RGB triplet where
  0.0<=hue<=360.0, 0.0<=saturation<=1.0, 0.0<=value<=1.0 */
  static Point ConvertFromHSVIntoRGB(const Scalar hue,
                                     const Scalar saturation,
                                     const Scalar value);

  /* Convert a RGB triplet to HSV where
  0<=red<=255, 0<=green<=255, 0<=blue<=255 */
  static Point ConvertFromRGBIntoHSV(const UnsignedInteger red,
                                     const UnsignedInteger green,
                                     const UnsignedInteger blue);

  /* Convert a RGB triplet to HSV where
  0.0<=red<=1.0, 0.0<=green<=1.0, 0.0<=blue<=1.0 */
  static Point ConvertFromRGBIntoHSV(const Scalar red,
                                     const Scalar green,
                                     const Scalar blue);

  /** Convert an HSV triplet to a valid hexadecimal code */
  static String ConvertFromHSV(const Scalar hue,
                               const Scalar saturation,
                               const Scalar value);

  /** Convert an HSVA quadruplet to a valid hexadecimal code */
  static String ConvertFromHSVA(const Scalar hue,
                                const Scalar saturation,
                                const Scalar value,
                                const Scalar alpha);

  /** Build default palette */
  static Description BuildDefaultPalette(const UnsignedInteger size);

  /** Build rainbow palette */
  static Description BuildRainbowPalette(const UnsignedInteger size);

  /** Build tableu palette */
  static Description BuildTableauPalette(const UnsignedInteger size);

}; /* class Drawable */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DRAWABLE_HXX */
