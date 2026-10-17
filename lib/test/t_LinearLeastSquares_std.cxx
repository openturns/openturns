//                                               -*- C++ -*-
/**
 *  @brief The test file of class LinearLeastSquares for standard methods
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

  try
  {
    Description input(2);
    input[0] = "x1";
    input[1] = "x2";
    Description formula(3);
    formula[0] = "x1*sin(x2)";
    formula[1] = "cos(x1+x2)";
    formula[2] = "(x2+1)*exp(x1-2*x2)";
    SymbolicFunction myFunc(input, formula);
    Sample data(9, myFunc.getInputDimension());
    Point point(myFunc.getInputDimension());
    point[0] = 0.5;
    point[1] = 0.5;
    data[0] = point;
    point[0] = -0.5;
    point[1] = -0.5;
    data[1] = point;
    point[0] = -0.5;
    point[1] = 0.5;
    data[2] = point;
    point[0] = 0.5;
    point[1] = -0.5;
    data[3] = point;
    point[0] = 0.5;
    point[1] = 0.5;
    data[4] = point;
    point[0] = -0.25;
    point[1] = -0.25;
    data[5] = point;
    point[0] = -0.25;
    point[1] = 0.25;
    data[6] = point;
    point[0] = 0.25;
    point[1] = -0.25;
    data[7] = point;
    point[0] = 0.25;
    point[1] = 0.25;
    data[8] = point;
    {
      Sample dataOut(myFunc(data));
      LinearLeastSquares myLeastSquares(data, dataOut);
      myLeastSquares.run();
      // The linear term follows the LinearFunction convention:
      // outputDimension rows, inputDimension columns
      const Point center(myLeastSquares.getCenter());
      const Point constant(myLeastSquares.getConstant());
      const Matrix linear(myLeastSquares.getLinear());
      assert(center.getDimension() == 2);
      assert(constant.getDimension() == 3);
      assert(linear.getNbRows() == 3);
      assert(linear.getNbColumns() == 2);
      assert_almost_equal(center, Point({0.0555556, 0.0555556}));
      assert_almost_equal(constant, Point({0.0266348, 0.819564, 1.03714}));
      Matrix expectedLinear(3, 2);
      expectedLinear(0, 0) = 0.0628755;
      expectedLinear(1, 0) = -0.082405;
      expectedLinear(2, 0) = 0.951099;
      expectedLinear(0, 1) = 0.0628755;
      expectedLinear(1, 1) = -0.082405;
      expectedLinear(2, 1) = -0.972737;
      assert_almost_equal(linear, expectedLinear);
      // The metamodel matches the LinearFunction definition:
      // the evaluation stores the linear term as is, the gradient stores its transpose
      Function responseSurface(myLeastSquares.getResult().getMetaModel());
      const EvaluationImplementation * p_implementation = responseSurface.getEvaluation().getImplementation().get();
      const LinearEvaluation * p_evaluation = dynamic_cast<const LinearEvaluation *>(p_implementation);
      assert(p_evaluation);
      assert_almost_equal(p_evaluation->getLinear(), linear);
      assert_almost_equal(responseSurface.gradient(center), linear.transpose());
      const Point in(myFunc.getInputDimension(), 0.1);
      assert_almost_equal(myFunc(in), Point({0.00998334, 0.980067, 0.995321}));
      assert_almost_equal(responseSurface(in), Point({0.0322237, 0.812239, 1.03617}));
      assert_almost_equal(myLeastSquares.getInputSample(), data);
      assert_almost_equal(myLeastSquares.getDataIn(), data);
      assert_almost_equal(myLeastSquares.getOutputSample(), dataOut);
      assert_almost_equal(myLeastSquares.getDataOut(), dataOut);
      fullprint << "myLeastSquares linear=" << linear << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
