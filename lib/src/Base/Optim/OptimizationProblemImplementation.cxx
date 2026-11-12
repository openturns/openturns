//                                               -*- C++ -*-
/**
 *  @brief OptimizationProblemImplementation allows one to describe an optimization problem
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

#include "openturns/OptimizationProblemImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/QuadraticFunction.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/LinearEvaluation.hxx"
#include "openturns/SpecFunc.hxx"

#include <limits>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(OptimizationProblemImplementation)

static const Factory<OptimizationProblemImplementation> Factory_OptimizationProblemImplementation;

namespace
{
/* A row whose lower and upper bounds are equal encodes an equality constraint */
Bool IsEqualityConstraintRow(const Point & lowerBound,
                             const Point & upperBound,
                             const Interval::BoolCollection & finiteLowerBound,
                             const Interval::BoolCollection & finiteUpperBound,
                             const UnsignedInteger row)
{
  return finiteLowerBound[row]
         && finiteUpperBound[row]
         && (lowerBound[row] == upperBound[row]);
}

/* A row that is not an equality constraint and has at least one finite bound
   encodes inequalities */
Bool IsInequalityConstraintRow(const Point & lowerBound,
                               const Point & upperBound,
                               const Interval::BoolCollection & finiteLowerBound,
                               const Interval::BoolCollection & finiteUpperBound,
                               const UnsignedInteger row)
{
  return !IsEqualityConstraintRow(lowerBound, upperBound, finiteLowerBound, finiteUpperBound, row)
         && (finiteLowerBound[row] || finiteUpperBound[row]);
}

/* Convert a constraint function to a linear function centered at the origin.
   An empty function clears the corresponding constraints. */
LinearFunction ConvertLinearConstraint(const Function & constraint,
                                       const UnsignedInteger dimension,
                                       const String & kind)
{
  if (!constraint.getEvaluation().getImplementation()->isActualImplementation())
    return LinearFunction(Point(dimension), Point(0), Matrix(0, dimension));
  if (constraint.getInputDimension() != dimension)
    throw InvalidDimensionException(HERE) << "The " << kind << " constraint input dimension (" << constraint.getInputDimension() << ") must match the problem dimension (" << dimension << ")";
  if (!dynamic_cast<const LinearFunction *>(&constraint) && !constraint.isLinear())
    throw InvalidArgumentException(HERE) << "The " << kind << " constraint must be linear, got " << constraint;
  const Point origin(dimension);
  // A Function built from the evaluation of a linear function has only a
  // finite-difference gradient, which loses the coefficients when the
  // constant term is large. Read them from the evaluation instead.
  const LinearEvaluation * linear = dynamic_cast<const LinearEvaluation *>(constraint.getEvaluation().getImplementation().get());
  if (linear)
    return LinearFunction(origin,
                          linear->getConstant() - linear->getLinear() * linear->getCenter(),
                          linear->getLinear());
  return LinearFunction(origin, constraint(origin), constraint.gradient(origin).transpose());
}

/* Rebuild a sparse constraint matrix: keep the rows whose index in newRowOf is
   not noRow and append the rows of a dense matrix after them. */
SparseMatrix MergeConstraintRows(const SparseMatrix & constraintCoefficients,
                                 const Indices & newRowOf,
                                 const UnsignedInteger keptRowDimension,
                                 const Matrix & addedCoefficients,
                                 const UnsignedInteger dimension)
{
  const UnsignedInteger noRow = std::numeric_limits<UnsignedInteger>::max();
  const Indices rowIndices(constraintCoefficients.getRowIndices());
  const Indices columnPointer(constraintCoefficients.getColumnPointer());
  const Point values(constraintCoefficients.getValues());
  Indices tripletRows;
  Indices tripletColumns;
  Point tripletValues;
  for (UnsignedInteger j = 0; j < dimension; ++j)
    for (UnsignedInteger k = columnPointer[j]; k < columnPointer[j + 1]; ++k)
    {
      const UnsignedInteger row = newRowOf[rowIndices[k]];
      if (row == noRow)
        continue;
      tripletRows.add(row);
      tripletColumns.add(j);
      tripletValues.add(values[k]);
    }
  const UnsignedInteger addedRowDimension = addedCoefficients.getNbRows();
  for (UnsignedInteger i = 0; i < addedRowDimension; ++i)
    for (UnsignedInteger j = 0; j < dimension; ++j)
      if (addedCoefficients(i, j) != 0.0)
      {
        tripletRows.add(keptRowDimension + i);
        tripletColumns.add(j);
        tripletValues.add(addedCoefficients(i, j));
      }
  return SparseMatrix(keptRowDimension + addedRowDimension, dimension, tripletRows, tripletColumns, tripletValues);
}
}

