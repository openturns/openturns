//                                               -*- C++ -*-
/**
 *  @brief The class that implements all random vectors
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

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Process.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Process)

/* Default constructor */
Process::Process()
  : TypedInterfaceObject<ProcessImplementation>(new ProcessImplementation())
{
  // Nothing to do
}

/* Parameters constructor */
Process::Process(const ProcessImplementation & implementation)
  : TypedInterfaceObject<ProcessImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
Process::Process(const Implementation & p_implementation)
  : TypedInterfaceObject<ProcessImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
Process::Process(ProcessImplementation * p_implementation)
  : TypedInterfaceObject<ProcessImplementation>(p_implementation)
{
  // Nothing to do
}

/* String converter */
String Process::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* String converter */
String Process::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

/* Description Accessor */
void Process::setDescription(const Description & description)
{
  copyOnWrite();
  getImplementation()->setDescription(description);
}


/* Description Accessor */
Description Process::getDescription() const
{
  return getImplementation()->getDescription();
}

/* Is the underlying a gaussian process ? */
Bool Process::isNormal() const
{
  return getImplementation()->isNormal();
}

/* Is the underlying a stationary process ? */
Bool Process::isStationary() const
{
  return getImplementation()->isStationary();
}

/* Is the underlying a composite process ? */
Bool Process::isComposite() const
{
  return getImplementation()->isComposite();
}

/* Covariance model accessor */
CovarianceModel Process::getCovarianceModel() const
{
  return getImplementation()->getCovarianceModel();
}

/* Trend accessor */
TrendTransform Process::getTrend() const
{
  return getImplementation()->getTrend();
}

/* Here is the interface that all derived class may implement */

/* Dimension accessor */
UnsignedInteger Process::getInputDimension() const
{
  return getImplementation()->getInputDimension();
}

UnsignedInteger Process::getOutputDimension() const
{
  return getImplementation()->getOutputDimension();
}

/* Discrete realization accessor */
Field Process::getRealization() const
{
  return getImplementation()->getRealization();
}

/* Continuous realization accessor */
Function Process::getContinuousRealization() const
{
  return getImplementation()->getContinuousRealization();
}

/* TimeGrid accessor - get */
RegularGrid Process::getTimeGrid() const
{
  return getImplementation()->getTimeGrid();
}

/* TimeGrid accessor - set */
void Process::setTimeGrid(const RegularGrid & timeGrid)
{
  copyOnWrite();
  return getImplementation()->setTimeGrid(timeGrid);
}

/* Mesh accessor - get */
Mesh Process::getMesh() const
{
  return getImplementation()->getMesh();
}

/* Mesh accessor - set */
void Process::setMesh(const Mesh & mesh)
{
  copyOnWrite();
  return getImplementation()->setMesh(mesh);
}

ProcessSample Process::getSample(const UnsignedInteger size) const
{
  return getImplementation()->getSample(size);
}

TimeSeries Process::getFuture(const UnsignedInteger stepNumber) const
{
  return getImplementation()->getFuture(stepNumber);
}

ProcessSample Process::getFuture(const UnsignedInteger stepNumber,
                                 const UnsignedInteger size) const
{
  return getImplementation()->getFuture(stepNumber, size);
}

/* Get the process corresponding to the i-th marginal component */
Process Process::getMarginal(const UnsignedInteger i) const
{
  return getImplementation()->getMarginal(i);
}

/* Get the marginal random vector corresponding to indices components */
Process Process::getMarginal(const Indices & indices) const
{
  return getImplementation()->getMarginal(indices);
}
END_NAMESPACE_OPENTURNS
