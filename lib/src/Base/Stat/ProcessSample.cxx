//                                               -*- C++ -*-
/**
 *  @brief ProcessSample Class
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

#include "openturns/ProcessSample.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ProcessSample)

ProcessSample::ProcessSample()
  : TypedInterfaceObject<ProcessSampleImplementation>(new ProcessSampleImplementation)
{
  // Nothing to do
}

/* Default constructor */
ProcessSample::ProcessSample(const UnsignedInteger size,
                             const Field & field)
  : TypedInterfaceObject<ProcessSampleImplementation>(new ProcessSampleImplementation(size, field))
{
  // Nothing to do
}

ProcessSample::ProcessSample(const Mesh & mesh,
                             const UnsignedInteger size,
                             const UnsignedInteger dimension)
  : TypedInterfaceObject<ProcessSampleImplementation>(new ProcessSampleImplementation(mesh, size, dimension))
{
  // Nothing to do
}


/* Parameters constructor */
ProcessSample::ProcessSample(const ProcessSampleImplementation & implementation)
  : TypedInterfaceObject<ProcessSampleImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
ProcessSample::ProcessSample(const Implementation & p_implementation)
  : TypedInterfaceObject<ProcessSampleImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
ProcessSample::ProcessSample(ProcessSampleImplementation * p_implementation)
  : TypedInterfaceObject<ProcessSampleImplementation>(p_implementation)
{
  // Nothing to do
}

/* String converter */
String ProcessSample::__repr__() const
{
  return getImplementation()->__repr__();
}

String ProcessSample::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

/* Partial copy constructor */
void ProcessSample::add(const Field & field)
{
  copyOnWrite();
  getImplementation()->add(field);
}

void ProcessSample::add(const Sample & values)
{
  copyOnWrite();
  getImplementation()->add(values);
}


/* Operators accessors */
Field ProcessSample::getField(const UnsignedInteger index) const
{
  return getImplementation()->getField(index);
}

void ProcessSample::setField(const Field & field,
                             const UnsignedInteger index)
{
  copyOnWrite();
  getImplementation()->setField(field, index);
}

Sample & ProcessSample::operator[] (const UnsignedInteger index)
{
  copyOnWrite();
  return getImplementation()->operator[](index);
}

const Sample & ProcessSample::operator[] (const UnsignedInteger index) const
{
  return getImplementation()->operator[](index);
}

/* Time grid accessors */
RegularGrid ProcessSample::getTimeGrid() const
{
  return getImplementation()->getTimeGrid();
}

/* Mesh accessors */
Mesh ProcessSample::getMesh() const
{
  return getImplementation()->getMesh();
}

/* Dimension accessors */
UnsignedInteger ProcessSample::getDimension() const
{
  return getImplementation()->getDimension();
}

/* Dimension accessors */
UnsignedInteger ProcessSample::getSize() const
{
  return getImplementation()->getSize();
}

Field ProcessSample::computeMean() const
{
  return getImplementation()->computeMean();
}

/* Compute the sample of spatial means of each field */
Sample ProcessSample::computeTemporalMean() const
{
  return getImplementation()->computeTemporalMean();
}

/* Compute the sample of spatial means of each field */
Sample ProcessSample::computeSpatialMean() const
{
  return getImplementation()->computeSpatialMean();
}

/*
 * Method computeQuantilePerComponent() gives the quantile per component of the sample
 */
Field ProcessSample::computeQuantilePerComponent(const Scalar prob) const
{
  return getImplementation()->computeQuantilePerComponent(prob);
}

ProcessSample ProcessSample::computeQuantilePerComponent(const Point & prob) const
{
  return getImplementation()->computeQuantilePerComponent(prob);
}

/* Get the i-th marginal process sample */
ProcessSample ProcessSample::getMarginal(const UnsignedInteger index) const
{
  return getImplementation()->getMarginal(index);
}

/* Get the marginal field corresponding to indices dimensions */
ProcessSample ProcessSample::getMarginal(const Indices & indices) const
{
  return getImplementation()->getMarginal(indices);
}

/* Draw a marginal of the ProcessSample, ie the collection of all the Field marginals */
Graph ProcessSample::drawMarginal(const UnsignedInteger index,
                                  const Bool interpolate) const
{
  return getImplementation()->drawMarginal(index, interpolate);
}


END_NAMESPACE_OPENTURNS
