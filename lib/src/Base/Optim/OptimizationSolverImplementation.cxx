//                                               -*- C++ -*-
/**
 *  @brief OptimizationSolverImplementation implements an algorithm for solving an optimization problem
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

#include "OptimizationSolverImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(OptimizationSolverImplementation);

/* Default constructor */
OptimizationSolverImplementation::OptimizationSolverImplementation()
  : PersistentObject()
  , startingPoint_(NumericalPoint(0))
  , maximumIterationsNumber_(ResourceMap::GetAsUnsignedInteger( "OptimizationSolverImplementation-DefaultMaximumIteration" ))
  , maximumAbsoluteError_(ResourceMap::GetAsNumericalScalar( "OptimizationSolverImplementation-DefaultMaximumAbsoluteError" ))
  , maximumRelativeError_(ResourceMap::GetAsNumericalScalar( "OptimizationSolverImplementation-DefaultMaximumRelativeError" ))
  , maximumResidualError_(ResourceMap::GetAsNumericalScalar( "OptimizationSolverImplementation-DefaultMaximumResidualError" ))
  , maximumConstraintError_(ResourceMap::GetAsNumericalScalar( "OptimizationSolverImplementation-DefaultMaximumConstraintError" ))
  , verbose_(false)
{
  // Nothing to do
}

/*
 * @brief Standard constructor: the optimization problem is managed by the optimization solver, and the actual solver is in charge to check if it is able to solve it.
 */
OptimizationSolverImplementation::OptimizationSolverImplementation(const OptimizationProblem & problem)
  : PersistentObject()
  , problem_(problem)
  , maximumIterationsNumber_(ResourceMap::GetAsUnsignedInteger( "OptimizationSolverImplementation-DefaultMaximumIteration" ))
  , maximumAbsoluteError_(ResourceMap::GetAsNumericalScalar( "OptimizationSolverImplementation-DefaultMaximumAbsoluteError" ))
  , maximumRelativeError_(ResourceMap::GetAsNumericalScalar( "OptimizationSolverImplementation-DefaultMaximumRelativeError" ))
  , maximumResidualError_(ResourceMap::GetAsNumericalScalar( "OptimizationSolverImplementation-DefaultMaximumResidualError" ))
  , maximumConstraintError_(ResourceMap::GetAsNumericalScalar( "OptimizationSolverImplementation-DefaultMaximumConstraintError" ))
  , verbose_(false)
{
  // Nothing to do
}

/* Starting point accessor */
NumericalPoint OptimizationSolverImplementation::getStartingPoint() const
{
  return startingPoint_;
}

/* Starting point accessor */
void OptimizationSolverImplementation::setStartingPoint(const NumericalPoint & startingPoint)
{
  startingPoint_ = startingPoint;
}

/* Result accessor */
OptimizationSolverImplementation::Result OptimizationSolverImplementation::getResult() const
{
  return result_;
}

/* Result accessor */
void OptimizationSolverImplementation::setResult(const Result & result)
{
  result_ = result;
}

/* Maximum iterations number accessor */
UnsignedInteger OptimizationSolverImplementation::getMaximumIterationsNumber() const
{
  return maximumIterationsNumber_;
}

/* Maximum iterations number accessor */
void OptimizationSolverImplementation::setMaximumIterationsNumber(const UnsignedInteger maximumIterationsNumber)
{
  maximumIterationsNumber_ = maximumIterationsNumber;
}

/* Maximum absolute error accessor */
NumericalScalar OptimizationSolverImplementation::getMaximumAbsoluteError() const
{
  return maximumAbsoluteError_;
}

/* Maximum absolute error accessor */
void OptimizationSolverImplementation::setMaximumAbsoluteError(const NumericalScalar maximumAbsoluteError)
{
  maximumAbsoluteError_ = maximumAbsoluteError;
}

/* Maximum relative error accessor */
NumericalScalar OptimizationSolverImplementation::getMaximumRelativeError() const
{
  return maximumRelativeError_;
}

