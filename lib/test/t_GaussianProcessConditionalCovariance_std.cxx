//                                               -*- C++ -*-
/**
 *  @brief The test file of GaussianProcessConditionalCovariance class
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
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();
  ResourceMap::SetAsUnsignedInteger("OptimizationAlgorithm-DefaultMaximumCallsNumber", 20000);
  ResourceMap::SetAsScalar("Cobyla-DefaultRhoBeg", 0.5);
  ResourceMap::SetAsScalar("OptimizationAlgorithm-DefaultMaximumAbsoluteError", 1e-8);


  try
  {
    // Test 1: a real value function
    {
      // Set Numerical precision to 4
      PlatformInfo::SetNumericalPrecision(4);
      const UnsignedInteger sampleSize = 6;
      const UnsignedInteger dimension = 1;

      const SymbolicFunction f("x", "x * sin(x)");

      Sample inputTrain(sampleSize, dimension);
      Sample inputTest(sampleSize, dimension);
      for (UnsignedInteger i = 0; i < sampleSize; ++i)
      {
        inputTrain(i, 0) = 3.0 + i;
        inputTest(i, 0) = 2.5 + i;
      }
      inputTrain(0, 0) = 1.0;
      inputTrain(1, 0) = 3.0;
      // input test sample
      inputTest(0, 0) = 2.0;
      inputTest(1, 0) = 4.0;
      const Sample outputTrain(f(inputTrain));

      // create covariance model
      const Basis basis(ConstantBasisFactory(dimension).build());
      const SquaredExponential covarianceModel;

      // create algorithm
      GaussianProcessFitter fit_algo(inputTrain, outputTrain, covarianceModel, basis);

      // set sensible optimization bounds and estimate hyper parameters
      fit_algo.setOptimizationBounds(Interval(inputTrain.getMin(), inputTrain.getMax()));
      fit_algo.run();

      // perform an evaluation
      GaussianProcessRegression algo(fit_algo.getResult());
      algo.run();
      GaussianProcessRegressionResult result(algo.getResult());

      // GPCC
      GaussianProcessConditionalCovariance gccc(result);

      CovarianceMatrix covariance(gccc.getConditionalCovariance(inputTrain));
      SquareMatrix nullMatrix(sampleSize);
      assert_almost_equal(covariance, nullMatrix, 0.0, 1e-13);

      // Covariance per marginal & extract variance component
      Collection<CovarianceMatrix> coll(gccc.getConditionalMarginalCovariance(inputTrain));
      Point variance(coll.getSize());
      const Point nullVariance(coll.getSize());
      for (UnsignedInteger index = 0; index < coll.getSize(); ++ index)
        variance[index] = coll[index](0, 0);

      assert_almost_equal(variance, nullVariance, 1e-14, 1e-13);

      // Variance per marginal
      assert_almost_equal(gccc.getConditionalMarginalVariance(inputTrain), Sample(sampleSize, 1), 1e-14, 1e-13);


      // Kriging variance is non-null on validation points
      CovarianceMatrix validCovariance(gccc.getConditionalCovariance(inputTest));

      const Point rowData = {0.81942182, -0.35599947, -0.17488593, 0.04622401, -0.03143555, 0.04054783, \
                             -0.35599947, 0.20874735, 0.10943841, -0.03236419, 0.02397483, -0.03269184, \
                             -0.17488593, 0.10943841, 0.05832917, -0.01779918, 0.01355719, -0.01891618, \
                             0.04622401, -0.03236419, -0.01779918, 0.00578327, -0.00467674, 0.00688697, \
                             -0.03143555, 0.02397483, 0.01355719, -0.00467674, 0.0040267, -0.00631173, \
                             0.04054783, -0.03269184, -0.01891618, 0.00688697, -0.00631173, 0.01059488
                            };

      Matrix values(sampleSize, sampleSize, rowData);

      assert_almost_equal(validCovariance - values.transpose(), nullMatrix, 1.e-5, 1e-6);
    }
    // Test 2 : 2 inputs, one output
    {
      const UnsignedInteger dimension = 2;
      const Description inputDesc = {"x", "y"};
      const Description outputDesc = {"cos(0.5*x) + sin(y)"};
      const SymbolicFunction model(inputDesc, outputDesc);

      const Indices levels = {8, 5};
      const Box box(levels);
      Sample inputSample(box.generate());

      // Scale each direction
      inputSample *= 10.0;
      const Sample outputSample(model(inputSample));

      //Definition of exponential model
      // The parameters have been calibrated using Cobyla optimization
      // and AbsoluteExponential models
      const Point scale = {5.33532, 2.61534};
      const Point amplitude = {1.61536};
      SquaredExponential covarianceModel(scale, amplitude);

      // Basis definition
      const Basis basis(ConstantBasisFactory(dimension).build());

      // GP algorithm
      // create algorithm
      GaussianProcessFitter fit_algo(inputSample, outputSample, covarianceModel, basis);
      // set sensible optimization bounds and estimate hyper parameters
      fit_algo.setOptimizationBounds(Interval(inputSample.getMin(), inputSample.getMax()));
      fit_algo.run();

      // perform an evaluation
      GaussianProcessRegression algo(fit_algo.getResult());
      algo.run();

      GaussianProcessRegressionResult result(algo.getResult());

      // 5) Kriging variance is 0 on learning points
      GaussianProcessConditionalCovariance gccc(result);

      const CovarianceMatrix covariance(gccc.getConditionalCovariance(inputSample));
      assert_almost_equal(covariance, SquareMatrix(inputSample.getSize()), 7e-7, 7e-7);

      // Covariance per marginal & extract variance component
      Collection<CovarianceMatrix> condMargCov(gccc.getConditionalMarginalCovariance(inputSample));
      Point variance(condMargCov.getSize());
      const Point nullVariance(condMargCov.getSize());
      for (UnsignedInteger index = 0; index < condMargCov.getSize(); ++index)
        variance[index] = condMargCov[index](0, 0);

      assert_almost_equal(variance, nullVariance, 1e-14, 1e-13);

      // Variance per marginal
      assert_almost_equal(gccc.getConditionalMarginalVariance(inputSample), \
                          Sample(inputSample.getSize(), 1), 1e-14, 1e-13);

    }
    // Test 3 : R --> R^2 function
    {
      const Description inputDesc = {"x"};
      const Description outputDesc = {"x * sin(x)", "x * cos(x)"};
      const SymbolicFunction f(inputDesc, outputDesc);
      const UnsignedInteger sampleSize = 8;
      const UnsignedInteger dimension = 1;
      Sample sampleX(sampleSize, dimension);
      for (UnsignedInteger i = 0; i < sampleSize; ++i)
        sampleX(i, 0) = 1.0 + i;
      const Sample sampleY(f(sampleX));
      const Point scale = {1.0};
      const Point amplitude = {1.0};
      SquaredExponential kernel(scale, amplitude);
      kernel.setActiveParameter(Indices(0));
      Collection<CovarianceModel> collectionCov;
      collectionCov.add(kernel);
      collectionCov.add(kernel);
      TensorizedCovarianceModel covarianceModel(collectionCov);

      // Basis definition
      // Build a basis phi from R --> R^2
      // phi_{0,0} = phi_{0,1} = x
      //# phi_{1,0} = phi_{1,1} = x^2
      const AggregatedFunction phi0({SymbolicFunction("x", "x"), SymbolicFunction("x", "x")});
      const AggregatedFunction phi1({SymbolicFunction("x", "x^2"), SymbolicFunction("x", "x^2")});
      const Collection<Function> collFunctions = {phi0, phi1};
      const Basis basis(collFunctions);

      // GP algorithm
      // create algorithm
      GaussianProcessFitter fit_algo(sampleX, sampleY, covarianceModel, basis);
      fit_algo.run();

      // perform an evaluation
      GaussianProcessRegression algo(fit_algo.getResult());
      algo.run();

      GaussianProcessRegressionResult result(algo.getResult());

      // Kriging variance is 0 on learning points
      GaussianProcessConditionalCovariance gccc(result);

      const Point rowData = {4.4527, 0.0, 8.34404, 0.0, 0.0, 2.8883, 0.0, 5.41246, \
                             8.34404, 0.0, 15.7824, 0.0, 0.0, 5.41246, 0.0, 10.2375
                            };
      const Matrix reference_covariance(4, 4, rowData);
      const SquareMatrix nullMatrix(4);
      const Point pointOfInterest = {9.5, 10.0};
      const Sample sample(Sample::BuildFromPoint(pointOfInterest));
      const CovarianceMatrix covarianceMat(gccc(sample).getCovariance());
      assert_almost_equal(covarianceMat - reference_covariance.transpose(), nullMatrix, 0.0, 2e-2);

    }
    // stationary cov function - fix https://github.com/openturns/openturns/issues/1861
    {
      RandomGenerator::SetSeed(0);
      const SymbolicFunction rho("tau", "exp(-abs(tau))*cos(2*pi_*abs(tau))");
      const StationaryFunctionalCovarianceModel covarianceModel({1.0}, {1.0}, rho);
      Sample X(Normal(0, 1.0).getSample(20));
      X.setDescription({"J0"});
      Sample Y(Normal(0, 0.1).getSample(20));
      Y = Y + X;
      Y.setDescription({"G0"});

      // Basis definition
      const Basis basis(LinearBasisFactory(1).build());

      // GP algorithm
      // create algorithm
      GaussianProcessFitter fit_algo(X, Y, covarianceModel, basis);
      fit_algo.run();

      // perform an evaluation
      GaussianProcessRegression algo(fit_algo.getResult());
      algo.run();

      GaussianProcessRegressionResult result(algo.getResult());
      // GPCC
      GaussianProcessConditionalCovariance gccc(result);
      // Variance per marginal
      assert_almost_equal(gccc.getConditionalMarginalVariance(X), Sample(X.getSize(), 1), 1e-15, 1e-15);

    }
    // GPR with already calibrated parameters
    {
      const UnsignedInteger sampleSize = 6;
      const UnsignedInteger dimension = 1;

      const SymbolicFunction f("x", "x * sin(x)");

      Sample inputTrain(sampleSize, dimension);
      Sample inputTest(sampleSize, dimension);
      for (UnsignedInteger i = 0; i < sampleSize; ++i)
      {
        inputTrain(i, 0) = 3.0 + i;
        inputTest(i, 0) = 2.5 + i;
      }
      inputTrain(0, 0) = 1.0;
      inputTrain(1, 0) = 3.0;
      // input test sample
      inputTest(0, 0) = 2.0;
      inputTest(1, 0) = 4.0;
      const Sample outputTrain(f(inputTrain));

      // create covariance model
      const Basis basis(ConstantBasisFactory(dimension).build());
      const Point scale = {1.6326932047296538};
      const Point amplitude = {4.895995962015954};
      const SquaredExponential covarianceModel(scale, amplitude);

      const SymbolicFunction trend_function("x", "1.49543");

      // create covariance model

      // create algorithm
      GaussianProcessRegression algo(inputTrain, outputTrain, covarianceModel, trend_function);
      algo.run();
      GaussianProcessRegressionResult result(algo.getResult());
      // GPCC
      GaussianProcessConditionalCovariance gccc(result);

      CovarianceMatrix covariance(gccc.getConditionalCovariance(inputTrain));
      SquareMatrix nullMatrix(sampleSize);
      assert_almost_equal(covariance, nullMatrix, 0.0, 1e-13);

      // Covariance per marginal & extract variance component
      Collection<CovarianceMatrix> coll(gccc.getConditionalMarginalCovariance(inputTrain));
      Point variance(coll.getSize());
      const Point nullVariance(coll.getSize());
      for (UnsignedInteger index = 0; index < coll.getSize(); ++ index)
        variance[index] = coll[index](0, 0);

      assert_almost_equal(variance, nullVariance, 1e-14, 1e-13);

      // Variance per marginal
      assert_almost_equal(gccc.getConditionalMarginalVariance(inputTrain), Sample(sampleSize, 1), 1e-14, 1e-13);


      // Kriging variance is non-null on validation points
      CovarianceMatrix validCovariance(gccc.getConditionalCovariance(inputTest));

      const Point rowData = {0.786400318519185, -0.342314710430317, -0.167625132016427, 0.0437937446519361, -0.0291542115306344, 0.0362074153614559, \
                             -0.342314710430317, 0.20307609313608, 0.106429376006901, -0.0313570361766278, 0.0230293899173111, -0.0308930847149105, \
                             -0.167625132016427, 0.106429376006901, 0.0567326538237296, -0.0172648099111221, 0.0130555631357385, -0.0179618049872801, \
                             0.0437937446519361, -0.0313570361766278, -0.0172648099111221, 0.00560441404059731, -0.00450884121944028, 0.00656752917461922, \
                             -0.0291542115306344, 0.0230293899173111, 0.0130555631357385, -0.00450884121944028, 0.00386908619998749, -0.00601186391616793, \
                             0.0362074153614559, -0.0308930847149105, -0.0179618049872801, 0.00656752917461922, -0.00601186391616793, 0.0100243621895402
                            };

      const Matrix values(sampleSize, sampleSize, rowData);
      assert_almost_equal(validCovariance - values.transpose(), nullMatrix, 1.e-8, 1e-8);
    }

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
