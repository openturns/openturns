//                                               -*- C++ -*-
/**
 *  @brief The test file of class AbdoRackwitz for standard methods
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

inline String printPoint(const Point & point, const UnsignedInteger digits)
{
  OSS oss;
  oss << "[";
  Scalar eps = pow(0.1, 1.0 * digits);
  for (UnsignedInteger i = 0; i < point.getDimension(); i++)
  {
    oss << std::fixed << std::setprecision(digits) << (i == 0 ? "" : ",") << Bulk<double>(((std::abs(point[i]) < eps) ? std::abs(point[i]) : point[i]));
  }
  oss << "]";
  return oss;
}

int main(int, char *[])
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
    SymbolicFunction levelFunction(input, Description(1, "x1+2*x2-3*x3+4*x4"));
    // Add a finite difference gradient to the function, as Abdo Rackwitz algorithm
    // needs it
    NonCenteredFiniteDifferenceGradient myGradient(1e-7, levelFunction.getEvaluation());
    /** Substitute the gradient */
    levelFunction.setGradient(new NonCenteredFiniteDifferenceGradient(myGradient));
    Point startingPoint(4, 0.0);
    AbdoRackwitz myAlgorithm(NearestPointProblem(levelFunction, 3.0));
    myAlgorithm.setStartingPoint(startingPoint);
    fullprint << "myAlgorithm = " << myAlgorithm << std::endl;
    myAlgorithm.run();
    fullprint << "result = " << printPoint(myAlgorithm.getResult().getOptimalPoint(), 4) << std::endl;
    fullprint << "multipliers = " << printPoint(myAlgorithm.getResult().getLagrangeMultipliers(), 4) << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  try
  {
    Description input(4);
    input[0] = "x1";
    input[1] = "x2";
    input[2] = "x3";
    input[3] = "x4";
    MemoizeFunction levelFunction(SymbolicFunction(input, Description(1, "x1*cos(x1)+2*x2*x3-3*x3+4*x3*x4")));
    // Activate the cache as we will use an analytical method
    levelFunction.enableCache();
    // Add a finite difference gradient to the function, as Abdo Rackwitz algorithm
    // needs it
    NonCenteredFiniteDifferenceGradient myGradient(1e-7, levelFunction.getEvaluation());
    fullprint << "myGradient=" << myGradient << std::endl;
    /** Substitute the gradient */
    levelFunction.setGradient(new NonCenteredFiniteDifferenceGradient(myGradient));
    Point startingPoint(4, 0.0);
    AbdoRackwitz myAlgorithm(NearestPointProblem(levelFunction, -0.5));
    myAlgorithm.setStartingPoint(startingPoint);
    fullprint << "myAlgorithm = " << myAlgorithm << std::endl;
    myAlgorithm.run();
    OptimizationResult result(myAlgorithm.getResult());
    fullprint << "result = " << printPoint(result.getOptimalPoint(), 4) << std::endl;
    Graph convergence(result.drawErrorHistory());
    fullprint << "evaluation cache hits=" <<   levelFunction.getCacheHits() << std::endl;
    fullprint << "evaluation calls number=" << levelFunction.getEvaluationCallsNumber() << std::endl;
    fullprint << "gradient   calls number=" << levelFunction.getGradientCallsNumber() << std::endl;
    fullprint << "hessian    calls number=" << levelFunction.getHessianCallsNumber() << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
