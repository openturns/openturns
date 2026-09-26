#!/usr/bin/env python
"""
Test case 2: C++ SparseExpansion LARS, checked against the defining
conditions and compared with a second, independent implementation.

Uses the same Ishigami sparse problem as test case 1 (100 samples, max degree 3).
Compares selection paths, active sets, and Sobol indices.

The two kinds of check play different roles and must not be confused.

* A defining condition decides on its own. At every state the LARS
  recursion records, the coefficients are the minimizer of the weighted
  least squares problem restricted to the active set, hence
  Phi_A^T W (y - Phi_A c) = 0 with the weights the caller asked for. That
  is checked here, on the states the C++ recorded, with no second
  implementation involved. It is the instrument that separates a correct
  weighted fit from one that ignored the weights: 1.6e-15 against 1.2e-1.

* The comparison with the Python implementation below is consistency
  evidence, not proof. A divergence means a defect in one of the two, and
  the defining condition above is what tells which; agreement only means
  that no shared defect has been found. The Python implementation is used
  because it makes the comparison cheap and legible, not because it is
  authoritative.
"""

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott
from math import sqrt

ot.TESTPREAMBLE()


class PythonLARS:
    """Second implementation of the LARS recursion, for comparison only.

    Implements the same recursion as SparseExpansion::runLARS with
    quadrature weights w: the correlations are c = Phi^T W (y - mu), the
    active Gram matrix is G_A = Phi_A^T W Phi_A, and the direction
    correlations are d = Phi^T W u. The default weights are the uniform
    ones, 1/n, so it also covers the unweighted case.
    """

    def __init__(self, input_sample, output_sample, distribution, basis,
                 basisSize, fittingAlgorithm, methodName="SVD", weight=None):
        self.input_sample = input_sample
        self.output_sample = output_sample
        self.distribution = distribution
        self.basis = basis
        self.basisSize = basisSize
        self.fittingAlgorithm = fittingAlgorithm
        self.methodName = methodName
        self.sample_size = input_sample.getSize()
        if weight is None:
            weight = ot.Point(self.sample_size, 1.0 / self.sample_size)
        assert weight.getSize() == self.sample_size
        self.weight = weight
        # Psi = sqrt(W) Phi, so that Psi^T (sqrt(W) v) = Phi^T W v
        self.weightSqrt = ot.Point(self.sample_size)
        for s in range(self.sample_size):
            self.weightSqrt[s] = sqrt(weight[s])

    def run(self):
        transformation = ot.DistributionTransformation(
            self.distribution, self.basis.getMeasure()
        )
        standard_input = transformation(self.input_sample)
        sample_size = standard_input.getSize()
        output_dimension = self.output_sample.getDimension()

        functions = [self.basis.build(i) for i in range(self.basisSize)]
        designProxy = ot.DesignProxy(standard_input, functions)

        self.coefficients_map = {}
        self.selection_history = []
        self.coefficient_history = []

        for output_index in range(output_dimension):
            marginal_output = self.output_sample.getMarginal(output_index)
            marginal_selection = [0]

            leastSquaresMethod = ot.LeastSquaresMethod.Build(
                self.methodName, designProxy, self.weight, marginal_selection
            )
            rhs = marginal_output.asPoint()
            # weighted mean of the marginal output
            weight_sum = 0.0
            weighted_sum = 0.0
            for s in range(sample_size):
                weight_sum += self.weight[s]
                weighted_sum += self.weight[s] * rhs[s]
            marginal_output_mean = weighted_sum / weight_sum
            coefficients = [marginal_output_mean]

            # Current prediction mu
            mu = ot.Point(sample_size, marginal_output_mean)

            # The constant function alone is the first state of the path
            self.selection_history.append([0])
            self.coefficient_history.append(ot.Point([marginal_output_mean]))

            # Full design, raw and weighted by sqrt(w), for the correlations
            fullX = designProxy.computeDesign(range(self.basisSize))
            weightedFullX = ot.Matrix(sample_size, self.basisSize)
            for s in range(sample_size):
                for j in range(self.basisSize):
                    weightedFullX[s, j] = self.weightSqrt[s] * fullX[s, j]

            # The reference runs the whole path, without any cross-validation
            # stopping, and stops on its own criterion: the largest remaining
            # correlation below roundoff, ie a model that has captured
            # everything the design can resolve
            for iteration in range(self.basisSize - 1):
                # Stop if active set would exceed sample size (LS becomes rank-deficient)
                if len(marginal_selection) >= sample_size:
                    break
                # Compute residual
                residual = rhs - mu

                # Weighted correlations c = Phi^T W (y - mu)
                weightedResidual = ot.Point(sample_size)
                for s in range(sample_size):
                    weightedResidual[s] = self.weightSqrt[s] * residual[s]
                correlations = weightedFullX.getImplementation().genVectProd(
                    weightedResidual, True
                )

                # Find max absolute correlation among inactive
                cMax = -1.0
                best_idx = None
                for k in range(self.basisSize):
                    if k in marginal_selection:
                        continue
                    cAbs = abs(correlations[k])
                    if cAbs > cMax:
                        cMax = cAbs
                        best_idx = k

                if best_idx is None or cMax < 1e-14:
                    break

                # Update active set
                marginal_selection.append(best_idx)
                leastSquaresMethod.update([best_idx], marginal_selection[:-1], [])

                # Build sign vector for active set
                sC = ot.Point(len(marginal_selection))
                for j in range(len(marginal_selection)):
                    sC[j] = 1.0 if correlations[marginal_selection[j]] >= 0.0 else -1.0

                # Solve the weighted normal equation: G_A g = sC with
                # G_A = Phi_A^T W Phi_A
                psiAk = ot.Matrix(sample_size, len(marginal_selection))
                psiAkW = ot.Matrix(sample_size, len(marginal_selection))
                for s in range(sample_size):
                    for j in range(len(marginal_selection)):
                        psiAk[s, j] = fullX[s, marginal_selection[j]]
                        psiAkW[s, j] = self.weightSqrt[s] * fullX[s, marginal_selection[j]]
                GA = psiAkW.transpose() * psiAkW
                g = GA.solveLinearSystem(sC)

                cNorm = 1.0 / sqrt(sC.dot(g))
                u = psiAk * (cNorm * g)

                # Direction correlations d = Phi^T W u
                weightedU = ot.Point(sample_size)
                for s in range(sample_size):
                    weightedU[s] = self.weightSqrt[s] * u[s]
                dFull = weightedFullX.getImplementation().genVectProd(weightedU, True)
                d = ot.Point(self.basisSize)
                for k in range(self.basisSize):
                    if k not in marginal_selection:
                        d[k] = dFull[k]

                # Compute step size
                step = cMax / cNorm
                for k in range(self.basisSize):
                    if k in marginal_selection:
                        continue
                    cI = correlations[k]
                    if abs(cNorm - d[k]) > 1e-15:
                        lhs = (cMax - cI) / (cNorm - d[k])
                        if lhs > 0.0:
                            step = min(step, lhs)
                    if abs(cNorm + d[k]) > 1e-15:
                        rhs_step = (cMax + cI) / (cNorm + d[k])
                        if rhs_step > 0.0:
                            step = min(step, rhs_step)

                # Update prediction
                mu = mu + step * u

                # Solve LS for coefficients
                coefficients = leastSquaresMethod.solve(rhs)
                self.coefficient_history.append(ot.Point(coefficients))
                self.selection_history.append(marginal_selection.copy())

            for j in range(len(marginal_selection)):
                idx = marginal_selection[j]
                if idx not in self.coefficients_map:
                    self.coefficients_map[idx] = ot.Point(output_dimension, 0.0)
                self.coefficients_map[idx][output_index] = coefficients[j]

        sorted_indices = sorted(self.coefficients_map.keys())
        self.active_indices = ot.Indices(sorted_indices)
        coefficient_list = [self.coefficients_map[idx] for idx in sorted_indices]
        self.coefficient_sample = ot.Sample(coefficient_list)
        self.final_functions = [functions[idx] for idx in sorted_indices]
        self.transformation = transformation
        self.result = ot.FunctionalChaosResult(
            self.input_sample, self.output_sample, self.distribution,
            self.transformation, self.transformation.inverse(),
            self.basis, self.active_indices, self.coefficient_sample,
            self.final_functions,
        )


