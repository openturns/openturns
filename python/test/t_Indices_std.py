#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


size = 4
indices = ot.Indices(size, 0)
indices.fill()
print("indices=", repr(indices))
print("are indices valid with bound=", size, "? ", indices.check(size))
print("are indices valid with bound=", size // 2, "? ", indices.check(size // 2))
indices[0] = indices[size - 1]
print("indices after transformation=", repr(indices))
print("are indices valid with bound=", size, "? ", indices.check(size))
print("complement wrt 7=", indices.complement(7))
print("empty complement=", ot.Indices().complement(7))

# slicing
indices2 = ot.Indices((1, 2, 3, 4, 5))
print("indices2[2] = ", indices2[2])
print("indices2[1:3] = ", indices2[1:3])
print("indices2[:] = ", indices2[:])
indices2[1:3] = (88, 99)
print("indices2 = ", indices2)

index = indices2.index(88)
assert indices2[index] == 88, "wrong index"

with ott.assert_raises(TypeError):
    index = indices2.index(77777)

# Check contains
indices = ot.Indices()
assert not (0 in indices), "wrong contains"
assert not (1 in indices), "wrong contains"
indices = ot.Indices((1, 2, 3, 4, 5))
assert 1 in indices, "wrong contains"
assert 5 in indices, "wrong contains"
indices = ot.Indices((3, 5, 7))
assert 3 in indices, "wrong contains"
assert 5 in indices, "wrong contains"
assert 7 in indices, "wrong contains"
assert not (2 in indices), "wrong contains"
assert not (4 in indices), "wrong contains"
assert not (0 in indices), "wrong contains"
assert not (1 in indices), "wrong contains"

# Check norm1() and normInf()
indices = ot.Indices(())
assert indices.norm1() == 0, "wrong norm1"
assert indices.normInf() == 0, "wrong norm1"
indices = ot.Indices((3, 5, 7))
assert indices.norm1() == 15, "wrong norm1"
assert indices.normInf() == 7, "wrong norm1"
