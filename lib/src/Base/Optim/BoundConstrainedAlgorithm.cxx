//                                               -*- C++ -*-
/**
 *  @brief BoundConstrainedAlgorithm implements an algorithm for finding the
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
#include "BoundConstrainedAlgorithm.hxx"
#include "TNC.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(BoundConstrainedAlgorithm);

/* Default constructor */
BoundConstrainedAlgorithm::BoundConstrainedAlgorithm() :
  TypedInterfaceObject<BoundConstrainedAlgorithmImplementation>(new BoundConstrainedAlgorithmImplementation())
{
  // Nothing to do
}

/* Constructor from an implementation */
BoundConstrainedAlgorithm::BoundConstrainedAlgorithm(const BoundConstrainedAlgorithmImplementation & implementation) :
  TypedInterfaceObject<BoundConstrainedAlgorithmImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from a Pointer to an implementation */
BoundConstrainedAlgorithm::BoundConstrainedAlgorithm(const Implementation & p_implementation) :
  TypedInterfaceObject<BoundConstrainedAlgorithmImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor with parameters: no constraint, starting from the origin */
BoundConstrainedAlgorithm::BoundConstrainedAlgorithm(const NumericalMathFunction & objectiveFunction,
    const Bool verbose):
  TypedInterfaceObject<BoundConstrainedAlgorithmImplementation>(new TNC(objectiveFunction, verbose))
{
  // Nothing to do
}

/* Constructor with parameters: bound constraints, starting from the given point */
BoundConstrainedAlgorithm::BoundConstrainedAlgorithm(const NumericalMathFunction & objectiveFunction,
    const Interval & boundConstraints,
    const NumericalPoint & startingPoint,
    const OptimizationProblem optimization,
    const Bool verbose)
  : TypedInterfaceObject<BoundConstrainedAlgorithmImplementation>(new TNC(TNCSpecificParameters(), objectiveFunction, boundConstraints, startingPoint, optimization, verbose))
{
  // Nothing to do
}

/* Starting point accessor */
NumericalPoint BoundConstrainedAlgorithm::getStartingPoint() const
{
  return getImplementation()->getStartingPoint();
}

void BoundConstrainedAlgorithm::setStartingPoint(const NumericalPoint & startingPoint)
{
  copyOnWrite();
  getImplementation()->setStartingPoint(startingPoint);
}

/* Bound constraints accessor */
Interval BoundConstrainedAlgorithm::getBoundConstraints() const
{
  return getImplementation()->getBoundConstraints();
}

void BoundConstrainedAlgorithm::setBoundConstraints(const Interval & boundConstraints)
{
  copyOnWrite();
  getImplementation()->setBoundConstraints(boundConstraints);
}

/* Optimization problem accessor */
BoundConstrainedAlgorithm::OptimizationProblem BoundConstrainedAlgorithm::getOptimizationProblem() const
{
  return getImplementation()->getOptimizationProblem();
}

void BoundConstrainedAlgorithm::setOptimizationProblem(const OptimizationProblem optimization)
{
  copyOnWrite();
  getImplementation()->setOptimizationProblem(optimization);
}

/* Result accessor */
BoundConstrainedAlgorithm::Result BoundConstrainedAlgorithm::getResult() const
{
  return getImplementation()->getResult();
}

void BoundConstrainedAlgorithm::setResult(const Result & result)
{
  copyOnWrite();
  getImplementation()->setResult(result);
}

/* Maximum iterations number accessor */
UnsignedInteger BoundConstrainedAlgorithm::getMaximumEvaluationsNumber() const
{
  return getImplementation()->getMaximumEvaluationsNumber();
}

void BoundConstrainedAlgorithm::setMaximumEvaluationsNumber(const UnsignedInteger maximumEvaluationsNumber)
{
  copyOnWrite();
  getImplementation()->setMaximumEvaluationsNumber(maximumEvaluationsNumber);
}

/* Maximum absolute error accessor */
NumericalScalar BoundConstrainedAlgorithm::getMaximumAbsoluteError() const
{
  return getImplementation()->getMaximumAbsoluteError();
}

void BoundConstrainedAlgorithm::setMaximumAbsoluteError(const NumericalScalar maximumAbsoluteError)
{
  copyOnWrite();
  getImplementation()->setMaximumAbsoluteError(maximumAbsoluteError);
}

/* Maximum relative error accessor */
NumericalScalar BoundConstrainedAlgorithm::getMaximumRelativeError() const
{
  return getImplementation()->getMaximumRelativeError();
}

void BoundConstrainedAlgorithm::setMaximumRelativeError(const NumericalScalar maximumRelativeError)
{
  copyOnWrite();
  getImplementation()->setMaximumRelativeError(maximumRelativeError);
}

/* Maximum objective error accessor */
NumericalScalar BoundConstrainedAlgorithm::getMaximumObjectiveError() const
{
  return getImplementation()->getMaximumObjectiveError();
}

void BoundConstrainedAlgorithm::setMaximumObjectiveError(const NumericalScalar maximumObjectiveError)
{
  copyOnWrite();
  getImplementation()->setMaximumObjectiveError(maximumObjectiveError);
}

/* Maximum constraint error accessor */
NumericalScalar BoundConstrainedAlgorithm::getMaximumConstraintError() const
{
  return getImplementation()->getMaximumConstraintError();
}

void BoundConstrainedAlgorithm::setMaximumConstraintError(const NumericalScalar maximumConstraintError)
{
  copyOnWrite();
  getImplementation()->setMaximumConstraintError(maximumConstraintError);
}

/* String converter */
String BoundConstrainedAlgorithm::__repr__() const
{
  OSS oss;
  oss << "class=" << BoundConstrainedAlgorithm::GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* Objective function accessor */
NumericalMathFunction BoundConstrainedAlgorithm::getObjectiveFunction() const
{
  return getImplementation()->getObjectiveFunction();
}

void BoundConstrainedAlgorithm::setObjectiveFunction(const NumericalMathFunction & objectiveFunction)
{
  copyOnWrite();
  getImplementation()->setObjectiveFunction(objectiveFunction);
}

/* Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
void BoundConstrainedAlgorithm::run()
{
  getImplementation()->run();
}

/* Verbose accessor */
Bool BoundConstrainedAlgorithm::getVerbose() const
{
  return getImplementation()->getVerbose();
}

/* Verbose accessor */
void BoundConstrainedAlgorithm::setVerbose(const Bool verbose)
{
  copyOnWrite();
  getImplementation()->setVerbose(verbose);
}

END_NAMESPACE_OPENTURNS
