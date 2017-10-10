//                                               -*- C++ -*-
/**
 *  @brief The class Field implements samples indexed by a position
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include <fstream>
#include "openturns/Field.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Field)


/* Default constructor */
Field::Field()
  : TypedInterfaceObject<FieldImplementation>(FieldImplementation().clone())
{
  // Nothing to do
}

/* Constructor from implementation */
Field::Field(const FieldImplementation & implementation)
  : TypedInterfaceObject<FieldImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
Field::Field(const Implementation & implementation)
  : TypedInterfaceObject<FieldImplementation>(implementation)
{
  // Nothing to do
}

/* Constructor from a TimeGrid and a dimension */
Field::Field(const Mesh & mesh,
             const UnsignedInteger dim)
  : TypedInterfaceObject<FieldImplementation>(new FieldImplementation(mesh, dim))
{
  // Nothing to do
}

/* Constructor from a TimeGrid and a sample */
Field::Field(const Mesh & mesh,
             const Sample & sample)
  : TypedInterfaceObject<FieldImplementation>(new FieldImplementation(mesh, sample))
{
  // Nothing to do
}

/* Comparison operator */
Bool Field::operator ==(const Field & other) const
{
  return *getImplementation() == *(other.getImplementation());
}

/* Individual value accessor */
NSI_point Field::operator[](const UnsignedInteger index)
{
  copyOnWrite();
  return getImplementation()->operator[](index);
}

NSI_const_point Field::operator[](const UnsignedInteger index) const
{
  return getImplementation()->operator[](index);
}

Scalar & Field::operator () (const UnsignedInteger i,
                             const UnsignedInteger j)
{
#ifdef DEBUG_BOUNDCHECKING
  // No copyOnWrite() as the at() method already do it
  return this->at(i, j);
#else
  copyOnWrite();
  return (*getImplementation())[i][j];
#endif /* DEBUG_BOUNDCHECKING */
}

const Scalar & Field::operator () (const UnsignedInteger i,
                                   const UnsignedInteger j) const
{
#ifdef DEBUG_BOUNDCHECKING
  return this->at(i, j);
#else
  return (*getImplementation())[i][j];
#endif /* DEBUG_BOUNDCHECKING */
}


NSI_point Field::at (const UnsignedInteger index)
{
  if (index >= getSize()) throw OutOfBoundException(HERE) << "Index (" << index << ") is not less than size (" << getSize() << ")";
  copyOnWrite();
  return (*getImplementation())[index];
}

NSI_const_point Field::at (const UnsignedInteger index) const
{
  if (index >= getSize()) throw OutOfBoundException(HERE) << "Index (" << index << ") is not less than size (" << getSize() << ")";
  return (*getImplementation())[index];
}

Scalar & Field::at (const UnsignedInteger i,
                    const UnsignedInteger j)
{
  if (i >= getSize()) throw OutOfBoundException(HERE) << "i (" << i << ") is not less than size (" << getSize() << ")";
  if (j > getDimension()) throw OutOfBoundException(HERE) << "j (" << j << ") is greater than dimension (" << getDimension() << ")";
  copyOnWrite();
  return (*getImplementation())[i][j];
}

const Scalar & Field::at (const UnsignedInteger i,
                          const UnsignedInteger j) const
{
  if (i >= getSize()) throw OutOfBoundException(HERE) << "i (" << i << ") is not less than size (" << getSize() << ")";
  if (j > getDimension()) throw OutOfBoundException(HERE) << "j (" << j << ") is greater than dimension (" << getDimension() << ")";
  return (*getImplementation())[i][j];
}

/* Values accessor */
Point Field::getValueAtIndex(const UnsignedInteger index) const
{
  return getImplementation()->getValueAtIndex(index);
}

void Field::setValueAtIndex(const UnsignedInteger index,
                            const Point & val)
{
  copyOnWrite();
  getImplementation()->setValueAtIndex(index, val);
}

Point Field::getValueAtNearestPosition(const Point & position) const
{
  return getImplementation()->getValueAtNearestPosition(position);
}

void Field::setValueAtNearestPosition(const Point & position,
                                      const Point & val)
{
  copyOnWrite();
  getImplementation()->setValueAtNearestPosition(position, val);
}

Mesh Field::getMesh() const
{
  return getImplementation()->getMesh();
}

RegularGrid Field::getTimeGrid() const
{
  return getImplementation()->getTimeGrid();
}

/* String converter */
String Field::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " name=" << getName()
         << " description=" << getDescription()
         << " implementation=" << getImplementation()->__repr__();
}

String Field::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}


/* Description accessor */
void Field::setDescription(const Description & description)
{
  copyOnWrite();
  getImplementation()->setDescription(description);
}



/* Description accessor */
Description Field::getDescription() const
{
  return getImplementation()->getDescription();
}


/* Size accessor */
UnsignedInteger Field::getSize() const
{
  return getImplementation()->getSize();
}

/* Dimension accessor */
UnsignedInteger Field::getSpatialDimension() const
{
  return getImplementation()->getSpatialDimension();
}

UnsignedInteger Field::getDimension() const
{
  return getImplementation()->getDimension();
}


/* Return the values stored in the field as a sample */
Sample Field::getSample() const
{
  return getImplementation()->getSample();
}

Sample Field::getValues() const
{
  return getImplementation()->getValues();
}

void Field::setValues(const Sample & values)
{
  getImplementation()->setValues(values);
}

/* Get the i-th marginal field */
Field Field::getMarginal(const UnsignedInteger index) const
{
  return getImplementation()->getMarginal(index);
}

/* Get the marginal field corresponding to indices dimensions */
Field Field::getMarginal(const Indices & indices) const
{
  return getImplementation()->getMarginal(indices);
}


/* Return the field as a defomed mesh, ie its values are added to the components of the vertices if the dimensions match */
Mesh Field::asDeformedMesh() const
{
  return getImplementation()->asDeformedMesh();
}

/* Compute the spatial mean of the field */
Point Field::getSpatialMean() const
{
  return getImplementation()->getSpatialMean();
}

/* Compute the temporal mean of the field */
Point Field::getTemporalMean() const
{
  return getImplementation()->getTemporalMean();
}

/* Draw a marginal of the field */
Graph Field::drawMarginal(const UnsignedInteger index,
                          const Bool interpolate) const
{
  return getImplementation()->drawMarginal(index, interpolate);
}

Graph Field::draw() const
{
  return getImplementation()->draw();
}

/* VTK export */
void Field::exportToVTKFile(const String & fileName) const
{
  return getImplementation()->exportToVTKFile(fileName);
}


END_NAMESPACE_OPENTURNS
