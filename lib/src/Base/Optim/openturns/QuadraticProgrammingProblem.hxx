//                                               -*- C++ -*-
/**
 *  @brief QuadraticProgrammingProblem allows one to describe an optimization problem
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
#ifndef OPENTURNS_QUADRATICPROGRAMMINGPROBLEM_HXX
#define OPENTURNS_QUADRATICPROGRAMMINGPROBLEM_HXX

#include "openturns/OptimizationProblemImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class QuadraticProgrammingProblem
 * QuadraticProgrammingProblem allows one to describe a convex quadratic optimization problem
 */

class OT_API QuadraticProgrammingProblem
  : public OptimizationProblemImplementation
{

  CLASSNAME

public:

  /** Default constructor */
  QuadraticProgrammingProblem();

  /** Constructor with parameters */
  QuadraticProgrammingProblem(const SymmetricMatrix & hessian,
                              const Point & linearCost,
                              const Interval & bounds,
                              const Scalar constant = 0.0);

  /** Constructor with parameters */
  QuadraticProgrammingProblem(const SymmetricMatrix & hessian,
                              const Point & linearCost,
                              const Interval & bounds,
                              const Matrix & constraintCoefficients,
                              const Interval & constraintBounds,
                              const Scalar constant = 0.0);

  /** Virtual constructor */
  QuadraticProgrammingProblem * clone() const override;

  /** Quadratic flag accessor */
  Bool isQuadratic() const override;

  /** Objective accessor. The quadratic coefficients are the reference, so a
   * generic objective replacement is rejected: use setHessian, setLinearCost
   * and setConstant instead. */
  void setObjective(const Function & objective) override;

  /** Objective accessors */
  void setHessian(const SymmetricMatrix & hessian);
  SymmetricMatrix getHessian() const override;
  void setLinearCost(const Point & linearCost);
  Point getLinearCost() const override;
  void setConstant(const Scalar constant);
  Scalar getConstant() const override;

  /** Constraint accessor */
  void setLinearConstraint(const Matrix & constraintCoefficients, const Interval & constraintBounds);
  Matrix getLinearConstraintCoefficients() const override;
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

  /** Build a least-squares problem min ||A.x-b||^2 subject to bounds */
  static QuadraticProgrammingProblem FromLeastSquares(const Matrix & matrix,
      const Point & rhs,
      const Interval & bounds);

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** Rebuild the objective function from the quadratic data */
  void updateObjective();

  // The objective 1/2 x^T Q x + c^T x + d
  SymmetricMatrix hessian_;
  Point linearCost_;
  Scalar constant_ = 0.0;

  // The constraints L<=Ax<=U
  Matrix constraintCoefficients_;
  Interval constraintBounds_;

}; /* class QuadraticProgrammingProblem */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_QUADRATICPROGRAMMINGPROBLEM_HXX */
