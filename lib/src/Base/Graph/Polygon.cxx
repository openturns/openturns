//                                               -*- C++ -*-
/**
 *  @brief Polygon class for polygon plots
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
#include "Polygon.hxx"
#include "PersistentObjectFactory.hxx"
#include "Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Polygon);

static const Factory<Polygon> RegisteredFactory;

/* Default constructor */
Polygon::Polygon(const String & legend)
  : DrawableImplementation(NumericalSample(0, 2), legend)
  , edgeColor_("")
{
  // Nothing to do
  setEdgeColor(getColor());
}

/* Default constructor */
Polygon::Polygon(const NumericalSample & data,
                 const String & legend)
  : DrawableImplementation(NumericalSample(0, 2), legend)
  , edgeColor_("")
{
  // Check data validity
  setData(data);
  setEdgeColor(getColor());
}

/* Contructor from 2 data sets */
Polygon::Polygon(const NumericalSample & dataX,
                 const NumericalSample & dataY,
                 const String & legend)
  : DrawableImplementation(NumericalSample(0, 2), legend)
  , edgeColor_("")
{
  const UnsignedInteger size(dataX.getSize());
  if (dataY.getSize() != size) throw InvalidArgumentException(HERE) << "Error: cannot build a Polygon based on two numerical samples with different size.";
  if ((dataX.getDimension() != 1) || (dataY.getDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: cannot build a Polygon based on two numerical samples of dimension greater than 1.";
  NumericalSample dataFull(size, 2);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    dataFull[i][0] = dataX[i][0];
    dataFull[i][1] = dataY[i][0];
  }
  // Check data validity
  setData(dataFull);
  setEdgeColor(getColor());
}

/* Contructor from 2 data sets */
Polygon::Polygon(const NumericalPoint & dataX,
                 const NumericalPoint & dataY,
                 const String & legend)
  : DrawableImplementation(NumericalSample(0, 2), legend)
  , edgeColor_("")
{
  const UnsignedInteger size(dataX.getDimension());
  if (dataY.getDimension() != size) throw InvalidArgumentException(HERE) << "Error: cannot build a Polygon based on two numerical points with different size.";
  NumericalSample dataFull(size, 2);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    dataFull[i][0] = dataX[i];
    dataFull[i][1] = dataY[i];
  }
  // Check data validity
  setData(dataFull);
  setEdgeColor(getColor());
}

/* Constructor with parameters */
Polygon::Polygon(const NumericalSample & data,
                 const String & color,
                 const String & edgeColor,
                 const String & legend)
  : DrawableImplementation(NumericalSample(0, 2), legend)
  , edgeColor_("")
{
  NumericalSample dataFull;
  // If data is unidimensional, assume that it means Y values with indices as X values
  if (data.getDimension() == 1)
  {
    const UnsignedInteger size(data.getSize());
    dataFull = NumericalSample(size, 2);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      dataFull[i][0] = i;
      dataFull[i][1] = data[i][0];
    }
  }
  else dataFull = data;
  // Check data validity
  setData(dataFull);
  setColor(color);
  setEdgeColor(edgeColor);
}

/* String converter */
String Polygon::__repr__() const
{
  OSS oss;
  oss << "class=" << Polygon::GetClassName()
      << " name=" << getName()
      << " derived from " << DrawableImplementation::__repr__()
      << " edge color=" << edgeColor_;
  return oss;
}

/* Draw method */
String Polygon::draw() const
{
  dataFileName_ = "";
  OSS oss;
  // Stores the data in a temporary file
  oss << DrawableImplementation::draw();
  // The specific R command for drawing
  oss << "polygon(dataOT[,1], dataOT[,2]"
      << ", border="
      << "\"" << edgeColor_ << "\""
      << ", lty=\"" << lineStyle_ << "\""
      << ", col="
      << "\"" << color_ << "\""
      << ", lwd=" << lineWidth_;
  if (pointStyle_ != "none")
  {
    const String code((OSS() << getPointCode(pointStyle_)));
    oss << ", pch=" << (pointStyle_ == "dot" ? "\".\"" : code);
  }
  oss << ")";

  return oss;
}

/* Clone method */
Polygon * Polygon::clone() const
{
  return new Polygon(*this);
}
/* Check validity of data */
void Polygon::checkData(const NumericalSample & data) const
{
  if (data.getDimension() != 2)
  {
    throw InvalidDimensionException(HERE) << "Expected sample of dimension 2: got " << data.getDimension();
  }
}

/* Accessor for edge color */
String Polygon::getEdgeColor() const
{
  return edgeColor_;
}

/* Accessor for edge color */
void Polygon::setEdgeColor(const String & edgeColor)
{
  if(!IsValidColor(edgeColor)) throw InvalidArgumentException(HERE) << "Given edge color = " << edgeColor << " is incorrect";

  edgeColor_ = edgeColor;
}

/* Method save() stores the object through the StorageManager */
void Polygon::save(Advocate & adv) const
{
  DrawableImplementation::save(adv);
  adv.saveAttribute( "edgeColor_", edgeColor_ );
}

/* Method load() reloads the object from the StorageManager */
void Polygon::load(Advocate & adv)
{
  DrawableImplementation::load(adv);
  adv.loadAttribute( "edgeColor_", edgeColor_ );
}

END_NAMESPACE_OPENTURNS