def designMatrix(input_sample, basis, basis_size, distribution):
    """Rows of the basis evaluated on the standardized input sample."""
    transformation = ot.DistributionTransformation(
        distribution, basis.getMeasure()
    )
    proxy = ot.DesignProxy(
        transformation(input_sample),
        [basis.build(i) for i in range(basis_size)],
    )
    return ot.Matrix(proxy.computeDesign(ot.Indices(range(basis_size))))


def checkStatesAreMinimizers(label, phi, output, weight, indices_history,
                             coefficients_history, tolerance=1.0e-12):
    """At each recorded state, the coefficients minimize the weighted problem.

    The LARS recursion records, at every step, the least squares solution
    on the active set, hence Phi_A^T W (y - Phi_A c) = 0 with the weights
    the caller asked for. This is the defining condition of that state, so
    it holds whatever produced the state: a solver that dropped or
    misread the weights fails it by orders of magnitude, while the same
    solver evaluated with the weights it was given always passes.
    """
    size = output.getSize()
    values = output.asPoint()
    worst = 0.0
    for k in range(len(indices_history)):
        active = list(indices_history[k])
        coefficients = ot.Point(coefficients_history[k])
        scale = 0.0
        for i in range(size):
            residual = values[i] - sum(
                phi[i, active[j]] * coefficients[j] for j in range(len(active))
            )
            scale = max(scale, abs(weight[i] * residual))
        for j in range(len(active)):
            accumulator = 0.0
            for i in range(size):
                residual = values[i] - sum(
                    phi[i, active[m]] * coefficients[m]
                    for m in range(len(active))
                )
                accumulator += phi[i, active[j]] * weight[i] * residual
            worst = max(worst, abs(accumulator) / (scale * len(active)))
    print(f"{label}: normal equations residual over "
          f"{len(indices_history)} states = {worst:.3e}")
    assert worst < tolerance, (
        f"{label}: the recorded states are not the minimizers of the weighted "
        f"least squares problem, residual={worst:.3e}"
    )


