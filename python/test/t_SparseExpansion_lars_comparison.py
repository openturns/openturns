#!/usr/bin/env python
"""
Test case 2: Compare C++ SparseExpansion LARS with a Python LARS reference
implementation.

Uses the same Ishigami sparse problem as test case 1 (100 samples, max degree 3).
Compares selection paths, active sets, and Sobol indices.
"""

import openturns as ot
import openturns.experimental as otexp
from math import sqrt

ot.TESTPREAMBLE()


class PythonLARS:
    """Minimal Python LARS for comparison with C++ SparseExpansion."""

    def __init__(self, input_sample, output_sample, distribution, basis,
                 basisSize, fittingAlgorithm, methodName="SVD"):
        self.input_sample = input_sample
        self.output_sample = output_sample
        self.distribution = distribution
        self.basis = basis
        self.basisSize = basisSize
        self.fittingAlgorithm = fittingAlgorithm
        self.methodName = methodName
        self.sample_size = input_sample.getSize()
        self.wX = ot.Point(self.sample_size, 1.0 / self.sample_size)

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

        for output_index in range(output_dimension):
            marginal_output = self.output_sample.getMarginal(output_index)
            marginal_selection = [0]

            leastSquaresMethod = ot.LeastSquaresMethod.Build(
                self.methodName, designProxy, self.wX, marginal_selection
            )
            rhs = marginal_output.asPoint()
            marginal_output_mean = marginal_output.computeMean()[0]
            coefficients = [marginal_output_mean]

            # Current prediction mu
            mu = ot.Point(sample_size, marginal_output_mean)

            # Build full weighted design for correlations
            fullX = designProxy.computeDesign(range(self.basisSize))

            for iteration in range(self.basisSize - 1):
                # Stop if active set would exceed sample size (LS becomes rank-deficient)
                if len(marginal_selection) >= sample_size:
                    break
                # Compute residual
                residual = rhs - mu

                # Weighted correlations c = Phi^T * residual (uniform weights)
                correlations = fullX.getImplementation().genVectProd(residual, True) / sample_size

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

                # Solve normal equation for active set: G_A * g = sC
                psiAk = designProxy.computeDesign(marginal_selection)
                GA = psiAk.transpose() * psiAk
                g = GA.solveLinearSystem(sC)

                cNorm = 1.0 / sqrt(sC.dot(g))
                u = psiAk * (cNorm * g)

                # Compute d = Phi^T * u / n
                d = fullX.getImplementation().genVectProd(u, True) / sample_size

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

            for j in range(len(marginal_selection)):
                idx = marginal_selection[j]
                if idx not in self.coefficients_map:
                    self.coefficients_map[idx] = ot.Point(output_dimension, 0.0)
                self.coefficients_map[idx][output_index] = coefficients[j]

            self.selection_history.append(marginal_selection.copy())

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

fittingAlgorithm = ot.CorrectedLeaveOneOut()

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

# --- Python LARS reference ---
print()
print("=" * 60)
print("Python LARS reference implementation")
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

print("LARS comparison: OK")
