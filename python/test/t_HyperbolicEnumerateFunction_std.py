#! /usr/bin/env python

import openturns as ot
import math


def q_norm(enumerate_function, indices):
    """
    Compute the Q-Norm of the multi-index

    Note. We reimplement qNorm, because it is a private method of HyperbolicEnumerateFunction
    """
    result = 0.0
    q = enumerate_function.getQ()
    dimension = len(indices)

    if q == 1.0:
        for j in range(dimension):
            result += indices[j]
        return result

    for j in range(dimension):
        result += math.pow(float(indices[j]), q)

    return math.pow(result, 1.0 / q)


ot.TESTPREAMBLE()

print(
    "Default q :",
    ot.ResourceMap.GetAsScalar("HyperbolicEnumerateFunction-DefaultQ"),
    "\n",
)

# check constructor
f = ot.HyperbolicEnumerateFunction(5, 0.75)

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
    f = ot.HyperbolicEnumerateFunction(dimension, 1.0)
    g = ot.LinearEnumerateFunction(dimension)
    print("First", size, "values for dimension", dimension)
    previousIndexQNorm = 0.0
    for index in range(size):
        multiindex = f(index)
        indexQNorm = q_norm(f, multiindex)
        referenceIndexQNorm = q_norm(f, multiindex)
        print(f"index={index}, multi-index={multiindex}, indexQNorm={indexQNorm:.4f}")
        if not f(index) == g(index):
            raise Exception(f"Computed index={index}, "
                            f"multiindex={multiindex} (q-norm={indexQNorm:.4f}), "
                            f"reference multiindex={g(index)} (q-norm={referenceIndexQNorm:.4f})")
        # Check that the Q-Norms are increasing
        if index > 0:
            assert indexQNorm >= previousIndexQNorm
        # Update Q-Norm for next loop
        previousIndexQNorm = indexQNorm
    strataCardinal = []
    for index in range(stratas):
        strataCardinal.append(int(f.getStrataCardinal(index)))
    print("And first", stratas, "strata cardinals :", strataCardinal, "\n")


# now let q vary
size = 25
for dimension in range(2, 5):
    for q in [0.75, 0.5, 0.25]:
        print("First", size, "values dimension=", dimension, " q=", q)
        f = ot.HyperbolicEnumerateFunction(dimension, q)
        previousIndexQNorm = 0.0
        for index in range(size):
            multiindex = f(index)
            indexQNorm = q_norm(f, multiindex)
            print(f"index={index}, multiindex={multiindex}, Q-Norm={indexQNorm:.4f}")
            # Check that the Q-Norms are increasing
            if index > 0:
                assert indexQNorm >= previousIndexQNorm
            # Update Q-Norm for next loop
            previousIndexQNorm = indexQNorm
        # Check getStrataCardinal()
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
    f = ot.HyperbolicEnumerateFunction(dimension, q)
    for d in range(10):
        idx = f.getMaximumDegreeStrataIndex(d)
        print("dim", dimension, "degree", d, "max_degree_strata_index", idx)
        size = f.getBasisSizeFromTotalDegree(d)
        print("degree", d, "size", size)

# with bounds
f = ot.HyperbolicEnumerateFunction(2, 1.0)
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
#
print("Test getMarginal() from indices")
f = ot.HyperbolicEnumerateFunction(5, 0.5)
marginalf = f.getMarginal([0, 3, 4])
for index in range(10):
    m = marginalf(index)
    index_inv = marginalf.inverse(m)
    print("index=", index, repr(m), index_inv)

print("Test getMarginal() from a single integer")
f = ot.HyperbolicEnumerateFunction(5, 0.5)
marginalf = f.getMarginal(3)
for index in range(10):
    m = marginalf(index)
    index_inv = marginalf.inverse(m)
    print("index=", index, repr(m), index_inv)
