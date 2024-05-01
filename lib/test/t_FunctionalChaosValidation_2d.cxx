//                                               -*- C++ -*-
/**
 *  @brief The test file of FunctionalChaosValidation class
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
#include "openturns/IshigamiUseCase.hxx"
#include "openturns/GSobolUseCase.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {

    IshigamiUseCase useCaseIshigami;
    UnsignedInteger inputDimension = useCaseIshigami.getDimension();
    const Point a({1.0, 10.0, 100.0});
    GSobolUseCase useCaseGSobol(inputDimension, a);
    const UnsignedInteger outputDimension = 2;
    Collection<Function> functionCollection(outputDimension);
    functionCollection[0] = useCaseIshigami.getModel();
    functionCollection[1] = useCaseGSobol.getModel();
    AggregatedFunction model(functionCollection);

    // Create the input distribution
    Distribution distribution(useCaseIshigami.getInputDistribution());

    // Create the orthogonal basis
    Collection<OrthogonalUniVariatePolynomialFamily> polynomialCollection(inputDimension);
    polynomialCollection[0] = LegendreFactory();
    polynomialCollection[1] = LegendreFactory();
    polynomialCollection[2] = LegendreFactory();

    const LinearEnumerateFunction enumerateFunction(inputDimension);
    const OrthogonalProductPolynomialFactory productBasis(polynomialCollection, enumerateFunction);

    // Create the adaptive strategy
    // We can choose amongst several strategies
    // First, the most efficient (but more complex!) strategy
    const UnsignedInteger basisDimension = 100;
    fullprint << "basisDimension = " << basisDimension << std::endl;
    const FixedStrategy adaptiveStrategy(productBasis, basisDimension);
    // Compute the sample size from number of folds to guarantee a constant integer
    // number of points per fold
    const UnsignedInteger kFoldParameter = 10;
    UnsignedInteger foldSampleSize = 12;
    const UnsignedInteger sampleSize = foldSampleSize * kFoldParameter;
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
    Sample residualsLOOSample(sampleSize, outputDimension);
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
      residualsLOOSample[j] = residualsLOOTest;
    }
    Point mseLOOnaive(outputDimension);
    for (UnsignedInteger k = 0; k < outputDimension; ++k)
    {
      Point residualsLOO = residualsLOOSample.getMarginal(k).asPoint();
      mseLOOnaive[k] = residualsLOO.normSquare() / sampleSize;
    }
    fullprint << "Naive LOO MSE = " << mseLOOnaive << std::endl;
    
    // Test
    const Scalar rtolLOO = 1.e-10;
    const Scalar atolLOO = 0.0;
    assert_almost_equal(mseLOOAnalytical, mseLOOnaive, rtolLOO, atolLOO);
    
    // Analytical K-Fold
    KFoldSplitter splitterKF(sampleSize, kFoldParameter);
    FunctionalChaosValidation validationKFold(chaosResult, splitterKF);
    fullprint << "KFold with K = " << kFoldParameter << std::endl;
    const Point mseKFoldAnalytical(validationKFold.computeMeanSquaredError());
    fullprint << "Analytical KFold MSE = " << mseKFoldAnalytical << std::endl;
    assert_equal(validationKFold.getSplitter().getN(), sampleSize);
    // assert_equal(validationKFold.getSplitter().getSize(), kFoldParameter);  // TODO: fix this

    // Naive KFold
    Sample squaredResidualsKFold(sampleSize, outputDimension);
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
      for (UnsignedInteger k = 0; k < outputDimension; ++k)
      {
        const Function marginalMetamodel(metamodelKFold.getMarginal(k));
        const Sample marginalPredictionKFoldTest(marginalMetamodel(inputSampleKFoldTest));
        const Sample marginalOutputSampleKFoldTest = outputSampleKFoldTest.getMarginal(k);
        const Point residualsKFoldTest(marginalPredictionKFoldTest.asPoint() - marginalOutputSampleKFoldTest.asPoint());
        for (UnsignedInteger localIndex = 0; localIndex < foldSize; ++localIndex)
          squaredResidualsKFold(indicesTest[localIndex], k) = std::pow(residualsKFoldTest[localIndex], 2);
      }
    }
    Point mseKFoldnaive(squaredResidualsKFold.computeMean());
    fullprint << "Naive KFold MSE = " << mseKFoldnaive << std::endl;
    
    // Test
    const Scalar rtolKFold = 1.e-7;
    const Scalar atolKFold = 0.0;
    assert_almost_equal(mseKFoldAnalytical, mseKFoldnaive, rtolKFold, atolKFold);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
