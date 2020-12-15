//                                               -*- C++ -*-
/**
 *  @brief Karhunen-Loeve decomposition and projection
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/KarhunenLoeveResult.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(KarhunenLoeveResult)


/* Default constructor */
KarhunenLoeveResult::KarhunenLoeveResult()
  : TypedInterfaceObject<KarhunenLoeveResultImplementation>( new KarhunenLoeveResultImplementation() )
{
  // Nothing to do
}

/* Constructor with parameters */
KarhunenLoeveResult::KarhunenLoeveResult(const CovarianceModel & covariance,
    const Scalar threshold,
    const Point & eigenvalues,
    const FunctionCollection & modes,
    const ProcessSample & modesAsProcessSample,
    const Matrix & projection)
  : TypedInterfaceObject<KarhunenLoeveResultImplementation>( new KarhunenLoeveResultImplementation(covariance, threshold, eigenvalues, modes, modesAsProcessSample, projection) )
{
  // Nothing to do
}

/* Constructor from implementation */
KarhunenLoeveResult::KarhunenLoeveResult(const KarhunenLoeveResultImplementation & implementation)
  : TypedInterfaceObject<KarhunenLoeveResultImplementation>( implementation.clone() )
{
  // Nothing to do
}

/* Constructor from implementation */
KarhunenLoeveResult::KarhunenLoeveResult(const Implementation & p_implementation)
  : TypedInterfaceObject<KarhunenLoeveResultImplementation>( p_implementation )
{
  // Nothing to do
}

/* Threshold accessors */
Scalar KarhunenLoeveResult::getThreshold() const
{
  return getImplementation()->getThreshold();
}

/* Covariance model accessor */
CovarianceModel KarhunenLoeveResult::getCovarianceModel() const
{
  return getImplementation()->getCovarianceModel();
}

/* Eigenvalues accessor */
Point KarhunenLoeveResult::getEigenvalues() const
{
  return getImplementation()->getEigenvalues();
}

Point KarhunenLoeveResult::getEigenValues() const
{
  return getImplementation()->getEigenValues();
}

/* Draw eigen values */
Graph KarhunenLoeveResult::drawEigenvalues() const
{
  return getImplementation()->drawEigenvalues();
}

Graph KarhunenLoeveResult::drawCumulatedEigenvaluesRemainder() const
{
  return getImplementation()->drawCumulatedEigenvaluesRemainder();
}

/* Modes accessors */
KarhunenLoeveResult::FunctionCollection KarhunenLoeveResult::getModes() const
{
  return getImplementation()->getModes();
}

ProcessSample KarhunenLoeveResult::getModesAsProcessSample() const
{
  return getImplementation()->getModesAsProcessSample();
}

/* Scaled modes accessors */
KarhunenLoeveResult::FunctionCollection KarhunenLoeveResult::getScaledModes() const
{
  return getImplementation()->getScaledModes();
}

ProcessSample KarhunenLoeveResult::getScaledModesAsProcessSample() const
{
  return getImplementation()->getScaledModesAsProcessSample();
}

/* Projection matrix accessor */
Matrix KarhunenLoeveResult::getProjectionMatrix() const
{
  return getImplementation()->getProjectionMatrix();
}

/* Mesh accessor */
Mesh KarhunenLoeveResult::getMesh() const
{
  return getImplementation()->getMesh();
}

/* Projection method */
Point KarhunenLoeveResult::project(const Function & function) const
{
  return getImplementation()->project(function);
}

Point KarhunenLoeveResult::project(const Sample & values) const
{
  return getImplementation()->project(values);
}

Sample KarhunenLoeveResult::project(const FunctionCollection & functionCollection) const
{
  return getImplementation()->project(functionCollection);
}

Sample KarhunenLoeveResult::project(const ProcessSample & sample) const
{
  return getImplementation()->project(sample);
}

/* Lift method */
Function KarhunenLoeveResult::lift(const Point & coefficients) const
{
  return getImplementation()->lift(coefficients);
}

Sample KarhunenLoeveResult::liftAsSample(const Point & coefficients) const
{
  return getImplementation()->liftAsSample(coefficients);
}

Field KarhunenLoeveResult::liftAsField(const Point & coefficients) const
{
  return getImplementation()->liftAsField(coefficients);
}

/* String converter */
String KarhunenLoeveResult::__repr__() const
{
  return getImplementation()->__repr__();
}

String KarhunenLoeveResult::__str__(const String & offset) const
{
  return getImplementation()->__str__( offset );
}

END_NAMESPACE_OPENTURNS
