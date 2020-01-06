//                                               -*- C++ -*-
/**
 *  @brief The test file of class SquaredExponential
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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


static void test_model(const CovarianceModel & myModel)
{
  OStream fullprint(std::cout);

  fullprint << "myModel = " << myModel << std::endl;

  const UnsignedInteger inputDimension = myModel.getInputDimension();
  const UnsignedInteger dimension = myModel.getOutputDimension();

  Point x1(inputDimension);
  Point x2(inputDimension);
  for (UnsignedInteger j = 0; j < inputDimension; ++ j)
  {
    x1[j] = 8.0 * (0.5 - j);
    x2[j] = -1.0 * (3.0 - 2.0 * j);
  }
  fullprint << "myModel(" << x1 << ", " << x2 << ")=" << myModel(x1, x2) << std::endl;


  Matrix grad(myModel.partialGradient(x1, x2));
  fullprint << "dCov =" << grad << std::endl;

  Scalar eps = 1e-3;
  if (dimension == 1)
  {
    Point gradfd(inputDimension);
    for (UnsignedInteger j = 0; j < inputDimension; ++ j)
    {
      Point x1_g(x1);
      Point x1_d(x1);
      x1_g[j] += eps;
      x1_d[j] -= eps;
      gradfd[j] = (myModel(x1_g, x2)(0, 0) - myModel(x1_d, x2)(0, 0)) / (2.0 * eps);
    }
    fullprint << "dCov (FD)=" << gradfd << std::endl;
  }
  else
  {
    Matrix gradfd(inputDimension, dimension * dimension);
    CovarianceMatrix covarianceX1X2 = myModel(x1, x2);
    // Convert result into MatrixImplementation to symmetrize & get the collection
    MatrixImplementation covarianceX1X2Implementation(*covarianceX1X2.getImplementation());
    covarianceX1X2Implementation.symmetrize();
    const Point centralValue(covarianceX1X2Implementation);
    // Loop over the shifted points
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
    {
      Point currentPoint(x1);
      currentPoint[i] += eps;
      CovarianceMatrix localCovariance = myModel(currentPoint, x2);
      MatrixImplementation localCovarianceImplementation(*localCovariance.getImplementation());
      localCovarianceImplementation.symmetrize();
      const Point currentValue(localCovarianceImplementation);
      for (UnsignedInteger j = 0; j < centralValue.getDimension(); ++j)
        gradfd(i, j) = (currentValue[j] - centralValue[j]) / eps;
    }
    fullprint << "dCov (FD)=" << gradfd << std::endl;
  }
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


    {
      /* Default constructor */
      SquaredExponential myDefautModel;
      fullprint << "myDefautModel = " << myDefautModel << std::endl;


      SquaredExponential myModel(dimension);
      test_model(myModel);

    }
    {
      /* Default constructor */
      GeneralizedExponential myDefautModel;
      fullprint << "myDefautModel = " << myDefautModel << std::endl;

      GeneralizedExponential myModel(Point(dimension, 10.0), 1.5);
      test_model(myModel);
    }
    {
      /* Default constructor */
      AbsoluteExponential myDefautModel;
      fullprint << "myDefautModel = " << myDefautModel << std::endl;

      AbsoluteExponential myModel(Point(dimension, 10.0));
      test_model(myModel);
    }
    {
      /* Default constructor */
      MaternModel myDefautModel;
      fullprint << "myDefautModel = " << myDefautModel << std::endl;

      MaternModel myModel(Point(dimension, 8.0), 2.0);
      test_model(myModel);
    }
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
      fullprint << "myModel = " << myModel << std::endl;
      // Check that myModel(x) = myAbsoluteExponential * mySquaredExponential
      Point point(2);
      point[0] = 0.50;
      point[1] = -6.0;
      const Point x(1, point[0]);
      const Point y(1, point[1]);
      fullprint << "Validation of myModel(x_1, x_2) - myAbsoluteExponential(x_1) * mySquaredExponential(x_2) = " << myModel(point) - myAbsoluteExponential(x) * mySquaredExponential(y)  << std::endl;
      // Gradient test in comparison with FD
      test_model(myModel);

    }

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
      // Add exponentialModel to the collection
      Point amplitude(2);
      amplitude[0] = 4.0;
      amplitude[1] = 2.0;
      // Define scale
      Point scale(2, 1.0);
      // Define a spatial correlation
      CorrelationMatrix spatialCorrelation(inputDimension);
      spatialCorrelation(1, 0) = 0.3;
      ExponentialModel myExponentialModel(scale, amplitude, spatialCorrelation);
      collection.add(myExponentialModel);
      // Build TensorizedCovarianceModel with scale = [1,..,1]
      TensorizedCovarianceModel myModel(collection);
      test_model(myModel);
      // Define new scale
      scale[0] = 2.5;
      scale[1] = 1.5;
      myModel.setScale(scale);
      test_model(myModel);
    }

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
