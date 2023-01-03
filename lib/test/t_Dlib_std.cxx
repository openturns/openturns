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
    /** Rosenbrock function creation */
    Description input(2);
    input[0] = "x";
    input[1] = "y";
    Description formula(1);
    formula[0] = "(1-x)^2 + 100*(y-x^2)^2";
    SymbolicFunction rosenbrock(input, formula);

    /** Starting point definition */
    Point start(2);
    start[0] = 3;
    start[1] = -1.5;
    fullprint << "Starting point = " << start.__str__() << std::endl;

    /** Creation of OptimizationProblem */
    // Without bounds
    OptimizationProblem unboundedProblem( rosenbrock,
                                          Function(),
                                          Function(),
                                          Interval() );

    // With bounds including optimal point */
    Point lower(2, -5.0);
    Point upper(2, 5.0);
    Interval notConstrainingBounds(lower, upper);
    OptimizationProblem notConstrainingBoundsProblem( rosenbrock,
        Function(),
        Function(),
        notConstrainingBounds);

    // With bounds not including optimal point */
    lower[0] =  0.0;
    lower[1] = -2.0;
    upper[0] =  5.0;
    upper[1] =  0.5;
    Interval constrainingBounds(lower, upper);
    OptimizationProblem constrainingBoundsProblem(rosenbrock,
        Function(),
        Function(),
        constrainingBounds);

    /** REFERENCE POINTS **/
    Point unboundedRefPoint(2, 1.0);
    Point boundedRefPoint(2);
    boundedRefPoint[0] = 0.70856;
    boundedRefPoint[1] = 0.5;

    /** PRINTING TESTS **/
    Dlib printCg(unboundedProblem, "cg");
    printCg.setStartingPoint(start);
    printCg.setMaximumEvaluationNumber(100);
    printCg.setMaximumIterationNumber(100);

    fullprint << printCg << std::endl;

