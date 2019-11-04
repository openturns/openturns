//                                               -*- C++ -*-
/**
 *  @brief OptimizationAlgorithmImplementation implements an algorithm for solving an optimization problem
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_OPTIMIZATIONALGORITHMIMPLEMENTATION_HXX
#define OPENTURNS_OPTIMIZATIONALGORITHMIMPLEMENTATION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/OptimizationProblem.hxx"
#include "openturns/OptimizationResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class OptimizationAlgorithmImplementation
 * OptimizationAlgorithmImplementation implements an algorithm for solving an optimization problem
 */

class OT_API OptimizationAlgorithmImplementation
  : public PersistentObject
{

  CLASSNAME
public:

  /** Default constructor */
  OptimizationAlgorithmImplementation();

  /** Constructor with parameters */
  explicit OptimizationAlgorithmImplementation(const OptimizationProblem & problem);

  /** Virtual constructor */
  virtual OptimizationAlgorithmImplementation * clone() const;

  /** Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
  virtual void run();

  /** Computes the Lagrange multipliers associated with the constraints as a post-processing of the result. Actual algorithms should overload this method. */
  virtual Point computeLagrangeMultipliers(const Point & x) const;

  /** Starting point accessor */
  virtual Point getStartingPoint() const;

  /** Starting point accessor */
  virtual void setStartingPoint(const Point & startingPoint);

  /** Problem accessor */
  virtual OptimizationProblem getProblem() const;
  virtual void setProblem(const OptimizationProblem & problem);

  /** Result accessor */
  virtual OptimizationResult getResult() const;

  /** Result accessor */
  virtual void setResult(const OptimizationResult & result);

  /** Maximum iterations number accessor */
  virtual void setMaximumIterationNumber(const UnsignedInteger maximumIterationNumber);
  virtual UnsignedInteger getMaximumIterationNumber() const;

  /** Maximum evaluations number accessor */
  virtual void setMaximumEvaluationNumber(const UnsignedInteger maximumEvaluationNumber);
  virtual UnsignedInteger getMaximumEvaluationNumber() const;

  /** Maximum absolute error accessor */
  virtual Scalar getMaximumAbsoluteError() const;

  /** Maximum absolute error accessor */
  virtual void setMaximumAbsoluteError(const Scalar maximumAbsoluteError);

  /** Maximum relative error accessor */
  virtual Scalar getMaximumRelativeError() const;

  /** Maximum relative error accessor */
  virtual void setMaximumRelativeError(const Scalar maximumRelativeError);

  /** Maximum residual error accessor */
  virtual Scalar getMaximumResidualError() const;

  /** Maximum residual error accessor */
  virtual void setMaximumResidualError(const Scalar maximumResidualError);

  /** Maximum constraint error accessor */
  virtual Scalar getMaximumConstraintError() const;

  /** Maximum constraint error accessor */
  virtual void setMaximumConstraintError(const Scalar maximumConstraintError);

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

  /** Verbose accessor */
  virtual Bool getVerbose() const;
  virtual void setVerbose(const Bool verbose);

  /** Progress callback */
  typedef void (*ProgressCallback)(Scalar, void * state);
  virtual void setProgressCallback(ProgressCallback callBack, void * state = 0);

  /** Stop callback */
  typedef Bool (*StopCallback)(void * state);
  virtual void setStopCallback(StopCallback callBack, void * state = 0);

protected:
  /** Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
  virtual void checkProblem(const OptimizationProblem & problem) const;

  /** The result of the algorithm */
  OptimizationResult result_;

  // callbacks
  std::pair< ProgressCallback, void *> progressCallback_;
  std::pair< StopCallback, void *> stopCallback_;

private:
  Point startingPoint_;
  OptimizationProblem  problem_;

  /** Number of outermost iterations (in case of nested iterations) */
  UnsignedInteger maximumIterationNumber_;

  /** Maximum function calls */
  UnsignedInteger maximumEvaluationNumber_;

  Scalar maximumAbsoluteError_;    /**< Value of ||x_n - x_{n-1}|| */
  Scalar maximumRelativeError_;    /**< Value of ||x_n - x_{n-1}|| / ||x_n|| */
  Scalar maximumResidualError_;    /**< Value of ||objectiveFunction(x_n) - objectiveFunction(x_{n-1})|| */
  Scalar maximumConstraintError_;  /**< Value of ||constraints(x_n)|| for the active constraints */
  Bool verbose_;

} ; /* class OptimizationAlgorithmImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_OPTIMIZATIONALGORITHMIMPLEMENTATION_HXX */
