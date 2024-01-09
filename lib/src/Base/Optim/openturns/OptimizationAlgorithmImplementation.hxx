//                                               -*- C++ -*-
/**
 *  @brief OptimizationAlgorithmImplementation implements an algorithm for solving an optimization problem
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
  OptimizationAlgorithmImplementation * clone() const override;

  /** Performs the actual computation */
  virtual void run();

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

  /** Maximum calls number accessor */
  virtual void setMaximumCallsNumber(const UnsignedInteger maximumCallsNumber);
  virtual UnsignedInteger getMaximumCallsNumber() const;
  // @deprecated
  virtual void setMaximumEvaluationNumber(const UnsignedInteger maximumEvaluationNumber);
  virtual UnsignedInteger getMaximumEvaluationNumber() const;

  /** Maximum absolute error accessor */
  virtual Scalar getMaximumAbsoluteError() const;
  virtual void setMaximumAbsoluteError(const Scalar maximumAbsoluteError);

  /** Maximum relative error accessor */
  virtual Scalar getMaximumRelativeError() const;
  virtual void setMaximumRelativeError(const Scalar maximumRelativeError);

  /** Maximum residual error accessor */
  virtual Scalar getMaximumResidualError() const;
  virtual void setMaximumResidualError(const Scalar maximumResidualError);

  /** Maximum constraint error accessor */
  virtual Scalar getMaximumConstraintError() const;
  virtual void setMaximumConstraintError(const Scalar maximumConstraintError);

  /** Maximum time accessor */
  virtual void setMaximumTimeDuration(const Scalar maximumTime);
  virtual Scalar getMaximumTimeDuration() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Progress callback */
  typedef void (*ProgressCallback)(Scalar, void * state);
  virtual void setProgressCallback(ProgressCallback callBack, void * state = 0);

  /** Stop callback */
  typedef Bool (*StopCallback)(void * state);
  virtual void setStopCallback(StopCallback callBack, void * state = 0);

  /** Check status accessor */
  void setCheckStatus(const Bool checkStatus);
  Bool getCheckStatus() const;

protected:
  /** Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
  virtual void checkProblem(const OptimizationProblem & problem) const;

  /** The result of the algorithm */
  OptimizationResult result_;

  // callbacks
  std::pair< ProgressCallback, void *> progressCallback_;
  std::pair< StopCallback, void *> stopCallback_;

  // set the result from evaluation history
  void setResultFromEvaluationHistory(const Sample & inputHistory, const Sample & outputHistory,
                                      const Sample & inequalityHistory = Sample(), const Sample & equalityHistory = Sample());

private:
  Point startingPoint_;
  OptimizationProblem problem_;

  /** Number of outermost iterations (in case of nested iterations) */
  UnsignedInteger maximumIterationNumber_ = 0.0;

  /** Maximum function calls */
  UnsignedInteger maximumCallsNumber_ = 0.0;

  Scalar maximumTimeDuration_ = -1.0;

  Scalar maximumAbsoluteError_ = 0.0;    /**< Value of ||x_n - x_{n-1}|| */
  Scalar maximumRelativeError_ = 0.0;    /**< Value of ||x_n - x_{n-1}|| / ||x_n|| */
  Scalar maximumResidualError_ = 0.0;    /**< Value of ||objectiveFunction(x_n) - objectiveFunction(x_{n-1})|| */
  Scalar maximumConstraintError_ = 0.0;  /**< Value of ||constraints(x_n)|| for the active constraints */

  /* Whether to check if optimization has converged or not */
  Bool checkStatus_ = true;

} ; /* class OptimizationAlgorithmImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_OPTIMIZATIONALGORITHMIMPLEMENTATION_HXX */
