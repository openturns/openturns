#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

RTOL = 1.0e-12
ATOL = 1.0e-12


def hessian_point(value):
    """Flatten a hessian (SymmetricTensor) into a Point via full indexing."""
    nb_sheets = value.getNbSheets()
    dim = value.getNbRows()
    return ot.Point(
        [value[i, j, k] for k in range(nb_sheets) for i in range(dim) for j in range(dim)]
    )


def expected_hessian_point(sheets):
    """Flatten a list of nested sheets into the expected hessian Point."""
    return ot.Point(
        [
            value
            for sheet in sheets
            for row in sheet
            for value in row
        ]
    )


# Product
# First, build a function from R^3->R
inVar = ["x0", "x1", "x2"]
formula = ["x0^2 + 2 * x1 * x2 + 3 * x2"]
f1 = ot.SymbolicFunction(inVar, formula)
# Second, build a function from R^3->R^2
formula = ["x2 - x0 + x1"]
formula.append("x0 + x1 * x0 + x2")
f2 = ot.SymbolicFunction(inVar, formula)
# Third, build the product function
myFunction = f1 * f2
inPoint = ot.Point([1.2, 2.3, 3.4])
ott.assert_almost_equal(myFunction(inPoint), ot.Point([122.76, 200.7808]), RTOL, ATOL)
ott.assert_almost_equal(
    myFunction.gradient(inPoint),
    ot.Matrix([[-16.48, 107.688], [57.88, 82.784], [61.48, 83.216]]),
    RTOL,
    ATOL,
)
ott.assert_almost_equal(
    hessian_point(myFunction.hessian(inPoint)),
    expected_hessian_point(
        [
            [[4.2, -4.4, -5.2], [-4.4, 13.6, 23.4], [-5.2, 23.4, 15.2]],
            [[30.56, 52.6, 27.48], [52.6, 16.32, 30.64], [27.48, 30.64, 15.2]],
        ]
    ),
    RTOL,
    ATOL,
)

# Product between a Function and a FunctionImplementation (any combination)
# Both must share the input dimension and at least one must be 1D output
pf1 = ot.SymbolicFunction(["x"], ["x^2 + 2 * x"])
pf2 = ot.SymbolicFunction(["x"], ["3 * x - 1"])
pimpl1 = pf1.getImplementation()
pimpl2 = pf2.getImplementation()
pPoint = ot.Point([2.0])
# reference value: (x^2 + 2x) * (3x - 1) at 2 = (4 + 4) * (6 - 1) = 40
pRef = pf1 * pf2
for label, prod in {
    "function * function": pf1 * pf2,
    "function * implementation": pf1 * pimpl2,
    "implementation * function": pimpl1 * pf2,
    "implementation * implementation": pimpl1 * pimpl2,
}.items():
    ott.assert_almost_equal(prod(pPoint), pRef(pPoint), RTOL, ATOL)
    ott.assert_almost_equal(prod.gradient(pPoint), pRef.gradient(pPoint), RTOL, ATOL)
    ott.assert_almost_equal(
        hessian_point(prod.hessian(pPoint)),
        hessian_point(pRef.hessian(pPoint)),
        RTOL,
        ATOL,
    )
    print("product {}: ok".format(label))
# The four combinations above must produce the same product value
ott.assert_almost_equal((pf1 * pimpl2)(pPoint), [40.0], RTOL, ATOL)
ott.assert_almost_equal((pimpl1 * pf2)(pPoint), [40.0], RTOL, ATOL)
ott.assert_almost_equal((pimpl1 * pimpl2)(pPoint), [40.0], RTOL, ATOL)

# Product enforces the dimension requirements: same input dimension and at
# least one 1D output dimension
highDim1 = ot.SymbolicFunction(["x", "y"], ["x + y", "x - y"])
highDim2 = ot.SymbolicFunction(["x", "y"], ["x", "y"])
with ott.assert_raises(TypeError):
    highDim1 * highDim2  # both outputs are 2D
with ott.assert_raises(TypeError):
    highDim1 * highDim1.getImplementation()
with ott.assert_raises(TypeError):
    highDim1.getImplementation() * highDim2
