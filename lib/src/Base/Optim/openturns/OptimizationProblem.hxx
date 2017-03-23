//                                               -*- C++ -*-
/**
 *  @brief OptimizationProblem allows to describe an optimization problem
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
#ifndef OPENTURNS_OPTIMIZATIONPROBLEM_HXX
#define OPENTURNS_OPTIMIZATIONPROBLEM_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/NumericalMathFunction.hxx"
#include "openturns/OptimizationProblemImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class OptimizationProblem
 * OptimizationProblem allows to define an optimization problem, including:
 * + multiple objective functions
 * + multiple equality, inequality and bounds constraints
 * + continuous or discrete (lattice) search spaces
 */

class OT_API OptimizationProblem
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
  explicit OptimizationProblem(const NumericalMathFunction & objective);

  /** Constructor with parameters */
  OptimizationProblem(const NumericalMathFunction & objective,
                      const NumericalMathFunction & equalityConstraint,
                      const NumericalMathFunction & inequalityConstraint,
                      const Interval & bounds);

  /** Constructor with parameters */
  OptimizationProblem(const NumericalMathFunction & levelFunction,
                      NumericalScalar levelValue);

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

  /** Level function accessor */
  NumericalMathFunction getLevelFunction() const;
  void setLevelFunction(const NumericalMathFunction & levelFunction);
  Bool hasLevelFunction() const;

  /** Level value accessor */
  NumericalScalar getLevelValue() const;
  void setLevelValue(NumericalScalar levelValue);

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /** Minimization accessor */
  void setMinimization(Bool minimization);
  Bool isMinimization() const;

  /** isValid method */
  Bool isValid() const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

} ; /* class OptimizationProblem */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_OPTIMIZATIONPROBLEM_HXX */
