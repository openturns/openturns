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
TestResult LinearModelTest::LinearModelFisher(const Sample & firstSample,
    const Sample & secondSample,
    const LinearModel & linearModel,
    const Scalar level)
{
  return RunTwoSamplesALinearModelRTest(firstSample, secondSample, linearModel, level, "LmFisher");
}

/*  */
TestResult LinearModelTest::LinearModelFisher(const Sample & firstSample,
    const Sample & secondSample,
    const Scalar level)
{
  return LinearModelFisher(firstSample, secondSample, LinearModelFactory().build(firstSample, secondSample, 1.0 - level), level);
}

/*  */
TestResult LinearModelTest::LinearModelResidualMean(const Sample & firstSample,
    const Sample & secondSample,
    const LinearModel & linearModel,
    const Scalar level)
{
  return RunTwoSamplesALinearModelRTest(firstSample, secondSample, linearModel, level, "LmResidualMean");
}

/*  */
TestResult LinearModelTest::LinearModelResidualMean(const Sample & firstSample,
    const Sample & secondSample,
    const Scalar level)
{
  return LinearModelResidualMean(firstSample, secondSample, LinearModelFactory().build(firstSample, secondSample, 1.0 - level), level);
}

/* Generic invocation of a R script for testing a linear model against two samples */
TestResult LinearModelTest::RunTwoSamplesALinearModelRTest(const Sample & firstSample,
    const Sample & secondSample,
    const LinearModel & linearModel,
    const Scalar level,
    const String & testName)
{
  String firstDataFileName(firstSample.storeToTemporaryFile());
  String secondDataFileName(secondSample.storeToTemporaryFile());
  Sample regression(1, linearModel.getRegression());
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
  cmdFile << "(firstSample, regression, secondSample, " << 1.0 - level << ")" << std::endl;
  cmdFile << "f <- file(\"" << resultFileName << "\",\"wt\")" << std::endl;
  cmdFile << "cat(res$test, res$testResult, res$threshold, res$pValue, sep=\"\\n\", file=f)" << std::endl;
  cmdFile << "close(f)" << std::endl;
  cmdFile.close();
  const String RExecutable(ResourceMap::GetAsString("R-executable-command"));
  OSS systemCommand;
  if (RExecutable != "") systemCommand << RExecutable << " --no-save --silent < \"" << commandFileName << "\"" << Os::GetDeleteCommandOutput();
  else throw NotYetImplementedException(HERE) << "In LinearModelTest::RunTwoSamplesALinearModelRTest(const Sample & firstSample, const Sample & secondSample, const LinearModel & linearModel, const Scalar level, const String & testName): needs R. Please install it and set the absolute path of the R executable in ResourceMap.";
  int returnCode(Os::ExecuteCommand(systemCommand));
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
  Os::Remove(firstDataFileName);
  Os::Remove(secondDataFileName);
  Os::Remove(regressionFileName);
  Os::Remove(resultFileName);
  Os::Remove(commandFileName);

  return TestResult(testType, testResult, pValue, pThreshold);
}

/*  */
TestResult LinearModelTest::LinearModelHarrisonMcCabe(const Sample & firstSample,
    const Sample & secondSample,
    const LinearModel & linearModel,
    const Scalar level,
    const Scalar breakPoint,
    const Scalar simulationSize)
{
  const Sample residuals(linearModel.getResidual(firstSample, secondSample));
  const UnsignedInteger residualSize = firstSample.getSize();

  /* Split the sample using the breakPoint*/
  const UnsignedInteger breakIndex = std::floor(residualSize * breakPoint);

  Scalar sumSelectResiduals = 0;
  for(UnsignedInteger i = 0; i < breakIndex; ++i)
  {
    const Point residual(residuals[i]);
    sumSelectResiduals += residual.normSquare();
  }

  const Scalar sumSquaredResiduals = residuals.computeVariance()[0] * (residualSize - 1);

  /* compute Harrison McCabe statistic */
  const Scalar hmc = sumSelectResiduals / sumSquaredResiduals;

  /* p-value computed by simultation */
  Scalar pValue = 0;
  for(UnsignedInteger i = 0; i < simulationSize; ++i)
  {
    const Sample sample(Normal().getSample(residualSize));
    const Sample stantardSample((sample - sample.computeMean()) / sample.computeStandardDeviationPerComponent());
    Scalar sumSelectResidualsSimulation = 0;
    for (UnsignedInteger j = 0; j < breakIndex; ++ j)
    {
      const Point stantardSamplePoint(stantardSample[j]);
      sumSelectResidualsSimulation += stantardSamplePoint.normSquare();
    }
    const Scalar sumSquaredResidualsSimulation = stantardSample.computeVariance()[0] * (residualSize - 1);
    const Scalar statistic = sumSelectResidualsSimulation / sumSquaredResidualsSimulation;
    if(statistic < hmc)
    {
      pValue += 1.0;
    }
  }
  pValue = pValue / simulationSize;

  return TestResult("HarrisonMcCabe", pValue > level, pValue, level);
}

/*  */
TestResult LinearModelTest::LinearModelHarrisonMcCabe(const Sample & firstSample,
    const Sample & secondSample,
    const Scalar level,
    const Scalar breakPoint,
    const Scalar simulationSize)
{
  return LinearModelHarrisonMcCabe(firstSample, secondSample,
                                   LinearModelFactory().build(firstSample, secondSample, 1.0 - level),
                                   level,
                                   breakPoint,
                                   simulationSize);
}

