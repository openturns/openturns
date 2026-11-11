//                                               -*- C++ -*-
/**
 *  @brief QuadraticProblem allows one to describe an optimization problem
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
#ifndef OPENTURNS_QUADRATICPROBLEM_HXX
#define OPENTURNS_QUADRATICPROBLEM_HXX

#include "openturns/OptimizationProblemImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class QuadraticProblem
 * QuadraticProblem allows one to describe a convex quadratic optimization problem
 */

class OT_API QuadraticProblem
  : public OptimizationProblemImplementation
{

  CLASSNAME

public:

  /** Default constructor */
  QuadraticProblem();

  /** Constructor with parameters */
  QuadraticProblem(const SymmetricMatrix & hessian,
                   const Point & linearCost,
                   const Interval & bounds);

  /** Constructor with parameters */
  QuadraticProblem(const SymmetricMatrix & hessian,
                   const Point & linearCost,
                   const Interval & bounds,
                   const Matrix & constraintCoefficients,
                   const Interval & constraintBounds);

  /** Virtual constructor */
  QuadraticProblem * clone() const override;

  /** Quadratic flag accessor */
  Bool isQuadratic() const override;

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

  /** Build a least-squares problem min ||A.x-b||^2 subject to bounds */
  static QuadraticProblem FromLeastSquares(const Matrix & matrix,
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

}; /* class QuadraticProblem */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_QUADRATICPROBLEM_HXX */
