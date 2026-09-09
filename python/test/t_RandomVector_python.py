#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


class RVEC(ot.PythonRandomVector):
    def __init__(self):
        super(RVEC, self).__init__(2)
        self.setDescription(["R", "S"])
        self._offset = 2.0

    def getRealization(self):
        X = [
            ot.RandomGenerator.Generate(),
            self._offset + ot.RandomGenerator.Generate(),
        ]
        return X

    def getSample(self, size):
        X = []
        for i in range(size):
            X.append(
                [
                    ot.RandomGenerator.Generate(),
                    self._offset + ot.RandomGenerator.Generate(),
                ]
            )
        return X

    def getMean(self):
        return [0.5, self._offset + 0.5]

    def getCovariance(self):
        return [[0.0833333, 0.0], [0.0, 0.0833333]]

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
print("dimension=", dim)

# Realization
X = myRV.getRealization()
print("realization=", X)

# Sample
X = myRV.getSample(5)
print("sample=", X)

# Mean
mean = myRV.getMean()
print("mean=", mean)

# Covariance
covariance = myRV.getCovariance()
print("covariance=", covariance)

isEvent = myRV.isEvent()
print("isEvent=", isEvent)

# Parameter description
print("description=", myRV.getParameterDescription())

# Parameter
myRV.setParameter([10.5])
print("new parameter=", myRV.getParameter())
print("new mean=", myRV.getMean())
print("new realization=", myRV.getRealization())
print("new sample=", myRV.getSample(5))

assert myRV == ot.RandomVector(myRV)


class RV2(ot.PythonRandomVector):
    def __init__(self):
        super().__init__(2)

    def getRealization(self):
        return [0.5, 0.6]

    def getDescription(self):
        return ["a", "b"]


rv2 = ot.RandomVector(RV2())
assert rv2.getDescription() == ["a", "b"]
ott.assert_almost_equal(rv2.getSample(2), [[0.5, 0.6]] * 2)
with ott.assert_raises(Exception):
    rv2.getMean()
with ott.assert_raises(Exception):
    rv2.getCovariance()
assert not rv2.isEvent()
assert rv2.getParameter() == []
assert rv2.getParameterDescription() == []
rv2.setParameter([1.0])
assert rv2 == ot.RandomVector(rv2)


class RVFull(ot.PythonRandomVector):
    def __init__(self):
        super().__init__(2)
        self.setDescription(["R", "S"])
        self._offset = 2.0

    def getRealization(self):
        return [0.5, 0.6]

    def getSample(self, size):
        return [[0.5, 0.6]] * size

    def getMean(self):
        return [0.5, 0.6]

    def getCovariance(self):
        return [[0.1, 0.0], [0.0, 0.1]]

    def isEvent(self):
        return True

    def getParameter(self):
        return [2.0]

    def getParameterDescription(self):
        return ["offset"]

    def setParameter(self, parameter):
        self._offset = parameter[0]


rvFull = ot.RandomVector(RVFull())
ott.assert_almost_equal(rvFull.getMean(), [0.5, 0.6])
ott.assert_almost_equal(
    ot.Matrix(rvFull.getCovariance()), ot.Matrix([[0.1, 0.0], [0.0, 0.1]])
)
assert rvFull.isEvent()
assert rvFull.getParameter() == [2.0]
assert rvFull.getParameterDescription() == ["offset"]


class RVNoRealization(ot.PythonRandomVector):
    def __init__(self):
        super().__init__(2)


with ott.assert_raises(Exception):
    ot.RandomVector(RVNoRealization())


class RVBadRealization(ot.PythonRandomVector):
    def __init__(self):
        super().__init__(2)

    def getRealization(self):
        return [0.1]


with ott.assert_raises(Exception):
    ot.RandomVector(RVBadRealization()).getRealization()


class RVBadSample(ot.PythonRandomVector):
    def __init__(self):
        super().__init__(2)

    def getRealization(self):
        return [0.1, 0.2]

    def getSample(self, size):
        return [[0.1, 0.2]] * (size + 1)


with ott.assert_raises(Exception):
    ot.RandomVector(RVBadSample()).getSample(2)


class RVBadMean(ot.PythonRandomVector):
    def __init__(self):
        super().__init__(2)

    def getRealization(self):
        return [0.1, 0.2]

    def getMean(self):
        return [0.1]


with ott.assert_raises(Exception):
    ot.RandomVector(RVBadMean()).getMean()


class RVBadCovariance(ot.PythonRandomVector):
    def __init__(self):
        super().__init__(2)

    def getRealization(self):
        return [0.1, 0.2]

    def getCovariance(self):
        return [[0.1]]


with ott.assert_raises(Exception):
    ot.RandomVector(RVBadCovariance()).getCovariance()
