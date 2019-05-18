#! /usr/bin/env python

from __future__ import print_function
from openturns import *
import numpy as np

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Instanciate one distribution object
    distribution = DiscreteCompoundDistribution(Bernoulli(0.5), Poisson(20.0))
    print("Distribution ", repr(distribution))
    print("Distribution ", distribution)
    upper_bound = distribution.computeIntegerUpperBound()
    print("Upper bound : {!r}".format(upper_bound))

    for i in range(upper_bound):
        print("Probability of {!r} = {!r}".format(i, distribution.computePDF(Point([i]))))

    points = np.array(range(upper_bound)).reshape(upper_bound, 1)

    pdf = distribution.computePDF(points)

    poisson_distribution = Poisson(10.0)
    poisson_pdf = poisson_distribution.computePDF(points)

except:
    import sys
    print("t_Geometric.py", sys.exc_info()[0], sys.exc_info()[1])
