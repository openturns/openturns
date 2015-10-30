//                                               -*- C++ -*-
/**
 *  @brief Factory for FisherSnedecor distribution
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
#include "FisherSnedecorFactory.hxx"
#include "MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "CenteredFiniteDifferenceGradient.hxx"
#include "SpecFunc.hxx"
#include "TNC.hxx"
#include "MaximumLikelihoodFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FisherSnedecorFactory);

/* Default constructor */
FisherSnedecorFactory::FisherSnedecorFactory():
  DistributionImplementationFactory()
{
}

/* Virtual constructor */
FisherSnedecorFactory * FisherSnedecorFactory::clone() const
{
  return new FisherSnedecorFactory(*this);
}

/* Here is the interface that all derived class must implement */

FisherSnedecorFactory::Implementation FisherSnedecorFactory::build(const NumericalSample & sample) const
{
  return buildAsFisherSnedecor(sample).clone();
}

FisherSnedecorFactory::Implementation FisherSnedecorFactory::build(const NumericalPoint & parameters) const
{
  return buildAsFisherSnedecor(parameters).clone();
}

FisherSnedecorFactory::Implementation FisherSnedecorFactory::build() const
{
  return buildAsFisherSnedecor().clone();
}


FisherSnedecor FisherSnedecorFactory::buildAsFisherSnedecor(const NumericalSample & sample) const
{
  const UnsignedInteger dimension = build()->getParameterDimension();
  MaximumLikelihoodFactory factory(*this);

  // override starting point
  OptimizationSolver solver(factory.getOptimizationSolver());
  solver.setStartingPoint(NumericalPoint(dimension, 0.0));
  factory.setOptimizationSolver(solver);

  // override bounds
  OptimizationProblem problem;
  NumericalPoint parametersLowerBound;
  parametersLowerBound.add(ResourceMap::GetAsNumericalScalar("FisherSnedecorFactory-D1LowerBound"));
  parametersLowerBound.add(ResourceMap::GetAsNumericalScalar("FisherSnedecorFactory-D2LowerBound"));
  problem.setBounds(Interval(parametersLowerBound, NumericalPoint(dimension, SpecFunc::MaxNumericalScalar), Interval::BoolCollection(dimension, true), Interval::BoolCollection(dimension, false)));
  factory.setOptimizationProblem(problem);

  return buildAsFisherSnedecor(factory.buildParameter(sample));
}

FisherSnedecor FisherSnedecorFactory::buildAsFisherSnedecor(const NumericalPoint & parameters) const
{
  try
  {
    FisherSnedecor distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a FisherSnedecor distribution from the given parameters";
  }
}

FisherSnedecor FisherSnedecorFactory::buildAsFisherSnedecor() const
{
  return FisherSnedecor();
}

/* Optimization solver accessor */
OptimizationSolver FisherSnedecorFactory::getOptimizationSolver() const
{
  return solver_;
}

void FisherSnedecorFactory::setOptimizationSolver(const OptimizationSolver & solver)
{
  solver_ = solver;
}

END_NAMESPACE_OPENTURNS
