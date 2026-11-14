#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import numpy as np

ot.TESTPREAMBLE()

# Numpy interop for Point
# __eq__ keeps whole-object semantics (returns Bool)
pt = ot.Point([1.0, 2.0, 3.0])
assert pt == [1.0, 2.0, 3.0]
assert not (pt == [1.0])
assert pt == ot.Point([1.0, 2.0, 3.0])

# __array_interface__ / asarray (zero-copy, read-only view)
assert hasattr(pt, "__array_interface__")
assert hasattr(pt, "__array_ufunc__")
arr = np.asarray(pt)
ott.assert_almost_equal(list(arr), [1.0, 2.0, 3.0])
assert arr.shape == (3,)
assert arr.dtype == np.float64

# element-wise ufuncs, then np.where
eq = np.equal(pt, 1.0)
np.testing.assert_array_equal(eq, [True, False, False])
w = np.where(eq)
np.testing.assert_array_equal(w[0], [0])
w2 = np.where(np.asarray(pt) == 1.0)
np.testing.assert_array_equal(w2[0], [0])

# arithmetic ufuncs return ndarrays
np.testing.assert_allclose(np.add(pt, 1.0), [2.0, 3.0, 4.0])
np.testing.assert_allclose(np.multiply(pt, 2.0), [2.0, 4.0, 6.0])
np.testing.assert_allclose(np.add(pt, ot.Point([1.0, 1.0, 1.0])), [2.0, 3.0, 4.0])

# empty point
empty = ot.Point(0)
assert np.asarray(empty).shape == (0,)
# empty buffer advertises no address
assert empty.__array_interface__["data"] == (0, True)

# lifetime contract: the view borrows the internal buffer, so a
# size-changing mutation may invalidate it; retain a copy instead
pt2 = ot.Point([1.0, 2.0])
snapshot = np.asarray(pt2).copy()
pt2.add(3.0)
ott.assert_almost_equal(list(snapshot), [1.0, 2.0])
# a fresh view after mutation reflects the new content
ott.assert_almost_equal(list(np.asarray(pt2)), [1.0, 2.0, 3.0])

# fancy indexing with where() result still works
idx = np.where(np.asarray(pt) == 1.0)[0]
ott.assert_almost_equal(pt[list(idx)], [1.0])

print("ok")
