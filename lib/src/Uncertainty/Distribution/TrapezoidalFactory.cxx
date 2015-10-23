//                                               -*- C++ -*-
/**
 *  @brief Factory for Trapezoidal distribution
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "TrapezoidalFactory.hxx"
#include <limits>
#include "Distribution.hxx"
#include "SpecFunc.hxx"
#include "ResourceMap.hxx"
#include "algocobyla.h"
#include "Log.hxx"
#include "OptimizationSolver.hxx"
#include "Cobyla.hxx"
#include "MethodBoundNumericalMathEvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TrapezoidalFactory);

/* Default constructor */
TrapezoidalFactory::TrapezoidalFactory()
  : DistributionImplementationFactory()
  , solver_(new Cobyla())
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
  static_cast<Cobyla *>(solver_.getImplementation().get())->setSpecificParameters(CobylaSpecificParameters(ResourceMap::GetAsNumericalScalar("TrapezoidalFactory-RhoBeg")));
  solver_.setMaximumAbsoluteError(ResourceMap::GetAsNumericalScalar("TrapezoidalFactory-RhoEnd"));
  solver_.setMaximumIterationsNumber(ResourceMap::GetAsUnsignedInteger("TrapezoidalFactory-MaximumIteration"));
}

/* Virtual constructor */
TrapezoidalFactory * TrapezoidalFactory::clone() const
{
  return new TrapezoidalFactory(*this);
}

/* Compute the log-likelihood function */
NumericalScalar TrapezoidalFactory::computeLogLikelihood(const NumericalPoint & x) const
{
  NumericalScalar result(0.0);
  const UnsignedInteger size(sample_.getSize());
  const Trapezoidal distribution(buildAsTrapezoidal(x));
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const NumericalScalar pdf(distribution.computePDF(sample_[i]));
    if ( pdf > 0.0 ) result += std::log(pdf);
    else result += SpecFunc::LogMinNumericalScalar;
  }
  return result;
}

/* Compute the log-likelihood constraint */
NumericalPoint TrapezoidalFactory::computeLogLikelihoodInequalityConstraint(const NumericalPoint & x) const
{
  NumericalPoint result(3,0.0);
  result[0] = x[1] - x[0] ;                                // x[0] <= x[1]
  result[1] = x[2] - x[1] - SpecFunc::MinNumericalScalar;  // x[1] <  x[2]
  result[2] = x[3] - x[2] ;                                // x[3] <= x[2]
  return result;
}

/* Compute the log-likelihood function accessor */
NumericalMathFunction TrapezoidalFactory::getLogLikelihoodFunction() const
{
  return bindMethod <TrapezoidalFactory, NumericalScalar, NumericalPoint> ( *this, &TrapezoidalFactory::computeLogLikelihood, 4, 1);
}

/* Compute the log-likelihood constraint accessor */
NumericalMathFunction TrapezoidalFactory::getLogLikelihoodInequalityConstraint() const
{
  return bindMethod <TrapezoidalFactory, NumericalPoint, NumericalPoint> ( *this, &TrapezoidalFactory::computeLogLikelihoodInequalityConstraint, 4, 3);
}

/* Optimization solver accessor */
OptimizationSolver TrapezoidalFactory::getOptimizationSolver() const
{
  return solver_;
}

void TrapezoidalFactory::setOptimizationSolver(const OptimizationSolver & solver)
{
  solver_ = solver;
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
  const UnsignedInteger size(sample.getSize());
  if (size == 0)
    throw InvalidArgumentException(HERE) << "Error: cannot build a Trapezoidal distribution from an empty sample";

  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: can build a Trapezoidal distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();

  // starting point
  NumericalPoint x(4);

  const NumericalScalar min(sample.getMin()[0]);
  const NumericalScalar max(sample.getMax()[0]);
  x[0] = min - std::abs( min ) / ( 2.0 + size );// a
  x[1] = sample.computeQuantilePerComponent( 0.25 )[0];// b
  x[2] = sample.computeQuantilePerComponent( 0.75 )[0];// c
  x[3] = max + std::abs( max ) / ( 2.0 + size );// d

  // use attributes to pass the data
  sample_ = sample;

  // Define Optimization problem 
  OptimizationProblem problem;
  problem.setObjective(getLogLikelihoodFunction());
  problem.setInequalityConstraint(getLogLikelihoodInequalityConstraint());
  problem.setMinimization(false);
  solver_.setProblem(problem);
  solver_.setStartingPoint(x);

  // run Optimization problem
  solver_.run();

  // optimal point
  const NumericalPoint optpoint(solver_.getResult().getOptimalPoint());
  Trapezoidal result(optpoint[0], optpoint[1], optpoint[2], optpoint[3]);

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
