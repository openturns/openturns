//                                               -*- C++ -*-
/**
 *  @brief This class implements the fourth order fixed-step Runge-Kutta ODE integrator
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
#include "openturns/RungeKutta.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(RungeKutta)

static const Factory<RungeKutta> Factory_RungeKutta;

/* Default constructor */
RungeKutta::RungeKutta()
  : ODESolverImplementation()
{
  // Nothing to do
}

/* Default constructor */
RungeKutta::RungeKutta(const Function & transitionFunction)
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
Sample RungeKutta::solve(const Point & initialState,
                         const Point & timeGrid) const
{
  if (initialState.getDimension() != transitionFunction_.getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the initial state has a dimension=" << initialState.getDimension() << ", expected dimension=" << transitionFunction_.getInputDimension();
  if (!timeGrid.isMonotonic()) throw InvalidArgumentException(HERE) << "Error: expected a monotonic time grid.";
  // Quick return if the time grid is empty
  const UnsignedInteger steps = timeGrid.getSize();
  Sample result(steps, transitionFunction_.getOutputDimension());
  if (steps == 0) return result;
  Scalar t = timeGrid[0];
  Point state(initialState);
  result[0] = state;
  // Use a pointer to avoid many copies through setParameter
  Pointer<EvaluationImplementation> transitionFunction = transitionFunction_.getEvaluation().getImplementation()->clone();
  for (UnsignedInteger i = 1; i < steps; ++i)
  {
    const Scalar newT = timeGrid[i];
    const Scalar timeStep = newT - t;
    const Point phi(computeStep(transitionFunction, t, state, timeStep));
    for (UnsignedInteger j = 0; j < phi.getDimension(); ++ j)
      if (!SpecFunc::IsNormal(phi[j]))
        throw InvalidArgumentException(HERE) << "RungeKutta: Step is " << phi.__str__() << " at " << state.__str__() << " with t=" << t << " h=" << timeStep;
    state += timeStep * phi;
    result[i] = state;
    t = newT;
  }
  return result;
}

/* Perform one step of the RungeKutta method */
Point RungeKutta::computeStep(Pointer<EvaluationImplementation> & transitionFunction,
                              const Scalar t,
                              const Point & state,
                              const Scalar h) const
{
  Point parameter(1, t);
  transitionFunction->setParameter(parameter);
  const Point k1(transitionFunction->operator()(state));
  parameter[0] = t + 0.5 * h;
  transitionFunction->setParameter(parameter);
  const Point k2(transitionFunction->operator()(state + k1 * (0.5 * h)));
  const Point k3(transitionFunction->operator()(state + k2 * (0.5 * h)));
  parameter[0] = t + h;
  transitionFunction->setParameter(parameter);
  const Point k4(transitionFunction->operator()(state + k3 * h));
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
