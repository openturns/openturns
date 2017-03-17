//                                               -*- C++ -*-
/**
 *  @brief OptimizationAlgorithm provides capabilities to solve optimization problems
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/Cobyla.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(OptimizationAlgorithm);

/* Default constructor */
OptimizationAlgorithm::OptimizationAlgorithm() :
  TypedInterfaceObject<OptimizationAlgorithmImplementation>(new OptimizationAlgorithmImplementation())
{
  // Nothing to do
}

/* Constructor from an implementation */
OptimizationAlgorithm::OptimizationAlgorithm(const OptimizationAlgorithmImplementation & implementation) :
  TypedInterfaceObject<OptimizationAlgorithmImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from a Pointer to an implementation */
OptimizationAlgorithm::OptimizationAlgorithm(const Implementation & p_implementation) :
  TypedInterfaceObject<OptimizationAlgorithmImplementation>(p_implementation)
{
  // Nothing to do
}

/*
 * @brief  Standard constructor: the problem is defined by an OptimizationProblem
 */
OptimizationAlgorithm::OptimizationAlgorithm(const OptimizationProblem & problem):
  TypedInterfaceObject<OptimizationAlgorithmImplementation>(new Cobyla(problem))
{
  // Nothing to do
}

/* Starting point accessor */
NumericalPoint OptimizationAlgorithm::getStartingPoint() const
{
  return getImplementation()->getStartingPoint();
}

void OptimizationAlgorithm::setStartingPoint(const NumericalPoint & startingPoint)
{
  copyOnWrite();
  getImplementation()->setStartingPoint(startingPoint);
}

/* Problem accessor */
OptimizationProblem OptimizationAlgorithm::getProblem() const
{
  return getImplementation()->getProblem();
}

void OptimizationAlgorithm::setProblem(const OptimizationProblem & problem)
{
  copyOnWrite();
  getImplementation()->setProblem(problem);
}

/* Result accessor */
OptimizationAlgorithm::Result OptimizationAlgorithm::getResult() const
{
  return getImplementation()->getResult();
}

/* Result accessor */
void OptimizationAlgorithm::setResult(const Result & result)
{
  copyOnWrite();
  getImplementation()->setResult(result);
}

/* Maximum iterations number accessor */
UnsignedInteger OptimizationAlgorithm::getMaximumIterationNumber() const
{
  return getImplementation()->getMaximumIterationNumber();
}

/* Maximum iterations number accessor */
void OptimizationAlgorithm::setMaximumIterationNumber(const UnsignedInteger maximumIterationNumber)
{
  copyOnWrite();
  getImplementation()->setMaximumIterationNumber(maximumIterationNumber);
}

/* Maximum absolute error accessor */
NumericalScalar OptimizationAlgorithm::getMaximumAbsoluteError() const
{
  return getImplementation()->getMaximumAbsoluteError();
}

/* Maximum absolute error accessor */
void OptimizationAlgorithm::setMaximumAbsoluteError(const NumericalScalar maximumAbsoluteError)
{
  copyOnWrite();
  getImplementation()->setMaximumAbsoluteError(maximumAbsoluteError);
}

/* Maximum relative error accessor */
NumericalScalar OptimizationAlgorithm::getMaximumRelativeError() const
{
  return getImplementation()->getMaximumRelativeError();
}

/* Maximum relative error accessor */
void OptimizationAlgorithm::setMaximumRelativeError(const NumericalScalar maximumRelativeError)
{
  copyOnWrite();
  getImplementation()->setMaximumRelativeError(maximumRelativeError);
}

/* Maximum residual error accessor */
NumericalScalar OptimizationAlgorithm::getMaximumResidualError() const
{
  return getImplementation()->getMaximumResidualError();
}

/* Maximum residual error accessor */
void OptimizationAlgorithm::setMaximumResidualError(const NumericalScalar maximumResidualError)
{
  copyOnWrite();
  getImplementation()->setMaximumResidualError(maximumResidualError);
}

/* Maximum constraint error accessor */
NumericalScalar OptimizationAlgorithm::getMaximumConstraintError() const
{
  return getImplementation()->getMaximumConstraintError();
}

/* Maximum constraint error accessor */
void OptimizationAlgorithm::setMaximumConstraintError(const NumericalScalar maximumConstraintError)
{
  copyOnWrite();
  getImplementation()->setMaximumConstraintError(maximumConstraintError);
}

/* Verbose accessor */
Bool OptimizationAlgorithm::getVerbose() const
{
  return getImplementation()->getVerbose();
}

/* Verbose accessor */
void OptimizationAlgorithm::setVerbose(const Bool verbose)
{
  copyOnWrite();
  getImplementation()->setVerbose(verbose);
}

/* String converter */
String OptimizationAlgorithm::__repr__() const
{
  OSS oss(true);
  oss << "class=" << OptimizationAlgorithm::GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* String converter */
String OptimizationAlgorithm::__str__(const String & offset) const
{
  return __repr__();
}

/* Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
void OptimizationAlgorithm::run()
{
  getImplementation()->run();
}

void OptimizationAlgorithm::setProgressCallback(ProgressCallback callBack, void * data)
{
  getImplementation()->setProgressCallback(callBack, data);
}


void OptimizationAlgorithm::setStopCallback(StopCallback callBack, void * data)
{
  getImplementation()->setStopCallback(callBack, data);
}


END_NAMESPACE_OPENTURNS
