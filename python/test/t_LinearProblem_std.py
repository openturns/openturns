#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

objective = ot.SymbolicFunction(["x0", "x1"], ["1.1*x0 + x1"])
ineq = ot.SymbolicFunction(["x0", "x1"], ["x0 + 2*x1 - 5", "3*x0 + 2*x1 - 6"])
eq = ot.SymbolicFunction(["x0", "x1"], ["7*x0 + 9*x1 - 50"])
problem = ot.OptimizationProblem(objective)
problem.setInequalityConstraint(ineq)
problem.setEqualityConstraint(eq)
bounds = ot.Interval([0.0, 1.0], [4.0, 1e30])
problem.setBounds(bounds)
problem.setVariablesType(
    [
        ot.OptimizationProblemImplementation.CONTINUOUS,
        ot.OptimizationProblemImplementation.INTEGER,
    ]
)
location = [0.0] * 2
linearProblem = otexp.LinearProblem.Linearize(problem, location)
print(linearProblem)
cost = linearProblem.getLinearCost()
A = linearProblem.getLinearConstraintCoefficients()
LU = linearProblem.getLinearConstraintBounds()
assert cost == [1.1, 1.0]
ott.assert_almost_equal(A, ot.Matrix([[1, 2], [3, 2], [7, 9]]))
assert LU.getLowerBound() == [5.0, 6.0, 50.0]
assert LU.getUpperBound()[2] == 50.0
assert problem.getVariablesType()[1] == ot.OptimizationProblemImplementation.INTEGER
assert problem.getBounds().getLowerBound() == [0.0, 1.0]

# equality and inequality constraints derived from the linear coefficients:
# x0 + x1 = 2.5 and x0 - x1 <= 1
mixedA = ot.Matrix([[1.0, 1.0], [1.0, -1.0]])
mixedLU = ot.Interval([2.5, -1e30], [2.5, 1.0], [True, False], [True, True])
mixedProblem = otexp.LinearProblem(
    [1.0, 1.0], ot.Interval([0.0, 0.0], [10.0, 10.0]), mixedA, mixedLU
)
assert mixedProblem.hasEqualityConstraint()
assert mixedProblem.hasInequalityConstraint()
equalityMixed = mixedProblem.getEqualityConstraint()
inequalityMixed = mixedProblem.getInequalityConstraint()
assert isinstance(
    equalityMixed.getEvaluation().getImplementation(), ot.LinearEvaluation
)
assert isinstance(
    inequalityMixed.getEvaluation().getImplementation(), ot.LinearEvaluation
)
assert equalityMixed.getInputDimension() == 2
assert equalityMixed.getOutputDimension() == 1
assert inequalityMixed.getInputDimension() == 2
assert inequalityMixed.getOutputDimension() == 1
ott.assert_almost_equal(equalityMixed([0.75, 1.75]), [0.0])
ott.assert_almost_equal(equalityMixed([0.0, 0.0]), [-2.5])
ott.assert_almost_equal(
    equalityMixed.gradient([0.0, 0.0]), ot.Matrix([[1.0], [1.0]])
)
ott.assert_almost_equal(inequalityMixed([0.0, 0.0]), [1.0])
ott.assert_almost_equal(inequalityMixed([1.0, 1.5]), [1.5])
ott.assert_almost_equal(
    inequalityMixed.gradient([0.0, 0.0]), ot.Matrix([[-1.0], [1.0]])
)

# the cost dimension cannot change while linear constraints are stored
with ott.assert_raises(ValueError):
    mixedProblem.setLinearCost([1.0])

# a linear constraint function replaces the rows of its kind and preserves the
# other kind: 2 x0 - x1 = 1
mixedProblem.setEqualityConstraint(
    ot.LinearFunction([0.0, 0.0], [-1.0], ot.Matrix([[2.0, -1.0]]))
)
assert mixedProblem.hasEqualityConstraint()
assert mixedProblem.hasInequalityConstraint()
ott.assert_almost_equal(
    mixedProblem.getEqualityConstraint()([0.0, 0.0]), [-1.0]
)
ott.assert_almost_equal(
    mixedProblem.getEqualityConstraint()([1.0, 1.0]), [0.0]
)
ott.assert_almost_equal(mixedProblem.getInequalityConstraint()([0.0, 0.0]), [1.0])

