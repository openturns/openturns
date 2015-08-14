//                                               -*- C++ -*-
/**
 *  @brief BoundConstrainedAlgorithmImplementation implements an algorithm for finding the
 *         point of an interval that minimize the given objective function
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <cstdlib>

#include "BoundConstrainedAlgorithmImplementation.hxx"
#include "Log.hxx"
#include "ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(BoundConstrainedAlgorithmImplementation);

/* Default constructor */
BoundConstrainedAlgorithmImplementation::BoundConstrainedAlgorithmImplementation() :
  PersistentObject(),
  objectiveFunction_(NumericalMathFunction()),
  boundConstraints_(0),
  startingPoint_(NumericalPoint(0)),
  optimization_(Result::MINIMIZATION),
  maximumEvaluationsNumber_(ResourceMap::GetAsUnsignedInteger( "BoundConstrainedAlgorithmImplementation-DefaultMaximumEvaluationsNumber" )),
  maximumAbsoluteError_(ResourceMap::GetAsNumericalScalar( "BoundConstrainedAlgorithmImplementation-DefaultMaximumAbsoluteError" )),
  maximumRelativeError_(ResourceMap::GetAsNumericalScalar( "BoundConstrainedAlgorithmImplementation-DefaultMaximumRelativeError" )),
  maximumObjectiveError_(ResourceMap::GetAsNumericalScalar( "BoundConstrainedAlgorithmImplementation-DefaultMaximumObjectiveError" )),
  maximumConstraintError_(ResourceMap::GetAsNumericalScalar( "BoundConstrainedAlgorithmImplementation-DefaultMaximumConstraintError" )),
  result_(Result(startingPoint_, 0.0, Result::MINIMIZATION, 0, -1.0, -1.0, -1.0, -1.0)),
  verbose_(false)
{
  // Nothing to do
}

/* Constructor with parameters: no constraint, starting from the origin */
BoundConstrainedAlgorithmImplementation::BoundConstrainedAlgorithmImplementation(const NumericalMathFunction & objectiveFunction,
    const Bool verbose):
  PersistentObject(),
  objectiveFunction_(objectiveFunction),
  boundConstraints_(NumericalPoint(objectiveFunction.getInputDimension(), -1.0), NumericalPoint(objectiveFunction.getInputDimension(), 1.0), Interval::BoolCollection(objectiveFunction.getInputDimension(), 0), Interval::BoolCollection(objectiveFunction.getInputDimension(), 0)),
  startingPoint_(NumericalPoint(objectiveFunction.getInputDimension(), 0.0)),
  optimization_(Result::MINIMIZATION),
  maximumEvaluationsNumber_(ResourceMap::GetAsUnsignedInteger( "BoundConstrainedAlgorithmImplementation-DefaultMaximumEvaluationsNumber" )),
  maximumAbsoluteError_(ResourceMap::GetAsNumericalScalar( "BoundConstrainedAlgorithmImplementation-DefaultMaximumAbsoluteError" )),
  maximumRelativeError_(ResourceMap::GetAsNumericalScalar( "BoundConstrainedAlgorithmImplementation-DefaultMaximumRelativeError" )),
  maximumObjectiveError_(ResourceMap::GetAsNumericalScalar( "BoundConstrainedAlgorithmImplementation-DefaultMaximumObjectiveError" )),
  maximumConstraintError_(ResourceMap::GetAsNumericalScalar( "BoundConstrainedAlgorithmImplementation-DefaultMaximumConstraintError" )),
  result_(Result(startingPoint_, 0.0, Result::MINIMIZATION, 0, -1.0, -1.0, -1.0, -1.0)),
  verbose_(verbose)
{
  // Nothing to do
}

/* Constructor with parameters: bound constraints, starting from the given point */
BoundConstrainedAlgorithmImplementation::BoundConstrainedAlgorithmImplementation(const NumericalMathFunction & objectiveFunction,
    const Interval & boundConstraints,
    const NumericalPoint & startingPoint,
    const OptimizationProblem optimization,
    const Bool verbose):
  PersistentObject(),
  objectiveFunction_(objectiveFunction),
  boundConstraints_(boundConstraints),
  startingPoint_(startingPoint),
  optimization_(optimization),
  maximumEvaluationsNumber_(ResourceMap::GetAsUnsignedInteger( "BoundConstrainedAlgorithmImplementation-DefaultMaximumEvaluationsNumber" )),
  maximumAbsoluteError_(ResourceMap::GetAsNumericalScalar( "BoundConstrainedAlgorithmImplementation-DefaultMaximumAbsoluteError" )),
  maximumRelativeError_(ResourceMap::GetAsNumericalScalar( "BoundConstrainedAlgorithmImplementation-DefaultMaximumRelativeError" )),
  maximumObjectiveError_(ResourceMap::GetAsNumericalScalar( "BoundConstrainedAlgorithmImplementation-DefaultMaximumObjectiveError" )),
  maximumConstraintError_(ResourceMap::GetAsNumericalScalar( "BoundConstrainedAlgorithmImplementation-DefaultMaximumConstraintError" )),
  result_(Result(startingPoint_, 0.0, optimization, 0, -1.0, -1.0, -1.0, -1.0)),
  verbose_(verbose)
{
  // Check compatibility between the objective function, the constraints and the starting point
  if ((objectiveFunction.getInputDimension() != boundConstraints.getDimension()) || (boundConstraints.getDimension() != startingPoint.getDimension())) throw InvalidArgumentException(HERE) << "Error: the given objective function, bound constraints and starting point have incompatible dimensions";
  if (boundConstraints.isEmpty()) throw InvalidArgumentException(HERE) << "Error: the given bound constraints define an empty interval";
  if (!boundConstraints.contains(startingPoint)) LOGWARN("Warning: the given starting point does not satisfy the bound constraints");
}

