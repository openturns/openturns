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

#include "openturns/QuadraticProblem.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/QuadraticFunction.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/SymmetricTensor.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(QuadraticProblem)

static const Factory<QuadraticProblem> Factory_QuadraticProblem;

namespace
{
// A constraint row with equal finite bounds encodes an equality constraint
Bool IsEqualityConstraintRow(const Interval & bounds, const UnsignedInteger i)
{
  return bounds.getFiniteLowerBound()[i]
         && bounds.getFiniteUpperBound()[i]
         && (bounds.getLowerBound()[i] == bounds.getUpperBound()[i]);
}
}

/* Default constructor */
QuadraticProblem::QuadraticProblem()
  : OptimizationProblemImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
QuadraticProblem::QuadraticProblem(const SymmetricMatrix & hessian,
                                   const Point & linearCost,
                                   const Interval & bounds)
  : OptimizationProblemImplementation()
  , hessian_(hessian)
  , linearCost_(linearCost)
{
  if (hessian_.getDimension() != linearCost_.getDimension())
    throw InvalidDimensionException(HERE) << "Hessian dimension (" << hessian_.getDimension() << ") must match linear cost dimension (" << linearCost_.getDimension() << ")";
  updateObjective();
  setBounds(bounds);
}

/* Parameter constructor */
QuadraticProblem::QuadraticProblem(const SymmetricMatrix & hessian,
                                   const Point & linearCost,
                                   const Interval & bounds,
                                   const Scalar constant)
  : QuadraticProblem(hessian, linearCost, bounds)
{
  setConstant(constant);
}

/* Parameter constructor */
QuadraticProblem::QuadraticProblem(const SymmetricMatrix & hessian,
                                   const Point & linearCost,
                                   const Interval & bounds,
                                   const Matrix & constraintCoefficients,
                                   const Interval & constraintBounds)
  : OptimizationProblemImplementation()
  , hessian_(hessian)
  , linearCost_(linearCost)
{
  if (hessian_.getDimension() != linearCost_.getDimension())
    throw InvalidDimensionException(HERE) << "Hessian dimension (" << hessian_.getDimension() << ") must match linear cost dimension (" << linearCost_.getDimension() << ")";
  updateObjective();
  setBounds(bounds);
  setLinearConstraint(constraintCoefficients, constraintBounds);
}

/* Parameter constructor */
QuadraticProblem::QuadraticProblem(const SymmetricMatrix & hessian,
                                   const Point & linearCost,
                                   const Interval & bounds,
                                   const Matrix & constraintCoefficients,
                                   const Interval & constraintBounds,
                                   const Scalar constant)
  : QuadraticProblem(hessian, linearCost, bounds, constraintCoefficients, constraintBounds)
{
  setConstant(constant);
}


/* Virtual constructor */
QuadraticProblem * QuadraticProblem::clone() const
{
  return new QuadraticProblem(*this);
}


/* Quadratic flag accessor */
Bool QuadraticProblem::isQuadratic() const
{
  return true;
}


/* Objective accessor */
void QuadraticProblem::setHessian(const SymmetricMatrix & hessian)
{
  if (hessian.getDimension() != linearCost_.getDimension())
    throw InvalidDimensionException(HERE) << "Hessian dimension (" << hessian.getDimension() << ") must match linear cost dimension (" << linearCost_.getDimension() << ")";
  hessian_ = hessian;
  updateObjective();
}


SymmetricMatrix QuadraticProblem::getHessian() const
{
  return hessian_;
}


void QuadraticProblem::setLinearCost(const Point & linearCost)
{
  if (hessian_.getDimension() != linearCost.getDimension())
    throw InvalidDimensionException(HERE) << "Linear cost dimension (" << linearCost.getDimension() << ") must match hessian dimension (" << hessian_.getDimension() << ")";
  if ((constraintCoefficients_.getNbColumns() != 0) && (constraintCoefficients_.getNbColumns() != linearCost.getDimension()))
    throw InvalidDimensionException(HERE) << "Linear cost dimension (" << linearCost.getDimension() << ") must match constraint matrix column dimension (" << constraintCoefficients_.getNbColumns() << ")";
  linearCost_ = linearCost;
  updateObjective();
}


Point QuadraticProblem::getLinearCost() const
{
  return linearCost_;
}


void QuadraticProblem::setConstant(const Scalar constant)
{
  constant_ = constant;
  updateObjective();
}


Scalar QuadraticProblem::getConstant() const
{
  return constant_;
}


/* Constraint accessor */
void QuadraticProblem::setLinearConstraint(const Matrix & constraintCoefficients, const Interval & constraintBounds)
{
  if (constraintCoefficients.getNbColumns() != linearCost_.getDimension())
    throw InvalidDimensionException(HERE) << "Matrix column dimension (" << constraintCoefficients.getNbColumns() << ") must match problem dimension (" << linearCost_.getDimension() << ")";
  if (constraintCoefficients.getNbRows() != constraintBounds.getDimension())
    throw InvalidDimensionException(HERE) << "Constraint bounds dimension (" << constraintBounds.getDimension() << ") must match matrix row dimension (" << constraintCoefficients.getNbRows() << ")";
  constraintCoefficients_ = constraintCoefficients;
  constraintBounds_ = constraintBounds;
}


Matrix QuadraticProblem::getLinearConstraintCoefficients() const
{
  return constraintCoefficients_;
}


Interval QuadraticProblem::getLinearConstraintBounds() const
{
  return constraintBounds_;
}


/* Equality and inequality constraints, derived from the linear constraints */
Bool QuadraticProblem::hasEqualityConstraint() const
{
  const UnsignedInteger rowDimension = constraintBounds_.getDimension();
  for (UnsignedInteger i = 0; i < rowDimension; ++i)
    if (IsEqualityConstraintRow(constraintBounds_, i))
      return true;
  return false;
}


