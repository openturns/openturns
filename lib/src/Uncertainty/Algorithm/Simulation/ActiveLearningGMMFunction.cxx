//                                               -*- C++ -*-
/**
 *  @brief ActiveLearningGMMFunction implements the "Generalized Maximin"
 *  active learning criterion used in reliability analysis
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/ActiveLearningGMMFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class ActiveLearningGMMFunction
 */

CLASSNAMEINIT(ActiveLearningGMMFunction)

static const Factory<ActiveLearningGMMFunction> Factory_ActiveLearningGMMFunction;

/** Default constructor */
ActiveLearningGMMFunction::ActiveLearningGMMFunction()
  : ActiveLearningReliabilityFunctionImplementation()
{
  // Nothing to do
}

/* Constructor with parameters */
ActiveLearningGMMFunction::ActiveLearningGMMFunction(const Scalar reliabilityThreshold,
                                                     const Scalar learningThreshold,
                                                     const Distribution & inputDistribution,
                                                     const Scalar outputQuantileLevel)
  : ActiveLearningReliabilityFunctionImplementation(reliabilityThreshold,
                                                    learningThreshold)
  , inputDistribution_(inputDistribution)
  , outputQuantileLevel_(outputQuantileLevel)
{
  if (!(outputQuantileLevel >= 0.0) || !(outputQuantileLevel <= 1.0)) throw InvalidArgumentException(HERE) << "Quantile level must be in [0, 1], here level=" << outputQuantileLevel;
}

/* Virtual constructor */
ActiveLearningGMMFunction * ActiveLearningGMMFunction::clone() const
{
  return new ActiveLearningGMMFunction(*this);
}


/* String converter */
String ActiveLearningGMMFunction::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " derived from " << ActiveLearningReliabilityFunctionImplementation::__repr__();
  return oss;
}


/* Compute U criterion for one sample */
Scalar ActiveLearningGMMFunction::computeAsScalar(const Point &) const
{
  // The GMM criterion scales each candidate by the quantile of the candidate
  // pool itself, so it is only defined for a whole sample, and it needs the
  // current design of experiment to compute the distances.
  throw InvalidArgumentException(HERE) << "The GMM criterion is defined on a whole candidate sample together with the "
    "current design of experiment: use operator()(inputSample, inputDoE) or getInfillSample(), "
    "a point-by-point evaluation has no meaning";
}


Sample ActiveLearningGMMFunction::operator()(const Sample & inputSample, const Sample & inputDoE) const
{
  // Generalized max-min (GMM) criterion, see:
  // Lacaze S., Missoum S., A generalized "max-min" sample for surrogate
  // update, Structural and Multidisciplinary Optimization, 49(4), 683-687,
  // 2014.
  // The criterion is restricted to the candidates close to the limit state:
  // the quantile filter below is an implementation-specific acceleration,
  // it is not part of the published criterion.

  const UnsignedInteger inputSampleSize = inputSample.getSize();
  const UnsignedInteger inputDoESize = inputDoE.getSize();
  if (inputDoESize == 0) throw InvalidArgumentException(HERE) << "The GMM criterion needs a nonempty design of experiment";
  const UnsignedInteger dimension = inputSample.getDimension();
  if (inputDoE.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "The GMM criterion needs a design of experiment with the same dimension as the candidate sample ("
                                         << inputDoE.getDimension() << " vs " << dimension << ")";
  // computation of quantile of abs (\hat(g) - t)
  const Sample mx = gprCov_.getConditionalMean(inputSample);

  Sample scaledOutputSample(inputSampleSize, 1);
  for (UnsignedInteger i = 0; i < inputSampleSize; ++i)
    scaledOutputSample(i, 0) = std::abs(mx(i, 0) - reliabilityThreshold_);

  const Scalar outputQuantile = scaledOutputSample.computeQuantile(outputQuantileLevel_)[0];

  Sample outS(inputSampleSize, 1);

  // compute min distance between inputSample and inputDoE weighted by PDF
  for (UnsignedInteger i = 0; i < inputSampleSize; ++i)
  {
    const Scalar scaledGPValue = std::abs(mx(i, 0) - reliabilityThreshold_);
    if (scaledGPValue > outputQuantile) // check if current sample is near the limit state
    {
      outS(i, 0) = 0.0;
      continue;
    }
    // The PDF factor only depends on the candidate: hoist it out of the
    // design-of-experiment loop. The geometric mean of the PDF is computed
    // from the log-density, as pow(pdf(x), 1 / d) overflows in large
    // dimension when the joint density exceeds the floating-point range.
    const Point inputSample_i(inputSample[i]);
    const Scalar pdfFactor = std::exp(inputDistribution_.computeLogPDF(inputSample_i) / dimension);

    Scalar currentMin = 0.0;
    for (UnsignedInteger j = 0; j < inputDoESize; ++j)
    {
      const Scalar proposalMin = (inputSample_i - inputDoE[j]).norm() * pdfFactor;
      if ((j == 0) || (proposalMin < currentMin))
        currentMin = proposalMin;
    }
    outS(i, 0) = currentMin;
  }
  return outS;
}

/* Compute U criterion for one sample */
Bool ActiveLearningGMMFunction::checkConvergenceLearning(const Sample & criterionValues) const
{
  if (criterionValues.getSize() == 0) throw InvalidArgumentException(HERE) << "Cannot check learning convergence on an empty sample of criterion values";
  const Scalar max_GMM = criterionValues.getMax()[0];

  if (max_GMM <=learningThreshold_)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/* Return sample corresponding to GMM criterion*/
Sample ActiveLearningGMMFunction::getInfillSample(const Sample & inputSample,
                                                  const Sample & criterionValues) const
{
  Indices indextoReturn(1);
  indextoReturn[0] = criterionValues.argsort(false)[0];

  Sample sampleToReturn = Sample(inputSample.select(indextoReturn));

  return sampleToReturn;
}

/* Selection direction: the infill point maximizes GMM */
Bool ActiveLearningGMMFunction::isMaximization() const
{
  return true;
}

/* Method save() stores the object through the StorageManager */
void ActiveLearningGMMFunction::save(Advocate & adv) const
{
  ActiveLearningReliabilityFunctionImplementation::save(adv);
  adv.saveAttribute("inputDistribution_", inputDistribution_);
  adv.saveAttribute("outputQuantileLevel_", outputQuantileLevel_);
}

/* Method load() reloads the object from the StorageManager */
void ActiveLearningGMMFunction::load(Advocate & adv)
{
  ActiveLearningReliabilityFunctionImplementation::load(adv);
  adv.loadAttribute("inputDistribution_", inputDistribution_);
  adv.loadAttribute("outputQuantileLevel_", outputQuantileLevel_);
}

END_NAMESPACE_OPENTURNS
