//                                               -*- C++ -*-
/**
 *  @brief The test file of class LinearModelAlgorithm
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{

  TESTPREAMBLE;
  OStream fullprint(std::cout);

  {
    setRandomGenerator();
    fullprint << "Fit y ~ 3 - 2 x1 + x2 + epsilon" << std::endl;
    const UnsignedInteger kFoldParameter = 4;
    UnsignedInteger foldRootSize = 3;
    // Makes so that k does not divide the sample size.
    // In this case, we must take into account for the different weight of
    // each fold.
    const UnsignedInteger sampleSize = foldRootSize * kFoldParameter + 1;
    fullprint << "sampleSize = " << sampleSize << std::endl;
    JointDistribution::DistributionCollection aCollection;
    Uniform marginal1(-1.0, 1.0);
    aCollection.add(marginal1);
    aCollection.add(marginal1);
    JointDistribution distribution(aCollection);
    Sample inputSample(distribution.getSample(sampleSize));
    fullprint << "inputSample=" << inputSample << std::endl;
    Description inputVariables(2);
    inputVariables[0] = "x1";
    inputVariables[1] = "x2";
    Description formula(1);
    formula[0] = (OSS() << "3 - 2 * x1 + x2");
    SymbolicFunction g(inputVariables, formula);
    fullprint << "g=" << g << std::endl;
    Normal noise(0.0, 0.5);
    Sample outputSample(g(inputSample) + noise.getSample(sampleSize));
    fullprint << "outputSample=" << outputSample << std::endl;
    LinearModelAlgorithm lmAlgo(inputSample, outputSample);
    LinearModelResult result(lmAlgo.getResult());
    
    // Create LOO validation
    LeaveOneOutSplitter splitterLOO(sampleSize);
    LinearModelValidation validationLOO(result, splitterLOO);
    fullprint << validationLOO.__str__() << std::endl;

    // Compute analytical LOO MSE
    fullprint << "Compute Analytical LOO MSE" << std::endl;
    const Point mseLOOAnalytical(validationLOO.computeMeanSquaredError());
    fullprint << "Analytical LOO MSE = " << mseLOOAnalytical << std::endl;

    // Compute naive leave-one-out
    Point residualsLOO(sampleSize);
    Indices indicesLOOTest;
    for (UnsignedInteger j = 0; j < sampleSize; ++j)
    {
      const Indices indicesLOOTrain(splitterLOO.generate(indicesLOOTest));
      const Sample inputSampleTrainLOO(inputSample.select(indicesLOOTrain));
      const Sample outputSampleTrainLOO(outputSample.select(indicesLOOTrain));
      const Sample inputSampleLOOTest(inputSample.select(indicesLOOTest));
      const Sample outputSampleLOOTest(outputSample.select(indicesLOOTest));
      LinearModelAlgorithm lmAlgoLOO(inputSampleTrainLOO, outputSampleTrainLOO);
      LinearModelResult resultLOO(lmAlgoLOO.getResult());
      const Function metamodelLOO = resultLOO.getMetaModel();
      const Sample predictionLOOTest = metamodelLOO(inputSampleLOOTest);
      const Point residualsLOOTest(predictionLOOTest.asPoint() - outputSampleLOOTest.asPoint());
      residualsLOO[j] = residualsLOOTest[0];
    }
    Scalar mseLOOnaive = residualsLOO.normSquare() / sampleSize;
    fullprint << "Naive LOO MSE = " << mseLOOnaive << std::endl;
    
    // Test
    Scalar rtolLOO = 1.e-12;
    Scalar atolLOO = 0.0;
    assert_almost_equal(mseLOOAnalytical[0], mseLOOnaive, rtolLOO, atolLOO);

    // Check LOO R2
    const Point r2ScoreLOO(validationLOO.computeR2Score());
    fullprint << "Analytical LOO R2 score = " << r2ScoreLOO << std::endl;
    const Point sampleVariance(outputSample.computeCentralMoment(2));
    fullprint << "sampleVariance = " << sampleVariance << std::endl;
    Point r2ScoreReference(1);
    r2ScoreReference[0] = 1.0 - mseLOOAnalytical[0] / sampleVariance[0];
    fullprint << "Computed R2 score = " << r2ScoreReference << std::endl;
    rtolLOO = 1.e-12;
    atolLOO = 0.0;
    assert_almost_equal(r2ScoreReference, r2ScoreLOO, rtolLOO, atolLOO);

    // Create KFold validation
    KFoldSplitter splitterKF(sampleSize, kFoldParameter);
    LinearModelValidation validationKFold(result, splitterKF);
    fullprint << validationKFold.__str__() << std::endl;

    // Compute analytical KFold MSE
    const Point mseKFoldAnalytical(validationKFold.computeMeanSquaredError());
    fullprint << "Analytical KFold MSE=" << mseKFoldAnalytical << std::endl;

    // Naive KFold
    Point residualsKFold(sampleSize);
    KFoldSplitter splitter(sampleSize, kFoldParameter);
    Indices indicesKFoldTest;
    for (UnsignedInteger foldIndex = 0; foldIndex < kFoldParameter; ++foldIndex)
    {
      const Indices indicesKFoldTrain(splitter.generate(indicesKFoldTest));
      const Sample inputSampleKFoldTrain(inputSample.select(indicesKFoldTrain));
      const Sample outputSampleKFoldTrain(outputSample.select(indicesKFoldTrain));
      const Sample inputSampleKFoldTest(inputSample.select(indicesKFoldTest));
      const Sample outputSampleKFoldTest(outputSample.select(indicesKFoldTest));
      LinearModelAlgorithm lmAlgoKFold(inputSampleKFoldTrain, outputSampleKFoldTrain);
      LinearModelResult resultKFold(lmAlgoKFold.getResult());
      const Function metamodelKFold(resultKFold.getMetaModel());
      const Sample predictionKFoldTest(metamodelKFold(inputSampleKFoldTest));
      const Point residualsKFoldTest(predictionKFoldTest.asPoint() - outputSampleKFoldTest.asPoint());
      const UnsignedInteger foldSize = indicesKFoldTest.getSize();
      for (UnsignedInteger localIndex = 0; localIndex < foldSize; ++localIndex)
        residualsKFold[indicesKFoldTest[localIndex]] = residualsKFoldTest[localIndex];
    }
    Scalar mseKFoldnaive(residualsKFold.normSquare() / sampleSize);
    fullprint << "Naive KFold MSE = " << mseKFoldnaive << std::endl;
    
    // Test
    Scalar rtolKFold = 1.e-7;
    Scalar atolKFold = 0.0;
    assert_almost_equal(mseKFoldAnalytical[0], mseKFoldnaive, rtolKFold, atolKFold);

    // Check K-Fold R2
    const Point r2ScoreKFold(validationKFold.computeR2Score());
    fullprint << "Analytical K-Fold R2 score = " << r2ScoreKFold << std::endl;
    r2ScoreReference[0] = 1.0 - mseKFoldAnalytical[0] / sampleVariance[0];
    fullprint << "Computed R2 score = " << r2ScoreReference << std::endl;
    rtolKFold = 1.e-12;
    atolKFold = 0.0;
    assert_almost_equal(r2ScoreReference, r2ScoreKFold, rtolLOO, atolLOO);
  }

  return ExitCode::Success;

}
