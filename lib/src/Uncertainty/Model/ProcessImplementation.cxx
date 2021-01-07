//                                               -*- C++ -*-
/**
 *  @brief An interface for all implementation class of process
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ProcessImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Function.hxx"
#include "openturns/PiecewiseLinearEvaluation.hxx"
#include "openturns/P1LagrangeEvaluation.hxx"
#include "openturns/RegularGrid.hxx"
#include "openturns/Process.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ProcessImplementation)

static const Factory<ProcessImplementation> Factory_ProcessImplementation;

/* Default constructor */
ProcessImplementation::ProcessImplementation()
  : PersistentObject()
  , description_()
  , ouputDimension_(1)
  , mesh_(RegularGrid(0.0, 1.0, 1))
{
  // Nothing to do
}

/* Virtual constructor */
ProcessImplementation * ProcessImplementation::clone() const
{
  return new ProcessImplementation(*this);
}

/* String converter */
String ProcessImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ProcessImplementation::GetClassName()
      << " ouputDimension=" << ouputDimension_
      << " description=" << description_
      << " mesh=" << mesh_;
  return oss;
}

/* String converter */
String ProcessImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << ProcessImplementation::GetClassName()
      << " ouputDimension=" << ouputDimension_
      << " description=" << description_.__str__(offset)
      << " mesh=" << mesh_.__str__(offset);
  return oss;
}

/* Dimension accessor */
UnsignedInteger ProcessImplementation::getInputDimension() const
{
  return mesh_.getDimension();
}

UnsignedInteger ProcessImplementation::getOutputDimension() const
{
  return ouputDimension_;
}

void ProcessImplementation::setOutputDimension(const UnsignedInteger ouputDimension)
{
  ouputDimension_ = ouputDimension;
}

/* Description accessor */
void ProcessImplementation::setDescription(const Description & description)
{
  description_ = description;
}

Description ProcessImplementation::getDescription() const
{
  return description_;
}

/* TimeGrid accessor */
RegularGrid ProcessImplementation::getTimeGrid() const
{
  return getMesh();
}

void ProcessImplementation::setTimeGrid(const RegularGrid & timeGrid)
{
  setMesh(timeGrid);
}

/* Mesh accessor */
Mesh ProcessImplementation::getMesh() const
{
  return mesh_;
}

void ProcessImplementation::setMesh(const Mesh & mesh)
{
  mesh_ = mesh;
}


/* Here is the interface that all derived class must implement */


/* Is the underlying a gaussian process ? */
Bool ProcessImplementation::isNormal() const
{
  return false;
}

/* Is the underlying a stationary process ? */
Bool ProcessImplementation::isStationary() const
{
  return false;
}

/* Is the underlying a composite process ? */
Bool ProcessImplementation::isComposite() const
{
  return false;
}

/* Covariance model accessor */
CovarianceModel ProcessImplementation::getCovarianceModel() const
{
  throw NotYetImplementedException(HERE) << "In ProcessImplementation::getCovarianceModel()";
}

/* Trend accessor */
TrendTransform ProcessImplementation::getTrend() const
{
  throw NotYetImplementedException(HERE) << "In ProcessImplementation::getTrend()";
}

/* Discrete realization accessor */
Field ProcessImplementation::getRealization() const
{
  throw NotYetImplementedException(HERE) << "In ProcessImplementation::getRealization() const";
}

/* Continuous realization accessor */
Function ProcessImplementation::getContinuousRealization() const
{
  // The continuous realization is obtained by a piecewise linear interpolation
  const Field field(getRealization());
  const Sample values(field.getValues());
  if (getInputDimension() == 1)
  {
    const Point locations(mesh_.getVertices().getImplementation()->getData());
    return PiecewiseLinearEvaluation(locations, values);
  }
  return P1LagrangeEvaluation(field);
}

ProcessSample ProcessImplementation::getSample(const UnsignedInteger size) const
{
  ProcessSample result(size, getRealization());
  for (UnsignedInteger i = 1; i < size; ++i) result[i] = getRealization().getValues();
  return result;
}

/* Future accessor */
TimeSeries ProcessImplementation::getFuture(const UnsignedInteger ) const
{
  throw NotYetImplementedException(HERE) << "In ProcessImplementation::getFuture(const UnsignedInteger stepNumber) const";
}

ProcessSample ProcessImplementation::getFuture(const UnsignedInteger stepNumber,
    const UnsignedInteger size) const
{
  if (getInputDimension() != 1) throw NotDefinedException(HERE) << "Error: can extend the realization of a process only if defined on a 1D mesh.";
  if (size == 0) return ProcessSample(mesh_, 0, getOutputDimension());
  ProcessSample result(size, getFuture(stepNumber));
  for (UnsignedInteger i = 1; i < size; ++i) result[i] = getFuture(stepNumber).getValues();
  return result;
}

/* Get the random vector corresponding to the i-th marginal component */
Process ProcessImplementation::getMarginal(const UnsignedInteger i) const
{
  return getMarginal(Indices(1, i));
}

/* Get the marginal random vector corresponding to indices components */
Process ProcessImplementation::getMarginal(const Indices & indices) const
{
  const UnsignedInteger outputDimension = getOutputDimension();
  if (!indices.check(outputDimension)) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal process must be in the range [0, dim-1] and must be different";
  if (outputDimension == 1) return clone();
  throw NotYetImplementedException(HERE) << "In ProcessImplementation::getMarginal(const Indices & indices) const";
}


/* Method save() stores the object through the StorageManager */
void ProcessImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "ouputDimension_", ouputDimension_ );
  adv.saveAttribute( "description_", description_ );
  adv.saveAttribute( "mesh_", mesh_ );
}

/* Method load() reloads the object from the StorageManager */
void ProcessImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "ouputDimension_", ouputDimension_ );
  adv.loadAttribute( "description_", description_ );
  adv.loadAttribute( "mesh_", mesh_ );
}

END_NAMESPACE_OPENTURNS
