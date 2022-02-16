//                                               -*- C++ -*-
/**
 *  @brief NAIS implement Non Parametric Adaptive Importance Sampling algorithm
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/NAISResult.hxx"
#include "openturns/NAIS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SimulationAlgorithm.hxx"
BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NAIS
 */

CLASSNAMEINIT(NAIS);

static Factory<NAIS> Factory_NAIS;


/** Default constructor */
NAIS::NAIS()
  : EventSimulation()
{
  // Nothing TO DO
}


/* Default constructor */
NAIS::NAIS(const RandomVector & event,
           const UnsignedInteger numberOfSamples,
           const Scalar rhoQuantile)
  : EventSimulation(event)
  , numberOfSample_(numberOfSamples)
  , initialDistribution_(getEvent().getAntecedent().getDistribution())
{
  const Interval range(initialDistribution_.getRange());
  const Interval::BoolCollection rangeUpper(range.getFiniteUpperBound());
  const Interval::BoolCollection rangeLower(range.getFiniteLowerBound());
  for (UnsignedInteger i = 0; i < rangeUpper.getSize(); ++i)
    if (rangeUpper[i] || rangeLower[i]) throw InvalidArgumentException(HERE) << "Current version of NAIS is only adapted to unbounded distribution" ;
  rhoQuantile_ = (getEvent().getOperator()(0, 1) ? rhoQuantile : 1.0 - rhoQuantile);
}

// get weights
Point NAIS::getWeights() const
{
  return weights_;
}

// set weights
void NAIS::setWeights(const Point & weight)
{
  weights_ = weight;
}

// get outputsamples
Sample NAIS::getOutputSample() const
{
  return outputSample_;
}

// set outputsamples
void NAIS::setOutputSample(const Sample & outputSample)
{
  outputSample_ = outputSample;
}

// Function computing the auxiliary distribution as a function of current samples and associated weights_
Distribution NAIS::computeAuxiliaryDistribution(const Sample & sample,
    const Point & weights_)
{
  Scalar weightsPower2 = weights_.normSquare();
  Scalar sumWeights(0);
  for (UnsignedInteger i = 0; i < weights_.getSize(); ++i)
  {
    sumWeights += weights_[i];
  }
  // computation of weight
  const Scalar neff = sumWeights * sumWeights / weightsPower2;

  // computation of bandwidth using Silverman rule
  const Point stdPerComponent(sample.computeStandardDeviation());
  const UnsignedInteger dimensionSample = getEvent().getAntecedent().getDimension();
  const Point silverman(stdPerComponent * std::pow(neff * (dimensionSample + 2.0) / 4.0, -1.0 / (dimensionSample + 4.0)));

  Collection<Distribution> margins(dimensionSample);

  // computation of auxiliary distribution using ot.Mixture
  for (UnsignedInteger k = 0; k < dimensionSample ; ++k)
  {
    Collection<Distribution> collectionOfDistribution(numberOfSample_);
    for (UnsignedInteger i = 0; i < numberOfSample_ ; ++i)
    {
      collectionOfDistribution[i] = Normal(sample(i, k), silverman[k]);
    }
    margins[k] = Mixture(collectionOfDistribution, weights_);
  }
  naisResult_.setAuxiliaryDensity(ComposedDistribution(margins));
  return naisResult_.getAuxiliaryDensity();
}

/** Function computing weigths of sample */
Point NAIS::computeWeights(const Sample & samples,
                           const Sample & respectiveSamples,
                           const Scalar eventThresholdLocal,
                           const Distribution & auxiliaryDistribution)
{
  weights_ = Point(samples.getSize());
  for (UnsignedInteger i = 0; i < samples.getSize(); ++i)
  {
    const Bool weightBool = getEvent().getOperator()(respectiveSamples(i, 0), eventThresholdLocal);
    if (weightBool)
    {
      const Scalar fValue = initialDistribution_.computeLogPDF(samples[i]);
      const Scalar gValue = auxiliaryDistribution.computeLogPDF(samples[i]);
      weights_[i] = std::exp(fValue - gValue);
    }
  }
  return weights_;
}

/** Main function that computes the failure probability */
void NAIS::run()
{

  UnsignedInteger iter = 1;

// drawing of samples using initial density
  sample_ = initialDistribution_.getSample(numberOfSample_);

// evaluation on limit state function
  Sample responsiveSample(getEvent().getFunction()(sample_));

// computation of current quantile
  Scalar quantileCourant = responsiveSample.computeQuantile(rhoQuantile_).getCollection()[0];

// computation of weights_
  weights_ = computeWeights(sample_, responsiveSample, quantileCourant, initialDistribution_);

// computation of auxiliary distribution
  Distribution auxiliaryDistribution(computeAuxiliaryDistribution(sample_, weights_));
  iter += 1;

  while (getEvent().getOperator()(getEvent().getThreshold(), quantileCourant))
  {
    // drawing of samples using auxiliary density
    sample_ = auxiliaryDistribution.getSample(numberOfSample_);

    // evaluation on limit state function
    responsiveSample = getEvent().getFunction()(sample_);

    // computation of current quantile
    quantileCourant = responsiveSample.computeQuantile(rhoQuantile_)[0];

    // if failure probability reached, stop the adaptation
    if (getEvent().getOperator()(quantileCourant, getEvent().getThreshold()))
    {
      quantileCourant = getEvent().getThreshold();
    }
    else
    {
      // computation of weights_
      weights_ = computeWeights(sample_, responsiveSample, quantileCourant, auxiliaryDistribution);

      //update of auxiliary distribution
      auxiliaryDistribution = computeAuxiliaryDistribution(sample_, weights_);
    }
  }

// Estimation of failure probability
  Interval::BoolCollection failureProbability(responsiveSample.getSize());
  for (UnsignedInteger i = 0; i < responsiveSample.getSize(); ++i)
  {
    // find failure Points
    failureProbability[i] = (getEvent().getOperator()(responsiveSample(i, 0), getEvent().getThreshold()));
  }

// find failure samples indices
  Indices indicesCritic(0);
  for(UnsignedInteger i = 0; i < failureProbability.getSize(); ++i)
  {
    if (failureProbability[i] == 1)
    {
      indicesCritic.add(i);
    }
  }

  Sample resp_sampleCritic(responsiveSample.select(indicesCritic));

  Sample sampleCritic(sample_.select(indicesCritic));

//evaluate initial PDF on failure samples
  Sample pdfInitCritic(initialDistribution_.computePDF(sampleCritic));

//evaluate auxiliary PDF on failure samples
  Sample pdfAuxiliaryCritic(auxiliaryDistribution.computePDF(sampleCritic));

  Scalar sumPdfCritic = 0;
  for(UnsignedInteger i = 0; i < pdfInitCritic.getSize(); ++i)
  {
    sumPdfCritic += (pdfInitCritic(i, 0) / pdfAuxiliaryCritic(i, 0)); //.getCollection()[0];
  }

//Calculation of failure probability
  Scalar probabilityEstimate = sumPdfCritic / numberOfSample_  ;

// Save of data in Simulation naisResult_ structure
  naisResult_.setProbabilityEstimate(probabilityEstimate);
  naisResult_.setSamples(sample_);
  naisResult_.setAuxiliaryDensity(auxiliaryDistribution);
  outputSample_ = responsiveSample;

}

/** Accessor to naisResult_s */
NAISResult NAIS::getResult() const
{
  return naisResult_;
}

END_NAMESPACE_OPENTURNS
