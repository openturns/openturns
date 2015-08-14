//                                               -*- C++ -*-
/**
 *  @brief PostAnalyticalImportanceSampling is an implementation of the  importance sampling Monte Carlo simulation method in standard space
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
#include "PostAnalyticalImportanceSampling.hxx"
#include "NumericalPoint.hxx"
#include "ComparisonOperatorImplementation.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



/*
 * @class PostAnalyticalImportanceSampling
 */

CLASSNAMEINIT(PostAnalyticalImportanceSampling);

static Factory<PostAnalyticalImportanceSampling> RegisteredFactory("PostAnalyticalImportanceSampling");

/* Constructor with parameters */
PostAnalyticalImportanceSampling::PostAnalyticalImportanceSampling()
  : PostAnalyticalSimulation()
{
  // Nothing to do
}

/* Constructor with parameters */
PostAnalyticalImportanceSampling::PostAnalyticalImportanceSampling(const AnalyticalResult & analyticalResult):
  PostAnalyticalSimulation(analyticalResult)
{
  // Nothing to do
}

/* Virtual constructor */
PostAnalyticalImportanceSampling * PostAnalyticalImportanceSampling::clone() const
{
  return new PostAnalyticalImportanceSampling(*this);
}

/* Compute the block sample */
NumericalSample PostAnalyticalImportanceSampling::computeBlockSample()
{
  const UnsignedInteger blockSize(getBlockSize());
  const NumericalPoint standardSpaceDesignPoint(analyticalResult_.getStandardSpaceDesignPoint());
  // Get the threshold and the reliability index
  const NumericalScalar threshold(event_.getThreshold());
  // First, compute a sample of the importance distribution. It is simply
  // the standard distribution translated to the design point
  NumericalSample inputSample(standardDistribution_.getSample(blockSize));
  inputSample += standardSpaceDesignPoint;
  // Then, evaluate the function on this sample
  NumericalSample blockSample(getEvent().getImplementation()->getFunction()(inputSample));
  // realizedEventSample = NumericalSample(blockSize_, inputSample.getDimension());
  // Then, modify in place this sample to take into account the change in the input distribution
  for (UnsignedInteger i = 0; i < blockSize; ++i)
  {
    const Bool isRealized(getEvent().getOperator()(blockSample[i][0], threshold));
    // If the event has occured
    if (isRealized)
    {
      // If the event occured, the value is p_initial(x[i]) / p_importance(x[i])
      const NumericalScalar weight(standardDistribution_.computePDF(inputSample[i]) / standardDistribution_.computePDF(inputSample[i] - standardSpaceDesignPoint));
      blockSample[i][0] = weight;
    }
    else blockSample[i][0] = 0.0;
  }
  return blockSample;
}

/* String converter */
String PostAnalyticalImportanceSampling::__repr__() const
{
  OSS oss;
  oss << "class=" << PostAnalyticalImportanceSampling::GetClassName()
      << " derived from " << PostAnalyticalSimulation::__repr__();
  return oss;
}

END_NAMESPACE_OPENTURNS
