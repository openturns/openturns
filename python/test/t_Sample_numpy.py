#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import numpy as np

ot.TESTPREAMBLE()

# Numpy interop for Sample
# __eq__ keeps whole-object semantics (returns Bool)
smp = ot.Sample([[1.0, 2.0], [2.0, 3.0], [1.0, 2.0]])
assert smp == ot.Sample([[1.0, 2.0], [2.0, 3.0], [1.0, 2.0]])
assert not (smp == [[1.0, 2.0]])
assert hasattr(smp, "__array_interface__")
assert hasattr(smp, "__array_ufunc__")
arr_s = np.asarray(smp)
assert arr_s.shape == (3, 2)
ott.assert_almost_equal(arr_s[0, 0], 1.0)

eq_s = np.equal(smp, 1.0)
np.testing.assert_array_equal(
    eq_s, [[True, False], [False, False], [True, False]]
)
w_s = np.where(eq_s)
np.testing.assert_array_equal(w_s[0], [0, 2])
np.testing.assert_array_equal(w_s[1], [0, 0])
np.testing.assert_allclose(np.add(smp, 1.0)[0], [2.0, 3.0])

# fancy indexing with where() result still works
ott.assert_almost_equal(smp[list(np.where(arr_s[:, 0] == 1.0)[0])][0], [1.0, 2.0])

print("ok")
