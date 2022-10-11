#! /usr/bin/env python

import openturns as ot
import math as m

ot.TESTPREAMBLE()

# Function
sampleSize = 20
levelFunction = ot.SymbolicFunction(["x1", "x2", "x3", "x4"], ["x1+2*x2-3*x3+4*x4"])
myOperator = ot.LessOrEqual()
threshold = 2.0
mySample = ot.Sample(0, levelFunction.getInputDimension())
random = 0.1

for index in range(sampleSize):
    point = ot.Point(levelFunction.getInputDimension())
    norm = 0.0
    for coordinate in range(levelFunction.getInputDimension()):
        point[coordinate] = m.sqrt(-2.0 * m.log(random))
        random = m.fmod(random + m.sqrt(2.0), 1.0)
        point[coordinate] *= m.cos(2.0 * m.atan(1.0) * random)
        norm += point[coordinate] * point[coordinate]

    for coordinate in range(levelFunction.getInputDimension()):
        point[coordinate] /= m.sqrt(norm)

    mySample.add(point)

myNearestPointChecker = ot.NearestPointChecker(
    levelFunction, myOperator, threshold, mySample
)
for index in range(sampleSize):
    print(repr(mySample[index]))
myNearestPointChecker.run()
print("myNearestPointChecker = ", myNearestPointChecker)
