//                                               -*- C++ -*-
/**
 *  @brief GenericSimulationResult is a handle to any concrete
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
#include "openturns/GenericSimulationResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GenericSimulationResult
 */

CLASSNAMEINIT(GenericSimulationResult)

/* Default constructor */
GenericSimulationResult::GenericSimulationResult()
  : TypedInterfaceObject<ProbabilitySimulationResult>()
{
  // Nothing to do
}

/* Constructor from a concrete result: clone for independence */
GenericSimulationResult::GenericSimulationResult(const ProbabilitySimulationResult & result)
  : TypedInterfaceObject<ProbabilitySimulationResult>(Pointer<ProbabilitySimulationResult>(result.clone()))
{
  // Nothing to do
}

/* Constructor from implementation */
GenericSimulationResult::GenericSimulationResult(const Implementation & p_implementation)
  : TypedInterfaceObject<ProbabilitySimulationResult>(p_implementation)
{
  // Nothing to do
}

#ifndef SWIG
/* Constructor from implementation pointer */
GenericSimulationResult::GenericSimulationResult(ProbabilitySimulationResult * p_implementation)
  : TypedInterfaceObject<ProbabilitySimulationResult>(p_implementation)
{
  // Nothing to do
}
#endif

/* String converter */
String GenericSimulationResult::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " wrapped=" << getImplementation()->__repr__();
  return oss;
}

String GenericSimulationResult::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(";
  oss << getImplementation()->__str__(offset) << ")";
  return oss;
}

/* Probability estimate accessor */
Scalar GenericSimulationResult::getProbabilityEstimate() const
{
  return getImplementation()->getProbabilityEstimate();
}

/* Variance estimate accessor */
Scalar GenericSimulationResult::getVarianceEstimate() const
{
  return getImplementation()->getVarianceEstimate();
}

/* Coefficient of variation estimate accessor */
Scalar GenericSimulationResult::getCoefficientOfVariation() const
{
  return getImplementation()->getCoefficientOfVariation();
}

/* Standard deviation estimate accessor */
Scalar GenericSimulationResult::getStandardDeviation() const
{
  return getImplementation()->getStandardDeviation();
}

/* Outer sampling accessor */
UnsignedInteger GenericSimulationResult::getOuterSampling() const
{
  return getImplementation()->getOuterSampling();
}

/* Block size accessor */
UnsignedInteger GenericSimulationResult::getBlockSize() const
{
  return getImplementation()->getBlockSize();
}

/* Event accessor */
RandomVector GenericSimulationResult::getEvent() const
{
  return getImplementation()->getEvent();
}

END_NAMESPACE_OPENTURNS