# --- Ishigami sparse test case (from PR #2987) ---
dimension = 3
a = 7.0
b = 0.1
pi = 3.141592653589793
covTh = (b**2 * pi**8) / 18.0 + (b * pi**4) / 5.0 + (a**2) / 8.0 + 1.0 / 2.0
sob_1_ref = [
    (b * pi**4 / 5.0 + b**2 * pi**8 / 50.0 + 1.0 / 2.0) / covTh,
    (a**2 / 8.0) / covTh,
    0.0,
]
sob_T1_ref = [
    sob_1_ref[0] + 0.0 + (b**2 * pi**8 / 18.0 - b**2 * pi**8 / 50.0) / covTh + 0.0,
    sob_1_ref[1] + 0.0 + 0.0 + 0.0,
    sob_1_ref[2] + (b**2 * pi**8 / 18.0 - b**2 * pi**8 / 50.0) / covTh + 0.0,
]

inputVariables = ["xi1", "xi2", "xi3"]
formula = ["sin(xi1) + 7.0 * (sin(xi2))^2 + 0.1 * xi3^4 * sin(xi1)"]
model = ot.SymbolicFunction(inputVariables, formula)
distribution = ot.JointDistribution([ot.Uniform(-pi, pi)] * dimension)

enumerateFunction = ot.LinearEnumerateFunction(dimension)
productBasis = ot.OrthogonalProductPolynomialFactory(
    [ot.LegendreFactory()] * dimension, enumerateFunction
)

samplingSize = 75
ot.RandomGenerator.SetSeed(0)
inputSample = distribution.getSample(samplingSize)
outputSample = model(inputSample)

degree = 10
basisSize = enumerateFunction.getStrataCumulatedCardinal(degree)
print("basisSize =", basisSize)

fittingAlgorithm = ot.KFold()
# LARS cross-validation error oscillates along the equiangular path, so the
# default 2 consecutive increases stop the selection prematurely (7 active
# functions, poor L2 error). Use 5 consecutive increases instead.
ot.ResourceMap.SetAsUnsignedInteger("SparseExpansion-ConsecutiveIncreases", 5)

# --- C++ SparseExpansion LARS ---
print("=" * 60)
print("C++ SparseExpansion with LARS selection")
print("=" * 60)
algo_cpp = otexp.SparseExpansion(
    inputSample, outputSample, distribution,
    productBasis, basisSize, "QR", fittingAlgorithm
)
algo_cpp.setSelectionMethod("LARS")
algo_cpp.run()
result_cpp = algo_cpp.getResult()

sobol_cpp = ot.FunctionalChaosSobolIndices(result_cpp)
print("Active functions:", list(result_cpp.getIndices()))
print("Number of active functions:", result_cpp.getIndices().getSize())
for i in range(dimension):
    s1 = sobol_cpp.getSobolIndex(i)
    st = sobol_cpp.getSobolTotalIndex(i)
    print(f"  X{i + 1}: S1={s1:.6f} (ref={sob_1_ref[i]:.6f}), ST={st:.6f} (ref={sob_T1_ref[i]:.6f})")

