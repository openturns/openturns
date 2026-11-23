//                                               -*- C++ -*-
/**
 *  @brief LinearProgrammingProblem allows one to describe an optimization problem
 *
 *  Copyright 2005-2027 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/LinearProgrammingProblem.hxx"
#include <algorithm>
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/QuadraticFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LinearProgrammingProblem)

static const Factory<LinearProgrammingProblem> Factory_LinearProgrammingProblem;

/* Default constructor */
LinearProgrammingProblem::LinearProgrammingProblem()
  : OptimizationProblemImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
LinearProgrammingProblem::LinearProgrammingProblem(const Point & cost, const Interval & bounds,
    const Matrix & constraintCoefficients, const Interval & constraintBounds,
    const Scalar constant)
  : LinearProgrammingProblem(cost, bounds, SparseMatrix::FromDenseMatrix(constraintCoefficients), constraintBounds, constant)
{
  // Nothing to do
}

/* Parameter constructor with sparse constraint coefficients */
LinearProgrammingProblem::LinearProgrammingProblem(const Point & cost, const Interval & bounds,
    const SparseMatrix & constraintCoefficients, const Interval & constraintBounds,
    const Scalar constant)
  : OptimizationProblemImplementation()
{
  if (bounds.getDimension() && bounds.getDimension() != cost.getDimension())
    throw InvalidDimensionException(HERE) << "Bounds dimension (" << bounds.getDimension() << ") must match cost dimension (" << cost.getDimension() << ")";
  setLinearCost(cost);
  setLinearConstraint(constraintCoefficients, constraintBounds);
  bounds_ = bounds;
  setConstant(constant);
}


/* Virtual constructor */
LinearProgrammingProblem * LinearProgrammingProblem::clone() const
{
  return new LinearProgrammingProblem(*this);
}


/* Objective accessor */
void LinearProgrammingProblem::setLinearCost(const Point & cost)
{
  if ((constraintCoefficients_.getNbColumns() != 0) && (constraintCoefficients_.getNbColumns() != cost.getDimension()))
    throw InvalidDimensionException(HERE) << "Linear cost dimension (" << cost.getDimension() << ") must match constraint matrix column dimension (" << constraintCoefficients_.getNbColumns() << ")";
  cost_ = cost;
  updateObjective();
}


Point LinearProgrammingProblem::getLinearCost() const
{
  return cost_;
}


void LinearProgrammingProblem::setConstant(const Scalar constant)
{
  constant_ = constant;
  updateObjective();
}


Scalar LinearProgrammingProblem::getConstant() const
{
  return constant_;
}


void LinearProgrammingProblem::updateObjective()
{
  const UnsignedInteger dimension = cost_.getDimension();
  Matrix linear(1, dimension);
  for (UnsignedInteger j = 0; j < dimension; ++ j)
    linear(0, j) = cost_[j];
  setObjective(LinearFunction(Point(dimension), Point({constant_}), linear));
}


/* Constraint accessor */
void LinearProgrammingProblem::setLinearConstraint(const Matrix & constraintCoefficients, const Interval & constraintBounds)
{
  setLinearConstraint(SparseMatrix::FromDenseMatrix(constraintCoefficients), constraintBounds);
}


void LinearProgrammingProblem::setLinearConstraint(const SparseMatrix & constraintCoefficients, const Interval & constraintBounds)
{
  if (constraintCoefficients.getNbColumns() != cost_.getDimension())
    throw InvalidDimensionException(HERE) << "Matrix column dimension (" << constraintCoefficients.getNbColumns() << ") must match problem dimension (" << cost_.getDimension() << ")";
  if (constraintCoefficients.getNbRows() != constraintBounds.getDimension())
    throw InvalidDimensionException(HERE) << "Inequality bounds dimension (" << constraintBounds.getDimension() << ") must match matrix row dimension (" << constraintCoefficients.getNbRows() << ")";
  constraintCoefficients_ = constraintCoefficients;
  constraintBounds_ = constraintBounds;
}


Matrix LinearProgrammingProblem::getLinearConstraintCoefficients() const
{
  return constraintCoefficients_.asDenseMatrix();
}


