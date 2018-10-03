#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)


class RVEC(PythonRandomVector):

    def __init__(self):
        super(RVEC, self).__init__(2)
        self.setDescription(['R', 'S'])

    def getRealization(self):
        X = [RandomGenerator.Generate(), 2 + RandomGenerator.Generate()]
        return X

    def getSample(self, size):
        X = []
        for i in range(size):
            X.append(
                [RandomGenerator.Generate(), 2 + RandomGenerator.Generate()])
        return X

    def getMean(self):
        return [0.5, 2.5]

    def getCovariance(self):
        return [[0.0833333, 0.], [0., 0.0833333]]


R = RVEC()
print(R)

try:
    # Instance creation
    myRV = RandomVector(R)
    print("myRV=", repr(myRV))

    # Copy constructor
    newRV = RandomVector(myRV)

    # Dimension
    dim = myRV.getDimension()
    print('dimension=', dim)

    # Realization
    X = myRV.getRealization()
    print('realization=', X)

    # Sample
    X = myRV.getSample(5)
    print('sample=', X)

    # Mean
    mean = myRV.getMean()
    print('mean=', mean)

    # Covariance
    covariance = myRV.getCovariance()
    print('covariance=', covariance)

except:
    import sys
    print("t_RandomVector_python.py", sys.exc_info()[0], sys.exc_info()[1])