/* Maximum relative error accessor */
void OptimizationSolverImplementation::setMaximumRelativeError(const NumericalScalar maximumRelativeError)
{
  maximumRelativeError_ = maximumRelativeError;
}

/* Maximum residual error accessor */
NumericalScalar OptimizationSolverImplementation::getMaximumResidualError() const
{
  return maximumResidualError_;
}

/* Maximum residual error accessor */
void OptimizationSolverImplementation::setMaximumResidualError(const NumericalScalar maximumResidualError)
{
  maximumResidualError_ = maximumResidualError;
}

/* Maximum constraint error accessor */
NumericalScalar OptimizationSolverImplementation::getMaximumConstraintError() const
{
  return maximumConstraintError_;
}

/* Maximum constraint error accessor */
void OptimizationSolverImplementation::setMaximumConstraintError(const NumericalScalar maximumConstraintError)
{
  maximumConstraintError_ = maximumConstraintError;
}

/* String converter */
String OptimizationSolverImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << OptimizationSolverImplementation::GetClassName()
      << " problem=" << problem_
      << " startingPoint=" << startingPoint_
      << " maximumIterationsNumber=" << maximumIterationsNumber_
      << " maximumAbsoluteError=" << maximumAbsoluteError_
      << " maximumRelativeError=" << maximumRelativeError_
      << " maximumResidualError=" << maximumResidualError_
      << " maximumConstraintError=" << maximumConstraintError_
      << " verbose=" << (verbose_ ? "true" : "false");
  return oss;
}

/* Problem accessor */
OptimizationProblem OptimizationSolverImplementation::getProblem() const
{
  return problem_;
}

void OptimizationSolverImplementation::setProblem(const OptimizationProblem & problem)
{
  checkProblem(problem);
  problem_ = problem;
}

/* Performs the actual checks. Must be overloaded by the actual optimisation algorithm */
void OptimizationSolverImplementation::checkProblem(const OptimizationProblem & problem) const
{
  throw NotYetImplementedException(HERE) << "In OptimizationSolverImplementation::checkProblem()";
}

/* Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
void OptimizationSolverImplementation::run()
{
  throw NotYetImplementedException(HERE) << "In OptimizationSolverImplementation::run()";
}

/* Virtual constructor */
OptimizationSolverImplementation * OptimizationSolverImplementation::clone() const
{
  return new OptimizationSolverImplementation(*this);
}

/* Verbose accessor */
Bool OptimizationSolverImplementation::getVerbose() const
{
  return verbose_;
}

/* Verbose accessor */
void OptimizationSolverImplementation::setVerbose(const Bool verbose)
{
  verbose_ = verbose;
}

/* Method save() stores the object through the StorageManager */
void OptimizationSolverImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "startingPoint_", startingPoint_);
  adv.saveAttribute( "problem_", problem_);
  adv.saveAttribute( "maximumIterationsNumber_", maximumIterationsNumber_);
  adv.saveAttribute( "maximumAbsoluteError_", maximumAbsoluteError_);
  adv.saveAttribute( "maximumRelativeError_", maximumRelativeError_);
  adv.saveAttribute( "maximumResidualError_", maximumResidualError_);
  adv.saveAttribute( "maximumConstraintError_", maximumConstraintError_);
  adv.saveAttribute( "verbose_", verbose_);
}


/* Method load() reloads the object from the StorageManager */
void OptimizationSolverImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "startingPoint_", startingPoint_);
  adv.loadAttribute( "problem_", problem_);
  adv.loadAttribute( "maximumIterationsNumber_", maximumIterationsNumber_);
  adv.loadAttribute( "maximumAbsoluteError_", maximumAbsoluteError_);
  adv.loadAttribute( "maximumRelativeError_", maximumRelativeError_);
  adv.loadAttribute( "maximumResidualError_", maximumResidualError_);
  adv.loadAttribute( "maximumConstraintError_", maximumConstraintError_);
  adv.loadAttribute( "verbose_", verbose_);
}

END_NAMESPACE_OPENTURNS
