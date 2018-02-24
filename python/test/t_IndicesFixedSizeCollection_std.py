#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

# Default constructor
flat1 = ot.IndicesFixedSizeCollection()
print('flat1=', flat1)

# Constructor with Collection<Indices>
coll2 = ot.IndicesCollection()
ind1 = ot.Indices(1)
ind4 = ot.Indices(4)
ind4.fill(8)
ind3 = ot.Indices(3)
ind3.fill(3)
coll2.add(ind1)
coll2.add(ind4)
coll2.add(ind3)
flat2 =  ot.IndicesFixedSizeCollection(coll2);
print('flat2=', flat2)

# Copy constructor
flat3 = ot.IndicesFixedSizeCollection(flat2);
print('flat3=', flat3)

# Strided
coll4 = ot.IndicesCollection()
coll4.add(ind4)
coll4.add(ind4)
coll4.add(ind4)
strided1 =  ot.IndicesFixedSizeCollection(coll4);
print('strided1=', strided1)

# Constructor with dimensions and values
full = ot.Indices(16);
full.fill();
strided2 = ot.IndicesFixedSizeCollection(8, 2, full)
print('strided2=', strided2)

# slicing
print('strided2[2] = ', strided2[2])
strided2[2] = [1, 3]
print('strided2 = ', strided2)
try:
    strided2[2] = [1, 2, 3]
    print('Should throw')
except TypeError:
    pass

# loop
for x in strided2:
    print(x)