# differing input dimensions are rejected
differentDim = ot.SymbolicFunction(["x", "y", "z"], ["x + y + z"])
with ott.assert_raises(TypeError):
    pf1 * differentDim

# Division between a Function and a FunctionImplementation (any combination)
# Both must share the input dimension and the right operand must be 1D output
df1 = ot.SymbolicFunction(["x"], ["x^2 + 2 * x"])
df2 = ot.SymbolicFunction(["x"], ["x + 1"])
dimpl1 = df1.getImplementation()
dimpl2 = df2.getImplementation()
dPoint = ot.Point([3.0])
# reference value: (x^2 + 2x) / (x + 1) at 3 = (9 + 6) / 4 = 3.75
dRef = df1 / df2
for label, div in {
    "function / function": df1 / df2,
    "function / implementation": df1 / dimpl2,
    "implementation / function": dimpl1 / df2,
    "implementation / implementation": dimpl1 / dimpl2,
}.items():
    ott.assert_almost_equal(div(dPoint), dRef(dPoint), RTOL, ATOL)
    ott.assert_almost_equal(div.gradient(dPoint), dRef.gradient(dPoint), RTOL, ATOL)
    ott.assert_almost_equal(
        hessian_point(div.hessian(dPoint)),
        hessian_point(dRef.hessian(dPoint)),
        RTOL,
        ATOL,
    )
    print("division {}: ok".format(label))
# The four combinations above must produce the same division value
ott.assert_almost_equal((df1 / dimpl2)(dPoint), [3.75], RTOL, ATOL)
ott.assert_almost_equal((dimpl1 / df2)(dPoint), [3.75], RTOL, ATOL)
ott.assert_almost_equal((dimpl1 / dimpl2)(dPoint), [3.75], RTOL, ATOL)

# Division with multi-output left operand (any output dimension) and 1D right
# f: R^2->R^2, g: R^2->R^1
mf1 = ot.SymbolicFunction(["x", "y"], ["x + y", "x * y"])
mg1 = ot.SymbolicFunction(["x", "y"], ["x - y"])
mRef = mf1 / mg1
mPoint = ot.Point([3.0, 1.0])
# (3+1)/(3-1)=2, (3*1)/(3-1)=1.5
ott.assert_almost_equal(mRef(mPoint), [2.0, 1.5], RTOL, ATOL)
ott.assert_almost_equal(
    mRef.gradient(mPoint),
    ot.Matrix([[-0.5, -0.25], [1.5, 2.25]]),
    RTOL,
    ATOL,
)
for label, div in {
    "function / function": mf1 / mg1,
    "function / implementation": mf1 / mg1.getImplementation(),
    "implementation / function": mf1.getImplementation() / mg1,
    "implementation / implementation": mf1.getImplementation() / mg1.getImplementation(),
}.items():
    ott.assert_almost_equal(div(mPoint), mRef(mPoint), RTOL, ATOL)
    print("division multi-output {}: ok".format(label))

# 3D output / 1D output
mf3 = ot.SymbolicFunction(["x"], ["x^2", "2 * x", "x^3"])
mg3 = ot.SymbolicFunction(["x"], ["x + 1"])
# at x=2: (4/3, 4/3, 8/3)
ott.assert_almost_equal((mf3 / mg3)([2.0]), [4.0 / 3, 4.0 / 3, 8.0 / 3], RTOL, ATOL)

# Division enforces the dimension requirements: same input dimension and
# right operand must have 1D output
with ott.assert_raises(TypeError):
    highDim1 / highDim2  # both outputs are 2D
with ott.assert_raises(TypeError):
    highDim1 / highDim1.getImplementation()
with ott.assert_raises(TypeError):
    highDim1.getImplementation() / highDim2
with ott.assert_raises(TypeError):
    pf1 / differentDim  # differing input dimensions