// ============================================================================================================================= //

    /** TESTS WITH CG METHOD **/

    fullprint << std::endl << "TESTS WITH CG ALGORITHM: " << std::endl << std::endl;

    // Without bounds
    Dlib unboundedCg(unboundedProblem, "cg");
    unboundedCg.setStartingPoint(start);
    unboundedCg.setMaximumEvaluationNumber(1000000);
    unboundedCg.setMaximumIterationNumber(100000);
    unboundedCg.run();

    fullprint << "*** Unbounded CG run completed:" << std::endl;
    fullprint << "      -- Iteration number = " << unboundedCg.getResult().getIterationNumber() << std::endl;
    fullprint << "      -- Evaluation number = " << unboundedCg.getResult().getEvaluationNumber() << std::endl;
    fullprint << "      -- optimalPoint = " << unboundedCg.getResult().getOptimalPoint().__str__() << std::endl;
    fullprint << "      -- optimalValue = " << unboundedCg.getResult().getOptimalValue().__str__() << std::endl;
    fullprint << "      -- Absolute error = " << unboundedCg.getResult().getAbsoluteError() << std::endl;
    fullprint << "      -- Relative error = " << unboundedCg.getResult().getRelativeError() << std::endl;
    fullprint << "      -- Residual error = " << unboundedCg.getResult().getResidualError() << std::endl;
    fullprint << "      -- Constraint error = " << unboundedCg.getResult().getConstraintError() << std::endl << std::endl;

    // With not constraining bounds
    Dlib notConstrainingBoundsCg(notConstrainingBoundsProblem, "cg");
    notConstrainingBoundsCg.setStartingPoint(start);
    notConstrainingBoundsCg.setMaximumEvaluationNumber(1000000);
    notConstrainingBoundsCg.setMaximumIterationNumber(100000);
    notConstrainingBoundsCg.run();

    fullprint << "*** Bounded (optimum included) CG run completed:" << std::endl;
    fullprint << "      -- Iteration number = " << notConstrainingBoundsCg.getResult().getIterationNumber() << std::endl;
    fullprint << "      -- Evaluation number = " << notConstrainingBoundsCg.getResult().getEvaluationNumber() << std::endl;
    fullprint << "      -- optimalPoint = " << notConstrainingBoundsCg.getResult().getOptimalPoint().__str__() << std::endl;
    fullprint << "      -- optimalValue = " << notConstrainingBoundsCg.getResult().getOptimalValue().__str__() << std::endl;
    fullprint << "      -- Absolute error = " << notConstrainingBoundsCg.getResult().getAbsoluteError() << std::endl;
    fullprint << "      -- Relative error = " << notConstrainingBoundsCg.getResult().getRelativeError() << std::endl;
    fullprint << "      -- Residual error = " << notConstrainingBoundsCg.getResult().getResidualError() << std::endl;
    fullprint << "      -- Constraint error = " << notConstrainingBoundsCg.getResult().getConstraintError() << std::endl << std::endl;


    // With constraining bounds
    Dlib constrainingBoundsCg(constrainingBoundsProblem, "cg");
    constrainingBoundsCg.setStartingPoint(start);
    constrainingBoundsCg.setMaximumEvaluationNumber(1000000);
    constrainingBoundsCg.setMaximumIterationNumber(100000);
    constrainingBoundsCg.run();

    fullprint << "*** Bounded (optimum not included) CG run completed:" << std::endl;
    fullprint << "      -- Iteration number = " << constrainingBoundsCg.getResult().getIterationNumber() << std::endl;
    fullprint << "      -- Evaluation number = " << constrainingBoundsCg.getResult().getEvaluationNumber() << std::endl;
    fullprint << "      -- optimalPoint = " << constrainingBoundsCg.getResult().getOptimalPoint().__str__() << std::endl;
    fullprint << "      -- optimalValue = " << constrainingBoundsCg.getResult().getOptimalValue().__str__() << std::endl;
    fullprint << "      -- Absolute error = " << constrainingBoundsCg.getResult().getAbsoluteError() << std::endl;
    fullprint << "      -- Relative error = " << constrainingBoundsCg.getResult().getRelativeError() << std::endl;
    fullprint << "      -- Residual error = " << constrainingBoundsCg.getResult().getResidualError() << std::endl;
    fullprint << "      -- Constraint error = " << constrainingBoundsCg.getResult().getConstraintError() << std::endl << std::endl;

