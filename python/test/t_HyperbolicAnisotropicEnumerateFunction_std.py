#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

print(
    "Default q :",
    ot.ResourceMap.GetAsScalar("HyperbolicAnisotropicEnumerateFunction-DefaultQ"),
    "\n",
)

# check weight constructor
f = ot.HyperbolicAnisotropicEnumerateFunction((1.0, 2.0, 3.0, 4.0, 5.0), 0.75)

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
    f = ot.HyperbolicAnisotropicEnumerateFunction(dimension, 1.0)
    g = ot.LinearEnumerateFunction(dimension)
    print("First", size, "values for dimension", dimension)
    for index in range(size):
        if not f(index) == g(index):
            raise Exception("spam", f(index), g(index))
        print("index=", index, repr(f(index)))
    strataCardinal = []
    for index in range(stratas):
        strataCardinal.append(int(f.getStrataCardinal(index)))
    print("And first", stratas, "strata cardinals :", strataCardinal, "\n")


# now let q vary
size = 25
for dimension in range(2, 5):
    for q in [0.75, 0.5, 0.25]:
        print("First", size, "values dimension=", dimension, " q=", q)
        f = ot.HyperbolicAnisotropicEnumerateFunction(dimension, q)
        for index in range(size):
            print("index=", index, repr(f(index)))
        strataCardinal = []
        for index in range(stratas):
            strataCardinal.append(int(f.getStrataCardinal(index)))
        print("And first", stratas, "strata cardinals :", strataCardinal)

        cardinalsPerDegree = []
        for deg in range(5):
            cardinalsPerDegree.append(int(f.getMaximumDegreeCardinal(deg)))
        print("Cardinals per degree=", cardinalsPerDegree, "\n")

for dimension in range(1, 4):
    q = 0.8
    f = ot.HyperbolicAnisotropicEnumerateFunction(dimension, q)
    for d in range(10):
        idx = f.getMaximumDegreeStrataIndex(d)
        print("dim", dimension, "degree", d, "max_degree_strata_index", idx)
        size = f.getBasisSizeFromTotalDegree(d)
        print("degree", d, "size", size)

# with bounds
f = ot.HyperbolicAnisotropicEnumerateFunction(2, 1.0)
f.setUpperBound([int(1e8), 3])
for index in range(25):
    m = f(index)
    index_inv = f.inverse(m)
    print("index=", index, repr(m))
    assert m[1] <= 3, "wrong bound"
    assert index == index_inv, "wrong inverse"
f.setUpperBound([3, int(1e8)])
for index in range(25):
    m = f(index)
    index_inv = f.inverse(m)
    print("index=", index, repr(m))
    assert m[0] <= 3, "wrong bound"
    assert index == index_inv, "wrong inverse"
