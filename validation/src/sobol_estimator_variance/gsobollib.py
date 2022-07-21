#
# A simple sensitivity analysis, with Sobol indices.
# "Introduction to sensitivity analysis with NISP
# Michael Baudin (EDF), Jean-Marc Martinez (CEA)"
#
# EDF R&D - 2017 - Michael Baudin
#

from openturns import ComposedDistribution, Uniform
from numpy import array, prod, ones, zeros


def gsobol(X, a):
    d = len(a)
    X = array(X)
    sampleSize = X.shape[0]
    Y = ones(sampleSize)
    for i in range(d):
        Yi = (abs(4 * X[:, i] - 2.0) + a[i]) / (1.0 + a[i])
        Y = Y * Yi
    gsobol = zeros((sampleSize, 1))
    gsobol[:, 0] = Y
    return gsobol


# Calculs exacts
def gsobolSAExact(a):
    # Exact mean
    muexact = 1
    # Exact variance
    vexact = prod(1 + 1.0 / (3 * (1 + a) ** 2)) - 1
    # First order sensitivity indices
    sexact = 1.0 / (3 * (1 + a) ** 2) / vexact
    # Total sensitivity indices
    # Compute the product, for j different from i
    # To do this, compute all products, then divide by i.
    suexact = prod(1 + 1.0 / (3 * (1 + a) ** 2))
    suexact = suexact / (1.0 + 1.0 / (3 * (1 + a) ** 2)) - 1
    stexact = 1 - suexact / vexact
    return [muexact, vexact, sexact, stexact]


def gsobolDistribution(d):
    distribution = ComposedDistribution([Uniform(0, 1)] * d)
    return distribution


if __name__ == "__main__":
    a = array([0, 9, 99])
    d = len(a)

    [muexact, vexact, sexact, stexact] = gsobolSAExact(a)
    print("Mean:%.6f" % (muexact))
    print("Variance:%.6f" % (vexact))
    print("First order:%.6f,%.6f,%.6f " % (sexact[0], sexact[1], sexact[2]))
    print("Total order:%.6f,%.6f,%.6f " % (stexact[0], stexact[1], stexact[2]))
    # muexact 1.
    # vexact 0.3378224
    # sxexact 0.9867118    0.0098671    0.0000987
    # stexact 0.9900339    0.0131566    0.0001320
