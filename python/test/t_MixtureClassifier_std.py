#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

# Create a collection of distribution
aCollection = [ot.Normal(0.0, 4), ot.Uniform(5.0, 7.0), ot.Triangular(7.0, 8.0, 9.0)]

# Instantiate one distribution object
distribution = ot.Mixture(aCollection)
print("mixture=", distribution)
classifier = ot.MixtureClassifier(distribution)
inS = ot.Sample([[2.0], [4.0], [6.0], [8.0]])

for i in range(inS.getSize()):
    print("inP=", inS[i], " class=", classifier.classify(inS[i]))

print("classes=", classifier.classify(inS))

for i in range(inS.getSize()):
    for j in range(len(aCollection)):
        print("inP=", inS[i], " grade|", j, "= %g" % classifier.grade(inS[i], j))

for j in range(len(aCollection)):
    print("grades|", j, "=", classifier.grade(inS, ot.Indices(inS.getSize(), j)))
