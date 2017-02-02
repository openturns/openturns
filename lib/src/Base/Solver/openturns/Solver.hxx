//                                               -*- C++ -*-
/**
 *  @brief Interface class for a nonlinear scalar solver
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
  CLASSNAME;
public:

  typedef Pointer<SolverImplementation>           Implementation;

  /** Default constructor */
  Solver();

  /** Parameter constructor */
  Solver(const Implementation & p_implementation);

  /** Parameter from an implementation */
  Solver(const SolverImplementation & implementation);

  /** Second parameter constructor */
  Solver(const NumericalScalar absoluteError,
         const NumericalScalar relativeError,
         const NumericalScalar residualError,
         const UnsignedInteger maximumFunctionEvaluation);


  /** Comparison operator */
  Bool operator ==(const Solver & other) const;

  /** String converter */
  virtual String __repr__() const;

  /** Solve attempt to find one root to the equation function(x) = value in [infPoint, supPoint] */
  virtual NumericalScalar solve(const NumericalMathFunction & function,
                                const NumericalScalar value,
                                const NumericalScalar infPoint,
                                const NumericalScalar supPoint) const;

  /** Solve attempt to find one root to the equation function(x) = value in [infPoint, supPoint] given function(infPoint) and function(supPoint) */
  virtual NumericalScalar solve(const NumericalMathFunction & function,
                                const NumericalScalar value,
                                const NumericalScalar infPoint,
                                const NumericalScalar supPoint,
                                const NumericalScalar infValue,
                                const NumericalScalar supValue) const;

  /** Absolute error accessor */
  void setAbsoluteError(const NumericalScalar absoluteError);
  NumericalScalar getAbsoluteError() const;

  /** Relative error accessor */
  void setRelativeError(const NumericalScalar relativeError);
  NumericalScalar getRelativeError() const;

  /** Residual error accessor */
  void setResidualError(const NumericalScalar residualError);
  NumericalScalar getResidualError() const;

  /** Maximum function evaluation accessor */
  void setMaximumFunctionEvaluation(const UnsignedInteger maximumFunctionEvaluation);
  UnsignedInteger getMaximumFunctionEvaluation() const;

  /** Used function evaluation */
  UnsignedInteger getUsedFunctionEvaluation() const;

}; /* Class Solver */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SOLVER_HXX */
