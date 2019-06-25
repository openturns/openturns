//                                               -*- C++ -*-
/**
 *  @brief OptimizationAlgorithm provides capabilities to solve optimization problems
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/Ceres.hxx"
#include "openturns/CMinpack.hxx"
#include "openturns/Cobyla.hxx"
#include "openturns/Dlib.hxx"
#include "openturns/TNC.hxx"
#include "openturns/NLopt.hxx"
#include "openturns/OPTpp.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(OptimizationAlgorithm)

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
Point OptimizationAlgorithm::getStartingPoint() const
{
  return getImplementation()->getStartingPoint();
}

void OptimizationAlgorithm::setStartingPoint(const Point & startingPoint)
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

/* Maximum evaluations number accessor */
UnsignedInteger OptimizationAlgorithm::getMaximumEvaluationNumber() const
{
  return getImplementation()->getMaximumEvaluationNumber();
}

/* Maximum iterations number accessor */
void OptimizationAlgorithm::setMaximumEvaluationNumber(const UnsignedInteger maximumEvaluationNumber)
{
  copyOnWrite();
  getImplementation()->setMaximumEvaluationNumber(maximumEvaluationNumber);
}

/* Maximum absolute error accessor */
Scalar OptimizationAlgorithm::getMaximumAbsoluteError() const
{
  return getImplementation()->getMaximumAbsoluteError();
}

/* Maximum absolute error accessor */
void OptimizationAlgorithm::setMaximumAbsoluteError(const Scalar maximumAbsoluteError)
{
  copyOnWrite();
  getImplementation()->setMaximumAbsoluteError(maximumAbsoluteError);
}

/* Maximum relative error accessor */
Scalar OptimizationAlgorithm::getMaximumRelativeError() const
{
  return getImplementation()->getMaximumRelativeError();
}

/* Maximum relative error accessor */
void OptimizationAlgorithm::setMaximumRelativeError(const Scalar maximumRelativeError)
{
  copyOnWrite();
  getImplementation()->setMaximumRelativeError(maximumRelativeError);
}

/* Maximum residual error accessor */
Scalar OptimizationAlgorithm::getMaximumResidualError() const
{
  return getImplementation()->getMaximumResidualError();
}

/* Maximum residual error accessor */
void OptimizationAlgorithm::setMaximumResidualError(const Scalar maximumResidualError)
{
  copyOnWrite();
  getImplementation()->setMaximumResidualError(maximumResidualError);
}

/* Maximum constraint error accessor */
Scalar OptimizationAlgorithm::getMaximumConstraintError() const
{
  return getImplementation()->getMaximumConstraintError();
}

/* Maximum constraint error accessor */
void OptimizationAlgorithm::setMaximumConstraintError(const Scalar maximumConstraintError)
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
String OptimizationAlgorithm::__str__(const String & ) const
{
  return __repr__();
}

/* Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
void OptimizationAlgorithm::run()
{
  getImplementation()->run();
}

void OptimizationAlgorithm::setProgressCallback(ProgressCallback callBack, void * state)
{
  getImplementation()->setProgressCallback(callBack, state);
}


void OptimizationAlgorithm::setStopCallback(StopCallback callBack, void * state)
{
  getImplementation()->setStopCallback(callBack, state);
}


OptimizationAlgorithm OptimizationAlgorithm::Build(const String & solverName)
{
  OptimizationAlgorithm solver;
  if (Ceres::IsAvailable() && Ceres::GetAlgorithmNames().contains(solverName))
  {
    solver = Ceres(solverName);
  }
  else if (CMinpack::IsAvailable() && (solverName == "CMinpack"))
  {
    solver = CMinpack();
  }
  else if (solverName == "Cobyla")
  {
    solver = Cobyla();
  }
  else if (solverName == "TNC")
  {
    solver = TNC();
  }
  else if (NLopt::IsAvailable() && NLopt::GetAlgorithmNames().contains(solverName))
  {
    solver = NLopt(solverName);
  }
  else if (OPTpp::IsAvailable() && OPTpp::GetAlgorithmNames().contains(solverName))
  {
    solver = OPTpp(solverName);
  }
  else if (Dlib::IsAvailable() && Dlib::GetAlgorithmNames().contains(solverName))
  {
    solver = Dlib(solverName);
  }
  else
    throw InvalidArgumentException(HERE) << "Unknown optimization solver:" << solverName;
  return solver;
}


OptimizationAlgorithm OptimizationAlgorithm::Build(const OptimizationProblem & problem)
{
  // return the first algorithm that accepts the problem
  Description names(GetAlgorithmNames());
  for (UnsignedInteger i = 0; i < names.getSize(); ++ i)
  {
    OptimizationAlgorithm algorithm(Build(names[i]));
    try
    {
      algorithm.setProblem(problem);
      return algorithm;
    }
    catch (InvalidArgumentException &)
    {
      // try next algorithm
    }
  }
  throw NotYetImplementedException(HERE) << "No optimization algorithm available for this problem";
}


Description OptimizationAlgorithm::GetAlgorithmNames()
{
  Description names;
  if (Ceres::IsAvailable())
    names.add(Ceres::GetAlgorithmNames());
  if (CMinpack::IsAvailable())
    names.add("CMinpack");
  names.add("Cobyla");
  names.add("TNC");
  if (NLopt::IsAvailable())
    names.add(NLopt::GetAlgorithmNames());
  if (OPTpp::IsAvailable())
    names.add(OPTpp::GetAlgorithmNames());
  return names;
}


Description OptimizationAlgorithm::GetLeastSquaresAlgorithmNames()
{
  Description names;
  if (CMinpack::IsAvailable())
    names.add("CMinpack");
  if (Ceres::IsAvailable())
    names.add(Ceres::GetAlgorithmNames());
  return names;
}

END_NAMESPACE_OPENTURNS
