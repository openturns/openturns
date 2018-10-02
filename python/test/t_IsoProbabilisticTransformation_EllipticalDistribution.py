#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()
PlatformInfo.SetNumericalPrecision(5)


def cleanSymmetricTensor(inSymmetricTensor):
    rowDim = inSymmetricTensor.getNbRows()
    colDim = inSymmetricTensor.getNbColumns()
    sheetDim = inSymmetricTensor.getNbSheets()
    for i in range(rowDim):
        for j in range(colDim):
            for k in range(sheetDim):
                if (fabs(inSymmetricTensor[i, j, k]) < 1.e-6):
                    inSymmetricTensor[i, j, k] = 0.0
    return inSymmetricTensor


try:

    # Instanciate one distribution object
    dim = 3
    meanPoint = Point(dim, 1.0)
    meanPoint[0] = 0.5
    meanPoint[1] = -0.5
    sigma = Point(dim, 1.0)
    sigma[0] = 2.0
    sigma[1] = 3.0
    R = CorrelationMatrix(dim)
    for i in range(1, dim):
        R[i, i - 1] = 0.5
    distribution = Normal(meanPoint, sigma, R)

    # Test for sampling
    size = 10000
    sample = distribution.getSample(size)
    print("sample first=", repr(sample[0]), " last=", repr(sample[size - 1]))
    print("sample mean=", repr(sample.computeMean()))
    print("sample covariance=", repr(sample.computeCovariance()))

    transform = distribution.getIsoProbabilisticTransformation()
    print("isoprobabilistic transformation=", repr(transform))
    transformedSample = transform(sample)
    print("transformed sample first=", repr(
        transformedSample[0]), " last=", repr(transformedSample[size - 1]))
    print("transformed sample mean=", repr(transformedSample.computeMean()))
    print("transformed sample covariance=", repr(
        transformedSample.computeCovariance()))

    # Test for evaluation
    inverseTransform = distribution.getInverseIsoProbabilisticTransformation()
    print("inverse isoprobabilistic transformation=", repr(inverseTransform))
    transformedBackSample = inverseTransform(transformedSample)
    print("transformed back sample first=", repr(
        transformedBackSample[0]), " last=", repr(transformedBackSample[size - 1]))
    print("transformed back sample mean=", repr(
        transformedBackSample.computeMean()))
    print("transformed back sample covariance=", repr(
        transformedBackSample.computeCovariance()))
    point = Point(dim, 1.0)
    print("point=", repr(point))
    transformedPoint = transform(point)
    print("transform value at point        =", repr(transformedPoint))
    print("transform gradient at point     =", repr(
        transform.gradient(point).clean(1e-6)))
    print("transform gradient at point (FD)=", repr(CenteredFiniteDifferenceGradient(
        1.0e-5, transform.getEvaluation()).gradient(point).clean(1e-6)))
    print("transform hessian at point      =", repr(
        cleanSymmetricTensor(transform.hessian(point))))
    print("transform hessian at point (FD) =", repr(cleanSymmetricTensor(
        CenteredFiniteDifferenceHessian(1.0e-4, transform.getEvaluation()).hessian(point))))
    print("inverse transform value at transformed point        =",
          repr(inverseTransform(transformedPoint)))
    print("inverse transform gradient at transformed point (FD)=", repr(
        inverseTransform.gradient(transformedPoint).clean(1e-6)))
    print("inverse transform gradient at transformed point     =", repr(CenteredFiniteDifferenceGradient(
        1.0e-5, inverseTransform.getEvaluation()).gradient(transformedPoint).clean(1e-6)))
    print("inverse transform hessian at transformed point      =", repr(
        cleanSymmetricTensor(inverseTransform.hessian(transformedPoint))))
    print("inverse transform hessian at transformed point (FD) =", repr(cleanSymmetricTensor(
        CenteredFiniteDifferenceHessian(1.0e-4, inverseTransform.getEvaluation()).hessian(transformedPoint))))

    # Test for parameters
    print("parameters gradient at point=", repr(
        transform.parameterGradient(point)))

except:
    import sys
    print("t_IsoProbabilisticTransformation_EllipticalDistribution.py",
          sys.exc_info()[0], sys.exc_info()[1])
