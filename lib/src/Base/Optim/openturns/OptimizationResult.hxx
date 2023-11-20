//                                               -*- C++ -*-
/**
 *  @brief OptimizationResult implements the result of an algorithm for solving an optimization problem
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
#ifndef OPENTURNS_OPTIMIZATIONRESULT_HXX
#define OPENTURNS_OPTIMIZATIONRESULT_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/Function.hxx"
#include "openturns/Compact.hxx"
#include "openturns/OptimizationProblem.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class OptimizationResult
 * OptimizationResult stores the optimization result
 */
class OT_API OptimizationResult
  : public PersistentObject
{
  CLASSNAME

  // Make the OptimizationAlgorithmImplementation class friend in order to allow them to use protected methods
  friend class OptimizationAlgorithmImplementation;

public:
  typedef Collection<Sample> SampleCollection;

  /** Default constructor */
  OptimizationResult();

  /** Default constructor */
  explicit OptimizationResult(const OptimizationProblem & problem);

  /** Virtual constructor */
  OptimizationResult * clone() const override;

  /** Optimal point accessor */
  void setOptimalPoint(const Point & optimalPoint);
  Point getOptimalPoint() const;

  /** Optimal value accessor */
  void setOptimalValue(const Point & optimalValue);
  Point getOptimalValue() const;

  /** Final points accessor */
  void setFinalPoints(const Sample & finalPoints);
  Sample getFinalPoints() const;

  /** Final values accessor */
  void setFinalValues(const Sample & finalValues);
  Sample getFinalValues() const;

  /** Evaluation number accessor */
  void setEvaluationNumber(const UnsignedInteger evaluationNumber);
  UnsignedInteger getEvaluationNumber() const;

  /** Iteration number accessor */
  void setIterationNumber(const UnsignedInteger iterationNumber);
  UnsignedInteger getIterationNumber() const;

  /** Absolute error accessor */
  Scalar getAbsoluteError() const;
  Sample getAbsoluteErrorHistory() const;

  /** Relative error accessor */
  Scalar getRelativeError() const;
  Sample getRelativeErrorHistory() const;

  /** Residual error accessor */
  Scalar getResidualError() const;
  Sample getResidualErrorHistory() const;

  /** Constraint error accessor */
  Scalar getConstraintError() const;
  Sample getConstraintErrorHistory() const;

  /** Input / output sample accessor */
  Sample getInputSample() const;
  Sample getOutputSample() const;

  /** Problem accessor */
  void setProblem(const OptimizationProblem & problem);
  OptimizationProblem getProblem() const;

  /** Computes the Lagrange multipliers associated with the constraints */
  Point computeLagrangeMultipliers() const;
  Point computeLagrangeMultipliers(const Point & x) const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Incremental history storage */
  void store(const Point & inP,
             const Point & outP,
             const Scalar absoluteError,
             const Scalar relativeError,
             const Scalar residualError,
             const Scalar constraintError, const Scalar maximumConstraintError = 0.0);

  /** Draw error history graph */
  Graph drawErrorHistory() const;

  /** Draw optimal value graph */
  Graph drawOptimalValueHistory() const;

  /** Pareto fronts accessor */
  void setParetoFrontsIndices(const IndicesCollection & indices);
  IndicesCollection getParetoFrontsIndices() const;

  /** Status message accessor */
  void setStatusMessage(const String & statusMessage);
  String getStatusMessage() const;

protected:
  /** Absolute error accessor */
  void setAbsoluteError(const Scalar absoluteError);
  void setAbsoluteErrorHistory(const Sample & absoluteError);

  /** Relative error accessor */
  void setRelativeError(const Scalar relativeError);
  void setRelativeErrorHistory(const Sample & relativeError);

  /** Residual error accessor */
  void setResidualError(const Scalar residualError);
  void setResidualErrorHistory(const Sample & residualError);

  /** Constraint error accessor */
  void setConstraintError(const Scalar constraintError);
  void setConstraintErrorHistory(const Sample & constraintError);

private:
  Point optimalPoint_;
  Point optimalValue_;
  UnsignedInteger evaluationNumber_ = 0; // Number of function evaluations
  UnsignedInteger iterationNumber_ = 0; // Number of outermost iterations (in case of nested iterations)
  Scalar absoluteError_ = -1.0; /**< Value of ||x_n - x_{n-1}|| */
  Scalar relativeError_ = -1.0; /**< Value of ||x_n - x_{n-1}|| / ||x_n|| */
  Scalar residualError_ = -1.0; /**< Value of ||objectiveFunction(x_n) - objectiveFunction(x_{n-1})|| */
  Scalar constraintError_ = -1.0; /**< Value of ||constraints(x_n)|| for the active constraints */
  Compact absoluteErrorHistory_;
  Compact relativeErrorHistory_;
  Compact residualErrorHistory_;
  Compact constraintErrorHistory_;
  Compact inputHistory_;
  Compact outputHistory_;
  OptimizationProblem problem_;

  Sample finalPoints_;
  Sample finalValues_;

  // for multi-objective optimization
  IndicesCollection paretoFrontsIndices_;

  String statusMessage_;

}; // class OptimizationResult


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_OPTIMIZATIONRESULT_HXX */