/* Starting point accessor */
NumericalPoint BoundConstrainedAlgorithmImplementation::getStartingPoint() const
{
  return startingPoint_;
}

void BoundConstrainedAlgorithmImplementation::setStartingPoint(const NumericalPoint & startingPoint)
{
  startingPoint_ = startingPoint;
}

/* Bound constraints accessor */
Interval BoundConstrainedAlgorithmImplementation::getBoundConstraints() const
{
  return boundConstraints_;
}

void BoundConstrainedAlgorithmImplementation::setBoundConstraints(const Interval & boundConstraints)
{
  boundConstraints_ = boundConstraints;
}

/* Optimization problem accessor */
BoundConstrainedAlgorithmImplementation::OptimizationProblem BoundConstrainedAlgorithmImplementation::getOptimizationProblem() const
{
  return optimization_;
}

void BoundConstrainedAlgorithmImplementation::setOptimizationProblem(const OptimizationProblem optimization)
{
  optimization_ = optimization;
}

/* Result accessor */
BoundConstrainedAlgorithmImplementation::Result BoundConstrainedAlgorithmImplementation::getResult() const
{
  return result_;
}

void BoundConstrainedAlgorithmImplementation::setResult(const Result & result)
{
  result_ = result;
}

/* Maximum iterations number accessor */
UnsignedInteger BoundConstrainedAlgorithmImplementation::getMaximumEvaluationsNumber() const
{
  return maximumEvaluationsNumber_;
}

void BoundConstrainedAlgorithmImplementation::setMaximumEvaluationsNumber(const UnsignedInteger maximumEvaluationsNumber)
{
  maximumEvaluationsNumber_ = maximumEvaluationsNumber;
}

/* Maximum absolute error accessor */
NumericalScalar BoundConstrainedAlgorithmImplementation::getMaximumAbsoluteError() const
{
  return maximumAbsoluteError_;
}

void BoundConstrainedAlgorithmImplementation::setMaximumAbsoluteError(const NumericalScalar maximumAbsoluteError)
{
  maximumAbsoluteError_ = maximumAbsoluteError;
}

/* Maximum relative error accessor */
NumericalScalar BoundConstrainedAlgorithmImplementation::getMaximumRelativeError() const
{
  return maximumRelativeError_;
}

void BoundConstrainedAlgorithmImplementation::setMaximumRelativeError(const NumericalScalar maximumRelativeError)
{
  maximumRelativeError_ = maximumRelativeError;
}

/* Maximum objective error accessor */
NumericalScalar BoundConstrainedAlgorithmImplementation::getMaximumObjectiveError() const
{
  return maximumObjectiveError_;
}

void BoundConstrainedAlgorithmImplementation::setMaximumObjectiveError(const NumericalScalar maximumObjectiveError)
{
  maximumObjectiveError_ = maximumObjectiveError;
}

/* Maximum constraint error accessor */
NumericalScalar BoundConstrainedAlgorithmImplementation::getMaximumConstraintError() const
{
  return maximumConstraintError_;
}

void BoundConstrainedAlgorithmImplementation::setMaximumConstraintError(const NumericalScalar maximumConstraintError)
{
  maximumConstraintError_ = maximumConstraintError;
}

/* String converter */
String BoundConstrainedAlgorithmImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << BoundConstrainedAlgorithmImplementation::GetClassName()
      << " objectiveFunction=" << objectiveFunction_
      << " boundConstraints=" << boundConstraints_
      << " startingPoint=" << startingPoint_
      << " optimization problem=" << optimization_
      << " maximumEvaluationsNumber=" << maximumEvaluationsNumber_
      << " maximumAbsoluteError=" << maximumAbsoluteError_
      << " maximumRelativeError=" << maximumRelativeError_
      << " maximumObjectiveError=" << maximumObjectiveError_
      << " maximumConstraintError=" << maximumConstraintError_
      << " verbose=" << (verbose_ ? "true" : "false");
  return oss;
}

/* Objective function accessor */
NumericalMathFunction BoundConstrainedAlgorithmImplementation::getObjectiveFunction() const
{
  return objectiveFunction_;
}

void BoundConstrainedAlgorithmImplementation::setObjectiveFunction(const NumericalMathFunction & objectiveFunction)
{
  objectiveFunction_ = objectiveFunction;
}

/* Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
void BoundConstrainedAlgorithmImplementation::run()
{
  throw NotYetImplementedException(HERE) << "In BoundConstrainedAlgorithmImplementation::run()";
}

/* Virtual constructor */
BoundConstrainedAlgorithmImplementation * BoundConstrainedAlgorithmImplementation::clone() const
{
  return new BoundConstrainedAlgorithmImplementation(*this);
}

/* Verbose accessor */
Bool BoundConstrainedAlgorithmImplementation::getVerbose() const
{
  return verbose_;
}

/* Verbose accessor */
void BoundConstrainedAlgorithmImplementation::setVerbose(const Bool verbose)
{
  verbose_ = verbose;
}

END_NAMESPACE_OPENTURNS
