//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all Drawable
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Drawable.hxx"
#include "openturns/Curve.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Drawable)

/* Default constructor */
Drawable::Drawable():
  TypedInterfaceObject<DrawableImplementation>(Curve(Sample(1, 2)).clone())
{
  // Nothing to do
}

/* Default constructor */
Drawable::Drawable(const DrawableImplementation & implementation):
  TypedInterfaceObject<DrawableImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation pointer */
Drawable::Drawable(DrawableImplementation * p_implementation)
  : TypedInterfaceObject<DrawableImplementation>(p_implementation)
{
  // Nothing to do
}

/* String converter */
String Drawable::__repr__() const
{
  OSS oss;
  oss << "class=" << Drawable::GetClassName()
      << " name=" << getName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* Here is the interface */

/* Accessor for bounding box of the drawable */
Interval Drawable::getBoundingBox() const
{
  return getImplementation()->getBoundingBox();
}

/* Accessor for legend */
String Drawable::getLegend() const
{
  return getImplementation()->getLegend();
}

/* Accessor for legend */
void Drawable::setLegend(const String & legend)
{
  copyOnWrite();
  getImplementation()->setLegend(legend);
}

/* Accessor for legend line style */
String Drawable::getLineStyle() const
{
  return getImplementation()->getLineStyle();
}

/* Accessor for legend point style */
void Drawable::setLineStyle(const String & lineStyle)
{
  copyOnWrite();
  getImplementation()->setLineStyle(lineStyle);
}

/* Accessor for legend point style */
String Drawable::getPointStyle() const
{
  return getImplementation()->getPointStyle();
}

/* Accessor for legend point style */
void Drawable::setPointStyle(const String & pointStyle)
{
  copyOnWrite();
  getImplementation()->setPointStyle(pointStyle);
}

/* Accessor for legend fill style */
String Drawable::getFillStyle() const
{
  return getImplementation()->getFillStyle();
}

/* Accessor for legend fill style */
void Drawable::setFillStyle(const String & fillStyle)
{
  copyOnWrite();
  getImplementation()->setFillStyle(fillStyle);
}

/* Accessor for color */
String Drawable::getColor() const
{
  return getImplementation()->getColor();
}

String Drawable::getColorCode() const
{
  return getImplementation()->getColorCode();
}

/* Accessor for color */
void Drawable::setColor(const String & color)
{
  copyOnWrite();
  getImplementation()->setColor(color);
}

String Drawable::getEdgeColor() const
{
  return getImplementation()->getEdgeColor();
}

/* Accessor for explicit color validation flag*/
Bool Drawable::getIsColorExplicitlySet() const
{
  return getImplementation()->getIsColorExplicitlySet();
}

/* Accessor for line width */
Scalar Drawable::getLineWidth() const
{
  return getImplementation()->getLineWidth();
}

/* Accessor for line width */
void Drawable::setLineWidth(const Scalar lineWidth)
{
  copyOnWrite();
  getImplementation()->setLineWidth(lineWidth);
}

/* Accessor for pattern */
String Drawable::getPattern() const
{
  return getImplementation()->getPattern();
}

void Drawable::setPattern(const String style)
{
  copyOnWrite();
  getImplementation()->setPattern(style);
}

/* Accessor for center */
Point Drawable::getCenter() const
{
  return getImplementation()->getCenter();
}

void Drawable::setCenter(const Point & center)
{
  copyOnWrite();
  getImplementation()->setCenter(center);
}

/* Accessor for radius */
Scalar Drawable::getRadius() const
{
  return getImplementation()->getRadius();
}

void Drawable::setRadius(const Scalar radius)
{
  copyOnWrite();
  getImplementation()->setRadius(radius);
}

/* Accessor for labels */
Description Drawable::getLabels() const
{
  return getImplementation()->getLabels();
}

void Drawable::setLabels(const Description & labels)
{
  copyOnWrite();
  getImplementation()->setLabels(labels);
}

/* Accessor for color palette */
Description Drawable::getPalette() const
{
  return getImplementation()->getPalette();
}

void Drawable::setPalette(const Description & palette)
{
  copyOnWrite();
  getImplementation()->setPalette(palette);
}

Sample Drawable::getPaletteAsNormalizedRGBA() const
{
  return getImplementation()->getPaletteAsNormalizedRGBA();
}

/* Accessor for origin */
Scalar Drawable::getOrigin() const
{
  return getImplementation()->getOrigin();
}

void Drawable::setOrigin(const Scalar origin)
{
  copyOnWrite();
  getImplementation()->setOrigin(origin);
}

/* Accessor for first coordinate */
Sample Drawable::getX() const
{
  return getImplementation()->getX();
}

void Drawable::setX(const Sample & x)
{
  copyOnWrite();
  getImplementation()->setX(x);
}

/* Accessor for second coordinate */
Sample Drawable::getY() const
{
  return getImplementation()->getY();
}

void Drawable::setY(const Sample & y)
{
  copyOnWrite();
  getImplementation()->setY(y);
}

/* Accessor for levels */
Point Drawable::getLevels() const
{
  return getImplementation()->getLevels();
}

void Drawable::setLevels(const Point & levels)
{
  copyOnWrite();
  getImplementation()->setLevels(levels);
}

/* Accessor for drawLabels */
Bool Drawable::getDrawLabels() const
{
  return getImplementation()->getDrawLabels();
}

void Drawable::setDrawLabels(const Bool & drawLabels)
{
  copyOnWrite();
  getImplementation()->setDrawLabels(drawLabels);
}

/* Accessor for textAnnotations */
Description Drawable::getTextAnnotations() const
{
  return getImplementation()->getTextAnnotations();
}

void Drawable::setTextAnnotations(const Description & textAnnotations)
{
  copyOnWrite();
  getImplementation()->setTextAnnotations(textAnnotations);
}

/* Accessor for textPositions */
Description Drawable::getTextPositions() const
{
  return getImplementation()->getTextPositions();
}

void Drawable::setTextPositions(const Description & textPositions)
{
  copyOnWrite();
  getImplementation()->setTextPositions(textPositions);
}

/* Accessor for font size */
Scalar Drawable::getTextSize() const
{
  return getImplementation()->getTextSize();
}

void Drawable::setTextSize(const Scalar size)
{
  copyOnWrite();
  getImplementation()->setTextSize(size);
}

/* Accessor for data */
Sample Drawable::getData() const
{
  return getImplementation()->getData();
}

/* Give the colors name */
Description Drawable::GetValidColors()
{
  return DrawableImplementation::GetValidColors();
}

/* Give the line style names */
Description Drawable::GetValidLineStyles()
{
  return DrawableImplementation::GetValidLineStyles();
}

/* Give the fill style names */
Description Drawable::GetValidFillStyles()
{
  return DrawableImplementation::GetValidFillStyles();
}

/* Give the point style names */
Description Drawable::GetValidPointStyles()
{
  return DrawableImplementation::GetValidPointStyles();
}

/* Convert an hexadecimal code into an RGB triplet  */
Indices Drawable::ConvertToRGB(const String & key)
{
  return DrawableImplementation::ConvertToRGB(key);
}

/* Convert an hexadecimal code into an RGBA quadruplet  */
Indices Drawable::ConvertToRGBA(const String & key)
{
  return DrawableImplementation::ConvertToRGBA(key);
}

/* Convert an RGB triplet to a valid hexadecimal code */
String Drawable::ConvertFromName(const String & name)
{
  return DrawableImplementation::ConvertFromName(name);
}

/* Convert an RGB triplet to a valid hexadecimal code */
String Drawable::ConvertFromRGB(const UnsignedInteger red,
                                const UnsignedInteger green,
                                const UnsignedInteger blue)
{
  return DrawableImplementation::ConvertFromRGB(red, green, blue);
}

/* Convert an RGBA quadruplet to a valid hexadecimal code */
String Drawable::ConvertFromRGBA(const UnsignedInteger red,
                                 const UnsignedInteger green,
                                 const UnsignedInteger blue,
                                 const UnsignedInteger alpha)
{
  return DrawableImplementation::ConvertFromRGBA(red, green, blue, alpha);
}

/* Convert an RGB triplet to a valid hexadecimal code */
String Drawable::ConvertFromRGB(const Scalar red,
                                const Scalar green,
                                const Scalar blue)
{
  return DrawableImplementation::ConvertFromRGB(red, green, blue);
}

/* Convert an RGBA quadruplet to a valid hexadecimal code */
String Drawable::ConvertFromRGBA(const Scalar red,
                                 const Scalar green,
                                 const Scalar blue,
                                 const Scalar alpha)
{
  return DrawableImplementation::ConvertFromRGBA(red, green, blue, alpha);
}

/* Convert an HSV triplet into an RGB triplet */
Point Drawable::ConvertFromHSVIntoRGB(const Scalar hue,
                                      const Scalar saturation,
                                      const Scalar value)
{
  return DrawableImplementation::ConvertFromHSVIntoRGB(hue, saturation, value);
}

/* Convert an RGB triplet into an HSV triplet */
Point Drawable::ConvertFromRGBIntoHSV(const UnsignedInteger red,
                                      const UnsignedInteger green,
                                      const UnsignedInteger blue)
{
  return DrawableImplementation::ConvertFromRGBIntoHSV(red, green, blue);
}

/* Convert an RGB triplet into an HSV triplet */
Point Drawable::ConvertFromRGBIntoHSV(const Scalar red,
                                      const Scalar green,
                                      const Scalar blue)
{
  return DrawableImplementation::ConvertFromRGBIntoHSV(red, green, blue);
}

/* Convert an HSV triplet to a valid hexadecimal code */
String Drawable::ConvertFromHSV(const Scalar hue,
                                const Scalar saturation,
                                const Scalar value)
{
  return DrawableImplementation::ConvertFromHSV(hue, saturation, value);
}

/* Convert an HSVA quadruplet to a valid hexadecimal code */
String Drawable::ConvertFromHSVA(const Scalar hue,
                                 const Scalar saturation,
                                 const Scalar value,
                                 const Scalar alpha)
{
  return DrawableImplementation::ConvertFromHSVA(hue, saturation, value, alpha);
}

/* Build default palette */
Description Drawable::BuildDefaultPalette(const UnsignedInteger size)
{
  return DrawableImplementation::BuildDefaultPalette(size);
}

/* Build rainbow palette
   Cycle through the hue wheel with 10 nuances and increasing darkness */
Description Drawable::BuildRainbowPalette(const UnsignedInteger size)
{
  return DrawableImplementation::BuildRainbowPalette(size);
}

/* Build tableau palette
   Use 10 colors from Tableau palette. */
Description Drawable::BuildTableauPalette(const UnsignedInteger size)
{
  return DrawableImplementation::BuildTableauPalette(size);
}

END_NAMESPACE_OPENTURNS
