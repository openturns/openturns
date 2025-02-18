//                                               -*- C++ -*-
/**
 *  @brief Contour class for contourchart plots
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
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "openturns/Contour.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Contour)

static const Factory<Contour> Factory_Contour;

/* Default constructor */
Contour::Contour()
  : DrawableImplementation()
  , drawLabels_(ResourceMap::GetAsBool("Contour-DefaultDrawLabels"))
  , isFilled_(ResourceMap::GetAsBool("Contour-DefaultIsFilled"))
{
  isColorExplicitlySet_ = true;
  setColorBarPosition(ResourceMap::GetAsString("Contour-DefaultColorBarPosition"));
  setColorMap(ResourceMap::GetAsString("Contour-DefaultColorMap"));
  setAlpha(ResourceMap::GetAsScalar("Contour-DefaultAlpha"));
  setColorMapNorm(ResourceMap::GetAsString("Contour-DefaultColorMapNorm"));
  setExtend(ResourceMap::GetAsString("Contour-DefaultExtend"));
}

/* Constructor with parameters */
Contour::Contour(const UnsignedInteger dimX,
                 const UnsignedInteger dimY,
                 const Sample& data)
  : DrawableImplementation(data)
  , x_(Sample(dimX, 1))
  , y_(Sample(dimY, 1))
  , levels_(Point(ResourceMap::GetAsUnsignedInteger("Contour-DefaultLevelsNumber")))
  , labels_(ResourceMap::GetAsUnsignedInteger("Contour-DefaultLevelsNumber"))
  , drawLabels_(ResourceMap::GetAsBool("Contour-DefaultDrawLabels"))
  , isFilled_(ResourceMap::GetAsBool("Contour-DefaultIsFilled"))
{
  setColorBarPosition(ResourceMap::GetAsString("Contour-DefaultColorBarPosition"));
  setColorMap(ResourceMap::GetAsString("Contour-DefaultColorMap"));
  setAlpha(ResourceMap::GetAsScalar("Contour-DefaultAlpha"));
  setColorMapNorm(ResourceMap::GetAsString("Contour-DefaultColorMapNorm"));
  setExtend(ResourceMap::GetAsString("Contour-DefaultExtend"));
  if (!(dimX >= 2)) throw InvalidArgumentException(HERE) << "Error: the x dimension must be greater or equal to 2, but is " << dimX;
  if (!(dimY >= 2)) throw InvalidArgumentException(HERE) << "Error: the y dimension must be greater or equal to 2, but is " << dimY;
  if (dimX * dimY != data.getSize()) throw InvalidArgumentException(HERE) << "Error: the given dimensions are not compatible with the data";
  // Check data validity
  setData(data);
  // By default, x is assumed to be equally spaced in [0, 1]
  for (UnsignedInteger i = 0; i < dimX; ++i) x_(i, 0) = Scalar(i) / (dimX - 1.0);
  // By default, y is assumed to be equally spaced in [0, 1]
  for (UnsignedInteger i = 0; i < dimY; ++i) y_(i, 0) = Scalar(i) / (dimY - 1.0);
  isColorExplicitlySet_ = true;
  // Build the levels
  buildDefaultLevels();
  // Build the labels
  buildDefaultLabels();
}

/* Constructor with parameters */
Contour::Contour(const Sample & x,
                 const Sample & y,
                 const Sample & data)
  : DrawableImplementation(data)
  , x_(x)
  , y_(y)
  , levels_(Point(ResourceMap::GetAsUnsignedInteger("Contour-DefaultLevelsNumber")))
  , labels_(ResourceMap::GetAsUnsignedInteger("Contour-DefaultLevelsNumber"))
  , drawLabels_(ResourceMap::GetAsBool("Contour-DefaultDrawLabels"))
  , isFilled_(ResourceMap::GetAsBool("Contour-DefaultIsFilled"))
{
  setColorBarPosition(ResourceMap::GetAsString("Contour-DefaultColorBarPosition"));
  setColorMap(ResourceMap::GetAsString("Contour-DefaultColorMap"));
  setAlpha(ResourceMap::GetAsScalar("Contour-DefaultAlpha"));
  setColorMapNorm(ResourceMap::GetAsString("Contour-DefaultColorMapNorm"));
  setExtend(ResourceMap::GetAsString("Contour-DefaultExtend"));
  // Check data validity
  setData(data);
  isColorExplicitlySet_ = true;
  buildDefaultLevels();
  buildDefaultLabels();
}


/* String converter */
String Contour::__repr__() const
{
  OSS oss;
  oss << "class=" << Contour::GetClassName()
      << " name=" << getName()
      << " x=" << x_
      << " y=" << y_
      << " levels=" << levels_
      << " labels=" << labels_
      << " show labels=" << drawLabels_
      << " isFilled=" << isFilled_
      << " colorBarPosition=" << colorBarPosition_
      << " isVminUsed=" << isVminUsed_
      << " vmin=" << vmin_
      << " isVmaxUsed=" << isVmaxUsed_
      << " vmax=" << vmax_
      << " colorMap=" << colorMap_
      << " alpha=" << alpha_
      << " norm=" << norm_
      << " extend=" << extend_
      << " hatches=" << hatches_
      << " derived from " << DrawableImplementation::__repr__();
  return oss;
}

