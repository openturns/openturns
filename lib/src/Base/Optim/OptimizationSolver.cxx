//                                               -*- C++ -*-
/**
 *  @brief OptimizationSolver provides capabilities to solve optimization problems
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
#include "openturns/OptimizationSolver.hxx"
#include "openturns/Cobyla.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(OptimizationSolver);

/* Default constructor */
OptimizationSolver::OptimizationSolver() :
  TypedInterfaceObject<OptimizationSolverImplementation>(new OptimizationSolverImplementation())
{
  // Nothing to do
}

/* Constructor from an implementation */
OptimizationSolver::OptimizationSolver(const OptimizationSolverImplementation & implementation) :
  TypedInterfaceObject<OptimizationSolverImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from a Pointer to an implementation */
OptimizationSolver::OptimizationSolver(const Implementation & p_implementation) :
  TypedInterfaceObject<OptimizationSolverImplementation>(p_implementation)
{
  // Nothing to do
}

/*
 * @brief  Standard constructor: the problem is defined by an OptimizationProblem
 */
OptimizationSolver::OptimizationSolver(const OptimizationProblem & problem):
  TypedInterfaceObject<OptimizationSolverImplementation>(new Cobyla(problem))
{
  // Nothing to do
}

/* Starting point accessor */
NumericalPoint OptimizationSolver::getStartingPoint() const
{
  return getImplementation()->getStartingPoint();
}

void OptimizationSolver::setStartingPoint(const NumericalPoint & startingPoint)
{
  copyOnWrite();
  getImplementation()->setStartingPoint(startingPoint);
}

/* Problem accessor */
OptimizationProblem OptimizationSolver::getProblem() const
{
  return getImplementation()->getProblem();
}

void OptimizationSolver::setProblem(const OptimizationProblem & problem)
{
  copyOnWrite();
  getImplementation()->setProblem(problem);
}

/* Result accessor */
OptimizationSolver::Result OptimizationSolver::getResult() const
{
  return getImplementation()->getResult();
}

/* Result accessor */
void OptimizationSolver::setResult(const Result & result)
{
  copyOnWrite();
  getImplementation()->setResult(result);
}

/* Maximum iterations number accessor */
UnsignedInteger OptimizationSolver::getMaximumIterationNumber() const
{
  return getImplementation()->getMaximumIterationNumber();
}

/* Maximum iterations number accessor */
void OptimizationSolver::setMaximumIterationNumber(const UnsignedInteger maximumIterationNumber)
{
  copyOnWrite();
  getImplementation()->setMaximumIterationNumber(maximumIterationNumber);
}

/* Maximum absolute error accessor */
NumericalScalar OptimizationSolver::getMaximumAbsoluteError() const
{
  return getImplementation()->getMaximumAbsoluteError();
}

/* Maximum absolute error accessor */
void OptimizationSolver::setMaximumAbsoluteError(const NumericalScalar maximumAbsoluteError)
{
  copyOnWrite();
  getImplementation()->setMaximumAbsoluteError(maximumAbsoluteError);
}

/* Maximum relative error accessor */
NumericalScalar OptimizationSolver::getMaximumRelativeError() const
{
  return getImplementation()->getMaximumRelativeError();
}

/* Maximum relative error accessor */
void OptimizationSolver::setMaximumRelativeError(const NumericalScalar maximumRelativeError)
{
  copyOnWrite();
  getImplementation()->setMaximumRelativeError(maximumRelativeError);
}

/* Maximum residual error accessor */
NumericalScalar OptimizationSolver::getMaximumResidualError() const
{
  return getImplementation()->getMaximumResidualError();
}

/* Maximum residual error accessor */
void OptimizationSolver::setMaximumResidualError(const NumericalScalar maximumResidualError)
{
  copyOnWrite();
  getImplementation()->setMaximumResidualError(maximumResidualError);
}

/* Maximum constraint error accessor */
NumericalScalar OptimizationSolver::getMaximumConstraintError() const
{
  return getImplementation()->getMaximumConstraintError();
}

/* Maximum constraint error accessor */
void OptimizationSolver::setMaximumConstraintError(const NumericalScalar maximumConstraintError)
{
  copyOnWrite();
  getImplementation()->setMaximumConstraintError(maximumConstraintError);
}

/* Verbose accessor */
Bool OptimizationSolver::getVerbose() const
{
  return getImplementation()->getVerbose();
}

/* Verbose accessor */
void OptimizationSolver::setVerbose(const Bool verbose)
{
  copyOnWrite();
  getImplementation()->setVerbose(verbose);
}

/* String converter */
String OptimizationSolver::__repr__() const
{
  OSS oss(true);
  oss << "class=" << OptimizationSolver::GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* String converter */
String OptimizationSolver::__str__(const String & offset) const
{
  return __repr__();
}

/* Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
void OptimizationSolver::run()
{
  getImplementation()->run();
}


END_NAMESPACE_OPENTURNS