// ============================================================================================================================= //

    /** TESTS WITH BFGS METHOD **/

    fullprint << std::endl << "TESTS WITH BFGS ALGORITHM: " << std::endl << std::endl;

    // Without bounds
    Dlib unboundedBfgs(unboundedProblem, "bfgs");
    unboundedBfgs.setStartingPoint(start);
    unboundedBfgs.setMaximumEvaluationNumber(1000000);
    unboundedBfgs.setMaximumIterationNumber(100000);
    unboundedBfgs.run();

    fullprint << "*** Unbounded BFGS run completed:" << std::endl;
    fullprint << "      -- Iteration number = " << unboundedBfgs.getResult().getIterationNumber() << std::endl;
    fullprint << "      -- Evaluation number = " << unboundedBfgs.getResult().getEvaluationNumber() << std::endl;
    fullprint << "      -- optimalPoint = " << unboundedBfgs.getResult().getOptimalPoint().__str__() << std::endl;
    fullprint << "      -- optimalValue = " << unboundedBfgs.getResult().getOptimalValue().__str__() << std::endl;
    fullprint << "      -- Absolute error = " << unboundedBfgs.getResult().getAbsoluteError() << std::endl;
    fullprint << "      -- Relative error = " << unboundedBfgs.getResult().getRelativeError() << std::endl;
    fullprint << "      -- Residual error = " << unboundedBfgs.getResult().getResidualError() << std::endl;
    fullprint << "      -- Constraint error = " << unboundedBfgs.getResult().getConstraintError() << std::endl << std::endl;

    // With not constraining bounds
    Dlib notConstrainingBoundsBfgs(notConstrainingBoundsProblem, "bfgs");
    notConstrainingBoundsBfgs.setStartingPoint(start);
    notConstrainingBoundsBfgs.setMaximumEvaluationNumber(1000000);
    notConstrainingBoundsBfgs.setMaximumIterationNumber(100000);
    notConstrainingBoundsBfgs.run();

    fullprint << "*** Bounded (optimum included) BFGS run completed:" << std::endl;
    fullprint << "      -- Iteration number = " << notConstrainingBoundsBfgs.getResult().getIterationNumber() << std::endl;
    fullprint << "      -- Evaluation number = " << notConstrainingBoundsBfgs.getResult().getEvaluationNumber() << std::endl;
    fullprint << "      -- optimalPoint = " << notConstrainingBoundsBfgs.getResult().getOptimalPoint().__str__() << std::endl;
    fullprint << "      -- optimalValue = " << notConstrainingBoundsBfgs.getResult().getOptimalValue().__str__() << std::endl;
    fullprint << "      -- Absolute error = " << notConstrainingBoundsBfgs.getResult().getAbsoluteError() << std::endl;
    fullprint << "      -- Relative error = " << notConstrainingBoundsBfgs.getResult().getRelativeError() << std::endl;
    fullprint << "      -- Residual error = " << notConstrainingBoundsBfgs.getResult().getResidualError() << std::endl;
    fullprint << "      -- Constraint error = " << notConstrainingBoundsBfgs.getResult().getConstraintError() << std::endl << std::endl;

    // With constraining bounds
    Dlib constrainingBoundsBfgs(constrainingBoundsProblem, "bfgs");
    constrainingBoundsBfgs.setStartingPoint(start);
    constrainingBoundsBfgs.setMaximumEvaluationNumber(1000000);
    constrainingBoundsBfgs.setMaximumIterationNumber(100000);
    constrainingBoundsBfgs.run();

    fullprint << "*** Bounded (optimum not included) BFGS run completed:" << std::endl;
    fullprint << "      -- Iteration number = " << constrainingBoundsBfgs.getResult().getIterationNumber() << std::endl;
    fullprint << "      -- Evaluation number = " << constrainingBoundsBfgs.getResult().getEvaluationNumber() << std::endl;
    fullprint << "      -- optimalPoint = " << constrainingBoundsBfgs.getResult().getOptimalPoint().__str__() << std::endl;
    fullprint << "      -- optimalValue = " << constrainingBoundsBfgs.getResult().getOptimalValue().__str__() << std::endl;
    fullprint << "      -- Absolute error = " << constrainingBoundsBfgs.getResult().getAbsoluteError() << std::endl;
    fullprint << "      -- Relative error = " << constrainingBoundsBfgs.getResult().getRelativeError() << std::endl;
    fullprint << "      -- Residual error = " << constrainingBoundsBfgs.getResult().getResidualError() << std::endl;
    fullprint << "      -- Constraint error = " << constrainingBoundsBfgs.getResult().getConstraintError() << std::endl << std::endl;

