#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()


class RVEC(ot.PythonRandomVector):

    def __init__(self):
        super(RVEC, self).__init__(2)
        self.setDescription(['R', 'S'])

    def getRealization(self):
        X = [ot.RandomGenerator.Generate(), 2 + ot.RandomGenerator.Generate()]
        return X

    def getSample(self, size):
        X = []
        for i in range(size):
            X.append(
                [ot.RandomGenerator.Generate(), 2 + ot.RandomGenerator.Generate()])
        return X

    def getMean(self):
        return [0.5, 2.5]

    def getCovariance(self):
        return [[0.0833333, 0.], [0., 0.0833333]]

    def isEvent(self):
        return False


R = RVEC()
print(R)


# Instance creation
myRV = ot.RandomVector(R)
print("myRV=", repr(myRV))

# Copy constructor
newRV = ot.RandomVector(myRV)

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

isEvent = myRV.isEvent()
print('isEvent=', isEvent)
