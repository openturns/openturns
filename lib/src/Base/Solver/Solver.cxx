//                                               -*- C++ -*-
/**
 *  @brief Interface class for a nonlinear scalar solver
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
#include "Solver.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Solver);

/* Parameter constructor */
Solver::Solver(const Implementation & p_implementation)
  : TypedInterfaceObject<SolverImplementation>(p_implementation)
{
  // Nothing to do
}

/* Parameter constructor */
Solver::Solver(const SolverImplementation & implementation)
  : TypedInterfaceObject<SolverImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Default constructor */
Solver::Solver():
  TypedInterfaceObject<SolverImplementation>(new SolverImplementation())
{
  // Nothing to do
}

/* Second parameter constructor */
Solver::Solver(const NumericalScalar absoluteError,
               const NumericalScalar relativeError,
               const NumericalScalar residualError,
               const UnsignedInteger maximumFunctionEvaluation):
  TypedInterfaceObject<SolverImplementation>(new SolverImplementation(absoluteError, relativeError, residualError, maximumFunctionEvaluation))
{
  // Nothing to do
}

/* Comparison operator */
Bool Solver::operator ==(const Solver & other) const
{
  return (*getImplementation()) == (*other.getImplementation());
}

/* String converter */
String Solver::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* Solve attempt to find one root to the equation function(x) = value in [infPoint, supPoint] */
NumericalScalar Solver::solve(const NumericalMathFunction & function,
                              const NumericalScalar value,
                              const NumericalScalar infPoint,
                              const NumericalScalar supPoint) const
{
  return getImplementation()->solve(function, value, infPoint, supPoint);
}

/* Solve attempt to find one root to the equation function(x) = value in [infPoint, supPoint] given function(infPoint) and function(supPoint) */
NumericalScalar Solver::solve(const NumericalMathFunction & function,
                              const NumericalScalar value,
                              const NumericalScalar infPoint,
                              const NumericalScalar supPoint,
                              const NumericalScalar infValue,
                              const NumericalScalar supValue) const
{
  return getImplementation()->solve(function, value, infPoint, supPoint, infValue, supValue);
}

/* Absolute error accessor */
void Solver::setAbsoluteError(const NumericalScalar absoluteError)
{
  copyOnWrite();
  getImplementation()->setAbsoluteError(absoluteError);
}

NumericalScalar Solver::getAbsoluteError() const
{
  return getImplementation()->getAbsoluteError();
}

/* Relative error accessor */
void Solver::setRelativeError(const NumericalScalar relativeError)
{
  copyOnWrite();
  getImplementation()->setRelativeError(relativeError);
}

NumericalScalar Solver::getRelativeError() const
{
  return getImplementation()->getRelativeError();
}

/* Residual error accessor */
void Solver::setResidualError(const NumericalScalar residualError)
{
  copyOnWrite();
  getImplementation()->setResidualError(residualError);
}

NumericalScalar Solver::getResidualError() const
{
  return getImplementation()->getResidualError();
}

/* Maximum function evaluation accessor */
void Solver::setMaximumFunctionEvaluation(const UnsignedInteger maximumFunctionEvaluation)
{
  copyOnWrite();
  getImplementation()->setMaximumFunctionEvaluation(maximumFunctionEvaluation);
}

UnsignedInteger Solver::getMaximumFunctionEvaluation() const
{
  return getImplementation()->getMaximumFunctionEvaluation();
}

UnsignedInteger Solver::getUsedFunctionEvaluation() const
{
  return getImplementation()->getUsedFunctionEvaluation();
}


END_NAMESPACE_OPENTURNS
