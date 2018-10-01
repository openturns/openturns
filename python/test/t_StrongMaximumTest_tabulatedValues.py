#! /usr/bin/env python

from __future__ import print_function
from openturns import *

from math import *

TESTPREAMBLE()

try:

    for dim in [2, 5, 10]:
        print("dimension = ", dim)

        # We create a numerical math function
        inputVar = ['X' + str(i) for i in range(dim)]
        myFunction = SymbolicFunction(inputVar, ['0'])

        # We create a normal distribution point of dimension 1
        mean = [0.] * dim
        sigma = [1.0] * dim
        R = IdentityMatrix(dim)
        myDistribution = Normal(mean, sigma, R)

        # We create a 'usual' RandomVector from the Distribution
        vect = RandomVector(myDistribution)

        # We create a composite random vector
        output = CompositeRandomVector(myFunction, vect)

        # We create a StandardEvent from this RandomVector
        myStandardEvent = StandardEvent(
            output, Less(), 2.0)

        std = Normal()

        beta = Point(3)
        beta[0] = round(-std.computeQuantile(1e-3)[0])
        beta[1] = round(-std.computeQuantile(1e-5)[0])
        beta[2] = round(-std.computeQuantile(1e-7)[0])

        importanceLevel = Point(3)
        importanceLevel[0] = 0.01
        importanceLevel[1] = 0.05
        importanceLevel[2] = 0.10

        accuracyLevel = Point(3)
        accuracyLevel[0] = 1.5
        accuracyLevel[1] = 2.0
        accuracyLevel[2] = 4.0

        confidenceLevel = Point(3)
        confidenceLevel[0] = 0.90
        confidenceLevel[1] = 0.95
        confidenceLevel[2] = 0.99

        pointNumber = UnsignedIntegerCollection(3)
        pointNumber[0] = 10
        pointNumber[1] = 100
        pointNumber[2] = 1000

        # TABLE 1 : we impose beta, the importance level, the accuracy level,
        # tne confidence level and we calculate the corresponding deltaEpsilon
        # and pointNumber N
        print("beta ", "importanceLevel ", "accuracyLevel ",
              "confidenceLevel ", "deltaEpsilon ", "pointNumber")

        # loop on beta
        for indexBeta in range(beta.getDimension()):

            # We create the design point
            designPoint = Point(dim, 0.0)
            designPoint[0] = beta[indexBeta]

            # loop on the importance level epsilon
            for indexImportanceLevel in range(importanceLevel.getDimension()):

                # loop on the accuracy level tau
                for indexAccuracyLevel in range(accuracyLevel.getDimension()):

                    # loop on the confidence level (1-q)
                    for indexConfidenceLevel in range(confidenceLevel.getDimension()):

                        # we calculate the corresponding deltaEpsilon and
                        # pointNumber N
                        myTest = StrongMaximumTest(myStandardEvent, designPoint, importanceLevel[
                                                   indexImportanceLevel], accuracyLevel[indexAccuracyLevel], confidenceLevel[indexConfidenceLevel])
                        print("%.6f" % beta[indexBeta], " %.6f" % importanceLevel[indexImportanceLevel], " %.6f" % accuracyLevel[
                              indexAccuracyLevel],  " %.6f" % confidenceLevel[indexConfidenceLevel], " %.6f" % myTest.getDeltaEpsilon(), " %.6f" % myTest.getPointNumber())

                        # TABLE 2 : we impose beta, the importance level, the accuracy level, the pointNumber N  and we calculate the corresponding deltaEpsilon and confidence level
                        #       std::cout << std::right
                        #                 << std::setw(10) << "beta "
                        #                 << std::setw(16) << "importanceLevel "
                        #                 <<  "accuracyLevel " << "pointNumber " << "deltaEpsilon " << "confidenceLevel" << std::endl
        print("beta ",  "importanceLevel ",  "accuracyLevel ",
              "pointNumber", "deltaEpsilon ", "confidenceLevel")

        # loop on beta
        for indexBeta in range(beta.getDimension()):

            # We create the design point
            designPoint = Point(dim, 0.0)
            designPoint[0] = beta[indexBeta]

            # loop on the importance level epsilon
            for indexImportanceLevel in range(importanceLevel.getDimension()):

                # loop on the accuracy level tau
                for indexAccuracyLevel in range(accuracyLevel.getDimension()):

                    # loop on the pointNumber N
                    for indexPointNumber in range(pointNumber.getSize()):

                        # we calculate the corresponding deltaEpsilon and
                        # confidenceLevel
                        myTest = StrongMaximumTest(myStandardEvent, designPoint, importanceLevel[
                                                   indexImportanceLevel], accuracyLevel[indexAccuracyLevel], pointNumber[indexPointNumber])
                        print("%.6f" % beta[indexBeta], " %.6f" % importanceLevel[indexImportanceLevel], " %.6f" % accuracyLevel[
                              indexAccuracyLevel], " %.6f" % pointNumber[indexPointNumber], " %.6f" % myTest.getDeltaEpsilon(), " %.6f" % myTest.getConfidenceLevel())

except:
    import sys
    print("t_strongMaximumTest_tabulatedValues.py",
          sys.exc_info()[0], sys.exc_info()[1])
