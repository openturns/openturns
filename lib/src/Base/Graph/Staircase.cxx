//                                               -*- C++ -*-
/**
 *  @brief Staircase class for staircase plots
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
#include "openturns/Staircase.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(Staircase)

static const Factory<Staircase> Factory_Staircase;

/* Default constructor */
Staircase::Staircase()
  : DrawableImplementation()
  , pattern_()
{
  // Nothing to do
}

/* Constructor with parameters */
Staircase::Staircase(const Sample & data,
                     const String & legend)
  : DrawableImplementation(data, legend)
  , pattern_(ResourceMap::GetAsString("Drawable-DefaultPattern"))
{
  // Check data validity
  setData(data);
}

/* Constructor with parameters */
Staircase::Staircase(const Sample & data,
                     const String & color,
                     const String & lineStyle,
                     const Scalar lineWidth,
                     const String & pattern,
                     const String & legend)
  : DrawableImplementation(data, legend)
{
  setColor(color);
  setLineStyle(lineStyle);
  setPattern(pattern);
  setLineWidth(lineWidth);
  // Check data validity
  setData(data);
}

/* Constructor with old parameters */
Staircase::Staircase(const Sample & data,
                     const String & color,
                     const String & lineStyle,
                     const String & pattern,
                     const String & legend)
  : DrawableImplementation(data, legend)
{
  setColor(color);
  setLineStyle(lineStyle);
  setPattern(pattern);
  setLineWidth(1);
  // Check data validity
  setData(data);
}

/* String converter */
String Staircase::__repr__() const
{
  OSS oss;
  oss << "class=" << Staircase::GetClassName()
      << " name=" << getName()
      << " pattern=" << pattern_
      << " derived from " << DrawableImplementation::__repr__();
  return oss;
}

/* Draw method */
String Staircase::draw() const
{
  dataFileName_ = "";
  OSS oss;
  // Stores the data in a temporary file
  oss << DrawableImplementation::draw() << "\n";
  // The specific R command for drawing
  oss << "lines(dataOT[, 1], dataOT[, 2]"
      << ", lty=\"" << lineStyle_
      << "\", col=\"" << color_
      << "\", lwd=" << lineWidth_
      << ", type=\"" << pattern_
      << "\")";

  return oss;
}

/* Clone method */
Staircase * Staircase::clone() const
{
  return new Staircase(*this);
}

/* Accessor for pattern */
String Staircase::getPattern() const
{
  return pattern_;
}

/* Accessor for pattern */
void Staircase::setPattern(const String style)
{
  if(!IsValidPattern(style)) throw InvalidArgumentException(HERE) << "Given staircase pattern = " << style << " is incorrect";
  pattern_ = style;
}

/* Check validity of data */
void Staircase::checkData(const Sample & data) const
{
  if (data.getDimension() != 2)
  {
    throw InvalidDimensionException(HERE) << "Expected sample of dimension 2: got " << data.getDimension();
  }
}

/* Method save() stores the object through the StorageManager */
void Staircase::save(Advocate & adv) const
{
  DrawableImplementation::save(adv);
  adv.saveAttribute( "pattern_", pattern_ );
}

/* Method load() reloads the object from the StorageManager */
void Staircase::load(Advocate & adv)
{
  DrawableImplementation::load(adv);
  adv.loadAttribute( "pattern_", pattern_ );
}



END_NAMESPACE_OPENTURNS
