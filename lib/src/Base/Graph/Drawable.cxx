//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all Drawable
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
#include "openturns/Drawable.hxx"
#include "openturns/Curve.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Drawable);

/* Default constructor */
Drawable::Drawable():
  TypedInterfaceObject<DrawableImplementation>(Curve(NumericalSample(1, 2)).clone())
{
  // Nothing to do
}

/* Default constructor */
Drawable::Drawable(const DrawableImplementation & implementation):
  TypedInterfaceObject<DrawableImplementation>(implementation.clone())
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
NumericalPoint Drawable::getBoundingBox() const
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

/* Accessor for line width */
UnsignedInteger Drawable::getLineWidth() const
{
  return getImplementation()->getLineWidth();
}

/* Accessor for line width */
void Drawable::setLineWidth(const UnsignedInteger lineWidth)
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
NumericalPoint Drawable::getCenter() const
{
  return getImplementation()->getCenter();
}

void Drawable::setCenter(const NumericalPoint & center)
{
  copyOnWrite();
  getImplementation()->setCenter(center);
}

/* Accessor for radius */
NumericalScalar Drawable::getRadius() const
{
  return getImplementation()->getRadius();
}

void Drawable::setRadius(const NumericalScalar radius)
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

/* Accessor for origin */
NumericalScalar Drawable::getOrigin() const
{
  return getImplementation()->getOrigin();
}

void Drawable::setOrigin(const NumericalScalar origin)
{
  copyOnWrite();
  getImplementation()->setOrigin(origin);
}

/* Accessor for first coordinate */
NumericalSample Drawable::getX() const
{
  return getImplementation()->getX();
}

void Drawable::setX(const NumericalSample & x)
{
  copyOnWrite();
  getImplementation()->setX(x);
}

/* Accessor for second coordinate */
NumericalSample Drawable::getY() const
{
  return getImplementation()->getY();
}

void Drawable::setY(const NumericalSample & y)
{
  copyOnWrite();
  getImplementation()->setY(y);
}

/* Accessor for levels */
NumericalPoint Drawable::getLevels() const
{
  return getImplementation()->getLevels();
}

void Drawable::setLevels(const NumericalPoint & levels)
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
Indices Drawable::getTextPositions() const
{
  return getImplementation()->getTextPositions();
}

void Drawable::setTextPositions(const Indices & textPositions)
{
  copyOnWrite();
  getImplementation()->setTextPositions(textPositions);
}

/* Accessor for data */
NumericalSample Drawable::getData() const
{
  return getImplementation()->getData();
}

/* Generate de R commands for plotting  the graphic */
String Drawable::draw() const
{
  return getImplementation()->draw();
}

/* Clean all the temporary data created by draw() method */
void Drawable::clean() const
{
  return getImplementation()->clean();
}

/* Get R point code from key */
UnsignedInteger Drawable::getPointCode(const String key) const
{
  return getImplementation()->getPointCode(key);
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
String Drawable::ConvertFromRGB(const NumericalScalar red,
                                const NumericalScalar green,
                                const NumericalScalar blue)
{
  return DrawableImplementation::ConvertFromRGB(red, green, blue);
}

/* Convert an RGBA quadruplet to a valid hexadecimal code */
String Drawable::ConvertFromRGBA(const NumericalScalar red,
                                 const NumericalScalar green,
                                 const NumericalScalar blue,
                                 const NumericalScalar alpha)
{
  return DrawableImplementation::ConvertFromRGBA(red, green, blue, alpha);
}

/* Convert an HSV triplet into an RGB triplet */
NumericalPoint Drawable::ConvertFromHSVIntoRGB(const NumericalScalar hue,
    const NumericalScalar saturation,
    const NumericalScalar value)
{
  return DrawableImplementation::ConvertFromHSVIntoRGB(hue, saturation, value);
}

/* Convert an HSV triplet to a valid hexadecimal code */
String Drawable::ConvertFromHSV(const NumericalScalar hue,
                                const NumericalScalar saturation,
                                const NumericalScalar value)
{
  return DrawableImplementation::ConvertFromHSV(hue, saturation, value);
}

/* Convert an HSVA quadruplet to a valid hexadecimal code */
String Drawable::ConvertFromHSVA(const NumericalScalar hue,
                                 const NumericalScalar saturation,
                                 const NumericalScalar value,
                                 const NumericalScalar alpha)
{
  return DrawableImplementation::ConvertFromHSVA(hue, saturation, value, alpha);
}

/* Build default palette
   Cycle through the hue wheel with 10 nuances and increasing darkness */
Description Drawable::BuildDefaultPalette(const UnsignedInteger size)
{
  return DrawableImplementation::BuildDefaultPalette(size);
}

END_NAMESPACE_OPENTURNS
