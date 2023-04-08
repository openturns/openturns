#! /usr/bin/env python

import openturns as ot

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

index = indices2.find(88)
assert index < indices2.getSize(), "wrong index"
assert indices2[index] == 88, "wrong index"

index = indices2.find(77777)
assert index == indices2.getSize(), "wrong index"

# Check contains
indices = ot.Indices()
assert not indices.contains(0), "wrong contains"
assert not indices.contains(1), "wrong contains"
indices = ot.Indices((1, 2, 3, 4, 5))
assert indices.contains(1), "wrong contains"
assert indices.contains(5), "wrong contains"
indices = ot.Indices((3, 5, 7))
assert indices.contains(3), "wrong contains"
assert indices.contains(5), "wrong contains"
assert indices.contains(7), "wrong contains"
assert not indices.contains(2), "wrong contains"
assert not indices.contains(4), "wrong contains"
assert not indices.contains(0), "wrong contains"
assert not indices.contains(1), "wrong contains"