# --- defining condition of the recorded states, uniform weights ---
# Checked on the C++ history alone: at every state it records, the
# coefficients must minimize the uniform least squares problem on the
# active set. No second implementation takes part in this verdict.
cppHistory = result_cpp.getIndicesHistory()
cppCoefficients = result_cpp.getCoefficientsHistory()
uniformWeight = ot.Point(samplingSize, 1.0 / samplingSize)
checkStatesAreMinimizers(
    "C++ LARS, uniform weights",
    designMatrix(inputSample, productBasis, basisSize, distribution),
    outputSample, uniformWeight, cppHistory, cppCoefficients,
)

# --- second implementation, for comparison ---
print()
print("=" * 60)
print("Python LARS, second implementation")
print("=" * 60)
algo_py = PythonLARS(
    inputSample, outputSample, distribution,
    productBasis, basisSize, fittingAlgorithm, "QR"
)
algo_py.run()
result_py = algo_py.result

sobol_py = ot.FunctionalChaosSobolIndices(result_py)
print("Active functions:", list(result_py.getIndices()))
print("Number of active functions:", result_py.getIndices().getSize())
for i in range(dimension):
    s1 = sobol_py.getSobolIndex(i)
    st = sobol_py.getSobolTotalIndex(i)
    print(f"  X{i + 1}: S1={s1:.6f} (ref={sob_1_ref[i]:.6f}), ST={st:.6f} (ref={sob_T1_ref[i]:.6f})")

# --- Compare ---
print()
print("=" * 60)
print("Comparison: C++ LARS vs Python LARS")
print("=" * 60)
cpp_indices = sorted(list(result_cpp.getIndices()))
py_indices = sorted(list(result_py.getIndices()))
print(f"C++ active indices: {cpp_indices}")
print(f"Python active indices: {py_indices}")
print(f"Same active set: {cpp_indices == py_indices}")

for i in range(dimension):
    s1_cpp = sobol_cpp.getSobolIndex(i)
    s1_py = sobol_py.getSobolIndex(i)
    print(f"X{i + 1} S1: C++={s1_cpp:.6f}, Python={s1_py:.6f}, diff={abs(s1_cpp - s1_py):.6e}")

# --- Selection path, uniform weights ---
# Consistency between the two implementations: every state they have in
# common is compared, the active set and the coefficients. The two runs
# stop for different reasons, the C++ one on the cross-validation
# criterion, the Python one when the largest remaining correlation falls
# below roundoff, ie when the model has captured everything the design can
# resolve. Agreement here is evidence, not proof: the defining condition
# checked above is what certifies the states.
compared = min(len(cppHistory), len(algo_py.selection_history))
print(f"C++ recorded {len(cppHistory) - 1} LARS iterations, "
      f"Python {len(algo_py.selection_history) - 1}, comparing {compared - 1}")
assert compared > 5
assert algo_py.selection_history[0] == [0]
for k in range(1, compared):
    assert algo_py.selection_history[k] == list(cppHistory[k]), (
        f"iteration {k}: {algo_py.selection_history[k]} != {list(cppHistory[k])}; "
        f"the defining condition checked above says which one is wrong"
    )
    ott.assert_almost_equal(
        algo_py.coefficient_history[k], cppCoefficients[k], 1.0e-9, 1.0e-9
    )
print(f"Uniform-weight LARS path: {compared - 1} iterations agree")

# --- Quadrature weights on a genuine quadrature design ---
# The design is chosen so that the Gauss product rule integrates every
# product of basis functions exactly: with 7 nodes per direction, the
# Legendre basis of total degree 6 is orthogonal for those weights, so the
# weighted Gram matrix of the full basis is the identity and the weighted
# least squares problem is perfectly conditioned. (A coarser rule, eg 3
# nodes in the first direction with a degree 10 basis, cannot resolve the
# high degree terms: its Gram matrix is singular, and no comparison on such
# a design would mean anything.)
quadratureDegree = 6
quadratureBasisSize = enumerateFunction.getStrataCumulatedCardinal(quadratureDegree)
quadratureInput, quadratureWeight = ot.GaussProductExperiment(
    distribution, (7, 7, 7)
).generateWithWeights()
print(f"quadrature design: size={quadratureInput.getSize()} basisSize={quadratureBasisSize}"
      f" weight ratio={max(quadratureWeight) / min(quadratureWeight):.1f}")
