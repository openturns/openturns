//                                               -*- C++ -*-
/**
 *  @brief The test file of class Dlib for standard methods
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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



int main()
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    /** Rastrigin function creation */
    Description input(2);
    input[0] = "x";
    input[1] = "y";
    Description formula(1);
    formula[0] = "20 + x^2 - 10*cos(2*pi_*x) + y^2 - 10*cos(2*pi_*y)";
    SymbolicFunction rastrigin(input, formula);


    /** Creation of OptimizationProblem */
    // Without bounds
    OptimizationProblem unboundedProblem( rastrigin,
                                          Function(),
                                          Function(),
                                          Interval() );

    // With bounds including optimal point */
    Point lower(2, -5.0);
    Point upper(2, 2.0);
    Interval notConstrainingBounds(lower, upper);
    OptimizationProblem notConstrainingBoundsProblem( rastrigin,
        Function(),
        Function(),
        notConstrainingBounds);

    // With bounds not including optimal point */
    lower[0] = -1.0;
    lower[1] = -2.0;
    upper[0] =  5.0;
    upper[1] = -0.5;
    Interval constrainingBounds(lower, upper);
    OptimizationProblem constrainingBoundsProblem(rastrigin,
        Function(),
        Function(),
        constrainingBounds);
    /** REFERENCE POINTS **/
    Point unboundedRefPoint(2, 0.0);
    Point boundedRefPoint(2);
    boundedRefPoint[0] = 0.0;
    boundedRefPoint[1] = -1.0;

// ============================================================================================================================= //

    /** TESTS WITH GLOBAL METHOD **/

    fullprint << std::endl << "TESTS WITH GLOBAL ALGORITHM: " << std::endl << std::endl;

    // With not constraining bounds
    Dlib notConstrainingBoundsGlobal(notConstrainingBoundsProblem, "global");
    notConstrainingBoundsGlobal.setMaximumEvaluationNumber(300);
    notConstrainingBoundsGlobal.setStartingPoint(Point(2));
    notConstrainingBoundsGlobal.run();

    fullprint << "*** Bounded (optimum included) Global run completed:" << std::endl;
    fullprint << "      -- Evaluation number = " << notConstrainingBoundsGlobal.getResult().getEvaluationNumber() << std::endl;
    fullprint << "      -- optimalPoint = " << notConstrainingBoundsGlobal.getResult().getOptimalPoint().__str__() << std::endl;
    fullprint << "      -- optimalValue = " << notConstrainingBoundsGlobal.getResult().getOptimalValue().__str__() << std::endl << std::endl;

    // With constraining bounds
    Dlib constrainingBoundsGlobal(constrainingBoundsProblem, "global");
    constrainingBoundsGlobal.setMaximumEvaluationNumber(300);
    constrainingBoundsGlobal.setStartingPoint(Point(2));
    constrainingBoundsGlobal.run();

    fullprint << "*** Bounded (optimum not included) Global run completed:" << std::endl;
    fullprint << "      -- Evaluation number = " << constrainingBoundsGlobal.getResult().getEvaluationNumber() << std::endl;
    fullprint << "      -- optimalPoint = " << constrainingBoundsGlobal.getResult().getOptimalPoint().__str__() << std::endl;
    fullprint << "      -- optimalValue = " << constrainingBoundsGlobal.getResult().getOptimalValue().__str__() << std::endl << std::endl;

    // Check results
    if (notConstrainingBoundsGlobal.getResult().getOptimalValue()[0] > 4.0)
      throw InvalidArgumentException(HERE) << "optimum not found";
    assert_almost_equal(constrainingBoundsGlobal.getResult().getOptimalPoint(), boundedRefPoint, 1, 1e-2);

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
