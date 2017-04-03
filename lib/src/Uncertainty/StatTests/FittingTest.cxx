//                                               -*- C++ -*-
/**
 *  @brief StatTest implements statistical tests
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
#include "openturns/OTconfig.hxx"

BEGIN_NAMESPACE_OPENTURNS


/* Default constructor */
FittingTest::FittingTest()
{
  // Nothing to do
}

/* Best model for a given numerical sample by BIC */
Distribution FittingTest::BestModelBIC(const Sample & sample,
                                       const DistributionFactoryCollection & factoryCollection)
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
    catch (InvalidArgumentException & ex)
    {
      LOGWARN(OSS(false) << "Warning! Impossible to use factory " << factory << ". Reason=" << ex);
    }
  }
  if(!builtAtLeastOne) throw InvalidArgumentException(HERE) << "None of the factories could build a model.";
  if (bestConcordanceMeasure == SpecFunc::MaxScalar) LOGWARN(OSS(false) << "Be careful, the best model has an infinite concordance measure. The output distribution must be severely wrong.");
  return bestDistribution;
}

/* Best model for a given numerical sample by BIC */
Distribution FittingTest::BestModelBIC(const Sample  & sample,
                                       const DistributionCollection & distributionCollection)
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
  Scalar bestPValue = -1.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const DistributionFactory factory(factoryCollection[i]);
    try
    {
      LOGINFO(OSS(false) << "Trying factory " << factory);
      const Distribution distribution(factoryCollection[i].build(sample));
      const TestResult result(Kolmogorov(sample, distribution, fakeLevel, distribution.getParameterDimension()));
      LOGINFO(OSS(false) << "Resulting distribution=" << distribution << ", test result=" << result);
      if (result.getPValue() > bestPValue)
      {
        bestPValue = result.getPValue();
        bestResult = result;
        bestDistribution = distribution;
        builtAtLeastOne = true;
      }
    }
    catch (InvalidArgumentException & ex)
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
    if (logPDF[i][0] == -SpecFunc::MaxScalar) return SpecFunc::MaxScalar;
    logLikelihood += logPDF[i][0];
  }
  return (-2.0 * logLikelihood + estimatedParameters * log(1.0 * size)) / size;
}

/* Bayesian Information Criterion computation */
Scalar FittingTest::BIC(const Sample & sample,
                                 const DistributionFactory & factory)
{
  const Distribution distribution(factory.build(sample));
  return BIC(sample, distribution, distribution.getParameterDimension());
}


