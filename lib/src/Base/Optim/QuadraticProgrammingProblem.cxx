//                                               -*- C++ -*-
/**
 *  @brief QuadraticProgrammingProblem allows one to describe an optimization problem
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

#include "openturns/QuadraticProgrammingProblem.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/QuadraticFunction.hxx"
#include "openturns/SymmetricTensor.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(QuadraticProgrammingProblem)

static const Factory<QuadraticProgrammingProblem> Factory_QuadraticProgrammingProblem;

/* Default constructor */
QuadraticProgrammingProblem::QuadraticProgrammingProblem()
  : OptimizationProblemImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
QuadraticProgrammingProblem::QuadraticProgrammingProblem(const SymmetricMatrix & hessian,
    const Point & linearCost,
    const Interval & bounds,
    const Scalar constant)
  : OptimizationProblemImplementation()
  , hessian_(hessian)
  , linearCost_(linearCost)
  , constant_(constant)
{
  if (hessian_.getDimension() != linearCost_.getDimension())
    throw InvalidDimensionException(HERE) << "Hessian dimension (" << hessian_.getDimension() << ") must match linear cost dimension (" << linearCost_.getDimension() << ")";
  updateObjective();
  setBounds(bounds);
}

/* Parameter constructor */
QuadraticProgrammingProblem::QuadraticProgrammingProblem(const SymmetricMatrix & hessian,
    const Point & linearCost,
    const Interval & bounds,
    const Matrix & constraintCoefficients,
    const Interval & constraintBounds,
    const Scalar constant)
  : OptimizationProblemImplementation()
  , hessian_(hessian)
  , linearCost_(linearCost)
  , constant_(constant)
{
  if (hessian_.getDimension() != linearCost_.getDimension())
    throw InvalidDimensionException(HERE) << "Hessian dimension (" << hessian_.getDimension() << ") must match linear cost dimension (" << linearCost_.getDimension() << ")";
  updateObjective();
  setBounds(bounds);
  setLinearConstraint(constraintCoefficients, constraintBounds);
}


/* Virtual constructor */
QuadraticProgrammingProblem * QuadraticProgrammingProblem::clone() const
{
  return new QuadraticProgrammingProblem(*this);
}


/* Quadratic flag accessor */
Bool QuadraticProgrammingProblem::isQuadratic() const
{
  return true;
}


/* Objective accessor */
void QuadraticProgrammingProblem::setHessian(const SymmetricMatrix & hessian)
{
  if (hessian.getDimension() != linearCost_.getDimension())
    throw InvalidDimensionException(HERE) << "Hessian dimension (" << hessian.getDimension() << ") must match linear cost dimension (" << linearCost_.getDimension() << ")";
  hessian_ = hessian;
  updateObjective();
}


SymmetricMatrix QuadraticProgrammingProblem::getHessian() const
{
  return hessian_;
}


void QuadraticProgrammingProblem::setLinearCost(const Point & linearCost)
{
  if (hessian_.getDimension() != linearCost.getDimension())
    throw InvalidDimensionException(HERE) << "Linear cost dimension (" << linearCost.getDimension() << ") must match hessian dimension (" << hessian_.getDimension() << ")";
  if ((constraintCoefficients_.getNbColumns() != 0) && (constraintCoefficients_.getNbColumns() != linearCost.getDimension()))
    throw InvalidDimensionException(HERE) << "Linear cost dimension (" << linearCost.getDimension() << ") must match constraint matrix column dimension (" << constraintCoefficients_.getNbColumns() << ")";
  linearCost_ = linearCost;
  updateObjective();
}


Point QuadraticProgrammingProblem::getLinearCost() const
{
  return linearCost_;
}


void QuadraticProgrammingProblem::setConstant(const Scalar constant)
{
  constant_ = constant;
  updateObjective();
}


Scalar QuadraticProgrammingProblem::getConstant() const
{
  return constant_;
}


/* Constraint accessor */
void QuadraticProgrammingProblem::setLinearConstraint(const Matrix & constraintCoefficients, const Interval & constraintBounds)
{
  if (constraintCoefficients.getNbColumns() != linearCost_.getDimension())
    throw InvalidDimensionException(HERE) << "Matrix column dimension (" << constraintCoefficients.getNbColumns() << ") must match problem dimension (" << linearCost_.getDimension() << ")";
  if (constraintCoefficients.getNbRows() != constraintBounds.getDimension())
    throw InvalidDimensionException(HERE) << "Constraint bounds dimension (" << constraintBounds.getDimension() << ") must match matrix row dimension (" << constraintCoefficients.getNbRows() << ")";
  constraintCoefficients_ = constraintCoefficients;
  constraintBounds_ = constraintBounds;
}


Matrix QuadraticProgrammingProblem::getLinearConstraintCoefficients() const
{
  return constraintCoefficients_;
}


Interval QuadraticProgrammingProblem::getLinearConstraintBounds() const
{
  return constraintBounds_;
}


/* Equality and inequality constraints, derived from the linear constraints */
Bool QuadraticProgrammingProblem::hasEqualityConstraint() const
{
  return HasLinearEqualityConstraint(constraintBounds_);
}


