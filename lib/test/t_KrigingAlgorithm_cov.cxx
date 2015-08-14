//                                               -*- C++ -*-
/**
 *  @brief The test file of class SquaredExponential
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Id:      $Id$
 */
#include <iostream>
#include "OT.hxx"
#include "OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;


static void test_model(const CovarianceModel & myModel)
{
  OStream fullprint(std::cout);

  fullprint << "myModel = " << myModel << std::endl;

  const UnsignedInteger dimension = myModel.getSpatialDimension();

  NumericalPoint x1(dimension);
  NumericalPoint x2(dimension);
  for (UnsignedInteger j = 0; j < dimension; ++ j)
  {
    x1[j] = 8.0 * (0.5 - j);
    x2[j] = -1.0 * (3.0 - 2.0 * j);
  }
  fullprint << "myModel(" << x1 << ", " << x2 << ")=" << myModel(x1, x2) << std::endl;


  Matrix grad(myModel.partialGradient(x1, x2));
  fullprint << "dCov =" << grad << std::endl;

  NumericalScalar eps = 1e-3;
  NumericalPoint gradfd(dimension);
  for (UnsignedInteger j = 0; j < dimension; ++ j)
  {
    NumericalPoint x1_g(x1);
    NumericalPoint x1_d(x1);
    x1_g[j] += eps;
    x1_d[j] -= eps;
    gradfd[j] = (myModel(x1_g, x2)(0, 0) - myModel(x1_d, x2)(0, 0)) / (2.0 * eps);
  }
  fullprint << "dCov (FD)=" << gradfd << std::endl;
}


int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    UnsignedInteger precision(PlatformInfo::GetNumericalPrecision());
    PlatformInfo::SetNumericalPrecision(3);
    // Default spatial dimension parameter to evaluate the model
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

      GeneralizedExponential myModel(dimension, 10.0, 1.5);
      test_model(myModel);
    }
    {
      /* Default constructor */
      AbsoluteExponential myDefautModel;
      fullprint << "myDefautModel = " << myDefautModel << std::endl;

      AbsoluteExponential myModel(dimension, 10.0);
      test_model(myModel);
    }
    {
      /* Default constructor */
      MaternModel myDefautModel;
      fullprint << "myDefautModel = " << myDefautModel << std::endl;

      MaternModel myModel(dimension, 8.0, 2.0);
      test_model(myModel);
    }
    {
      // Generate collection
      Collection<CovarianceModel> collection;
      // Collection ==> add covariance models
      AbsoluteExponential myAbsoluteExponential(1, 3.0);
      collection.add(myAbsoluteExponential);
      SquaredExponential mySquaredExponential(1, 2.0);
      collection.add(mySquaredExponential);
      // Build ProductCovarianceModel
      ProductCovarianceModel myModel(collection);
      fullprint << "myModel = " << myModel << std::endl;
      // Check that myModel(x) = myAbsoluteExponential * mySquaredExponential
      NumericalPoint point(2);
      point[0] = 0.50;
      point[1] = -6.0;
      const NumericalPoint x(1, point[0]);
      const NumericalPoint y(1, point[1]);
      fullprint << "Validation of myModel(x_1, x_2) - myAbsoluteExponential(x_1) * mySquaredExponential(x_2) = " << myModel(point) - myAbsoluteExponential(x) * mySquaredExponential(y)  << std::endl;
      // Gradient test in comparison with FD
      test_model(myModel);

    }

    PlatformInfo::SetNumericalPrecision(precision);

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
