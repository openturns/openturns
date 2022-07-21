#! /usr/bin/env python
# -*- coding: utf-8 -*-
"""
@author: Antoine Dumas (Phiméca)

Calcul de la variance des estimateurs de Sobol d'openTURNS
"""


import openturns as ot
import numpy as np


def computeSobolVariance(U, psi, size):
    """
    Compute the variance of the estimators

    Parameters
    ----------
    U : sample
        The sample of yA, yB, yE or combination of them, defined according the
        sobol estimators
    psi : Function
        The function that computes the sobol estimates.
    size : int
        The size of the sample.
    """
    mean_psi = psi.gradient(U.computeMean()) * ot.Point(
        1, 1
    )  # to transform into a Point
    variance = mean_psi.dot(U.computeCovariance() * mean_psi) / size
    return variance


class SaltelliSensitivityAlgorithm(ot.SaltelliSensitivityAlgorithm):
    def __init__(self, inputDesign, outputDesign, N):
        super(SaltelliSensitivityAlgorithm, self).__init__(inputDesign, outputDesign, N)
        self.inputDesign = inputDesign
        self.input_dim = inputDesign.getDimension()
        self.output_dim = outputDesign.getDimension()
        self.size = N
        # centrage de l'échantillon de sortie
        self.outputDesign = outputDesign  # - outputDesign.computeMean()[0]

    def computeVariance(self):

        x = ot.Description.BuildDefault(self.output_dim, "X")
        y = ot.Description.BuildDefault(self.output_dim, "Y")
        # in order X0, Y0, X1, Y1, ...
        xy = list(x)
        for i, yy in enumerate(y):
            xy.insert(2 * i + 1, yy)
        # psi  = (x1 + x2 + ...) / (y1 + y2 + ...).
        symbolic_num = ""
        symbolic_denom = ""
        for i in range(self.output_dim):
            symbolic_num += x[i]
            symbolic_denom += y[i]
            if i < self.output_dim - 1:
                symbolic_num += "+"
                symbolic_denom += "+"
        psi_fo = ot.SymbolicFunction(
            xy, ["(" + symbolic_num + ")/(" + symbolic_denom + ")"]
        )
        psi_to = ot.SymbolicFunction(
            xy, ["1 - " + "(" + symbolic_num + ")/(" + symbolic_denom + ")"]
        )

        varianceFO = ot.Point(self.input_dim)
        varianceTO = ot.Point(self.input_dim)
        for p in range(self.input_dim):
            U_fo = ot.Sample(self.size, 0)
            U_to = ot.Sample(self.size, 0)
            for q in range(self.output_dim):

                yA = ot.Sample(self.outputDesign[:, q], 0, self.size)
                yB = ot.Sample(self.outputDesign[:, q], self.size, 2 * self.size)
                yAc = yA - yA.computeMean()[0]
                yBc = yB - yB.computeMean()[0]
                yE = ot.Sample(
                    self.outputDesign[:, q], (2 + p) * self.size, (3 + p) * self.size
                )
                yEc = yE - yE.computeMean()[0]

                # first order
                U_fo.stack(np.array(yBc) * np.array(yEc))
                U_fo.stack(np.array(yAc) ** 2)  # centré dans tous les cas ici

                # total order
                U_to.stack(np.array(yAc) * np.array(yEc))
                U_to.stack(np.array(yAc) ** 2)  # centré dans tous les cas ici

            varianceFO[p] = computeSobolVariance(U_fo, psi_fo, self.size)
            varianceTO[p] = computeSobolVariance(U_to, psi_to, self.size)

        return varianceFO, varianceTO

    def getFirstOrderAsymptoticDistribution(self):
        indicesFO = self.getAggregatedFirstOrderIndices()
        varianceFO, varianceTO = self.computeVariance()
        foDist = ot.DistributionCollection(self.input_dim)
        for p in range(self.input_dim):
            foDist[p] = ot.Normal(indicesFO[p], np.sqrt(varianceFO[p]))
        return foDist

    def getTotalOrderAsymptoticDistribution(self):
        indicesTO = self.getAggregatedTotalOrderIndices()
        varianceFO, varianceTO = self.computeVariance()
        toDist = ot.DistributionCollection(self.input_dim)
        for p in range(self.input_dim):
            toDist[p] = ot.Normal(indicesTO[p], np.sqrt(varianceTO[p]))
        return toDist

    def getAggregatedFirstOrderIndices(self):

        sumVariance = 0
        VarianceI = ot.Point(self.input_dim)
        for q in range(self.output_dim):
            yA = ot.Sample(self.outputDesign[:, q], 0, self.size)
            yAc = yA - yA.computeMean()[0]
            yB = ot.Sample(self.outputDesign[:, q], self.size, 2 * self.size)
            yBc = yB - yB.computeMean()[0]
            sumVariance += yA.computeVariance()[0]

            # FOindices = ot.Point(self.input_dim)
            for p in range(self.input_dim):
                yE = ot.Sample(
                    self.outputDesign[:, q], (2 + p) * self.size, (3 + p) * self.size
                )
                yEc = yE - yE.computeMean()[0]

                xc = np.array(yBc) * np.array(yEc)
                VarianceI[p] += np.mean(xc) - np.mean(yAc) * np.mean(yBc)

        FOindices = ot.Point(VarianceI / sumVariance)
        return FOindices


