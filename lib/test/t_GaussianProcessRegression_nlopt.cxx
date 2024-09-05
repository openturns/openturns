//                                               -*- C++ -*-
/**
 *  @brief The test file of GaussianProcessRegression class using NLOpt
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
  setRandomGenerator();
  ResourceMap::SetAsUnsignedInteger("OptimizationAlgorithm-DefaultMaximumCallsNumber", 10000);
  ResourceMap::SetAsScalar("Cobyla-DefaultRhoBeg", 0.5);

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
      for (UnsignedInteger i = 0; i < sampleSize; ++i)
      {
        inputTrain(i, 0) = 3.0 + i;
      }
      inputTrain(0, 0) = 1.0;
      inputTrain(1, 0) = 3.0;
      // input test sample
      const Sample outputTrain(f(inputTrain));

      // create covariance model
      const Basis basis(ConstantBasisFactory(dimension).build());
      const SquaredExponential covarianceModel;

      // create algorithm
      GaussianProcessFitter fit_algo(inputTrain, outputTrain, covarianceModel, basis);

      // set sensible optimization bounds and estimate hyper parameters
      fit_algo.setOptimizationBounds(Interval(inputTrain.getMin(), inputTrain.getMax()));
      NLopt solver("LN_NELDERMEAD");
      fit_algo.setOptimizationAlgorithm(solver);
      fit_algo.run();

      // perform an evaluation
      GaussianProcessRegression algo(fit_algo.getResult());
      algo.run();
      GaussianProcessRegressionResult result(algo.getResult());
      assert_almost_equal(result.getMetaModel()(inputTrain), outputTrain);
      assert_almost_equal(result.getResiduals(), {1.32804e-07}, 1e-3, 1e-3);
      assert_almost_equal(result.getRelativeErrors(), {5.20873e-21});

      // Prediction accuracy
      const Point point = {7.5};
      assert_almost_equal(result.getMetaModel()(point), f(point), 0.3, 0.0);

    }
    // Test 2 : 2 inputs, one output    
    {
      const UnsignedInteger sampleSize = 10;
      const UnsignedInteger dimension = 2;
      const Description inputDesc ={"x", "y"};
      const Description outputDesc ={"cos(0.5*x) + sin(y)"};
      const SymbolicFunction model(inputDesc, outputDesc);

      const Indices levels = {8, 5};
      const Box box(levels);
      Sample inputSample(box.generate());

      // Scale each direction
      inputSample *= 10.0;
      const Sample outputSample(model(inputSample));

      // Validation
      Collection<Distribution> coll;
      coll.add(Uniform(0, 10.0));
      coll.add(Uniform(0, 10.0));
      const JointDistribution dist(coll);
      const Sample inputValidSample(dist.getSample(sampleSize));
      const Sample outputValidSample(model(inputValidSample));

      //Definition of exponential model
      // The parameters have been calibrated using TNC optimization 
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
      NLopt solver("LN_NELDERMEAD");
      fit_algo.setOptimizationAlgorithm(solver);
      fit_algo.run();

      // perform an evaluation
      GaussianProcessRegression algo(fit_algo.getResult());
      algo.run();

      GaussianProcessRegressionResult result(algo.getResult());
      // Get meta model
      Function metaModel(result.getMetaModel());
      const Sample outData(metaModel(inputValidSample));
      // 5) Errors
      // Interpolation
      assert_almost_equal(outputSample, metaModel(inputSample), 3.0e-5, 3.0e-5);
      // Prediction
      assert_almost_equal(outputValidSample, outData, 1.0e-1, 1e-1);
    }
    // Test 3 : R --> R^2 function
    {
      const Description inputDesc ={"x"};
      const Description outputDesc ={"x * sin(x)", "x * cos(x)"};
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
      NLopt solver("LN_NELDERMEAD");
      fit_algo.setOptimizationAlgorithm(solver);
      fit_algo.run();

      // perform an evaluation
      GaussianProcessRegression algo(fit_algo.getResult());
      algo.run();

      GaussianProcessRegressionResult result(algo.getResult());
      // Get meta model
      Function metaModel(result.getMetaModel());
      // Prediction & interpolation
      //assert_almost_equal(sampleY, metaModel(sampleX));
      const Point point = {5.5};
      const Point outputValue = {-3.88363, 3.90286};
      assert_almost_equal(metaModel(point), outputValue);
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
      NLopt solver("LN_NELDERMEAD");
      fit_algo.setOptimizationAlgorithm(solver);
      fit_algo.run();

      // perform an evaluation
      GaussianProcessRegression algo = (fit_algo.getResult());
      algo.run();

      GaussianProcessRegressionResult result(algo.getResult());
      // Get meta model
      Function metaModel(result.getMetaModel());
      const Point point = {5.5};
      const Point outputValue = {5.58838};
      assert_almost_equal(metaModel(point), outputValue);
    }

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
