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
#ifndef OPENTURNS_OPTIMIZATIONALGORITHM_HXX
#define OPENTURNS_OPTIMIZATIONALGORITHM_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/NumericalMathFunction.hxx"
#include "openturns/OptimizationAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class OptimizationAlgorithm
 * This class provides capabilities to solve optimization problems
 */

class OT_API OptimizationAlgorithm
  : public TypedInterfaceObject<OptimizationAlgorithmImplementation>
{

  CLASSNAME;
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
  NumericalPoint getStartingPoint() const;
  void setStartingPoint(const NumericalPoint & startingPoint);

  /** Problem accessor */
  OptimizationProblem getProblem() const;
  void setProblem(const OptimizationProblem & problem);

  /** Result accessor */
  Result getResult() const;
  void setResult(const Result & result);

  /** Maximum iterations number accessor */
  UnsignedInteger getMaximumIterationNumber() const;
  void setMaximumIterationNumber(const UnsignedInteger maximumIterationNumber);

  /** Maximum absolute error accessor */
  NumericalScalar getMaximumAbsoluteError() const;
  void setMaximumAbsoluteError(const NumericalScalar maximumAbsoluteError);

  /** Maximum relative error accessor */
  NumericalScalar getMaximumRelativeError() const;
  void setMaximumRelativeError(const NumericalScalar maximumRelativeError);

  /** Maximum residual error accessor */
  NumericalScalar getMaximumResidualError() const;
  void setMaximumResidualError(const NumericalScalar maximumResidualError);

  /** Maximum constraint error accessor */
  NumericalScalar getMaximumConstraintError() const;
  void setMaximumConstraintError(const NumericalScalar maximumConstraintError);

  /* Verbose accessor */
  Bool getVerbose() const;
  void setVerbose(const Bool verbose);

  /** Progress callback */
  typedef void (*ProgressCallback)(NumericalScalar, void * data);
  void setProgressCallback(ProgressCallback callBack, void * data = 0);

  /** Stop callback */
  typedef Bool (*StopCallback)(void * data);
  void setStopCallback(StopCallback callBack, void * data = 0);

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

} ; /* class OptimizationAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_OPTIMIZATIONALGORITHM_HXX */