# the weights are the quadrature weights of a density, they sum to 1
ott.assert_almost_equal(sum(quadratureWeight), 1.0, 1.0e-12, 0.0)
quadratureTransformation = ot.DistributionTransformation(
    distribution, productBasis.getMeasure()
)
quadratureProxy = ot.DesignProxy(
    quadratureTransformation(quadratureInput),
    [productBasis.build(i) for i in range(quadratureBasisSize)],
)
quadratureMethod = ot.LeastSquaresMethod.Build(
    "QR", quadratureProxy, quadratureWeight,
    ot.Indices(range(quadratureBasisSize)),
)
quadratureMethod.update([], ot.Indices(range(quadratureBasisSize)), [])
orthogonalDesign = quadratureMethod.computeWeightedDesign()
gramOfBasis = ot.Matrix(orthogonalDesign.transpose() * orthogonalDesign)
for i in range(quadratureBasisSize):
    ott.assert_almost_equal(
        [gramOfBasis[i, j] for j in range(quadratureBasisSize)],
        [1.0 if i == j else 0.0 for j in range(quadratureBasisSize)],
        1.0e-12, 1.0e-12,
    )
print("Weighted Gram matrix of the full basis is the identity")
quadratureOutput = model(quadratureInput)
quadraturePhi = designMatrix(
    quadratureInput, productBasis, quadratureBasisSize, distribution
)
algo_cpp_weighted = otexp.SparseExpansion(
    quadratureInput, quadratureWeight, quadratureOutput, distribution,
    productBasis, quadratureBasisSize, "QR", fittingAlgorithm
)
algo_cpp_weighted.setSelectionMethod("LARS")
algo_cpp_weighted.run()
result_cpp_weighted = algo_cpp_weighted.getResult()
weightedHistory = result_cpp_weighted.getIndicesHistory()
weightedCoefficients = result_cpp_weighted.getCoefficientsHistory()

# --- defining condition of the recorded states, quadrature weights ---
# The instrument that decides: with the weights the caller asked for, the
# coefficients recorded at every state must minimize the weighted least
# squares problem on the active set. Ignoring the weights gives 1.2e-1
# here, so the margin is about eleven orders of magnitude.
checkStatesAreMinimizers(
    "C++ LARS, quadrature weights",
    quadraturePhi, quadratureOutput, quadratureWeight,
    weightedHistory, weightedCoefficients,
)

# --- consistency with the second implementation, quadrature weights ---
algo_py_weighted = PythonLARS(
    quadratureInput, quadratureOutput, distribution,
    productBasis, quadratureBasisSize, fittingAlgorithm, "QR", quadratureWeight
)
algo_py_weighted.run()
weightedCompared = min(len(weightedHistory), len(algo_py_weighted.selection_history))
print(f"C++ recorded {len(weightedHistory) - 1} LARS iterations with quadrature "
      f"weights, Python {len(algo_py_weighted.selection_history) - 1}, "
      f"comparing {weightedCompared - 1}")
assert weightedCompared > 3
for k in range(1, weightedCompared):
    assert algo_py_weighted.selection_history[k] == list(weightedHistory[k]), (
        f"iteration {k}: {algo_py_weighted.selection_history[k]}"
        f" != {list(weightedHistory[k])}; the defining condition checked above "
        f"says which one is wrong"
    )
    ott.assert_almost_equal(
        algo_py_weighted.coefficient_history[k], weightedCoefficients[k], 1.0e-9, 1.0e-9
    )
print(f"Weighted LARS path: {weightedCompared - 1} iterations agree")

# The comparison is sensitive to the weights, so it is not vacuous here: the
# same implementation on the same nodes with uniform weights departs from
# the weighted path almost immediately.
algo_py_uniform_on_quadrature = PythonLARS(
    quadratureInput, quadratureOutput, distribution,
    productBasis, quadratureBasisSize, fittingAlgorithm, "QR"
)
algo_py_uniform_on_quadrature.run()
firstDifference = next(
    (
        k
        for k in range(1, min(len(algo_py_uniform_on_quadrature.selection_history), len(weightedHistory)))
        if algo_py_uniform_on_quadrature.selection_history[k] != list(weightedHistory[k])
    ),
    None,
)
assert firstDifference is not None and firstDifference <= 3, (
    f"the weighted and the uniform paths agree up to iteration {firstDifference}"
)
print(f"The weighted path departs from the uniform one at iteration {firstDifference}")
# The weighted selection must differ from the uniform-weights selection of the
# Monte Carlo design above
assert list(result_cpp_weighted.getIndices()) != cpp_indices
print(f"C++ active indices with quadrature weights: {sorted(result_cpp_weighted.getIndices())}")

