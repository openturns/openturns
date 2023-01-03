//                                               -*- C++ -*-
/**
 *  @brief Interface class for a nonlinear scalar solver
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SOLVER_HXX
#define OPENTURNS_SOLVER_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/SolverImplementation.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/TypedInterfaceObject.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Solver
 *
 * This class is an interface for the 1D nonlinear solvers
 */
class OT_API Solver :
  public TypedInterfaceObject<SolverImplementation>
{
  CLASSNAME
public:

  typedef Pointer<SolverImplementation>           Implementation;

  /** Default constructor */
  Solver();

  /** Parameter constructor */
  Solver(const Implementation & p_implementation);

  /** Parameter from an implementation */
  Solver(const SolverImplementation & implementation);

  /** Second parameter constructor */
  Solver(const Scalar absoluteError,
         const Scalar relativeError,
         const Scalar residualError,
         const UnsignedInteger maximumFunctionEvaluation);


  /** Comparison operator */
  Bool operator ==(const Solver & other) const;

  /** String converter */
  String __repr__() const override;

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

}; /* Class Solver */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SOLVER_HXX */
