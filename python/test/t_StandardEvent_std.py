#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    # We create a numerical math function */
    myFunction = SymbolicFunction(
        ["E", "F", "L", "I"], ["-F*L^3/(3*E*I)"])

    dim = myFunction.getInputDimension()

    # We create a normal distribution point of dimension dim
    myDistribution = Normal(dim)

    # We create a 'usual' RandomVector from the Distribution
    vect = RandomVector(myDistribution)

    # We create a composite random vector
    output = CompositeRandomVector(myFunction, vect)

    # We create an StandardEvent from this RandomVector
    myStandardEvent = StandardEvent(output, Less(), 1.0)
    print("myStandardEvent=", myStandardEvent)

    # We compute one realization of the event
    # E = (Y=f(X), operator, threshold)
    # E as a RandomVector : Y
    print("myStandardEvent (as a RandomVector) realization =",
          repr(RandomVector.getRealization(myStandardEvent)))

    # E as a Bernoulli
    print("myStandardEvent realization=", repr(
        myStandardEvent.getRealization()))

    # We compute a sample of the event
    print("myStandardEvent sample=", repr(myStandardEvent.getSample(10)))

    # Build a standard event based on an event

    R = CorrelationMatrix(dim)
    for i in range(dim - 1):
        R[i + 1, i] = 0.5

    mean = Point(dim, 0.0)
    sigma = Point(dim, 1.0)
    myDistribution2 = Normal(mean, sigma, R)

    # We create a 'usual' RandomVector from the Distribution
    vect2 = RandomVector(myDistribution2)

    # We create a composite random vector
    output2 = CompositeRandomVector(myFunction, vect2)

    # We create an Event from this RandomVector */
    myEvent = ThresholdEvent(output2, Less(), 1.0)

    # Create a StandardEvent based on this Event */
    stdEvent = StandardEvent(myEvent)

    # Check if the StandardEvent is really a StandardEvent */
    # Get a sample from the second antecedent of the standard event */
    size = 2000
    # Check if the failure probabilities are the same */
    print("Failure probability (Event)=%.6f" %
          myEvent.getSample(size).computeMean()[0])
    print("Failure probability (StandardEvent)=%.6f" %
          stdEvent.getSample(size).computeMean()[0])

    x = [[1.0, 0.5, 1.0, 0.5], [2.0, 1.0, 2.0, 1.0], [3.0, 1.5, 3.0, 1.5]]
    p = [0.3, 0.325, 0.375]
    myDistribution3 = UserDefined(x, p)

    # We create a 'usual' RandomVector from the Distribution */
    vect3 = RandomVector(myDistribution3)

    # We create a composite random vector */
    output3 = CompositeRandomVector(myFunction, vect3)

    # We try to create a StandardEvent from this RandomVector */
    try:
        myStandardEvent3 = StandardEvent(output3, Less(), 1.0)
    except:
        print("Error trying to build myStandardEvent3")

except:
    import sys
    print("t_StandardEvent_std.py", sys.exc_info()[0], sys.exc_info()[1])
