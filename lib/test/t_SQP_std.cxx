//                                               -*- C++ -*-
/**
 *  @brief The test file of class SQP for standard methods
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

    // Test function operator ()
    const Description input = {"x1", "x2", "x3", "x4"};
    SymbolicFunction levelFunction(input, Description(1, "x1+2*x2-3*x3+4*x4"));
    // Add a finite difference gradient to the function,
    NonCenteredFiniteDifferenceGradient myGradient(1e-7, levelFunction.getEvaluation());
    /** Substitute the gradient */
    levelFunction.setGradient(new NonCenteredFiniteDifferenceGradient(myGradient));
    Point startingPoint(4, 0.0);
    SQP mySQPAlgorithm(NearestPointProblem(levelFunction, 3.0));
    mySQPAlgorithm.setStartingPoint(startingPoint);
    fullprint << "mySQPAlgorithm=" << mySQPAlgorithm << std::endl;
    mySQPAlgorithm.run();
    fullprint << "result=" << mySQPAlgorithm.getResult().getOptimalPoint() << std::endl;
    fullprint << "multipliers=" << mySQPAlgorithm.getResult().computeLagrangeMultipliers() << std::endl;
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
    // Add a finite difference gradient to the function, as SQP algorithm
    // needs it
    CenteredFiniteDifferenceGradient myGradient(1e-7, levelFunction.getEvaluation());
    /** Substitute the gradient */
    levelFunction.setGradient(new CenteredFiniteDifferenceGradient(myGradient));

    // Add a finite difference hessian to the function, as SQP algorithm
    // needs it
    CenteredFiniteDifferenceHessian myHessian(1e-3, levelFunction.getEvaluation());
    /** Substitute the hessian */
    levelFunction.setHessian(new CenteredFiniteDifferenceHessian(myHessian));
    Point startingPoint(4, 0.0);
    SQP mySQPAlgorithm(NearestPointProblem(levelFunction, 3.0));
    mySQPAlgorithm.setStartingPoint(startingPoint);
    fullprint << "mySQPAlgorithm=" << mySQPAlgorithm << std::endl;
    mySQPAlgorithm.run();
    OptimizationResult result(mySQPAlgorithm.getResult());
    fullprint << "result = " << result.getOptimalPoint() << std::endl;
    fullprint << "multipliers = " << result.computeLagrangeMultipliers() << std::endl;
    result.drawErrorHistory();
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
