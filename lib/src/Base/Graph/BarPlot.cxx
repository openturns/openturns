//                                               -*- C++ -*-
/**
 *  @brief BarPlot class for barplots
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/BarPlot.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(BarPlot)

static const Factory<BarPlot> Factory_BarPlot;

/* Default constructor */
BarPlot::BarPlot()
  : DrawableImplementation()
  , origin_(0.0)
{}

/* Default constructor */
BarPlot::BarPlot(const Sample & data,
                 const Scalar origin,
                 const String & legend)
  : DrawableImplementation(data, legend)
  , origin_(origin)
{
  // Check data validity
  setData(data);
}

/* Constructor with parameters */
BarPlot::BarPlot(const Sample & data,
                 const Scalar origin,
                 const String & color,
                 const String & fillStyle,
                 const String & lineStyle,
                 const Scalar lineWidth,
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
BarPlot::BarPlot(const Sample & data,
                 const Scalar origin,
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
Scalar BarPlot::getOrigin() const
{
  return origin_;
}

/* Accessor for origin */
void BarPlot::setOrigin(const Scalar origin)
{
  origin_ = origin;
}

/* Accessor for boundingbox */
Interval BarPlot::getBoundingBox() const
{
  const Point min(data_.getMin());
  const Point max(data_.getMax());
  const UnsignedInteger size = data_.getSize();
  Point lowerBound(2);
  lowerBound[0] = origin_;
  lowerBound[1] = std::min(0.0, min[1]);
  Point upperBound(2);
  upperBound[0] = origin_;
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    upperBound[0] += data_(i, 0);
  }
  upperBound[1] = std::max(0.0, max[1]);
  return Interval(lowerBound, upperBound);
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
void BarPlot::checkData(const Sample & data) const
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
