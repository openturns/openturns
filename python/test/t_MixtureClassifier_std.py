#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Create a collection of distribution
    aCollection = DistributionCollection()
    aCollection.add(Normal(0., 4))
    aCollection.add(Uniform(5., 7.))
    aCollection.add(Triangular(7., 8., 9.))

    # Instanciate one distribution object
    distribution = Mixture(
        aCollection, Point(aCollection.getSize(), 1.0))
    print("mixture=",  distribution)
    classifier = Classifier(MixtureClassifier(distribution))
    inS = Sample()
    inS.add(Point(1, 2.))
    inS.add(Point(1, 4.))
    inS.add(Point(1, 6.))
    inS.add(Point(1, 8.))

    for i in range(inS.getSize()):
        print("inP=",  inS[i], " class=",  classifier.classify(inS[i]))

    print("classes=", classifier.classify(inS))

    for i in range(inS.getSize()):
        for j in range(aCollection.getSize()):
            print("inP=",  inS[i],  " grade|", j, "= %g" %
                  classifier.grade(inS[i], j))

    for j in range(aCollection.getSize()):
        print("grades|", j, "=",  classifier.grade(
            inS, Indices(inS.getSize(), j)))

except:
    import sys
    print("t_MixtureClassifier_std.py", sys.exc_info()[0], sys.exc_info()[1])
