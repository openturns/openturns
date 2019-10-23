#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

print('Default q :', ResourceMap.GetAsScalar(
    'HyperbolicAnisotropicEnumerateFunction-DefaultQ'), '\n')

# check weight constructor
f = HyperbolicAnisotropicEnumerateFunction((1., 2., 3., 4., 5.), 0.75)

# check inverse when the cache is empty
print("inverse([0,0,0,0,0])=", f.inverse([0, 0, 0, 0, 0]))
# fill the cache
for i in range(4):
    res = f(i)
# check inverse when the cache is filled
print("inverse([2,0,0,0,0])=", f.inverse([2, 0, 0, 0, 0]))

# verify consistency with LinearEnumerateFunction
size = 10
stratas = 5
for dimension in range(1, 4):
    f = HyperbolicAnisotropicEnumerateFunction(dimension, 1.0)
    g = LinearEnumerateFunction(dimension)
    print("First", size, "values for dimension", dimension)
    for index in range(size):
        if (not f(index) == g(index)):
            raise Exception('spam', f(index), g(index))
        print("index=", index, repr(f(index)))
    strataCardinal = []
    for index in range(stratas):
        strataCardinal.append(int(f.getStrataCardinal(index)))
    print('And first', stratas, 'strata cardinals :', strataCardinal, '\n')


# now let q vary
size = 25
qValues = []
qValues.append(0.75)
qValues.append(0.5)
qValues.append(0.25)

for dimension in range(2, 5):
    for j in range(len(qValues)):
        q = qValues[j]
        print('First', size, 'values dimension=', dimension, ' q=', q)
        f = HyperbolicAnisotropicEnumerateFunction(dimension, q)
        for index in range(size):
            print("index=", index, repr(f(index)))
        strataCardinal = []
        for index in range(stratas):
            strataCardinal.append(int(f.getStrataCardinal(index)))
        print('And first', stratas, 'strata cardinals :', strataCardinal)

        cardinalsPerDegree = []
        for deg in range(5):
            cardinalsPerDegree.append(int(f.getMaximumDegreeCardinal(deg)))
        print('Cardinals per degree=', cardinalsPerDegree, '\n')