Function QuadraticProgrammingProblem::getEqualityConstraint() const
{
  return GetLinearEqualityConstraint(constraintCoefficients_, constraintBounds_, linearCost_.getDimension());
}


void QuadraticProgrammingProblem::setEqualityConstraint(const Function & equalityConstraint)
{
  SetLinearEqualityConstraint(constraintCoefficients_, constraintBounds_, equalityConstraint, linearCost_.getDimension());
}


Bool QuadraticProgrammingProblem::hasInequalityConstraint() const
{
  return HasLinearInequalityConstraint(constraintBounds_);
}


Function QuadraticProgrammingProblem::getInequalityConstraint() const
{
  return GetLinearInequalityConstraint(constraintCoefficients_, constraintBounds_, linearCost_.getDimension());
}


void QuadraticProgrammingProblem::setInequalityConstraint(const Function & inequalityConstraint)
{
  SetLinearInequalityConstraint(constraintCoefficients_, constraintBounds_, inequalityConstraint, linearCost_.getDimension());
}


/* Build a least-squares problem */
QuadraticProgrammingProblem QuadraticProgrammingProblem::FromLeastSquares(const Matrix & matrix,
    const Point & rhs,
    const Interval & bounds)
{
  if (matrix.getNbRows() != rhs.getDimension())
    throw InvalidDimensionException(HERE) << "Matrix row dimension (" << matrix.getNbRows() << ") must match right hand side dimension (" << rhs.getDimension() << ")";
  const UnsignedInteger dimension = matrix.getNbColumns();
  if ((bounds.getDimension() != 0) && (bounds.getDimension() != dimension))
    throw InvalidDimensionException(HERE) << "Bounds dimension (" << bounds.getDimension() << ") must match matrix column dimension (" << dimension << ")";

  // min ||A.x-b||^2 = 1/2 x^T (2 A^T A) x - 2 (A^T b)^T x + ||b||^2
  const SymmetricMatrix hessian(matrix.computeGram(true) * 2.0);
  // Use the raw MatrixImplementation interface to avoid a copy of A in the transpose product
  const Point atb(matrix.getImplementation()->genVectProd(rhs, true));
  Point linearCost(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    linearCost[i] = -2.0 * atb[i];

  QuadraticProgrammingProblem problem(hessian, linearCost, bounds);
  problem.setConstant(rhs.normSquare());
  return problem;
}


/* Objective accessor: the quadratic coefficients are the reference, so a
 * generic objective replacement is rejected instead of leaving a stale
 * Hessian, linear cost or constant for the solvers. */
void QuadraticProgrammingProblem::setObjective(const Function &)
{
  throw InvalidArgumentException(HERE) << "QuadraticProgrammingProblem does not support setObjective, use setHessian, setLinearCost and setConstant instead";
}


/* Rebuild the objective function from the quadratic data */
void QuadraticProgrammingProblem::updateObjective()
{
  const UnsignedInteger dimension = linearCost_.getDimension();
  Matrix linear(1, dimension);
  for (UnsignedInteger j = 0; j < dimension; ++j)
    linear(0, j) = linearCost_[j];
  SymmetricTensor quadratic(dimension, 1);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
      quadratic(i, j, 0) = hessian_(i, j);
  // setObjective() always resets the variable types to CONTINUOUS: restore
  // them when the dimension is unchanged.
  const Indices variablesType(getVariablesType());
  OptimizationProblemImplementation::setObjective(QuadraticFunction(Point(dimension), Point({constant_}), linear, quadratic));
  if (variablesType.getSize() == getDimension())
    setVariablesType(variablesType);
}


/* String converter */
String QuadraticProgrammingProblem::__repr__() const
{
  OSS oss;
  oss << "class=" << QuadraticProgrammingProblem::GetClassName();
  oss << " hessian=" << hessian_
      << " linearCost=" << linearCost_
      << " constant=" << constant_
      << " constraintCoefficients=" << constraintCoefficients_
      << " constraintBounds=" << constraintBounds_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void QuadraticProgrammingProblem::save(Advocate & adv) const
{
  OptimizationProblemImplementation::save(adv);
  adv.saveAttribute("hessian_", hessian_);
  adv.saveAttribute("linearCost_", linearCost_);
  adv.saveAttribute("constant_", constant_);
  adv.saveAttribute("constraintCoefficients_", constraintCoefficients_);
  adv.saveAttribute("constraintBounds_", constraintBounds_);
}

/* Method load() reloads the object from the StorageManager */
void QuadraticProgrammingProblem::load(Advocate & adv)
{
  OptimizationProblemImplementation::load(adv);
  adv.loadAttribute("hessian_", hessian_);
  adv.loadAttribute("linearCost_", linearCost_);
  adv.loadAttribute("constant_", constant_);
  adv.loadAttribute("constraintCoefficients_", constraintCoefficients_);
  adv.loadAttribute("constraintBounds_", constraintBounds_);
}

END_NAMESPACE_OPENTURNS