/* Accessor for color overridden to clear colorMap */
void Contour::setColor(const String& color)
{
  DrawableImplementation::setColor(color);
  colorMap_.clear();
}

/* Accessor for first coordinate */
Sample Contour::getX() const
{
  return x_;
}

void Contour::setX(const Sample & x)
{
  x_ = x;
}

/* Accessor for second coordinate */
Sample Contour::getY() const
{
  return y_;
}

void Contour::setY(const Sample & y)
{
  y_ = y;
}

/* Accessor for levels */
Point Contour::getLevels() const
{
  return levels_;
}

void Contour::setLevels(const Point & levels)
{
  levels_ = levels;
  buildDefaultLabels();
}

/* Accessor for labels */
Description Contour::getLabels() const
{
  return labels_;
}

void Contour::setLabels(const Description & labels)
{
  if (labels.getSize() != levels_.getDimension()) throw InvalidArgumentException(HERE) << "Error: the labels size must be equal to the levels dimension";
  labels_ = labels;
}

/* Accessor for drawLabels */
Bool Contour::getDrawLabels() const
{
  return drawLabels_;
}

void Contour::setDrawLabels(const Bool & drawLabels)
{
  drawLabels_ = drawLabels;
}

/** Accessor for isFilled */
Bool Contour::isFilled() const
{
  return isFilled_;
}

void Contour::setIsFilled(Bool isFilled)
{
  isFilled_ = isFilled;
}

/** Accessor for colorBarPosition */
String Contour::getColorBarPosition() const
{
  return colorBarPosition_;
}

void Contour::setColorBarPosition(const String & colorBarPosition)
{
  if (!IsValidColorBarPosition(colorBarPosition))
    throw InvalidArgumentException(HERE) << "Given color bar position = " << colorBarPosition << " is incorrect";
  colorBarPosition_ = colorBarPosition;
}

/** Accessor for isVminUsed */
Bool Contour::isVminUsed() const
{
  return isVminUsed_;
}

void Contour::setIsVminUsed(const Bool used)
{
  isVminUsed_ = used;
}

/** Accessor for vmin */
Scalar Contour::getVmin() const
{
  if(!isVminUsed_)
    throw InternalException(HERE) << "Vmin value is not used";
  return vmin_;
}

void Contour::setVmin(const Scalar vmin)
{
  isVminUsed_ = true;
  vmin_ = vmin;
}

/** Accessor for isVmaxUsed */
Bool Contour::isVmaxUsed() const
{
  return isVmaxUsed_;
}

void Contour::setIsVmaxUsed(const Bool used)
{
  isVmaxUsed_ = used;
}

/** Accessor for vmax */
Scalar Contour::getVmax() const
{
  if(!isVmaxUsed_)
    throw InternalException(HERE) << "Vmax value is not used";
  return vmax_;
}

void Contour::setVmax(const Scalar vmax)
{
  isVmaxUsed_ = true;
  vmax_ = vmax;
}

/** Accessor for colorMap */
String Contour::getColorMap() const
{
  return colorMap_;
}

void Contour::setColorMap(const String& colorMap)
{
  if (!IsValidColorMap(colorMap)) throw InvalidArgumentException(HERE) << "Given color map = " << colorMap << " is incorrect";
  isColorExplicitlySet_ = true; // To avoid being overridden when adding the contour to the graph
  colorMap_ = colorMap;
}

/** Accessor for alpha */
Scalar Contour::getAlpha() const
{
  return alpha_;
}

void Contour::setAlpha(Scalar alpha)
{
  if (alpha < 0 || alpha > 1) throw InvalidArgumentException(HERE) << "Given alpha = " << alpha << " not in [0, 1]";
  alpha_ = alpha;
}

/** Accessor for norm */
String Contour::getColorMapNorm() const
{
  return norm_;
}

void Contour::setColorMapNorm(const String& norm)
{
  if (!IsValidNorm(norm)) throw InvalidArgumentException(HERE) << "Given norm = " << norm << " is incorrect";
  norm_ = norm;
}

/** Accessor for extend */
String Contour::getExtend() const
{
  return extend_;
}
void Contour::setExtend(const String& extend)
{
  if (!IsValidExtend(extend)) throw InvalidArgumentException(HERE) << "Given extend = " << extend << " is incorrect";
  extend_ = extend;
}

/** Accessor for hatches */
Description Contour::getHatches() const
{
  return hatches_;
}

void Contour::setHatches(const Description& hatches)
{
  for(String h : hatches)
    for(char c : h)
      if(!strchr("/\\|-+xoO.*", c))
        throw InvalidArgumentException(HERE) << "Given hatch = " << h << " is incorrect";
  hatches_ = hatches;
}

