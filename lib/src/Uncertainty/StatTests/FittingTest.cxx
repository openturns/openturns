//                                               -*- C++ -*-
/**
 *  @brief StatTest implements statistical tests
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
#include <cmath>
#include <fstream>
#include "openturns/FittingTest.hxx"
#include "openturns/Point.hxx"
#include "openturns/Description.hxx"
#include "openturns/Path.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Log.hxx"
#include "openturns/Os.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/NearestNeighbour1D.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/HypothesisTest.hxx"

BEGIN_NAMESPACE_OPENTURNS


/* Default constructor */
FittingTest::FittingTest()
{
  // Nothing to do
}

/* Best model for a given numerical sample by BIC */
Distribution FittingTest::BestModelBIC(const Sample & sample,
                                       const DistributionFactoryCollection & factoryCollection,
                                       Scalar & bestBICOut)
{
  const UnsignedInteger size = factoryCollection.getSize();
  if (size == 0) throw InternalException(HERE) << "Error: no model given";
  Bool builtAtLeastOne = false;
  Distribution bestDistribution;
  Scalar bestConcordanceMeasure = SpecFunc::MaxScalar;
  Bool continuousCase = true;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const DistributionFactory factory(factoryCollection[i]);
    try
    {
      LOGINFO(OSS(false) << "Trying factory " << factory);
      const Distribution distribution(factory.build(sample));
      if (i == 0) continuousCase = distribution.isContinuous();
      else if (distribution.isContinuous() != continuousCase) throw InvalidArgumentException(HERE) << "Error: cannot merge continuous and non-continuous models for BIC selection.";
      const Scalar concordanceMeasure = BIC(sample, distribution, distribution.getParameterDimension());
      LOGINFO(OSS(false) << "Resulting distribution=" << distribution << ", BIC=" << concordanceMeasure);
      if (concordanceMeasure < bestConcordanceMeasure)
      {
        bestConcordanceMeasure = concordanceMeasure;
        bestDistribution = distribution;
        builtAtLeastOne = true;
      }
    }
    catch (const Exception & ex)
    {
      LOGWARN(OSS(false) << "Warning! Impossible to use factory " << factory << ". Reason=" << ex);
    }
  }
  if(!builtAtLeastOne) throw InvalidArgumentException(HERE) << "None of the factories could build a model.";
  if (bestConcordanceMeasure == SpecFunc::MaxScalar) LOGWARN(OSS(false) << "Be careful, the best model has an infinite concordance measure. The output distribution must be severely wrong.");
  bestBICOut = bestConcordanceMeasure;
  return bestDistribution;
}

/* Best model for a given numerical sample by BIC */
Distribution FittingTest::BestModelBIC(const Sample  & sample,
                                       const DistributionCollection & distributionCollection,
                                       Scalar & bestBICOut)
{
  const UnsignedInteger size = distributionCollection.getSize();
  if (size == 0) throw InternalException(HERE) << "Error: no model given";
  Distribution bestDistribution;
  Scalar bestConcordanceMeasure = SpecFunc::MaxScalar;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Distribution distribution(distributionCollection[i]);
    LOGINFO(OSS(false) << "Testing distribution " << distribution);
    const Scalar concordanceMeasure = BIC(sample, distribution);
    LOGINFO(OSS(false) << "BIC=" << concordanceMeasure);
    if (concordanceMeasure < bestConcordanceMeasure)
    {
      bestConcordanceMeasure = concordanceMeasure;
      bestDistribution = distribution;
    }
  }
  if (bestConcordanceMeasure > SpecFunc::MaxScalar) LOGWARN(OSS(false) << "Be careful, the best model has an infinite concordance measure. The output distribution must be severely wrong.");
  bestBICOut = bestConcordanceMeasure;
  return bestDistribution;
}




