//                                               -*- C++ -*-
/**
 *  @brief StatTest implements statistical tests
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
#include <cmath>
#include <fstream>
#include "openturns/HypothesisTest.hxx"
#include "openturns/HypothesisTest.hxx"
#include "openturns/Point.hxx"
#include "openturns/Path.hxx"
#include "openturns/Log.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Exception.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Os.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/UserDefined.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

HypothesisTest::HypothesisTest()
{
}


/* Independence test between 2 scalar scalar samples for discrete distributions
 * The samples here are issued from discrete distributions. Thus there is a transformation
 * from sample to a contingency table in order to apply the Chi square independence test
 * We rely on UserDefined distribution to get unique value samples and their proporprtions
*/
TestResult HypothesisTest::ChiSquared(const Sample & firstSample,
                                      const Sample & secondSample,
                                      const Scalar level)
{
  if ((firstSample.getDimension() != 1) || (secondSample.getDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: the ChiSquared test can be performed only between two 1D samples.";
  const UnsignedInteger size = firstSample.getSize();
  if (secondSample.getSize() != size) throw InvalidArgumentException(HERE) << "Error: the ChiSquared test can be performed only between two samples of same size.";
  
  // Ensure that more than 80 % of classes contain at least nMin points
  const UnsignedInteger nMin = ResourceMap::GetAsUnsignedInteger("FittingTest-ChiSquaredMinimumBinCount");
  if (size < 2 * nMin) 
      throw InvalidArgumentException(HERE) << "Error: ChiSquared test cannot be used with a sample size smaller than " << 2 * nMin 
      << ". Reduce the value of \"FittingTest-ChiSquaredMinimumBinCount below " << size / 2 << " if you really want to use this test.";

  // Derive unique values and frequencies for each 1D sample
  // first sample --> table
  const UserDefined discreteXDistribution(firstSample);
  const Sample tableX(discreteXDistribution.getSupport());
  const Point frequenciesX(discreteXDistribution.computePDF(tableX).asPoint());

  // Define new classes ensuring we get nMin elements / class
  UnsignedInteger binNumberX = 0;
  // If the bin number was too high to get only nMin points per bin on average, reduce the number of bins to get only nonempty bins
  Point ticksX(0);
  Point binX(0);
  const Scalar epsilon = ResourceMap::GetAsScalar("DiscreteDistribution-SupportEpsilon");
  UnsignedInteger cumulatedPoints = 0;
  ticksX.add(tableX(0 ,0) - epsilon * (1.0 + std::abs(tableX(0, 0))));

  for (UnsignedInteger i = 0; i < tableX.getSize(); ++i)
  {
    cumulatedPoints += (frequenciesX[i] + epsilon) * size;
    if (cumulatedPoints >= nMin)
    {
      binX.add(cumulatedPoints);
      cumulatedPoints = 0;
      if (i == tableX.getSize() - 1)
        ticksX.add(tableX(i, 0) + epsilon * (1.0 + std::abs(tableX(i, 0))));
      else
        ticksX.add(0.5 * (tableX(i, 0) + tableX(i + 1, 0)));
    }
  } // i
  binNumberX = ticksX.getSize() - 1;
  // Check if there is a pending bin
  if (cumulatedPoints > 0)
  {
    binX[binNumberX - 1] += cumulatedPoints;
    ticksX[binNumberX] = tableX(tableX.getSize() - 1, 0) + epsilon * (1.0 + std::abs(tableX(tableX.getSize() - 1, 0)));
  }
  if (binNumberX < 2)
    throw InvalidArgumentException(HERE) << "Error: the adjusted bin number=" << binNumberX << " must be at least equal to 2.";
    
 
  // second sample table
  const UserDefined discreteYDistribution(secondSample);
  const Sample tableY(discreteYDistribution.getSupport());
  const Point frequenciesY(discreteYDistribution.computePDF(tableY).asPoint());

  // Classes for second sample
  UnsignedInteger binNumberY = 0;
  // If the bin number was too high to get only nMin points per bin on average, reduce the number of bins to get only nonempty bins
  Point ticksY(0);
  Point binY(0);
  cumulatedPoints = 0;
  ticksY.add(tableY(0 ,0) - epsilon * (1.0 + std::abs(tableY(0, 0))));
  for (UnsignedInteger i = 0; i < tableY.getSize(); ++i)
  {
    cumulatedPoints += (frequenciesY[i] + epsilon) * size;
    if (cumulatedPoints >= nMin)
    {
      binY.add(cumulatedPoints);
      cumulatedPoints = 0;
      if (i == tableY.getSize() - 1)
        ticksY.add(tableY(i, 0) + epsilon * (1.0 + std::abs(tableY(i, 0))));
      else
        ticksY.add(0.5 * (tableY(i, 0) + tableY(i + 1, 0)));
    }
  } // i
  binNumberY = ticksY.getSize() - 1;
  // Check if there is a pending bin
  if (cumulatedPoints > 0)
  {
    binY[binNumberY - 1] += cumulatedPoints;
    ticksY[binNumberY] = tableY(tableY.getSize() - 1, 0) + epsilon * (1.0 + std::abs(tableY(tableY.getSize() - 1, 0)));
  }
  if (binNumberY < 2)
    throw InvalidArgumentException(HERE) << "Error: the adjusted bin number=" << binNumberY << " must be at least equal to 2.";

  // Now we define the table with elements E_{i,j} where E[i,j] counts the occurence of the element [xi, yj]
  // Bivariate sample
  Sample data(firstSample);
  data.stack(secondSample);
  const UserDefined bivariateDiscreteDistribution(data);
  const Sample table(bivariateDiscreteDistribution.getSupport());
  const Point frequencies(bivariateDiscreteDistribution.getProbabilities());

  Point classes(binNumberX * binNumberY);
  Point pointsInClasses(binNumberX * binNumberY);

  for (UnsignedInteger k = 0; k < table.getSize(); ++k)
  {
    const Scalar xi = table(k , 0);
    const Scalar yj = table(k , 1);
    Bool condition = true;
    UnsignedInteger indexI = 0;
    while (condition)
    {
      indexI++;
      condition = (xi > ticksX[indexI]) && (indexI < binNumberX);
    }
    // 
    condition = true;
    indexI--;
    UnsignedInteger indexJ = 0;
    while (condition)
    {
      indexJ++;
      condition = (yj > ticksY[indexJ]) && (indexJ < binNumberY);
    }
    indexJ--;

    // We get the corresponding class for the kth point
    pointsInClasses[indexI * binNumberY + indexJ] += (frequencies[k] + epsilon) * size;
  }

  // Theoretical value under H0 assumption
  Scalar theoretical = 0.0;
  // Let us compute the statistic
  Scalar squaredSum = 0.0;

  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < binNumberX; ++i)
  {
    for (UnsignedInteger j = 0; j < binNumberY; ++j)
    {
     theoretical = binX[i] * binY[j] / size;
     squaredSum += std::pow( pointsInClasses[index]  - theoretical, 2) / theoretical;
     index++;
    }
  }
  const UnsignedInteger df = (binNumberX - 1)*(binNumberY -1);
  const Scalar pValue =  DistFunc::pGamma(0.5 * df, 0.5 * squaredSum, true);
  Log::Debug ( OSS() << "In ChiSquared independence test : df = " << df << ", statistic = " << squaredSum << ", pValue = " << pValue );
  return TestResult("ChiSquared", pValue > level, pValue, level);
}

/* Independence Pearson test between 2 scalar samples which form a gaussian vector: test the linear relation  */
TestResult HypothesisTest::Pearson(const Sample & firstSample,
                                   const Sample & secondSample,
                                   const Scalar level)
{
  if ((firstSample.getDimension() != 1) || (secondSample.getDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: the Pearson test can be performed only between two 1D samples.";
  const UnsignedInteger size = firstSample.getSize();
  if (secondSample.getSize() != size) throw InvalidArgumentException(HERE) << "Error: the Pearson test can be performed only between two samples of same size.";
  if (size < 3)  throw InvalidArgumentException(HERE) << "Error: the Pearson test can not be performed with small samples.";
  // Implement the test using basic rho statistic
  Sample fullSample(firstSample);
  fullSample.stack(secondSample);
  const Scalar rho = fullSample.computePearsonCorrelation()(0, 1);
  // Here we check if rho is significantly different from 0
  const Scalar pValue = 2.0 * DistFunc::pPearsonCorrelation(size, rho, true);
  return TestResult("Pearson", pValue > level, pValue, level);
}

/* Smirnov test if two scalar samples (of sizes not necessarily equal) follow the same distribution (only for continuous distributions)*/
TestResult HypothesisTest::Smirnov(const Sample & firstSample,
                                   const Sample & secondSample,
                                   const Scalar level)
{
  LOGWARN(OSS() << "HypothesisTest::Smirnov(x, y, level) is deprecated, use HypothesisTest::TwoSamplesKolmogorov(x, y, level) instead");
  return TwoSamplesKolmogorov(firstSample, secondSample, level);
}

/* Two-sample Kolmogorov–Smirnov test */
TestResult HypothesisTest::TwoSamplesKolmogorov(const Sample & sample1,
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
    const Scalar sampleAllSorted_i = sampleAllSorted(i, 0);
    Scalar cdf1 = 0.0;
    for (UnsignedInteger j = 0; j < size1; ++ j)
    {
      if (sampleAllSorted(j, 0) <= sampleAllSorted_i)
      {
        cdf1 = (j + 1.0) / size1;
      }
      else
        break;
    }
    Scalar cdf2 = 0.0;
    for (UnsignedInteger j = 0; j < size2; ++ j)
    {
      if (sampleAllSorted(size1 + j, 0) <= sampleAllSorted_i)
      {
        cdf2 = (j + 1.0) / size2;
      }
      else
        break;
    }
    value = std::max(value, std::abs(cdf1 - cdf2));
  }
  const Scalar pValue = DistFunc::pKolmogorov((size1 * size2) / (size1 + size2), value, true);
  TestResult result(OSS(false) << "TwoSamplesKolmogorov", pValue > level, pValue, level);
  result.setDescription(Description(1, String(OSS() << "sample" << sample1.getName() << " vs sample " << sample2.getName())));
  LOGDEBUG(OSS() << result);
  return result;
}

/* Spearman test between 2 samples of dimension 1: it tests for null rank correlation between the two samples.*/
TestResult HypothesisTest::Spearman(const Sample & firstSample,
                                    const Sample & secondSample,
                                    const Scalar level)
{
  if ((firstSample.getDimension() != 1) || (secondSample.getDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: the Spearman test can be performed only between two 1D samples.";
  const UnsignedInteger size = firstSample.getSize();
  if (secondSample.getSize() != size) throw InvalidArgumentException(HERE) << "Error: the Spearman test can be performed only between two samples of same size.";

  const Bool ties = (firstSample.sortUnique().getSize() < size) || (secondSample.sortUnique().getSize() < size);
  Sample fullSample(firstSample);
  fullSample.stack(secondSample);
  const Scalar rho = fullSample.computeSpearmanCorrelation()(0, 1);
  // Here we check if rho is significantly different from 0
  const Scalar pValue = 2.0 * DistFunc::pSpearmanCorrelation(size, std::abs(rho), true, ties);
  return TestResult("Spearman", pValue > level, pValue, level);
}

/* Generic invocation of a R script for executing a test between two 1D samples */
TestResult HypothesisTest::RunTwoSamplesRTest(const Sample & firstSample,
    const Sample & secondSample,
    const Scalar level,
    const String & testName)
{
  const String firstDataFileName(firstSample.storeToTemporaryFile());
  const String secondDataFileName(secondSample.storeToTemporaryFile());
  const String resultFileName(Path::BuildTemporaryFileName("RResult.txt.XXXXXX"));
  const String commandFileName(Path::BuildTemporaryFileName("RCmd.R.XXXXXX"));
  std::ofstream cmdFile(commandFileName.c_str(), std::ios::out);
  // Fill-in the command file
  cmdFile << "library(rot)" << std::endl;
  cmdFile << "options(digits=17)" << std::endl;
  cmdFile << "options(warn=-1)" << std::endl;
  cmdFile << "options(stringsAsFactors = F)" << std::endl;
  cmdFile << "firstSample <- data.matrix(read.table(\"" << firstDataFileName << "\"))" << std::endl;
  cmdFile << "secondSample <- data.matrix(read.table(\"" << secondDataFileName << "\"))" << std::endl;
  cmdFile << "res <- computeTest" << testName;
  cmdFile << "(firstSample, secondSample, " << 1.0 - level << ")" << std::endl;
  cmdFile << "f <- file(\"" << resultFileName << "\",\"wt\")" << std::endl;
  cmdFile << "cat(res$test, res$testResult, res$threshold, res$pValue, sep=\"\\n\", file=f)" << std::endl;
  cmdFile << "close(f)" << std::endl;
  cmdFile.close();
  const String RExecutable(ResourceMap::GetAsString("R-executable-command"));
  OSS systemCommand;
  if (RExecutable != "") systemCommand << RExecutable << " --no-save --silent < \"" << commandFileName << "\"" << Os::GetDeleteCommandOutput();
  else throw NotYetImplementedException(HERE) << "In HypothesisTest::RunTwoSamplesRTest(const Sample & firstSample, const Sample & secondSample, const Scalar level, const String & testName): needs R. Please install it and set the absolute path of the R executable in ResourceMap.";
  const int returnCode(Os::ExecuteCommand(String(systemCommand).c_str()));
  if (returnCode != 0) throw InternalException(HERE) << "Error: unable to execute the system command " << String(systemCommand) << " returned code is " << returnCode;
  // Parse result file
  std::ifstream resultFile(resultFileName.c_str(), std::ios::in);
  // First, test type
  String testType;
  resultFile >> testType;
  // Second, test results
  Bool testResult;
  resultFile >> testResult;
  // Third, test threshold
  Scalar pThreshold = -1.0;
  resultFile >> pThreshold;
  // Fourth, test value
  Scalar pValue = -1.0;
  resultFile >> pValue;

  // Clean-up everything
  Os::Remove(firstDataFileName);
  Os::Remove(secondDataFileName);
  Os::Remove(resultFileName);
  Os::Remove(commandFileName);

  return TestResult(testType, testResult, pValue, pThreshold);
}

/* Independence Pearson test between 2 samples : firstSample of dimension n and secondSample of dimension 1. If firstSample[i] is the numeriacl sample extracted from firstSample (ith coordinate of each point of the numerical sample), PartialPearson performs the Independence Pearson test simultaneously on firstSample[i] and secondSample, for i in the selection. For all i, it is supposed that the couple (firstSample[i] and secondSample) is issued from a gaussian  vector. */
HypothesisTest::TestResultCollection HypothesisTest::PartialPearson(const Sample & firstSample,
    const Sample & secondSample,
    const Indices & selection,
    const Scalar level)
{
  if (secondSample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the partial Pearson test can be performed only with an 1-d ouput sample.";
  if (!selection.check(firstSample.getDimension())) throw InvalidArgumentException(HERE) << "Error: invalid selection, repeated indices or values out of bound";
  const UnsignedInteger size = selection.getSize();
  TestResultCollection results(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    results[i] = Pearson(firstSample.getMarginal(selection[i]), secondSample, level);
  return results;
}

/* Regression test between 2 samples : firstSample of dimension n and secondSample of dimension 1. If firstSample[i] is the numerical sample extracted from firstSample (ith coordinate of each point of the numerical sample), PartialRegression performs the Regression test simultaneously on all firstSample[i] and secondSample, for i in the selection. The Regression test tests ifthe regression model between two scalar numerical samples is significant. It is based on the deviation analysis of the regression. The Fisher distribution is used. */
HypothesisTest::TestResultCollection HypothesisTest::PartialRegression(const Sample & firstSample,
    const Sample & secondSample,
    const Indices & selection,
    const Scalar level)
{
  // TODO: this can be checked inside RunTwoSamplesASelectionRTest once the rot script is fixed
  if (secondSample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the Regression test can be performed only with an 1-d ouput sample.";

  if (!selection.check(firstSample.getDimension())) throw InvalidArgumentException(HERE) << "Error: invalid selection, repeated indices or values out of bound";

  // FIXME: unlike other rot scripts in/out sample args are swapped:
  return RunTwoSamplesASelectionRTest(secondSample, firstSample, selection, level, "PartialRegression");
}

/* Spearman test between 2 samples : firstSample if of dimension n and secondSample of dimension 1. PartialSpearman tests for null rank correlation between the selected marginals of the first sample wrt the second sample.*/
HypothesisTest::TestResultCollection HypothesisTest::PartialSpearman(const Sample & firstSample,
    const Sample & secondSample,
    const Indices & selection,
    const Scalar level)
{
  if (secondSample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the partial Spearman test can be performed only with an 1-d ouput sample.";
  if (!selection.check(firstSample.getDimension())) throw InvalidArgumentException(HERE) << "Error: invalid selection, repeated indices or values out of bound";
  const UnsignedInteger size = selection.getSize();
  TestResultCollection results(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    results[i] = Spearman(firstSample.getMarginal(selection[i]), secondSample, level);
  return results;
}

/* Independence Pearson test between 2 samples : firstSample of dimension n and secondSample of dimension 1. If firstSample[i] is the numerical sample extracted from firstSample (ith coordinate of each point of the numerical sample), FullPearson performs the Independence Pearson test simultaneously on all firstSample[i] and secondSample. For all i, it is supposed that the couple (firstSample[i] and secondSample) is issued from a gaussian  vector. */
HypothesisTest::TestResultCollection HypothesisTest::FullPearson(const Sample & firstSample,
    const Sample & secondSample,
    const Scalar level)
{
  const UnsignedInteger dimension = firstSample.getDimension();
  Indices selection(dimension);
  selection.fill();
  return PartialPearson(firstSample, secondSample, selection, level);
}

/* Regression test between 2 samples : firstSample of dimension n and secondSample of dimension 1. If firstSample[i] is the numerical sample extracted from firstSample (ith coordinate of each point of the numerical sample), FullRegression performs the Regression test simultaneously on all firstSample[i] and secondSample. The Regression test tests if the regression model between two scalar numerical samples is significant. It is based on the deviation analysis of the regression. The Fisher distribution is used. */
HypothesisTest::TestResultCollection HypothesisTest::FullRegression(const Sample & firstSample,
    const Sample & secondSample,
    const Scalar level)
{
  const UnsignedInteger dimension = firstSample.getDimension();
  Indices selection(dimension);
  selection.fill();
  return PartialRegression(firstSample, secondSample, selection, level);
}

/* Spearman test between 2 samples : firstSample if of dimension n and secondSample of dimension 1. FullSpearman tests for null rank correlation between the marginals of the first sample wrt the second sample.*/
HypothesisTest::TestResultCollection HypothesisTest::FullSpearman(const Sample & firstSample,
    const Sample & secondSample,
    const Scalar level)
{
  const UnsignedInteger dimension = firstSample.getDimension();
  Indices selection(dimension);
  selection.fill();
  return PartialSpearman(firstSample, secondSample, selection, level);
}

/* Generic invocation of a R script for testing the partial correlation between two samples */
HypothesisTest::TestResultCollection HypothesisTest::RunTwoSamplesASelectionRTest(const Sample & firstSample,
    const Sample & secondSample,
    const Indices & selection,
    const Scalar level,
    const String & testName)
{
  const String firstDataFileName(firstSample.storeToTemporaryFile());
  const String secondDataFileName(secondSample.storeToTemporaryFile());
  const UnsignedInteger size = selection.getSize();
  Sample selectionSample(size, 1);
  for (UnsignedInteger i = 0; i < size; ++ i) selectionSample(i, 0) = selection[i] + 1.0;
  const String selectionFileName(selectionSample.storeToTemporaryFile());
  const String resultFileName(Path::BuildTemporaryFileName("RResult.txt.XXXXXX"));
  const String commandFileName(Path::BuildTemporaryFileName("RCmd.R.XXXXXX"));
  std::ofstream cmdFile(commandFileName.c_str(), std::ios::out);
  // Fill-in the command file
  cmdFile << "library(rot)" << std::endl;
  cmdFile << "options(digits=17)" << std::endl;
  cmdFile << "options(warn=-1)" << std::endl;
  cmdFile << "options(stringsAsFactors = F)" << std::endl;
  cmdFile << "firstSample <- data.matrix(read.table(\"" << firstDataFileName << "\"))" << std::endl;
  cmdFile << "secondSample <- data.matrix(read.table(\"" << secondDataFileName << "\"))" << std::endl;
  cmdFile << "selection <- data.matrix(read.table(\"" << selectionFileName << "\"))" << std::endl;
  cmdFile << "res <- computeTest" << testName;
  cmdFile << "(firstSample, secondSample, selection, " << 1.0 - level << ")" << std::endl;
  cmdFile << "f <- file(\"" << resultFileName << "\",\"wt\")" << std::endl;
  cmdFile << "cat(res$test, res$testResult, res$threshold, res$pValue, sep=\"\\n\", file=f)" << std::endl;
  cmdFile << "close(f)" << std::endl;
  cmdFile.close();
  const String RExecutable(ResourceMap::GetAsString("R-executable-command"));
  OSS systemCommand;
  if (RExecutable != "") systemCommand << RExecutable << " --no-save --silent < \"" << commandFileName << "\"" << Os::GetDeleteCommandOutput();
  else throw NotYetImplementedException(HERE) << "In HypothesisTest::RunTwoSamplesASelectionRTest(const Sample & firstSample, const Sample & secondSample, const Indices & selection, const Scalar level, const String & testName): needs R. Please install it and set the absolute path of the R executable in ResourceMap.";
  const int returnCode = Os::ExecuteCommand(systemCommand);
  if (returnCode != 0) throw InternalException(HERE) << "Error: unable to execute the system command " << String(systemCommand) << " returned code is " << returnCode;
  // Parse result file
  std::ifstream resultFile(resultFileName.c_str(), std::ios::in);

  // First, test type
  String testType;
  resultFile >> testType;

  // Second, test results
  Indices results(size);
  // FIXME: rot script should not output intercept p-value
  Scalar intercept = 0.;
  if (testName == "PartialRegression")
    resultFile >> intercept;
  for (UnsignedInteger i = 0; i < size; ++ i) resultFile >> results[i];

  // Third, test threshold
  Scalar pThreshold = 0.;
  resultFile >> pThreshold;

  // Fourth, test values
  Point pValues(size);
  if (testName == "PartialRegression")
    resultFile >> intercept;
  for (UnsignedInteger i = 0; i < size; ++ i) resultFile >> pValues[i];

  // Clean-up everything
  Os::Remove(firstDataFileName);
  Os::Remove(secondDataFileName);
  Os::Remove(selectionFileName);
  Os::Remove(resultFileName);
  Os::Remove(commandFileName);

  // Then, build the collection of results
  TestResultCollection resultCollection(size);
  for (UnsignedInteger i = 0; i < size; ++ i) resultCollection[i] = TestResult(testType, results[i] == 1, pValues[i], pThreshold);
  return resultCollection;
}

END_NAMESPACE_OPENTURNS