/* Accessor for boundingbox */
Interval Contour::getBoundingBox() const
{
  Point lowerBound(2);
  lowerBound[0] = x_.getMin()[0];
  lowerBound[1] = y_.getMin()[0];
  Point upperBound(2);
  upperBound[0] = x_.getMax()[0];
  upperBound[1] = y_.getMax()[0];
  return Interval(lowerBound, upperBound);
}

/* Clone method */
Contour * Contour::clone() const
{
  return new Contour(*this);
}

/* Check for data validity */
void Contour::checkData(const Sample & data) const
{
  if (data.getDimension() != 1)
  {
    throw InvalidDimensionException(HERE) << "Expected sample of dimension 1: got " << data.getDimension();
  }
}

/* Build default levels using quantiles associated with regularly spaced probability levels
 */
void Contour::buildDefaultLevels(const UnsignedInteger number)
{
  // Use the empirical quantiles
  const Sample sortedData(data_.sort(0));
  const UnsignedInteger size = data_.getSize();
  levels_ = Point(number);
  for (UnsignedInteger i = 0; i < number; ++i) levels_[i] = sortedData(static_cast<UnsignedInteger>(size * (i + 0.5) / number), 0);
  levels_.erase(std::unique(levels_.begin(), levels_.end()), levels_.end());
}

/* Build default labels by taking the level values */
void Contour::buildDefaultLabels()
{
  const UnsignedInteger number = levels_.getDimension();
  labels_ = Description(number);
  for (UnsignedInteger i = 0; i < number; ++i) labels_[i] = OSS() << levels_[i];
}

/* Method save() stores the object through the StorageManager */
void Contour::save(Advocate & adv) const
{
  DrawableImplementation::save(adv);
  adv.saveAttribute( "x_", x_ );
  adv.saveAttribute( "y_", y_ );
  adv.saveAttribute( "levels_", levels_ );
  adv.saveAttribute( "labels_", labels_ );
  adv.saveAttribute("drawLabels_", drawLabels_);
  adv.saveAttribute("isFilled_", isFilled_);
  adv.saveAttribute("colorBarPosition_", colorBarPosition_);
  adv.saveAttribute("isVminUsed_", isVminUsed_);
  adv.saveAttribute("vmin_", vmin_);
  adv.saveAttribute("isVmaxUsed_", isVmaxUsed_);
  adv.saveAttribute("vmax_", vmax_);
  adv.saveAttribute("colorMap_", colorMap_);
  adv.saveAttribute("alpha_", alpha_);
  adv.saveAttribute("norm_", norm_);
  adv.saveAttribute("extend_", extend_);
  adv.saveAttribute("hatches_", hatches_);
}

/* Method load() reloads the object from the StorageManager */
void Contour::load(Advocate & adv)
{
  DrawableImplementation::load(adv);
  adv.loadAttribute( "x_", x_ );
  adv.loadAttribute( "y_", y_ );
  adv.loadAttribute( "levels_", levels_ );
  adv.loadAttribute( "labels_", labels_ );
  adv.loadAttribute( "drawLabels_", drawLabels_ );
  if (adv.hasAttribute("isFilled_"))
    adv.loadAttribute("isFilled_", isFilled_);
  if (adv.hasAttribute("colorBarPosition_"))
    adv.loadAttribute("colorBarPosition_", colorBarPosition_);
  else
    colorBarPosition_ = "";
  if(adv.hasAttribute("isVminUsed_"))
    adv.loadAttribute("isVminUsed_", isVminUsed_);
  else
    isVminUsed_ = false;
  if(adv.hasAttribute("vmin_"))
    adv.loadAttribute("vmin_", vmin_);
  else
    vmin_ = 0;
  if(adv.hasAttribute("isVmaxUsed_"))
    adv.loadAttribute("isVmaxUsed_", isVmaxUsed_);
  else
    isVmaxUsed_ = false;
  if (adv.hasAttribute("vmax_"))
    adv.loadAttribute("vmax_", vmax_);
  else
    vmax_ = 0;
  if (adv.hasAttribute("colorMap_"))
    adv.loadAttribute("colorMap_", colorMap_);
  else
    colorMap_.clear();
  if (adv.hasAttribute("alpha_"))
    adv.loadAttribute("alpha_", alpha_);
  else
    alpha_ = 1;
  if (adv.hasAttribute("norm_"))
    adv.loadAttribute("norm_", norm_);
  else
    norm_ = ResourceMap::GetAsString("Contour-DefaultColorMapNorm");
  if (adv.hasAttribute("extend_"))
    adv.loadAttribute("extend_", extend_);
  else
    extend_ = ResourceMap::GetAsString("Contour-DefaultExtend");
  if (adv.hasAttribute("hatches_"))
    adv.loadAttribute("hatches_", hatches_);
  else
    hatches_.clear();
}



END_NAMESPACE_OPENTURNS
