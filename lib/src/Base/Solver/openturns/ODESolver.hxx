//                                               -*- C++ -*-
/**
 *  @brief Cross-validation algorithm interface
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
#ifndef OPENTURNS_ODESOLVER_HXX
#define OPENTURNS_ODESOLVER_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/ODESolverImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

class ODESolverImplementation;

/**
 * @class ODESolver
 *
 * Cross-validation algorithm interface
 */
class OT_API ODESolver
  : public TypedInterfaceObject<ODESolverImplementation>
{
  CLASSNAME;

public:

  typedef TypedInterfaceObject<ODESolverImplementation>::Implementation  Implementation;

  /** Default constructor */
  ODESolver();

  /** Parameter constructor */
  explicit ODESolver(const DynamicalFunction & transitionFunction);

  /** Constructor from implementation */
  ODESolver(const Implementation & p_implementation);

  /** Constructor from implementation */
  ODESolver(const ODESolverImplementation & implementation);

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Perform cross-validation */
  NumericalSample solve(const NumericalPoint & initialState,
                        const NumericalPoint & timeGrid) const;

  /** Transition function accessor */
  void setTransitionFunction(const DynamicalFunction & transitionFunction);
  DynamicalFunction getTransitionFunction() const;

}; /* class ODESolver */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ODESOLVER_HXX */