class JansenSensitivityAlgorithm(ot.JansenSensitivityAlgorithm):
    def __init__(self, inputDesign, outputDesign, N):
        super(JansenSensitivityAlgorithm, self).__init__(inputDesign, outputDesign, N)
        self.inputDesign = inputDesign
        self.input_dim = inputDesign.getDimension()
        self.output_dim = outputDesign.getDimension()
        self.size = N
        # centrage de l'échantillon de sortie
        self.outputDesign = outputDesign  # - outputDesign.computeMean()[0]

    def computeVariance(self):

        x = ot.Description.BuildDefault(self.output_dim, "X")
        y = ot.Description.BuildDefault(self.output_dim, "Y")
        # in order X0, Y0, X1, Y1, ...
        xy = list(x)
        for i, yy in enumerate(y):
            xy.insert(2 * i + 1, yy)
        # psi  = 0.5*(x1 + x2 + ...) / (y1 + y2 + ...).
        symbolic_num = ""
        symbolic_denom = ""
        for i in range(self.output_dim):
            symbolic_num += x[i]
            symbolic_denom += y[i]
            if i < self.output_dim - 1:
                symbolic_num += "+"
                symbolic_denom += "+"
        psi_fo = ot.SymbolicFunction(
            xy, ["1 - " + "0.5*(" + symbolic_num + ")/(" + symbolic_denom + ")"]
        )
        psi_to = ot.SymbolicFunction(
            xy, ["0.5*(" + symbolic_num + ")/(" + symbolic_denom + ")"]
        )

        varianceFO = ot.Point(self.input_dim)
        varianceTO = ot.Point(self.input_dim)
        for p in range(self.input_dim):
            U_fo = ot.Sample(self.size, 0)
            U_to = ot.Sample(self.size, 0)

            for q in range(self.output_dim):
                yA = ot.Sample(self.outputDesign[:, q], 0, self.size)
                yB = ot.Sample(self.outputDesign[:, q], self.size, 2 * self.size)
                yAc = yA - yA.computeMean()[0]
                yBc = yB - yB.computeMean()[0]
                yE = ot.Sample(
                    self.outputDesign[:, q], (2 + p) * self.size, (3 + p) * self.size
                )
                yEc = yE - yE.computeMean()[0]

                # First order
                U_fo.stack(np.array(yEc - yBc) ** 2)
                U_fo.stack(np.array(yAc) ** 2)

                # Total order
                U_to.stack(np.array(yAc - yEc) ** 2)
                U_to.stack(np.array(yAc) ** 2)

            varianceFO[p] = computeSobolVariance(U_fo, psi_fo, self.size)
            varianceTO[p] = computeSobolVariance(U_to, psi_to, self.size)

        return varianceFO, varianceTO

    def getFirstOrderAsymptoticDistribution(self):
        indicesFO = self.getAggregatedFirstOrderIndices()
        varianceFO, varianceTO = self.computeVariance()
        foDist = ot.DistributionCollection(self.input_dim)
        for p in range(self.input_dim):
            foDist[p] = ot.Normal(indicesFO[p], np.sqrt(varianceFO[p]))
        return foDist

    def getTotalOrderAsymptoticDistribution(self):
        indicesTO = self.getAggregatedTotalOrderIndices()
        varianceFO, varianceTO = self.computeVariance()
        toDist = ot.DistributionCollection(self.input_dim)
        for p in range(self.input_dim):
            toDist[p] = ot.Normal(indicesTO[p], np.sqrt(varianceTO[p]))
        return toDist


