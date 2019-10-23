//                                               -*- C++ -*-
/**
 *  @brief Factory for FisherSnedecor distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/FisherSnedecorFactory.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/CenteredFiniteDifferenceGradient.hxx"
#include "openturns/SpecFunc.hxx"
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

/* Algorithm associated with the method of moments */
FisherSnedecor FisherSnedecorFactory::buildMethodOfMoments(const Sample & sample) const
{
  const Scalar mu = sample.computeMean()[0];
  if (mu <= 1.0) throw InvalidArgumentException(HERE) << "Error: cannot estimate a FisherSnedecor distribution based on a sample with sample mean less than 1 using the method of moments.";
  const Scalar sigma2 = sample.computeCovariance()(0, 0);
  if (sigma2 == 0.0) throw InvalidArgumentException(HERE) << "Error: cannot estimate a FisherSnedecor distribution based on a constant sample using the method of moments.";
  const Scalar d2 = 2 * mu / (mu - 1);
  const Scalar d1 = 2 * std::pow(d2, 2.0) * (d2 - 2) / (std::pow(d2 - 2.0, 2.0) * (d2 - 4) * sigma2 - 2 * std::pow(d2, 2.0));
  FisherSnedecor result(d1, d2);
  result.setDescription(sample.getDescription());
  return result;
}

Distribution FisherSnedecorFactory::build(const Sample & sample) const
{
  return buildAsFisherSnedecor(sample).clone();
}

Distribution FisherSnedecorFactory::build(const Point & parameters) const
{
  return buildAsFisherSnedecor(parameters).clone();
}

Distribution FisherSnedecorFactory::build() const
{
  return buildAsFisherSnedecor().clone();
}

DistributionFactoryResult FisherSnedecorFactory::buildEstimator(const Sample & sample) const
{
  return buildMaximumLikelihoodEstimator(sample, true);
}

FisherSnedecor FisherSnedecorFactory::buildAsFisherSnedecor(const Sample & sample) const
{
  // Use method of moments as starting point
  const FisherSnedecor estimatedMomentsDistribution = buildMethodOfMoments(sample);
  Point parametersFromMoments = estimatedMomentsDistribution.getParameter();

  const UnsignedInteger dimension = build().getParameterDimension();
  MaximumLikelihoodFactory factory(buildAsFisherSnedecor());

  Point parametersLowerBound;
  parametersLowerBound.add(ResourceMap::GetAsScalar("FisherSnedecorFactory-D1LowerBound"));
  parametersLowerBound.add(ResourceMap::GetAsScalar("FisherSnedecorFactory-D2LowerBound"));

  // override starting point
  OptimizationAlgorithm solver(factory.getOptimizationAlgorithm());
  solver.setStartingPoint(parametersFromMoments);
  factory.setOptimizationAlgorithm(solver);

  // override bounds
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
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a FisherSnedecor distribution from the given parameters";
  }
}

FisherSnedecor FisherSnedecorFactory::buildAsFisherSnedecor() const
{
  return FisherSnedecor();
}

END_NAMESPACE_OPENTURNS
