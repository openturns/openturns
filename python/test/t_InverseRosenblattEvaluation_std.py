#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

nu = 4.5
dim = 5
distribution = ot.Student(nu, dim)
transformation = ot.InverseRosenblattEvaluation(distribution)
print("transformation=", repr(transformation))
point = ot.Point(dim, 0.75)
print("transformation(", point, ")=", repr(transformation(point)))
# the transformation value matches pNormal mapped through the sequential quantile
pn = ot.Point([ot.DistFunc.pNormal(pi) for pi in point])
expected = distribution.computeSequentialConditionalQuantile(pn)
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
    tPlus = ot.InverseRosenblattEvaluation(distribution)
    tPlus.setParameter(pp)
    vPlus = tPlus(point)
    tMinus = ot.InverseRosenblattEvaluation(distribution)
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
    ot.InverseRosenblattEvaluation(ot.Binomial(10, 0.3)).parameterGradient([1.0])
# but a discrete distribution with continuous-only parameters is supported
poisDist = ot.Poisson(2.0)
poisTrans = ot.InverseRosenblattEvaluation(poisDist)
poisGrad = poisTrans.parameterGradient([0.3])
assert poisGrad.getNbRows() == 1 and poisGrad.getNbColumns() == 1
poisParams = poisDist.getParameter()
poisEps = 1e-5
poisPlus = ot.InverseRosenblattEvaluation(poisDist)
poisPPlus = ot.Point(poisParams)
poisPPlus[0] += poisEps
poisPlus.setParameter(poisPPlus)
poisMinus = ot.InverseRosenblattEvaluation(poisDist)
poisPMinus = ot.Point(poisParams)
poisPMinus[0] -= poisEps
poisMinus.setParameter(poisPMinus)
ott.assert_almost_equal(
    poisGrad[0, 0],
    (poisPlus([0.3])[0] - poisMinus([0.3])[0]) / (2.0 * poisEps),
    1e-4,
    1e-4,
)

# parameters are exposed and honored
distribution = ot.Normal(1.0, 2.0)
transformation = ot.InverseRosenblattEvaluation(distribution)
ott.assert_almost_equal(transformation.getParameter(), [1.0, 2.0])
transformation.setParameter([5.0, 7.0])
ott.assert_almost_equal(transformation.getParameter(), [5.0, 7.0])
ott.assert_almost_equal(transformation([0.3]), [5.0 + 7.0 * 0.3])
ott.assert_almost_equal(
    transformation.parameterGradient([0.3]), ot.Matrix([[1.0], [0.3]])
)
