//                                               -*- C++ -*-
/**
 *  @brief Polygon class for polygon plots
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
#include "openturns/Polygon.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Polygon)

static const Factory<Polygon> Factory_Polygon;

/* Default constructor */
Polygon::Polygon(const String & legend)
  : DrawableImplementation(Sample(0, 2), legend)
  , edgeColor_("")
{
  // Nothing to do
  setEdgeColor(getColor());
}

/* Default constructor */
Polygon::Polygon(const Sample & data,
                 const String & legend)
  : DrawableImplementation(Sample(0, 2), legend)
  , edgeColor_("")
{
  // Check data validity
  setData(data);
  setEdgeColor(getColor());
}

/* Constructor from 2 data sets */
Polygon::Polygon(const Sample & dataX,
                 const Sample & dataY,
                 const String & legend)
  : DrawableImplementation(Sample(0, 2), legend)
  , edgeColor_("")
{
  const UnsignedInteger size = dataX.getSize();
  if (dataY.getSize() != size) throw InvalidArgumentException(HERE) << "Error: cannot build a Polygon based on two numerical samples with different size.";
  if ((dataX.getDimension() != 1) || (dataY.getDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: cannot build a Polygon based on two numerical samples of dimension greater than 1.";
  Sample dataFull(size, 2);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    dataFull(i, 0) = dataX(i, 0);
    dataFull(i, 1) = dataY(i, 0);
  }
  // Check data validity
  setData(dataFull);
  setEdgeColor(getColor());
}

/* Constructor from 2 data sets */
Polygon::Polygon(const Point & dataX,
                 const Point & dataY,
                 const String & legend)
  : DrawableImplementation(Sample(0, 2), legend)
  , edgeColor_("")
{
  const UnsignedInteger size = dataX.getDimension();
  if (dataY.getDimension() != size) throw InvalidArgumentException(HERE) << "Error: cannot build a Polygon based on two numerical points with different size.";
  Sample dataFull(size, 2);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    dataFull(i, 0) = dataX[i];
    dataFull(i, 1) = dataY[i];
  }
  // Check data validity
  setData(dataFull);
  setEdgeColor(getColor());
}

/* Constructor with parameters */
Polygon::Polygon(const Sample & data,
                 const String & color,
                 const String & edgeColor,
                 const String & legend)
  : DrawableImplementation(Sample(0, 2), legend)
  , edgeColor_("")
{
  Sample dataFull;
  // If data is unidimensional, assume that it means Y values with indices as X values
  if (data.getDimension() == 1)
  {
    const UnsignedInteger size = data.getSize();
    dataFull = Sample(size, 2);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      dataFull(i, 0) = i;
      dataFull(i, 1) = data(i, 0);
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

/* Clone method */
Polygon * Polygon::clone() const
{
  return new Polygon(*this);
}
/* Check validity of data */
void Polygon::checkData(const Sample & data) const
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

/* Builds a polygon which fills the area between two curves */
Polygon Polygon::FillBetween(Sample const& dataX, Sample const& dataY1, Sample const& dataY2)
{
  return FillBetween(dataX.asPoint(), dataY1.asPoint(), dataY2.asPoint());
}

/* Builds a polygon which fills the area between two curves */
Polygon Polygon::FillBetween(Point const& dataX, Point const& dataY1, Point const& dataY2)
{
  const UnsignedInteger size = dataX.getSize();
  if ((dataY1.getSize() != size) || (dataY2.getSize() != size))
    throw InvalidArgumentException(HERE) << "Error: cannot fill between curves based on numerical samples with different size.";
  Sample dataFull(size * 2, 2);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    dataFull(i, 0) = dataX[i];
    dataFull(i, 1) = dataY1[i];
    dataFull(i + size, 0) = dataX[size - i - 1];
    dataFull(i + size, 1) = dataY2[size - i - 1];
  }
  Polygon polygon = Polygon(dataFull);
  polygon.setLineWidth(0); // To only draw between the curves
  return polygon;
}

END_NAMESPACE_OPENTURNS
