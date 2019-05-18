#! /usr/bin/env python

from __future__ import print_function
from openturns import *
#import numpy as np

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Instanciate one distribution object
    distribution = DiscreteCompoundDistribution(Bernoulli(0.5), Poisson(20.0))
    print("Distribution ", repr(distribution))
    print("Distribution ", distribution)
    upper_bound = distribution.computeIntegerUpperBound()
    print("Upper bound : {!r}".format(upper_bound))

    # Compare with mathematically equal distribution
    poisson_distribution = Poisson(10.0)

    for i in range(upper_bound):
        print("")
        print("Probability of {!r} = {:0.3f}".format(i, distribution.computePDF(Point([i]))))
        print("Probability of {!r} with equivalent Poisson distribution = {:0.3f}".format(i, poisson_distribution.computePDF(Point([i]))))
                                                                                          

    #numpy necessary for the last lines
    #points = np.array(range(upper_bound)).reshape(upper_bound, 1)
    #pdf = distribution.computePDF(points)
    #poisson_pdf = poisson_distribution.computePDF(points)

except:
    import sys
    print("t_DiscreteCompoundDistribution.py", sys.exc_info()[0], sys.exc_info()[1])
