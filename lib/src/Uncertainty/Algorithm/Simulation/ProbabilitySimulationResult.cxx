//                                               -*- C++ -*-
/**
 *  @brief ProbabilitySimulationResult is a handle to any concrete
 *  simulation result, preserving its dynamic type
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ProbabilitySimulationResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ProbabilitySimulationResult
 */

CLASSNAMEINIT(ProbabilitySimulationResult)

/* Default constructor */
ProbabilitySimulationResult::ProbabilitySimulationResult()
  : TypedInterfaceObject<ProbabilitySimulationResultImplementation>(new ProbabilitySimulationResultImplementation())
{
  // Nothing to do
}

/* Constructor from a concrete result: clone for independence */
ProbabilitySimulationResult::ProbabilitySimulationResult(const ProbabilitySimulationResultImplementation & result)
  : TypedInterfaceObject<ProbabilitySimulationResultImplementation>(Pointer<ProbabilitySimulationResultImplementation>(result.clone()))
{
  // Nothing to do
}

/* Constructor from implementation */
ProbabilitySimulationResult::ProbabilitySimulationResult(const Implementation & p_implementation)
  : TypedInterfaceObject<ProbabilitySimulationResultImplementation>(p_implementation)
{
  // Nothing to do
}

#ifndef SWIG
/* Constructor from implementation pointer */
ProbabilitySimulationResult::ProbabilitySimulationResult(ProbabilitySimulationResultImplementation * p_implementation)
  : TypedInterfaceObject<ProbabilitySimulationResultImplementation>(p_implementation)
{
  // Nothing to do
}
#endif

/* String converter */
String ProbabilitySimulationResult::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName();
  if (getImplementation())
    oss << " wrapped=" << getImplementation()->__repr__();
  else
    oss << " wrapped=null";
  return oss;
}

String ProbabilitySimulationResult::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(";
  if (getImplementation())
    oss << getImplementation()->__str__(offset);
  else
    oss << "null";
  oss << ")";
  return oss;
}

/* Probability estimate accessor */
Scalar ProbabilitySimulationResult::getProbabilityEstimate() const
{
  return getImplementation()->getProbabilityEstimate();
}

/* Variance estimate accessor */
Scalar ProbabilitySimulationResult::getVarianceEstimate() const
{
  return getImplementation()->getVarianceEstimate();
}

/* Coefficient of variation estimate accessor */
Scalar ProbabilitySimulationResult::getCoefficientOfVariation() const
{
  return getImplementation()->getCoefficientOfVariation();
}

/* Standard deviation estimate accessor */
Scalar ProbabilitySimulationResult::getStandardDeviation() const
{
  return getImplementation()->getStandardDeviation();
}

/* Outer sampling accessor */
UnsignedInteger ProbabilitySimulationResult::getOuterSampling() const
{
  return getImplementation()->getOuterSampling();
}

/* Block size accessor */
UnsignedInteger ProbabilitySimulationResult::getBlockSize() const
{
  return getImplementation()->getBlockSize();
}

/* Event accessor */
RandomVector ProbabilitySimulationResult::getEvent() const
{
  return getImplementation()->getEvent();
}

END_NAMESPACE_OPENTURNS