// ============================================================================================================================= //

    /** TESTS WITH L-BFGS METHOD **/

    fullprint << std::endl << "TESTS WITH L-BFGS ALGORITHM: " << std::endl << std::endl;

    // Without bounds
    Dlib unboundedLbfgs(unboundedProblem, "lbfgs");
    unboundedLbfgs.setStartingPoint(start);
    unboundedLbfgs.setMaximumEvaluationNumber(1000000);
    unboundedLbfgs.setMaximumIterationNumber(100000);
    unboundedLbfgs.run();

    fullprint << "*** Unbounded L-BFGS run completed:" << std::endl;
    fullprint << "      -- Iteration number = " << unboundedLbfgs.getResult().getIterationNumber() << std::endl;
    fullprint << "      -- Evaluation number = " << unboundedLbfgs.getResult().getEvaluationNumber() << std::endl;
    fullprint << "      -- optimalPoint = " << unboundedLbfgs.getResult().getOptimalPoint().__str__() << std::endl;
    fullprint << "      -- optimalValue = " << unboundedLbfgs.getResult().getOptimalValue().__str__() << std::endl;
    fullprint << "      -- Absolute error = " << unboundedLbfgs.getResult().getAbsoluteError() << std::endl;
    fullprint << "      -- Relative error = " << unboundedLbfgs.getResult().getRelativeError() << std::endl;
    fullprint << "      -- Residual error = " << unboundedLbfgs.getResult().getResidualError() << std::endl;
    fullprint << "      -- Constraint error = " << unboundedLbfgs.getResult().getConstraintError() << std::endl << std::endl;

    // With not constraining bounds
    Dlib notConstrainingBoundsLbfgs(notConstrainingBoundsProblem, "lbfgs");
    notConstrainingBoundsLbfgs.setStartingPoint(start);
    notConstrainingBoundsLbfgs.setMaximumEvaluationNumber(1000000);
    notConstrainingBoundsLbfgs.setMaximumIterationNumber(100000);
    notConstrainingBoundsLbfgs.run();

    fullprint << "*** Bounded (optimum included) L-BFGS run completed:" << std::endl;
    fullprint << "      -- Iteration number = " << notConstrainingBoundsLbfgs.getResult().getIterationNumber() << std::endl;
    fullprint << "      -- Evaluation number = " << notConstrainingBoundsLbfgs.getResult().getEvaluationNumber() << std::endl;
    fullprint << "      -- optimalPoint = " << notConstrainingBoundsLbfgs.getResult().getOptimalPoint().__str__() << std::endl;
    fullprint << "      -- optimalValue = " << notConstrainingBoundsLbfgs.getResult().getOptimalValue().__str__() << std::endl;
    fullprint << "      -- Absolute error = " << notConstrainingBoundsLbfgs.getResult().getAbsoluteError() << std::endl;
    fullprint << "      -- Relative error = " << notConstrainingBoundsLbfgs.getResult().getRelativeError() << std::endl;
    fullprint << "      -- Residual error = " << notConstrainingBoundsLbfgs.getResult().getResidualError() << std::endl;
    fullprint << "      -- Constraint error = " << notConstrainingBoundsLbfgs.getResult().getConstraintError() << std::endl << std::endl;

    // With constraining bounds
    Dlib constrainingBoundsLbfgs(constrainingBoundsProblem, "lbfgs");
    constrainingBoundsLbfgs.setStartingPoint(start);
    constrainingBoundsLbfgs.setMaximumEvaluationNumber(1000000);
    constrainingBoundsLbfgs.setMaximumIterationNumber(100000);
    constrainingBoundsLbfgs.run();

    fullprint << "*** Bounded (optimum not included) L-BFGS run completed:" << std::endl;
    fullprint << "      -- Iteration number = " << constrainingBoundsLbfgs.getResult().getIterationNumber() << std::endl;
    fullprint << "      -- Evaluation number = " << constrainingBoundsLbfgs.getResult().getEvaluationNumber() << std::endl;
    fullprint << "      -- optimalPoint = " << constrainingBoundsLbfgs.getResult().getOptimalPoint().__str__() << std::endl;
    fullprint << "      -- optimalValue = " << constrainingBoundsLbfgs.getResult().getOptimalValue().__str__() << std::endl;
    fullprint << "      -- Absolute error = " << constrainingBoundsLbfgs.getResult().getAbsoluteError() << std::endl;
    fullprint << "      -- Relative error = " << constrainingBoundsLbfgs.getResult().getRelativeError() << std::endl;
    fullprint << "      -- Residual error = " << constrainingBoundsLbfgs.getResult().getResidualError() << std::endl;
    fullprint << "      -- Constraint error = " << constrainingBoundsLbfgs.getResult().getConstraintError() << std::endl << std::endl;

