//                                               -*- C++ -*-
/**
 *  @brief Interface class for a nonlinear scalar solver
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
#include "openturns/Solver.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Solver)

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
Solver::Solver(const Scalar absoluteError,
               const Scalar relativeError,
               const Scalar residualError,
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
Scalar Solver::solve(const Function & function,
                     const Scalar value,
                     const Scalar infPoint,
                     const Scalar supPoint) const
{
  return getImplementation()->solve(function, value, infPoint, supPoint);
}

/* Solve attempt to find one root to the equation function(x) = value in [infPoint, supPoint] given function(infPoint) and function(supPoint) */
Scalar Solver::solve(const Function & function,
                     const Scalar value,
                     const Scalar infPoint,
                     const Scalar supPoint,
                     const Scalar infValue,
                     const Scalar supValue) const
{
  return getImplementation()->solve(function, value, infPoint, supPoint, infValue, supValue);
}

/* Absolute error accessor */
void Solver::setAbsoluteError(const Scalar absoluteError)
{
  copyOnWrite();
  getImplementation()->setAbsoluteError(absoluteError);
}

Scalar Solver::getAbsoluteError() const
{
  return getImplementation()->getAbsoluteError();
}

/* Relative error accessor */
void Solver::setRelativeError(const Scalar relativeError)
{
  copyOnWrite();
  getImplementation()->setRelativeError(relativeError);
}

Scalar Solver::getRelativeError() const
{
  return getImplementation()->getRelativeError();
}

/* Residual error accessor */
void Solver::setResidualError(const Scalar residualError)
{
  copyOnWrite();
  getImplementation()->setResidualError(residualError);
}

Scalar Solver::getResidualError() const
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
