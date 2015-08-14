//                                               -*- C++ -*-
/**
 *  @brief OptimizationProblem allows to describe an optimization problem
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
#ifndef OPENTURNS_OPTIMIZATIONPROBLEM_HXX
#define OPENTURNS_OPTIMIZATIONPROBLEM_HXX

#include "OTprivate.hxx"
#include "TypedInterfaceObject.hxx"
#include "NumericalMathFunction.hxx"
#include "OptimizationProblemImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class OptimizationProblem
 * OptimizationProblem allows to define an optimization problem, including:
 * + multiple objective functions
 * + multiple equality, inequality and bounds constraints
 * + continuous or discrete (lattice) search spaces
 */

class OptimizationProblem
  : public TypedInterfaceObject<OptimizationProblemImplementation>
{

  CLASSNAME;
public:

  typedef Pointer<OptimizationProblemImplementation>   Implementation;

  /** Default constructor */
  OptimizationProblem();

  /** Constructor from an implementation */
  OptimizationProblem(const OptimizationProblemImplementation & implementation);

  /** Constructor from a Pointer to an implementation */
  OptimizationProblem(const Implementation & p_implementation);

  /** Constructor with parameters */
  OptimizationProblem(const NumericalMathFunction & objective,
                      const NumericalMathFunction & equalityConstraint,
                      const NumericalMathFunction & inequalityConstraint,
                      const Interval & bounds);

  /** Objective functions accessor */
  NumericalMathFunction getObjective() const;
  void setObjective(const NumericalMathFunction & objective);
  Bool hasMultipleObjective() const;

  /** Equality constraint function accessor */
  NumericalMathFunction getEqualityConstraint() const;
  void setEqualityConstraint(const NumericalMathFunction & equalityConstraint);
  Bool hasEqualityConstraint() const;

  /** Inequality constraint function accessor */
  NumericalMathFunction getInequalityConstraint() const;
  void setInequalityConstraint(const NumericalMathFunction & inequalityConstraint);
  Bool hasInequalityConstraint() const;

  /** Bounds accessor */
  Interval getBounds() const;
  void setBounds(const Interval & bounds);
  Bool hasBounds() const;

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

} ; /* class OptimizationProblem */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_OPTIMIZATIONPROBLEM_HXX */
