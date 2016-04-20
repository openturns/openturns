//                                               -*- C++ -*-
/**
 *  @brief PostAnalyticalControlledImportanceSampling is an implementation of the controlled importance sampling Monte Carlo simulation method in standard space
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/PostAnalyticalControlledImportanceSampling.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/ComparisonOperatorImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



/*
 * @class PostAnalyticalControlledImportanceSampling
 */

CLASSNAMEINIT(PostAnalyticalControlledImportanceSampling);

static const Factory<PostAnalyticalControlledImportanceSampling> RegisteredFactory;

/* Constructor with parameters */
PostAnalyticalControlledImportanceSampling::PostAnalyticalControlledImportanceSampling()
  : PostAnalyticalSimulation()
{
  // Nothing to do
}

/* Constructor with parameters */
PostAnalyticalControlledImportanceSampling::PostAnalyticalControlledImportanceSampling(const AnalyticalResult & analyticalResult)
  : PostAnalyticalSimulation(analyticalResult)
{
  // Nothing to do
}

/* Virtual constructor */
PostAnalyticalControlledImportanceSampling * PostAnalyticalControlledImportanceSampling::clone() const
{
  return new PostAnalyticalControlledImportanceSampling(*this);
}

/* Compute the block sample */
NumericalSample PostAnalyticalControlledImportanceSampling::computeBlockSample()
{
  const UnsignedInteger blockSize = getBlockSize();
  const NumericalPoint standardSpaceDesignPoint(analyticalResult_.getStandardSpaceDesignPoint());
  const Bool originFailure = analyticalResult_.getIsStandardPointOriginInFailureSpace();
  // Get the threshold and the reliability index
  const NumericalScalar threshold = event_.getThreshold();
  const NumericalScalar reliabilityIndex = analyticalResult_.getHasoferReliabilityIndex();
  const NumericalScalar betaSquare = reliabilityIndex * reliabilityIndex;
  // Initialize the probability with the control probability
  NumericalScalar probability = controlProbability_;
  // First, compute a sample of the importance distribution. It is simply
  // the standard distribution translated to the design point
  NumericalSample inputSample(standardDistribution_.getSample(blockSize));
  inputSample += standardSpaceDesignPoint;
  // Then, evaluate the function on this sample
  NumericalSample blockSample(getEvent().getImplementation()->getFunction()(inputSample));
  // Then, modify in place this sample to take into account the change in the input distribution
  for (UnsignedInteger i = 0; i < blockSize; ++i)
  {
    const NumericalPoint realization(inputSample[i]);
    Bool failureControl = dot(realization, standardSpaceDesignPoint) > betaSquare;
    // If the origin is not in the failure domain, the control is made using the linear event dot(u,u*) > beta^2,
    // else it is made using the linear event dot(u,u*) < beta^2.
    failureControl = (failureControl && !originFailure) || (!failureControl && originFailure);
    const Bool failureEvent = event_.getOperator()(blockSample[i][0], threshold);
    blockSample[i][0] = probability;
    const NumericalScalar factor = (!failureControl && failureEvent) - (failureControl && !failureEvent);
    if (factor != 0.0) blockSample[i][0] = blockSample[i][0] + factor * standardDistribution_.computePDF(realization) / standardDistribution_.computePDF(realization - standardSpaceDesignPoint);
  }
  return blockSample;
}

/* String converter */
String PostAnalyticalControlledImportanceSampling::__repr__() const
{
  OSS oss;
  oss << "class=" << PostAnalyticalControlledImportanceSampling::GetClassName()
      << " derived from " << PostAnalyticalSimulation::__repr__();
  return oss;
}

END_NAMESPACE_OPENTURNS
