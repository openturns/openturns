//                                               -*- C++ -*-
/**
 *  @brief StatTest implements statistical tests
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
#include <cmath>
#include <fstream>
#include "LinearModelTest.hxx"
#include "LinearModelFactory.hxx"
#include "Path.hxx"
#include "ResourceMap.hxx"
#include "OTconfig.hxx"
#include "Log.hxx"
#include "Os.hxx"

BEGIN_NAMESPACE_OPENTURNS


LinearModelTest::LinearModelTest()
{
}


/*  */
TestResult LinearModelTest::LinearModelAdjustedRSquared(const NumericalSample & firstSample,
    const NumericalSample & secondSample,
    const LinearModel & linearModel,
    const NumericalScalar level)
{
  return RunTwoSamplesALinearModelRTest(firstSample, secondSample, linearModel, level, "LmAdjustedRSquare");
}

/*  */
TestResult LinearModelTest::LinearModelAdjustedRSquared(const NumericalSample & firstSample,
    const NumericalSample & secondSample,
    const NumericalScalar level)
{
  return LinearModelAdjustedRSquared(firstSample, secondSample, LinearModelFactory().build(firstSample, secondSample, level), level);
}

/*  */
TestResult LinearModelTest::LinearModelFisher(const NumericalSample & firstSample,
    const NumericalSample & secondSample,
    const LinearModel & linearModel,
    const NumericalScalar level)
{
  return RunTwoSamplesALinearModelRTest(firstSample, secondSample, linearModel, level, "LmFisher");
}

/*  */
TestResult LinearModelTest::LinearModelFisher(const NumericalSample & firstSample,
    const NumericalSample & secondSample,
    const NumericalScalar level)
{
  return LinearModelFisher(firstSample, secondSample, LinearModelFactory().build(firstSample, secondSample, level), level);
}

/*  */
TestResult LinearModelTest::LinearModelResidualMean(const NumericalSample & firstSample,
    const NumericalSample & secondSample,
    const LinearModel & linearModel,
    const NumericalScalar level)
{
  return RunTwoSamplesALinearModelRTest(firstSample, secondSample, linearModel, level, "LmResidualMean");
}

/*  */
TestResult LinearModelTest::LinearModelResidualMean(const NumericalSample & firstSample,
    const NumericalSample & secondSample,
    const NumericalScalar level)
{
  return LinearModelResidualMean(firstSample, secondSample, LinearModelFactory().build(firstSample, secondSample, level), level);
}

/*  */
TestResult LinearModelTest::LinearModelRSquared(const NumericalSample & firstSample,
    const NumericalSample & secondSample,
    const LinearModel & linearModel,
    const NumericalScalar level)
{
  return RunTwoSamplesALinearModelRTest(firstSample, secondSample, linearModel, level, "LmRsquared");
}

/*  */
TestResult LinearModelTest::LinearModelRSquared(const NumericalSample & firstSample,
    const NumericalSample & secondSample,
    const NumericalScalar level)
{
  return LinearModelRSquared(firstSample, secondSample, LinearModelFactory().build(firstSample, secondSample, level), level);
}

/* Generic invocation of a R script for testing a linear model against two samples */
TestResult LinearModelTest::RunTwoSamplesALinearModelRTest(const NumericalSample & firstSample,
    const NumericalSample & secondSample,
    const LinearModel & linearModel,
    const NumericalScalar level,
    const String & testName)
{
  String firstDataFileName(firstSample.storeToTemporaryFile());
  String secondDataFileName(secondSample.storeToTemporaryFile());
  NumericalSample regression(1, linearModel.getRegression());
  String regressionFileName(regression.storeToTemporaryFile());
  String resultFileName(Path::BuildTemporaryFileName("RResult.txt.XXXXXX"));
  String commandFileName(Path::BuildTemporaryFileName("RCmd.R.XXXXXX"));
  std::ofstream cmdFile(commandFileName.c_str(), std::ios::out);
  // Fill-in the command file
  cmdFile << "library(rot)" << std::endl;
  cmdFile << "options(digits=17)" << std::endl;
  cmdFile << "options(warn=-1)" << std::endl;
  cmdFile << "options(stringsAsFactors = F)" << std::endl;
  cmdFile << "firstSample <- data.matrix(read.table(\"" << firstDataFileName << "\"))" << std::endl;
  cmdFile << "secondSample <- data.matrix(read.table(\"" << secondDataFileName << "\"))" << std::endl;
  cmdFile << "regression <- t(data.matrix(read.table(\"" << regressionFileName << "\")))" << std::endl;
  cmdFile << "res <- test" << testName;
  cmdFile << "(firstSample, regression, secondSample, " << level << ")" << std::endl;
  cmdFile << "f <- file(\"" << resultFileName << "\",\"wt\")" << std::endl;
  cmdFile << "cat(res$test, res$testResult, res$threshold, res$pValue, sep=\"\\n\", file=f)" << std::endl;
  cmdFile << "close(f)" << std::endl;
  cmdFile.close();
  const String RExecutable(ResourceMap::Get("R-executable-command"));
  OSS systemCommand;
  if (RExecutable != "") systemCommand << RExecutable << " --no-save --silent < \"" << commandFileName << "\"" << Os::GetDeleteCommandOutput();
  else throw NotYetImplementedException(HERE) << "In LinearModelTest::RunTwoSamplesALinearModelRTest(const NumericalSample & firstSample, const NumericalSample & secondSample, const LinearModel & linearModel, const NumericalScalar level, const String & testName): needs R. Please install it and set the absolute path of the R executable in ResourceMap.";
  int returnCode(Os::ExecuteCommand(systemCommand));
  if (returnCode != 0) throw InternalException(HERE) << "Error: unable to execute the system command " << String(systemCommand) << " returned code is " << returnCode;
  // Parse result file
  std::ifstream resultFile(resultFileName.c_str(), std::ios::in);
  String testType;
  resultFile >> testType;
  Bool testResult;
  resultFile >> testResult;
  NumericalScalar pThreshold;
  resultFile >> pThreshold;
  NumericalScalar pValue;
  resultFile >> pValue;

  // Clean-up everything
  Os::Remove(firstDataFileName);
  Os::Remove(secondDataFileName);
  Os::Remove(regressionFileName);
  Os::Remove(resultFileName);
  Os::Remove(commandFileName);

  return TestResult(testType, testResult, pValue, pThreshold);
}


END_NAMESPACE_OPENTURNS
