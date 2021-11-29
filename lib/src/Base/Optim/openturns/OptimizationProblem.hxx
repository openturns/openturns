//                                               -*- C++ -*-
/**
 *  @brief OptimizationProblem allows to describe an optimization problem
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
#ifndef OPENTURNS_OPTIMIZATIONPROBLEM_HXX
#define OPENTURNS_OPTIMIZATIONPROBLEM_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/Function.hxx"
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

  CLASSNAME
public:

  typedef Pointer<OptimizationProblemImplementation>   Implementation;

  /** Default constructor */
  OptimizationProblem();

  /** Constructor from an implementation */
  OptimizationProblem(const OptimizationProblemImplementation & implementation);

  /** Constructor from a Pointer to an implementation */
  OptimizationProblem(const Implementation & p_implementation);

  /** Constructor with parameters */
  explicit OptimizationProblem(const Function & objective);

  /** Constructor with parameters */
  OptimizationProblem(const Function & objective,
                      const Function & equalityConstraint,
                      const Function & inequalityConstraint,
                      const Interval & bounds);

  /** Objective functions accessor */
  Function getObjective() const;
  void setObjective(const Function & objective);
  Bool hasMultipleObjective() const;

  /** Equality constraint function accessor */
  Function getEqualityConstraint() const;
  void setEqualityConstraint(const Function & equalityConstraint);
  Bool hasEqualityConstraint() const;

  /** Inequality constraint function accessor */
  Function getInequalityConstraint() const;
  void setInequalityConstraint(const Function & inequalityConstraint);
  Bool hasInequalityConstraint() const;

  /** Bounds accessor */
  Interval getBounds() const;
  void setBounds(const Interval & bounds);
  Bool hasBounds() const;

  /** Level function accessor */
  Function getLevelFunction() const;
  void setLevelFunction(const Function & levelFunction);
  Bool hasLevelFunction() const;

  /** Level value accessor */
  Scalar getLevelValue() const;
  void setLevelValue(Scalar levelValue);

  /** Residual function accessor */
  Function getResidualFunction() const;
  void setResidualFunction(const Function & residualFunction);
  Bool hasResidualFunction() const;

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /** Minimization accessor */
  void setMinimization(Bool minimization, UnsignedInteger marginalIndex = 0);
  Bool isMinimization(UnsignedInteger marginalIndex = 0) const;

  /** Variable types accessors */
  void setVariablesType(const Indices & variableType);
  Indices getVariablesType() const;
  bool isContinuous() const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

} ; /* class OptimizationProblem */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_OPTIMIZATIONPROBLEM_HXX */
