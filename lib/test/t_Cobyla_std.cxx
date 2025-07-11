//                                               -*- C++ -*-
/**
 *  @brief The test file of class Cobyla for standard methods
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
  try
  {
    Log::Show(Log::NONE);
    PlatformInfo::SetNumericalPrecision(4);
    // Test function operator ()
    const Description input = {"x1", "x2", "x3", "x4"};
    SymbolicFunction levelFunction(input, Description(1, "x1+2*x2-3*x3+4*x4"));
    Point startingPoint(4, 0.0);
    Cobyla myAlgorithm(NearestPointProblem(levelFunction, 3.0));
    myAlgorithm.setStartingPoint(startingPoint);
    fullprint << "myAlgorithm = " << myAlgorithm << std::endl;
    myAlgorithm.run();
    fullprint << "result = " << myAlgorithm.getResult().getOptimalPoint() << std::endl;
    fullprint << "multipliers = " << myAlgorithm.getResult().computeLagrangeMultipliers() << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  try
  {
    const Description input = {"x1", "x2", "x3", "x4"};
    SymbolicFunction levelFunction(input, Description(1, "x1*cos(x1)+2*x2*x3-3*x3+4*x3*x4"));
    Point startingPoint(4, 0.0);
    Cobyla myAlgorithm(NearestPointProblem(levelFunction, 3.0));
    myAlgorithm.setStartingPoint(startingPoint);
    myAlgorithm.setMaximumCallsNumber(400);
    myAlgorithm.setMaximumAbsoluteError(1.0e-10);
    myAlgorithm.setMaximumRelativeError(1.0e-10);
    myAlgorithm.setMaximumResidualError(1.0e-10);
    myAlgorithm.setMaximumConstraintError(1.0e-10);
    fullprint << "myAlgorithm = " << myAlgorithm << std::endl;
    myAlgorithm.run();
    OptimizationResult result(myAlgorithm.getResult());
    fullprint << "result = " << result.getOptimalPoint() << std::endl;
    fullprint << "multipliers = " << result.computeLagrangeMultipliers() << std::endl;
    result.drawErrorHistory();
    // fullprint << "evaluation calls number=" << levelFunction.getEvaluationCallsNumber() << std::endl;
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
