//                                               -*- C++ -*-
/**
 *  @brief This class implements the fourth order fixed-step Runge-Kutta ODE integrator
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

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/RungeKutta.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(RungeKutta);

static const Factory<RungeKutta> Factory_RungeKutta;

/* Default constructor */
RungeKutta::RungeKutta()
  : ODESolverImplementation()
{
  // Nothing to do
}

/* Default constructor */
RungeKutta::RungeKutta(const FieldFunction & transitionFunction)
  : ODESolverImplementation(transitionFunction)
{
  // Nothing to do
}

/* Virtual constructor */
RungeKutta * RungeKutta::clone() const
{
  return new RungeKutta( *this );
}

/* String converter */
String RungeKutta::__repr__() const
{
  return OSS() << "class=" << GetClassName();
}

/* Perform cross-validation */
Sample RungeKutta::solve(const NumericalPoint & initialState,
                                  const NumericalPoint & timeGrid) const
{
  if (initialState.getDimension() != transitionFunction_.getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the initial state has a dimension=" << initialState.getDimension() << ", expected dimension=" << transitionFunction_.getInputDimension();
  // Quick return if the time grid is empty
  const UnsignedInteger steps = timeGrid.getSize();
  Sample result(steps, transitionFunction_.getOutputDimension());
  if (steps == 0) return result;
  NumericalScalar t = timeGrid[0];
  NumericalPoint state(initialState);
  result[0] = state;
  for (UnsignedInteger i = 1; i < steps; ++i)
  {
    const NumericalScalar newT = timeGrid[i];
    const NumericalScalar timeStep = newT - t;
    const NumericalPoint phi(computeStep(t, state, timeStep));
    state += timeStep * phi;
    result[i] = state;
    t = newT;
  }
  return result;
}

/* Perform one step of the RungeKutta method */
NumericalPoint RungeKutta::computeStep(const NumericalScalar t,
                                       const NumericalPoint & state,
                                       const NumericalScalar h) const
{
  const NumericalPoint k1(transitionFunction_(t, state));
  const NumericalPoint k2(transitionFunction_(t + 0.5 * h, state + k1 * (0.5 * h)));
  const NumericalPoint k3(transitionFunction_(t + 0.5 * h, state + k2 * (0.5 * h)));
  const NumericalPoint k4(transitionFunction_(t + h, state + k3 * h));
  return (k1 + k2 * 2.0 + k3 * 2.0 + k4) * (1.0 / 6.0);
}

/* Method save() stores the object through the StorageManager */
void RungeKutta::save(Advocate & adv) const
{
  ODESolverImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void RungeKutta::load(Advocate & adv)
{
  ODESolverImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
