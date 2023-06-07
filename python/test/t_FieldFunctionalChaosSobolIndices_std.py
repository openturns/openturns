#!/usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()
# ot.Log.Show(ot.Log.INFO)

# Time grid parameters
T = 3.0
NT = 32
tg = ot.RegularGrid(0.0, T / NT, NT)

# Toy function to link input processes to the output process
in_dim = 4
out_dim = 1
spatial_dim = 1


class pyf2p(ot.OpenTURNSPythonFieldToPointFunction):
    def __init__(self, mesh):
        super(pyf2p, self).__init__(mesh, in_dim, out_dim)
        self.setInputDescription(["x1", "x2", "x3", "x4"])
        self.setOutputDescription(["g"])

    def _exec(self, X):
        Xs = ot.Sample(X)
        x1, x2, x3, x4 = Xs.computeMean()
        y = x1 + x2 + x3 - x4 + x1 * x2 - x3 * x4 - 0.1 * x1 * x2 * x3
        return [y]


f = ot.FieldToPointFunction(pyf2p(tg))

# First process: elementary process based on a bivariate random vector
f1 = ot.SymbolicFunction(["t"], ["sin(t)"])
f2 = ot.SymbolicFunction(["t"], ["cos(t)^2"])
myBasis = ot.Basis([f1, f2])
coefDis = ot.Normal([1.0] * 2, [0.6] * 2, ot.CorrelationMatrix(2))
p1 = ot.FunctionalBasisProcess(coefDis, myBasis, tg)

# Second process: smooth Gaussian process
p2 = ot.GaussianProcess(ot.SquaredExponential([1.0], [T / 4.0]), tg)

# Third process: elementary process based on a bivariate random vector
randomParameters = ot.ComposedDistribution([ot.Uniform(), ot.Normal()])
p3 = ot.FunctionalBasisProcess(
    randomParameters,
    ot.Basis(
        [ot.SymbolicFunction(["t"], ["1", "0"]), ot.SymbolicFunction(["t"], ["0", "1"])]
    ),
)

X = ot.AggregatedProcess([p1, p2, p3])
X.setMesh(tg)

N = 1000
x = X.getSample(N)
y = f(x)

# run algo
algo = otexp.FieldToPointFunctionalChaosAlgorithm(x, y)
ot.ResourceMap.SetAsUnsignedInteger("FunctionalChaosAlgorithm-BasisSize", N)
ot.ResourceMap.SetAsBool("FunctionalChaosAlgorithm-Sparse", True)
algo.setThreshold(4e-2)
algo.setRecompress(True)
algo.run()
result = algo.getResult()
bi = result.getBlockIndices()
print(f"block indices={bi}")
assert len(bi) == 4, "wrong number of blocks"

# inspect modes
kl_results = result.getInputKLResultCollection()
n_modes = [len(res.getEigenvalues()) for res in kl_results]
print(f"n_modes={n_modes}")
ratios = [res.getSelectionRatio() for res in kl_results]
print(f"ratios={ratios}")

# retrieve Sobol indices
sensitivity = otexp.FieldFunctionalChaosSobolIndices(result)
sobol_0 = sensitivity.getFirstOrderIndices()
sobol_0t = sensitivity.getTotalOrderIndices()
print(f"first order={sobol_0}")
print(f"total order={sobol_0t}")
ott.assert_almost_equal(sobol_0, [0.0673166, 0.439809, 0.0946051, 0.278286])
ott.assert_almost_equal(sobol_0t, [0.0908364, 0.463752, 0.191125, 0.374326])
graph = sensitivity.draw()
# ot.Show(graph)

sobol2 = dict(
    [((j, i), sensitivity.getSobolIndex([i, j])) for i in range(4) for j in range(i)]
)
print(sobol2)
ott.assert_almost_equal(sobol2[(0, 1)], 0.530589)

# rerun with block indices
blockIndices = [[0], [1], [2, 3]]
algo.setBlockIndices(blockIndices)
algo.run()
result = algo.getResult()

# retrieve Sobol indices
sensitivity = otexp.FieldFunctionalChaosSobolIndices(result)
sobol_0 = sensitivity.getFirstOrderIndices()
sobol_0t = sensitivity.getTotalOrderIndices()
print(f"first order={sobol_0}")
print(f"total order={sobol_0t}")
ott.assert_almost_equal(sobol_0, [0.06653, 0.434889, 0.474891])
ott.assert_almost_equal(sobol_0t, [0.0897988, 0.458579, 0.475371])
graph = sensitivity.draw()
# ot.Show(graph)
