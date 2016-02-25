//                                               -*- C++ -*-
/**
 *  @brief OptimizationResult implements the result of an algorithm for solving an optimization problem
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/NumericalMathFunction.hxx"
#include "openturns/Compact.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class OptimizationResult
 * OptimizationResult stores the optimization result
 */
class OT_API OptimizationResult
  : public PersistentObject
{
  CLASSNAME;

public:


  /** Default constructor */
  OptimizationResult();


  /** Standard constructor */
  OptimizationResult(const NumericalPoint & optimalPoint,
      const NumericalPoint & optimalValue,
      const UnsignedInteger iterationNumber,
      const NumericalScalar absoluteError,
      const NumericalScalar relativeError,
      const NumericalScalar residualError,
      const NumericalScalar constraintError);

  /** Virtual constructor */
  virtual OptimizationResult * clone() const;

  /** OptimalPoint accessors */
  NumericalPoint getOptimalPoint() const;

  /** Optimal value accessor */
  NumericalPoint getOptimalValue() const;

  /** Iteration number accessor */
  UnsignedInteger getIterationNumber() const;

  /** Absolute error accessor */
  NumericalScalar getAbsoluteError() const;
  NumericalSample getAbsoluteErrorHistory() const;

  /** Relative error accessor */
  NumericalScalar getRelativeError() const;
  NumericalSample getRelativeErrorHistory() const;

  /** Residual error accessor */
  NumericalScalar getResidualError() const;
  NumericalSample getResidualErrorHistory() const;

  /** Constraint error accessor */
  NumericalScalar getConstraintError() const;
  NumericalSample getConstraintErrorHistory() const;

  /** Input / output sample accessor */
  NumericalSample getInputSample() const;
  NumericalSample getOutputSample() const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

  /** Update current state */
  void update(const NumericalPoint & optimalPoint, UnsignedInteger iterationNumber);

  /** Incremental history storage */
  void store(const NumericalPoint & inP,
             const NumericalPoint & outP,
             const NumericalScalar absoluteError,
             const NumericalScalar relativeError,
             const NumericalScalar residualError,
             const NumericalScalar constraintError);

  /** Draw error history graph */
  Graph drawErrorHistory() const;

protected:

  /** OptimalPoint accessors */
  void setOptimalPoint(const NumericalPoint & optimalPoint);

  /** Iteration number accessor */
  void setIterationNumber(const UnsignedInteger iterationNumber);

  /** Optimal value accessor */
  void setOptimalValue(const NumericalPoint & optimalValue);

  /** Absolute error accessor */
  void setAbsoluteError(const NumericalScalar absoluteError);
  void setAbsoluteErrorHistory(const NumericalSample & absoluteError);

  /** Relative error accessor */
  void setRelativeError(const NumericalScalar relativeError);
  void setRelativeErrorHistory(const NumericalSample & relativeError);

  /** Residual error accessor */
  void setResidualError(const NumericalScalar residualError);
  void setResidualErrorHistory(const NumericalSample & residualError);

  /** Constraint error accessor */
  void setConstraintError(const NumericalScalar constraintError);
  void setConstraintErrorHistory(const NumericalSample & constraintError);


private:

  NumericalPoint  optimalPoint_;
  NumericalPoint  optimalValue_;
  UnsignedInteger    iterationNumber_;       /**< Number of outermost iterations (in case of nested iterations) */
  NumericalScalar absoluteError_;   /**< Value of ||x_n - x_{n-1}|| */
  NumericalScalar relativeError_;   /**< Value of ||x_n - x_{n-1}|| / ||x_n|| */
  NumericalScalar residualError_;   /**< Value of ||objectiveFunction(x_n) - objectiveFunction(x_{n-1})|| */
  NumericalScalar constraintError_; /**< Value of ||constraints(x_n)|| for the active constraints */
  Compact absoluteErrorHistory_;
  Compact relativeErrorHistory_;
  Compact residualErrorHistory_;
  Compact constraintErrorHistory_;
  Compact inputHistory_;
  Compact outputHistory_;

}; // class OptimizationResult


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_OPTIMIZATIONRESULT_HXX */
