//                                               -*- C++ -*-
/**
 *  @brief PostAnalyticalImportanceSampling is an implementation of the  importance sampling Monte Carlo simulation method in standard space
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/PostAnalyticalImportanceSampling.hxx"
#include "openturns/Point.hxx"
#include "openturns/ComparisonOperatorImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



/*
 * @class PostAnalyticalImportanceSampling
 */

CLASSNAMEINIT(PostAnalyticalImportanceSampling)

static const Factory<PostAnalyticalImportanceSampling> Factory_PostAnalyticalImportanceSampling;

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
Sample PostAnalyticalImportanceSampling::computeBlockSample()
{
  const UnsignedInteger blockSize = getBlockSize();
  const Point standardSpaceDesignPoint(analyticalResult_.getStandardSpaceDesignPoint());
  // First, compute a sample of the importance distribution. It is simply
  // the standard distribution translated to the design point
  Sample inputSample(standardDistribution_.getSample(blockSize));
  inputSample += standardSpaceDesignPoint;
  // Then, evaluate the function on this sample
  Sample blockSample(getEvent().getImplementation()->getFunction()(inputSample));
  // realizedEventSample = Sample(blockSize_, inputSample.getDimension());
  // Then, modify in place this sample to take into account the change in the input distribution
  const DomainImplementation::BoolCollection isRealized(getEvent().getDomain().contains(blockSample));
  Indices realizedIndices(0);
  for (UnsignedInteger i = 0; i < blockSize; ++i)
  {
    blockSample(i, 0) = 0.0;
    // If the event has occured
    if (isRealized[i])
    {
      realizedIndices.add(i);
    }
  }
  const Sample realizedInputSample(inputSample.select(realizedIndices));
  // If the event occured, the value is p_initial(x[i]) / p_importance(x[i])
  const Sample p_initial(standardDistribution_.computePDF(realizedInputSample));
  const Sample p_importance(standardDistribution_.computePDF(realizedInputSample - standardSpaceDesignPoint));

  for (UnsignedInteger i = 0; i < realizedIndices.getSize(); ++i)
  {
    blockSample(realizedIndices[i], 0) = p_initial(i, 0) / p_importance(i, 0);
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