/* Kolmogorov test */
TestResult FittingTest::Kolmogorov(const Sample & sample,
                                   const DistributionFactory & factory,
                                   const Scalar level)
{
  if ((level <= 0.0) || (level >= 1.0)) throw InvalidArgumentException(HERE) << "Error: level must be in ]0, 1[, here level=" << level;
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: Kolmogorov test works only with 1D samples";
  const Distribution distribution(factory.build(sample));
  if (!distribution.getImplementation()->isContinuous()) throw InvalidArgumentException(HERE) << "Error: Kolmogorov test can be applied only to a continuous distribution";
  if (distribution.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: Kolmogorov test works only with 1D distribution";
  return Kolmogorov(sample, distribution, level, distribution.getParameterDimension());
}


/* Kolmogorov test */
TestResult FittingTest::Kolmogorov(const Sample & sample,
                                   const Distribution & distribution,
                                   const Scalar level,
                                   const UnsignedInteger estimatedParameters)
{
  if ((level <= 0.0) || (level >= 1.0)) throw InvalidArgumentException(HERE) << "Error: level must be in ]0, 1[, here level=" << level;
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: Kolmogorov test works only with 1D samples";
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: the sample is empty";
  if (!distribution.getImplementation()->isContinuous()) throw InvalidArgumentException(HERE) << "Error: Kolmogorov test can be applied only to a continuous distribution";
  if (distribution.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: Kolmogorov test works only with 1D distribution";
  if (estimatedParameters > 0) LOGINFO("Warning: using Kolmogorov test for a distribution with estimated parameters will result in an overestimated pValue");
  const UnsignedInteger size = sample.getSize();
  Scalar value = 0.0;
  const Sample cdfValues(distribution.computeCDF(sample.sort(0)));
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar cdfValue = cdfValues[i][0];
    value = std::max(value, std::max(std::abs(Scalar(i) / size - cdfValue), std::abs(cdfValue - Scalar(i + 1) / size)));
  }
  const Scalar pValue = DistFunc::pKolmogorov(size, value, true);
  TestResult result(OSS(false) << "Kolmogorov" << distribution.getClassName(), (pValue > 1.0 - level), pValue, 1.0 - level);
  result.setDescription(Description(1, String(OSS() << distribution.__str__() << " vs sample " << sample.getName())));
  LOGDEBUG(OSS() << result);
  return result;
}

/* Two-sample Kolmogorov–Smirnov test */
TestResult FittingTest::TwoSamplesKolmogorov(const Sample & sample1,
    const Sample & sample2,
    const Scalar level)
{
  if ((level <= 0.0) || (level >= 1.0)) throw InvalidArgumentException(HERE) << "Error: level must be in ]0, 1[, here level=" << level;
  if ((sample1.getDimension() != 1) || (sample2.getDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: Kolmogorov test works only with 1D samples";
  if ((sample1.getSize() == 0) || (sample2.getSize() == 0)) throw InvalidArgumentException(HERE) << "Error: the sample is empty";
  const UnsignedInteger size1 = sample1.getSize();
  const UnsignedInteger size2 = sample2.getSize();
  Sample sampleAllSorted(sample1.sort());
  sampleAllSorted.add(sample2.sort());
  Scalar value = 0.0;
  for (UnsignedInteger i = 0; i < size1 + size2; ++ i)
  {
    const Scalar sampleAllSorted_i = sampleAllSorted[i][0];
    Scalar cdf1 = 0.0;
    for (UnsignedInteger j = 0; j < size1; ++ j)
    {
      if (sampleAllSorted[j][0] <= sampleAllSorted_i)
      {
        cdf1 = (j + 1.0) / size1;
      }
      else
        break;
    }
    Scalar cdf2 = 0.0;
    for (UnsignedInteger j = 0; j < size2; ++ j)
    {
      if (sampleAllSorted[size1 + j][0] <= sampleAllSorted_i)
      {
        cdf2 = (j + 1.0) / size2;
      }
      else
        break;
    }
    value = std::max(value, std::abs(cdf1 - cdf2));
  }
  const Scalar pValue = DistFunc::pKolmogorov((size1 * size2) / (size1 + size2), value, true);
  TestResult result(OSS(false) << "Kolmogorov " << sample1.getName() << "/" << sample2.getName(), (pValue > 1.0 - level), pValue, 1.0 - level);
  result.setDescription(Description(1, String(OSS() << "sample" << sample1.getName() << " vs sample " << sample2.getName())));
  LOGDEBUG(OSS() << result);
  return result;
}

/* Chi-squared test */
TestResult FittingTest::ChiSquared(const Sample & sample,
                                   const DistributionFactory & factory,
                                   const Scalar level)
{
  if ((level <= 0.0) || (level >= 1.0)) throw InvalidArgumentException(HERE) << "Error: level must be in ]0, 1[, here level=" << level;
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: ChiSquared test works only with 1D samples";
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: the sample is empty";
  const Distribution distribution(factory.build(sample));
  if (distribution.getImplementation()->isContinuous()) throw InvalidArgumentException(HERE) << "Error: Chi-squared test cannot be applied to a continuous distribution";
  if (distribution.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: ChiSquared test works only with 1D distribution";
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
  if (distribution.getImplementation()->isContinuous()) throw InvalidArgumentException(HERE) << "Error: Chi-squared test cannot be applied to a continuous distribution";

#if 0
  // First, bin the data such that the theoretical frequency in each bin is at least nMin
  const UnsignedInteger nMin = ResourceMap::GetAsUnsignedInteger("FittingTest-ChiSquaredMinFrequency");
  const UnsignedInteger size = sample.getSize();
  if (size < 2 * nMin) throw InvalidArgumentException(HERE) << "Error: ChiSquared test cannot be used with a sample size smaller than " << 2 * nMin << ". Reduce the value of \"FittingTest-ChiSquaredMinFrequency\" below " << size / 2 << " if you really want to do this test.";
  // The test statistics
  Scalar xi = 0.0;
  // Sort the data
  const Sample sortedSample(sample.sort(0));
  const UnsignedInteger iMax = static_cast<UnsignedInteger>(round(size / nMin));
  UnsignedInteger dataIndex = 0;
  for (UnsignedInteger i = 1; i <= iMax; ++i)
  {
    const Scalar currentBound = distribution.computeQuantile(i / Scalar(iMax))[0];
    UnsignedInteger count = 0;
    while (sample[dataIndex][0] <= currentBound)
    {
      ++count;
      ++dataIndex;
    }
  }
#endif
  return RunRTest(sample, distribution, level, estimatedParameters, "ChiSquared");
}

/* Generic invocation of a R script for testing a distribution against a sample */
TestResult FittingTest::RunRTest(const Sample & sample,
                                 const Distribution & distribution,
                                 const Scalar level,
                                 const UnsignedInteger estimatedParameters,
                                 const String & testName)
{
  const String dataFileName(sample.storeToTemporaryFile());
  const String resultFileName(Path::BuildTemporaryFileName("RResult.txt.XXXXXX"));
  const String commandFileName(Path::BuildTemporaryFileName("RCmd.R.XXXXXX"));
  std::ofstream cmdFile(commandFileName.c_str(), std::ios::out);
  // Fill-in the command file
  cmdFile << "library(rot)" << std::endl;
  cmdFile << "options(digits=17)" << std::endl;
  cmdFile << "options(warn=-1)" << std::endl;
  cmdFile << "options(stringsAsFactors = F)" << std::endl;
  cmdFile << "sample <- data.matrix(read.table(\"" << dataFileName << "\"))" << std::endl;
  cmdFile << "res <- computeTest" << testName << distribution.getImplementation()->getClassName();
  cmdFile << "(sample, ";
  const Point parameters(distribution.getParametersCollection()[0]);
  const UnsignedInteger parametersNumber = parameters.getDimension();
  for (UnsignedInteger i = 0; i < parametersNumber; ++i) cmdFile << parameters[i] << ", ";
  cmdFile << level << ", " << estimatedParameters << ")" << std::endl;
  cmdFile << "f <- file(\"" << resultFileName << "\",\"wt\")" << std::endl;
  cmdFile << "cat(res$test, res$testResult, res$threshold, res$pValue, sep=\"\\n\", file=f)" << std::endl;
  cmdFile << "close(f)" << std::endl;
  cmdFile.close();
  const String RExecutable(ResourceMap::Get("R-executable-command"));
  OSS systemCommand;
  if (RExecutable != "") systemCommand << RExecutable << " --no-save --silent < \"" << commandFileName << "\"" << Os::GetDeleteCommandOutput();
  else throw NotYetImplementedException(HERE) << "In FittingTest::RunRTest(const Sample & sample, const Distribution & distribution, const Scalar level, const UnsignedInteger estimatedParameters, const String & testName): needs R. Please install it and set the absolute path of the R executable in ResourceMap.";
  const int returnCode(Os::ExecuteCommand(systemCommand));
  if (returnCode != 0) throw InternalException(HERE) << "Error: unable to execute the system command " << String(systemCommand) << " returned code is " << returnCode;
  // Parse result file
  std::ifstream resultFile(resultFileName.c_str(), std::ios::in);
  String testType;
  resultFile >> testType;
  Bool testResult;
  resultFile >> testResult;
  Scalar pThreshold = -1.0;
  resultFile >> pThreshold;
  Scalar pValue = -1.0;
  resultFile >> pValue;

  // Clean-up everything
  Os::Remove(dataFileName);
  Os::Remove(resultFileName);
  Os::Remove(commandFileName);

  TestResult result(testType, testResult, pValue, pThreshold);
  return result;
}


END_NAMESPACE_OPENTURNS
