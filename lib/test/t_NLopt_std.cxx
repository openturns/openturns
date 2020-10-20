//                                               -*- C++ -*-
/**
 *  @brief The test file of class NLopt for standard methods
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
    Description formula(1, "1+100*(x2-x1^2)^2+(1-x1)^2");

    SymbolicFunction f(inVars, formula);

    UnsignedInteger dim = f.getInputDimension();
    Point startingPoint(dim, 1e-3);

    Interval bounds(Point(dim, -1.5), Point(dim, 1.5));

    Description algoNames(NLopt::GetAlgorithmNames());
    for (UnsignedInteger i = 0; i < algoNames.getSize(); ++i)
    {
      // STOGO/AGS might not be enabled
      if ((algoNames[i] == "GD_STOGO") || (algoNames[i] == "GD_STOGO_RAND") || (algoNames[i] == "GN_AGS"))
        continue;
      // NEWUOA nan/-nan
      // COBYLA crashes on squeeze
      // ESCH not same results with 2.4.1
      // AUGLAG_EQ raises a roundoff-limited on i386
      if ((algoNames[i].find("NEWUOA") != std::string::npos)
          || (algoNames[i].find("MLSL") != std::string::npos)
          || (algoNames[i] == "LN_COBYLA")
          || (algoNames[i] == "GN_ESCH")
          || (algoNames[i].find("AUGLAG_EQ") != std::string::npos))
      {
        fullprint << "-- Skipped: algo=" << algoNames[i] << std::endl;
        continue;
      }

      NLopt algo(algoNames[i]);
      for (SignedInteger minimization = 1; minimization >= 0; -- minimization)
        for (SignedInteger inequality = 1; inequality >= 0; -- inequality)
          for (SignedInteger equality = 1; equality >= 0; -- equality)
            for(SignedInteger bound = 1; bound >= 0; -- bound)
            {
              if (!minimization && !bound)
                continue;
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
                NLopt::SetSeed(0);
                algo.setProblem(problem);
                algo.setStartingPoint(startingPoint);
                algo.setMaximumEvaluationNumber(100);
                fullprint << "algo=" << algoNames[i] << " minimization=" << minimization << " bounds=" << bound << " inequality=" << inequality << " equality=" << equality << std::endl;
                algo.run();
                OptimizationResult result(algo.getResult());
                fullprint << "x^=" << result.getOptimalPoint().__str__() << " y^=" << result.getOptimalValue().__str__() << std::endl;
              }
              catch (...)
              {
                fullprint << "-- Not supported: algo=" << algoNames[i] << " inequality=" << (inequality == 1 ? "true" : "false") << " equality=" << (equality == 1 ? "true" : "false") << std::endl;
              }
            } // bound
    } // algo
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