# Sum/difference of two functions from R^3->R^2
formula = ["x0 + 2 * x1 * x2 + 3 * x2", "x2 - x0 + x1 * x0"]
f1 = ot.SymbolicFunction(inVar, formula)
formula = ["x0 + x1 + x2", "-2 * x0 + 3 * x2 * x1 - x1"]
f2 = ot.SymbolicFunction(inVar, formula)
mySum = f1 + f2
ott.assert_almost_equal(mySum(inPoint), [33.94, 23.72], RTOL, ATOL)
ott.assert_almost_equal(
    mySum.gradient(inPoint), ot.Matrix([[2.0, -0.7], [7.8, 10.4], [8.6, 7.9]]), RTOL, ATOL
)
ott.assert_almost_equal(
    hessian_point(mySum.hessian(inPoint)),
    expected_hessian_point(
        [
            [[0.0, 0.0, 0.0], [0.0, 0.0, 2.0], [0.0, 2.0, 0.0]],
            [[0.0, 1.0, 0.0], [1.0, 0.0, 3.0], [0.0, 3.0, 0.0]],
        ]
    ),
    RTOL,
    ATOL,
)
myDiff = f1 - f2
ott.assert_almost_equal(myDiff(inPoint), [20.14, -13.8], RTOL, ATOL)
ott.assert_almost_equal(
    myDiff.gradient(inPoint), ot.Matrix([[0.0, 3.3], [5.8, -8.0], [6.6, -5.9]]), RTOL, ATOL
)
ott.assert_almost_equal(
    hessian_point(myDiff.hessian(inPoint)),
    expected_hessian_point(
        [
            [[0.0, 0.0, 0.0], [0.0, 0.0, 2.0], [0.0, 2.0, 0.0]],
            [[0.0, 1.0, 0.0], [1.0, 0.0, -3.0], [0.0, -3.0, 0.0]],
        ]
    ),
    RTOL,
    ATOL,
)

# Negation
myNeg = -f1
ott.assert_almost_equal(myNeg(inPoint), [-27.04, -4.96], RTOL, ATOL)
ott.assert_almost_equal(
    myNeg.gradient(inPoint), ot.Matrix([[-1.0, -1.3], [-6.8, -1.2], [-7.6, -1.0]]), RTOL, ATOL
)
ott.assert_almost_equal(
    hessian_point(myNeg.hessian(inPoint)),
    expected_hessian_point(
        [
            [[0.0, 0.0, 0.0], [0.0, 0.0, -2.0], [0.0, -2.0, 0.0]],
            [[0.0, -1.0, 0.0], [-1.0, 0.0, 0.0], [0.0, 0.0, 0.0]],
        ]
    ),
    RTOL,
    ATOL,
)

# Scalar multiplication: a * f, f * a
sf = ot.SymbolicFunction(["x", "y"], ["x + y", "x * y"])
sp = ot.Point([3.0, 1.0])
# 3.0 * f at (3,1): 3*(4,3) = (12,9)
ott.assert_almost_equal((3.0 * sf)(sp), [12.0, 9.0], RTOL, ATOL)
ott.assert_almost_equal((sf * 3.0)(sp), [12.0, 9.0], RTOL, ATOL)
# gradient of 3*f is 3 times the gradient of f
ott.assert_almost_equal(
    (3.0 * sf).gradient(sp), 3.0 * sf.gradient(sp), RTOL, ATOL
)
# f * 0 gives zero function
ott.assert_almost_equal((sf * 0.0)(sp), [0.0, 0.0], RTOL, ATOL)
# f * 1 returns the function unchanged (values, gradient, hessian)
ott.assert_almost_equal((sf * 1.0)(sp), sf(sp), RTOL, ATOL)
ott.assert_almost_equal((sf * 1.0).gradient(sp), sf.gradient(sp), RTOL, ATOL)
# f * (-1) is negation
ott.assert_almost_equal((sf * (-1.0))(sp), -sf(sp), RTOL, ATOL)
ott.assert_almost_equal((sf * (-1.0)).gradient(sp), -sf.gradient(sp), RTOL, ATOL)
# scalar * FunctionImpl and FunctionImpl * scalar
implResult = sf.getImplementation() * 2.0
ott.assert_almost_equal(implResult(sp), [8.0, 6.0], RTOL, ATOL)
print("scalar multiplication: ok")