Function QuadraticProblem::getEqualityConstraint() const
{
  const UnsignedInteger rowDimension = constraintBounds_.getDimension();
  Indices equalityRows;
  for (UnsignedInteger i = 0; i < rowDimension; ++i)
    if (IsEqualityConstraintRow(constraintBounds_, i))
      equalityRows.add(i);
  const UnsignedInteger dimension = linearCost_.getDimension();
  const UnsignedInteger outputDimension = equalityRows.getSize();
  Matrix coefficients(outputDimension, dimension);
  Point constant(outputDimension);
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
  {
    const UnsignedInteger row = equalityRows[i];
    for (UnsignedInteger j = 0; j < dimension; ++j)
      coefficients(i, j) = constraintCoefficients_(row, j);
    constant[i] = -constraintBounds_.getLowerBound()[row];
  }
  return LinearFunction(Point(dimension), constant, coefficients);
}


void QuadraticProblem::setEqualityConstraint(const Function & )
{
  throw InvalidArgumentException(HERE) << "QuadraticProblem only supports linear constraints, use setLinearConstraint with equal lower and upper bounds to define an equality constraint";
}


Bool QuadraticProblem::hasInequalityConstraint() const
{
  const UnsignedInteger rowDimension = constraintBounds_.getDimension();
  for (UnsignedInteger i = 0; i < rowDimension; ++i)
    if (!IsEqualityConstraintRow(constraintBounds_, i)
        && (constraintBounds_.getFiniteLowerBound()[i] || constraintBounds_.getFiniteUpperBound()[i]))
      return true;
  return false;
}


Function QuadraticProblem::getInequalityConstraint() const
{
  const UnsignedInteger rowDimension = constraintBounds_.getDimension();
  const Interval::BoolCollection finiteLowerBound(constraintBounds_.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpperBound(constraintBounds_.getFiniteUpperBound());
  UnsignedInteger outputDimension = 0;
  for (UnsignedInteger i = 0; i < rowDimension; ++i)
    if (!IsEqualityConstraintRow(constraintBounds_, i))
    {
      if (finiteLowerBound[i])
        ++outputDimension;
      if (finiteUpperBound[i])
        ++outputDimension;
    }
  const UnsignedInteger dimension = linearCost_.getDimension();
  if (outputDimension == 0)
    return LinearFunction(Point(dimension), Point(0), Matrix(0, dimension));
  Matrix coefficients(outputDimension, dimension);
  Point constant(outputDimension);
  UnsignedInteger row = 0;
  for (UnsignedInteger i = 0; i < rowDimension; ++i)
  {
    if (IsEqualityConstraintRow(constraintBounds_, i))
      continue;
    // L <= a.x is written a.x - L >= 0
    if (finiteLowerBound[i])
    {
      for (UnsignedInteger j = 0; j < dimension; ++j)
        coefficients(row, j) = constraintCoefficients_(i, j);
      constant[row] = -constraintBounds_.getLowerBound()[i];
      ++row;
    }
    // a.x <= U is written U - a.x >= 0
    if (finiteUpperBound[i])
    {
      for (UnsignedInteger j = 0; j < dimension; ++j)
        coefficients(row, j) = -constraintCoefficients_(i, j);
      constant[row] = constraintBounds_.getUpperBound()[i];
      ++row;
    }
  }
  return LinearFunction(Point(dimension), constant, coefficients);
}


void QuadraticProblem::setInequalityConstraint(const Function & )
{
  throw InvalidArgumentException(HERE) << "QuadraticProblem only supports linear constraints, use setLinearConstraint to define inequality constraints";
}


/* Build a least-squares problem */
QuadraticProblem QuadraticProblem::FromLeastSquares(const Matrix & matrix,
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

  QuadraticProblem problem(hessian, linearCost, bounds);
  problem.setConstant(rhs.normSquare());
  return problem;
}


/* Objective accessor: the quadratic coefficients are the reference, so a
 * generic objective replacement is rejected instead of leaving a stale
 * Hessian, linear cost or constant for the solvers. */
void QuadraticProblem::setObjective(const Function &)
{
  throw InvalidArgumentException(HERE) << "QuadraticProblem does not support setObjective, use setHessian, setLinearCost and setConstant instead";
}


/* Rebuild the objective function from the quadratic data */
void QuadraticProblem::updateObjective()
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
String QuadraticProblem::__repr__() const
{
  OSS oss;
  oss << "class=" << QuadraticProblem::GetClassName();
  oss << " hessian=" << hessian_
      << " linearCost=" << linearCost_
      << " constant=" << constant_
      << " constraintCoefficients=" << constraintCoefficients_
      << " constraintBounds=" << constraintBounds_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void QuadraticProblem::save(Advocate & adv) const
{
  OptimizationProblemImplementation::save(adv);
  adv.saveAttribute("hessian_", hessian_);
  adv.saveAttribute("linearCost_", linearCost_);
  adv.saveAttribute("constant_", constant_);
  adv.saveAttribute("constraintCoefficients_", constraintCoefficients_);
  adv.saveAttribute("constraintBounds_", constraintBounds_);
}

/* Method load() reloads the object from the StorageManager */
void QuadraticProblem::load(Advocate & adv)
{
  OptimizationProblemImplementation::load(adv);
  adv.loadAttribute("hessian_", hessian_);
  adv.loadAttribute("linearCost_", linearCost_);
  adv.loadAttribute("constant_", constant_);
  adv.loadAttribute("constraintCoefficients_", constraintCoefficients_);
  adv.loadAttribute("constraintBounds_", constraintBounds_);
}

END_NAMESPACE_OPENTURNS
