//                                               -*- C++ -*-
/**
 *  @brief LinearProblem allows one to describe an optimization problem
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cstdlib>

#include "openturns/LinearProblem.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/QuadraticFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LinearProblem)

static const Factory<LinearProblem> Factory_LinearProblem;

/* Default constructor */
LinearProblem::LinearProblem()
  : OptimizationProblemImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
LinearProblem::LinearProblem(const Point & cost, const Interval & bounds,
                const Matrix & constraintCoefficients, const Interval & constraintBounds)
  : OptimizationProblemImplementation()
{
  if (bounds.getDimension() && bounds.getDimension() != cost.getDimension())
    throw InvalidDimensionException(HERE) << "Bounds dimension (" << bounds.getDimension() << ") must match cost dimension (" << cost.getDimension() << ")";
  setLinearCost(cost);
  setLinearConstraint(constraintCoefficients, constraintBounds);
  bounds_ = bounds;
}


/* Virtual constructor */
LinearProblem * LinearProblem::clone() const
{
  return new LinearProblem(*this);
}


/* Objective accessor */
void LinearProblem::setLinearCost(const Point & cost)
{
  cost_ = cost;
  const UnsignedInteger dimension = cost.getDimension();
  Matrix linear(1, dimension);
  for (UnsignedInteger j = 0; j < dimension; ++ j)
    linear(0, j) = cost[j];
  setObjective(LinearFunction(Point(dimension), Point({0.0}), linear));
}


Point LinearProblem::getLinearCost() const
{
  return cost_;
}


/* Constraint accessor */
void LinearProblem::setLinearConstraint(const Matrix & constraintCoefficients, const Interval & constraintBounds)
{
  if (constraintCoefficients.getNbColumns() != cost_.getDimension())
    throw InvalidDimensionException(HERE) << "Matrix column dimension (" << constraintCoefficients.getNbColumns() << ") must match problem dimension (" << cost_.getDimension() << ")";
  if (constraintCoefficients.getNbRows() != constraintBounds.getDimension())
    throw InvalidDimensionException(HERE) << "Inequality bounds dimension (" << constraintBounds.getDimension() << ") must match matrix row dimension (" << constraintCoefficients.getNbRows() << ")";
  constraintCoefficients_ = constraintCoefficients;
  constraintBounds_ = constraintBounds;
}


Matrix LinearProblem::getLinearConstraintCoefficients() const
{
  return constraintCoefficients_;
}


Interval LinearProblem::getLinearConstraintBounds() const
{
  return constraintBounds_;
}


/* From function linearization */
LinearProblem LinearProblem::Linearize(const OptimizationProblem & problem, const Point & location)
{
  if (problem.hasMultipleObjective() || problem.hasLevelFunction() || problem.hasResidualFunction())
    throw InvalidArgumentException(HERE) << "Cannot linearize a multi-objective / nearest-point / least-squares problem";

  const UnsignedInteger problemDimension = problem.getDimension();
  if (location.getDimension() != problemDimension)
    throw InvalidArgumentException(HERE) << "The location must have the same dimension as the problem";
  const Matrix grad0(problem.getObjective().gradient(location));
  Point cost(problemDimension);
  for (UnsignedInteger i = 0; i < problemDimension; ++ i)
    cost[i] = grad0(i, 0);

  Matrix A;
  Point lb;
  Point ub;
  if (problem.hasInequalityConstraint())
  {
    const Point ineq0(problem.getInequalityConstraint()(location));
    lb = -1.0 * ineq0;
    ub = Point(ineq0.getDimension(), SpecFunc::MaxScalar);
    A = problem.getInequalityConstraint().gradient(location).transpose();
  }

  if (problem.hasEqualityConstraint())
  {
    const UnsignedInteger eqDim = problem.getEqualityConstraint().getOutputDimension();
    const Point eq0(problem.getEqualityConstraint()(location));
    const UnsignedInteger startRow = A.getNbRows();
    const Matrix eqGrad0(problem.getEqualityConstraint().gradient(location).transpose());
    A.getImplementation()->resize(A.getNbRows() + eqDim, problemDimension);
    for (UnsignedInteger j = 0; j < problemDimension; ++ j)
      for (UnsignedInteger i = 0; i < eqDim; ++ i)
        A(startRow + i, j) = eqGrad0(i, j);
    lb.add(-1.0 * eq0);
    ub.add(- 1.0 * eq0);
  }
  const Interval LU(lb, ub);
  LinearProblem linearProblem(cost, problem.getBounds(), A, LU);
  linearProblem.setVariablesType(problem.getVariablesType());
  return linearProblem;
}


/* Linear flag accessor */
Bool LinearProblem::isLinear() const
{
  return true;
}


/* String converter */
String LinearProblem::__repr__() const
{
  OSS oss;
  oss << "class=" << LinearProblem::GetClassName();
  oss << " cost=" << cost_
      << " constraintCoefficients=" << constraintCoefficients_
      << " constraintBounds=" << constraintBounds_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void LinearProblem::save(Advocate & adv) const
{
  OptimizationProblemImplementation::save(adv);
  adv.saveAttribute( "cost_", cost_ );
  adv.saveAttribute( "constraintCoefficients_", constraintCoefficients_ );
  adv.saveAttribute( "constraintBounds_", constraintBounds_ );
}

/* Method load() reloads the object from the StorageManager */
void LinearProblem::load(Advocate & adv)
{
  OptimizationProblemImplementation::load(adv);
  adv.loadAttribute( "cost_", cost_ );
  adv.loadAttribute( "constraintCoefficients_", constraintCoefficients_ );
  adv.loadAttribute( "constraintBounds_", constraintBounds_ );
}

END_NAMESPACE_OPENTURNS
