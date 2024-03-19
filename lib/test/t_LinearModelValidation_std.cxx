//                                               -*- C++ -*-
/**
 *  @brief The test file of class LinearModelAlgorithm
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
    const UnsignedInteger samplingSize = foldRootSize * kFoldParameter + 1;
    fullprint << "samplingSize = " << samplingSize << std::endl;
    ComposedDistribution::DistributionCollection aCollection;
    Uniform marginal1(-1.0, 1.0);
    aCollection.add( marginal1 );
    aCollection.add( marginal1 );
    ComposedDistribution distribution(aCollection);
    Sample inputSample(distribution.getSample(samplingSize));
    fullprint << "inputSample=" << inputSample << std::endl;
    Description inputVariables(2);
    inputVariables[0] = "x1";
    inputVariables[1] = "x2";
    Description formula(1);
    formula[0] = (OSS() << "3 - 2 * x1 + x2");
    SymbolicFunction g(inputVariables, formula);
    fullprint << "g=" << g << std::endl;
    Normal noise(0.0, 0.5);
    Sample outputSample(g(inputSample) + noise.getSample(samplingSize));
    fullprint << "outputSample=" << outputSample << std::endl;
    LinearModelAlgorithm lmAlgo(inputSample, outputSample);
    LinearModelResult result(lmAlgo.getResult());
    
    // Create LOO validation
    LinearModelValidation validationLOO(result, LinearModelValidation::LEAVEONEOUT);
    fullprint << validationLOO.__str__() << std::endl;
    assert_equal(validationLOO.getMethod(), UnsignedInteger(LinearModelValidation::LEAVEONEOUT));

    // Compute analytical LOO MSE
    fullprint << "Compute Analytical LOO MSE" << std::endl;
    const Point mseLOOAnalytical(validationLOO.computeMeanSquaredError());
    fullprint << "Analytical LOO MSE = " << mseLOOAnalytical << std::endl;

    // Compute naive leave-one-out
    Point residualsLOO(samplingSize);
    for (UnsignedInteger j = 0; j < samplingSize; ++j)
    {
      Indices indicesLOO(samplingSize);
      indicesLOO.fill();
      indicesLOO.erase(indicesLOO.begin() + j, indicesLOO.begin() + j + 1);
      const Sample inputSampleTrainLOO(inputSample.select(indicesLOO));
      const Sample outputSampleTrainLOO(outputSample.select(indicesLOO));
      const Point inputPointLOOTest(inputSample[j]);
      const Point outputPointLOOTest(outputSample[j]);
      LinearModelAlgorithm lmAlgoLOO(inputSampleTrainLOO, outputSampleTrainLOO);
      LinearModelResult resultLOO(lmAlgoLOO.getResult());
      const Function metamodelLOO = resultLOO.getMetaModel();
      const Point predictionLOOTest = metamodelLOO(inputPointLOOTest);
      const Point residualsLOOTest(predictionLOOTest - outputPointLOOTest);
      residualsLOO[j] = residualsLOOTest[0];
    }
    Scalar mseLOOnaive = residualsLOO.normSquare() / samplingSize;
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
    LinearModelValidation validationKFold(result, LinearModelValidation::KFOLD, kFoldParameter);
    fullprint << validationKFold.__str__() << std::endl;
    assert_equal(validationKFold.getKParameter(), kFoldParameter);
    assert_equal(validationKFold.getMethod(), UnsignedInteger(LinearModelValidation::KFOLD));

    // Compute analytical KFold MSE
    const Point mseKFoldAnalytical(validationKFold.computeMeanSquaredError());
    fullprint << "Analytical KFold MSE=" << mseKFoldAnalytical << std::endl;

    // Naive KFold
    Point residualsKFold(samplingSize);
    KFoldSplitter splitter(samplingSize, kFoldParameter);
    for (UnsignedInteger foldIndex = 0; foldIndex < kFoldParameter; ++foldIndex)
    {
      Indices indicesTest;
      const Indices indicesTrain(splitter.generate(indicesTest));
      const Sample inputSampleKFoldTrain(inputSample.select(indicesTrain));
      const Sample outputSampleKFoldTrain(outputSample.select(indicesTrain));
      const Sample inputSampleKFoldTest(inputSample.select(indicesTest));
      const Sample outputSampleKFoldTest(outputSample.select(indicesTest));
      LinearModelAlgorithm lmAlgoKFold(inputSampleKFoldTrain, outputSampleKFoldTrain);
      LinearModelResult resultKFold(lmAlgoKFold.getResult());
      const Function metamodelKFold(resultKFold.getMetaModel());
      const Sample predictionKFoldTest(metamodelKFold(inputSampleKFoldTest));
      const Point residualsKFoldTest(predictionKFoldTest.asPoint() - outputSampleKFoldTest.asPoint());
      const UnsignedInteger foldSize = indicesTest.getSize();
      for (UnsignedInteger localIndex = 0; localIndex < foldSize; ++localIndex)
        residualsKFold[indicesTest[localIndex]] = residualsKFoldTest[localIndex];
    }
    Scalar mseKFoldnaive(residualsKFold.normSquare() / samplingSize);
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
