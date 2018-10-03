#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)


def cleanSymmetricTensor(inSymmetricTensor):
    rowDim = inSymmetricTensor.getNbRows()
    colDim = inSymmetricTensor.getNbColumns()
    sheetDim = inSymmetricTensor.getNbSheets()
    for i in range(rowDim):
        for j in range(colDim):
            for k in range(sheetDim):
                inSymmetricTensor[i, j, k] = 1.e-4 * round(
                    1.e4 * inSymmetricTensor[i, j, k])
                if (abs(inSymmetricTensor[i, j, k]) < 1.e-6):
                    inSymmetricTensor[i, j, k] = 0.0
    return inSymmetricTensor


try:
    # Create a collection of distribution
    aCollection = DistributionCollection()

    aCollection.add(Uniform(-1.0, 2.0))
    aCollection.add(Gamma(2.0, 2.0, 0.0))

    dim = aCollection.getSize()

    # Create a copula
    RCopula = CorrelationMatrix(dim)
    for i in range(dim):
        for j in range(i):
            RCopula[i, j] = (i + j + 1.0) / (2.0 * dim)

    # Instanciate one distribution object
    distribution = ComposedDistribution(aCollection, NormalCopula(RCopula))
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
    print("inverse transform gradient at transformed point     =", repr(
        inverseTransform.gradient(transformedPoint).clean(1e-6)))
    print("inverse transform gradient at transformed point (FD)=", repr(CenteredFiniteDifferenceGradient(
        1.0e-5, inverseTransform.getEvaluation()).gradient(transformedPoint).clean(1e-6)))
    print("inverse transform hessian at transformed point      =", repr(
        cleanSymmetricTensor(inverseTransform.hessian(transformedPoint))))
    print("inverse transform hessian at transformed point (FD) =", repr(cleanSymmetricTensor(
        CenteredFiniteDifferenceHessian(1.0e-4, inverseTransform.getEvaluation()).hessian(transformedPoint))))

    # Test for parameters
    print("parameters gradient at point=", repr(
        transform.parameterGradient(point)))

    # Validation using finite difference
    eps = 1e-5
    factor = 1.0 / (2.0 * eps)
    gradient = Matrix(5, 2)

    # dT/dp0
    coll = DistributionCollection(dim)
    coll[0] = Uniform(-1.0 + eps, 2.0)
    coll[1] = aCollection[1]
    left = ComposedDistribution(
        coll, NormalCopula(RCopula)).getIsoProbabilisticTransformation()
    coll[0] = Uniform(-1.0 - eps, 2.0)
    right = ComposedDistribution(
        coll, NormalCopula(RCopula)).getIsoProbabilisticTransformation()
    dTdp = (left(point) - right(point)) * factor
    gradient[0, 0] = dTdp[0]
    gradient[0, 1] = dTdp[1]
    # dT/dp1
    coll = DistributionCollection(dim)
    coll[0] = Uniform(-1.0, 2.0 + eps)
    coll[1] = aCollection[1]
    left = ComposedDistribution(
        coll, NormalCopula(RCopula)).getIsoProbabilisticTransformation()
    coll[0] = Uniform(-1.0, 2.0 - eps)
    right = ComposedDistribution(
        coll, NormalCopula(RCopula)).getIsoProbabilisticTransformation()
    dTdp = (left(point) - right(point)) * factor
    gradient[1, 0] = dTdp[0]
    gradient[1, 1] = dTdp[1]
    # dT/dp2
    coll = DistributionCollection(dim)
    coll[0] = aCollection[0]
    coll[1] = Gamma(2.0 + eps, 2.0, 0.0)
    left = ComposedDistribution(
        coll, NormalCopula(RCopula)).getIsoProbabilisticTransformation()
    coll[1] = Gamma(2.0 - eps, 2.0, 0.0)
    right = ComposedDistribution(
        coll, NormalCopula(RCopula)).getIsoProbabilisticTransformation()
    dTdp = (left(point) - right(point)) * factor
    gradient[2, 0] = dTdp[0]
    gradient[2, 1] = dTdp[1]
    # dT/dp3
    coll = DistributionCollection(dim)
    coll[0] = aCollection[0]
    coll[1] = Gamma(2.0, 2.0 + eps, 0.0)
    left = ComposedDistribution(
        coll, NormalCopula(RCopula)).getIsoProbabilisticTransformation()
    coll[1] = Gamma(2.0, 2.0 - eps, 0.0)
    right = ComposedDistribution(
        coll, NormalCopula(RCopula)).getIsoProbabilisticTransformation()
    dTdp = (left(point) - right(point)) * factor
    gradient[3, 0] = dTdp[0]
    gradient[3, 1] = dTdp[1]
    # dT/dp4
    coll = DistributionCollection(dim)
    coll[0] = aCollection[0]
    coll[1] = Gamma(2.0, 2.0, 0.0 + eps)
    left = ComposedDistribution(
        coll, NormalCopula(RCopula)).getIsoProbabilisticTransformation()
    coll[1] = Gamma(2.0, 2.0, 0.0 - eps)
    right = ComposedDistribution(
        coll, NormalCopula(RCopula)).getIsoProbabilisticTransformation()
    dTdp = (left(point) - right(point)) * factor
    gradient[4, 0] = dTdp[0]
    gradient[4, 1] = dTdp[1]

    print("parameters gradient (FD)    =", repr(gradient))

    # Test for parameters
    print("(inverse) parameters gradient at point=", repr(
        inverseTransform.parameterGradient(point)))

    # dT/dp0
    coll = DistributionCollection(dim)
    coll[0] = Uniform(-1.0 + eps, 2.0)
    coll[1] = aCollection[1]
    left = ComposedDistribution(
        coll, NormalCopula(RCopula)).getInverseIsoProbabilisticTransformation()
    coll[0] = Uniform(-1.0 - eps, 2.0)
    right = ComposedDistribution(
        coll, NormalCopula(RCopula)).getInverseIsoProbabilisticTransformation()
    dTdp = (left(point) - right(point)) * factor
    gradient[0, 0] = dTdp[0]
    gradient[0, 1] = dTdp[1]
    # dT/dp1
    coll = DistributionCollection(dim)
    coll[0] = Uniform(-1.0, 2.0 + eps)
    coll[1] = aCollection[1]
    left = ComposedDistribution(
        coll, NormalCopula(RCopula)).getInverseIsoProbabilisticTransformation()
    coll[0] = Uniform(-1.0, 2.0 - eps)
    right = ComposedDistribution(
        coll, NormalCopula(RCopula)).getInverseIsoProbabilisticTransformation()
    dTdp = (left(point) - right(point)) * factor
    gradient[1, 0] = dTdp[0]
    gradient[1, 1] = dTdp[1]
    # dT/dp2
    coll = DistributionCollection(dim)
    coll[0] = aCollection[0]
    coll[1] = Gamma(2.0 + eps, 2.0, 0.0)
    left = ComposedDistribution(
        coll, NormalCopula(RCopula)).getInverseIsoProbabilisticTransformation()
    coll[1] = Gamma(2.0 - eps, 2.0, 0.0)
    right = ComposedDistribution(
        coll, NormalCopula(RCopula)).getInverseIsoProbabilisticTransformation()
    dTdp = (left(point) - right(point)) * factor
    gradient[2, 0] = dTdp[0]
    gradient[2, 1] = dTdp[1]
    # dT/dp3
    coll = DistributionCollection(dim)
    coll[0] = aCollection[0]
    coll[1] = Gamma(2.0, 2.0 + eps, 0.0)
    left = ComposedDistribution(
        coll, NormalCopula(RCopula)).getInverseIsoProbabilisticTransformation()
    coll[1] = Gamma(2.0, 2.0 - eps, 0.0)
    right = ComposedDistribution(
        coll, NormalCopula(RCopula)).getInverseIsoProbabilisticTransformation()
    dTdp = (left(point) - right(point)) * factor
    gradient[3, 0] = dTdp[0]
    gradient[3, 1] = dTdp[1]
    # dT/dp4
    coll = DistributionCollection(dim)
    coll[0] = aCollection[0]
    coll[1] = Gamma(2.0, 2.0, 0.0 + eps)
    left = ComposedDistribution(
        coll, NormalCopula(RCopula)).getInverseIsoProbabilisticTransformation()
    coll[1] = Gamma(2.0, 2.0, 0.0 - eps)
    right = ComposedDistribution(
        coll, NormalCopula(RCopula)).getInverseIsoProbabilisticTransformation()
    dTdp = (left(point) - right(point)) * factor
    gradient[4, 0] = dTdp[0]
    gradient[4, 1] = dTdp[1]

    print("(inverse) parameters gradient (FD)    =", repr(gradient))

except:
    import sys
    print("t_IsoProbabilisticTransformation_EllipticalCopula.py",
          sys.exc_info()[0], sys.exc_info()[1])