class MauntzKucherenkoSensitivityAlgorithm(ot.MauntzKucherenkoSensitivityAlgorithm):
    def __init__(self, inputDesign, outputDesign, N):
        super(MauntzKucherenkoSensitivityAlgorithm, self).__init__(
            inputDesign, outputDesign, N
        )
        self.inputDesign = inputDesign
        self.input_dim = inputDesign.getDimension()
        self.output_dim = outputDesign.getDimension()
        self.size = N
        # centrage de l'échantillon de sortie
        self.outputDesign = outputDesign  # - outputDesign.computeMean()[0]

    def computeVariance(self):

        x = ot.Description.BuildDefault(self.output_dim, "X")
        y = ot.Description.BuildDefault(self.output_dim, "Y")
        # in order X0, Y0, X1, Y1, ...
        xy = list(x)
        for i, yy in enumerate(y):
            xy.insert(2 * i + 1, yy)
        # psi  = (x1 + x2 + ...) / (y1 + y2 + ...).
        symbolic_num = ""
        symbolic_denom = ""
        for i in range(self.output_dim):
            symbolic_num += x[i]
            symbolic_denom += y[i]
            if i < self.output_dim - 1:
                symbolic_num += "+"
                symbolic_denom += "+"
        psi_fo = ot.SymbolicFunction(
            xy, ["(" + symbolic_num + ")/(" + symbolic_denom + ")"]
        )
        psi_to = ot.SymbolicFunction(
            xy, ["1 - " + "(" + symbolic_num + ")/(" + symbolic_denom + ")"]
        )

        varianceFO = ot.Point(self.input_dim)
        varianceTO = ot.Point(self.input_dim)
        for p in range(self.input_dim):
            U_fo = ot.Sample(self.size, 0)
            U_to = ot.Sample(self.size, 0)

            for q in range(self.output_dim):
                yA = ot.Sample(self.outputDesign[:, q], 0, self.size)
                yB = ot.Sample(self.outputDesign[:, q], self.size, 2 * self.size)
                yAc = yA - yA.computeMean()[0]
                yBc = yB - yB.computeMean()[0]
                yE = ot.Sample(
                    self.outputDesign[:, q], (2 + p) * self.size, (3 + p) * self.size
                )
                yEc = yE - yE.computeMean()[0]

                # First order
                U_fo.stack(np.array(yBc) * np.array(yEc - yAc))
                U_fo.stack(np.array(yAc) ** 2)
                # psi_fo = ot.SymbolicFunction(['x', 'y'], ['x / y'])

                # Total order
                U_to.stack(np.array(yAc) * np.array(yEc - yAc))
                U_to.stack(np.array(yAc) ** 2)
                # psi_to = ot.SymbolicFunction(['x', 'y'], ['1 - x / y'])

            varianceFO[p] += computeSobolVariance(U_fo, psi_fo, self.size)
            varianceTO[p] += computeSobolVariance(U_to, psi_to, self.size)

        return varianceFO, varianceTO

    def getFirstOrderAsymptoticDistribution(self):
        indicesFO = self.getAggregatedFirstOrderIndices()
        varianceFO, varianceTO = self.computeVariance()
        foDist = ot.DistributionCollection(self.input_dim)
        for p in range(self.input_dim):
            foDist[p] = ot.Normal(indicesFO[p], np.sqrt(varianceFO[p]))
        return foDist

    def getTotalOrderAsymptoticDistribution(self):
        indicesTO = self.getAggregatedTotalOrderIndices()
        varianceFO, varianceTO = self.computeVariance()
        toDist = ot.DistributionCollection(self.input_dim)
        for p in range(self.input_dim):
            toDist[p] = ot.Normal(indicesTO[p], np.sqrt(varianceTO[p]))
        return toDist