// ============================================================================================================================= //

    /** TESTS WITH NEWTON METHOD **/

    fullprint << std::endl << "TESTS WITH NEWTON ALGORITHM: " << std::endl << std::endl;

    // Without bounds
    Dlib unboundedNewton(unboundedProblem, "newton");
    unboundedNewton.setStartingPoint(start);
    unboundedNewton.setMaximumEvaluationNumber(1000000);
    unboundedNewton.setMaximumIterationNumber(100000);
    unboundedNewton.run();

    fullprint << "*** Unbounded Newton run completed:" << std::endl;
    fullprint << "      -- Iteration number = " << unboundedNewton.getResult().getIterationNumber() << std::endl;
    fullprint << "      -- Evaluation number = " << unboundedNewton.getResult().getEvaluationNumber() << std::endl;
    fullprint << "      -- optimalPoint = " << unboundedNewton.getResult().getOptimalPoint().__str__() << std::endl;
    fullprint << "      -- optimalValue = " << unboundedNewton.getResult().getOptimalValue().__str__() << std::endl;
    fullprint << "      -- Absolute error = " << unboundedNewton.getResult().getAbsoluteError() << std::endl;
    fullprint << "      -- Relative error = " << unboundedNewton.getResult().getRelativeError() << std::endl;
    fullprint << "      -- Residual error = " << unboundedNewton.getResult().getResidualError() << std::endl;
    fullprint << "      -- Constraint error = " << unboundedNewton.getResult().getConstraintError() << std::endl << std::endl;

    // With not constraining bounds
    Dlib notConstrainingBoundsNewton(notConstrainingBoundsProblem, "newton");
    notConstrainingBoundsNewton.setStartingPoint(start);
    notConstrainingBoundsNewton.setMaximumEvaluationNumber(1000000);
    notConstrainingBoundsNewton.setMaximumIterationNumber(100000);
    notConstrainingBoundsNewton.run();

    fullprint << "*** Bounded (optimum included) Newton run completed:" << std::endl;
    fullprint << "      -- Iteration number = " << notConstrainingBoundsNewton.getResult().getIterationNumber() << std::endl;
    fullprint << "      -- Evaluation number = " << notConstrainingBoundsNewton.getResult().getEvaluationNumber() << std::endl;
    fullprint << "      -- optimalPoint = " << notConstrainingBoundsNewton.getResult().getOptimalPoint().__str__() << std::endl;
    fullprint << "      -- optimalValue = " << notConstrainingBoundsNewton.getResult().getOptimalValue().__str__() << std::endl;
    fullprint << "      -- Absolute error = " << notConstrainingBoundsNewton.getResult().getAbsoluteError() << std::endl;
    fullprint << "      -- Relative error = " << notConstrainingBoundsNewton.getResult().getRelativeError() << std::endl;
    fullprint << "      -- Residual error = " << notConstrainingBoundsNewton.getResult().getResidualError() << std::endl;
    fullprint << "      -- Constraint error = " << notConstrainingBoundsNewton.getResult().getConstraintError() << std::endl << std::endl;

    // With constraining bounds
    Dlib constrainingBoundsNewton(constrainingBoundsProblem, "newton");
    constrainingBoundsNewton.setStartingPoint(start);
    constrainingBoundsNewton.setMaximumEvaluationNumber(1000000);
    constrainingBoundsNewton.setMaximumIterationNumber(100000);
    constrainingBoundsNewton.run();

    fullprint << "*** Bounded (optimum not included) Newton run completed:" << std::endl;
    fullprint << "      -- Iteration number = " << constrainingBoundsNewton.getResult().getIterationNumber() << std::endl;
    fullprint << "      -- Evaluation number = " << constrainingBoundsNewton.getResult().getEvaluationNumber() << std::endl;
    fullprint << "      -- optimalPoint = " << constrainingBoundsNewton.getResult().getOptimalPoint().__str__() << std::endl;
    fullprint << "      -- optimalValue = " << constrainingBoundsNewton.getResult().getOptimalValue().__str__() << std::endl;
    fullprint << "      -- Absolute error = " << constrainingBoundsNewton.getResult().getAbsoluteError() << std::endl;
    fullprint << "      -- Relative error = " << constrainingBoundsNewton.getResult().getRelativeError() << std::endl;
    fullprint << "      -- Residual error = " << constrainingBoundsNewton.getResult().getResidualError() << std::endl;
    fullprint << "      -- Constraint error = " << constrainingBoundsNewton.getResult().getConstraintError() << std::endl << std::endl;

