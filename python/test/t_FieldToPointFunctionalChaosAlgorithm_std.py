#!/usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()
#ot.Log.Show(ot.Log.INFO)

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
        self.setOutputDescription(['g'])
    def _exec(self, X):
        Xs = ot.Sample(X)
        x1, x2, x3, x4 = Xs.computeMean()
        y = x1 + x2 + x3 - x4 + x1 * x2 - x3 * x4 - 0.1 * x1 * x2 * x3
        return [y]
f = ot.FieldToPointFunction(pyf2p(tg))

# First process: elementary process based on a bivariate random vector
f1 = ot.SymbolicFunction(['t'], ['sin(t)'])
f2 = ot.SymbolicFunction(['t'], ['cos(t)^2'])
myBasis = ot.Basis([f1, f2])
coefDis = ot.Normal([1.0] * 2, [0.6] * 2, ot.CorrelationMatrix(2))
p1 = ot.FunctionalBasisProcess(coefDis, myBasis, tg)

# Second process: smooth Gaussian process
p2 = ot.GaussianProcess(ot.SquaredExponential([1.0], [T / 4.0]), tg)

# Third process: elementary process based on a bivariate random vector
randomParameters = ot.ComposedDistribution([ot.Uniform(), ot.Normal()])
p3 = ot.FunctionalBasisProcess(randomParameters, ot.Basis([ot.SymbolicFunction(["t"], ["1", "0"]), ot.SymbolicFunction(["t"], ["0", "1"])]))

X = ot.AggregatedProcess([p1, p2, p3])
X.setMesh(tg)

N = 1000
x = X.getSample(N)
y = f(x)


# run algo
algo = otexp.FieldToPointFunctionalChaosAlgorithm(x, y)
algo.setThreshold(4e-2)
algo.setRecompress(True)
ot.ResourceMap.SetAsUnsignedInteger('FunctionalChaosAlgorithm-BasisSize', N)
ot.ResourceMap.SetAsBool('FunctionalChaosAlgorithm-Sparse', True)
algo.run()
result = algo.getResult()

# check metamodel
metamodel = result.getFieldToPointMetamodel()
assert metamodel.getInputDimension() == x.getDimension(), "wrong in dim"
assert metamodel.getOutputDimension() == y.getDimension(), "wrong out dim"

# test single evaluation
xm = x.computeMean()
print('f(xm)=', f(xm))
fhat_xm = metamodel(xm)
print('f^(xm)=', fhat_xm)
ott.assert_almost_equal(fhat_xm, [1.09112])

# test residual
residuals = result.getFCEResult().getResiduals()
print('residuals', residuals)
assert residuals.norm() < 5e-3, "residual too large"

# check modes retained
kl_results = result.getInputKLResultCollection()
n_modes = [len(res.getEigenvalues()) for res in kl_results]
print(f"n_modes={n_modes}")
assert sum(n_modes) == 6, "wrong modes"

# rerun with block indices
blockIndices = [[0], [1], [2, 3]]
algo.setBlockIndices(blockIndices)
algo.run()
result = algo.getResult()

# test residual
residuals = result.getFCEResult().getResiduals()
print('residuals', residuals)
assert residuals.norm() < 5e-3, "residual too large"

# check modes retained
kl_results = result.getInputKLResultCollection()
n_modes = [len(res.getEigenvalues()) for res in kl_results]
print(f"n_modes={n_modes}")
assert sum(n_modes) == 6, "wrong modes"
