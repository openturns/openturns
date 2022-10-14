//                                               -*- C++ -*-
/**
 *  @brief PolygonArray class for polygonArray plots
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/PolygonArray.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PolygonArray)

static const Factory<PolygonArray> Factory_PolygonArray;

/* Default constructor */
PolygonArray::PolygonArray(const String & legend)
  : DrawableImplementation(Sample(0, 2), legend)
  , verticesNumber_(2)
  , palette_(0)
{
  // Nothing to do
}

/* Default constructor */
PolygonArray::PolygonArray(const PolygonCollection & polygons,
                           const String & legend)
  : DrawableImplementation(Sample(0, 2), legend)
  , verticesNumber_(2)
  , palette_(0)
{
  // Convert the collection of polygons into a valid (coordinates, verticesNumber) pair
  const UnsignedInteger size = polygons.getSize();
  if (size > 0)
  {
    palette_ = Description(size);
    // The first polygon gives the number of vertices
    verticesNumber_ = polygons[0].getData().getSize();
    if (!(verticesNumber_ >= 2)) throw InvalidArgumentException(HERE) << "Error: cannot have polygons with less than 2 vertices, here there are " << verticesNumber_ << " vertices.";
    data_ = Sample(size * verticesNumber_, 2);
    UnsignedInteger index = 0;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Sample polygonData(polygons[i].getData());
      if (polygonData.getSize() != verticesNumber_) throw InvalidArgumentException(HERE) << "Error: expected polygons with " << verticesNumber_ << " but polygon " << i << " has " << polygonData.getSize() << " vertices.";
      for (UnsignedInteger j = 0; j < verticesNumber_; ++j)
      {
        data_[index] = polygonData[j];
        ++index;
      } // j
      palette_[i] = polygons[i].getColor();
    } // i
  } // size > 0
}

/* Constructor from 2 data sets */
PolygonArray::PolygonArray(const Sample & coordinates,
                           const UnsignedInteger verticesNumber,
                           const Description & palette,
                           const String & legend)
  : DrawableImplementation(Sample(0, 2), legend)
  , verticesNumber_(0)
  , palette_(palette)
{
  // First, set the coordinates and the verticesNumber
  setCoordinatesAndVerticesNumber(coordinates, verticesNumber);
  // Second, set the palette as the check is done wrt the previous data
  setPalette(palette);
}

/* String converter */
String PolygonArray::__repr__() const
{
  OSS oss;
  oss << "class=" << PolygonArray::GetClassName()
      << " name=" << getName()
      << " derived from " << DrawableImplementation::__repr__()
      << " vertices number=" << verticesNumber_
      << " palette=" << palette_;
  return oss;
}

/* Clone method */
PolygonArray * PolygonArray::clone() const
{
  return new PolygonArray(*this);
}

/* Check validity of data */
void PolygonArray::checkData(const Sample & data) const
{
  if (data.getDimension() != 2)
  {
    throw InvalidDimensionException(HERE) << "Expected sample of dimension 2: got " << data.getDimension();
  }
}

/* Palette of the edge/face of the polygons */
void PolygonArray::setPalette(const Description & palette)
{
  if (data_.getSize() != verticesNumber_ * palette.getSize()) throw InvalidArgumentException(HERE) << "Error: expected a palette with " << data_.getSize() / verticesNumber_ << " colors, got " << palette.getSize();
  palette_ = palette;
}

Description PolygonArray::getPalette() const
{
  return palette_;
}

/* Coordinates and vertices number of the vertices of the polygons */
void PolygonArray::setCoordinatesAndVerticesNumber(const Sample & coordinates,
    const UnsignedInteger verticesNumber)
{
  if (!(verticesNumber >= 2)) throw InvalidArgumentException(HERE) << "Error: cannot have polygons with less than 2 vertices, here there are " << verticesNumber << " vertices.";
  if (coordinates.getSize() % verticesNumber != 0) throw InvalidArgumentException(HERE) << "Error: the coordinates size=" << coordinates.getSize() << " is not compatible with the vertices number=" << verticesNumber;
  setData(coordinates);
  verticesNumber_ = verticesNumber;
}

/* Coordinates accessor */
Sample PolygonArray::getCoordinates() const
{
  return getData();
}

/* Vertices number accessor */
UnsignedInteger PolygonArray::getVerticesNumber() const
{
  return verticesNumber_;
}

/* Method save() stores the object through the StorageManager */
void PolygonArray::save(Advocate & adv) const
{
  DrawableImplementation::save(adv);
  adv.saveAttribute( "verticesNumber_", verticesNumber_ );
  adv.saveAttribute( "palette_", palette_ );
}

/* Method load() reloads the object from the StorageManager */
void PolygonArray::load(Advocate & adv)
{
  DrawableImplementation::load(adv);
  adv.loadAttribute( "verticesNumber_", verticesNumber_ );
  adv.loadAttribute( "palette_", palette_ );
}

END_NAMESPACE_OPENTURNS
