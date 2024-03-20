//                                               -*- C++ -*-
/**
 *  @brief ODE solver interface
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

#include "openturns/ODESolver.hxx"
#include "openturns/ODESolverImplementation.hxx"
#include "openturns/RungeKutta.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ODESolver)

/* Default constructor */
ODESolver::ODESolver()
  : TypedInterfaceObject<ODESolverImplementation>(new ODESolverImplementation())
{
  // Nothing to do
}

/* Default constructor */
ODESolver::ODESolver(const Function & transitionFunction)
  : TypedInterfaceObject<ODESolverImplementation>(RungeKutta(transitionFunction).clone())
{
  // Nothing to do
}

/* Constructor from implementation */
ODESolver::ODESolver(const Implementation & p_implementation)
  : TypedInterfaceObject<ODESolverImplementation>( p_implementation )
{
  // Nothing to do
}

/* Constructor from implementation */
ODESolver::ODESolver(const ODESolverImplementation & implementation)
  : TypedInterfaceObject<ODESolverImplementation>( implementation.clone() )
{
  // Nothing to do
}

/* String converter */
String ODESolver::__repr__() const
{
  return getImplementation()->__repr__();
}

String ODESolver::__str__(const String & offset) const
{
  return getImplementation()->__str__( offset );
}

/* Solve ODE */
Sample ODESolver::solve(const Point & initialState,
                        const Point & timeGrid) const
{
  return getImplementation()->solve(initialState, timeGrid);
}

Sample ODESolver::solve(const Point & initialState,
                        const Mesh & timeGrid) const
{
  return getImplementation()->solve(initialState, timeGrid);
}

/* Transition function accessor */
void ODESolver::setTransitionFunction(const Function & transitionFunction)
{
  copyOnWrite();
  getImplementation()->setTransitionFunction(transitionFunction);
}

Function ODESolver::getTransitionFunction() const
{
  return getImplementation()->getTransitionFunction();
}

END_NAMESPACE_OPENTURNS