/* Default constructor */
OptimizationProblemImplementation::OptimizationProblemImplementation()
  : PersistentObject()
  , minimizationCollection_(1, true)
{
  // Nothing to do
}

OptimizationProblemImplementation::OptimizationProblemImplementation(const Function & objective)
  : PersistentObject()
  , objective_(objective)
  , minimizationCollection_(objective.getOutputDimension(), true)
  , dimension_(objective.getInputDimension())
  , variablesType_(dimension_, CONTINUOUS)
{
  // Nothing to do
}

/*
 * @brief General multi-objective equality, inequality and bound constraints
 */
OptimizationProblemImplementation::OptimizationProblemImplementation( const Function & objective,
    const Function & equalityConstraint,
    const Function & inequalityConstraint,
    const Interval & bounds)
  : PersistentObject()
  , objective_(objective)
  , minimizationCollection_(objective.getOutputDimension(), true)
  , dimension_(objective.getInputDimension())
  , variablesType_(Indices(dimension_, CONTINUOUS))
{
  // Set constraints
  setEqualityConstraint(equalityConstraint);
  setInequalityConstraint(inequalityConstraint);

  // Set bounds
  setBounds(bounds);
}


/* Virtual constructor */
OptimizationProblemImplementation * OptimizationProblemImplementation::clone() const
{
  return new OptimizationProblemImplementation(*this);
}

/* Objective accessor */
Function OptimizationProblemImplementation::getObjective() const
{
  return objective_;
}

void OptimizationProblemImplementation::setObjective(const Function & objective)
{
  if (objective.getInputDimension() != objective_.getInputDimension())
  {
    LOGINFO(OSS() << "Clearing constraints, bounds and variables types");
    // Clear constraints
    if (equalityConstraint_.getEvaluation().getImplementation()->isActualImplementation() || inequalityConstraint_.getEvaluation().getImplementation()->isActualImplementation())
    {
      equalityConstraint_ = Function();
      inequalityConstraint_ = Function();
    }

    // Clear bounds
    bounds_ = Interval(0);

    // Clear variables types
    variablesType_ = Indices(dimension_, CONTINUOUS);

  }
  objective_ = objective;

  // Update dimension_ member accordingly
  dimension_ = objective.getInputDimension();

  // Update variablesType_ member accordingly
  variablesType_ = Indices(dimension_, CONTINUOUS);
}

Bool OptimizationProblemImplementation::hasMultipleObjective() const
{
  return objective_.getOutputDimension() > 1;
}

/* Equality constraint accessor */
Function OptimizationProblemImplementation::getEqualityConstraint() const
{
  return equalityConstraint_;
}

void OptimizationProblemImplementation::setEqualityConstraint(const Function & equalityConstraint)
{
  if (!(equalityConstraint.getInputDimension() == 0 || equalityConstraint.getInputDimension() == dimension_))
    throw InvalidArgumentException(HERE) << "Error: the given equality constraints have an input dimension=" << equalityConstraint.getInputDimension() << " different from the input dimension=" << dimension_ << " of the objective.";

  equalityConstraint_ = equalityConstraint;
}

Bool OptimizationProblemImplementation::hasEqualityConstraint() const
{
  return equalityConstraint_.getEvaluation().getImplementation()->isActualImplementation();
}

/* Inequality constraint accessor */
Function OptimizationProblemImplementation::getInequalityConstraint() const
{
  return inequalityConstraint_;
}

void OptimizationProblemImplementation::setInequalityConstraint(const Function & inequalityConstraint)
{
  if (!(inequalityConstraint.getInputDimension() == 0 || inequalityConstraint.getInputDimension() == dimension_)) throw InvalidArgumentException(HERE) << "Error: the given inequality constraints have an input dimension=" << inequalityConstraint.getInputDimension() << " different from the input dimension=" << dimension_ << " of the objective.";

  inequalityConstraint_ = inequalityConstraint;
}

Bool OptimizationProblemImplementation::hasInequalityConstraint() const
{
  return inequalityConstraint_.getEvaluation().getImplementation()->isActualImplementation();
}

/* Bounds accessor */
Interval OptimizationProblemImplementation::getBounds() const
{
  return bounds_;
}

