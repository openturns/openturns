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
#include "openturns/LinearModelTest.hxx"
#include "openturns/LinearModelFactory.hxx"
#include "openturns/TestResult.hxx"
#include "openturns/Description.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Normal.hxx"
#include "openturns/ChiSquare.hxx"
#include "openturns/Path.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/Log.hxx"
#include "openturns/Os.hxx"

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

/*  */
TestResult LinearModelTest::LinearModelHarrisonMcCabe(const NumericalSample & firstSample,
  const NumericalSample & secondSample,
  const LinearModel & linearModel,
  const NumericalScalar level,
  const NumericalScalar breakPoint,
  const NumericalScalar simulationSize)
{
  const NumericalSample residuals(linearModel.getResidual(firstSample, secondSample));
  const UnsignedInteger residualSize(firstSample.getSize());

  /* Split the sample using the breakPoint*/ 
  const UnsignedInteger breakIndex(std::floor(residualSize * breakPoint));

  NumericalScalar sumSelectResiduals(0);
  for(UnsignedInteger i = 0; i < breakIndex; ++i)
  { 
    const NumericalPoint residual(residuals[i]);
    sumSelectResiduals += residual.normSquare();
  }

  const NumericalScalar sumSquaredResiduals(residuals.computeVariance()[0] * (residualSize - 1));

  /* compute Harrison McCabe statistic */
  const NumericalScalar hmc = sumSelectResiduals / sumSquaredResiduals;
  
  /* p-value computed by simultation */
  NumericalScalar pValue(0);
  for(UnsignedInteger i = 0; i < simulationSize; ++i)
  {
    const NumericalSample sample(Normal().getSample(residualSize));
    const NumericalSample stantardSample((sample - sample.computeMean()) / sample.computeStandardDeviationPerComponent());
    NumericalScalar sumSelectResidualsSimulation(0);
    for (UnsignedInteger j = 0; j < breakIndex; ++ j)
    { 
      const NumericalPoint stantardSamplePoint(stantardSample[j]);
      sumSelectResidualsSimulation += stantardSamplePoint.normSquare();
    }
    const NumericalScalar sumSquaredResidualsSimulation(stantardSample.computeVariance()[0] * (residualSize - 1));
    const NumericalScalar statistic(sumSelectResidualsSimulation / sumSquaredResidualsSimulation);
    if(statistic < hmc)
    {
      pValue += 1.0;
    }
  }
  pValue = pValue / simulationSize;

  return TestResult(String("HarrisonMcCabe"), Bool(pValue > 1.0 - level), pValue, NumericalScalar(level));
}

/*  */
TestResult LinearModelTest::LinearModelHarrisonMcCabe(const NumericalSample & firstSample,
  const NumericalSample & secondSample,
  const NumericalScalar level,
  const NumericalScalar breakPoint,
  const NumericalScalar simulationSize)
{   
  return LinearModelHarrisonMcCabe(firstSample, secondSample,
                                   LinearModelFactory().build(firstSample, secondSample, level),
                                   level,
                                   breakPoint,
                                   simulationSize);
}

/*  */
TestResult LinearModelTest::LinearModelBreuschPagan(const NumericalSample & firstSample,
  const NumericalSample & secondSample,
  const LinearModel & linearModel,
  const NumericalScalar level)
{
  const NumericalSample residuals(linearModel.getResidual(firstSample, secondSample));
  const UnsignedInteger residualSize(firstSample.getSize());

  /* compute variance of the residuals*/
  const NumericalScalar residualsVariance(residuals.computeVariance()[0]);

  NumericalSample w(residualSize, 1);
  for(UnsignedInteger i = 0; i < residualSize; ++i)
  {
    const NumericalPoint residual(residuals[i]);
    w[i][0] = (residual.normSquare() - residualsVariance);
  }

  /* Build a linear model on the squared residuals */
  const LinearModel linearModelResiduals(LinearModelFactory().build(firstSample, w));
  /* Predicted values of the squared residuals*/
  const NumericalSample wPredicted(linearModelResiduals.getPredicted(firstSample));
  /* Compute variances */
  const NumericalScalar wPredictedVar(wPredicted.computeVariance()[0]);
  const NumericalScalar wVariance(w.computeVariance()[0]);
  /* Compute the Breusch Pagan statistic */
  const NumericalScalar bp(residualSize * wPredictedVar / wVariance);
  /* Get the degree of freedom */
  const UnsignedInteger dof(firstSample.getDimension());
  /* Compute the p-value */
  const NumericalScalar pValue(ChiSquare(dof).computeComplementaryCDF(bp));

  return TestResult(String("BreuschPagan"), Bool(pValue > 1.0 - level), pValue, level);
}

