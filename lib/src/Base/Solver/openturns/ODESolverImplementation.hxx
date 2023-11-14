//                                               -*- C++ -*-
/**
 *  @brief ODE solver base class
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
#ifndef OPENTURNS_ODESOLVERIMPLEMENTATION_HXX
#define OPENTURNS_ODESOLVERIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Function.hxx"
#include "openturns/Mesh.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ODESolverImplementation
 *
 * ODE solver base class
 */
class OT_API ODESolverImplementation
  : public PersistentObject
{
  CLASSNAME

public:

  /** Default constructor */
  ODESolverImplementation();

  /** Parameter constructor */
  explicit ODESolverImplementation(const Function & transitionFunction);

  /** Virtual constructor */
  ODESolverImplementation * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Solve the ODE */
  virtual Sample solve(const Point & initialState,
                       const Point & timeGrid) const;

  virtual Sample solve(const Point & initialState,
                       const Mesh & timeGrid) const;

  /** Transition function accessor */
  void setTransitionFunction(const Function & transitionFunction);
  Function getTransitionFunction() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Transition function */
  Function transitionFunction_;

}; /* class ODESolverImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ODESOLVERIMPLEMENTATION_HXX */