# a generic Function known to be linear is accepted through isLinear()
mixedProblem.setInequalityConstraint(
    ot.SymbolicFunction(["x0", "x1"], ["1.0 - x0 + x1"])
)
assert mixedProblem.hasEqualityConstraint()
assert mixedProblem.hasInequalityConstraint()
ott.assert_almost_equal(mixedProblem.getInequalityConstraint()([0.0, 0.0]), [1.0])
ott.assert_almost_equal(mixedProblem.getInequalityConstraint()([1.0, 1.5]), [1.5])

# an evaluation-only wrapper around a linear evaluation keeps its coefficients:
# the finite-difference gradient misses them when the constant term is large
largeOffsetProblem = otexp.LinearProblem(
    [1.0], ot.Interval([-1e30], [1e30]), ot.Matrix(0, 1), ot.Interval()
)
largeOffsetProblem.setInequalityConstraint(
    ot.Function(
        ot.LinearFunction([0.0], [1.0e12], ot.Matrix([[1.0]]))
        .getEvaluation()
        .getImplementation()
    )
)
largeOffsetInequality = largeOffsetProblem.getInequalityConstraint()
ott.assert_almost_equal(largeOffsetInequality([0.0]), [1.0e12])
ott.assert_almost_equal(largeOffsetInequality([-2.0e12]), [-1.0e12])
ott.assert_almost_equal(
    largeOffsetInequality.gradient([0.0]), ot.Matrix([[1.0]])
)

# nonlinear functions and input dimension mismatches are rejected
for setter in (
    mixedProblem.setEqualityConstraint,
    mixedProblem.setInequalityConstraint,
):
    with ott.assert_raises(TypeError):
        setter(ot.SymbolicFunction(["x0", "x1"], ["x0^2 + x1"]))
    with ott.assert_raises(ValueError):
        setter(ot.LinearFunction([0.0], [0.0], ot.Matrix([[1.0]])))

# an empty function clears the corresponding constraints
mixedProblem.setInequalityConstraint(ot.Function())
assert not mixedProblem.hasInequalityConstraint()
assert mixedProblem.hasEqualityConstraint()
mixedProblem.setEqualityConstraint(ot.Function())
assert not mixedProblem.hasEqualityConstraint()

# constraints set through functions are mapped to the HiGHS model: minimize x0
# subject to x0 + x1 = 2.5, x0 - x1 <= 1 and 0 <= x_i <= 10
functionProblem = otexp.LinearProblem(
    [1.0, 0.0], ot.Interval([0.0, 0.0], [10.0, 10.0]), ot.Matrix(0, 2), ot.Interval()
)
functionProblem.setEqualityConstraint(
    ot.LinearFunction([0.0, 0.0], [-2.5], ot.Matrix([[1.0, 1.0]]))
)
functionProblem.setInequalityConstraint(
    ot.LinearFunction([0.0, 0.0], [1.0], ot.Matrix([[-1.0, 1.0]]))
)
if ot.PlatformInfo.HasFeature("highs"):
    algoFunctions = otexp.HiGHS(functionProblem)
    algoFunctions.run()
    ott.assert_almost_equal(algoFunctions.getResult().getOptimalPoint(), [0.0, 2.5])

# the sparse constraint matrix gives the same problem
sparseA = otexp.SparseMatrix.FromDenseMatrix(A)
assert sparseA.getNbNonZeros() == 6, "non zeros"
sparseProblem = otexp.LinearProblem(cost, bounds, sparseA, LU)
ott.assert_almost_equal(sparseProblem.getLinearConstraintCoefficients(), A)
ott.assert_almost_equal(
    sparseProblem.getSparseLinearConstraintCoefficients().asDenseMatrix(), A
)
ott.assert_almost_equal(sparseProblem.getLinearCost(), cost)
assert (
    sparseProblem.getSparseLinearConstraintCoefficients().getColumnPointer()
    == sparseA.getColumnPointer()
)
assert (
    sparseProblem.getSparseLinearConstraintCoefficients().getRowIndices()
    == sparseA.getRowIndices()
)
ott.assert_almost_equal(
    sparseProblem.getSparseLinearConstraintCoefficients().getValues(),
    sparseA.getValues(),
)

# the linearization result and the dense constructor agree in sparse form too
ott.assert_almost_equal(
    linearProblem.getSparseLinearConstraintCoefficients().asDenseMatrix(), A
)

# the dense constructor skips the zero coefficients once
sparseZeroProblem = otexp.LinearProblem(
    [1.0, 1.0], ot.Interval(2), ot.Matrix([[1.0, 0.0], [0.0, 2.0]]), ot.Interval(2)
)
assert sparseZeroProblem.getSparseLinearConstraintCoefficients().getNbNonZeros() == 2

