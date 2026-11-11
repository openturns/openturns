#! /usr/bin/env python

import os

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# min 1/2 x^T Q x + c^T x + d with 0 <= x0 <= 0.5, 0 <= x1 <= 10
Q = ot.SymmetricMatrix([[2.0, 0.0], [0.0, 2.0]])
c = [-2.0, -4.0]
bounds = ot.Interval([0.0, 0.0], [0.5, 10.0])
problem = otexp.QuadraticProblem(Q, c, bounds)
problem.setConstant(1.0)

assert problem.isQuadratic()
assert not problem.isLinear()
assert problem.getHessian() == Q
assert problem.getLinearCost() == c
assert problem.getConstant() == 1.0
assert problem.getBounds() == bounds
print(problem)

# the objective includes the constant term
ott.assert_almost_equal(problem.getObjective()([0.5, 2.0]), [-3.75])

# bounds can be replaced or cleared
problem.setBounds(ot.Interval([0.0, 0.0], [0.25, 10.0]))
assert problem.getBounds() == ot.Interval([0.0, 0.0], [0.25, 10.0])
problem.setBounds(ot.Interval())
assert problem.getBounds().getDimension() == 0
problem.setBounds(bounds)
assert problem.getBounds() == bounds

# methods added to the OptimizationProblem base class
implementation = ot.OptimizationProblemImplementation()
assert not implementation.isQuadratic()
assert implementation.getConstant() == 0.0
with ott.assert_raises(RuntimeError):
    implementation.getHessian()

algo = otexp.HiGHS(problem)
assert algo.getAlgorithmName() == "choose"
assert "qpasm" in otexp.HiGHS.GetAlgorithmNames()
algo.run()
result = algo.getResult()
assert result.getStatus() == ot.OptimizationResult.SUCCESS
ott.assert_almost_equal(result.getOptimalPoint(), [0.5, 2.0])
ott.assert_almost_equal(result.getOptimalValue(), [-3.75])

# active-set QP solver: the qpasm name requires HiGHS 1.14 or later, so an
# older version rejects it either when the algorithm is built or when it runs.
try:
    algo = otexp.HiGHS(problem, "qpasm")
    algo.run()
except TypeError:
    pass
else:
    ott.assert_almost_equal(algo.getResult().getOptimalPoint(), [0.5, 2.0])

# HiGHS builds without the parallel interior point solver reject "hipo"
# at run time: GetAlgorithmNames() lists it but not every build provides it.
try:
    algoHipo = otexp.HiGHS(problem, "hipo")
    algoHipo.run()
except TypeError:
    pass
else:
    ott.assert_almost_equal(algoHipo.getResult().getOptimalPoint(), [0.5, 2.0])

# LP-only solvers are rejected on a quadratic problem
for solver in ["simplex", "ipm", "ipx", "pdlp", "hipdlp"]:
    with ott.assert_raises(TypeError):
        otexp.HiGHS(problem, solver)

# mixed-integer quadratic problems are not supported
problemMIP = otexp.QuadraticProblem(Q, c, bounds)
problemMIP.setVariablesType(
    [
        ot.OptimizationProblemImplementation.CONTINUOUS,
        ot.OptimizationProblemImplementation.INTEGER,
    ]
)
with ott.assert_raises(TypeError):
    otexp.HiGHS(problemMIP)

# constraint functions cannot be mapped to the HiGHS model: only the linear
# constraints given to setLinearConstraint are supported
problemEquality = otexp.QuadraticProblem(Q, c, ot.Interval())
problemEquality.setEqualityConstraint(ot.SymbolicFunction(["x0", "x1"], ["x0 + x1"]))
with ott.assert_raises(TypeError):
    otexp.HiGHS(problemEquality)
problemInequality = otexp.QuadraticProblem(Q, c, ot.Interval())
problemInequality.setInequalityConstraint(ot.SymbolicFunction(["x0", "x1"], ["x0 + x1"]))
with ott.assert_raises(TypeError):
    otexp.HiGHS(problemInequality)

