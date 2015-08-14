//                                               -*- C++ -*-
/**
 *  @brief Cross-validation algorithm implementation
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

#include "PersistentObjectFactory.hxx"
#include "ODESolverImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ODESolverImplementation);

static Factory<ODESolverImplementation> RegisteredFactory("ODESolverImplementation");

/* Default constructor */
ODESolverImplementation::ODESolverImplementation()
  : PersistentObject()
  , transitionFunction_()
{
  // Nothing to do
}

/* Parameter constructor */
ODESolverImplementation::ODESolverImplementation(const DynamicalFunction & transitionFunction)
  : PersistentObject()
  , transitionFunction_(transitionFunction)
{
  // Nothing to do
}

/* Virtual constructor */
ODESolverImplementation * ODESolverImplementation::clone() const
{
  return new ODESolverImplementation(*this);
}

/* Solve the ODE */
NumericalSample ODESolverImplementation::solve(const NumericalPoint & initialState,
    const NumericalPoint & timeGrid) const
{
  throw NotYetImplementedException(HERE) << "In ODESolverImplementation::solve(const NumericalPoint & initialState, const NumericalPoint & timeGrid) const";
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
void ODESolverImplementation::setTransitionFunction(const DynamicalFunction & transitionFunction)
{
  transitionFunction_ = transitionFunction;
}

DynamicalFunction ODESolverImplementation::getTransitionFunction() const
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
