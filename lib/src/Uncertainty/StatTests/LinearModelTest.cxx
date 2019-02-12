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
#include "openturns/LinearCombinationFunction.hxx"
#include "openturns/Path.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/Log.hxx"
#include "openturns/LinearModelAnalysis.hxx"

BEGIN_NAMESPACE_OPENTURNS


LinearModelTest::LinearModelTest()
{
}

/*  */
TestResult LinearModelTest::LinearModelFisher(const Sample & firstSample,
    const Sample & secondSample,
    const Point & trendCoefficients,
    const Scalar level)
{
  const UnsignedInteger dimension = firstSample.getDimension();
  if (secondSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  if (firstSample.getSize() != secondSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
  const UnsignedInteger size = firstSample.getSize();
  if (size < 3) throw InvalidArgumentException(HERE) << "Error: sample too small. Sample should contains at least 3 elements";
  if (size < dimension + 1) throw InvalidArgumentException(HERE) << "Error: sample too small. Size should be greater than degree of freedom";
  const UnsignedInteger df = size - dimension - 1;

  // Regression coefficient
  const LinearCombinationFunction fHat(LinearBasisFactory(dimension).build(), trendCoefficients);
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
  const Scalar statistic = sumSquaredResiduals / dimension / (sumSquaredExplained / (df));
  Log::Debug(OSS() << "F-statistic = " << statistic);
  const Scalar pValue =  FisherSnedecor(dimension, size - dimension - 1).computeComplementaryCDF(statistic);
  return TestResult("Fisher", pValue > level, pValue, level);
}

/* @deprecated */
TestResult LinearModelTest::LinearModelFisher(const Sample & firstSample,
    const Sample & secondSample,
    const LinearModel & linearModel,
    const Scalar level)
{
  LOGWARN(OSS() << "LinearModelFisher(..., LinearModel) is deprecated");
  return LinearModelFisher(firstSample, secondSample, linearModel.getRegression());
}

/*  */
TestResult LinearModelTest::LinearModelFisher(const Sample & firstSample,
    const Sample & secondSample,
    const Scalar level)
{

  const UnsignedInteger dimension = firstSample.getDimension();
  if (secondSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  if (firstSample.getSize() != secondSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
  const UnsignedInteger size = firstSample.getSize();
  if (size < 3) throw InvalidArgumentException(HERE) << "Error: sample too small. Sample should contains at least 3 elements";
  if (size < dimension + 1) throw InvalidArgumentException(HERE) << "Error: sample too small. Size should be greater than degree of freedom";
  const UnsignedInteger df = size - dimension - 1;


  LinearLeastSquares regressionAlgorithm(firstSample , secondSample);
  regressionAlgorithm.run();
  // Regression coefficient
  const Sample yHat(regressionAlgorithm.getResponseSurface()(firstSample)); 
  const Sample residualSample(secondSample - yHat);
 
  // For the Fisher test, we need both Sum of Squared Explained (SSE)
  // and the Sum of Squared Residuals
  const Scalar sumSquaredExplained = (yHat - secondSample.computeMean()).computeRawMoment(2)[0] * firstSample.getSize() ;
  const Scalar sumSquaredResiduals = residualSample.computeRawMoment(2)[0] * firstSample.getSize() ;
  // The statistical test checks the nullity of the regression linear model coefficients
  // H0 : Beta_i = 0
  // H1 : Beta_i < 0 or Beta_i > 0
  // The statistics follows a Fisher distribution
  const Scalar numerator = sumSquaredExplained / dimension;
  const Scalar denumerator = sumSquaredResiduals / df;
  const Scalar statistic = numerator / denumerator;
  Log::Debug(OSS() << "F-statistic = " << statistic);
  const Scalar pValue =  FisherSnedecor(dimension, df).computeComplementaryCDF(statistic);
  return TestResult("Fisher", pValue > level, pValue, level);
}

/*  */
TestResult LinearModelTest::LinearModelResidualMean(const Sample & firstSample,
    const Sample & secondSample,
    const Point & trendCoefficients,
    const Scalar level)
{
  const UnsignedInteger dimension = firstSample.getDimension();
  if (secondSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  if (firstSample.getSize() != secondSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
    const UnsignedInteger size = firstSample.getSize();
  if (size < 3) throw InvalidArgumentException(HERE) << "Error: sample too small. Sample should contains at least 3 elements";
  if (size < dimension + 1) throw InvalidArgumentException(HERE) << "Error: sample too small. Size should be greater than degree of freedom";
  const UnsignedInteger df = size - dimension - 1;

  // Regression coefficient
  const LinearCombinationFunction fHat(LinearBasisFactory(dimension).build(), trendCoefficients);
  const Sample residualSample(secondSample - fHat(firstSample));
  // Compute mean & standard deviation
  const Scalar mean = residualSample.computeMean()[0];
  const Scalar std = residualSample.computeStandardDeviationPerComponent()[0];
  // The statistical test checks whether the mean is 0 or not
  // H0 : mu = 0
  // H1 : mu < 0 or mu > 0
  // The statstic of test is then (xm - 0) / sd * sqrt(n)
  // The statistics follows a Student distribution
  const Scalar statistic = mean / std * std::sqrt(firstSample.getSize() * 1.0);
  Log::Debug(OSS() << "t-statistic = " << statistic);
  const Scalar pValue =  2.0 * DistFunc::pStudent(df, statistic, true);
  return TestResult("ResidualMean", pValue > level, pValue, level);
}

/* @deprecated */
TestResult LinearModelTest::LinearModelResidualMean(const Sample & firstSample,
    const Sample & secondSample,
    const LinearModel & linearModel,
    const Scalar level)
{
  LOGWARN(OSS() << "LinearModelResidualMean(..., LinearModel) is deprecated");
  return LinearModelResidualMean(firstSample, secondSample, linearModel.getRegression(), level);
}

/*  */
TestResult LinearModelTest::LinearModelResidualMean(const Sample & firstSample,
    const Sample & secondSample,
    const Scalar level)
{
  const UnsignedInteger dimension = firstSample.getDimension();
  if (secondSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  if (firstSample.getSize() != secondSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
  const UnsignedInteger size = firstSample.getSize();
  if (size < 3) throw InvalidArgumentException(HERE) << "Error: sample too small. Sample should contains at least 3 elements";
  if (size < dimension + 1) throw InvalidArgumentException(HERE) << "Error: sample too small. Size should be greater than degree of freedom";
  const UnsignedInteger df = firstSample.getSize() - dimension - 1;

  LinearLeastSquares regressionAlgorithm(firstSample , secondSample);
  regressionAlgorithm.run();
  const Sample residualSample(secondSample - regressionAlgorithm.getResponseSurface()(firstSample));
  // Compute mean & standard deviation
  const Scalar mean = residualSample.computeMean()[0];
  const Scalar std = residualSample.computeStandardDeviationPerComponent()[0];
  // The statistical test checks whether the mean is 0 or not
  // H0 : mu = 0
  // H1 : mu < 0 or mu > 0
  // The statstic of test is then (xm - 0) / sd * sqrt(n)
  // The statistics follows a Student distribution
  const Scalar statistic = mean / std * std::sqrt(firstSample.getSize() * 1.0);
  const Scalar pValue =  2.0 * DistFunc::pStudent(df, statistic, true);
  Log::Debug(OSS() << "t-statistic = " << statistic);
  return TestResult("ResidualMean", pValue > level, pValue, level);
}

/*  */
TestResult LinearModelTest::LinearModelHarrisonMcCabe(const Sample & firstSample,
    const Sample & secondSample,
    const Point & trendCoefficients,
    const Scalar level,
    const Scalar breakPoint,
    const Scalar simulationSize)
{
  if (trendCoefficients.getSize() != firstSample.getDimension()+1) throw InvalidArgumentException(HERE) << "Not enough trend coefficients";
  const LinearCombinationFunction fHat(LinearBasisFactory(trendCoefficients.getSize() - 1).build(), trendCoefficients);
  const Sample yHat(fHat(firstSample)); 
  const Sample residuals(secondSample - yHat);

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

/* @deprecated  */
TestResult LinearModelTest::LinearModelHarrisonMcCabe(const Sample & firstSample,
    const Sample & secondSample,
    const LinearModel & linearModel,
    const Scalar level,
    const Scalar breakPoint,
    const Scalar simulationSize)
{
  LOGWARN(OSS() << "LinearModelHarrisonMcCabe(..., LinearModel) is deprecated");
  return LinearModelHarrisonMcCabe(firstSample, secondSample, linearModel.getRegression(), level, breakPoint, simulationSize);
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
                                   result.getTrendCoefficients(),
                                   level,
                                   breakPoint,
                                   simulationSize);
}

/*  */
TestResult LinearModelTest::LinearModelBreuschPagan(const Sample & firstSample,
    const Sample & secondSample,
    const Point & trendCoefficients,
    const Scalar level)
{
  // Regression coefficient
  if (trendCoefficients.getSize() != firstSample.getDimension()+1) throw InvalidArgumentException(HERE) << "Not enough trend coefficients";
  const LinearCombinationFunction fHat(LinearBasisFactory(trendCoefficients.getDimension() - 1).build(), trendCoefficients);
  const Sample yHat(fHat(firstSample));
  const Sample residuals(secondSample - yHat);
  
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
  LinearModelAlgorithm algo(firstSample, w);
  const LinearModelResult result(algo.getResult());
  const LinearModel linearModelResiduals(result.getTrendCoefficients());
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


/* @deprecated */
TestResult LinearModelTest::LinearModelBreuschPagan(const Sample & firstSample,
    const Sample & secondSample,
    const LinearModel & linearModel,
    const Scalar level)
{
  LOGWARN(OSS() << "LinearModelBreuschPagan(..., LinearModel) is deprecated");
  return LinearModelBreuschPagan(firstSample, secondSample, linearModel.getRegression(), level);
}

/*  */
TestResult LinearModelTest::LinearModelBreuschPagan(const Sample & firstSample,
    const Sample & secondSample,
    const Scalar level)
{
  LinearModelAlgorithm algo(firstSample, secondSample);
  const LinearModelResult result(algo.getResult());
  return LinearModelBreuschPagan(firstSample, secondSample,
                                 result.getTrendCoefficients(),
                                 level);
}


/*  */
TestResult LinearModelTest::LinearModelDurbinWatson(const Sample & firstSample,
    const Sample & secondSample,
    const Point & trendCoefficients,
    const String hypothesis,
    const Scalar level)
{
  if (trendCoefficients.getSize() != firstSample.getDimension()+1) throw InvalidArgumentException(HERE) << "Not enough trend coefficients";
  const LinearCombinationFunction fHat(LinearBasisFactory(trendCoefficients.getDimension() - 1).build(), trendCoefficients);
  const Sample yHat(fHat(firstSample)); 
  const Sample residuals(secondSample - yHat);

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
  Scalar pValue = 0.0;
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
  else
    throw InvalidArgumentException(HERE) << "Invalid hypothesis string, use Equal|Less|Greater";

  /* Set test result */
  TestResult result("DurbinWatson", pValue > level, pValue, level);
  result.setDescription(description);
  return result;
}

/* @deprecated */
TestResult LinearModelTest::LinearModelDurbinWatson(const Sample & firstSample,
    const Sample & secondSample,
    const LinearModel & linearModel,
    const String hypothesis,
    const Scalar level)
{
  LOGWARN(OSS() << "LinearModelDurbinWatson(..., LinearModel) is deprecated");
  return LinearModelDurbinWatson(firstSample, secondSample, linearModel.getRegression(), hypothesis, level);
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
                                 result.getTrendCoefficients(),
                                 hypothesis,
                                 level);
}


/* Regression test between 2 samples : firstSample of dimension n and secondSample of dimension 1. If firstSample[i] is the numerical sample extracted from firstSample (ith coordinate of each point of the numerical sample), PartialRegression performs the Regression test simultaneously on all firstSample[i] and secondSample, for i in the selection. The Regression test tests ifthe regression model between two scalar numerical samples is significant. It is based on the deviation analysis of the regression. The Fisher distribution is used. */
LinearModelTest::TestResultCollection LinearModelTest::PartialRegression(const Sample & firstSample,
    const Sample & secondSample,
    const Indices & selection,
    const Scalar level)
{
  if (secondSample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the Regression test can be performed only with an 1-d output sample.";

  LinearModelAlgorithm algo(firstSample.getMarginal(selection), secondSample);
  const LinearModelResult result(algo.getResult());
  const LinearModelAnalysis analysis(result);
  const UnsignedInteger size = selection.getSize() + 1;
  const Point pValues(analysis.getCoefficientsPValues());

  // Then, build the collection of results
  TestResultCollection resultCollection;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const Scalar pValue = pValues[i];
    resultCollection.add(TestResult("Regression", pValue > level, pValue, level));
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