/*  */
TestResult LinearModelTest::LinearModelBreuschPagan(const NumericalSample & firstSample,
  const NumericalSample & secondSample,
  const NumericalScalar level)
{   
  return LinearModelBreuschPagan(firstSample, secondSample,
                                 LinearModelFactory().build(firstSample, secondSample, level),
                                 level);
}


/*  */
TestResult LinearModelTest::LinearModelDurbinWatson(const NumericalSample & firstSample,
  const NumericalSample & secondSample,
  const LinearModel & linearModel,
  const String hypothesis,
  const NumericalScalar level)
{
  const NumericalSample residuals(linearModel.getResidual(firstSample, secondSample));
  const UnsignedInteger residualSize(firstSample.getSize());
  const UnsignedInteger dimension(firstSample.getDimension());

  const NumericalScalar sumSquaredResiduals(residuals.computeVariance()[0] * (residualSize - 1));

  NumericalScalar sumSquaredDifference(0);
  for(UnsignedInteger i = 1; i < residualSize; ++i)
  {
    const NumericalPoint residualDifference(residuals[i] - residuals[i - 1]);
    sumSquaredDifference += residualDifference.normSquare(); 
  }

  /* Compute the Durbin Watson statistic */
  const NumericalScalar dw(sumSquaredDifference / sumSquaredResiduals);

  /* Normal approximation of dw to compute the p-value*/
  /* Create the matrix [1 x]*/
  Matrix X(residualSize, dimension + 1);
  for(UnsignedInteger i = 0; i < residualSize; ++i)
  {
    X(i, 0) = 1;
    X(i, 1) = firstSample[i][0];
  }

  Matrix AX(residualSize, dimension + 1);
  AX(0, 1) = firstSample[0][0] - firstSample[1][0];
  AX(residualSize - 1, 1) = firstSample[residualSize-1][0] - firstSample[residualSize-2][0];
  for(UnsignedInteger i = 0; i < residualSize - 2; ++i)
  {
    AX(i + 1, 1) = -firstSample[i][0] + 2 * firstSample[i + 1][0] - firstSample[i + 2][0];
  }

  CovarianceMatrix XtX(X.computeGram());
  const SquareMatrix XAXQt(XtX.solveLinearSystem(AX.transpose() * X).getImplementation());
  const NumericalScalar P(2 * (residualSize - 1) - XAXQt.computeTrace());
  const NumericalScalar XAXTrace(XtX.solveLinearSystem(AX.computeGram(), false).getImplementation()->computeTrace());
  const NumericalScalar Q(2 * (3 * residualSize - 4) - 2 * XAXTrace + (XAXQt * XAXQt).getImplementation()->computeTrace());
  const NumericalScalar dmean(P / (residualSize - (dimension + 1)));
  const NumericalScalar dvar(2.0 / ((residualSize - (dimension + 1)) * (residualSize - (dimension + 1) + 2)) * (Q - P * dmean));

  /* Compute the p-value with respect to the hypothesis */
  // Initial values defined for hypothesis = "Equal"
  NumericalScalar pValue(2 * DistFunc::pNormal((dw - dmean) / std::sqrt(dvar), true));
  Description description(1, "Hypothesis test: autocorrelation equals 0.");
  if(hypothesis == "Less")
  { 
    pValue = 1 - pValue / 2;
    description[0] = "Hypothesis test: autocorrelation is less than 0";
  }
  else if(hypothesis == "Greater")
  {
    pValue = pValue / 2;
    description[0] = "Hypothesis test: autocorrelation is greater than 0";
  }

  /* Set test result */
  TestResult result(String("DurbinWatson"), Bool(pValue > 1 - level), pValue, level);
  result.setDescription(description);
  return result;
}

/*  */
TestResult LinearModelTest::LinearModelDurbinWatson(const NumericalSample & firstSample,
  const NumericalSample & secondSample,
  const String hypothesis,
  const NumericalScalar level)
{   
  return LinearModelDurbinWatson(firstSample, secondSample,
                                 LinearModelFactory().build(firstSample, secondSample, level),
                                 hypothesis,
                                 level);
}

END_NAMESPACE_OPENTURNS
