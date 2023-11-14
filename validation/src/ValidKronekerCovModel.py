#! /usr/bin/env python

"""
 This validation purpose is to check that the operator() yields a kronecker product
 between a fixed covariance and embedded kernel (rho) evaluation
 We make comparison point by point.
 For discretize operator, we check hin unit tests that everything is ok 
"""

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)


def convert_hmat_to_matrix(hmat):
    res = ot.Matrix(hmat.getNbRows(), hmat.getNbColumns())
    for i in range(hmat.getNbRows()):
        x = ot.Point(hmat.getNbColumns())
        x[i] = 1.0
        y = ot.Point(hmat.getNbRows())
        hmat.gemv('N', 1.0, x, 0.0, y)
        for j in range(hmat.getNbColumns()):
            res[i, j] = y[j]
    return res

if __name__ == "__main__":

    models = []
    refModels =[]
    # rho correlation
    scale = [4, 5]
    rho = ot.GeneralizedExponential(scale, 1)

    # Amplitude values
    amplitude = [1, 2]
    myModel = ot.KroneckerCovarianceModel(rho, amplitude)
    models.append(myModel)

    spatialCorrelation = ot.CorrelationMatrix(2)
    spatialCorrelation[0, 1] = 0.8
    myModel = ot.KroneckerCovarianceModel(rho, amplitude, spatialCorrelation)
    models.append(myModel)

    spatialCovariance = ot.CovarianceMatrix(2)
    spatialCovariance[0, 0] = 4.0
    spatialCovariance[1, 1] = 5.0
    spatialCovariance[1, 0] = 1.2

    myModel = ot.KroneckerCovarianceModel(rho, spatialCovariance)
    models.append(myModel)

    mesher = ot.IntervalMesher([5, 5])
    lowerbound = [-1, -1]
    upperBound = [2.0, 4.0]
    interval = ot.Interval(lowerbound, upperBound)
    mesh = mesher.build(interval)
    vertices =  mesh.getVertices()

    for k, myModel in enumerate(models):
        sigma = myModel.getAmplitude()
        correlation = myModel.getOutputCorrelation()
        covariance = ot.CovarianceMatrix(correlation.getDimension())
        for j in range(correlation.getDimension()):
            covariance[j, j] = sigma[j] * sigma[j]
            for i in range(j + 1, correlation.getDimension()):
                covariance[i, j] = sigma[i] * sigma[j] * correlation[i, j]
        covariance.checkSymmetry()
        # Assert that models are the same
        for vertex in vertices:
            value = rho.computeAsScalar(vertex)
            ott.assert_almost_equal(myModel(vertex), covariance * value, 1e-16, 1e-15)

        C = myModel.discretize(vertices)
        C.checkSymmetry()
        # HMat checks
        p = ot.HMatrixParameters()
        p.setAssemblyEpsilon(1e-16)
        p.setRecompressionEpsilon(1e-16)
        hmat = myModel.discretizeHMatrix(vertices, p)
        M = convert_hmat_to_matrix(hmat)
        ott.assert_almost_equal(C, M, 1e-16, 1e-15)
