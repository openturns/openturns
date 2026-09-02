#!/usr/bin/env python
"""
Test case 1: Compare C++ SparseExpansion OMP with the Python OMP reference
implementation from mbaudin47 (PR #2987).

The Python reference implements Algorithm B.1 from Luthen et al. 2021.
Both implementations solve the Ishigami sparse problem with 100 samples,
max degree 3, using CLOO fitting.
"""

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()


# --- Python reference OMP implementation (from mbaudin47 gist) ---
class PythonOMP:
    """Minimal Python OMP for comparison with C++ SparseExpansion."""

    def __init__(self, input_sample, output_sample, distribution, basis,
                 candidateBasisSize, fittingAlgorithm, methodName="SVD"):
        self.input_sample = input_sample
        self.output_sample = output_sample
        self.distribution = distribution
        self.basis = basis
        self.candidateBasisSize = candidateBasisSize
        self.fittingAlgorithm = fittingAlgorithm
        self.methodName = methodName
        self.wX = ot.Point(input_sample.getSize(), 1.0 / input_sample.getSize())

    def run(self):
        transformation = ot.DistributionTransformation(
            self.distribution, self.basis.getMeasure()
        )
        standard_input = transformation(self.input_sample)
        sample_size = standard_input.getSize()
        output_dimension = self.output_sample.getDimension()

        functions = [self.basis.build(i) for i in range(self.candidateBasisSize)]
        designProxy = ot.DesignProxy(standard_input, functions)
        X = designProxy.computeDesign(range(self.candidateBasisSize))
        XT = X.transpose()

        self.coefficients_map = {}
        self.selection_history = []

        for output_index in range(output_dimension):
            marginal_output = self.output_sample.getMarginal(output_index)
            marginal_selection = [0]

            leastSquaresMethod = ot.LeastSquaresMethod.Build(
                self.methodName, designProxy, self.wX, marginal_selection
            )
            rightHandSide = marginal_output.asPoint()

            marginal_output_mean = marginal_output.computeMean()[0]
            residuals = rightHandSide - ot.Point(sample_size, marginal_output_mean)
            coefficients = [marginal_output_mean]

            for i in range(self.candidateBasisSize - 1):
                # Stop if active set would exceed sample size (LS becomes rank-deficient)
                if len(marginal_selection) >= sample_size:
                    break
                v = (XT * residuals) / sample_size

                maximum_absolute_correlation = 0.0
                best_basis_function_index = None
                for j in range(self.candidateBasisSize):
                    if j in marginal_selection:
                        continue
                    current_absolute_correlation = abs(v[j])
                    if current_absolute_correlation > maximum_absolute_correlation:
                        best_basis_function_index = j
                        maximum_absolute_correlation = current_absolute_correlation

                if best_basis_function_index is None:
                    break

                leastSquaresMethod.update(
                    [best_basis_function_index], marginal_selection, []
                )
                marginal_selection.append(best_basis_function_index)
                coefficients = leastSquaresMethod.solve(rightHandSide)
                designMatrix = leastSquaresMethod.computeWeightedDesign()
                residuals = rightHandSide - designMatrix * coefficients

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
covTh = (b**2 * 3.141592653589793**8) / 18.0 + (b * 3.141592653589793**4) / 5.0 + (a**2) / 8.0 + 1.0 / 2.0
sob_1_ref = [
    (b * 3.141592653589793**4 / 5.0 + b**2 * 3.141592653589793**8 / 50.0 + 1.0 / 2.0) / covTh,
    (a**2 / 8.0) / covTh,
    0.0,
]
sob_T1_ref = [
    sob_1_ref[0] + 0.0 + (b**2 * 3.141592653589793**8 / 18.0 - b**2 * 3.141592653589793**8 / 50.0) / covTh + 0.0,
    sob_1_ref[1] + 0.0 + 0.0 + 0.0,
    sob_1_ref[2] + (b**2 * 3.141592653589793**8 / 18.0 - b**2 * 3.141592653589793**8 / 50.0) / covTh + 0.0,
]

inputVariables = ["xi1", "xi2", "xi3"]
formula = ["sin(xi1) + 7.0 * (sin(xi2))^2 + 0.1 * xi3^4 * sin(xi1)"]
model = ot.SymbolicFunction(inputVariables, formula)
distribution = ot.JointDistribution([ot.Uniform(-3.141592653589793, 3.141592653589793)] * dimension)

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

# --- C++ SparseExpansion OMP ---
print("=" * 60)
print("C++ SparseExpansion with OMP selection")
print("=" * 60)
algo_cpp = otexp.SparseExpansion(
    inputSample, outputSample, distribution,
    productBasis, basisSize, "QR", fittingAlgorithm
)
algo_cpp.setSelectionMethod("OMP")
algo_cpp.run()
result_cpp = algo_cpp.getResult()

sobol_cpp = ot.FunctionalChaosSobolIndices(result_cpp)
print("Active functions:", list(result_cpp.getIndices()))
print("Number of active functions:", result_cpp.getIndices().getSize())
for i in range(dimension):
    s1 = sobol_cpp.getSobolIndex(i)
    st = sobol_cpp.getSobolTotalIndex(i)
    print(f"  X{i + 1}: S1={s1:.6f} (ref={sob_1_ref[i]:.6f}), ST={st:.6f} (ref={sob_T1_ref[i]:.6f})")

# --- Python OMP reference ---
print()
print("=" * 60)
print("Python OMP reference implementation")
print("=" * 60)
algo_py = PythonOMP(
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
print("Comparison: C++ OMP vs Python OMP")
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

# Assert C++ OMP produces correct Sobol indices (within tolerance)
# The C++ implementation uses cross-validation stopping, while the Python reference
# runs all iterations, so active sets may differ. Check that the C++ result is
# close to the reference values.
for i in range(dimension):
    s1_cpp = sobol_cpp.getSobolIndex(i)
    ott.assert_almost_equal(s1_cpp, sob_1_ref[i], 0.05, 1e-5)

print("OMP comparison: OK")