void OptimizationProblemImplementation::setBounds(const Interval & bounds)
{
  if (!(bounds.getDimension() == 0 || bounds.getDimension() == dimension_)) throw InvalidArgumentException(HERE) << "Error: the given bounds are of dimension=" << bounds.getDimension() << " different from the input dimension=" << dimension_ << " of the objective.";

  bounds_ = bounds;
}

Bool OptimizationProblemImplementation::hasBounds() const
{
  return bounds_.getDimension() > 0;
}

/* Level function accessor */
Function OptimizationProblemImplementation::getLevelFunction() const
{
  throw NotYetImplementedException(HERE) << "in OptimizationProblemImplementation::getLevelFunction";
}

void OptimizationProblemImplementation::setLevelFunction(const Function & /*levelFunction*/)
{
  throw NotYetImplementedException(HERE) << "in OptimizationProblemImplementation::setLevelFunction";
}

Bool OptimizationProblemImplementation::hasLevelFunction() const
{
  return false;
}

Function OptimizationProblemImplementation::getResidualFunction() const
{
  throw NotYetImplementedException(HERE) << "in OptimizationProblemImplementation::getResidualFunction";
}

void OptimizationProblemImplementation::setResidualFunction(const Function & /*residualFunction*/)
{
  throw NotYetImplementedException(HERE) << "in OptimizationProblemImplementation::setResidualFunction";
}

Bool OptimizationProblemImplementation::hasResidualFunction() const
{
  return false;
}

/* Level value accessor */
Scalar OptimizationProblemImplementation::getLevelValue() const
{
  throw NotYetImplementedException(HERE) << "in OptimizationProblemImplementation::getLevelValue";
}

void OptimizationProblemImplementation::setLevelValue(Scalar /*levelValue*/)
{
  throw NotYetImplementedException(HERE) << "in OptimizationProblemImplementation::setLevelValue";
}

/* Linear flag accessor */
Bool OptimizationProblemImplementation::isLinear() const
{
  return false;
}

Point OptimizationProblemImplementation::getLinearCost() const
{
  throw NotYetImplementedException(HERE) << "in OptimizationProblemImplementation::getLinearCost";
}

Matrix OptimizationProblemImplementation::getLinearConstraintCoefficients() const
{
  throw NotYetImplementedException(HERE) << "in OptimizationProblemImplementation::getLinearConstraintCoefficients";
}

SparseMatrix OptimizationProblemImplementation::getSparseLinearConstraintCoefficients() const
{
  return SparseMatrix::FromDenseMatrix(getLinearConstraintCoefficients());
}

Interval OptimizationProblemImplementation::getLinearConstraintBounds() const
{
  throw NotYetImplementedException(HERE) << "in OptimizationProblemImplementation::getLinearConstraintBounds";
}

