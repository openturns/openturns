//                                               -*- C++ -*-
/**
 *  @brief LinearProblem allows one to describe an optimization problem
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
#ifndef OPENTURNS_LINEARPROBLEM_HXX
#define OPENTURNS_LINEARPROBLEM_HXX

#include "openturns/OptimizationProblemImplementation.hxx"
#include "openturns/OptimizationProblem.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LinearProblem
 * LinearProblem allows one to describe an optimization problem
 */

class OT_API LinearProblem
  : public OptimizationProblemImplementation
{

  CLASSNAME

public:

  /** Default constructor */
  LinearProblem();

  /** Constructor with parameters */
  LinearProblem(const Point & cost, const Interval & bounds,
                const Matrix & constraintCoefficients, const Interval & constraintBounds);

  /** Virtual constructor */
  LinearProblem * clone() const override;

  /** Linear flag accessor */
  Bool isLinear() const override;
  
  /** Objective accessor */
  void setLinearCost(const Point & cost);
  Point getLinearCost() const override;

  /** Constraint accessor */
  void setLinearConstraint(const Matrix & constraintCoefficients, const Interval & LU);
  Matrix getLinearConstraintCoefficients() const override;
  Interval getLinearConstraintBounds() const override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** From problem linearization */
  static LinearProblem Linearize(const OptimizationProblem & problem,
                                 const Point & location);

private:

  // The objective c^Tx
  Point cost_;

  // The constraints L<=Ax<=U
  Matrix constraintCoefficients_;
  Interval constraintBounds_;

}; /* class LinearProblem */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LINEARPROBLEM_HXX */
