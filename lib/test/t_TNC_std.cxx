//                                               -*- C++ -*-
/**
 *  @brief The test file of class Tnc for standard methods
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
    // Test function operator ()
    const Description input = {"x1", "x2", "x3", "x4"};
    SymbolicFunction levelFunction(input, Description(1, "x1+2*x2-3*x3+4*x4"));
    Point startingPoint(4, 0.0);
    Interval bounds(Point(4, -3.0), Point(4, 5.0));

    // Define Optimization Solver :
    OptimizationAlgorithm solver(new TNC());
    solver.setStartingPoint(startingPoint);

    // Define Optimization Problem : minimization
    OptimizationProblem problem(levelFunction);
    problem.setBounds(bounds);
    problem.setMinimization(true);
    solver.setProblem(problem);
    solver.run();

    fullprint << "minimizer = " << solver.getResult().getOptimalPoint() << " value=" << solver.getResult().getOptimalValue()[0] << std::endl;
    fullprint << "multipliers = " << solver.getResult().computeLagrangeMultipliers() << std::endl;

    // Define Optimization Problem : maximization
    problem.setMinimization(false);
    solver.setProblem(problem);
    solver.run();

    fullprint << "maximizer = " << solver.getResult().getOptimalPoint() << " value=" << solver.getResult().getOptimalValue()[0] << std::endl;
    fullprint << "multipliers = " << solver.getResult().computeLagrangeMultipliers() << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  try
  {

    // Test function operator ()
    const Description input = {"x1", "x2", "x3", "x4"};
    SymbolicFunction levelFunction(input, Description(1, "x1*cos(x1)+2*x2*x3-3*x3+4*x3*x4"));
    Point startingPointNearMinimizationCorner(4);
    startingPointNearMinimizationCorner[0] = 3.0;
    startingPointNearMinimizationCorner[0] = -2.5;
    startingPointNearMinimizationCorner[0] = 4.5;
    startingPointNearMinimizationCorner[0] = -2.5;
    Point startingPointNearMaximizationCorner(4);
    startingPointNearMaximizationCorner[0] = -2.5;
    startingPointNearMaximizationCorner[1] = 4.5;
    startingPointNearMaximizationCorner[2] = 4.5;
    startingPointNearMaximizationCorner[3] = 4.5;
    Interval bounds(Point(4, -3.0), Point(4, 5.0));

    // Define Optimization Solver :
    OptimizationAlgorithm solver(new TNC());

    OptimizationProblem problem(levelFunction);
    problem.setBounds(bounds);

    {
      // Define Optimization Problem : minimization
      problem.setMinimization(true);
      solver.setProblem(problem);
      solver.setStartingPoint(startingPointNearMinimizationCorner);
      solver.run();
      fullprint << "minimizer = " << solver.getResult().getOptimalPoint() << " value=" << solver.getResult().getOptimalValue()[0] << std::endl;
      fullprint << "multipliers = " << solver.getResult().computeLagrangeMultipliers() << std::endl;
    }
    {
      // Define Optimization Problem : maximization
      problem.setMinimization(false);
      solver.setProblem(problem);
      solver.setStartingPoint(startingPointNearMaximizationCorner);
      solver.run();
      fullprint << "maximizer = " << solver.getResult().getOptimalPoint() << " value=" << solver.getResult().getOptimalValue()[0] << std::endl;
      fullprint << "multipliers = " << solver.getResult().computeLagrangeMultipliers() << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
