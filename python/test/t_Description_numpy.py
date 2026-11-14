#! /usr/bin/env python

import openturns as ot
import numpy as np

ot.TESTPREAMBLE()

# Numpy interop for Description
# __eq__ keeps whole-object semantics (returns Bool)
desc = ot.Description(["a", "b", "a"])
assert desc == ["a", "b", "a"]
assert not (desc == ["a"])
assert hasattr(desc, "__array_ufunc__")

# asarray (copy-based: variable-length strings cannot be zero-copy)
arr = np.asarray(desc)
assert arr.shape == (3,)
np.testing.assert_array_equal(arr, ["a", "b", "a"])

# element-wise ufuncs, then np.where
eq = np.equal(desc, "a")
np.testing.assert_array_equal(eq, [True, False, True])
w = np.where(eq)
np.testing.assert_array_equal(w[0], [0, 2])
w2 = np.where(np.asarray(desc) == "a")
np.testing.assert_array_equal(w2[0], [0, 2])

# roundtrips with numpy string arrays
desc2 = ot.Description(np.array(["x", "y"]))
assert list(desc2) == ["x", "y"]
smp = ot.Sample(2, 2)
smp.setDescription(np.array(["u", "v"]))
assert list(smp.getDescription()) == ["u", "v"]

print("ok")