// ============================================================================================================================= //

    /** TESTS WITH TRUST REGION METHOD **/

    fullprint << std::endl << "TESTS WITH TRUST REGION ALGORITHM: " << std::endl << std::endl;

    // Without bounds
    Dlib unboundedTR(unboundedProblem, "trust_region");
    unboundedTR.setStartingPoint(start);
    unboundedTR.setMaximumEvaluationNumber(10000);
    unboundedTR.setMaximumIterationNumber(1000);
    unboundedTR.run();

    fullprint << "*** Unbounded TrustRegion run completed:" << std::endl;
    fullprint << "      -- Iteration number = " << unboundedTR.getResult().getIterationNumber() << std::endl;
    fullprint << "      -- Evaluation number = " << unboundedTR.getResult().getEvaluationNumber() << std::endl;
    fullprint << "      -- optimalPoint = " << unboundedTR.getResult().getOptimalPoint().__str__() << std::endl;
    fullprint << "      -- optimalValue = " << unboundedTR.getResult().getOptimalValue().__str__() << std::endl;
    fullprint << "      -- Absolute error = " << unboundedTR.getResult().getAbsoluteError() << std::endl;
    fullprint << "      -- Relative error = " << unboundedTR.getResult().getRelativeError() << std::endl;
    fullprint << "      -- Residual error = " << unboundedTR.getResult().getResidualError() << std::endl;
    fullprint << "      -- Constraint error = " << unboundedTR.getResult().getConstraintError() << std::endl << std::endl;

// ============================================================================================================================= //

    /** PREPARE TESTS WITH LSQ METHODS **/

    /** Residual function creation */
    // Model is a*x + b
    // Inputs are 2, 3, 5, 7, 9
    // Outputs are 4, 5, 7, 10, 15
    Description params(2);
    params[0] = "a";
    params[1] = "b";
    Description residualFormula(5);
    residualFormula[0] = "a*(2)+b - 4";
    residualFormula[1] = "a*(3)+b - 5";
    residualFormula[2] = "a*(5)+b - 7";
    residualFormula[3] = "a*(7)+b - 10";
    residualFormula[4] = "a*(9)+b - 15";
    SymbolicFunction residual(params, residualFormula);

    /** Creation of OptimizationProblem */
    LeastSquaresProblem lsqProblem(residual);

    /** Creation of Dlib algorithms **/
    Dlib lsqAlgorithm(lsqProblem, "least_squares");
    Dlib lsqlmAlgorithm(lsqProblem, "least_squares_lm");

    /** Creation of reference point **/
    Point lsqRefPoint(2);
    lsqRefPoint[0] = 1.518;
    lsqRefPoint[1] = 0.305;

// ============================================================================================================================= //

    /** TESTS WITH LSQ METHOD **/

    // Printing
    fullprint << std::endl << "TESTS WITH LSQ ALGORITHM: " << std::endl << std::endl;

    // Without bounds
    lsqAlgorithm.setStartingPoint(start);
    lsqAlgorithm.setMaximumEvaluationNumber(10000);
    lsqAlgorithm.setMaximumIterationNumber(100);

    fullprint << lsqAlgorithm << std::endl << std::endl;

    lsqAlgorithm.run();

    fullprint << "*** LSQ run completed:" << std::endl;
    fullprint << "      -- Iteration number = " << lsqAlgorithm.getResult().getIterationNumber() << std::endl;
    fullprint << "      -- Evaluation number = " << lsqAlgorithm.getResult().getEvaluationNumber() << std::endl;
    fullprint << "      -- optimalPoint = " << lsqAlgorithm.getResult().getOptimalPoint().__str__() << std::endl;
    fullprint << "      -- optimalValue = " << lsqAlgorithm.getResult().getOptimalValue().__str__() << std::endl;
    fullprint << "      -- Absolute error = " << lsqAlgorithm.getResult().getAbsoluteError() << std::endl;
    fullprint << "      -- Relative error = " << lsqAlgorithm.getResult().getRelativeError() << std::endl;
    fullprint << "      -- Residual error = " << lsqAlgorithm.getResult().getResidualError() << std::endl;
    fullprint << "      -- Constraint error = " << lsqAlgorithm.getResult().getConstraintError() << std::endl << std::endl;

