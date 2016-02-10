//                                               -*- C++ -*-
/**
 *  @brief Cross-validation algorithm implementation
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
#ifndef OPENTURNS_ODESOLVERIMPLEMENTATION_HXX
#define OPENTURNS_ODESOLVERIMPLEMENTATION_HXX

#include "PersistentObject.hxx"
#include "DynamicalFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ODESolverImplementation
 *
 * Cross-validation algorithm implementation
 */
class OT_API ODESolverImplementation
  : public PersistentObject
{
  CLASSNAME;

public:

  /** Default constructor */
  ODESolverImplementation();

  /** Parameter constructor */
  explicit ODESolverImplementation(const DynamicalFunction & transitionFunction);

  /** Virtual constructor */
  virtual ODESolverImplementation * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Solve the ODE */
  virtual NumericalSample solve(const NumericalPoint & initialState,
                                const NumericalPoint & timeGrid) const;

  /** Transition function accessor */
  void setTransitionFunction(const DynamicalFunction & transitionFunction);
  DynamicalFunction getTransitionFunction() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:

  /** Transition function */
  DynamicalFunction transitionFunction_;

}; /* class ODESolverImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ODESOLVERIMPLEMENTATION_HXX */
