#!/usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()
# ot.Log.Show(ot.Log.INFO)

# Time grid parameters
T = 3.0
NT = 32
mesh = ot.RegularGrid(0.0, T / NT, NT)

g = ot.SymbolicFunction(["t", "x1", "x2", "x3", "x4"],
                        ["x1 + x2 * sin(t)", "x2 + x3 * cos(t)", "x4 * t"])
f = ot.VertexValuePointToFieldFunction(g, mesh)

X = ot.Normal(4)

N = 1000
x = X.getSample(N)
y = f(x)

# run algo
algo = otexp.PointToFieldFunctionalChaosAlgorithm(x, y, X)
algo.setThreshold(4e-2)
algo.setRecompress(True)
ot.ResourceMap.SetAsUnsignedInteger("FunctionalChaosAlgorithm-BasisSize", N)
ot.ResourceMap.SetAsBool("FunctionalChaosAlgorithm-Sparse", True)
algo.run()
result = algo.getResult()

# check metamodel
metamodel = result.getPointToFieldMetaModel()
print(metamodel.getInputDimension(), metamodel.getOutputDimension())
assert metamodel.getInputDimension() == x.getDimension(), "wrong in dim"
assert metamodel.getOutputDimension() == y.getDimension(), "wrong out dim"

# test single evaluation
xm = x.computeMean()
print("f(xm)=", f(xm))
fhat_xm = metamodel(xm)
print("f^(xm)=", fhat_xm)
# ott.assert_almost_equal(fhat_xm, [1.09018], 1e-3, 1e-3)

# test residual
residuals = result.getFCEResult().getResiduals()
print("residuals", residuals)
assert residuals.norm() < 5e-3, "residual too large"

# check modes retained
kl_results = result.getOutputKLResultCollection()
n_modes = [len(res.getEigenvalues()) for res in kl_results]
print(f"n_modes={n_modes}")
assert sum(n_modes) == 3, "wrong modes"

# separate components {0,1} from {2} and rerun
blockIndices = [[0, 1], [2]]
algo.setBlockIndices(blockIndices)
algo.run()
result = algo.getResult()

# test residual
residuals = result.getFCEResult().getResiduals()
print("residuals", residuals)
assert residuals.norm() < 5e-3, "residual too large"

# check modes retained
kl_results = result.getOutputKLResultCollection()
n_modes = [len(res.getEigenvalues()) for res in kl_results]
print(f"n_modes={n_modes}")
# assert sum(n_modes) == 6, "wrong modes"

# retrieve Sobol indices
sensitivity = otexp.FieldFunctionalChaosSobolIndices(result)
for marginalIndex in range(len(blockIndices)):
    s1 = sensitivity.getFirstOrderIndices(marginalIndex)
    st = sensitivity.getTotalOrderIndices(marginalIndex)
    print(s1, st)
ott.assert_almost_equal(sensitivity.getFirstOrderIndices(0), [0.457798, 0.447243, 0.0905814, 0.00437806], 0.0, 2e-2)
