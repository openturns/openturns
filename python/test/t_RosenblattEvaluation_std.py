#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

nu = 4.5
dim = 5
distribution = ot.Student(nu, dim)
transformation = ot.RosenblattEvaluation(distribution)
print("transformation=", repr(transformation))
point = ot.Point(dim, 0.75)
print("transformation(", point, ")=", repr(transformation(point)))
# the transformation value matches the sequential CDF mapped through qNormal
q = distribution.computeSequentialConditionalCDF(point)
expected = ot.Point([ot.DistFunc.qNormal(qi) for qi in q])
ott.assert_almost_equal(transformation(point), expected, 1e-12, 1e-12)
gradient = transformation.parameterGradient(point)
print("transformation parameters gradient=", repr(gradient))
# the finite-difference gradient covers all distribution parameters
assert gradient.getNbRows() == distribution.getParameter().getDimension()
assert gradient.getNbColumns() == dim
# independent finite-difference reference from perturbed transformations
eps = 1e-5
params = distribution.getParameter()
ref = ot.Matrix(len(params), dim)
for k in range(len(params)):
    pp = ot.Point(params)
    pm = ot.Point(params)
    pp[k] += eps
    pm[k] -= eps
    tPlus = ot.RosenblattEvaluation(distribution)
    tPlus.setParameter(pp)
    vPlus = tPlus(point)
    tMinus = ot.RosenblattEvaluation(distribution)
    tMinus.setParameter(pm)
    vMinus = tMinus(point)
    for j in range(dim):
        ref[k, j] = (vPlus[j] - vMinus[j]) / (2.0 * eps)
ott.assert_almost_equal(gradient, ref, 1e-4, 1e-4)
print("input dimension=", transformation.getInputDimension())
print("output dimension=", transformation.getOutputDimension())
# no gradient when a parameter cannot be perturbed (e.g. integer parameters),
# even for otherwise continuous distributions
with ott.assert_raises(RuntimeError):
    ot.RosenblattEvaluation(ot.Binomial(10, 0.3)).parameterGradient([1.0])
# but a discrete distribution with continuous-only parameters is supported
poisDist = ot.Poisson(2.0)
poisTrans = ot.RosenblattEvaluation(poisDist)
poisGrad = poisTrans.parameterGradient([1.0])
assert poisGrad.getNbRows() == 1 and poisGrad.getNbColumns() == 1
poisParams = poisDist.getParameter()
poisEps = 1e-5
poisPlus = ot.RosenblattEvaluation(poisDist)
poisPPlus = ot.Point(poisParams)
poisPPlus[0] += poisEps
poisPlus.setParameter(poisPPlus)
poisMinus = ot.RosenblattEvaluation(poisDist)
poisPMinus = ot.Point(poisParams)
poisPMinus[0] -= poisEps
poisMinus.setParameter(poisPMinus)
ott.assert_almost_equal(
    poisGrad[0, 0],
    (poisPlus([1.0])[0] - poisMinus([1.0])[0]) / (2.0 * poisEps),
    1e-4,
    1e-4,
)

# parameters are exposed and honored
distribution = ot.Normal(1.0, 2.0)
transformation = ot.RosenblattEvaluation(distribution)
ott.assert_almost_equal(transformation.getParameter(), [1.0, 2.0])
ott.assert_almost_equal(transformation([0.0]), [-0.5])
transformation.setParameter([5.0, 7.0])
ott.assert_almost_equal(transformation.getParameter(), [5.0, 7.0])
ott.assert_almost_equal(transformation([0.0]), [-5.0 / 7.0])
ott.assert_almost_equal(
    transformation.parameterGradient([0.0]), ot.Matrix([[-1.0 / 7.0], [5.0 / 49.0]])
)
