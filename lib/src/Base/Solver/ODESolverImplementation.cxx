//                                               -*- C++ -*-
/**
 *  @brief ODE solver base class
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
#include "openturns/ODESolverImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ODESolverImplementation)

static const Factory<ODESolverImplementation> Factory_ODESolverImplementation;

/* Default constructor */
ODESolverImplementation::ODESolverImplementation()
  : PersistentObject()
  , transitionFunction_()
{
  // Nothing to do
}

/* Parameter constructor */
ODESolverImplementation::ODESolverImplementation(const Function & transitionFunction)
  : PersistentObject()
  , transitionFunction_(transitionFunction)
{
  if (transitionFunction.getParameter().getDimension() != 1)
    throw InvalidArgumentException(HERE) << "The function must have one parameter";
}

/* Virtual constructor */
ODESolverImplementation * ODESolverImplementation::clone() const
{
  return new ODESolverImplementation(*this);
}

/* Solve the ODE */
Sample ODESolverImplementation::solve(const Point &,
                                      const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In ODESolverImplementation::solve(const Point & initialState, const Point & timeGrid) const";
}

Sample ODESolverImplementation::solve(const Point & initialState,
                                      const Mesh & timeGrid) const
{
  if (timeGrid.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected a mesh of dimension=1, here dimension=" << timeGrid.getDimension();
  const Sample vertices(timeGrid.getVertices());
  return solve(initialState, vertices.getImplementation()->getData());
}

/* String converter */
String ODESolverImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << ", transition function=" << transitionFunction_;
  return oss;
}

/* Transition function accessor */
void ODESolverImplementation::setTransitionFunction(const Function & transitionFunction)
{
  transitionFunction_ = transitionFunction;
}

Function ODESolverImplementation::getTransitionFunction() const
{
  return transitionFunction_;
}

/* Method save() stores the object through the StorageManager */
void ODESolverImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "transitionFunction_", transitionFunction_ );
}

/* Method load() reloads the object from the StorageManager */
void ODESolverImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "transitionFunction_", transitionFunction_ );
}

END_NAMESPACE_OPENTURNS
