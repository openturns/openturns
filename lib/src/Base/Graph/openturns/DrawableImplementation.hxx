//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all drawables
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_DRAWABLEIMPLEMENTATION_HXX
#define OPENTURNS_DRAWABLEIMPLEMENTATION_HXX

#include <map>
#include "openturns/Description.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Interval.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DrawableImplementation
 *
 * The class describes the a drawable graphic
 * Instances of this class can actually be plotted
 */

class OT_API DrawableImplementation :
  public PersistentObject
{

  CLASSNAME

  friend class GraphImplementation;

public:

  /** Default constructor */
  DrawableImplementation();

  /** Default constructor with legend label */
  explicit DrawableImplementation(const Sample & data,
                                  const String & legend = "");

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Comparison operator */
  using PersistentObject::operator ==;
  Bool operator == (const DrawableImplementation & other) const;

  /** Legend accessor */
  virtual String getLegend() const;
  virtual void setLegend(const String & legend);

  /** Data accessor */
  virtual Sample getData() const;

  /** Line style accessor */
  virtual String getLineStyle() const;
  virtual void setLineStyle(const String & lineStyle);

  /** Color accessor */
  virtual String getColor() const;
  virtual String getColorCode() const;
  virtual void setColor(const String & color);
  virtual String getEdgeColor() const;

  /** Point style accessor */
  virtual String getPointStyle() const;
  virtual void setPointStyle(const String & pointStyle);

  /** Filling style (for surfaces) accessor */
  virtual String getFillStyle() const;
  virtual void setFillStyle(const String & fillStyle);

  /** Line width accessor */
  virtual Scalar getLineWidth() const;
  virtual void setLineWidth(const Scalar lineWidth);

  /** Accessor for pattern */
  virtual String getPattern() const;
  virtual void setPattern(const String style);

  /** Accessor for center */
  virtual Point getCenter() const;
  virtual void setCenter(const Point & center);

  /** Accessor for radius */
  virtual Scalar getRadius() const;
  virtual void setRadius(const Scalar radius);

  /** Accessor for labels */
  virtual Description getLabels() const;
  virtual void setLabels(const Description & labels);

  /** Accessor for color palette */
  virtual Description getPalette() const;
  virtual void setPalette(const Description & palette);
  virtual Sample getPaletteAsNormalizedRGBA() const;

  /** Accessor for origin */
  virtual Scalar getOrigin() const;
  virtual void setOrigin(const Scalar origin);

  /** Accessor for first coordinate */
  virtual Sample getX() const;
  virtual void setX(const Sample & x);

  /** Accessor for second coordinate */
  virtual Sample getY() const;
  virtual void setY(const Sample & y);

  /** Accessor for levels */
  virtual Point getLevels() const;
  virtual void setLevels(const Point & levels);

  /** Accessor for drawLabels */
  virtual Bool getDrawLabels() const;
  virtual void setDrawLabels(const Bool & drawLabels);

  /** Accessor for textAnnotations */
  virtual Description getTextAnnotations() const;
  virtual void setTextAnnotations(const Description & textAnnotations);

  /** Accessor for textPositions */
  virtual Description getTextPositions() const;
  virtual void setTextPositions(const Description & textPositions);

  /** Accessor for the text size */
  virtual Scalar getTextSize() const;
  virtual void setTextSize(const Scalar size);

  /** Bounding box accessor */
  virtual Interval getBoundingBox() const;

  /** Clone method */
  DrawableImplementation * clone() const override;

  /** Give the colors name */
  static Description GetValidColors();

  /** Give the line style names */
  static Description GetValidLineStyles();

  /** Give the fill style names */
  static Description GetValidFillStyles();

  /** Give the point style names */
  static Description GetValidPointStyles();

  /** Convert a color name to a valid hexadecimal code */
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

  /** Convert an HSV triplet into an RGB triplet */
  static Point ConvertFromHSVIntoRGB(const Scalar hue,
                                     const Scalar saturation,
                                     const Scalar value);

  /* Convert a RGB triplet to HSV */
  static Point ConvertFromRGBIntoHSV(const UnsignedInteger red,
                                     const UnsignedInteger green,
                                     const UnsignedInteger blue);

  /* Convert a RGB triplet to HSV */
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

  /** Build tableau palette */
  static Description BuildTableauPalette(const UnsignedInteger size);

  /** Build viridis palette */
  static Description BuildViridisPalette(const UnsignedInteger size);

  /** Build gray scale palette */
  static Description BuildGrayScalePalette(const UnsignedInteger size);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() stores the object through the StorageManager */
  void load(Advocate & adv) override;

protected:
  static Bool IsFirstInitialization;

  /** Check validity of color */
  static void ScanColorCode(const String & key,
                            Indices & rgba);
  static Bool IsValidColorCode(const String & key);
  static Bool IsValidColorName(const String & key);
  static Bool IsValidColor(const String & key);

  /** Check validity of line style */
  static Bool IsValidLineStyle(const String & key);

  /** Check validity of point style */
  static Bool IsValidPointStyle(const String & key);

  /** Check validity of fill style */
  static Bool IsValidFillStyle(const String & key);

  /** Check validity of pattern parameter */
  static Bool IsValidPattern(const String & pattern);

  /** A map matching keys with HTML definition for R colors */
  static std::map<String, String> ColorCodes;

  /** Valid line styles */
  static Description ValidLineStyles;

  /** Valid fill styles */
  static Description ValidFillStyles;

  /** Valid point styles */
  static Description ValidPointStyles;

  /** Viridis colors */
  static Description ViridisColors;

  /** Data accessor */
  virtual void setData(const Sample & data);
  virtual void setData(const Point & data);

  /** Check data */
  virtual void checkData(const Sample & data) const;
  virtual void checkData(const Point & data) const;

  /** Explicit color validation flag accessor */
  virtual Bool isColorExplicitlySet() const;

  /** The legend of the drawable, to be displayed in the legend of the graph */
  String legend_;

  /** The data to be plotted */
  Sample data_;

  /** Color of the curve */
  String color_;

  /** Fill style */
  String fillStyle_;

  /** Line style of the curve */
  String lineStyle_;

  /** Line style of the curve */
  String pointStyle_;

  /** Line width of the curve */
  Scalar lineWidth_;

  /** Explicit color validation flag */
  Bool isColorExplicitlySet_ = false;

  /** Initialize Valid parameter list */
  static void InitializeValidParameterList();

}; /* class DrawableImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DRAWABLEIMPLEMENTATION_HXX */
