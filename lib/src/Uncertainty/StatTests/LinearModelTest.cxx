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
#include "openturns/LinearModelTest.hxx"
#include "openturns/LinearModelAlgorithm.hxx"
#include "openturns/TestResult.hxx"
#include "openturns/Description.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Normal.hxx"
#include "openturns/ChiSquare.hxx"
#include "openturns/FisherSnedecor.hxx"
#include "openturns/LinearLeastSquares.hxx"
#include "openturns/LinearBasisFactory.hxx"
#include "openturns/Path.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/Log.hxx"
#include "openturns/LinearModelAnalysis.hxx"
#include "openturns/LinearCombinationFunction.hxx"
#include "openturns/DesignProxy.hxx"


BEGIN_NAMESPACE_OPENTURNS


LinearModelTest::LinearModelTest()
{
}

/*  */
TestResult LinearModelTest::LinearModelFisher(const Sample & firstSample,
    const Sample & secondSample,
    const LinearModelResult & linearModelResult,
    const Scalar level)
{
  if (firstSample.getSize() != secondSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
  if (secondSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  const UnsignedInteger size = firstSample.getSize();
  if (size < 3) throw InvalidArgumentException(HERE) << "Error: sample too small. Sample should contains at least 3 elements";
  // As we rely on a linear model result, we should be very generic
  // Instead of using input dimension, one should use parameter size
  const UnsignedInteger df = linearModelResult.getDegreesOfFreedom();

  // Regression coefficient
  const Function fHat(linearModelResult.getMetaModel());
  const Sample yHat(fHat(firstSample));
  const Sample residualSample(secondSample - yHat);

  // For the Fisher test, we need both Sum of Squared Explained (SSE)
  // and the Sum of Squared Residuals

  const Scalar sumSquaredExplained = (yHat - secondSample.computeMean()).computeRawMoment(2)[0] * firstSample.getSize() ;
  const Scalar sumSquaredResiduals = residualSample.computeRawMoment(2)[0] * firstSample.getSize() ;


  // The statistical test checks the nullity of the regression linear model coefficients
  // H0 : Beta_i = 0
  // H1 : Beta_i < 0 or Beta_i > 0
  // The statistics follows a Fisher distribution
  const Scalar numerator = sumSquaredExplained / (size - df - 1);
  const Scalar denomerator = sumSquaredResiduals / df;

  const Scalar statistic = numerator / denomerator;
  Log::Debug(OSS() << "F-statistic = " << statistic);
  const Scalar pValue =  FisherSnedecor(size - df - 1, df).computeComplementaryCDF(statistic);
  return TestResult("Fisher", pValue > level, pValue, level, statistic);
}

/*  */
TestResult LinearModelTest::LinearModelFisher(const Sample & firstSample,
    const Sample & secondSample,
    const Scalar level)
{

  if (secondSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  if (firstSample.getSize() != secondSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
  const UnsignedInteger size = firstSample.getSize();
  const UnsignedInteger dimension = firstSample.getDimension();
  if (size < 3) throw InvalidArgumentException(HERE) << "Error: sample too small. Sample should contains at least 3 elements";
  if (size < dimension + 1) throw InvalidArgumentException(HERE) << "Error: sample too small. Size should be greater than degree of freedom";

  LinearModelAlgorithm algo(firstSample, secondSample);
  algo.run();
  const LinearModelResult result(algo.getResult());
  return LinearModelFisher(firstSample, secondSample, result, level);
}

/*  */
TestResult LinearModelTest::LinearModelResidualMean(const Sample & firstSample,
    const Sample & secondSample,
    const LinearModelResult & linearModelResult,
    const Scalar level)
{

  if (firstSample.getSize() != secondSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
  if (secondSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  const UnsignedInteger size = firstSample.getSize();
  if (size < 3) throw InvalidArgumentException(HERE) << "Error: sample too small. Sample should contains at least 3 elements";
  // As we rely on a linear model result, we should be very generic
  // Instead of using input dimension, one should use parameter size
  const UnsignedInteger df = linearModelResult.getDegreesOfFreedom();
  // Residuals
  const Sample residualSample(linearModelResult.getSampleResiduals());
  // Compute mean & standard deviation
  const Scalar mean = residualSample.computeMean()[0];
  const Scalar std = residualSample.computeStandardDeviationPerComponent()[0];
  // The statistical test checks whether the mean is 0 or not
  // H0 : mu = 0
  // H1 : mu < 0 or mu > 0
  // The statstic of test is then (xm - 0) / sd * sqrt(size)
  // The statistics follows a Student distribution
  const Scalar statistic = std::abs(mean) * std::sqrt(size * 1.0) / std;
  Log::Debug(OSS() << "t-statistic = " << statistic);
  const Scalar pValue =  2.0 * DistFunc::pStudent(df, statistic, true);
  return TestResult("ResidualMean", pValue > level, pValue, level, statistic);
}

/*  */
TestResult LinearModelTest::LinearModelResidualMean(const Sample & firstSample,
    const Sample & secondSample,
    const Scalar level)
{
  if (firstSample.getSize() != secondSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
  if (secondSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  const UnsignedInteger size = firstSample.getSize();
  const UnsignedInteger dimension = firstSample.getDimension();
  if (size < 3) throw InvalidArgumentException(HERE) << "Error: sample too small. Sample should contains at least 3 elements";
  if (size < dimension + 1) throw InvalidArgumentException(HERE) << "Error: sample too small. Size should be greater than degree of freedom";

  LinearModelAlgorithm algo(firstSample, secondSample);
  algo.run();
  const LinearModelResult result(algo.getResult());
  return LinearModelResidualMean(firstSample, secondSample, result, level);
}

/*  */
TestResult LinearModelTest::LinearModelHarrisonMcCabe(const Sample & firstSample,
    const Sample & secondSample,
    const LinearModelResult & linearModelResult,
    const Scalar level,
    const Scalar breakPoint,
    const Scalar simulationSize)
{
  if (firstSample.getSize() != secondSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
  if (secondSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  const UnsignedInteger residualSize = firstSample.getSize();
  if (residualSize < 3) throw InvalidArgumentException(HERE) << "Error: sample too small. Sample should contains at least 3 elements";
  if (!(breakPoint > 0) && !(breakPoint < 1)) throw InvalidArgumentException(HERE) << "breakPoint should be in ]0,1[";
  const Sample residuals(linearModelResult.getSampleResiduals());

  /* Split the sample using the breakPoint*/
  const UnsignedInteger breakIndex = std::floor(residualSize * breakPoint);

  Scalar sumSelectResiduals = 0;
  for(UnsignedInteger i = 0; i < breakIndex; ++i)
  {
    // 1D algo : useless to create point twice!
    sumSelectResiduals += residuals(i, 0) * residuals(i, 0);
  }

  const Scalar sumSquaredResiduals = residuals.computeVariance()[0] * (residualSize - 1);

  /* compute Harrison McCabe statistic */
  const Scalar hmc = sumSelectResiduals / sumSquaredResiduals;

  /* p-value computed by simultation */
  Scalar pValue = 0;
  for(UnsignedInteger i = 0; i < simulationSize; ++i)
  {
    const Sample sample(Normal().getSample(residualSize));
    const Sample standardSample((sample - sample.computeMean()) / sample.computeStandardDeviationPerComponent());
    Scalar sumSelectResidualsSimulation = 0;
    for (UnsignedInteger j = 0; j < breakIndex; ++ j)
    {
      sumSelectResidualsSimulation += standardSample(j, 0) * standardSample(j, 0);
    }
    const Scalar sumSquaredResidualsSimulation = standardSample.computeVariance()[0] * (residualSize - 1);
    const Scalar statistic = sumSelectResidualsSimulation / sumSquaredResidualsSimulation;
    if(statistic < hmc)
    {
      pValue += 1.0;
    }
  }
  pValue = pValue / simulationSize;

  return TestResult("HarrisonMcCabe", pValue > level, pValue, level, hmc);
}

/*  */
TestResult LinearModelTest::LinearModelHarrisonMcCabe(const Sample & firstSample,
    const Sample & secondSample,
    const Scalar level,
    const Scalar breakPoint,
    const Scalar simulationSize)
{
  LinearModelAlgorithm algo(firstSample, secondSample);
  const LinearModelResult result(algo.getResult());
  return LinearModelHarrisonMcCabe(firstSample, secondSample,
                                   result,
                                   level,
                                   breakPoint,
                                   simulationSize);
}

/*  */
TestResult LinearModelTest::LinearModelBreuschPagan(const Sample & firstSample,
    const Sample & secondSample,
    const LinearModelResult & linearModelResult,
    const Scalar level)
{
  if (firstSample.getSize() != secondSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
  if (secondSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  const UnsignedInteger size = firstSample.getSize();
  if (size < 3) throw InvalidArgumentException(HERE) << "Error: sample too small. Sample should contains at least 3 elements";

  // Regression coefficient
  const Function fHat(linearModelResult.getMetaModel());
  const Sample yHat(fHat(firstSample));
  const Sample residuals(secondSample - yHat);

  const UnsignedInteger residualSize = firstSample.getSize();

  /* compute variance of the residuals*/
  const Scalar residualsVariance = residuals.computeVariance()[0];

  Sample w(residualSize, 1);
  for(UnsignedInteger i = 0; i < residualSize; ++i)
  {
    w(i, 0) = (residuals(i, 0) * residuals(i, 0) - residualsVariance);
  }

  /* Build a linear model on the squared residuals */
  LinearModelAlgorithm algo(firstSample, linearModelResult.getBasis(), w);
  const LinearModelResult result(algo.getResult());
  /* Predicted values of the squared residuals*/
  const Sample wPredicted(result.getFittedSample());
  /* Compute variances */
  const Scalar wPredictedVar = wPredicted.computeVariance()[0];
  const Scalar wVariance = w.computeVariance()[0];
  /* Compute the Breusch Pagan statistic */
  const Scalar statistic = residualSize * wPredictedVar / wVariance;
  /* Get the basis size */
  const UnsignedInteger basisSize = linearModelResult.getBasis().getSize();
  /* Compute the p-value */
  const Scalar pValue = DistFunc::pGamma(0.5 * (basisSize - 1), 0.5 * statistic, true);

  return TestResult("BreuschPagan", pValue > level, pValue, level, statistic);
}


/*  */
TestResult LinearModelTest::LinearModelBreuschPagan(const Sample & firstSample,
    const Sample & secondSample,
    const Scalar level)
{
  LinearModelAlgorithm algo(firstSample, secondSample);
  const LinearModelResult result(algo.getResult());
  return LinearModelBreuschPagan(firstSample, secondSample,
                                 result,
                                 level);
}


/* Durbin-Watson test
   Testing for Serial Correlation in Least Squares Regression. II
   By J. Durbin and G.S. Watson
   Biometrika Vol 38 No 1/2 (Hun 1951) pp 159-177   */
TestResult LinearModelTest::LinearModelDurbinWatson(const Sample & firstSample,
    const Sample & secondSample,
    const LinearModelResult & linearModelResult,
    const String hypothesis,
    const Scalar level)
{
  if (firstSample.getSize() != secondSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
  if (secondSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  const UnsignedInteger residualSize = firstSample.getSize();

  if (residualSize < 3) throw InvalidArgumentException(HERE) << "Error: sample too small. Sample should contains at least 3 elements";

  const Function fHat(linearModelResult.getMetaModel());
  const Sample yHat(fHat(firstSample));
  const Sample residuals(secondSample - yHat);


  const Scalar sumSquaredResiduals = residuals.computeVariance()[0] * (residualSize - 1);

  Scalar sumSquaredDifference = 0;
  for(UnsignedInteger i = 1; i < residualSize; ++i)
  {
    const Scalar residualDifference = residuals(i, 0) - residuals(i - 1, 0);
    sumSquaredDifference += residualDifference * residualDifference;
  }

  /* Compute the Durbin Watson statistic */
  const Scalar dw = sumSquaredDifference / sumSquaredResiduals;

  /* Normal approximation of dw to compute the p-value*/
  /* Create the design matrix */
  const Basis psi(linearModelResult.getBasis());
  const UnsignedInteger basisSize = psi.getSize();

  // Define the design proxy
  DesignProxy proxy(firstSample, psi);
  // The design proxy evaluated on the basis function
  // Define the design matrix
  Indices indices(basisSize);
  indices.fill();
  const Matrix X(proxy.computeDesign(indices));

  // We compute A * X
  // A is a real sym matrix (1D finite differences like)
  // A = [1 -1 0 . . . 0]
  //     [-1 2 1 . . . 0]
  //     [0 -1 2 1 . . 0]
  //     [. . -1 2 1 . 0]
  //     [0 . . . . -1 1]
  Matrix AX(residualSize, basisSize);
  for(UnsignedInteger j = 0; j < basisSize; ++j)
  {
    AX(0, j) = X(0, j) - X(1, j);
    AX(residualSize - 1, j) = X(residualSize - 1, j) - X(residualSize - 2, j);
  }
  for(UnsignedInteger j = 0; j < basisSize; ++j)
  {
    for(UnsignedInteger i = 0; i < residualSize - 2; ++i)
    {
      AX(i + 1, j) = -X(i, j) + 2 * X(i + 1, j) - X(i + 2, j);
    }
  }

  // Normal approximation of the dw statistic
  // Eval dmean & dvar
  CovarianceMatrix XtX(X.computeGram());
  const SquareMatrix XAXQt(XtX.solveLinearSystem(AX.transpose() * X).getImplementation());
  const Scalar P = 2 * (residualSize - 1) - XAXQt.computeTrace();
  const Scalar XAXTrace = XtX.solveLinearSystem(AX.computeGram(), false).getImplementation()->computeTrace();
  const Scalar Q = 2 * (3 * residualSize - 4) - 2 * XAXTrace + (XAXQt * XAXQt).getImplementation()->computeTrace();
  const Scalar dmean = P / (residualSize - basisSize);
  const Scalar dvar = 2.0 / ((residualSize - basisSize) * (residualSize - basisSize + 2)) * (Q - P * dmean);

  /* Compute the p-value with respect to the hypothesis */
  Scalar pValue = 0.0;
  Description description(1);
  Scalar statistic;
  if (hypothesis == "Equal")
  {
    statistic = std::abs(dw - dmean) / std::sqrt(dvar);
    pValue = 2.0 * DistFunc::pNormal(statistic, true);
    description[0] = "H0: auto.cor=0";
  }
  else if(hypothesis == "Less")
  {
    statistic = (dw - dmean) / std::sqrt(dvar);
    pValue = DistFunc::pNormal(statistic);
    description[0] = "H0: auto.cor<0";
  }
  else if(hypothesis == "Greater")
  {
    statistic = (dw - dmean) / std::sqrt(dvar);
    pValue = DistFunc::pNormal(statistic, true);
    description[0] = "H0: auto.cor>0";
  }
  else
    throw InvalidArgumentException(HERE) << "Invalid hypothesis string, use Equal|Less|Greater";

  /* Set test result */
  TestResult result("DurbinWatson", pValue > level, pValue, level, statistic);
  result.setDescription(description);
  return result;
}


/*  */
TestResult LinearModelTest::LinearModelDurbinWatson(const Sample & firstSample,
    const Sample & secondSample,
    const String hypothesis,
    const Scalar level)
{
  LinearModelAlgorithm algo(firstSample, secondSample);
  const LinearModelResult result(algo.getResult());
  return LinearModelDurbinWatson(firstSample, secondSample,
                                 result,
                                 hypothesis,
                                 level);
}


/* Regression test between 2 samples : firstSample of dimension n and secondSample of dimension 1. If firstSample[i] is the numerical sample extracted from firstSample (ith coordinate of each point of the numerical sample), PartialRegression performs the Regression test simultaneously on all firstSample[i] and secondSample, for i in the selection. The Regression test tests ifthe regression model between two scalar numerical samples is significant. It is based on the deviation analysis of the regression. The Fisher distribution is used. */
LinearModelTest::TestResultCollection LinearModelTest::PartialRegression(const Sample & firstSample,
    const Sample & secondSample,
    const Indices & selection,
    const Scalar level)
{
  LinearModelAlgorithm algo(firstSample.getMarginal(selection), secondSample);
  const LinearModelResult result(algo.getResult());
  const LinearModelAnalysis analysis(result);
  const UnsignedInteger size = selection.getSize() + 1;
  const Point pValues(analysis.getCoefficientsPValues());
  const Point statistics(analysis.getCoefficientsTScores());

  // Then, build the collection of results
  TestResultCollection resultCollection;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const Scalar pValue = pValues[i];
    const Scalar statistic = statistics[i];
    resultCollection.add(TestResult("Regression", pValue > level, pValue, level, statistic));
  }
  return resultCollection;
}

/* Regression test between 2 samples : firstSample of dimension n and secondSample of dimension 1. If firstSample[i] is the numerical sample extracted from firstSample (ith coordinate of each point of the numerical sample), FullRegression performs the Regression test simultaneously on all firstSample[i] and secondSample. The Regression test tests if the regression model between two scalar numerical samples is significant. It is based on the deviation analysis of the regression. The Fisher distribution is used. */
LinearModelTest::TestResultCollection LinearModelTest::FullRegression(const Sample & firstSample,
    const Sample & secondSample,
    const Scalar level)
{
  const UnsignedInteger dimension = firstSample.getDimension();
  Indices selection(dimension);
  selection.fill();
  return PartialRegression(firstSample, secondSample, selection, level);
}

END_NAMESPACE_OPENTURNS
