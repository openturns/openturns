#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

# We try to use the Collection as a Python object (sequence)
ulc1 = ot.UnsignedIntegerCollection((1, 2, 3, 4, 5))
ulc2 = ot.UnsignedIntegerCollection([1, 2, 3, 4, 5])
t = (1, 2, 3, 4, 5)
l = [1, 2, 3, 4, 5]
ulc3 = ot.UnsignedIntegerCollection(t)
ulc4 = ot.UnsignedIntegerCollection(l)

ulc1[2] = 100
print(ulc1)
print(len(ulc2), ulc2[0])
for ul in ulc1:
    print(ul)

# Large collection, to see its length
ul5 = ot.UnsignedIntegerCollection(list(range(12)))
print(ul5)

del ul5[6]
print(ul5)