/*  */
TestResult LinearModelTest::LinearModelBreuschPagan(const Sample & firstSample,
    const Sample & secondSample,
    const LinearModel & linearModel,
    const Scalar level)
{
  const Sample residuals(linearModel.getResidual(firstSample, secondSample));
  const UnsignedInteger residualSize = firstSample.getSize();

  /* compute variance of the residuals*/
  const Scalar residualsVariance = residuals.computeVariance()[0];

  Sample w(residualSize, 1);
  for(UnsignedInteger i = 0; i < residualSize; ++i)
  {
    const Point residual(residuals[i]);
    w(i, 0) = (residual.normSquare() - residualsVariance);
  }

  /* Build a linear model on the squared residuals */
  const LinearModel linearModelResiduals(LinearModelFactory().build(firstSample, w));
  /* Predicted values of the squared residuals*/
  const Sample wPredicted(linearModelResiduals.getPredicted(firstSample));
  /* Compute variances */
  const Scalar wPredictedVar = wPredicted.computeVariance()[0];
  const Scalar wVariance = w.computeVariance()[0];
  /* Compute the Breusch Pagan statistic */
  const Scalar bp = residualSize * wPredictedVar / wVariance;
  /* Get the degree of freedom */
  const UnsignedInteger dof = firstSample.getDimension();
  /* Compute the p-value */
  const Scalar pValue = ChiSquare(dof).computeComplementaryCDF(bp);

  return TestResult("BreuschPagan", pValue > level, pValue, level);
}

/*  */
TestResult LinearModelTest::LinearModelBreuschPagan(const Sample & firstSample,
    const Sample & secondSample,
    const Scalar level)
{
  return LinearModelBreuschPagan(firstSample, secondSample,
                                 LinearModelFactory().build(firstSample, secondSample, 1.0 - level),
                                 level);
}


/*  */
TestResult LinearModelTest::LinearModelDurbinWatson(const Sample & firstSample,
    const Sample & secondSample,
    const LinearModel & linearModel,
    const String hypothesis,
    const Scalar level)
{
  const Sample residuals(linearModel.getResidual(firstSample, secondSample));
  const UnsignedInteger residualSize = firstSample.getSize();
  const UnsignedInteger dimension = firstSample.getDimension();

  const Scalar sumSquaredResiduals = residuals.computeVariance()[0] * (residualSize - 1);

  Scalar sumSquaredDifference = 0;
  for(UnsignedInteger i = 1; i < residualSize; ++i)
  {
    const Point residualDifference(residuals[i] - residuals[i - 1]);
    sumSquaredDifference += residualDifference.normSquare();
  }

  /* Compute the Durbin Watson statistic */
  const Scalar dw = sumSquaredDifference / sumSquaredResiduals;

  /* Normal approximation of dw to compute the p-value*/
  /* Create the matrix [1 x]*/
  Matrix X(residualSize, dimension + 1);
  for(UnsignedInteger j = 0; j < dimension; ++j)
  {
    for(UnsignedInteger i = 0; i < residualSize; ++i)
    {
        X(i, 0) = 1.0;
        X(i, j+1) = firstSample(i, j);
    }
  }

  Matrix AX(residualSize, dimension + 1);
  for(UnsignedInteger j = 0; j < dimension; ++j)
  {
    AX(0, j+1) = firstSample(0, j) - firstSample(1, j);
    AX(residualSize - 1, j+1) = firstSample(residualSize - 1, j) - firstSample(residualSize - 2, j);
  }
  for(UnsignedInteger j = 0; j < dimension; ++j)
  {
    for(UnsignedInteger i = 0; i < residualSize - 2; ++i)
    {
      AX(i + 1, j+1) = -firstSample(i, j) + 2 * firstSample(i + 1, j) - firstSample(i + 2, j);
    } 
  }

  CovarianceMatrix XtX(X.computeGram());
  const SquareMatrix XAXQt(XtX.solveLinearSystem(AX.transpose() * X).getImplementation());
  const Scalar P = 2 * (residualSize - 1) - XAXQt.computeTrace();
  const Scalar XAXTrace = XtX.solveLinearSystem(AX.computeGram(), false).getImplementation()->computeTrace();
  const Scalar Q = 2 * (3 * residualSize - 4) - 2 * XAXTrace + (XAXQt * XAXQt).getImplementation()->computeTrace();
  const Scalar dmean = P / (residualSize - (dimension + 1));
  const Scalar dvar = 2.0 / ((residualSize - (dimension + 1)) * (residualSize - (dimension + 1) + 2)) * (Q - P * dmean);

  /* Compute the p-value with respect to the hypothesis */
  Scalar pValue;
  Description description(1);
  if (hypothesis == "Equal")
  {
    pValue = 2.0 * DistFunc::pNormal(std::abs(dw - dmean) / std::sqrt(dvar), true);
    description[0] = "H0: auto.cor=0";
  }
  else if(hypothesis == "Less")
  {
    pValue = DistFunc::pNormal((dw - dmean) / std::sqrt(dvar));
    description[0] = "H0: auto.cor<0";
  }
  else if(hypothesis == "Greater")
  {
    pValue = DistFunc::pNormal((dw - dmean) / std::sqrt(dvar), true);
    description[0] = "H0: auto.cor>0";
  }

  /* Set test result */
  TestResult result("DurbinWatson", pValue > level, pValue, level);
  result.setDescription(description);
  return result;
}

/*  */
TestResult LinearModelTest::LinearModelDurbinWatson(const Sample & firstSample,
    const Sample & secondSample,
    const String hypothesis,
    const Scalar level)
{
  return LinearModelDurbinWatson(firstSample, secondSample,
                                 LinearModelFactory().build(firstSample, secondSample, 1.0 - level),
                                 hypothesis,
                                 level);
}

END_NAMESPACE_OPENTURNS
