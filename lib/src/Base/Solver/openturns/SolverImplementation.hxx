//                                               -*- C++ -*-
/**
 *  @brief Implementation class for a nonlinear scalar solver
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
#ifndef OPENTURNS_SOLVERIMPLEMENTATION_HXX
#define OPENTURNS_SOLVERIMPLEMENTATION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/Function.hxx"
#include "openturns/UniVariateFunction.hxx"
#include "openturns/PersistentObject.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SolverImplementation
 *
 * This class is an interface for the 1D nonlinear solverImplementations
 */
class OT_API SolverImplementation :
  public PersistentObject
{
  CLASSNAME
public:

  /** Parameter constructor */
  explicit SolverImplementation(const Scalar absoluteError = ResourceMap::GetAsScalar("Solver-DefaultAbsoluteError"),
                                const Scalar relativeError = ResourceMap::GetAsScalar("Solver-DefaultRelativeError"),
                                const Scalar residualError = ResourceMap::GetAsScalar("Solver-DefaultResidualError"),
                                const UnsignedInteger maximumFunctionEvaluation = ResourceMap::GetAsUnsignedInteger("Solver-DefaultMaximumFunctionEvaluation"));


  /** Virtual constructor */
  SolverImplementation * clone() const override;

  /** Comparison operator */
  using PersistentObject::operator ==;
  Bool operator ==(const SolverImplementation & other) const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Solve attempt to find one root to the equation function(x) = value in [infPoint, supPoint] */
  virtual Scalar solve(const Function & function,
                       const Scalar value,
                       const Scalar infPoint,
                       const Scalar supPoint) const;

  /** Solve attempt to find one root to the equation function(x) = value in [infPoint, supPoint] given function(infPoint) and function(supPoint) */
  virtual Scalar solve(const Function & function,
                       const Scalar value,
                       const Scalar infPoint,
                       const Scalar supPoint,
                       const Scalar infValue,
                       const Scalar supValue) const;

  /** Solve attempt to find one root to the equation function(x) = value in [infPoint, supPoint] */
  virtual Scalar solve(const UniVariateFunction & function,
                       const Scalar value,
                       const Scalar infPoint,
                       const Scalar supPoint) const;

  /** Solve attempt to find one root to the equation function(x) = value in [infPoint, supPoint] given function(infPoint) and function(supPoint) */
  virtual Scalar solve(const UniVariateFunction & function,
                       const Scalar value,
                       const Scalar infPoint,
                       const Scalar supPoint,
                       const Scalar infValue,
                       const Scalar supValue) const;

  /** Absolute error accessor */
  void setAbsoluteError(const Scalar absoluteError);
  Scalar getAbsoluteError() const;

  /** Relative error accessor */
  void setRelativeError(const Scalar relativeError);
  Scalar getRelativeError() const;

  /** Residual error accessor */
  void setResidualError(const Scalar residualError);
  Scalar getResidualError() const;

  /** Maximum function evaluation accessor */
  void setMaximumFunctionEvaluation(const UnsignedInteger maximumFunctionEvaluation);
  UnsignedInteger getMaximumFunctionEvaluation() const;

  /** Used function evaluation */
  UnsignedInteger getUsedFunctionEvaluation() const;

protected:


  /** Used function evaluation statistics */
  mutable UnsignedInteger maximumFunctionEvaluation_;
  mutable UnsignedInteger usedFunctionEvaluation_;

private:
  Scalar absoluteError_;
  Scalar relativeError_;
  Scalar residualError_;

}; /* Class SolverImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SOLVERIMPLEMENTATION_HXX */