# least-squares factory: min ||A.x-b||^2 with 0 <= x0 <= 0.5, 0 <= x1 <= 10
A = ot.Matrix([[1.0, 0.0], [0.0, 1.0], [1.0, 1.0]])
b = [1.0, 2.0, 3.0]
problemLS = otexp.QuadraticProblem.FromLeastSquares(A, b, bounds)
assert problemLS.isQuadratic()
ott.assert_almost_equal(
    problemLS.getHessian(), ot.SymmetricMatrix([[4.0, 2.0], [2.0, 4.0]])
)
ott.assert_almost_equal(problemLS.getLinearCost(), [-8.0, -10.0])
ott.assert_almost_equal(problemLS.getConstant(), 14.0)
algoLS = otexp.HiGHS(problemLS)
algoLS.run()
resultLS = algoLS.getResult()
ott.assert_almost_equal(resultLS.getOptimalPoint(), [0.5, 2.25])
# ||A.x-b||^2 = 0.375
ott.assert_almost_equal(resultLS.getOptimalValue(), [0.375])

# empty bounds leave the least-squares problem unconstrained: solution [1, 2]
problemLSUnbounded = otexp.QuadraticProblem.FromLeastSquares(A, b, ot.Interval())
algoLSUnbounded = otexp.HiGHS(problemLSUnbounded)
algoLSUnbounded.run()
ott.assert_almost_equal(algoLSUnbounded.getResult().getOptimalPoint(), [1.0, 2.0])
ott.assert_almost_equal(algoLSUnbounded.getResult().getOptimalValue(), [0.0])

# linear constraints: projection of (1, 2) onto x0 + x1 = 2.5
Aconstraint = ot.Matrix([[1.0, 1.0]])
constraintBounds = ot.Interval([2.5], [2.5])
problemC = otexp.QuadraticProblem(
    Q, c, ot.Interval(), Aconstraint, constraintBounds
)
algoC = otexp.HiGHS(problemC)
algoC.run()
ott.assert_almost_equal(algoC.getResult().getOptimalPoint(), [0.75, 1.75])

# inequality constraint x0 + x1 <= 2.5
constraintBoundsUpper = ot.Interval([-1e30], [2.5], [False], [True])
problemIneq = otexp.QuadraticProblem(
    Q, c, ot.Interval(), Aconstraint, constraintBoundsUpper
)
algoIneq = otexp.HiGHS(problemIneq)
algoIneq.run()
ott.assert_almost_equal(algoIneq.getResult().getOptimalPoint(), [0.75, 1.75], 1e-4)

# accessors
problemC.setHessian(Q)
problemC.setLinearCost(c)
problemC.setConstant(0.0)
problemC.setLinearConstraint(Aconstraint, constraintBounds)
assert problemC.getLinearConstraintCoefficients() == Aconstraint
assert problemC.getLinearConstraintBounds() == constraintBounds

# dimension checks
with ott.assert_raises(ValueError):
    otexp.QuadraticProblem(ot.SymmetricMatrix(2), [1.0], bounds)
with ott.assert_raises(TypeError):
    otexp.QuadraticProblem(Q, c, ot.Interval([0.0], [1.0]))
with ott.assert_raises(ValueError):
    problem.setHessian(ot.SymmetricMatrix(1))
with ott.assert_raises(ValueError):
    problem.setLinearCost([1.0])
with ott.assert_raises(ValueError):
    otexp.QuadraticProblem.FromLeastSquares(
        ot.Matrix(2, 1), [1.0, 2.0, 3.0], ot.Interval()
    )
with ott.assert_raises(ValueError):
    otexp.QuadraticProblem.FromLeastSquares(
        ot.Matrix(2, 1), [1.0, 2.0], ot.Interval(2)
    )
with ott.assert_raises(ValueError):
    problemC.setLinearConstraint(ot.Matrix(2, 1), ot.Interval(2))
with ott.assert_raises(ValueError):
    problemC.setLinearConstraint(ot.Matrix(1, 3), ot.Interval(1))

# save/load round trip
studyFile = "quadratic_problem.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(studyFile))
study.add("problem", problemLS)
study.add("problemC", problemC)
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager(studyFile))
loadedStudy.load()
loadedProblem = otexp.QuadraticProblem()
loadedStudy.fillObject("problem", loadedProblem)
assert loadedProblem.getHessian() == problemLS.getHessian()
assert loadedProblem.getLinearCost() == problemLS.getLinearCost()
assert loadedProblem.getConstant() == problemLS.getConstant()
assert loadedProblem.getBounds() == problemLS.getBounds()
assert loadedProblem.isQuadratic()
ott.assert_almost_equal(loadedProblem.getObjective()([0.0, 0.0]), [14.0])
loadedProblemC = otexp.QuadraticProblem()
loadedStudy.fillObject("problemC", loadedProblemC)
assert loadedProblemC.getLinearConstraintCoefficients() == Aconstraint
assert loadedProblemC.getLinearConstraintBounds() == constraintBounds
os.remove(studyFile)
