//                                               -*- C++ -*-
/**
 *  @brief OptimizationAlgorithm provides capabilities to solve optimization problems
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_OPTIMIZATIONALGORITHM_HXX
#define OPENTURNS_OPTIMIZATIONALGORITHM_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/Function.hxx"
#include "openturns/OptimizationAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class OptimizationAlgorithm
 * This class provides capabilities to solve optimization problems
 */

class OT_API OptimizationAlgorithm
  : public TypedInterfaceObject<OptimizationAlgorithmImplementation>
{

  CLASSNAME
public:

  typedef Pointer<OptimizationAlgorithmImplementation>   Implementation;
  typedef OptimizationResult              Result;

  /** Default constructor */
  OptimizationAlgorithm();

  /** Constructor from an implementation */
  OptimizationAlgorithm(const OptimizationAlgorithmImplementation & implementation);

  /** Constructor from a Pointer to an implementation */
  OptimizationAlgorithm(const Implementation & p_implementation);

  /** Constructor with parameters */
  explicit OptimizationAlgorithm(const OptimizationProblem & problem);

  /** Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
  void run();

  /** Starting point accessor */
  Point getStartingPoint() const;
  void setStartingPoint(const Point & startingPoint);

  /** Problem accessor */
  OptimizationProblem getProblem() const;
  void setProblem(const OptimizationProblem & problem);

  /** Result accessor */
  Result getResult() const;
  void setResult(const Result & result);

  /** Maximum iterations number accessor */
  UnsignedInteger getMaximumIterationNumber() const;
  void setMaximumIterationNumber(const UnsignedInteger maximumIterationNumber);

  /** Maximum calls number accessor */
  void setMaximumCallsNumber(const UnsignedInteger maximumCallsNumber);
  UnsignedInteger getMaximumCallsNumber() const;
  // @deprecated
  void setMaximumEvaluationNumber(const UnsignedInteger maximumEvaluationNumber);
  UnsignedInteger getMaximumEvaluationNumber() const;

  /** Maximum absolute error accessor */
  Scalar getMaximumAbsoluteError() const;
  void setMaximumAbsoluteError(const Scalar maximumAbsoluteError);

  /** Maximum relative error accessor */
  Scalar getMaximumRelativeError() const;
  void setMaximumRelativeError(const Scalar maximumRelativeError);

  /** Maximum residual error accessor */
  Scalar getMaximumResidualError() const;
  void setMaximumResidualError(const Scalar maximumResidualError);

  /** Maximum constraint error accessor */
  Scalar getMaximumConstraintError() const;
  void setMaximumConstraintError(const Scalar maximumConstraintError);

  /** Maximum time accessor */
  void setMaximumTimeDuration(const Scalar maximumTime);
  Scalar getMaximumTimeDuration() const;

  /** Progress callback */
  typedef void (*ProgressCallback)(Scalar, void * state);
  void setProgressCallback(ProgressCallback callBack, void * state = 0);

  /** Stop callback */
  typedef Bool (*StopCallback)(void * state);
  void setStopCallback(StopCallback callBack, void * state = 0);

  /** Check status accessor */
  void setCheckStatus(const Bool checkStatus);
  Bool getCheckStatus() const;
  
  /** Build an instance from name */
  static OptimizationAlgorithm Build(const String & solverName);

  /** Build an instance from problem */
  static OptimizationAlgorithm Build(const OptimizationProblem & problem);

  /** Get all optimization algorithm names */
  static Description GetAlgorithmNames();
  static Description GetAlgorithmNames(const OptimizationProblem & problem);

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

} ; /* class OptimizationAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_OPTIMIZATIONALGORITHM_HXX */
