//                                               -*- C++ -*-
/**
 *  @brief BarPlot class for barplots
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/BarPlot.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(BarPlot);

static const Factory<BarPlot> RegisteredFactory;

/* Default constructor */
BarPlot::BarPlot(const NumericalSample & data,
                 const NumericalScalar origin,
                 const String & legend)
  : DrawableImplementation(data, legend)
  , origin_(origin)
{
  // Check data validity
  setData(data);
}

/* Constructor with parameters */
BarPlot::BarPlot(const NumericalSample & data,
                 const NumericalScalar origin,
                 const String & color,
                 const String & fillStyle,
                 const String & lineStyle,
                 const UnsignedInteger lineWidth,
                 const String & legend)
  : DrawableImplementation(data, legend)
  , origin_(origin)
{
  // Check data validity
  setData(data);
  setColor(color);
  setFillStyle(fillStyle);
  setLineStyle(lineStyle);
  setLineWidth(lineWidth);
}

/* Constructor with old parameters */
BarPlot::BarPlot(const NumericalSample & data,
                 const NumericalScalar origin,
                 const String & color,
                 const String & fillStyle,
                 const String & lineStyle,
                 const String & legend)
  : DrawableImplementation(data, legend)
  , origin_(origin)
{
  // Check data validity
  setData(data);
  setColor(color);
  setFillStyle(fillStyle);
  setLineStyle(lineStyle);
  setLineWidth(1);
}

/* String converter */
String BarPlot::__repr__() const
{
  OSS oss;
  oss << "class=" << BarPlot::GetClassName()
      << " name=" << getName()
      << " origin=" << origin_
      << " derived from " << DrawableImplementation::__repr__();
  return oss;
}

/* Accessor for origin */
NumericalScalar BarPlot::getOrigin() const
{
  return origin_;
}

/* Accessor for origin */
void BarPlot::setOrigin(const NumericalScalar origin)
{
  origin_ = origin;
}

/* Accessor for boundingbox */
BarPlot::BoundingBox BarPlot::getBoundingBox() const
{
  BoundingBox boundingBox(BoundingBoxSize);
  const NumericalPoint max(data_.getMax());
  const NumericalPoint min(data_.getMin());
  boundingBox[0] = origin_;
  boundingBox[1] = origin_;
  const UnsignedInteger size(data_.getSize());
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    boundingBox[1] += data_[i][0];
  }
  boundingBox[2] = std::min(0.0, min[1]);
  boundingBox[3] = std::max(0.0, max[1]);
  return boundingBox;
}

/* Draw method */
String BarPlot::draw() const
{
  dataFileName_ = "";
  OSS oss;
  // Stores the data in a temporary file
  oss << DrawableImplementation::draw() << "\n";
  // The specific R command for drawing
  oss << "x <- dataOT[, 1];"
      << "y <- dataOT[, 2];"
      << "n <- length(y)" << ";"
      << "x <- cumsum(c(" << origin_ << ", x));"
      << "rect(x[1:n], rep(0, n), x[2:(n + 1)], y, col=\"" << color_
      << "\", lwd=" << lineWidth_
      << ",lty=\"" << lineStyle_;

  if(getFillStyle() == "shaded")
    oss << "\", density=20)";
  else oss << "\")";

  return oss;
}

/* Clone method */
BarPlot * BarPlot::clone() const
{
  return new BarPlot(*this);
}

/* Check validity of data */
void BarPlot::checkData(const NumericalSample & data) const
{
  if (data.getDimension() != 2)
  {
    throw InvalidDimensionException(HERE) << "Expected sample of dimension 2: got " << data.getDimension();
  }
}

/* Method save() stores the object through the StorageManager */
void BarPlot::save(Advocate & adv) const
{
  DrawableImplementation::save(adv);
  adv.saveAttribute( "origin_", origin_ );
}

/* Method load() reloads the object from the StorageManager */
void BarPlot::load(Advocate & adv)
{
  DrawableImplementation::load(adv);
  adv.loadAttribute( "origin_", origin_ );
}



END_NAMESPACE_OPENTURNS
