#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    iMax = 5
    distributionCollection = DistributionCollection()
    distributionCollection.add(Laplace(1.0, 0.0))
    distributionCollection.add(Logistic(0.0, 1.0))
    distributionCollection.add(LogNormal(0.0, 1.0, 0.0))
    distributionCollection.add(Normal(0.0, 1.0))
    distributionCollection.add(Rayleigh(1.0))
    distributionCollection.add(Student(22))
    distributionCollection.add(Triangular(-1.0, 0.3, 1.0))
    distributionCollection.add(Uniform(-1.0, 1.0))
    distributionCollection.add(Weibull(1.0, 3.0))
    for n in range(distributionCollection.getSize()):
        distribution = distributionCollection[n]
        name = distribution.getImplementation().getClassName()
        polynomialFactory = StandardDistributionPolynomialFactory(distribution)
        print("polynomialFactory(", name, "=", polynomialFactory, ")")
        for i in range(iMax):
            print(name, " polynomial(", i, ")=", polynomialFactory.build(i))
        roots = polynomialFactory.getRoots(iMax - 1)
        print(name, " polynomial(", iMax - 1, ") roots=", roots)
        nodes, weights = polynomialFactory.getNodesAndWeights(iMax - 1)
        print(name, " polynomial(", iMax - 1, ") nodes=",
              nodes, " and weights=", weights)

except:
    import sys
    print("t_StandardDistributionPolynomialFactory_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
