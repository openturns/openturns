//                                               -*- C++ -*-
/**
 *  @brief OptimizationSolverImplementation implements an algorithm for
 *         finding the point of an implicitely defined manifold the nearest
 *         to the origin.
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
#ifndef OPENTURNS_OPTIMIZATIONSOLVERIMPLEMENTATION_HXX
#define OPENTURNS_OPTIMIZATIONSOLVERIMPLEMENTATION_HXX

#include "OTprivate.hxx"
#include "PersistentObject.hxx"
#include "OptimizationProblem.hxx"
#include "OptimizationSolverImplementationResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class OptimizationSolverImplementation
 * OptimizationSolverImplementation implements an algorithm for solving an optimization problem
 */

class OptimizationSolverImplementation
  : public PersistentObject
{

  CLASSNAME;
public:

  /** Default constructor */
  OptimizationSolverImplementation();

  /** Constructor with parameters */
  explicit OptimizationSolverImplementation(const NumericalMathFunction & levelFunction,
      const Bool verbose = false);


  /** Virtual constructor */
  virtual OptimizationSolverImplementation * clone() const;

  /** Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
  virtual void run();

  /** Starting point accessor */
  NumericalPoint getStartingPoint() const;

  /** Starting point accessor */
  void setStartingPoint(const NumericalPoint & startingPoint);

  /** Level function accessor */
  NumericalMathFunction getLevelFunction() const;

  /** Level function accessor */
  void setLevelFunction(const NumericalMathFunction & levelFunction);

  /** Level value accessor */
  NumericalScalar getLevelValue() const;

  /** Level value accessor */
  void setLevelValue(const NumericalScalar levelValue);

  /** Result accessor */
  Result getResult() const;

  /** Result accessor */
  void setResult(const Result & result);

  /** Maximum iterations number accessor */
  UnsignedInteger getMaximumIterationsNumber() const;

  /** Maximum iterations number accessor */
  void setMaximumIterationsNumber(const UnsignedInteger maximumIterationsNumber);

  /** Maximum absolute error accessor */
  NumericalScalar getMaximumAbsoluteError() const;

  /** Maximum absolute error accessor */
  void setMaximumAbsoluteError(const NumericalScalar maximumAbsoluteError);

  /** Maximum relative error accessor */
  NumericalScalar getMaximumRelativeError() const;

  /** Maximum relative error accessor */
  void setMaximumRelativeError(const NumericalScalar maximumRelativeError);

  /** Maximum residual error accessor */
  NumericalScalar getMaximumResidualError() const;

  /** Maximum residual error accessor */
  void setMaximumResidualError(const NumericalScalar maximumResidualError);

  /** Maximum constraint error accessor */
  NumericalScalar getMaximumConstraintError() const;

  /** Maximum constraint error accessor */
  void setMaximumConstraintError(const NumericalScalar maximumConstraintError);

  /** String converter */
  String __repr__() const;

  /** Verbose accessor */
  Bool getVerbose() const;
  void setVerbose(const Bool verbose);

protected:
  Result result_;

private:
  NumericalMathFunction levelFunction_;
  NumericalPoint startingPoint_;
  NumericalScalar levelValue_;
  UnsignedInteger    maximumIterationsNumber_; /**< Number of outermost iterations (in case of nested iterations) */
  NumericalScalar maximumAbsoluteError_;    /**< Value of ||x_n - x_{n-1}|| */
  NumericalScalar maximumRelativeError_;    /**< Value of ||x_n - x_{n-1}|| / ||x_n|| */
  NumericalScalar maximumResidualError_;    /**< Value of ||objectiveFunction(x_n) - objectiveFunction(x_{n-1})|| */
  NumericalScalar maximumConstraintError_;  /**< Value of ||constraints(x_n)|| for the active constraints */
  Bool verbose_;

} ; /* class OptimizationSolverImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_OPTIMIZATIONSOLVERIMPLEMENTATION_HXX */
