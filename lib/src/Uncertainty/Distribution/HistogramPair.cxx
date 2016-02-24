//                                               -*- C++ -*-
/**
 *  @brief The HistogramPair class.
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
#include "openturns/HistogramPair.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HistogramPair);

static const Factory<HistogramPair> RegisteredFactory;

/* Default constructor */
HistogramPair::HistogramPair()
  : width_(1.0)
  , height_(1.0)
  , surface_(1.0)
{
  // Nothing to do
}

/* Parameters constructor */
HistogramPair::HistogramPair(const NumericalScalar width,
                             const NumericalScalar height)
  : width_(width)
  , height_(height)
  , surface_(width * height)
{
  // Check if the data are consistent
  if (width < 0.0) throw InvalidArgumentException(HERE) << "Error: an HistogramPair cannot have a negative width.";
  if (height < 0.0) throw InvalidArgumentException(HERE) << "Error: an HistogramPair cannot have a negative height.";
}

/* Width accessor */
void HistogramPair::setWidth(const NumericalScalar width)
{
  if (width < 0.0) throw InvalidArgumentException(HERE) << "Error: an HistogramPair cannot have a negative width.";
  width_ = width;
  surface_ = width_ * height_;
}

NumericalScalar HistogramPair::getWidth() const
{
  return width_;
}

/* Height accessor */
void HistogramPair::setHeight(const NumericalScalar height)
{
  if (height < 0.0) throw InvalidArgumentException(HERE) << "Error: an HistogramPair cannot have a negative height.";
  height_ = height;
  surface_ = width_ * height_;
}

NumericalScalar HistogramPair::getHeight() const
{
  return height_;
}

NumericalScalar HistogramPair::getSurface() const
{
  return surface_;
}

/* Comparison operator */
Bool HistogramPair::operator == (const HistogramPair & other) const
{
  if (this == &other) return true;
  return (width_ == other.width_) && (height_ == other.height_);
}

/* Virtual constructor */
HistogramPair * HistogramPair::clone() const
{
  return new HistogramPair(*this);
}

String HistogramPair::__repr__() const
{
  OSS oss;
  oss << "(width=" << width_ << ", height=" << height_ << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void HistogramPair::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "width_", width_ );
  adv.saveAttribute( "height_", height_ );
  adv.saveAttribute( "surface_", surface_ );
}

/* Method load() reloads the object from the StorageManager */
void HistogramPair::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "width_", width_ );
  adv.loadAttribute( "height_", height_ );
  adv.loadAttribute( "surface_", surface_ );
}

END_NAMESPACE_OPENTURNS
