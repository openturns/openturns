//                                               -*- C++ -*-
/**
 *  @brief P1InterpolatedProcess implements a process whose realizations are obtained by P1 Lagrange interpolation of another process on a finer mesh.
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/P1InterpolatedProcess.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/P1LagrangeInterpolation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(P1InterpolatedProcess)

static const Factory<P1InterpolatedProcess> Factory_P1InterpolatedProcess;

P1InterpolatedProcess::P1InterpolatedProcess()
  : ProcessImplementation()
  , compoundProcess_()
  , p1Interpolation_()
{
  // Nothing to do
}

/* Standard constructor */
P1InterpolatedProcess::P1InterpolatedProcess(const Process & compoundProcess,
    const Mesh & mesh)
  : ProcessImplementation()
  , compoundProcess_(compoundProcess)
{
  const UnsignedInteger outputDim = compoundProcess_.getOutputDimension();
  const P1LagrangeInterpolation interpolation(compoundProcess_.getMesh(), mesh, outputDim);
  p1Interpolation_ = FieldFunction(interpolation);
  setMesh(mesh);
  setOutputDimension(outputDim);
  setDescription(compoundProcess_.getDescription());
}

/* Virtual constructor */
P1InterpolatedProcess * P1InterpolatedProcess::clone() const
{
  return new P1InterpolatedProcess(*this);
}

/* String converter */
String P1InterpolatedProcess::__repr__() const
{
  OSS oss(true);
  oss << "class=" << P1InterpolatedProcess::GetClassName()
      << " compoundProcess=" << compoundProcess_.getImplementation()->__repr__()
      << " p1Interpolation=" << p1Interpolation_.__repr__();
  return oss;
}

String P1InterpolatedProcess::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(" << compoundProcess_.__str__()
      << ", " << mesh_.__str__() << ")";
  return oss;
}

/* Realization accessor */
Field P1InterpolatedProcess::getRealization() const
{
  const Field preField(compoundProcess_.getRealization());
  Sample values(p1Interpolation_(preField.getValues()));
  values.setDescription(getDescription());
  return Field(mesh_, values);
}

/* Compound process accessor */
Process P1InterpolatedProcess::getCompoundProcess() const
{
  return compoundProcess_;
}

/* Mesh accessor */
void P1InterpolatedProcess::setMesh(const Mesh & mesh)
{
  if (mesh.getDimension() != compoundProcess_.getInputDimension())
    throw InvalidArgumentException(HERE) << "Error: trying to set a mesh with dimension "
                                          << mesh.getDimension()
                                          << " on a P1InterpolatedProcess whose compound process has input dimension "
                                          << compoundProcess_.getInputDimension();
  const UnsignedInteger outputDim = compoundProcess_.getOutputDimension();
  const P1LagrangeInterpolation interpolation(compoundProcess_.getMesh(), mesh, outputDim);
  ProcessImplementation::setMesh(mesh);
  p1Interpolation_ = FieldFunction(interpolation);
}

/* Method save() stores the object through the StorageManager */
void P1InterpolatedProcess::save(Advocate & adv) const
{
  ProcessImplementation::save(adv);
  adv.saveAttribute( "compoundProcess_", compoundProcess_ );
  adv.saveAttribute( "p1Interpolation_", p1Interpolation_ );
}

/* Method load() reloads the object from the StorageManager */
void P1InterpolatedProcess::load(Advocate & adv)
{
  ProcessImplementation::load(adv);
  adv.loadAttribute( "compoundProcess_", compoundProcess_ );
  adv.loadAttribute( "p1Interpolation_", p1Interpolation_ );
}

END_NAMESPACE_OPENTURNS