// ============================================================================================================================= //

    /** TESTS WITH LSQLM METHOD **/

    fullprint << std::endl << "TESTS WITH LSQLM ALGORITHM: " << std::endl << std::endl;

    // Without bounds
    lsqlmAlgorithm.setStartingPoint(start);
    lsqlmAlgorithm.setMaximumEvaluationNumber(10000);
    lsqlmAlgorithm.setMaximumIterationNumber(1000);
    lsqlmAlgorithm.run();

    fullprint << "*** LSQLM run completed:" << std::endl;
    fullprint << "      -- Iteration number = " << lsqlmAlgorithm.getResult().getIterationNumber() << std::endl;
    fullprint << "      -- Evaluation number = " << lsqlmAlgorithm.getResult().getEvaluationNumber() << std::endl;
    fullprint << "      -- optimalPoint = " << lsqlmAlgorithm.getResult().getOptimalPoint().__str__() << std::endl;
    fullprint << "      -- optimalValue = " << lsqlmAlgorithm.getResult().getOptimalValue().__str__() << std::endl;
    fullprint << "      -- Absolute error = " << lsqlmAlgorithm.getResult().getAbsoluteError() << std::endl;
    fullprint << "      -- Relative error = " << lsqlmAlgorithm.getResult().getRelativeError() << std::endl;
    fullprint << "      -- Residual error = " << lsqlmAlgorithm.getResult().getResidualError() << std::endl;
    fullprint << "      -- Constraint error = " << lsqlmAlgorithm.getResult().getConstraintError() << std::endl << std::endl;

// Check results

    assert_almost_equal(unboundedCg.getResult().getOptimalPoint(), unboundedRefPoint, 0.05);
    assert_almost_equal(notConstrainingBoundsCg.getResult().getOptimalPoint(), unboundedRefPoint, 0.05);
    assert_almost_equal(constrainingBoundsCg.getResult().getOptimalPoint(), boundedRefPoint, 0.05);

    assert_almost_equal(unboundedBfgs.getResult().getOptimalPoint(), unboundedRefPoint, 0.05);
    assert_almost_equal(notConstrainingBoundsBfgs.getResult().getOptimalPoint(), unboundedRefPoint, 0.05);
    assert_almost_equal(constrainingBoundsBfgs.getResult().getOptimalPoint(), boundedRefPoint, 0.05);

    assert_almost_equal(unboundedLbfgs.getResult().getOptimalPoint(), unboundedRefPoint, 0.05);
    assert_almost_equal(notConstrainingBoundsLbfgs.getResult().getOptimalPoint(), unboundedRefPoint, 0.05);
    assert_almost_equal(constrainingBoundsLbfgs.getResult().getOptimalPoint(), boundedRefPoint, 0.05);

    assert_almost_equal(unboundedNewton.getResult().getOptimalPoint(), unboundedRefPoint, 0.05);
    assert_almost_equal(notConstrainingBoundsNewton.getResult().getOptimalPoint(), unboundedRefPoint, 0.05);
    assert_almost_equal(constrainingBoundsNewton.getResult().getOptimalPoint(), boundedRefPoint, 0.05);

    assert_almost_equal(unboundedTR.getResult().getOptimalPoint(), unboundedRefPoint, 0.5);

    assert_almost_equal(lsqAlgorithm.getResult().getOptimalPoint(), lsqRefPoint, 0.05);
    assert_almost_equal(lsqlmAlgorithm.getResult().getOptimalPoint(), lsqRefPoint, 0.05);

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
