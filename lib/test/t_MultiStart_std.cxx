//                                               -*- C++ -*-
/**
 *  @brief The test file of class NLopt for standard methods
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
    Description inVars;
    inVars.add("x");
    inVars.add("y");
    Description formula(1, "3*(1-x)^2*exp(-x^2-(y+1)^2)-10*(x/5-x^3-y^5)*exp(-x^2-y^2)-exp(-(x+1)^2-y^2)/3");
    SymbolicFunction model(inVars, formula);

    UnsignedInteger dim = model.getInputDimension();
    Point startingPoint(dim);

    Interval bounds(Point(dim, -3.0), Point(dim, 3.0));

    // problem
    OptimizationProblem problem(model);
    problem.setBounds(bounds);

    // solver
    TNC solver(problem);
    solver.setStartingPoint(startingPoint);

    // run locally
    solver.run();
    OptimizationResult result(solver.getResult());
    const Point localOptimalPoint = {0.296446, 0.320196};
    Scalar localOptimalValue = -0.0649359;
    assert_almost_equal(result.getOptimalPoint(), localOptimalPoint, 1e-5, 0.0);
    assert_almost_equal(result.getOptimalValue()[0], localOptimalValue, 1e-5, 0.0);


    // multistart
    Normal distribution(dim);
    const UnsignedInteger size = 20;
    LHSExperiment experiment(distribution, size);
    Sample startingSample(experiment.generate());
    MultiStart algo(solver, startingSample);
    algo.setMaximumEvaluationNumber(100);
    algo.run();
    result = algo.getResult();
    const Point trueOptimalPoint = {0.228279, -1.62553};
    Scalar trueOptimalValue = -6.55113;
    assert_almost_equal(result.getOptimalPoint(), trueOptimalPoint, 1e-5, 0.0);
    assert_almost_equal(result.getOptimalValue()[0], trueOptimalValue, 1e-5, 0.0);
    fullprint << "intermediate results=" << algo.getResultCollection() << std::endl;
    // Deactivate intermediate results history
    algo = MultiStart(solver, startingSample);
    algo.setKeepResults(false);
    algo.run();
    fullprint << "intermediate results=" << algo.getResultCollection() << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
