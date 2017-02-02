//                                               -*- C++ -*-
/**
 *  @brief The test file of class NLopt for standard methods
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

inline String printNumericalPoint(const NumericalPoint & point, const UnsignedInteger digits)
{
  OSS oss;
  oss << "[";
  NumericalScalar eps = pow(0.1, 1.0 * digits);
  for (UnsignedInteger i = 0; i < point.getDimension(); i++)
  {
    oss << std::fixed << std::setprecision(digits) << (i == 0 ? "" : ",") << Bulk<double>((std::abs(point[i]) < eps) ? std::abs(point[i]) : point[i]);
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
    Description inVars;
    inVars.add("x");
    inVars.add("y");
    Description outVar(1, "z");
    Description formula(1, "3*(1-x)^2*exp(-x^2-(y+1)^2)-10*(x/5-x^3-y^5)*exp(-x^2-y^2)-exp(-(x+1)^2-y^2)/3");
    NumericalMathFunction model(inVars, outVar, formula);

    UnsignedInteger dim = model.getInputDimension();
    NumericalPoint startingPoint(dim);

    Interval bounds(NumericalPoint(dim, -3.0), NumericalPoint(dim, 3.0));

    // problem
    OptimizationProblem problem(model);
    problem.setBounds(bounds);

    // solver
    TNC solver(problem);
    solver.setStartingPoint(startingPoint);

    // run locally
    OptimizationSolver algo = solver;
    algo.run();
    OptimizationResult result(algo.getResult());
    fullprint << "local search x*=" << result.getOptimalPoint() << " f(x*)=" << result.getOptimalValue() << std::endl;

    // multistart
    Normal distribution(dim);
    LHSExperiment experiment(distribution, 20);
    NumericalSample startingPoints(experiment.generate());
    algo = MultiStart(solver, startingPoints);
    algo.run();
    result = algo.getResult();
    fullprint << "multistart x*=" << result.getOptimalPoint() << " f(x*)=" << result.getOptimalValue() << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
