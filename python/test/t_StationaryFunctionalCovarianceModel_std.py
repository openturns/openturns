#!/usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m


f = ot.SymbolicFunction(
    ["tau", "alpha"], ["cos(4*tau)*cosh((tau-pi_)/alpha)/cosh(pi_/alpha)"]
)
alpha = 1.36
rho = ot.ParametricFunction(f, [1], [alpha])

cov = ot.StationaryFunctionalCovarianceModel([1.0], [1.0], rho)
print(cov)

tau = 0.1
c = cov([tau])[0, 0]
print("tau=", tau)
print("c=", c)
c_ref = m.cos(4 * tau) * m.cosh((tau - m.pi) / alpha) / m.cosh(m.pi / alpha)
ott.assert_almost_equal(c, c_ref)

assert len(cov.getFullParameter()) == 3, "wrong full parameter"
assert len(cov.getFullParameterDescription()) == 3, "wrong full parameter description"

print(cov.getFullParameter())
print(cov.getFullParameterDescription())

assert len(cov.getActiveParameter()) == 2, "wrong active parameter"
cov.setActiveParameter(range(3))
cov.setParameter([1.0, 1.0, 0.5])
assert len(cov.getParameter()) == 3, "wrong parameter"
c = cov([tau])[0, 0]
print("c=", c)
assert c != c_ref, "inactive parameter"

# test discretization
X = ot.Uniform(1, 2).getSample(10)
C = cov.discretize(X)
assert C.getNbRows() == 10, "wrong size"

# thread-safety test
for i in range(1000):

    def fun_mixte(X):
        xx, z = X
        if z == 0:
            y = m.sin(7.0 * xx)
        else:
            y = 2.0 * m.sin(7.0 * xx)
        return y

    XX_input = ot.Sample(
        [
            [0.1, 0],
            [0.32, 0],
            [0.6, 0],
            [0.9, 0],
            [0.07, 1],
            [0.1, 1],
            [0.4, 1],
            [0.5, 1],
            [0.85, 1],
        ]
    )
    y_output = ot.Sample(len(XX_input), 1)
    for i in range(len(XX_input)):
        y_output[i, 0] = fun_mixte(XX_input[i])

    def C(s, t):
        return m.exp(-4.0 * abs(s - t) / (1 + (s * s + t * t)))

    N = 32
    a = 4.0
    myMesh = ot.IntervalMesher([N]).build(ot.Interval(-a, a))

    myCovariance = ot.CovarianceMatrix(myMesh.getVerticesNumber())
    for k in range(myMesh.getVerticesNumber()):
        t = myMesh.getVertices()[k]
        for ll in range(k + 1):
            s = myMesh.getVertices()[ll]
            myCovariance[k, ll] = C(s[0], t[0])

    covModel_discrete = ot.UserDefinedCovarianceModel(myMesh, myCovariance)
    f_ = ot.SymbolicFunction(
        ["tau", "theta", "sigma"],
        [
            "(tau!=0) * exp(-1/theta) * sigma * sigma +  (tau==0) * exp(0) * sigma * sigma"
        ],
    )
    rho = ot.ParametricFunction(f_, [1, 2], [0.2, 0.3])
    covModel_discrete = ot.StationaryFunctionalCovarianceModel([1.0], [1.0], rho)
    covModel_continuous = ot.SquaredExponential([1.0], [1.0])
    covarianceModel = ot.ProductCovarianceModel(
        [covModel_continuous, covModel_discrete]
    )
    covarianceModel.discretize(XX_input)
