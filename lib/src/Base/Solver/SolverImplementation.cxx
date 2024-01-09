//                                               -*- C++ -*-
/**
 *  @brief
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
#include <cstdlib>

#include "openturns/SolverImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Point.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SolverImplementation)

static const Factory<SolverImplementation> Factory_SolverImplementation;

/** Second parameter constructor */
SolverImplementation::SolverImplementation(const Scalar absoluteError,
    const Scalar relativeError,
    const Scalar residualError,
    const UnsignedInteger maximumFunctionEvaluation)
  : PersistentObject()
  , maximumCallsNumber_(maximumFunctionEvaluation)
  , absoluteError_(absoluteError)
  , relativeError_(relativeError)
  , residualError_(residualError)
{
  // Nothing to do
}

/** Virtual constructor */
SolverImplementation * SolverImplementation::clone() const
{
  return new SolverImplementation(*this);
}

/** Comparison operator */
Bool SolverImplementation::operator ==(const SolverImplementation & other) const
{
  if (this == &other) return true;
  return (absoluteError_ == other.absoluteError_) &&
         (relativeError_ == other.relativeError_) &&
         (residualError_ == other.residualError_) &&
         (maximumCallsNumber_ == other.maximumCallsNumber_)
         && (callsNumber_ == other.callsNumber_);
}

/** String converter */
String SolverImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << SolverImplementation::GetClassName()
      << " absoluteError=" << absoluteError_
      << " relativeError=" << relativeError_
      << " residualError=" << residualError_
      << " maximumCallsNumber=" << maximumCallsNumber_
      << " callsNumber=" << callsNumber_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SolverImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("absoluteError_", absoluteError_);
  adv.saveAttribute("relativeError_", relativeError_);
  adv.saveAttribute("residualError_", residualError_);
  adv.saveAttribute("maximumCallsNumber_", maximumCallsNumber_);
}

/* Method load() reloads the object from the StorageManager */
void SolverImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("absoluteError_", absoluteError_);
  adv.loadAttribute("relativeError_", relativeError_);
  adv.loadAttribute("residualError_", residualError_);
  if (adv.hasAttribute("maximumCallsNumber_"))
    adv.loadAttribute("maximumCallsNumber_", maximumCallsNumber_);
  else
    adv.loadAttribute("maximumFunctionEvaluation_", maximumCallsNumber_);
}

/** Absolute error accessor */
void SolverImplementation::setAbsoluteError(const Scalar absoluteError)
{
  absoluteError_ = absoluteError;
}

Scalar SolverImplementation::getAbsoluteError() const
{
  return absoluteError_;
}

/** Relative error accessor */
void SolverImplementation::setRelativeError(const Scalar relativeError)
{
  relativeError_ = relativeError;
}

Scalar SolverImplementation::getRelativeError() const
{
  return relativeError_;
}

/** Residual error accessor */
void SolverImplementation::setResidualError(const Scalar residualError)
{
  residualError_ = residualError;
}

Scalar SolverImplementation::getResidualError() const
{
  return residualError_;
}

/** Maximum function calls accessor */
void SolverImplementation::setMaximumCallsNumber(const UnsignedInteger maximumCallsNumber)
{
  maximumCallsNumber_ = maximumCallsNumber;
}

UnsignedInteger SolverImplementation::getMaximumCallsNumber() const
{
  return maximumCallsNumber_;
}

void SolverImplementation::setMaximumFunctionEvaluation(const UnsignedInteger maximumFunctionEvaluation)
{
  LOGWARN("Solver.setMaximumFunctionEvaluation is deprecated, use setMaximumCallsNumber");
  setMaximumCallsNumber(maximumFunctionEvaluation);
}

UnsignedInteger SolverImplementation::getMaximumFunctionEvaluation() const
{
  LOGWARN("Solver.getMaximumFunctionEvaluation is deprecated, use setMaximumCallsNumber");
  return getMaximumCallsNumber();
}

UnsignedInteger SolverImplementation::getCallsNumber() const
{
  return callsNumber_;
}

UnsignedInteger SolverImplementation::getUsedFunctionEvaluation() const
{
  LOGWARN("Solver.getUsedFunctionEvaluation is deprecated, use getCallsNumber");
  return getCallsNumber();
}

namespace
{
class UniVariateFunctionFromFunction: public UniVariateFunctionImplementation
{
public:
  UniVariateFunctionFromFunction(const Function & function)
    : UniVariateFunctionImplementation()
    , function_(function)
  {
    // Nothing to do
  }

  Scalar operator() (const Scalar x) const
  {
    return function_(Point(1, x))[0];
  }

private:
  const Function & function_;
}; // UniVariateFunctionFromFunction
} // anonymous namespace

/** Solve attempt to find one root to the equation function(x) = value in [infPoint, supPoint] */
Scalar SolverImplementation::solve(const Function & function,
                                   const Scalar value,
                                   const Scalar infPoint,
                                   const Scalar supPoint) const
{
  if ((function.getInputDimension() != 1) || (function.getOutputDimension() != 1)) throw InvalidDimensionException(HERE) << "Error: solver implementation requires a scalar function, here input dimension=" << function.getInputDimension() << " and output dimension=" << function.getOutputDimension();
  return solve(UniVariateFunction(new UniVariateFunctionFromFunction(function)), value, infPoint, supPoint);
}

/** Solve attempt to find one root to the equation function(x) = value in [infPoint, supPoint] given function(infPoint) and function(supPoint) */
Scalar SolverImplementation::solve(const Function & function,
                                   const Scalar value,
                                   const Scalar infPoint,
                                   const Scalar supPoint,
                                   const Scalar infValue,
                                   const Scalar supValue) const
{
  if ((function.getInputDimension() != 1) || (function.getOutputDimension() != 1)) throw InvalidDimensionException(HERE) << "Error: solver implementation requires a scalar function, here input dimension=" << function.getInputDimension() << " and output dimension=" << function.getOutputDimension();
  return solve(UniVariateFunction(new UniVariateFunctionFromFunction(function)), value, infPoint, supPoint, infValue, supValue);
}

/** Solve attempt to find one root to the equation function(x) = value in [infPoint, supPoint] */
Scalar SolverImplementation::solve(const UniVariateFunction & function,
                                   const Scalar value,
                                   const Scalar infPoint,
                                   const Scalar supPoint) const
{
  if (!(maximumCallsNumber_ >= 2)) throw InternalException(HERE) << "Error: solver needs to evaluate the function at least two times, here maximumFunctionEvaluation=" << maximumCallsNumber_;
  /* We take into account the fact that we use 2 function calls when using the other solve method */
  maximumCallsNumber_ -= 2;
  Scalar root = solve(function, value, infPoint, supPoint, function(infPoint), function(supPoint));
  maximumCallsNumber_ += 2;
  callsNumber_ += 2;
  return root;
}

/** Solve attempt to find one root to the equation function(x) = value in [infPoint, supPoint] given function(infPoint) and function(supPoint) */
Scalar SolverImplementation::solve(const UniVariateFunction &,
                                   const Scalar,
                                   const Scalar,
                                   const Scalar,
                                   const Scalar,
                                   const Scalar ) const
{
  throw NotYetImplementedException(HERE) << "In SolverImplementation::solve(const UniVariateFunction & function, const Scalar value, const Scalar infPoint, const Scalar supPoint, const Scalar infValue, const Scalar supValue) const";
}

END_NAMESPACE_OPENTURNS