# LinearCombinationFunction shortcuts: merging, scaling and flattening
f1 = ot.SymbolicFunction(["x"], ["x^2"])
f2 = ot.SymbolicFunction(["x"], ["2 * x"])
f3 = ot.SymbolicFunction(["x"], ["x^3"])
inPoint = ot.Point([2.0])
# lcf = f1 + 2 * f2
lcf = ot.LinearCombinationFunction([f1, f2], [1.0, 2.0])
# Merging a plain function adds a new atom with a unit weight
sumWithAtom = lcf + f3
# at 2: f1=4, f2=4, f3=8 -> 4 + 2*4 + 8 = 20
ott.assert_almost_equal(sumWithAtom(inPoint), [20.0], RTOL, ATOL)
# Merging two linear combinations sums the weights of the common atoms
lcf2 = ot.LinearCombinationFunction([f1, f3], [5.0, 7.0])
merged = lcf + lcf2
# at 2: f1 weight 1+5=6, f2 weight 2, f3 weight 7 -> 6*4 + 2*4 + 7*8 = 88
ott.assert_almost_equal(merged(inPoint), [88.0], RTOL, ATOL)
# Unary minus of a linear combination keeps the same atoms with opposite coefficients
negated = -lcf
# at 2: -(1*4 + 2*4) = -12
ott.assert_almost_equal(negated(inPoint), [-12.0], RTOL, ATOL)
# The difference of two linear combinations cancels the common atoms
zero = lcf - lcf
ott.assert_almost_equal(zero(inPoint), [0.0], RTOL, ATOL)
# Flattening of nested linear combinations
inner = ot.LinearCombinationFunction([f2, f3], [3.0, 4.0])
outer = ot.LinearCombinationFunction([f1, inner], [1.0, 2.0])
flattened = outer + f2
# at 2: f1 weight 1, f2 weight 2*3+1=7, f3 weight 2*4=8 -> 1*4 + 7*4 + 8*8 = 96
ott.assert_almost_equal(flattened(inPoint), [96.0], RTOL, ATOL)

# Operations between different derived implementations (Symbolic + Constant)
const = ot.ConstantFunction(1, ot.Point([2.0]))
# at 2: x^2 + 2 = 6
ott.assert_almost_equal((f1 + const)(inPoint), [6.0], RTOL, ATOL)
# at 2: 2 + 4 + 2*4 = 14
ott.assert_almost_equal((const + lcf)(inPoint), [14.0], RTOL, ATOL)
# FunctionImplementation-level operations must merge the same way
implSum = f1.getImplementation() + f2.getImplementation()
ott.assert_almost_equal(implSum(inPoint), [8.0], RTOL, ATOL)
implNeg = -(lcf.getImplementation())
ott.assert_almost_equal(implNeg(inPoint), [-12.0], RTOL, ATOL)
# Adding a linear combination to a plain function on the right-hand side
# at 2: 4 + (4 + 2*4) = 16
ott.assert_almost_equal((f1 + lcf)(inPoint), [16.0], RTOL, ATOL)

# The LinearCombinationFunction-MergeDuplicates key controls whether the
# parameter constructor merges its atoms; the operators always merge.
# Default: no merging in the constructor (duplicates and nesting are kept)
lc_dup = ot.LinearCombinationFunction([f1, f1], [1.0, 2.0])
ott.assert_almost_equal(lc_dup(inPoint), [12.0], RTOL, ATOL)
# Nested linear combination kept as-is by default
inner = ot.LinearCombinationFunction([f2], [2.0])
lc_nested = ot.LinearCombinationFunction([f1, inner], [1.0, 1.0])
ott.assert_almost_equal(lc_nested(inPoint), [12.0], RTOL, ATOL)
# With the key set, the constructor merges duplicates and flattens nesting
ot.ResourceMap.SetAsBool("LinearCombinationFunction-MergeDuplicates", True)
lc_merged = ot.LinearCombinationFunction([f1, f1], [1.0, 2.0])
# at 2: f1 weight 1+2=3 -> 3*4 = 12
ott.assert_almost_equal(lc_merged(inPoint), [12.0], RTOL, ATOL)
# Operators still merge regardless of the key
# at 2: merged (3*f1) + lcf (f1 + 2*f2) -> 4*f1 + 2*f2 -> 4*4 + 2*4 = 24
ott.assert_almost_equal((lc_merged + lcf)(inPoint), [24.0], RTOL, ATOL)
# Restore the default value of the key
ot.ResourceMap.SetAsBool("LinearCombinationFunction-MergeDuplicates", False)
