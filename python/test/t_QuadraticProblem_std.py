#! /usr/bin/env python

import os

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

HAVE_LIBXML2 = ot.PlatformInfo.HasFeature("libxml2")

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

# the constant term can also be given to the constructor
problemConstant = otexp.QuadraticProblem(Q, c, bounds, 2.0)
assert problemConstant.getConstant() == 2.0
ott.assert_almost_equal(problemConstant.getObjective()([0.5, 2.0]), [-2.75])

# a generic objective replacement would leave stale quadratic data, it is
# rejected in favor of the Hessian/linear cost/constant accessors
with ott.assert_raises(TypeError):
    problem.setObjective(ot.SymbolicFunction(["x0", "x1"], ["x0 + x1"]))

# updating a quadratic coefficient keeps the variable types
integerTypes = [
    ot.OptimizationProblemImplementation.INTEGER,
    ot.OptimizationProblemImplementation.CONTINUOUS,
]
problem.setVariablesType(integerTypes)
problem.setLinearCost(c)
assert problem.getVariablesType() == integerTypes
problem.setVariablesType([ot.OptimizationProblemImplementation.CONTINUOUS] * 2)

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
# constraints given to setLinearConstraint are supported. A QuadraticProblem
# refuses nonlinear constraint functions outright; a generic problem that
# carries them is rejected by HiGHS.
problemFunctions = otexp.QuadraticProblem(Q, c, ot.Interval())
with ott.assert_raises(TypeError):
    problemFunctions.setEqualityConstraint(
        ot.SymbolicFunction(["x0", "x1"], ["x0 + x1"])
    )
with ott.assert_raises(TypeError):
    problemFunctions.setInequalityConstraint(
        ot.SymbolicFunction(["x0", "x1"], ["x0 + x1"])
    )
baseProblem = ot.OptimizationProblem(ot.SymbolicFunction(["x0", "x1"], ["x0^2 + x1^2"]))
baseProblem.setEqualityConstraint(ot.SymbolicFunction(["x0", "x1"], ["x0 + x1"]))
with ott.assert_raises(TypeError):
    otexp.HiGHS(baseProblem)

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
problemC = otexp.QuadraticProblem(Q, c, ot.Interval(), Aconstraint, constraintBounds)
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

# the getters always return linear functions of the problem dimension, even
# when no constraint of that kind is defined
emptyEquality = problem.getEqualityConstraint()
emptyInequality = problem.getInequalityConstraint()
for emptyConstraint in [emptyEquality, emptyInequality]:
    assert isinstance(
        emptyConstraint.getEvaluation().getImplementation(), ot.LinearEvaluation
    )
    assert emptyConstraint.getInputDimension() == 2
    assert emptyConstraint.getOutputDimension() == 0
assert not problem.hasEqualityConstraint()
assert not problem.hasInequalityConstraint()

# equality and inequality constraints are derived from the linear constraints
assert problemC.hasEqualityConstraint()
assert not problemC.hasInequalityConstraint()
equalityC = problemC.getEqualityConstraint()
assert isinstance(equalityC.getEvaluation().getImplementation(), ot.LinearEvaluation)
assert equalityC.getInputDimension() == 2
assert equalityC.getOutputDimension() == 1
ott.assert_almost_equal(equalityC([0.75, 1.75]), [0.0])
ott.assert_almost_equal(equalityC([1.0, 1.0]), [-0.5])

assert problemIneq.hasInequalityConstraint()
assert not problemIneq.hasEqualityConstraint()
inequalityC = problemIneq.getInequalityConstraint()
assert isinstance(inequalityC.getEvaluation().getImplementation(), ot.LinearEvaluation)
assert inequalityC.getInputDimension() == 2
assert inequalityC.getOutputDimension() == 1
ott.assert_almost_equal(inequalityC([0.75, 1.75]), [0.0])
ott.assert_almost_equal(inequalityC([0.0, 0.0]), [2.5])

# a two-sided constraint generates one component per finite bound
problemTwoSided = otexp.QuadraticProblem(
    Q, c, ot.Interval(), Aconstraint, ot.Interval([0.0], [2.5])
)
inequalityTwoSided = problemTwoSided.getInequalityConstraint()
assert inequalityTwoSided.getOutputDimension() == 2
ott.assert_almost_equal(inequalityTwoSided([1.0, 0.0]), [1.0, 1.5])

# a problem can mix equality and inequality rows
mixedMatrix = ot.Matrix([[1.0, 1.0], [1.0, -1.0]])
mixedBounds = ot.Interval([2.5, -1e30], [2.5, 1.0], [True, False], [True, True])
problemMixed = otexp.QuadraticProblem(Q, c, ot.Interval(), mixedMatrix, mixedBounds)
assert problemMixed.hasEqualityConstraint()
assert problemMixed.hasInequalityConstraint()
equalityMixed = problemMixed.getEqualityConstraint()
inequalityMixed = problemMixed.getInequalityConstraint()
assert equalityMixed.getInputDimension() == 2
assert equalityMixed.getOutputDimension() == 1
assert inequalityMixed.getInputDimension() == 2
assert inequalityMixed.getOutputDimension() == 1
ott.assert_almost_equal(equalityMixed([0.75, 1.75]), [0.0])
ott.assert_almost_equal(inequalityMixed([0.75, 1.75]), [2.0])

# generic algorithms enforce the derived constraints
algoCobyla = ot.Cobyla(problemC)
algoCobyla.setStartingPoint([0.0, 0.0])
algoCobyla.run()
pointCobyla = algoCobyla.getResult().getOptimalPoint()
ott.assert_almost_equal([pointCobyla[0] + pointCobyla[1]], [2.5], 1e-3)

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
    otexp.QuadraticProblem.FromLeastSquares(ot.Matrix(2, 1), [1.0, 2.0], ot.Interval(2))
with ott.assert_raises(ValueError):
    problemC.setLinearConstraint(ot.Matrix(2, 1), ot.Interval(2))
with ott.assert_raises(ValueError):
    problemC.setLinearConstraint(ot.Matrix(1, 3), ot.Interval(1))

# save/load round trip
if HAVE_LIBXML2:
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