# SparseMatrix is exposed by the experimental module, the stable wrapper does
# not expose the sparse accessor
assert not hasattr(ot.OptimizationProblem, "getSparseLinearConstraintCoefficients")

# the sparse constraint setter
sparseProblem.setLinearConstraint(sparseA, LU)
ott.assert_almost_equal(sparseProblem.getLinearConstraintCoefficients(), A)
with ott.assert_raises(ValueError):
    sparseProblem.setLinearConstraint(otexp.SparseMatrix(3, 1), ot.Interval(3))
with ott.assert_raises(ValueError):
    sparseProblem.setLinearConstraint(otexp.SparseMatrix(1, 2), ot.Interval(2))

# the dense constraint setter accepts a matrix without zero
sparseProblem.setLinearConstraint(A, LU)
ott.assert_almost_equal(
    sparseProblem.getSparseLinearConstraintCoefficients().asDenseMatrix(), A
)

# the equality and inequality constraint functions are read from a sparse
# problem row by row, without densifying its coefficients
mixedSparse = otexp.LinearProblem(
    [1.0, 1.0], ot.Interval([0.0, 0.0], [10.0, 10.0]),
    otexp.SparseMatrix.FromDenseMatrix(mixedA), mixedLU,
)
assert mixedSparse.hasEqualityConstraint()
assert mixedSparse.hasInequalityConstraint()
ott.assert_almost_equal(mixedSparse.getEqualityConstraint()([0.0, 0.0]), [-2.5])
ott.assert_almost_equal(mixedSparse.getEqualityConstraint()([0.75, 1.75]), [0.0])
ott.assert_almost_equal(mixedSparse.getInequalityConstraint()([0.0, 0.0]), [1.0])
ott.assert_almost_equal(mixedSparse.getInequalityConstraint()([1.0, 1.5]), [1.5])

# replacing a constraint kind keeps the sparse storage and the other kind
mixedSparse.setEqualityConstraint(
    ot.LinearFunction([0.0, 0.0], [-1.0], ot.Matrix([[2.0, -1.0]]))
)
assert mixedSparse.hasEqualityConstraint()
assert mixedSparse.hasInequalityConstraint()
ott.assert_almost_equal(
    mixedSparse.getSparseLinearConstraintCoefficients().asDenseMatrix(),
    ot.Matrix([[1.0, -1.0], [2.0, -1.0]]),
)
ott.assert_almost_equal(mixedSparse.getEqualityConstraint()([1.0, 1.0]), [0.0])
mixedSparse.setInequalityConstraint(
    ot.LinearFunction([0.0, 0.0], [1.0], ot.Matrix([[-1.0, 1.0]]))
)
ott.assert_almost_equal(
    mixedSparse.getSparseLinearConstraintCoefficients().asDenseMatrix(),
    ot.Matrix([[2.0, -1.0], [-1.0, 1.0]]),
)
ott.assert_almost_equal(mixedSparse.getInequalityConstraint()([0.0, 0.0]), [1.0])

# dimension checks on the sparse constructor
with ott.assert_raises(ValueError):
    otexp.LinearProblem(cost, bounds, otexp.SparseMatrix(3, 3), LU)
with ott.assert_raises(ValueError):
    otexp.LinearProblem(cost, bounds, otexp.SparseMatrix(2, 2), LU)

# persistence
if ot.PlatformInfo.HasFeature("libxml2"):
    import os

    fileName = "linear_problem_study.xml"
    study = ot.Study()
    study.setStorageManager(ot.XMLStorageManager(fileName))
    study.add("problem", sparseProblem)
    study.save()
    study = ot.Study()
    study.setStorageManager(ot.XMLStorageManager(fileName))
    study.load()
    loadedProblem = otexp.LinearProblem()
    study.fillObject("problem", loadedProblem)
    os.remove(fileName)
    ott.assert_almost_equal(loadedProblem.getLinearCost(), cost)
    ott.assert_almost_equal(loadedProblem.getLinearConstraintCoefficients(), A)
    ott.assert_almost_equal(
        loadedProblem.getLinearConstraintBounds().getLowerBound(),
        LU.getLowerBound(),
    )
    ott.assert_almost_equal(
        loadedProblem.getLinearConstraintBounds().getUpperBound(),
        LU.getUpperBound(),
    )
