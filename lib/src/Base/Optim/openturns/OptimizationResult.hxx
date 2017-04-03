//                                               -*- C++ -*-
/**
 *  @brief OptimizationResult implements the result of an algorithm for solving an optimization problem
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
  CLASSNAME;

  // Make the OptimizationAlgorithmImplementation class friend in order to allow them to use protected methods
  friend class OptimizationAlgorithmImplementation;

public:


  /** Default constructor */
  OptimizationResult();


  /** Standard constructor */
  OptimizationResult(const Point & optimalPoint,
                     const Point & optimalValue,
                     const UnsignedInteger iterationNumber,
                     const NumericalScalar absoluteError,
                     const NumericalScalar relativeError,
                     const NumericalScalar residualError,
                     const NumericalScalar constraintError,
                     const OptimizationProblem & problem);

  /** Virtual constructor */
  virtual OptimizationResult * clone() const;

  /** OptimalPoint accessors */
  void setOptimalPoint(const Point & optimalPoint);
  Point getOptimalPoint() const;

  /** Optimal value accessor */
  void setOptimalValue(const Point & optimalValue);
  Point getOptimalValue() const;

  /** Iteration number accessor */
  void setIterationNumber(const UnsignedInteger iterationNumber);
  UnsignedInteger getIterationNumber() const;

  /** Absolute error accessor */
  NumericalScalar getAbsoluteError() const;
  Sample getAbsoluteErrorHistory() const;

  /** Relative error accessor */
  NumericalScalar getRelativeError() const;
  Sample getRelativeErrorHistory() const;

  /** Residual error accessor */
  NumericalScalar getResidualError() const;
  Sample getResidualErrorHistory() const;

  /** Constraint error accessor */
  NumericalScalar getConstraintError() const;
  Sample getConstraintErrorHistory() const;

  /** Input / output sample accessor */
  Sample getInputSample() const;
  Sample getOutputSample() const;

  /** Problem accessor */
  void setProblem(const OptimizationProblem & problem);
  OptimizationProblem getProblem() const;

  /** Lagrange multipliers accessor */
  void setLagrangeMultipliers(const Point & lagrangeMultipliers);
  Point getLagrangeMultipliers() const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

  /** Incremental history storage */
  void store(const Point & inP,
             const Point & outP,
             const NumericalScalar absoluteError,
             const NumericalScalar relativeError,
             const NumericalScalar residualError,
             const NumericalScalar constraintError);

  /** Draw error history graph */
  Graph drawErrorHistory() const;

  /** Draw optimal value graph */
  Graph drawOptimalValueHistory() const;

protected:
  /** Absolute error accessor */
  void setAbsoluteError(const NumericalScalar absoluteError);
  void setAbsoluteErrorHistory(const Sample & absoluteError);

  /** Relative error accessor */
  void setRelativeError(const NumericalScalar relativeError);
  void setRelativeErrorHistory(const Sample & relativeError);

  /** Residual error accessor */
  void setResidualError(const NumericalScalar residualError);
  void setResidualErrorHistory(const Sample & residualError);

  /** Constraint error accessor */
  void setConstraintError(const NumericalScalar constraintError);
  void setConstraintErrorHistory(const Sample & constraintError);

private:

  Point  optimalPoint_;
  Point  optimalValue_;
  UnsignedInteger    iterationNumber_;       /**< Number of outermost iterations (in case of nested iterations) */
  NumericalScalar absoluteError_;   /**< Value of ||x_n - x_{n-1}|| */
  NumericalScalar relativeError_;   /**< Value of ||x_n - x_{n-1}|| / ||x_n|| */
  NumericalScalar residualError_;   /**< Value of ||objectiveFunction(x_n) - objectiveFunction(x_{n-1})|| */
  NumericalScalar constraintError_; /**< Value of ||constraints(x_n)|| for the active constraints */
  Point lagrangeMultipliers_;
  Compact absoluteErrorHistory_;
  Compact relativeErrorHistory_;
  Compact residualErrorHistory_;
  Compact constraintErrorHistory_;
  Compact inputHistory_;
  Compact outputHistory_;
  OptimizationProblem problem_;

}; // class OptimizationResult


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_OPTIMIZATIONRESULT_HXX */

