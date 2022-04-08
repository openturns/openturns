#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


class RVEC(ot.PythonRandomVector):

    def __init__(self):
        super(RVEC, self).__init__(2)
        self.setDescription(['R', 'S'])
        self._offset = 2.0

    def getRealization(self):
        X = [ot.RandomGenerator.Generate(), self._offset +
             ot.RandomGenerator.Generate()]
        return X

    def getSample(self, size):
        X = []
        for i in range(size):
            X.append(
                [ot.RandomGenerator.Generate(), self._offset + ot.RandomGenerator.Generate()])
        return X

    def getMean(self):
        return [0.5, self._offset + 0.5]

    def getCovariance(self):
        return [[0.0833333, 0.], [0., 0.0833333]]

    def isEvent(self):
        return False

    def setParameter(self, offset_point):
        self._offset = offset_point[0]

    def getParameter(self):
        return [self._offset]

    def getParameterDescription(self):
        return ["offset"]


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

# Parameter description
print('description=', myRV.getParameterDescription())

# Parameter
myRV.setParameter([10.5])
print('new parameter=', myRV.getParameter())
print('new mean=', myRV.getMean())
print('new realization=', myRV.getRealization())
print('new sample=', myRV.getSample(5))