/* Best model for a given numerical sample by Kolmogorov */
Distribution FittingTest::BestModelKolmogorov(const Sample & sample,
    const DistributionFactoryCollection & factoryCollection,
    TestResult & bestResult)
{
  const UnsignedInteger size = factoryCollection.getSize();
  if (size == 0) throw InternalException(HERE) << "Error: no model given";
  const Scalar fakeLevel = 0.5;
  Bool builtAtLeastOne = false;
  Distribution bestDistribution;
  Distribution distribution;
  Scalar bestPValue = -1.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const DistributionFactory factory(factoryCollection[i]);
    try
    {
      LOGINFO(OSS(false) << "Trying factory " << factory);
      const TestResult result(Kolmogorov(sample, factory, distribution, fakeLevel));
      LOGINFO(OSS(false) << "Resulting distribution=" << distribution << ", test result=" << result);
      if (result.getPValue() > bestPValue)
      {
        bestPValue = result.getPValue();
        bestResult = result;
        bestDistribution = distribution;
        builtAtLeastOne = true;
      }
    }
    // The factories can raise many different exceptions (InvalidArgumenException, InternalException, NotDefinedException...). Here we catch everything and echo the reason of the exception.
    catch (const Exception & ex)
    {
      LOGWARN(OSS(false) << "Warning! Impossible to use factory " << factory << ". Reason=" << ex);
    }
  }
  if(!builtAtLeastOne) throw InvalidArgumentException(HERE) << "None of the factories could build a model.";
  if ( bestPValue == 0.0) LOGWARN(OSS(false) << "Be careful, the best model has a p-value of zero. The output distribution must be severely wrong.");
  return bestDistribution;
}

/* Best model for a given numerical sample by Kolmogorov */
Distribution FittingTest::BestModelKolmogorov(const Sample & sample,
    const DistributionCollection & distributionCollection,
    TestResult & bestResult)
{
  const UnsignedInteger size = distributionCollection.getSize();
  if (size == 0) throw InternalException(HERE) << "Error: no model given";
  Distribution bestDistribution;
  Scalar bestPValue = -1.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Distribution distribution(distributionCollection[i]);
    LOGINFO(OSS(false) << "Testing distribution " << distribution);
    const TestResult result(Kolmogorov(sample, distribution));
    LOGINFO(OSS(false) << "Test result=" << result);
    if (result.getPValue() > bestPValue)
    {
      bestPValue = result.getPValue();
      bestResult = result;
      bestDistribution = distribution;
    }
  }
  if ( bestPValue == 0.0) LOGWARN(OSS(false) << "Be careful, the best model has a p-value of zero. The output distribution must be severely wrong.");
  return bestDistribution;
}


/* Best model for a given numerical sample by ChiSquared */
Distribution FittingTest::BestModelChiSquared(const Sample & sample,
    const DistributionFactoryCollection & factoryCollection,
    TestResult & bestResult)
{
  const UnsignedInteger size = factoryCollection.getSize();
  if (size == 0) throw InternalException(HERE) << "Error: no model given";
  const Scalar fakeLevel = 0.5;
  Distribution bestDistribution(factoryCollection[0].build(sample));
  bestResult = ChiSquared(sample, bestDistribution, fakeLevel, bestDistribution.getParameterDimension());
  for (UnsignedInteger i = 1; i < size; ++i)
  {
    const Distribution distribution(factoryCollection[i].build(sample));
    const TestResult result(ChiSquared(sample, distribution, fakeLevel, distribution.getParameterDimension()));
    if (result.getPValue() > bestResult.getPValue())
    {
      bestResult = result;
      bestDistribution = distribution;
    }
  }
  if ( bestResult.getPValue() == 0.0) LOGWARN(OSS(false) << "Be careful, the best model has a p-value of zero.");
  return bestDistribution;
}

/* Best model for a given numerical sample by ChiSquared */
Distribution FittingTest::BestModelChiSquared(const Sample & sample,
    const DistributionCollection & distributionCollection,
    TestResult & bestResult)
{
  const UnsignedInteger size = distributionCollection.getSize();
  if (size == 0) throw InternalException(HERE) << "Error: no model given";
  Distribution bestDistribution(distributionCollection[0]);
  bestResult = ChiSquared(sample, bestDistribution);
  for (UnsignedInteger i = 1; i < size; ++i)
  {
    const Distribution distribution(distributionCollection[i]);
    const TestResult result(ChiSquared(sample, distribution));
    if (result.getPValue() > bestResult.getPValue())
    {
      bestResult = result;
      bestDistribution = distribution;
    }
  }
  if ( bestResult.getPValue() == 0.0) LOGWARN(OSS(false) << "Be careful, the best model has a p-value of zero.");
  return bestDistribution;
}

