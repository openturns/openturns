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
