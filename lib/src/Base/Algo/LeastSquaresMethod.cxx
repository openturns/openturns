//                                               -*- C++ -*-
/**
 *  @brief LeastSquares solving algorithm interface
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/LeastSquaresMethod.hxx"
#include "openturns/LeastSquaresMethodImplementation.hxx"
#include "openturns/SVDMethod.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(LeastSquaresMethod);

/* Default constructor */
LeastSquaresMethod::LeastSquaresMethod()
  : TypedInterfaceObject<LeastSquaresMethodImplementation>()
{
  // Nothing to do
}

/* Parameters constructor */
LeastSquaresMethod::LeastSquaresMethod (const DesignProxy & proxy,
                                        const NumericalSample & outputSample,
                                        const NumericalPoint & weight,
                                        const Indices & indices)
  : TypedInterfaceObject<LeastSquaresMethodImplementation>(new SVDMethod(proxy, outputSample, weight, indices))
{
  // Nothing to do
}

/* Parameters constructor */
LeastSquaresMethod::LeastSquaresMethod (const DesignProxy & proxy,
                                        const NumericalSample & outputSample,
                                        const Indices & indices)
  : TypedInterfaceObject<LeastSquaresMethodImplementation>(new SVDMethod(proxy, outputSample, indices))
{
  // Nothing to do
}

/* Constructor from implementation */
LeastSquaresMethod::LeastSquaresMethod(const Implementation & p_implementation)
  : TypedInterfaceObject<LeastSquaresMethodImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation */
LeastSquaresMethod::LeastSquaresMethod(const LeastSquaresMethodImplementation & implementation)
  : TypedInterfaceObject<LeastSquaresMethodImplementation>(implementation.clone())
{
  // Nothing to do
}

/* String converter */
String LeastSquaresMethod::__repr__() const
{
  return getImplementation()->__repr__();
}

String LeastSquaresMethod::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

NumericalPoint LeastSquaresMethod::solve(const NumericalPoint & rhs)
{
  return getImplementation()->solve(rhs);
}

NumericalPoint LeastSquaresMethod::solveNormal(const NumericalPoint & rhs)
{
  return getImplementation()->solveNormal(rhs);
}

NumericalPoint LeastSquaresMethod::getHDiag() const
{
  return getImplementation()->getHDiag();
}

NumericalScalar LeastSquaresMethod::getGramInverseTrace() const
{
  return getImplementation()->getGramInverseTrace();
}

void LeastSquaresMethod::update(const Indices& addedIndices,
                                const Indices& conservedIndices,
                                const Indices& removedIndices,
                                const Bool row)
{
  getImplementation()->update(addedIndices, conservedIndices, removedIndices, row);
}

Basis LeastSquaresMethod::buildCurrentBasis() const
{
  return getImplementation()->buildCurrentBasis();
}

Basis LeastSquaresMethod::getBasis() const
{
  return getImplementation()->getBasis();
}

Indices LeastSquaresMethod::getCurrentIndices() const
{
  return getImplementation()->getCurrentIndices();
}

Indices LeastSquaresMethod::getInitialIndices() const
{
  return getImplementation()->getInitialIndices();
}

NumericalSample LeastSquaresMethod::getInputSample() const
{
  return getImplementation()->getInputSample();
}

NumericalSample LeastSquaresMethod::getOutputSample() const
{
  return getImplementation()->getOutputSample();
}

NumericalPoint LeastSquaresMethod::getWeight() const
{
  return getImplementation()->getWeight();
}

Matrix LeastSquaresMethod::computeWeightedDesign(bool whole) const
{
  return getImplementation()->computeWeightedDesign(whole);
}


END_NAMESPACE_OPENTURNS