/* Bayesian Information Criterion computation */
Scalar FittingTest::BIC(const Sample & sample,
                        const Distribution & distribution,
                        const UnsignedInteger estimatedParameters)
{
  if (sample.getDimension() != distribution.getDimension()) throw InvalidArgumentException(HERE) << "Error: the sample dimension and the distribution dimension must be equal";
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: the sample is empty";
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger parametersNumber = distribution.getParameterDimension();
  if (parametersNumber < estimatedParameters) throw InvalidArgumentException(HERE) << "Error: the number of estimated parameters cannot exceed the number of parameters of the distribution";
  Scalar logLikelihood = 0.0;
  const Sample logPDF(distribution.computeLogPDF(sample));
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (logPDF(i, 0) == -SpecFunc::MaxScalar) return SpecFunc::MaxScalar;
    logLikelihood += logPDF(i, 0);
  }
  return (-2.0 * logLikelihood + estimatedParameters * log(1.0 * size)) / size;
}

/* Bayesian Information Criterion computation */
Distribution FittingTest::BIC(const Sample & sample,
                              const DistributionFactory & factory,
                              Scalar & bestBICOut)
{
  const Distribution distribution(factory.build(sample));
  bestBICOut =  BIC(sample, distribution, distribution.getParameterDimension());
  return distribution;
}


/* Kolmogorov test */

Scalar FittingTest::ComputeKolmogorovStatistics(const Sample & sample,
    const Distribution & distribution)
{
  const UnsignedInteger size = sample.getSize();
  Scalar value = 0.0;
  const Sample cdfValues(distribution.computeCDF(sample.sort(0)));
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar cdfValue = cdfValues(i, 0);
    value = std::max(value, std::max(std::abs(Scalar(i) / size - cdfValue), std::abs(cdfValue - Scalar(i + 1) / size)));
  }
  return value;
}

TestResult FittingTest::Kolmogorov(const Sample & sample,
                                   const DistributionFactory & factory,
                                   Distribution & estimatedDistribution,
                                   const Scalar level)