SparseMatrix LinearProgrammingProblem::getSparseLinearConstraintCoefficients() const
{
  return constraintCoefficients_;
}


Interval LinearProgrammingProblem::getLinearConstraintBounds() const
{
  return constraintBounds_;
}


/* Equality and inequality constraints, derived from the linear constraints */
Bool LinearProgrammingProblem::hasEqualityConstraint() const
{
  return HasLinearEqualityConstraint(constraintBounds_);
}


Function LinearProgrammingProblem::getEqualityConstraint() const
{
  return GetLinearEqualityConstraint(constraintCoefficients_, constraintBounds_, cost_.getDimension());
}


void LinearProgrammingProblem::setEqualityConstraint(const Function & equalityConstraint)
{
  SetLinearEqualityConstraint(constraintCoefficients_, constraintBounds_, equalityConstraint, cost_.getDimension());
}


Bool LinearProgrammingProblem::hasInequalityConstraint() const
{
  return HasLinearInequalityConstraint(constraintBounds_);
}


Function LinearProgrammingProblem::getInequalityConstraint() const
{
  return GetLinearInequalityConstraint(constraintCoefficients_, constraintBounds_, cost_.getDimension());
}


void LinearProgrammingProblem::setInequalityConstraint(const Function & inequalityConstraint)
{
  SetLinearInequalityConstraint(constraintCoefficients_, constraintBounds_, inequalityConstraint, cost_.getDimension());
}


/* From function linearization */
LinearProgrammingProblem LinearProgrammingProblem::Linearize(const OptimizationProblem & problem, const Point & location)
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
      std::copy(eqGrad0.getImplementation()->begin() + j * eqDim, eqGrad0.getImplementation()->begin() + (j + 1) * eqDim, A.getImplementation()->begin() + j * A.getNbRows() + startRow);
    lb.add(-1.0 * eq0);
    ub.add(- 1.0 * eq0);
  }
  const Interval LU(lb, ub);
  LinearProgrammingProblem linearProblem(cost, problem.getBounds(), A, LU);
  // the constant term carries the linearization offset so that the
  // linearized objective matches the original function at the location
  linearProblem.setConstant(problem.getObjective()(location)[0] - cost.dot(location));
  linearProblem.setVariablesType(problem.getVariablesType());
  return linearProblem;
}


/* Linear flag accessor */
Bool LinearProgrammingProblem::isLinear() const
{
  return true;
}


/* String converter */
String LinearProgrammingProblem::__repr__() const
{
  OSS oss;
  oss << "class=" << LinearProgrammingProblem::GetClassName();
  oss << " cost=" << cost_
      << " constant=" << constant_
      << " constraintCoefficients=" << constraintCoefficients_
      << " constraintBounds=" << constraintBounds_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void LinearProgrammingProblem::save(Advocate & adv) const
{
  OptimizationProblemImplementation::save(adv);
  adv.saveAttribute( "cost_", cost_ );
  adv.saveAttribute( "constant_", constant_ );
  adv.saveAttribute( "sparseConstraintCoefficients_", constraintCoefficients_ );
  adv.saveAttribute( "constraintBounds_", constraintBounds_ );
}

/* Method load() reloads the object from the StorageManager */
void LinearProgrammingProblem::load(Advocate & adv)
{
  OptimizationProblemImplementation::load(adv);
  adv.loadAttribute( "cost_", cost_ );
  if (adv.hasAttribute("constant_")) adv.loadAttribute( "constant_", constant_ );
  // Studies saved before the sparse representation stored the coefficients
  // as a dense matrix under the constraintCoefficients_ attribute
  if (adv.hasAttribute("sparseConstraintCoefficients_"))
    adv.loadAttribute( "sparseConstraintCoefficients_", constraintCoefficients_ );
  else
  {
    Matrix denseConstraintCoefficients;
    adv.loadAttribute( "constraintCoefficients_", denseConstraintCoefficients );
    constraintCoefficients_ = SparseMatrix::FromDenseMatrix(denseConstraintCoefficients);
  }
  adv.loadAttribute( "constraintBounds_", constraintBounds_ );
}

END_NAMESPACE_OPENTURNS
