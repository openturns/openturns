#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott
import math as m


f = ot.SymbolicFunction(
    ['tau', 'alpha'], ['cos(4*tau)*cosh((tau-pi_)/alpha)/cosh(pi_/alpha)'])
alpha = 1.36
rho = ot.ParametricFunction(f, [1], [alpha])

cov = ot.StationaryFunctionalCovarianceModel([1.0], [1.0], rho)
print(cov)

tau = 0.1
c = cov([tau])[0, 0]
print("tau=", tau)
print("c=", c)
c_ref = m.cos(4*tau)*m.cosh((tau-m.pi)/alpha)/m.cosh(m.pi/alpha)
ott.assert_almost_equal(c, c_ref)

assert len(cov.getFullParameter()) == 3, "wrong full parameter"
assert len(cov.getFullParameterDescription()
           ) == 3, "wrong full parameter description"

print(cov.getFullParameter())
print(cov.getFullParameterDescription())

assert len(cov.getActiveParameter()) == 2, "wrong active parameter"
cov.setActiveParameter(range(3))
cov.setParameter([1.0, 1.0, 0.5])
assert len(cov.getParameter()) == 3, "wrong parameter"
c = cov([tau])[0, 0]
print("c=", c)
assert c != c_ref, "inactive parameter"
