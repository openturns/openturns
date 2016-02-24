//                                               -*- C++ -*-
/**
 *  @brief A class which implements the CompositeProcess process
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

#include "openturns/CompositeProcess.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/WhiteNoise.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CompositeProcess);

static const Factory<CompositeProcess> RegisteredFactory;

CompositeProcess::CompositeProcess()
  : ProcessImplementation()
  , function_(NumericalMathFunction("x", "x^2"))
  , p_antecedent_(WhiteNoise().clone())
{
  // Set the mesh
  setMesh(function_.getOutputMesh(p_antecedent_->getMesh()));
}

/* Standard constructor */
CompositeProcess::CompositeProcess(const DynamicalFunction & function,
                                   const Antecedent & p_antecedent)
  : ProcessImplementation()
{
  *this = CompositeProcess(function, *p_antecedent);
}

/* Standard constructor */
CompositeProcess::CompositeProcess(const DynamicalFunction & function,
                                   const Process & antecedent)
  : ProcessImplementation()
  , function_(function)
  , p_antecedent_(antecedent.getImplementation())
{
  if (function.getInputDimension() != p_antecedent_->getDimension())
    throw InvalidArgumentException(HERE) << "Error: trying to build a CompositeProcess from a Process and a DynamicalFunction with incompatible dimensions "
                                         << "here Process dimension=" << p_antecedent_->getDimension()
                                         << " and DynamicalFunction input dimension=" << function.getInputDimension();
  if (function.getSpatialDimension() != p_antecedent_->getSpatialDimension())
    throw InvalidArgumentException(HERE) << "Error: trying to build a CompositeProcess from a Process and a DynamicalFunction with incompatible mesh dimensions "
                                         << "here Process mesh dimension=" << p_antecedent_->getSpatialDimension()
                                         << " and DynamicalFunction mesh dimension=" << function.getSpatialDimension();
  setMesh(function.getOutputMesh(p_antecedent_->getMesh()));
  setDimension(function.getOutputDimension());
  setDescription(function.getOutputDescription());
}

/* Virtual constructor */
CompositeProcess * CompositeProcess::clone() const
{
  return new CompositeProcess(*this);
}

/* String converter */
String CompositeProcess::__repr__() const
{
  OSS oss(true);
  oss << "class=" << CompositeProcess::GetClassName()
      << " function=" << function_.__repr__()
      << " antecedent=" << p_antecedent_->__repr__();
  return oss;
}

String CompositeProcess::__str__(const String & offset) const
{
  OSS oss(false);
  oss << getClassName() << "(" << function_.__str__()
      << "(" << p_antecedent_->__str__() << ")";
  return oss;
}

/* Is the underlying a composite process ? */
Bool CompositeProcess::isComposite() const
{
  return true;
}

/* Antecedent accessor */
CompositeProcess::Antecedent CompositeProcess::getAntecedent() const
{
  return p_antecedent_;
}

/* Function accessor */
DynamicalFunction CompositeProcess::getFunction() const
{
  return function_;
}

/* Realization accessor */
Field CompositeProcess::getRealization() const
{
  Field result(function_(p_antecedent_->getRealization()));
  NumericalSample values(result.getValues());
  values.setDescription(getDescription());
  result.setValues(values);
  return result;
}

/* Compute the next steps of a random walk */
TimeSeries CompositeProcess::getFuture(const UnsignedInteger stepNumber) const
{
  /* TimeGrid of the process */
  RegularGrid timeGrid;
  try
  {
    timeGrid = getTimeGrid();
  }
  catch (...)
  {
    throw InternalException(HERE) << "Error: can extend the realization of a process only if defined on a regular grid.";
  }
  if (stepNumber == 0) throw InvalidArgumentException(HERE) << "Error: the number of future steps must be positive.";
  /* TimeGrid associated with the possible future */
  const NumericalScalar timeStep(timeGrid.getStep());
  const RegularGrid futurTimeGrid(timeGrid.getEnd(), timeStep, stepNumber);
  return TimeSeries(futurTimeGrid, function_(p_antecedent_->getFuture(stepNumber)).getValues());
}

/* Get the random vector corresponding to the i-th marginal component */
CompositeProcess::Implementation CompositeProcess::getMarginal(const UnsignedInteger i) const
{
  return new CompositeProcess(function_.getMarginal(i), Pointer<ProcessImplementation>(p_antecedent_->getMarginal(i)));
}

/* Get the marginal random vector corresponding to indices components */
CompositeProcess::Implementation CompositeProcess::getMarginal(const Indices & indices) const
{
  return new CompositeProcess(function_.getMarginal(indices), Pointer<ProcessImplementation>(p_antecedent_->getMarginal(indices)));
}

/* Method save() stores the object through the StorageManager */
void CompositeProcess::save(Advocate & adv) const
{
  ProcessImplementation::save(adv);
  adv.saveAttribute( "function_", function_ );
  adv.saveAttribute( "antecedent_", *p_antecedent_ );
}

/* Method load() reloads the object from the StorageManager */
void CompositeProcess::load(Advocate & adv)
{
  ProcessImplementation::load(adv);
  adv.loadAttribute( "function_", function_ );
  TypedInterfaceObject<ProcessImplementation> antecedent;
  adv.loadAttribute( "antecedent_", antecedent );
  p_antecedent_ = antecedent.getImplementation();
}

END_NAMESPACE_OPENTURNS
