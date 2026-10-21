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
        grade = classifier.grade(inS[i], j)
        # TODO JM: remove the check after the use of infs has been thoroughly tested
        if grade <= ot.SpecFunc.LowestScalar:
            grade *= 2.0
        print("inP=", inS[i], " grade|", j, "= %g" % grade)

for j in range(len(aCollection)):
    grades = classifier.grade(inS, ot.Indices(inS.getSize(), j))
    for num, grade in enumerate(grades):
        # TODO JM: remove the check after the use of infs has been thoroughly tested
        if grade <= ot.SpecFunc.LowestScalar:
            grade *= 2.0
        grades[num] = grade
    print("grades|", j, "=", grades)


class SerialLaw(ot.PythonDistribution):
    # Python distributions are never parallel-safe

    def __init__(self):
        super().__init__(1)

    def getRange(self):
        return ot.Interval(-1.0, 1.0)

    def computeCDF(self, X):
        return 0.5


# setMixture refreshes the parallel flag from the replacement mixture
parallel_mixture = ot.Mixture([ot.Normal(), ot.Normal()])
serial_mixture = ot.Mixture([ot.Normal(), ot.Distribution(SerialLaw())])
assert parallel_mixture.isParallel()
assert not serial_mixture.isParallel()
classifier.setMixture(serial_mixture)
assert not classifier.isParallel()
classifier.setMixture(parallel_mixture)
assert classifier.isParallel()
