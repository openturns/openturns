//                                               -*- C++ -*-
/**
 *  @brief StatTest implements statistical tests
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/LinearModelFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Os.hxx"
#include "openturns/OTconfig.hxx"

BEGIN_NAMESPACE_OPENTURNS

HypothesisTest::HypothesisTest()
{
}


/* Independance test between 2 scalar scalar samples for discrete distributions  */
TestResult HypothesisTest::ChiSquared(const Sample & firstSample,
                                      const Sample & secondSample,
                                      const Scalar level)
{
  if ((firstSample.getDimension() != 1) || (secondSample.getDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: the ChiSquared test can be performed only between two 1D samples.";
  return RunTwoSamplesRTest(firstSample, secondSample, level, "TwoSampleChiSquared");
}

/* Independence Pearson test between 2 scalar samples which form a gaussian vector: test the linear relation  */
TestResult HypothesisTest::Pearson(const Sample & firstSample,
                                   const Sample & secondSample,
                                   const Scalar level)
{
  if ((firstSample.getDimension() != 1) || (secondSample.getDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: the Pearson test can be performed only between two 1D samples.";
  return RunTwoSamplesRTest(firstSample, secondSample, level, "TwoSamplePearson");
}

/* Smirnov test if two scalar samples (of sizes not necessarily equal) follow the same distribution (only for continuous distributions)*/
TestResult HypothesisTest::Smirnov(const Sample & firstSample,
                                   const Sample & secondSample,
                                   const Scalar level)
{
  if ((firstSample.getDimension() != 1) || (secondSample.getDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: the Smirnov test can be performed only between two 1D samples.";
  return RunTwoSamplesRTest(firstSample, secondSample, level, "TwoSampleSmirnov");
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
  cmdFile << "(firstSample, secondSample, " << level << ")" << std::endl;
  cmdFile << "f <- file(\"" << resultFileName << "\",\"wt\")" << std::endl;
  cmdFile << "cat(res$test, res$testResult, res$threshold, res$pValue, sep=\"\\n\", file=f)" << std::endl;
  cmdFile << "close(f)" << std::endl;
  cmdFile.close();
  const String RExecutable(ResourceMap::Get("R-executable-command"));
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
  return RunTwoSamplesASelectionRTest(firstSample, secondSample, selection, level, "PartialPearson");
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
  cmdFile << "(firstSample, secondSample, selection, " << level << ")" << std::endl;
  cmdFile << "f <- file(\"" << resultFileName << "\",\"wt\")" << std::endl;
  cmdFile << "cat(res$test, res$testResult, res$threshold, res$pValue, sep=\"\\n\", file=f)" << std::endl;
  cmdFile << "close(f)" << std::endl;
  cmdFile.close();
  const String RExecutable(ResourceMap::Get("R-executable-command"));
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
