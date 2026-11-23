//                                               -*- C++ -*-
/**
 *  @brief The test file of class MinlpProblem
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
#include "openturns/MinlpProblem.hxx"

#include <chrono>

using namespace OT;
using namespace OT::Test;

static void check(const Bool condition, const String & message)
{
  if (!condition)
    throw TestFailed(message);
}

static Bool alwaysStop(void *)
{
  return true;
}

int main()
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

    //--------------------------------------------------------------------
    // Structured linear problem: L <= A.x <= U is normalized as functions
    //--------------------------------------------------------------------
    const Point cost = {1.0, 2.0, 3.0};
    const Interval bounds(Point(3, -5.0), Point(3, 5.0));

    Matrix coefficients(4, 3);
    coefficients(0, 0) = 1.0;
    coefficients(0, 1) = 1.0;
    coefficients(1, 0) = 1.0;
    coefficients(1, 2) = 1.0;
    coefficients(2, 1) = 1.0;
    coefficients(2, 2) = 1.0;
    // the last row is unconstrained

    const Scalar infinity = SpecFunc::Infinity;
    const Point constraintLower = {1.0, 0.0, -1.0, -infinity};
    const Point constraintUpper = {1.0, 2.0, infinity, infinity};
    const Interval constraintBounds(constraintLower, constraintUpper);

    LinearProgrammingProblem linearProblem(cost, bounds, coefficients, constraintBounds);
    linearProblem.setVariablesType({OptimizationProblemImplementation::CONTINUOUS,
                                    OptimizationProblemImplementation::INTEGER,
                                    OptimizationProblemImplementation::CONTINUOUS});

    MinlpProblem linearCore(linearProblem, Point(3), 100, 0.0, now);
    check(linearCore.getDimension() == 3, "wrong dimension");
    check(linearCore.isLinear(), "the problem should be linear");
    check(linearCore.hasEqualityConstraint(), "missing equality constraint");
    check(linearCore.getEqualityDimension() == 1, "wrong equality dimension");
    check(linearCore.hasInequalityConstraint(), "missing inequality constraint");
    check(linearCore.getInequalityDimension() == 3, "wrong inequality dimension");

    const Point x = {1.0, 0.0, 2.0};

    Point equalityValues;
    check(linearCore.evaluateEqualityConstraint(x, equalityValues), "equality evaluation failed");
    assert_almost_equal(equalityValues, Point(1, 0.0));

    Point inequalityValues;
    check(linearCore.evaluateInequalityConstraint(x, inequalityValues), "inequality evaluation failed");
    assert_almost_equal(inequalityValues, Point({3.0, -1.0, 3.0}));

    Matrix equalityGradient;
    check(linearCore.evaluateEqualityConstraintGradient(x, equalityGradient), "equality gradient failed");
    Matrix referenceEqualityGradient(3, 1);
    referenceEqualityGradient(0, 0) = 1.0;
    referenceEqualityGradient(1, 0) = 1.0;
    assert_almost_equal(equalityGradient, referenceEqualityGradient);

    Matrix inequalityGradient;
    check(linearCore.evaluateInequalityConstraintGradient(x, inequalityGradient), "inequality gradient failed");
    Matrix referenceInequalityGradient(3, 3);
    referenceInequalityGradient(0, 0) = 1.0;
    referenceInequalityGradient(2, 0) = 1.0;
    referenceInequalityGradient(0, 1) = -1.0;
    referenceInequalityGradient(2, 1) = -1.0;
    referenceInequalityGradient(1, 2) = 1.0;
    referenceInequalityGradient(2, 2) = 1.0;
    assert_almost_equal(inequalityGradient, referenceInequalityGradient);

    Scalar marginalValue;
    check(linearCore.evaluateConstraintMarginal(0, x, marginalValue), "marginal evaluation failed");
    assert_almost_equal(marginalValue, 0.0);
    check(linearCore.evaluateConstraintMarginal(1, x, marginalValue), "marginal evaluation failed");
    assert_almost_equal(marginalValue, 3.0);
    check(linearCore.evaluateConstraintMarginal(3, x, marginalValue), "marginal evaluation failed");
    assert_almost_equal(marginalValue, 3.0);

    // The normalized problem exposes the matrix constraints as functions
    const OptimizationProblem normalized(linearCore.getNormalizedProblem());
    check(normalized.hasEqualityConstraint(), "normalized problem lost the equality constraint");
    check(normalized.hasInequalityConstraint(), "normalized problem lost the inequality constraint");
    check(normalized.getEqualityConstraint().getOutputDimension() == 1, "wrong normalized equality dimension");
    check(normalized.getInequalityConstraint().getOutputDimension() == 3, "wrong normalized inequality dimension");
    check(normalized.getVariablesType()[1] == OptimizationProblemImplementation::INTEGER, "normalized problem lost the variables type");

    // setLinearCost must not reset the variables types
    LinearProgrammingProblem costedProblem(cost, bounds, coefficients, constraintBounds);
    costedProblem.setVariablesType({OptimizationProblemImplementation::CONTINUOUS,
                                    OptimizationProblemImplementation::INTEGER,
                                    OptimizationProblemImplementation::CONTINUOUS});
    costedProblem.setLinearCost(Point({3.0, 2.0, 1.0}));
    check(costedProblem.getVariablesType()[1] == OptimizationProblemImplementation::INTEGER, "setLinearCost reset the variables types");
    assert_almost_equal(costedProblem.getLinearCost(), Point({3.0, 2.0, 1.0}));

    //--------------------------------------------------------------------
    // Generic nonlinear problem: the constraints are used as is
    //--------------------------------------------------------------------
    const SymbolicFunction objectiveFunction(Description({"x0", "x1"}),
                                             Description({"x0^2 + x1^2"}));
    const SymbolicFunction equalityFunction(Description({"x0", "x1"}),
                                            Description({"x0 + x1 - 2"}));
    const SymbolicFunction inequalityFunction(Description({"x0", "x1"}),
                                              Description({"x0 - x1"}));
    OptimizationProblem problem(objectiveFunction);
    problem.setEqualityConstraint(equalityFunction);
    problem.setInequalityConstraint(inequalityFunction);
    problem.setBounds(Interval(Point(2, -10.0), Point(2, 10.0)));

    MinlpProblem core(problem, Point(2, 1.0), 100, 0.0, now);
    check(!core.isLinear(), "the problem should not be linear");
    check(core.getEqualityDimension() == 1, "wrong equality dimension");
    check(core.getInequalityDimension() == 1, "wrong inequality dimension");

    Scalar value;
    check(core.evaluateObjective(Point({1.0, 1.0}), value), "objective evaluation failed");
    assert_almost_equal(value, 2.0);
    check(core.getInputHistory().getSize() == 1, "wrong history size");
    check(core.getOutputHistory().getSize() == 1, "wrong history size");

    Matrix gradient;
    check(core.evaluateObjectiveGradient(Point({1.0, 1.0}), gradient), "objective gradient failed");
    Matrix referenceGradient(2, 1);
    referenceGradient(0, 0) = 2.0;
    referenceGradient(1, 0) = 2.0;
    assert_almost_equal(gradient, referenceGradient);
    check(core.getGradientCallsNumber() == 1, "wrong gradient calls number");

    SymmetricMatrix hessian;
    check(core.evaluateObjectiveHessian(Point({1.0, 1.0}), hessian), "objective hessian failed");
    SymmetricMatrix referenceHessian(2);
    referenceHessian(0, 0) = 2.0;
    referenceHessian(1, 1) = 2.0;
    assert_almost_equal(hessian, referenceHessian);
    check(core.getHessianCallsNumber() == 1, "wrong hessian calls number");

    //--------------------------------------------------------------------
    // Budget, stop callback and time limit
    //--------------------------------------------------------------------
    MinlpProblem budgetCore(problem, Point(2, 1.0), 2, 0.0, now);
    check(budgetCore.evaluateObjective(Point(2, 0.0), value), "first evaluation should be allowed");
    check(budgetCore.evaluateObjective(Point(2, 0.0), value), "second evaluation should be allowed");
    check(!budgetCore.evaluateObjective(Point(2, 0.0), value), "third evaluation should exceed the budget");
    check(budgetCore.isCallLimitReached(), "the call limit should be reached");
    check(!budgetCore.checkBudget(), "the budget should be exhausted");
    check(budgetCore.getInputHistory().getSize() == 3, "the failing evaluation should be in the history");

    MinlpProblem stopCore(problem, Point(2, 1.0), 10, 0.0, now);
    stopCore.setStopCallback(alwaysStop);
    check(!stopCore.evaluateObjective(Point(2, 0.0), value), "the stop callback should stop the evaluation");
    check(stopCore.isStopped(), "the problem should be stopped");

    const std::chrono::steady_clock::time_point past = std::chrono::steady_clock::now() - std::chrono::seconds(10);
    MinlpProblem timeoutCore(problem, Point(2, 1.0), 10, 1.0, past);
    check(!timeoutCore.evaluateObjective(Point(2, 0.0), value), "the time limit should stop the evaluation");
    check(timeoutCore.isTimeOut(), "the problem should have timed out");

    fullprint << "TEST PASSED" << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
