//                                               -*- C++ -*-
/**
 *  @brief The test file of class NLopt for standard methods
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
  NumericalScalar eps(pow(0.1, 1.0 * digits));
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
      // bounds
      Description inVars;
      inVars.add("x1");
      inVars.add("x2");
      inVars.add("x3");
      inVars.add("x4");
      Description outVar(1, "y1");
      Description formula(1, "x1+2*x2-3*x3+4*x4");

      NumericalMathFunction linear(inVars, outVar, formula);

      UnsignedInteger dim(linear.getInputDimension());
      NumericalPoint startingPoint(dim);

      Interval bounds(NumericalPoint(dim, -3.0), NumericalPoint(dim, 5.0));

      Collection<OptimizationSolver> allAlgo;
      allAlgo.add(SLSQP());
      allAlgo.add(LBFGS());
      allAlgo.add(MMA());
      allAlgo.add(NelderMead());
      allAlgo.add(CCSAQ());
      allAlgo.add(COBYLANLOPT());
      allAlgo.add(BOBYQA());
      Indices allCodes(NLopt::GetAlgorithmCodes());
      for (UnsignedInteger i = 0; i < allCodes.getSize(); ++i)
        allAlgo.add(NLopt(allCodes[i]));
      for (UnsignedInteger i = 0; i < allAlgo.getSize(); ++i)
        {
          OptimizationSolver algo(allAlgo[i]);
	  // Here we skip all the odd algorithms that results either in infinite loop or segmentation fault, as of NLopt version 2.4.2
	  if ((i == 5) || (i == 9) || (i == 10) || (i == 22) || (i == 23) || (i == 41) || (i == 42) || (i == 44) || (i == 46))
	    {
	      fullprint << "-- Skipped: algo=" << algo.getClassName() << std::endl;
	      continue;
	    }
          for (UnsignedInteger minimization = 0; minimization < 2; ++minimization)
            for (UnsignedInteger inequality = 0; inequality < 2; ++inequality)
              for (UnsignedInteger equality = 0; equality < 2; ++equality)
                {
                  OptimizationProblem problem(linear, NumericalMathFunction(), NumericalMathFunction(), bounds);
                  problem.setMinimization(minimization == 0);
                  if (inequality == 0)
                    // x3 <= x1
                    problem.setInequalityConstraint(NumericalMathFunction(inVars, Description(1, "ineq"), Description(1, "x1-x3")));
                  if (equality == 0)
                    // x4 = 2
                    problem.setEqualityConstraint(NumericalMathFunction(inVars, Description(1, "eq"), Description(1, "x4-2")));
                  try
                    {
                      NLopt::SetSeed(0);
                      algo.setProblem(problem);
                      algo.setStartingPoint(startingPoint);
                      fullprint << "algo=" << algo << std::endl;
                      algo.run();
                      OptimizationResult result(algo.getResult());
                      fullprint << "x^=" << printNumericalPoint(result.getOptimalPoint(), 3) << std::endl;
                    }
                  catch (...)
                    {
                      fullprint << "-- Not supported: algo=" << algo.getClassName() << " inequality=" << (inequality == 0 ? "true" : "false") << " equality=" << (equality == 0 ? "true" : "false") << std::endl;
                    }
                } // equality
        } // algo
    }
  catch (TestFailed & ex)
    {
      std::cerr << ex << std::endl;
      return ExitCode::Error;
    }

  return ExitCode::Success;
}
