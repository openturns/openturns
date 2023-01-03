//                                               -*- C++ -*-
/**
 *  @brief Contour class for contourchart plots
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
#include <cstdio>
#include <cstdlib>

#include "openturns/Contour.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Contour)

static const Factory<Contour> Factory_Contour;

/* Default constructor */
Contour::Contour()
  : DrawableImplementation()
  , drawLabels_(true)
{}

/* Default constructor */
Contour::Contour(const UnsignedInteger dimX,
                 const UnsignedInteger dimY,
                 const Sample & data,
                 const String & legend)
  : DrawableImplementation(data, legend)
  , x_(Sample(dimX, 1))
  , y_(Sample(dimY, 1))
  , levels_(Point(ResourceMap::GetAsUnsignedInteger( "Contour-DefaultLevelsNumber" )))
  , labels_(ResourceMap::GetAsUnsignedInteger( "Contour-DefaultLevelsNumber" ))
  , drawLabels_(true)
{
  if (!(dimX >= 2)) throw InvalidArgumentException(HERE) << "Error: the x dimension must be greater or equal to 2, but is " << dimX;
  if (!(dimY >= 2)) throw InvalidArgumentException(HERE) << "Error: the y dimension must be greater or equal to 2, but is " << dimY;
  if (dimX * dimY != data.getSize()) throw InvalidArgumentException(HERE) << "Error: the given dimensions are not compatible with the data";
  // Check data validity
  setData(data);
  // By default, x is assumed to be equally spaced in [0, 1]
  for (UnsignedInteger i = 0; i < dimX; ++i) x_(i, 0) = Scalar(i) / (dimX - 1.0);
  // By default, y is assumed to be equally spaced in [0, 1]
  for (UnsignedInteger i = 0; i < dimY; ++i) y_(i, 0) = Scalar(i) / (dimY - 1.0);
  // Build the levels
  buildDefaultLevels();
  // Build the labels
  buildDefaultLabels();
}

/* Constructor with parameters */
Contour::Contour(const Sample & x,
                 const Sample & y,
                 const Sample & data,
                 const Point & levels,
                 const Description & labels,
                 const Bool drawLabels,
                 const String & legend)
  : DrawableImplementation(data, legend)
  , x_(x)
  , y_(y)
  , levels_(levels)
  , labels_(labels)
  , drawLabels_(drawLabels)
{
  if (levels.getDimension() == 0) buildDefaultLevels();
  if (drawLabels && (labels.getSize() == 0)) buildDefaultLabels();
  if (drawLabels && (levels.getDimension() > 0) && (labels.getSize() > 0) && (levels.getDimension() != labels.getSize())) throw InvalidArgumentException(HERE) << "Error: the levels are incompatible with the labels";
  // Check data validity
  setData(data);
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
      << " derived from " << DrawableImplementation::__repr__();
  return oss;
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
  adv.saveAttribute( "drawLabels_", drawLabels_ );
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
}



END_NAMESPACE_OPENTURNS
