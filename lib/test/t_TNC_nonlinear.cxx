//                                               -*- C++ -*-
/**
 *  @brief The test file of class TNC for solving a linear problem
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
 */
#include "OT.hxx"
#include "OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

String printNumericalPoint(const NumericalPoint & point, const UnsignedInteger digits)
{
  OSS oss;
  oss << "[";
  NumericalScalar eps(pow(0.1, 1.0 * digits));
  for (UnsignedInteger i = 0; i < point.getDimension(); i++)
  {
    oss << std::fixed << std::setprecision(digits) << (i == 0 ? "" : ",") << Bulk<double>((fabs(point[i]) < eps) ? fabs(point[i]) : point[i]);
  }
  oss << "]";
  return oss;
}

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

      // Test function operator ()
      Description input(4);
      input[0] = "x1";
      input[1] = "x2";
      input[2] = "x3";
      input[3] = "x4";
      NumericalMathFunction levelFunction(input, Description(1, "y1"), Description(1, "x1*cos(x1)+2*x2*x3-3*x3+4*x3*x4"));
      TNCSpecificParameters specific;
      NumericalPoint startingPointNearMinimizationCorner(4);
      startingPointNearMinimizationCorner[0] = 3.0;
      startingPointNearMinimizationCorner[0] = -2.5;
      startingPointNearMinimizationCorner[0] = 4.5;
      startingPointNearMinimizationCorner[0] = -2.5;
      NumericalPoint startingPointNearMaximizationCorner(4);
      startingPointNearMaximizationCorner[0] = -2.5;
      startingPointNearMaximizationCorner[1] = 4.5;
      startingPointNearMaximizationCorner[2] = 4.5;
      startingPointNearMaximizationCorner[3] = 4.5;

      Interval bounds(NumericalPoint(4, -3.0), NumericalPoint(4, 5.0));
      {
        TNC myAlgorithm(specific, levelFunction, bounds, startingPointNearMinimizationCorner, TNC::Result::MINIMIZATION);
        myAlgorithm.run();
        fullprint << "minimizer = " << printNumericalPoint(myAlgorithm.getResult().getOptimizer(), 4) << " value=" << myAlgorithm.getResult().getOptimalValue() << std::endl;
      }
      {
        TNC myAlgorithm(specific, levelFunction, bounds, startingPointNearMaximizationCorner, TNC::Result::MAXIMIZATION);
        myAlgorithm.run();
        fullprint << "maximizer = " << printNumericalPoint(myAlgorithm.getResult().getOptimizer(), 4) << " value=" << myAlgorithm.getResult().getOptimalValue() << std::endl;
      }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
