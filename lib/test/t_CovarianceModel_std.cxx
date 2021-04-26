//                                               -*- C++ -*-
/**
 *  @brief The test file of class CovarianceModel(s)
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <iostream>
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;


static void test_model(const CovarianceModel & myModel, const Bool test_grad = true)
{
  OStream fullprint(std::cout);

  const UnsignedInteger inputDimension = myModel.getInputDimension();
  const UnsignedInteger dimension = myModel.getOutputDimension();

  Point x1(inputDimension);
  Point x2(inputDimension);
  for (UnsignedInteger j = 0; j < inputDimension; ++j)
  {
    x1[j] = 8.0 * (0.5 - j);
    x2[j] = -1.0 * (3.0 - 2.0 * j);
  }

  if (myModel.isStationary())
  {
    assert_almost_equal(myModel(x1 - x2), myModel(x1, x2), 0, 0);
    assert_almost_equal(myModel(x2 - x1), myModel(x1, x2), 0, 0);
  }

  // interval mesher
  Indices levels(inputDimension);
  for (UnsignedInteger k = 0; k < inputDimension; ++k)
    levels[k] = 9;
  IntervalMesher intervalMesher(levels);

  // Building interval
  const Point lowerBounds(inputDimension, -10);
  const Point upperBounds(inputDimension, 10);
  Interval interval(lowerBounds, upperBounds);
  Mesh mesh(intervalMesher.build(interval));
  const Sample vertices(mesh.getVertices());

  // Discretize the covariance model over the mesh
  CovarianceMatrix cov(myModel.discretize(mesh));

  if (dimension == 1)
  {
    // Check that discretize & computeAsScalar provide the same values
    for (UnsignedInteger j = 0; j < vertices.getSize(); ++j)
      for (UnsignedInteger i = j; i < vertices.getSize(); ++i)
        assert_almost_equal(cov(i, j), myModel.computeAsScalar(vertices[i], vertices[j]), 1e-14, 1e-14);
  }
  else
  {
    // Check that discretize &operator() provide the same values
    SquareMatrix localMatrix(dimension);
    for (UnsignedInteger j = 0; j < vertices.getSize(); ++j)
    {
      for (UnsignedInteger i = j; i < vertices.getSize(); ++i)
      {
        for (UnsignedInteger localJ = 0; localJ < dimension; ++localJ)
        {
          for (UnsignedInteger localI = 0; localI < dimension; ++localI)
          {
            localMatrix(localI, localJ) = cov(i * dimension + localI, j * dimension + localJ);
          }
        }
        assert_almost_equal(localMatrix, myModel(vertices[i], vertices[j]), 1e-14, 1e-14);
      }
    }
  }

  // gradient testing
  if (test_grad)
  {
    // Testing partial gradient
    Matrix grad(myModel.partialGradient(x1, x2));

    Scalar eps = 1.0e-3;
    if (dimension == 1)
    {
      Matrix gradfd(inputDimension, 1);
      for (UnsignedInteger j = 0; j < inputDimension; ++j)
      {
        Point x1_g(x1);
        Point x1_d(x1);
        x1_g[j] += eps;
        x1_d[j] -= eps;
        gradfd(j, 0) = (myModel(x1_g, x2)(0, 0) - myModel(x1_d, x2)(0, 0)) / (2.0 * eps);
      }
      assert_almost_equal(grad, gradfd, 1e-6, 1e-6, OSS() << "in " << myModel.getImplementation()->getClassName() << " grad");
    }
    else
    {
      Matrix gradfd(inputDimension, dimension * dimension);
      SquareMatrix covarianceX1X2 = myModel(x1, x2);
      // Convert result into MatrixImplementation to get the collection
      MatrixImplementation covarianceX1X2Implementation(*covarianceX1X2.getImplementation());
      const Point centralValue(covarianceX1X2Implementation);
      // Loop over the shifted points
      for (UnsignedInteger i = 0; i < inputDimension; ++i)
      {
        Point currentPoint(x1);
        currentPoint[i] += eps;
        SquareMatrix localCovariance = myModel(currentPoint, x2);
        MatrixImplementation localCovarianceImplementation(*localCovariance.getImplementation());
        const Point currentValue(localCovarianceImplementation);
        for (UnsignedInteger j = 0; j < centralValue.getDimension(); ++j)
          gradfd(i, j) = (currentValue[j] - centralValue[j]) / eps;
      }
      assert_almost_equal(grad, gradfd, 2e5, 2e-5, OSS() << "in " << myModel.__str__() << " grad");
    }
  }
}

static void test_scalar_model(const CovarianceModel &myModel)
{
  OStream fullprint(std::cout);


  const UnsignedInteger inputDimension = 1;

  const Point x1(inputDimension,  2.0);
  const Point x2(inputDimension, -3.0);
  assert_almost_equal(myModel.computeAsScalar(x1[0], x2[0]), myModel.computeAsScalar(x1, x2), 1.0e-14, 1.0e-14);

  const  Matrix grad(myModel.partialGradient(x1, x2));

  Scalar eps = 1e-3;
  Point gradfd(inputDimension);
  for (UnsignedInteger j = 0; j < inputDimension; ++j)
  {
    Point x1_g(x1);
    Point x1_d(x1);
    x1_g[j] += eps;
    x1_d[j] -= eps;
    gradfd[j] = (myModel(x1_g, x2)(0, 0) - myModel(x1_d, x2)(0, 0)) / (2.0 * eps);
  }
  assert_almost_equal(gradfd[0], grad(0, 0), 1.0e-5, 1.0e-5);
}

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    PlatformInfo::SetNumericalPrecision(3);
    // Default input dimension parameter to evaluate the model
    const UnsignedInteger dimension = 2;

    // 1) Squared exponential model
    {
      /* Default constructor */
      SquaredExponential myDefautModel;
      test_model(myDefautModel);

      SquaredExponential myModel(dimension);
      test_model(myModel);
    }

    // 2) Generalized exponential model
    {
      /* Default constructor */
      GeneralizedExponential myDefautModel;
      test_model(myDefautModel);

      GeneralizedExponential myModel(Point(dimension, 10.0), 1.5);
      test_model(myModel);
    }

    // 3) Absolute exponential model
    {
      /* Default constructor */
      AbsoluteExponential myDefautModel;
      test_model(myDefautModel);

      AbsoluteExponential myModel(Point(dimension, 10.0));
      test_model(myModel);
    }

    // 4) Matern  model
    {
      /* Default constructor */
      MaternModel myDefautModel;
      test_model(myDefautModel);

      MaternModel myModel(Point(dimension, 8.0), 2.0);
      test_model(myModel);
    }

    // 5) ExponentiallyDampedCosineModel
    {

      ExponentiallyDampedCosineModel myDefautModel;
      test_model(myDefautModel);

      const Point scale = {2, 2};
      const Point amplitude = {3};

      ExponentiallyDampedCosineModel myModel(scale, amplitude, 1);
      assert_almost_equal(myModel.getScale(), scale, 0, 0);
      assert_almost_equal(myModel.getAmplitude(), amplitude, 0, 0);
      assert_almost_equal(myModel.getFrequency(), 1, 0, 0);
      test_model(myModel);
      // set new freq
      myModel.setFrequency(3);
      assert_almost_equal(myModel.getFrequency(), 3, 0, 0);
    }

    // 6) Spherical model
    {

      SphericalModel myDefautModel;
      test_model(myDefautModel);

      const UnsignedInteger dimension = 2;
      SphericalModel myModel(Point(dimension, 2), Point(1, 3), 4.5);
      assert_almost_equal(myModel.getScale(), Point(dimension, 2), 0, 0);
      assert_almost_equal(myModel.getAmplitude(), Point(1, 3), 0, 0);
      assert_almost_equal(myModel.getRadius(), 4.5, 0, 0);
      test_model(myModel);
      myModel.setRadius(1.5);
      assert_almost_equal(myModel.getRadius(), 1.5, 0, 0);
    }

    // 7) FractionalBrownianMotionModel
    {
      FractionalBrownianMotionModel myModel(2.0, 3.0, 0.25);
      test_model(myModel);
    }

    // 8) DiracCovarianceModel
    {
      DiracCovarianceModel myDefautModel;
      test_model(myDefautModel, false);

      // Should not check the partialGradient Dirac model
      test_model(myDefautModel, false);

      const Point amplitude = {1.5, 3.5};

      const UnsignedInteger dimension = 2;
      CorrelationMatrix spatialCorrelation(dimension);
      for (UnsignedInteger j = 0; j < dimension; ++j)
        for (UnsignedInteger i = j + 1; j < dimension; ++j)
          spatialCorrelation(i, j) = (i + 1.0) / dimension - (j + 1.0) / dimension;
      DiracCovarianceModel myModel(2, amplitude, spatialCorrelation);
      assert_almost_equal(myModel.getScale(), Point(2, 1), 0, 0);
      assert_almost_equal(myModel.getAmplitude(), amplitude, 0, 0);
      test_model(myModel, false);
    }

    // 9 StationaryFunctionalCovarianceModel
    {
      const Description inputs  = {"tau"};
      const Description formula = {"exp(-abs(tau))*cos(2*pi_*abs(tau))"};

      SymbolicFunction rho(inputs, formula);

      const Point scale = {1};
      const Point amplitude = {1};

      StationaryFunctionalCovarianceModel myModel(scale, amplitude, rho);
      assert_almost_equal(myModel.getScale(), scale, 0, 0);
      assert_almost_equal(myModel.getAmplitude(), amplitude, 0, 0);
      test_model(myModel);
    }

    // 10) Product covariance model
    {
      // Generate collection
      Collection<CovarianceModel> collection;
      // Collection ==> add covariance models
      AbsoluteExponential myAbsoluteExponential(Point(1, 3.0));
      collection.add(myAbsoluteExponential);
      SquaredExponential mySquaredExponential(Point(1, 2.0));
      collection.add(mySquaredExponential);
      // Build ProductCovarianceModel
      ProductCovarianceModel myModel(collection);
      // Check that myModel(x) = myAbsoluteExponential * mySquaredExponential
      Point point(2);
      point[0] = 0.50;
      point[1] = -6.0;
      const Point x(1, point[0]);
      const Point y(1, point[1]);
      assert_almost_equal(myModel.computeAsScalar(point), myAbsoluteExponential.computeAsScalar(x) * mySquaredExponential.computeAsScalar(y), 1.0e-15, 1.0e-15);
      // Gradient test in comparison with FD
      test_model(myModel);
    }

    // 11) Tensorized model
    {
      // Tensorized covariance model
      const UnsignedInteger inputDimension = 2;
      Collection<CovarianceModel> collection;
      // Collection ==> add covariance models
      // Add AbsoluteExponentialModel to the collection
      AbsoluteExponential myAbsoluteExponential(Point(inputDimension, 3.0));
      collection.add(myAbsoluteExponential);
      // Add SquaredExponentialModel to the collection
      SquaredExponential mySquaredExponential(Point(inputDimension, 2.0));
      collection.add(mySquaredExponential);
      GeneralizedExponential myGeneralizedExponential(Point(dimension, 10.0), 1.5);
      collection.add(myGeneralizedExponential);
      // Build TensorizedCovarianceModel with scale = [1,..,1]
      TensorizedCovarianceModel myModel(collection);
      test_model(myModel);
      // Define new scale
      Point scale(2, 1.0);
      scale[0] = 2.5;
      scale[1] = 1.5;
      myModel.setScale(scale);
      test_model(myModel);
    }

    // 12) 1d in/out models

    {
      Collection<CovarianceModel> collection(0);
      collection.add(AbsoluteExponential(1));
      collection.add(SquaredExponential(1));
      collection.add(GeneralizedExponential(1));
      collection.add(MaternModel(1));
      collection.add(SphericalModel(1));
      collection.add(ExponentiallyDampedCosineModel(1));
      for (UnsignedInteger i = 0; i < collection.getSize(); ++i)
      {
        test_scalar_model(collection[i]);
      }
    }

    // 13) isotropic model
    {
      Point scalePoint = {3.5};
      Point amplitudePoint = {1.5};
      SquaredExponential myOneDimensionalKernel(scalePoint, amplitudePoint);
      IsotropicCovarianceModel myIsotropicKernel(myOneDimensionalKernel, 2);

      // Test consistency of isotropic model with underlying 1D kernel
      assert_almost_equal(myIsotropicKernel.getAmplitude(), amplitudePoint, 1e-12, 0.0);
      assert_almost_equal(myIsotropicKernel.getScale(), scalePoint, 1e-12, 0.0);
      assert_almost_equal(myIsotropicKernel.getKernel().getAmplitude(), amplitudePoint, 1e-12, 0.0);
      assert_almost_equal(myIsotropicKernel.getKernel().getScale(), scalePoint, 1e-12, 0.0);

      // Standard tests applied
      test_model(myIsotropicKernel);

      // Test consistency of isotropic kernel's discretization
      Point inputVector(2);
      inputVector[0] = 0.3;
      inputVector[1] = 1.7;
      Point inputVectorNorm(1, inputVector.norm());
      assert_almost_equal(myOneDimensionalKernel(inputVectorNorm)(0, 0), 1.992315565746, 1e-12, 0.0);
      assert_almost_equal(myIsotropicKernel(inputVector)(0, 0), 1.992315565746, 1e-12, 0.0);
      Sample inputSample(2, 2);
      inputSample[1] = inputVector;
      Sample inputSampleNorm(2, 1);
      inputSampleNorm[1] = inputVectorNorm;
      CovarianceMatrix oneDimensionalCovMatrix(myOneDimensionalKernel.discretize(inputSampleNorm));
      CovarianceMatrix isotropicCovMatrix(myIsotropicKernel.discretize(inputSample));
      assert_almost_equal(oneDimensionalCovMatrix(0, 0), 2.250000000002, 1e-12, 0.0);
      assert_almost_equal(oneDimensionalCovMatrix(1, 1), 2.250000000002, 1e-12, 0.0);
      assert_almost_equal(isotropicCovMatrix(0, 0), 2.250000000002, 1e-12, 0.0);
      assert_almost_equal(isotropicCovMatrix(1, 1), 2.250000000002, 1e-12, 0.0);
      assert_almost_equal(oneDimensionalCovMatrix(0, 1), 1.992315565746, 1e-12, 0.0);
      assert_almost_equal(isotropicCovMatrix(0, 1), 1.992315565746, 1e-12, 0.0);
    }

    // Exponential cov model
    {
      // Exponential covariance model
      const Point scale = {4, 5};
      CovarianceMatrix spatialCovariance(2);
      spatialCovariance(0, 0) = 4;
      spatialCovariance(1, 1) = 5;
      spatialCovariance(1, 0) = 1.2;
      ExponentialModel myModel(scale, spatialCovariance);
      test_model(myModel);
      // assert that spatialCovariance is taken into account
      Bool checkDiag = spatialCovariance.isDiagonal() == myModel.isDiagonal();
      if (!checkDiag)
        throw TestFailed(OSS() << "isDiagonal differ between spatial covariance & covariance model");
      const Scalar rho = spatialCovariance(1, 0) / std::sqrt(spatialCovariance(0, 0) * spatialCovariance(1, 1));
      assert_almost_equal(myModel.getOutputCorrelation()(0, 1), rho, 0, 0, "in ExponentialModel correlation");
    }

    // Kronecker cov model
    {
      // rho correlation
      Point scale = {4, 5};
      GeneralizedExponential rho(scale, 1);

      // Amplitude values
      Point amplitude = {1, 2};
      KroneckerCovarianceModel myModel(rho, amplitude);
      test_model(myModel);

      CorrelationMatrix spatialCorrelation(2);
      spatialCorrelation(0, 1) = 0.8;
      myModel = KroneckerCovarianceModel(rho, amplitude, spatialCorrelation);
      test_model(myModel);

      CovarianceMatrix spatialCovariance(2);
      spatialCovariance(0, 0) = 4.0;
      spatialCovariance(1, 1) = 5.0;
      spatialCovariance(1, 0) = 1.2;

      myModel = KroneckerCovarianceModel(rho, spatialCovariance);
      test_model(myModel);
    }

    // Kronecker cov using isotropic model
    {
      IsotropicCovarianceModel rho(MaternModel(1), 3);
      CorrelationMatrix outputCorrelation(2);
      outputCorrelation(0, 1) = 0.8;
      // Amplitude values
      Point amplitude = {1, 2};
      Point scale = {1};
      KroneckerCovarianceModel myModel(rho, amplitude, outputCorrelation);
      test_model(myModel);
      assert_almost_equal(myModel.getInputDimension(), 3, 0, 0, "in kronecker dimension check");
      assert_almost_equal(myModel.getScale(), scale, 0, 0, "in kronecker scale check");
      // full param size = 5 (scale(1), amplitude(2), spatialCorrelation(1), Matern nu(1))
      Point fullParameter = {1, 1, 2, 0.8, 1.5};
      assert_almost_equal(myModel.getFullParameter(), fullParameter, 0, 0, "in kronecker full param check");
      assert_almost_equal(myModel.getFullParameter().getSize(), 5, 0, 0, "in kronecker param size check");
      assert_almost_equal(myModel.getFullParameterDescription().getSize(), 5, 0, 0, "in kronecker param description size check");
      Indices active(3);
      active.fill();
      assert_almost_equal(myModel.getActiveParameter(), active, "in kronecker active param check");
      fullParameter = {2, 1, 2, .5, 2.5};
      myModel.setFullParameter(fullParameter);
      assert_almost_equal(myModel.getFullParameter(), fullParameter, 0, 0, "in kronecker param check");
      active.add(4);
      myModel.setActiveParameter(active);
      assert_almost_equal(myModel.getActiveParameter(), active, "in kronecker active param check");
      // Now we should get all values except correlation
      Point parameter = {2, 1, 2, 2.5};
      assert_almost_equal(myModel.getParameter(), parameter, 0, 0, "in kronecker param check");
      Description description = {"scale_0", "amplitude_0", "amplitude_1", "R_1_0", "nu"};
      Bool checkDesc = myModel.getFullParameterDescription() == description;
      if (!checkDesc)
        throw TestFailed(OSS() << "descriptions differ");
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