/* Helpers for problems storing their constraints as L <= A.x <= U */
Bool OptimizationProblemImplementation::HasLinearEqualityConstraint(const Interval & constraintBounds)
{
  const Point lowerBound(constraintBounds.getLowerBound());
  const Point upperBound(constraintBounds.getUpperBound());
  const Interval::BoolCollection finiteLowerBound(constraintBounds.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpperBound(constraintBounds.getFiniteUpperBound());
  for (UnsignedInteger i = 0; i < constraintBounds.getDimension(); ++i)
    if (IsEqualityConstraintRow(lowerBound, upperBound, finiteLowerBound, finiteUpperBound, i))
      return true;
  return false;
}

Bool OptimizationProblemImplementation::HasLinearInequalityConstraint(const Interval & constraintBounds)
{
  const Point lowerBound(constraintBounds.getLowerBound());
  const Point upperBound(constraintBounds.getUpperBound());
  const Interval::BoolCollection finiteLowerBound(constraintBounds.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpperBound(constraintBounds.getFiniteUpperBound());
  for (UnsignedInteger i = 0; i < constraintBounds.getDimension(); ++i)
    if (IsInequalityConstraintRow(lowerBound, upperBound, finiteLowerBound, finiteUpperBound, i))
      return true;
  return false;
}

Function OptimizationProblemImplementation::GetLinearEqualityConstraint(const Matrix & constraintCoefficients,
    const Interval & constraintBounds,
    const UnsignedInteger dimension)
{
  const UnsignedInteger rowDimension = constraintBounds.getDimension();
  const Point lowerBound(constraintBounds.getLowerBound());
  const Point upperBound(constraintBounds.getUpperBound());
  const Interval::BoolCollection finiteLowerBound(constraintBounds.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpperBound(constraintBounds.getFiniteUpperBound());
  UnsignedInteger outputDimension = 0;
  for (UnsignedInteger i = 0; i < rowDimension; ++i)
    if (IsEqualityConstraintRow(lowerBound, upperBound, finiteLowerBound, finiteUpperBound, i))
      ++outputDimension;
  Matrix coefficients(outputDimension, dimension);
  Point constant(outputDimension);
  UnsignedInteger outputRow = 0;
  for (UnsignedInteger i = 0; i < rowDimension; ++i)
  {
    if (!IsEqualityConstraintRow(lowerBound, upperBound, finiteLowerBound, finiteUpperBound, i))
      continue;
    for (UnsignedInteger j = 0; j < dimension; ++j)
      coefficients(outputRow, j) = constraintCoefficients(i, j);
    constant[outputRow] = -lowerBound[i];
    ++outputRow;
  }
  return LinearFunction(Point(dimension), constant, coefficients);
}

Function OptimizationProblemImplementation::GetLinearInequalityConstraint(const Matrix & constraintCoefficients,
    const Interval & constraintBounds,
    const UnsignedInteger dimension)
{
  const UnsignedInteger rowDimension = constraintBounds.getDimension();
  const Point lowerBound(constraintBounds.getLowerBound());
  const Point upperBound(constraintBounds.getUpperBound());
  const Interval::BoolCollection finiteLowerBound(constraintBounds.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpperBound(constraintBounds.getFiniteUpperBound());
  UnsignedInteger outputDimension = 0;
  for (UnsignedInteger i = 0; i < rowDimension; ++i)
    if (IsInequalityConstraintRow(lowerBound, upperBound, finiteLowerBound, finiteUpperBound, i))
    {
      if (finiteLowerBound[i])
        ++outputDimension;
      if (finiteUpperBound[i])
        ++outputDimension;
    }
  Matrix coefficients(outputDimension, dimension);
  Point constant(outputDimension);
  UnsignedInteger outputRow = 0;
  for (UnsignedInteger i = 0; i < rowDimension; ++i)
  {
    if (!IsInequalityConstraintRow(lowerBound, upperBound, finiteLowerBound, finiteUpperBound, i))
      continue;
    // L <= a.x is written a.x - L >= 0
    if (finiteLowerBound[i])
    {
      for (UnsignedInteger j = 0; j < dimension; ++j)
        coefficients(outputRow, j) = constraintCoefficients(i, j);
      constant[outputRow] = -lowerBound[i];
      ++outputRow;
    }
    // a.x <= U is written U - a.x >= 0
    if (finiteUpperBound[i])
    {
      for (UnsignedInteger j = 0; j < dimension; ++j)
        coefficients(outputRow, j) = -constraintCoefficients(i, j);
      constant[outputRow] = upperBound[i];
      ++outputRow;
    }
  }
  return LinearFunction(Point(dimension), constant, coefficients);
}

void OptimizationProblemImplementation::SetLinearEqualityConstraint(Matrix & constraintCoefficients,
    Interval & constraintBounds,
    const Function & equalityConstraint,
    const UnsignedInteger dimension)
{
  const LinearFunction converted(ConvertLinearConstraint(equalityConstraint, dimension, "equality"));
  const UnsignedInteger rowDimension = constraintBounds.getDimension();
  const Point lowerBound(constraintBounds.getLowerBound());
  const Point upperBound(constraintBounds.getUpperBound());
  const Interval::BoolCollection finiteLowerBound(constraintBounds.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpperBound(constraintBounds.getFiniteUpperBound());
  Indices keptRows;
  for (UnsignedInteger i = 0; i < rowDimension; ++i)
    if (!IsEqualityConstraintRow(lowerBound, upperBound, finiteLowerBound, finiteUpperBound, i))
      keptRows.add(i);
  const UnsignedInteger addedDimension = converted.getOutputDimension();
  const UnsignedInteger newRowDimension = keptRows.getSize() + addedDimension;
  Matrix newCoefficients(newRowDimension, dimension);
  Point newLowerBound(newRowDimension);
  Point newUpperBound(newRowDimension);
  Interval::BoolCollection newFiniteLowerBound(newRowDimension);
  Interval::BoolCollection newFiniteUpperBound(newRowDimension);
  UnsignedInteger newRow = 0;
  for (UnsignedInteger k = 0; k < keptRows.getSize(); ++k)
  {
    const UnsignedInteger row = keptRows[k];
    for (UnsignedInteger j = 0; j < dimension; ++j)
      newCoefficients(newRow, j) = constraintCoefficients(row, j);
    newLowerBound[newRow] = lowerBound[row];
    newUpperBound[newRow] = upperBound[row];
    newFiniteLowerBound[newRow] = finiteLowerBound[row];
    newFiniteUpperBound[newRow] = finiteUpperBound[row];
    ++newRow;
  }
  const Matrix addedCoefficients(converted.getLinear());
  const Point addedConstant(converted.getConstant());
  for (UnsignedInteger i = 0; i < addedDimension; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension; ++j)
      newCoefficients(newRow, j) = addedCoefficients(i, j);
    newLowerBound[newRow] = -addedConstant[i];
    newUpperBound[newRow] = -addedConstant[i];
    newFiniteLowerBound[newRow] = true;
    newFiniteUpperBound[newRow] = true;
    ++newRow;
  }
  constraintCoefficients = newCoefficients;
  constraintBounds = Interval(newLowerBound, newUpperBound, newFiniteLowerBound, newFiniteUpperBound);
}

void OptimizationProblemImplementation::SetLinearInequalityConstraint(Matrix & constraintCoefficients,
    Interval & constraintBounds,
    const Function & inequalityConstraint,
    const UnsignedInteger dimension)
{
  const LinearFunction converted(ConvertLinearConstraint(inequalityConstraint, dimension, "inequality"));
  const UnsignedInteger rowDimension = constraintBounds.getDimension();
  const Point lowerBound(constraintBounds.getLowerBound());
  const Point upperBound(constraintBounds.getUpperBound());
  const Interval::BoolCollection finiteLowerBound(constraintBounds.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpperBound(constraintBounds.getFiniteUpperBound());
  Indices keptRows;
  for (UnsignedInteger i = 0; i < rowDimension; ++i)
  {
    const Bool isFree = !finiteLowerBound[i] && !finiteUpperBound[i];
    if (IsEqualityConstraintRow(lowerBound, upperBound, finiteLowerBound, finiteUpperBound, i) || isFree)
      keptRows.add(i);
  }
  const UnsignedInteger addedDimension = converted.getOutputDimension();
  const UnsignedInteger newRowDimension = keptRows.getSize() + addedDimension;
  Matrix newCoefficients(newRowDimension, dimension);
  Point newLowerBound(newRowDimension);
  Point newUpperBound(newRowDimension);
  Interval::BoolCollection newFiniteLowerBound(newRowDimension);
  Interval::BoolCollection newFiniteUpperBound(newRowDimension);
  UnsignedInteger newRow = 0;
  for (UnsignedInteger k = 0; k < keptRows.getSize(); ++k)
  {
    const UnsignedInteger row = keptRows[k];
    for (UnsignedInteger j = 0; j < dimension; ++j)
      newCoefficients(newRow, j) = constraintCoefficients(row, j);
    newLowerBound[newRow] = lowerBound[row];
    newUpperBound[newRow] = upperBound[row];
    newFiniteLowerBound[newRow] = finiteLowerBound[row];
    newFiniteUpperBound[newRow] = finiteUpperBound[row];
    ++newRow;
  }
  const Matrix addedCoefficients(converted.getLinear());
  const Point addedConstant(converted.getConstant());
  for (UnsignedInteger i = 0; i < addedDimension; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension; ++j)
      newCoefficients(newRow, j) = addedCoefficients(i, j);
    newLowerBound[newRow] = -addedConstant[i];
    newUpperBound[newRow] = SpecFunc::Infinity;
    newFiniteLowerBound[newRow] = true;
    newFiniteUpperBound[newRow] = false;
    ++newRow;
  }
  constraintCoefficients = newCoefficients;
  constraintBounds = Interval(newLowerBound, newUpperBound, newFiniteLowerBound, newFiniteUpperBound);
}

/* Sparse counterparts of the helpers above: the coefficients of a problem that
   stores its constraint matrix in compressed sparse column form are visited in
   storage order, so that the cost stays proportional to the number of non-zero
   coefficients instead of the number of rows times the number of columns. */
Function OptimizationProblemImplementation::GetLinearEqualityConstraint(const SparseMatrix & constraintCoefficients,
    const Interval & constraintBounds,
    const UnsignedInteger dimension)
{
  const UnsignedInteger rowDimension = constraintBounds.getDimension();
  const Point lowerBound(constraintBounds.getLowerBound());
  const Point upperBound(constraintBounds.getUpperBound());
  const Interval::BoolCollection finiteLowerBound(constraintBounds.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpperBound(constraintBounds.getFiniteUpperBound());
  const UnsignedInteger noRow = std::numeric_limits<UnsignedInteger>::max();
  Indices outputRowOf(rowDimension, noRow);
  UnsignedInteger outputDimension = 0;
  for (UnsignedInteger i = 0; i < rowDimension; ++i)
    if (IsEqualityConstraintRow(lowerBound, upperBound, finiteLowerBound, finiteUpperBound, i))
      outputRowOf[i] = outputDimension++;
  Matrix coefficients(outputDimension, dimension);
  Point constant(outputDimension);
  const Indices rowIndices(constraintCoefficients.getRowIndices());
  const Indices columnPointer(constraintCoefficients.getColumnPointer());
  const Point values(constraintCoefficients.getValues());
  for (UnsignedInteger j = 0; j < dimension; ++j)
    for (UnsignedInteger k = columnPointer[j]; k < columnPointer[j + 1]; ++k)
    {
      const UnsignedInteger row = outputRowOf[rowIndices[k]];
      if (row != noRow)
        coefficients(row, j) = values[k];
    }
  for (UnsignedInteger i = 0; i < rowDimension; ++i)
    if (outputRowOf[i] != noRow)
      constant[outputRowOf[i]] = -lowerBound[i];
  return LinearFunction(Point(dimension), constant, coefficients);
}

Function OptimizationProblemImplementation::GetLinearInequalityConstraint(const SparseMatrix & constraintCoefficients,
    const Interval & constraintBounds,
    const UnsignedInteger dimension)
{
  const UnsignedInteger rowDimension = constraintBounds.getDimension();
  const Point lowerBound(constraintBounds.getLowerBound());
  const Point upperBound(constraintBounds.getUpperBound());
  const Interval::BoolCollection finiteLowerBound(constraintBounds.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpperBound(constraintBounds.getFiniteUpperBound());
  const UnsignedInteger noRow = std::numeric_limits<UnsignedInteger>::max();
  // L <= a.x is written a.x - L >= 0 and a.x <= U as U - a.x >= 0: each row
  // contributes its lower-bound output before its upper-bound one, as in the
  // dense helper
  Indices lowerRowOf(rowDimension, noRow);
  Indices upperRowOf(rowDimension, noRow);
  UnsignedInteger outputDimension = 0;
  for (UnsignedInteger i = 0; i < rowDimension; ++i)
  {
    if (!IsInequalityConstraintRow(lowerBound, upperBound, finiteLowerBound, finiteUpperBound, i))
      continue;
    if (finiteLowerBound[i])
      lowerRowOf[i] = outputDimension++;
    if (finiteUpperBound[i])
      upperRowOf[i] = outputDimension++;
  }
  Matrix coefficients(outputDimension, dimension);
  Point constant(outputDimension);
  const Indices rowIndices(constraintCoefficients.getRowIndices());
  const Indices columnPointer(constraintCoefficients.getColumnPointer());
  const Point values(constraintCoefficients.getValues());
  for (UnsignedInteger j = 0; j < dimension; ++j)
    for (UnsignedInteger k = columnPointer[j]; k < columnPointer[j + 1]; ++k)
    {
      const UnsignedInteger i = rowIndices[k];
      if (lowerRowOf[i] != noRow)
        coefficients(lowerRowOf[i], j) = values[k];
      if (upperRowOf[i] != noRow)
        coefficients(upperRowOf[i], j) = -values[k];
    }
  UnsignedInteger outputRow = 0;
  for (UnsignedInteger i = 0; i < rowDimension; ++i)
  {
    if (!IsInequalityConstraintRow(lowerBound, upperBound, finiteLowerBound, finiteUpperBound, i))
      continue;
    if (finiteLowerBound[i])
    {
      constant[outputRow] = -lowerBound[i];
      ++outputRow;
    }
    if (finiteUpperBound[i])
    {
      constant[outputRow] = upperBound[i];
      ++outputRow;
    }
  }
  return LinearFunction(Point(dimension), constant, coefficients);
}

void OptimizationProblemImplementation::SetLinearEqualityConstraint(SparseMatrix & constraintCoefficients,
    Interval & constraintBounds,
    const Function & equalityConstraint,
    const UnsignedInteger dimension)
{
  const LinearFunction converted(ConvertLinearConstraint(equalityConstraint, dimension, "equality"));
  const UnsignedInteger rowDimension = constraintBounds.getDimension();
  const Point lowerBound(constraintBounds.getLowerBound());
  const Point upperBound(constraintBounds.getUpperBound());
  const Interval::BoolCollection finiteLowerBound(constraintBounds.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpperBound(constraintBounds.getFiniteUpperBound());
  const UnsignedInteger noRow = std::numeric_limits<UnsignedInteger>::max();
  Indices newRowOf(rowDimension, noRow);
  Indices keptRows;
  for (UnsignedInteger i = 0; i < rowDimension; ++i)
    if (!IsEqualityConstraintRow(lowerBound, upperBound, finiteLowerBound, finiteUpperBound, i))
    {
      newRowOf[i] = keptRows.getSize();
      keptRows.add(i);
    }
  const UnsignedInteger addedDimension = converted.getOutputDimension();
  const UnsignedInteger newRowDimension = keptRows.getSize() + addedDimension;
  Point newLowerBound(newRowDimension);
  Point newUpperBound(newRowDimension);
  Interval::BoolCollection newFiniteLowerBound(newRowDimension);
  Interval::BoolCollection newFiniteUpperBound(newRowDimension);
  for (UnsignedInteger i = 0; i < rowDimension; ++i)
    if (newRowOf[i] != noRow)
    {
      newLowerBound[newRowOf[i]] = lowerBound[i];
      newUpperBound[newRowOf[i]] = upperBound[i];
      newFiniteLowerBound[newRowOf[i]] = finiteLowerBound[i];
      newFiniteUpperBound[newRowOf[i]] = finiteUpperBound[i];
    }
  const Matrix addedCoefficients(converted.getLinear());
  const Point addedConstant(converted.getConstant());
  for (UnsignedInteger i = 0; i < addedDimension; ++i)
  {
    newLowerBound[keptRows.getSize() + i] = -addedConstant[i];
    newUpperBound[keptRows.getSize() + i] = -addedConstant[i];
    newFiniteLowerBound[keptRows.getSize() + i] = true;
    newFiniteUpperBound[keptRows.getSize() + i] = true;
  }
  constraintCoefficients = MergeConstraintRows(constraintCoefficients, newRowOf, keptRows.getSize(), addedCoefficients, dimension);
  constraintBounds = Interval(newLowerBound, newUpperBound, newFiniteLowerBound, newFiniteUpperBound);
}

void OptimizationProblemImplementation::SetLinearInequalityConstraint(SparseMatrix & constraintCoefficients,
    Interval & constraintBounds,
    const Function & inequalityConstraint,
    const UnsignedInteger dimension)
{
  const LinearFunction converted(ConvertLinearConstraint(inequalityConstraint, dimension, "inequality"));
  const UnsignedInteger rowDimension = constraintBounds.getDimension();
  const Point lowerBound(constraintBounds.getLowerBound());
  const Point upperBound(constraintBounds.getUpperBound());
  const Interval::BoolCollection finiteLowerBound(constraintBounds.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpperBound(constraintBounds.getFiniteUpperBound());
  const UnsignedInteger noRow = std::numeric_limits<UnsignedInteger>::max();
  Indices newRowOf(rowDimension, noRow);
  Indices keptRows;
  for (UnsignedInteger i = 0; i < rowDimension; ++i)
  {
    const Bool isFree = !finiteLowerBound[i] && !finiteUpperBound[i];
    if (IsEqualityConstraintRow(lowerBound, upperBound, finiteLowerBound, finiteUpperBound, i) || isFree)
    {
      newRowOf[i] = keptRows.getSize();
      keptRows.add(i);
    }
  }
  const UnsignedInteger addedDimension = converted.getOutputDimension();
  const UnsignedInteger newRowDimension = keptRows.getSize() + addedDimension;
  Point newLowerBound(newRowDimension);
  Point newUpperBound(newRowDimension);
  Interval::BoolCollection newFiniteLowerBound(newRowDimension);
  Interval::BoolCollection newFiniteUpperBound(newRowDimension);
  for (UnsignedInteger i = 0; i < rowDimension; ++i)
    if (newRowOf[i] != noRow)
    {
      newLowerBound[newRowOf[i]] = lowerBound[i];
      newUpperBound[newRowOf[i]] = upperBound[i];
      newFiniteLowerBound[newRowOf[i]] = finiteLowerBound[i];
      newFiniteUpperBound[newRowOf[i]] = finiteUpperBound[i];
    }
  const Matrix addedCoefficients(converted.getLinear());
  const Point addedConstant(converted.getConstant());
  for (UnsignedInteger i = 0; i < addedDimension; ++i)
  {
    newLowerBound[keptRows.getSize() + i] = -addedConstant[i];
    newUpperBound[keptRows.getSize() + i] = SpecFunc::Infinity;
    newFiniteLowerBound[keptRows.getSize() + i] = true;
    newFiniteUpperBound[keptRows.getSize() + i] = false;
  }
  constraintCoefficients = MergeConstraintRows(constraintCoefficients, newRowOf, keptRows.getSize(), addedCoefficients, dimension);
  constraintBounds = Interval(newLowerBound, newUpperBound, newFiniteLowerBound, newFiniteUpperBound);
}

/* Quadratic flag accessor */
Bool OptimizationProblemImplementation::isQuadratic() const
{
  return false;
}

SymmetricMatrix OptimizationProblemImplementation::getHessian() const
{
  throw NotYetImplementedException(HERE) << "in OptimizationProblemImplementation::getHessian";
}

Scalar OptimizationProblemImplementation::getConstant() const
{
  return 0.0;
}

/* Dimension accessor */
UnsignedInteger OptimizationProblemImplementation::getDimension() const
{
  return dimension_;
}

/* Minimization accessor */
void OptimizationProblemImplementation::setMinimization(Bool minimization, UnsignedInteger marginalIndex)
{
  if (marginalIndex >= objective_.getOutputDimension())
    throw InvalidDimensionException(HERE) << "marginal index (" << marginalIndex << ") cannot exceed objective dimension (" << objective_.getOutputDimension();
  minimizationCollection_[marginalIndex] = minimization;
}

Bool OptimizationProblemImplementation::isMinimization(UnsignedInteger marginalIndex) const
{
  if (marginalIndex >= objective_.getOutputDimension())
    throw InvalidDimensionException(HERE) << "marginal index (" << marginalIndex << ") cannot exceed objective dimension (" << objective_.getOutputDimension();
  return minimizationCollection_[marginalIndex];
}

// Variables type table
void OptimizationProblemImplementation::setVariablesType(const Indices & variablesType)
{
  if (variablesType.getSize() != getDimension())
    throw InvalidDimensionException(HERE) << "variables type table dimension is invalid (" << variablesType.getSize() << ", expected " << getDimension();

  variablesType_ = variablesType;
}

Indices OptimizationProblemImplementation::getVariablesType() const
{
  return variablesType_;
}

Bool OptimizationProblemImplementation::isContinuous() const
{
  if (dimension_ == 0)
    return true;

  for (UnsignedInteger i = 0; i < dimension_; ++i)
    if (variablesType_[i] != CONTINUOUS) return false;

  return true;
}

/* String converter */
String OptimizationProblemImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << OptimizationProblemImplementation::GetClassName();
  oss << " objective=" << objective_
      << " equality constraint=" << (hasEqualityConstraint() ? equalityConstraint_.__repr__() : "none")
      << " inequality constraint=" << (hasInequalityConstraint() ? inequalityConstraint_.__repr__() : "none");
  oss << " bounds=" << (hasBounds() ? bounds_.__repr__() : "none");
  if (minimizationCollection_.getSize() == 1)
    oss << " minimization=" << (minimizationCollection_[0] ? true : false);
  else
    oss << " minimization=" << minimizationCollection_;
  oss << " dimension=" << dimension_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void OptimizationProblemImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "objective_", objective_ );
  adv.saveAttribute( "equalityConstraint_", equalityConstraint_ );
  adv.saveAttribute( "inequalityConstraint_", inequalityConstraint_ );
  adv.saveAttribute( "bounds_", bounds_ );
  adv.saveAttribute( "minimizationCollection_", minimizationCollection_ );
  adv.saveAttribute( "dimension_", dimension_ );
}

/* Method load() reloads the object from the StorageManager */
void OptimizationProblemImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "objective_", objective_ );
  adv.loadAttribute( "equalityConstraint_", equalityConstraint_ );
  adv.loadAttribute( "inequalityConstraint_", inequalityConstraint_ );
  adv.loadAttribute( "bounds_", bounds_ );
  if (adv.hasAttribute("minimizationCollection_"))
    adv.loadAttribute("minimizationCollection_", minimizationCollection_);
  else
  {
    Bool minimization = true;
    adv.loadAttribute("minimization_", minimization);
    minimizationCollection_ = BoolCollection(1, minimization);
  }
  adv.loadAttribute( "dimension_", dimension_ );
}

END_NAMESPACE_OPENTURNS
