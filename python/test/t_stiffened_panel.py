#! /usr/bin/env python

import openturns as ot
from openturns.testing import assert_almost_equal
from openturns.usecases import stiffened_panel as stiffened_panel


ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(15)

print("Test the StiffenedPanel class.")
sp = stiffened_panel.StiffenedPanel()

print(sp.distribution.getMean())
# test parameter
assert sp.dim == 10

# test marginals means
assert_almost_equal(sp.youngModulus.getMean()[0], 1.1e11, 1e-12)
assert_almost_equal(sp.nu.getMean()[0], 0.375, 1e-12)
assert_almost_equal(sp.h_c.getMean()[0], 0.03, 1e-12)
assert_almost_equal(sp.ell.getMean()[0], 0.049, 1e-12)
assert_almost_equal(sp.f_1.getMean()[0], 0.028, 1e-12)
assert_almost_equal(sp.f_2.getMean()[0], 0.0066, 1e-12)
assert_almost_equal(sp.t.getMean()[0], 8.1005e-5, 1e-12)
assert_almost_equal(sp.a.getMean()[0], 0.61, 1e-12)
assert_almost_equal(sp.b_0.getMean()[0], 0.045, 1e-12)
assert_almost_equal(sp.p.getMean()[0], 0.038, 1e-12)

meanInput = sp.distribution.getMean()
print("meanInput = ", meanInput)
meanOutput = sp.model(meanInput)
print("meanOutput = ", meanOutput)
assert_almost_equal(meanOutput, [197.76173070456073], 1.0e-15)
