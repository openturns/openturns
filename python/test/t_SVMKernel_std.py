#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    # Create all the kernels
    kernelCollection = SVMKernelCollection()
    kernelCollection.add(SVMKernel(NormalRBF(50.0)))
    # parameters : sigma
    # parameters : sigma
    kernelCollection.add(SVMKernel(ExponentialRBF(50.0)))
    # parameters : degree, linear, constant
    kernelCollection.add(SVMKernel(PolynomialKernel(3.0, 2.0, 1.0)))
    # parameters : constant
    kernelCollection.add(SVMKernel(RationalKernel(1.0)))
    numberOfKernel = kernelCollection.getSize()

    dimension = 6

    # Create two points
    x1 = NumericalPoint(dimension)
    x1[0] = 4.0
    x1[1] = 8.0
    x1[2] = 15.0
    x1[3] = 16.0
    x1[4] = 23.0
    x1[5] = 42.0
    x2 = NumericalPoint(dimension)
    x2[0] = 0.1
    x2[1] = 0.2
    x2[2] = 0.3
    x2[3] = 0.4
    x2[4] = 0.5
    x2[5] = 0.6

    # Stream out the first point
    print(repr(x1))

    # for each kernel
    for i in range(numberOfKernel):
        # Stream out the kernel
        print(kernelCollection[i])

        # Retrieve the parameters
        parameters = kernelCollection[i].getParameters()
        print("parameters=", repr(parameters))

        # Evaluate the kernel
        kernelEvaluation = kernelCollection[i](x1, x2)
        kernelGradient = kernelCollection[i].partialGradient(x1, x2)
        kernelHessian = kernelCollection[i].partialHessian(x1, x2)

        # Stream out the result
        print("evaluation=", kernelEvaluation)
        print("gradient=", repr(kernelGradient))
        print("hessian=", repr(kernelHessian))

except:
    import sys
    print("t_SVMKernel.py", sys.exc_info()[0], sys.exc_info()[1])
