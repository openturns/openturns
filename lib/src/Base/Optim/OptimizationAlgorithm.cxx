//                                               -*- C++ -*-
/**
 *  @brief OptimizationAlgorithm provides capabilities to solve optimization problems
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/AbdoRackwitz.hxx"
#include "openturns/Bonmin.hxx"
#include "openturns/Ceres.hxx"
#include "openturns/CMinpack.hxx"
#include "openturns/Cobyla.hxx"
#include "openturns/Dlib.hxx"
#include "openturns/Ipopt.hxx"
#include "openturns/SQP.hxx"
#include "openturns/TNC.hxx"
#include "openturns/NLopt.hxx"
#include "openturns/Pagmo.hxx"
#include "openturns/PlatformInfo.hxx"

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

/* Starting sample accessor */
Sample OptimizationAlgorithm::getStartingSample() const
{
  return getImplementation()->getStartingSample();
}

void OptimizationAlgorithm::setStartingSample(const Sample & startingSample)
{
  copyOnWrite();
  getImplementation()->setStartingSample(startingSample);
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

/* Maximum calls number accessor */
UnsignedInteger OptimizationAlgorithm::getMaximumCallsNumber() const
{
  return getImplementation()->getMaximumCallsNumber();
}

void OptimizationAlgorithm::setMaximumCallsNumber(const UnsignedInteger maximumCallsNumber)
{
  copyOnWrite();
  getImplementation()->setMaximumCallsNumber(maximumCallsNumber);
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

/* Maximum time accessor */
void OptimizationAlgorithm::setMaximumTimeDuration(const Scalar maximumTime)
{
  copyOnWrite();
  getImplementation()->setMaximumTimeDuration(maximumTime);
}

Scalar OptimizationAlgorithm::getMaximumTimeDuration() const
{
  return getImplementation()->getMaximumTimeDuration();
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
  copyOnWrite();
  getImplementation()->setProgressCallback(callBack, state);
}

void OptimizationAlgorithm::setStopCallback(StopCallback callBack, void * state)
{
  copyOnWrite();
  getImplementation()->setStopCallback(callBack, state);
}

/* Check status accessor */
void OptimizationAlgorithm::setCheckStatus(const Bool checkStatus)
{
  copyOnWrite();
  getImplementation()->setCheckStatus(checkStatus);
}

Bool OptimizationAlgorithm::getCheckStatus() const
{
  return getImplementation()->getCheckStatus();
}

OptimizationAlgorithm OptimizationAlgorithm::GetByName(const String & solverName)
{
  OptimizationAlgorithm solver;
  if (solverName == "AbdoRackwitz")
  {
    solver = AbdoRackwitz();
  }
  else if (PlatformInfo::HasFeature("ceres") && Ceres::GetAlgorithmNames().contains(solverName))
  {
    solver = Ceres(solverName);
  }
  else if (PlatformInfo::HasFeature("cminpack") && (solverName == "CMinpack"))
  {
    solver = CMinpack();
  }
  else if (solverName == "Cobyla")
  {
    solver = Cobyla();
  }
  else if (solverName == "SQP")
  {
    solver = SQP();
  }
  else if (solverName == "TNC")
  {
    solver = TNC();
  }
  else if (PlatformInfo::HasFeature("nlopt") && NLopt::GetAlgorithmNames().contains(solverName))
  {
    solver = NLopt(solverName);
  }
  else if (PlatformInfo::HasFeature("dlib") && Dlib::GetAlgorithmNames().contains(solverName))
  {
    solver = Dlib(solverName);
  }
  else if (PlatformInfo::HasFeature("bonmin") && Bonmin::GetAlgorithmNames().contains(solverName))
  {
    solver = Bonmin(solverName);
  }
  else if (PlatformInfo::HasFeature("ipopt") && solverName == "Ipopt")
  {
    solver = Ipopt();
  }
  else if (PlatformInfo::HasFeature("pagmo") && Pagmo::GetAlgorithmNames().contains(solverName))
  {
    solver = Pagmo(solverName);
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
    OptimizationAlgorithm algorithm(GetByName(names[i]));
    try
    {
      algorithm.setProblem(problem);
      return algorithm;
    }
    catch (const InvalidArgumentException &)
    {
      // try next algorithm
    }
  }
  throw NotYetImplementedException(HERE) << "No optimization algorithm available for this problem";
}


Description OptimizationAlgorithm::GetAlgorithmNames()
{
  Description names;
  names.add("AbdoRackwitz");
  if (PlatformInfo::HasFeature("bonmin"))
    names.add(Bonmin::GetAlgorithmNames());
  if (PlatformInfo::HasFeature("ipopt"))
    names.add("Ipopt");
  if (PlatformInfo::HasFeature("ceres"))
    names.add(Ceres::GetAlgorithmNames());
  if (PlatformInfo::HasFeature("cminpack"))
    names.add("CMinpack");
  names.add("Cobyla");
  if (PlatformInfo::HasFeature("dlib"))
    names.add(Dlib::GetAlgorithmNames());
  names.add("SQP");
  names.add("TNC");
  if (PlatformInfo::HasFeature("nlopt"))
    names.add(NLopt::GetAlgorithmNames());
  if (PlatformInfo::HasFeature("pagmo"))
    names.add(Pagmo::GetAlgorithmNames());
  return names;
}


Description OptimizationAlgorithm::GetAlgorithmNames(const OptimizationProblem & problem)
{
  // return the first algorithm that accepts the problem
  Description names(GetAlgorithmNames());
  Description result;
  for (UnsignedInteger i = 0; i < names.getSize(); ++ i)
  {
    OptimizationAlgorithm algorithm(GetByName(names[i]));
    try
    {
      algorithm.setProblem(problem);
      result.add(names[i]);
    }
    catch (const InvalidArgumentException &)
    {
      // try next algorithm
    }
  }
  return result;
}


END_NAMESPACE_OPENTURNS
