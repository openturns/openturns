//                                               -*- C++ -*-
/**
 *  @brief The test file of class NLopt for standard methods
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
  PlatformInfo::SetNumericalPrecision(3);

  try
  {
    // bounds
    Description inVars;
    inVars.add("x1");
    inVars.add("x2");

    SymbolicFunction f(inVars, Description(1, "1+100*(x2-x1^2)^2+(1-x1)^2"));

    UnsignedInteger dim = f.getInputDimension();
    Point startingPoint(dim, 1e-3);

    Interval bounds(Point(dim, -1.5), Point(dim, 1.5));

    Description algoNames(OPTpp::GetAlgorithmNames());
    for (UnsignedInteger i = 0; i < algoNames.getSize(); ++i)
    {
      // see t_Optpp_std.py
      if ((algoNames[i] == "OptBaNewton")
          || (algoNames[i] == "OptBCNewton")
          || (algoNames[i] == "OptNewton")
          || (algoNames[i] == "OptFDNewton")
          || (algoNames[i] == "OptConstrFDNewton")
          || (algoNames[i] == "OptConstrNewton")
          || (algoNames[i] == "OptDHNIPS")
          || (algoNames[i] == "OptPDS")
          || (algoNames[i] == "OptNIPS")
          || (algoNames[i] == "OptFDNIPS"))
      {
        fullprint << "-- Skipped: algo=" << algoNames[i] << std::endl;
        continue;
      }

      OPTpp algo(algoNames[i]);
      for (UnsignedInteger minimization = 0; minimization < 2; ++minimization)
        for (UnsignedInteger inequality = 0; inequality < 2; ++inequality)
          for (UnsignedInteger equality = 0; equality < 2; ++equality)
            for (UnsignedInteger bound = 0; bound < 2; ++bound)
            {
              fullprint << "algo=" << algoNames[i] << " minimization=" << minimization << " bounds=" << bound << " inequality=" << inequality << " equality=" << equality << std::endl;
              OptimizationProblem problem(f);
              problem.setMinimization(minimization == 1);
              if (inequality)
                // x1^2+x2^2 <= 1
                problem.setInequalityConstraint(SymbolicFunction(inVars, Description(1, "1-x1^2-x2^2")));
              if (equality)
                // x1 = x2
                problem.setEqualityConstraint(SymbolicFunction(inVars, Description(1, "x1-x2")));
              if (bound)
                problem.setBounds(bounds);
              try
              {
                algo.setProblem(problem);
//                 algo.setMaximumEvaluationNumber(5000);
                algo.setStartingPoint(startingPoint);
                algo.run();
                OptimizationResult result(algo.getResult());
                fullprint << "x^=" << result.getOptimalPoint() << std::endl;
              }
              catch (...)
              {
                fullprint << "-- Not supported" << std::endl;
              }
            }
    } // algo
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
