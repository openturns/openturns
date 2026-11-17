//                                               -*- C++ -*-
/**
 *  @brief LinearProgrammingProblem allows one to describe an optimization problem
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_LINEARPROGRAMMINGPROBLEM_HXX
#define OPENTURNS_LINEARPROGRAMMINGPROBLEM_HXX

#include "openturns/OptimizationProblemImplementation.hxx"
#include "openturns/OptimizationProblem.hxx"
#include "openturns/SparseMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LinearProgrammingProblem
 * LinearProgrammingProblem allows one to describe an optimization problem
 */

class OT_API LinearProgrammingProblem
  : public OptimizationProblemImplementation
{

  CLASSNAME

public:

  /** Default constructor */
  LinearProgrammingProblem();

  /** Constructor with parameters */
  LinearProgrammingProblem(const Point & cost, const Interval & bounds,
                           const Matrix & constraintCoefficients, const Interval & constraintBounds,
                           const Scalar constant = 0.0);

  /** Constructor with sparse constraint coefficients */
  LinearProgrammingProblem(const Point & cost, const Interval & bounds,
                           const SparseMatrix & constraintCoefficients, const Interval & constraintBounds,
                           const Scalar constant = 0.0);

  /** Virtual constructor */
  LinearProgrammingProblem * clone() const override;

  /** Linear flag accessor */
  Bool isLinear() const override;

  /** Objective accessor */
  void setLinearCost(const Point & cost);
  Point getLinearCost() const override;

  /** Constant accessor */
  void setConstant(const Scalar constant);
  Scalar getConstant() const override;

  /** Constraint accessor */
  void setLinearConstraint(const Matrix & constraintCoefficients, const Interval & LU);
  void setLinearConstraint(const SparseMatrix & constraintCoefficients, const Interval & LU);
  Matrix getLinearConstraintCoefficients() const override;
#ifndef SWIG
  SparseMatrix getSparseLinearConstraintCoefficients() const override;
#endif
  Interval getLinearConstraintBounds() const override;

  /** Equality and inequality constraints derived from the linear constraints.
      A row whose lower and upper bounds are equal is an equality constraint,
      the other finite rows are inequality constraints. */
  Function getEqualityConstraint() const override;
  void setEqualityConstraint(const Function & equalityConstraint) override;
  Bool hasEqualityConstraint() const override;
  Function getInequalityConstraint() const override;
  void setInequalityConstraint(const Function & inequalityConstraint) override;
  Bool hasInequalityConstraint() const override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** From problem linearization */
  static LinearProgrammingProblem Linearize(const OptimizationProblem & problem,
      const Point & location);

private:

  /** Rebuild the objective function from the linear data */
  void updateObjective();

  // The objective c^Tx + d
  Point cost_;
  Scalar constant_ = 0.0;

  // The constraints L<=Ax<=U, stored in compressed sparse column form
  SparseMatrix constraintCoefficients_;
  Interval constraintBounds_;

}; /* class LinearProgrammingProblem */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LINEARPROGRAMMINGPROBLEM_HXX */
