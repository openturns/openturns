//                                               -*- C++ -*-
/**
 *  @brief The test file of FunctionalChaosValidation class
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/IshigamiUseCase.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    IshigamiUseCase useCase;
    Function model(useCase.getModel());

    // Create the input distribution
    UnsignedInteger dimension = useCase.getDimension();
    Distribution distribution(useCase.getInputDistribution());

    // Create the orthogonal basis
    Collection<OrthogonalUniVariatePolynomialFamily> polynomialCollection(dimension);
    polynomialCollection[0] = LegendreFactory();
    polynomialCollection[1] = LegendreFactory();
    polynomialCollection[2] = LegendreFactory();

    const LinearEnumerateFunction enumerateFunction(dimension);
    const OrthogonalProductPolynomialFactory productBasis(polynomialCollection, enumerateFunction);

    // Create the adaptive strategy
    // We can choose amongst several strategies
    // First, the most efficient (but more complex!) strategy
    const UnsignedInteger totalDegree = 8;
    const UnsignedInteger basisDimension = enumerateFunction.getBasisSizeFromTotalDegree(totalDegree);
    fullprint << "basisDimension = " << basisDimension << std::endl;
    const FixedStrategy adaptiveStrategy(productBasis, basisDimension);
    // Compute the sample size from number of folds to guarantee a non constant integer
    // number of points per fold
    const UnsignedInteger kFoldParameter = 10;
    UnsignedInteger foldSampleSize = 20;
    const UnsignedInteger sampleSize = foldSampleSize * kFoldParameter + 1;
    fullprint << "sampleSize = " << sampleSize << std::endl;
    // Create the projection strategy
    const MonteCarloExperiment experiment(distribution, sampleSize);
    RandomGenerator::SetSeed(0);
    const Sample inputSample(experiment.generate());
    const Sample outputSample(model(inputSample));

    // Create the polynomial chaos algorithm
    FunctionalChaosAlgorithm algo(inputSample, outputSample, distribution, adaptiveStrategy, LeastSquaresStrategy());
    algo.run();

    // Examine the results
    const FunctionalChaosResult chaosResult = algo.getResult();
    ResourceMap::SetAsUnsignedInteger("FunctionalChaosResult-PrintEllipsisThreshold", 20);
    fullprint << chaosResult.__repr_markdown__() << std::endl;

    // Analytical leave-one-out
    LeaveOneOutSplitter splitterLOO(sampleSize);
    FunctionalChaosValidation validationLOO(chaosResult, splitterLOO);
    const Point mseLOOAnalytical(validationLOO.computeMeanSquaredError());
    fullprint << "Analytical LOO MSE = " << mseLOOAnalytical << std::endl;
    assert_equal(validationLOO.getSplitter().getN(), sampleSize);

    // Naive leave-one-out
    Point residualsLOO(sampleSize);
    for (UnsignedInteger j = 0; j < sampleSize; ++j)
    {
      Indices indicesLOO(sampleSize);
      indicesLOO.fill();
      indicesLOO.erase(indicesLOO.begin() + j, indicesLOO.begin() + j + 1);
      const Sample inputSampleTrainLOO(inputSample.select(indicesLOO));
      const Sample outputSampleTrainLOO(outputSample.select(indicesLOO));
      const Point inputPointLOOTest(inputSample[j]);
      const Point outputPointLOOTest(outputSample[j]);
      FunctionalChaosAlgorithm algoLOO(inputSampleTrainLOO, outputSampleTrainLOO, distribution, adaptiveStrategy, LeastSquaresStrategy());
      algoLOO.run();
      const FunctionalChaosResult resultLOO(algoLOO.getResult());
      const Function metamodelLOO(resultLOO.getMetaModel());
      const Point predictionLOOTest(metamodelLOO(inputPointLOOTest));
      const Point residualsLOOTest(predictionLOOTest - outputPointLOOTest);
      residualsLOO[j] = residualsLOOTest[0];
    }
    Point mseLOOnaive(1);
    mseLOOnaive[0] = residualsLOO.normSquare() / sampleSize;
    fullprint << "Naive LOO MSE = " << mseLOOnaive << std::endl;

    // Test
    Scalar rtolLOO = 1.e-8;
    Scalar atolLOO = 0.0;
    assert_almost_equal(mseLOOAnalytical, mseLOOnaive, rtolLOO, atolLOO);

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

    // Analytical leave-one-out with default constructor
    FunctionalChaosValidation validationLOODefault(chaosResult);
    const Point r2ScoreLOODefault(validationLOODefault.computeR2Score());
    assert_almost_equal(r2ScoreReference, r2ScoreLOODefault, rtolLOO, atolLOO);

    // Analytical K-Fold
    KFoldSplitter splitterKF(sampleSize, kFoldParameter);
    FunctionalChaosValidation validationKFold(chaosResult, splitterKF);
    fullprint << "KFold with K = " << kFoldParameter << std::endl;
    assert_equal(validationKFold.getSplitter().getN(), sampleSize);
    // assert_equal(validationKFold.getSplitter().getSize(), kFoldParameter);  // TODO: fix this

    // Compute mean squared error
    const Point mseKFoldAnalytical(validationKFold.computeMeanSquaredError());
    fullprint << "Analytical KFold MSE = " << mseKFoldAnalytical << std::endl;

    // Naive KFold
    Point residualsKFold(sampleSize);
    KFoldSplitter splitter(sampleSize, kFoldParameter);
    for (UnsignedInteger foldIndex = 0; foldIndex < kFoldParameter; ++foldIndex)
    {
      Indices indicesTest;
      const Indices indicesTrain(splitter.generate(indicesTest));
      const UnsignedInteger foldSize = indicesTest.getSize();
      const Sample inputSampleKFoldTrain(inputSample.select(indicesTrain));
      const Sample outputSampleKFoldTrain(outputSample.select(indicesTrain));
      const Sample inputSampleKFoldTest(inputSample.select(indicesTest));
      const Sample outputSampleKFoldTest(outputSample.select(indicesTest));
      FunctionalChaosAlgorithm algoKFold(inputSampleKFoldTrain, outputSampleKFoldTrain, distribution, adaptiveStrategy, LeastSquaresStrategy());
      algoKFold.run();
      const FunctionalChaosResult resultKFold(algoKFold.getResult());
      const Function metamodelKFold(resultKFold.getMetaModel());
      const Sample predictionKFoldTest(metamodelKFold(inputSampleKFoldTest));
      const Point residualsKFoldTest(predictionKFoldTest.asPoint() - outputSampleKFoldTest.asPoint());
      for (UnsignedInteger localIndex = 0; localIndex < foldSize; ++localIndex)
        residualsKFold[indicesTest[localIndex]] = residualsKFoldTest[localIndex];
    }
    Point mseKFoldnaive(1);
    mseKFoldnaive[0] = residualsKFold.normSquare() / sampleSize;
    fullprint << "Naive KFold MSE = " << mseKFoldnaive << std::endl;

    // Test
    Scalar rtolKFold = 1.e-5;
    Scalar atolKFold = 0.0;
    assert_almost_equal(mseKFoldAnalytical, mseKFoldnaive, rtolKFold, atolKFold);

    // Check K-Fold R2
    const Point r2ScoreKFold(validationKFold.computeR2Score());
    fullprint << "Analytical K-Fold R2 score = " << r2ScoreKFold << std::endl;
    r2ScoreReference[0] = 1.0 - mseKFoldAnalytical[0] / sampleVariance[0];
    fullprint << "Computed R2 score = " << r2ScoreReference << std::endl;
    rtolKFold = 1.e-12;
    atolKFold = 0.0;
    assert_almost_equal(r2ScoreReference, r2ScoreKFold, rtolKFold, atolKFold);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
