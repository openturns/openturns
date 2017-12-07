//                                               -*- C++ -*-
/**
 *  @brief Factory for FisherSnedecor distribution
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
#include "openturns/FisherSnedecorFactory.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/CenteredFiniteDifferenceGradient.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/TNC.hxx"
#include "openturns/MaximumLikelihoodFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FisherSnedecorFactory)

static const Factory<FisherSnedecorFactory> Factory_FisherSnedecorFactory;

/* Default constructor */
FisherSnedecorFactory::FisherSnedecorFactory():
  DistributionFactoryImplementation()
{
}

/* Virtual constructor */
FisherSnedecorFactory * FisherSnedecorFactory::clone() const
{
  return new FisherSnedecorFactory(*this);
}

/* Here is the interface that all derived class must implement */

FisherSnedecorFactory::Implementation FisherSnedecorFactory::build(const Sample & sample) const
{
  return buildAsFisherSnedecor(sample).clone();
}

FisherSnedecorFactory::Implementation FisherSnedecorFactory::build(const Point & parameters) const
{
  return buildAsFisherSnedecor(parameters).clone();
}

FisherSnedecorFactory::Implementation FisherSnedecorFactory::build() const
{
  return buildAsFisherSnedecor().clone();
}

DistributionFactoryResult FisherSnedecorFactory::buildEstimator(const Sample & sample) const
{
  return buildMaximumLikelihoodEstimator(sample, true);
}

FisherSnedecor FisherSnedecorFactory::buildAsFisherSnedecor(const Sample & sample) const
{
  const UnsignedInteger dimension = build()->getParameterDimension();
  MaximumLikelihoodFactory factory(buildAsFisherSnedecor());

  // override starting point
  OptimizationAlgorithm solver(factory.getOptimizationAlgorithm());
  solver.setStartingPoint(Point(dimension, 0.0));
  factory.setOptimizationAlgorithm(solver);

  // override bounds
  Point parametersLowerBound;
  parametersLowerBound.add(ResourceMap::GetAsScalar("FisherSnedecorFactory-D1LowerBound"));
  parametersLowerBound.add(ResourceMap::GetAsScalar("FisherSnedecorFactory-D2LowerBound"));
  Interval bounds(parametersLowerBound, Point(dimension, SpecFunc::MaxScalar), Interval::BoolCollection(dimension, true), Interval::BoolCollection(dimension, false));
  factory.setOptimizationBounds(bounds);

  return buildAsFisherSnedecor(factory.buildParameter(sample));
}

FisherSnedecor FisherSnedecorFactory::buildAsFisherSnedecor(const Point & parameters) const
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

END_NAMESPACE_OPENTURNS