# Assert C++ LARS produces reasonable Sobol indices.
# The C++ implementation uses cross-validation stopping, while the Python
# implementation runs the whole path, so the two active sets differ. The
# cross-validated model is a sparse approximation, so the bounds are loose.
for i in range(dimension):
    s1_cpp = sobol_cpp.getSobolIndex(i)
    print(f"X{i + 1} S1: C++={s1_cpp:.6f}, ref={sob_1_ref[i]:.6f}")
    # Loose bounds: the CV-selected sparse model approximates the
    # full-model Sobol indices coarsely by design
    ott.assert_almost_equal(s1_cpp, sob_1_ref[i], 0.5, 0.1)
    st_cpp = sobol_cpp.getSobolTotalIndex(i)
    ott.assert_almost_equal(st_cpp, sob_T1_ref[i], 0.5, 0.1)

# --- quality of the weighted approximation and of its by-products ---
# Absolute criteria, not a comparison: the weights are the quadrature
# weights of a density and sum to 1, so the weighted sum of squares of the
# residuals estimates the L2 error over the distribution, and it can be
# compared with the variance the same way estimates it.
quadratureValues = quadratureOutput.asPoint()
quadratureSize = quadratureInput.getSize()
weightSum = sum(quadratureWeight)
weightedMean = sum(
    quadratureWeight[i] * quadratureValues[i] for i in range(quadratureSize)
) / weightSum
quadratureVariance = sum(
    quadratureWeight[i] * (quadratureValues[i] - weightedMean) ** 2
    for i in range(quadratureSize)
)
weightedResiduals = quadratureValues - result_cpp_weighted.getMetaModel()(
    quadratureInput
).asPoint()
quadratureL2 = sum(
    quadratureWeight[i] * weightedResiduals[i] ** 2 for i in range(quadratureSize)
)
relativeL2 = quadratureL2 / quadratureVariance
print(f"weighted L2 error^2 = {quadratureL2:.6e}, variance = {quadratureVariance:.6f},"
      f" relative = {relativeL2:.4e}")
# the sparse weighted model explains more than 95% of the variance
assert relativeL2 < 0.05

# and its by-products are close to the analytical Ishigami values. The model
# keeps 10 terms out of 84, so the indices are only approximate: the measured
# errors are 0.020 and 0.045 for S1, 0.045 for ST.
sobolWeighted = ot.FunctionalChaosSobolIndices(result_cpp_weighted)
for i in range(dimension):
    s1 = sobolWeighted.getSobolIndex(i)
    st = sobolWeighted.getSobolTotalIndex(i)
    print(f"  weighted X{i + 1}: S1={s1:.6f} (ref={sob_1_ref[i]:.6f}),"
          f" ST={st:.6f} (ref={sob_T1_ref[i]:.6f})")
    ott.assert_almost_equal(s1, sob_1_ref[i], 0.15, 0.05)
    ott.assert_almost_equal(st, sob_T1_ref[i], 0.15, 0.05)
# the second variable enters additively, so its total index equals its main
# index; the third one only through an interaction, so the expansion must not
# invent a main effect for it
ott.assert_almost_equal(
    sobolWeighted.getSobolTotalIndex(1), sobolWeighted.getSobolIndex(1),
    1.0e-12, 1.0e-12,
)
ott.assert_almost_equal(sobolWeighted.getSobolIndex(2), 0.0, 1.0e-12, 1.0e-12)

# L2 error of the C++ approximation on a large independent Monte Carlo sample.
# The CV-selected model is coarse by design, the measured error is about 0.39
l2TestSample = distribution.getSample(10000)
l2TestOutput = model(l2TestSample)
l2ApproxOutput = result_cpp.getMetaModel()(l2TestSample)
l2MeanSquare = (l2TestOutput - l2ApproxOutput).computeRawMoment(2)[0]
ott.assert_almost_equal(l2MeanSquare, 0.0, 0.0, 0.6)
print(f"L2 mean-square error: {l2MeanSquare:.6e}")

print("LARS comparison: OK")