{
  if ((level <= 0.0) || (level >= 1.0)) throw InvalidArgumentException(HERE) << "Error: level must be in ]0, 1[, here level=" << level;
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: Kolmogorov test works only with 1D samples";
  if (!factory.build().isContinuous()) throw InvalidArgumentException(HERE) << "Error: Kolmogorov test can be applied only to a continuous distribution";
  const Distribution distribution(factory.build(sample));
  if (distribution.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: Kolmogorov test works only with 1D distribution";
  // The Kolmogorov test p-value is overestimated if parameters have been estimated
  // We fix it by computing a Monte Carlo estimate of the p-value
  const UnsignedInteger samplingSize = ResourceMap::GetAsUnsignedInteger("FittingTest-KolmogorovSamplingSize");
  Sample kolmogorovStatistics(samplingSize, 1);
  const UnsignedInteger size = sample.getSize();
  for (UnsignedInteger i = 0; i < samplingSize; ++i)
  {
    const Sample newSample(distribution.getSample(size));
    kolmogorovStatistics(i, 0) = ComputeKolmogorovStatistics(newSample, factory.build(newSample));
  }
  // The p-value is estimated using the empirical CDF of the K-statistics at the
  // actual sample K-statistics
  const Scalar statistic = ComputeKolmogorovStatistics(sample, distribution);
  const Scalar pValue = kolmogorovStatistics.computeEmpiricalCDF(Point(1, statistic), true);
  TestResult result(OSS(false) << "Kolmogorov " << distribution.getImplementation()->getClassName(), (pValue > level), pValue, level, statistic);
  result.setDescription(Description(1, String(OSS() << distribution.__str__() << " vs sample " << sample.getName())));
  LOGDEBUG(OSS() << result);
  estimatedDistribution = distribution;
  return result;
}

TestResult FittingTest::Kolmogorov(const Sample & sample,
                                   const Distribution & distribution,
                                   const Scalar level)
{
  if ((level <= 0.0) || (level >= 1.0)) throw InvalidArgumentException(HERE) << "Error: level must be in ]0, 1[, here level=" << level;
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: Kolmogorov test works only with 1D samples";
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: the sample is empty";
  if (!distribution.getImplementation()->isContinuous()) throw InvalidArgumentException(HERE) << "Error: Kolmogorov test can be applied only to a continuous distribution";
  if (distribution.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: Kolmogorov test works only with 1D distribution";
  const Scalar statistic = ComputeKolmogorovStatistics(sample, distribution);
  const Scalar pValue = DistFunc::pKolmogorov(sample.getSize(), statistic, true);
  TestResult result(OSS(false) << "Kolmogorov " << distribution.getImplementation()->getClassName(), (pValue > level), pValue, level, statistic);
  result.setDescription(Description(1, String(OSS() << distribution.__str__() << " vs sample " << sample.getName())));
  LOGDEBUG(OSS() << result);
  return result;
}

/* Chi-squared test */
TestResult FittingTest::ChiSquared(const Sample & sample,
                                   const DistributionFactory & factory,
                                   Distribution & estimatedDistribution,
                                   const Scalar level)
{
  if ((level <= 0.0) || (level >= 1.0)) throw InvalidArgumentException(HERE) << "Error: level must be in ]0, 1[, here level=" << level;
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: ChiSquared test works only with 1D samples";
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: the sample is empty";
  const Distribution distribution(factory.build(sample));
  if (distribution.getImplementation()->isContinuous()) throw InvalidArgumentException(HERE) << "Error: Chi-squared test cannot be applied to a continuous distribution";
  if (distribution.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: ChiSquared test works only with 1D distribution";
  estimatedDistribution = distribution;
  return ChiSquared(sample, distribution, level, distribution.getParameterDimension());
}


/* Chi-squared test */
TestResult FittingTest::ChiSquared(const Sample & sample,
                                   const Distribution & distribution,
                                   const Scalar level,
                                   const UnsignedInteger estimatedParameters)
{
  if ((level <= 0.0) || (level >= 1.0)) throw InvalidArgumentException(HERE) << "Error: level must be in ]0, 1[, here level=" << level;
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: ChiSquared test works only with 1D samples";
  if (distribution.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: ChiSquared test works only with 1D distributions";

  // First, bin the data such that the theoretical frequency in each bin is at least nMin
  const UnsignedInteger nMin = ResourceMap::GetAsUnsignedInteger("FittingTest-ChiSquaredMinimumBinCount");
  const UnsignedInteger size = sample.getSize();
  if (size < 2 * nMin) throw InvalidArgumentException(HERE) << "Error: ChiSquared test cannot be used with a sample size smaller than " << 2 * nMin << ". Reduce the value of \"FittingTest-ChiSquaredMinimumBinCount\" below " << size / 2 << " if you really want to use this test.";
  // Sort the data
  const Sample sortedSample(sample.sort(0));
  UnsignedInteger binNumber = 0;
  // If the bin number was too high to get only nMin points per bin on average, reduce the number of bins to get only nonempty bins
  Point ticks(0);
  Point binProbabilities(0);
  if (distribution.isContinuous())
  {
    binNumber = size / nMin;
    if (binNumber < estimatedParameters + 2) throw InvalidArgumentException(HERE) << "Error: the bin number=" << binNumber << " must be at least equal to the number of estimated parameters + 2=" << estimatedParameters + 2 << " to apply ChiSquared test.";
    // No need to check for repeated quantiles for continuous distributions
    ticks = distribution.computeQuantile(0.0, 1.0, binNumber + 1).asPoint();
    binProbabilities = Point(binNumber, 1.0 / binNumber);
  }
  else
  {
    const Scalar epsilon = ResourceMap::GetAsScalar("DiscreteDistribution-SupportEpsilon");
    const Sample support(distribution.getSupport());
    // Here we should check that the given sample contains only values in the distribution support
    if (ResourceMap::GetAsBool("FittingTest-ChiSquaredCheckSample"))
    {
      const NearestNeighbour1D proximityAlgorithm(support);
      const Indices indices(proximityAlgorithm.queryScalar(sortedSample.asPoint()));
      for (UnsignedInteger i = 0; i < size; ++i)
        if (std::abs(sortedSample(i, 0) - support(indices[i], 0)) > epsilon)
          throw InvalidArgumentException(HERE) << "Error: the given sample contains points which are not in the support of the given distribution wrt the absolute precision=" << epsilon << ". Check the keys 'DiscreteDistribution-SupportEpsilon' and 'FittingTest-ChiSquaredCheckSample' in ResourceMap";
    } // Check sample
    const Point probabilities(distribution.getProbabilities());
    const Scalar probabilityThreshold = (1.0 * nMin) / size;
    const UnsignedInteger supportSize = support.getSize();
    Scalar cumulatedProbabilities = 0.0;
    ticks.add(support(0, 0) - epsilon * (1.0 + std::abs(support(0, 0))));
    for (UnsignedInteger i = 0; i < supportSize; ++i)
    {
      cumulatedProbabilities += probabilities[i];
      if (cumulatedProbabilities >= probabilityThreshold)
      {
        binProbabilities.add(cumulatedProbabilities);
        cumulatedProbabilities = 0.0;
        if (i == supportSize - 1)
          ticks.add(support(i, 0) + epsilon * (1.0 + std::abs(support(i, 0))));
        else
          ticks.add(0.5 * (support(i, 0) + support(i + 1, 0)));
      }
    } // i
    binNumber = ticks.getSize() - 1;
    // Check if there is a pending bin
    if (cumulatedProbabilities > 0.0)
    {
      binProbabilities[binNumber - 1] += cumulatedProbabilities;
      ticks[binNumber] = support(supportSize - 1, 0) + epsilon * (1.0 + std::abs(support(supportSize - 1, 0)));
    }
    if (binNumber < 2) throw InvalidArgumentException(HERE) << "Error: the adjusted bin number=" << binNumber << " must be at least equal to 2.";
    if (binNumber < estimatedParameters + 2) throw InvalidArgumentException(HERE) << "Error: the bin number=" << binNumber << " must be at least equal to the number of estimated parameters + 2=" << estimatedParameters + 2 << " to apply ChiSquared test.";
  } // discrete
  LOGDEBUG(OSS() << "Ticks for ChiSquared test=" << ticks);
  const Scalar w = 1.0 / size;
  UnsignedInteger index = 0;
  Scalar testStatistics = 0.0;
  for (UnsignedInteger i = 0; i < binNumber; ++i)
  {
    const Scalar bound = ticks[i + 1];
    const Scalar probability = binProbabilities[i];
    // Aggregate all the points that fall in the current bin
    Scalar frequency = 0.0;
    while ((index < size) && (sortedSample(index, 0) <= bound))
    {
      frequency += w;
      ++index;
    }
    testStatistics += std::pow(frequency - probability, 2.0) / probability;
    LOGDEBUG(OSS() << "Bin number=" << i << ", bound=" << bound << ", probability=" << probability << ", frequency=" << frequency << ", test statistic=" << testStatistics);
  } // i
  testStatistics *= size;
  // Use the asymptotic statistics corrected from the number of estimated parameters
  const Scalar pValue = DistFunc::pGamma(0.5 * (binNumber - (estimatedParameters + 1)), 0.5 * testStatistics, true);
  TestResult result(OSS(false) << "ChiSquared " << distribution.getImplementation()->getClassName(), (pValue > level), pValue, level, testStatistics);
  result.setDescription(Description(1, String(OSS() << distribution.__str__() << " vs sample " << sample.getName())));
  LOGDEBUG(OSS() << result);
  return result;
}

END_NAMESPACE_OPENTURNS
