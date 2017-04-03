//                                               -*- C++ -*-
/**
 *  @brief Factory for Trapezoidal distribution
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
#include "openturns/TrapezoidalFactory.hxx"
#include <limits>
#include "openturns/Distribution.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Log.hxx"
#include "openturns/MaximumLikelihoodFactory.hxx"
#include "openturns/Cobyla.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TrapezoidalFactory);

static const Factory<TrapezoidalFactory> Factory_TrapezoidalFactory;

/* Default constructor */
TrapezoidalFactory::TrapezoidalFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
TrapezoidalFactory * TrapezoidalFactory::clone() const
{
  return new TrapezoidalFactory(*this);
}

/* Compute the log-likelihood constraint */
NumericalPoint TrapezoidalFactory::computeLogLikelihoodInequalityConstraint(const NumericalPoint & x) const
{
  NumericalPoint result(3, 0.0);
  result[0] = x[1] - x[0] ;                                // x[0] <= x[1]
  result[1] = x[2] - x[1] - SpecFunc::NumericalScalarEpsilon;  // x[1] <  x[2]
  result[2] = x[3] - x[2] ;                                // x[2] <= x[3]
  return result;
}

/* Compute the log-likelihood constraint accessor */
NumericalMathFunction TrapezoidalFactory::getLogLikelihoodInequalityConstraint() const
{
  return bindMethod <TrapezoidalFactory, NumericalPoint, NumericalPoint> (*this, &TrapezoidalFactory::computeLogLikelihoodInequalityConstraint, 4, 3);
}

/* Optimization solver accessor */
OptimizationAlgorithm TrapezoidalFactory::getOptimizationAlgorithm() const
{
  return solver_;
}

void TrapezoidalFactory::setOptimizationAlgorithm(const OptimizationAlgorithm & solver)
{
  solver_ = solver;
}

OptimizationAlgorithm TrapezoidalFactory::getOptimizationSolver() const
{
  Log::Warn(OSS() << "TrapezoidalFactory::getOptimizationSolver is deprecated");
  return getOptimizationAlgorithm();
}

void TrapezoidalFactory::setOptimizationSolver(const OptimizationAlgorithm & solver)
{
  Log::Warn(OSS() << "TrapezoidalFactory::setOptimizationSolver is deprecated");
  setOptimizationAlgorithm(solver);
}


/* Here is the interface that all derived class must implement */

TrapezoidalFactory::Implementation TrapezoidalFactory::build(const NumericalSample & sample) const
{
  return buildAsTrapezoidal(sample).clone();
}

TrapezoidalFactory::Implementation TrapezoidalFactory::build(const NumericalPoint & parameters) const
{
  return buildAsTrapezoidal(parameters).clone();
}

TrapezoidalFactory::Implementation TrapezoidalFactory::build() const
{
  return buildAsTrapezoidal().clone();
}

Trapezoidal TrapezoidalFactory::buildAsTrapezoidal(const NumericalSample & sample) const
{

  const UnsignedInteger size = sample.getSize();

  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: can build a Trapezoidal distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();

  const UnsignedInteger dimension = build()->getParameterDimension();

  // starting point
  NumericalPoint startingPoint(dimension);
  const NumericalScalar min = sample.getMin()[0];
  const NumericalScalar max = sample.getMax()[0];
  if (!SpecFunc::IsNormal(min) || !SpecFunc::IsNormal(max)) throw InvalidArgumentException(HERE) << "Error: cannot build a Trapezoidal distribution if data contains NaN or Inf";
  //  if (max <= min - std::sqrt(SpecFunc::NumericalScalarEpsilon))
  if (min == max)
  {
    const NumericalScalar delta = std::max(std::abs(min), 10.0) * SpecFunc::NumericalScalarEpsilon;
    Trapezoidal result(min - delta, min, max, max + delta);
    result.setDescription(sample.getDescription());
    return result;
  }
  startingPoint[0] = min - std::abs(min) / (2.0 + size);// a
  startingPoint[1] = sample.computeQuantilePerComponent(0.25)[0];// b
  startingPoint[2] = sample.computeQuantilePerComponent(0.75)[0];// c
  startingPoint[3] = max + std::abs(max) / (2.0 + size);// d

  MaximumLikelihoodFactory factory(buildAsTrapezoidal());

  // override starting point
  Cobyla solver;
  solver.setRhoBeg(ResourceMap::GetAsNumericalScalar("TrapezoidalFactory-RhoBeg"));
  solver.setMaximumAbsoluteError(ResourceMap::GetAsNumericalScalar("TrapezoidalFactory-RhoEnd"));
  solver.setMaximumIterationNumber(ResourceMap::GetAsUnsignedInteger("TrapezoidalFactory-MaximumIteration"));
  solver.setStartingPoint(startingPoint);
  factory.setOptimizationAlgorithm(solver);

  // override constraint
  factory.setOptimizationInequalityConstraint(getLogLikelihoodInequalityConstraint());

  Trapezoidal result(buildAsTrapezoidal(factory.buildParameter(sample)));
  result.setDescription(sample.getDescription());
  return result;
}

Trapezoidal TrapezoidalFactory::buildAsTrapezoidal(const NumericalPoint & parameters) const
{
  try
  {
    Trapezoidal distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Trapezoidal distribution from the given parameters: " << parameters;
  }
}

Trapezoidal TrapezoidalFactory::buildAsTrapezoidal() const
{
  return Trapezoidal();
}

END_NAMESPACE_OPENTURNS