class MartinezSensitivityAlgorithm(ot.MartinezSensitivityAlgorithm):
    def __init__(self, inputDesign, outputDesign, N):
        super(MartinezSensitivityAlgorithm, self).__init__(inputDesign, outputDesign, N)
        self.inputDesign = inputDesign
        self.input_dim = inputDesign.getDimension()
        self.output_dim = outputDesign.getDimension()
        self.size = N
        # centrage de l'échantillon de sortie
        self.outputDesign = outputDesign  # - outputDesign.computeMean()[0]

    def computeVariance(self):

        x = ot.Description.BuildDefault(self.output_dim, "X")
        y = ot.Description.BuildDefault(self.output_dim, "Y")
        z = ot.Description.BuildDefault(self.output_dim, "Z")
        # in order X0, Y0, Z0, X1, Y1, Z1, ...
        xyz = list(x)
        for i in range(self.output_dim):
            xyz.insert(3 * i + 1, y[i])
            xyz.insert(3 * i + 2, z[i])

        # psi_fo  = (x1 + x2 + ...) / (sqrt(y1*z1) + sqrt(y2*z2) + ...).
        symbolic_num = ""
        symbolic_denom = ""
        for i in range(self.output_dim):
            symbolic_num += x[i]
            symbolic_denom += "sqrt(" + y[i] + "*" + z[i] + ")"
            if i < self.output_dim - 1:
                symbolic_num += "+"
                symbolic_denom += "+"
        psi_fo = ot.SymbolicFunction(
            xyz, ["(" + symbolic_num + ")/(" + symbolic_denom + ")"]
        )
        psi_to = ot.SymbolicFunction(
            xyz, ["1 - " + "(" + symbolic_num + ")/(" + symbolic_denom + ")"]
        )

        varianceFO = ot.Point(self.input_dim)
        varianceTO = ot.Point(self.input_dim)
        for p in range(self.input_dim):
            U_fo = ot.Sample(self.size, 0)
            U_to = ot.Sample(self.size, 0)

            for q in range(self.output_dim):
                yA = ot.Sample(self.outputDesign[:, q], 0, self.size)
                yB = ot.Sample(self.outputDesign[:, q], self.size, 2 * self.size)
                yAc = yA - yA.computeMean()[0]
                yBc = yB - yB.computeMean()[0]
                yE = ot.Sample(
                    self.outputDesign[:, q], (2 + p) * self.size, (3 + p) * self.size
                )
                yEc = yE - yE.computeMean()[0]

                # First order
                U_fo.stack(np.array(yBc) * np.array(yEc))
                U_fo.stack(np.array(yBc) ** 2)
                U_fo.stack(np.array(yEc) ** 2)
                # psi_fo = ot.SymbolicFunction(['x', 'y', 'z'], ['x / sqrt(y*z)'])

                # Total order
                U_to.stack(np.array(yAc) * np.array(yEc))
                U_to.stack(np.array(yAc) ** 2)
                U_to.stack(np.array(yEc) ** 2)
                # psi_to = ot.SymbolicFunction(['x', 'y', 'z'], ['1 - x / sqrt(y*z)'])

            varianceFO[p] += computeSobolVariance(U_fo, psi_fo, self.size)
            varianceTO[p] += computeSobolVariance(U_to, psi_to, self.size)

        return varianceFO, varianceTO

    def getFirstOrderAsymptoticDistribution(self):
        indicesFO = self.getAggregatedFirstOrderIndices()
        varianceFO, varianceTO = self.computeVariance()
        foDist = ot.DistributionCollection(self.input_dim)
        for p in range(self.input_dim):
            foDist[p] = ot.Normal(indicesFO[p], np.sqrt(varianceFO[p]))
        return foDist

    def getTotalOrderAsymptoticDistribution(self):
        indicesTO = self.getAggregatedTotalOrderIndices()
        varianceFO, varianceTO = self.computeVariance()
        toDist = ot.DistributionCollection(self.input_dim)
        for p in range(self.input_dim):
            toDist[p] = ot.Normal(indicesTO[p], np.sqrt(varianceTO[p]))
        return toDist
