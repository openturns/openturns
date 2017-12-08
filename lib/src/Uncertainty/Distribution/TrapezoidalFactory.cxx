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
#include "openturns/SymbolicFunction.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Log.hxx"
#include "openturns/MaximumLikelihoodFactory.hxx"
#include "openturns/Cobyla.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TrapezoidalFactory)

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

/* Optimization solver accessor */
OptimizationAlgorithm TrapezoidalFactory::getOptimizationAlgorithm() const
{
  return solver_;
}

void TrapezoidalFactory::setOptimizationAlgorithm(const OptimizationAlgorithm & solver)
{
  solver_ = solver;
}

/* Here is the interface that all derived class must implement */

TrapezoidalFactory::Implementation TrapezoidalFactory::build(const Sample & sample) const
{
  return buildAsTrapezoidal(sample).clone();
}

TrapezoidalFactory::Implementation TrapezoidalFactory::build(const Point & parameters) const
{
  return buildAsTrapezoidal(parameters).clone();
}

TrapezoidalFactory::Implementation TrapezoidalFactory::build() const
{
  return buildAsTrapezoidal().clone();
}

Trapezoidal TrapezoidalFactory::buildAsTrapezoidal(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();

  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: can build a Trapezoidal distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();

  // starting point
  Point startingPoint(4);
  const Scalar min = sample.getMin()[0];
  const Scalar max = sample.getMax()[0];
  if (!SpecFunc::IsNormal(min) || !SpecFunc::IsNormal(max)) throw InvalidArgumentException(HERE) << "Error: cannot build a Trapezoidal distribution if data contains NaN or Inf";
  //  if (max <= min - std::sqrt(SpecFunc::ScalarEpsilon))
  if (min == max)
  {
    const Scalar delta = std::max(std::abs(min), 10.0) * SpecFunc::ScalarEpsilon;
    Trapezoidal result(min - delta, min, max, max + delta);
    result.setDescription(sample.getDescription());
    return result;
  }
  startingPoint[0] = min - std::abs(min) / (2.0 + size);// a
  startingPoint[1] = sample.computeQuantilePerComponent(0.25)[0];// b
  startingPoint[2] = sample.computeQuantilePerComponent(0.75)[0];// c
  startingPoint[3] = max + std::abs(max) / (2.0 + size);// d

  MaximumLikelihoodFactory factory(buildAsTrapezoidal());

  // override solver
  Cobyla solver;
  solver.setRhoBeg(ResourceMap::GetAsScalar("TrapezoidalFactory-RhoBeg"));
  solver.setMaximumAbsoluteError(ResourceMap::GetAsScalar("TrapezoidalFactory-RhoEnd"));
  solver.setMaximumIterationNumber(ResourceMap::GetAsUnsignedInteger("TrapezoidalFactory-MaximumIteration"));
  solver.setStartingPoint(startingPoint);
  factory.setOptimizationAlgorithm(solver);

  // override constraint
  Description input(4);
  input[0] = "a";
  input[1] = "b";
  input[2] = "c";
  input[3] = "d";
  Description formula(3);
  formula[0] = "b - a";// a <= b
  formula[1] = OSS() << "c - b - " << SpecFunc::ScalarEpsilon;// b < c
  formula[2] = "d - c";// c <= d
  SymbolicFunction constraint(input, formula);
  factory.setOptimizationInequalityConstraint(constraint);

  Trapezoidal result(buildAsTrapezoidal(factory.buildParameter(sample)));
  result.setDescription(sample.getDescription());
  return result;
}

Trapezoidal TrapezoidalFactory::buildAsTrapezoidal(const Point & parameters) const
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
