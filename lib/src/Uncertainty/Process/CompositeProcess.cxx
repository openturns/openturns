//                                               -*- C++ -*-
/**
 *  @brief A class which implements the CompositeProcess process
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

#include "openturns/CompositeProcess.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/WhiteNoise.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/ValueFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CompositeProcess)

static const Factory<CompositeProcess> Factory_CompositeProcess;

CompositeProcess::CompositeProcess()
  : ProcessImplementation()
  , function_(ValueFunction(SymbolicFunction("x", "x^2"), WhiteNoise().getMesh()))
  , antecedent_(new WhiteNoise())
{
  // Set the mesh
  setMesh(function_.getOutputMesh());
}

/* Standard constructor */
CompositeProcess::CompositeProcess(const FieldFunction & function,
                                   const Process & antecedent)
  : ProcessImplementation()
  , function_(function)
  , antecedent_(antecedent)
{
  if (function.getInputDimension() != antecedent_.getOutputDimension())
    throw InvalidArgumentException(HERE) << "Error: trying to build a CompositeProcess from a Process and a FieldFunction with incompatible dimensions "
                                         << "here Process dimension=" << antecedent_.getOutputDimension()
                                         << " and FieldFunction input dimension=" << function.getInputDimension();
  if (function.getInputMesh().getDimension() != antecedent_.getInputDimension())
    throw InvalidArgumentException(HERE) << "Error: trying to build a CompositeProcess from a Process and a FieldFunction with incompatible mesh dimensions "
                                         << "here Process mesh dimension=" << antecedent_.getInputDimension()
                                         << " and FieldFunction mesh dimension=" << function.getInputMesh().getDimension();
  setMesh(function.getOutputMesh());
  setOutputDimension(function.getOutputDimension());
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
      << " antecedent=" << antecedent_.getImplementation()->__repr__();
  return oss;
}

String CompositeProcess::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(" << function_.__str__()
      << "(" << antecedent_.__str__() << ")";
  return oss;
}

/* Is the underlying a composite process ? */
Bool CompositeProcess::isComposite() const
{
  return true;
}

/* Antecedent accessor */
Process CompositeProcess::getAntecedent() const
{
  return antecedent_;
}

/* Function accessor */
FieldFunction CompositeProcess::getFunction() const
{
  return function_;
}

/* Realization accessor */
Field CompositeProcess::getRealization() const
{
  Sample values(function_(antecedent_.getRealization().getValues()));
  values.setDescription(getDescription());
  Field result(function_.getOutputMesh(), values);
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
  catch (const InvalidArgumentException &)
  {
    throw InternalException(HERE) << "Error: can extend the realization of a process only if defined on a regular grid.";
  }
  if (stepNumber == 0) throw InvalidArgumentException(HERE) << "Error: the number of future steps must be positive.";
  /* TimeGrid associated with the possible future */
  const Scalar timeStep = timeGrid.getStep();
  const RegularGrid futurTimeGrid(timeGrid.getEnd(), timeStep, stepNumber);
  FieldFunction function(function_);
  if (!function.isActingPointwise())
    throw NotDefinedException(HERE) << "Error: can only ask future of a process with a point-wise function";
  function.setInputMesh(futurTimeGrid);
  return TimeSeries(futurTimeGrid, function(antecedent_.getFuture(stepNumber).getValues()));
}

/* Get the marginal random vector corresponding to indices components */
Process CompositeProcess::getMarginal(const Indices & indices) const
{
  return new CompositeProcess(function_.getMarginal(indices), antecedent_.getMarginal(indices));
}

/* Method save() stores the object through the StorageManager */
void CompositeProcess::save(Advocate & adv) const
{
  ProcessImplementation::save(adv);
  adv.saveAttribute( "function_", function_ );
  adv.saveAttribute( "antecedent_", antecedent_ );
}

/* Method load() reloads the object from the StorageManager */
void CompositeProcess::load(Advocate & adv)
{
  ProcessImplementation::load(adv);
  adv.loadAttribute( "function_", function_ );
  adv.loadAttribute( "antecedent_", antecedent_ );
}

END_NAMESPACE_OPENTURNS
