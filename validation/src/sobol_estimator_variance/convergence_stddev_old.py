#! /usr/bin/env python
# -*- coding: utf-8 -*-
"""
@author: Michaël Baudin (EDF), Antoine Dumas (Phiméca)
"""


from __future__ import print_function
import openturns as ot
from gsobollib import gsobolDistribution, gsobol
from ishigamilib import (
    ishigamiDistribution,
    ishigamiGSymbolic,
)
from numpy import sqrt, array
import numpy as np
import matplotlib as mpl

import pylab as pl
from sobol_variance_estimators import (
    SaltelliSensitivityAlgorithm,
    MartinezSensitivityAlgorithm,
    JansenSensitivityAlgorithm,
    MauntzKucherenkoSensitivityAlgorithm,
)

mpl.use("Agg")  # to remove to show figure
ot.Log.Show(ot.Log.ERROR)


def computeReferenceStandardDeviation(
    model, distribution, sobol_estimator, ref_sampleSize, ref_nrepetitions
):
    # # Get the asymptotic variance with 10000 sample and 1000 repetitions as reference
    # sensitivity_test = SensitivityConfidenceTest(model, distribution,
    #                                              sobol_estimator,
    #                                              sampleSize=ref_sampleSize,
    #                                              nrepetitions=ref_nrepetitions)
    # std_first_empirical = sensitivity_test.std_first_empirical
    # std_total_empirical = sensitivity_test.std_total_empirical
    # return std_first_empirical, std_total_empirical
    sobolexperiment = ot.SobolIndicesExperiment(
        distribution, int(ref_sampleSize), False
    )
    inputDesign = sobolexperiment.generate()
    outputDesign = model(inputDesign)
    sensitivity_algorithm = sobol_estimator(
        inputDesign, outputDesign, int(ref_sampleSize)
    )

    # Récupère les distributions asymptotiques
    distFirstCol = sensitivity_algorithm.getFirstOrderAsymptoticDistribution()
    distTotalCol = sensitivity_algorithm.getTotalOrderAsymptoticDistribution()
    std_first_asymptotic = ot.Point([d.getStandardDeviation()[0] for d in distFirstCol])
    std_total_asymptotic = ot.Point([d.getStandardDeviation()[0] for d in distTotalCol])
    return std_first_asymptotic, std_total_asymptotic


def computeErrors(
    model, distribution, sobol_estimator, sampleSize, ref_std_fo, ref_std_to
):

    sobolexperiment = ot.SobolIndicesExperiment(distribution, int(sampleSize), False)
    inputDesign = sobolexperiment.generate()
    outputDesign = model(inputDesign)
    sensitivity_algorithm = sobol_estimator(inputDesign, outputDesign, int(sampleSize))

    # Récupère les distributions asymptotiques
    distFirstCol = sensitivity_algorithm.getFirstOrderAsymptoticDistribution()
    distTotalCol = sensitivity_algorithm.getTotalOrderAsymptoticDistribution()
    std_first_asymptotic = ot.Point([d.getStandardDeviation()[0] for d in distFirstCol])
    std_total_asymptotic = ot.Point([d.getStandardDeviation()[0] for d in distTotalCol])

    # Compute absolute errors
    AE_vfo = np.abs(ref_std_fo - std_first_asymptotic)
    AE_vto = np.abs(ref_std_to - std_total_asymptotic)
    # compute relative errors
    RE_vfo = AE_vfo / ref_std_fo
    RE_vto = AE_vto / ref_std_to

    absErrFirst = np.max([AE_vfo])
    absErrTotal = np.max([AE_vto])
    relErrFirt = np.max([RE_vfo])
    relErrTotal = np.max([RE_vto])
    return absErrFirst, absErrTotal, relErrFirt, relErrTotal


def plotAbsoluteError(algorithmName, sampleSize, absErrorFirst, absErrorTotal):
    fig, ax = pl.subplots()
    ax.set_title(algorithmName)
    ax.plot(sampleSize, 1.0 / sqrt(sampleSize), "-", label="1/sqrt(n)")
    ax.plot(sampleSize, absErrorFirst, "o", label="First order")
    ax.plot(sampleSize, absErrorTotal, "o", label="Total order")
    ax.set_xlabel("N")
    ax.set_ylabel("Absolute error")
    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.legend()
    fig.show()
    fig.savefig(
        "Absolute Error - " + algorithmName + ".png",
        transparent=True,
        bbox_inches="tight",
    )
    return None


def plotRelativeError(
    algorithmName, sampleSize, relErrorFirst, relErrorTotal, ref_std_fo, ref_std_to
):
    fig, ax = pl.subplots()
    ax.set_title(algorithmName)
    ax.plot(
        sampleSize,
        1.0 / (np.max(ref_std_fo) * sqrt(sampleSize)),
        "-",
        label="1/(max(std_reference_fo)*sqrt(n))",
    )
    ax.plot(
        sampleSize,
        1.0 / (np.max(ref_std_to) * sqrt(sampleSize)),
        "-",
        label="1/(max(std_reference_to)*sqrt(n))",
    )
    ax.plot(sampleSize, relErrorFirst, "o", label="First order")
    ax.plot(sampleSize, relErrorTotal, "o", label="Total order")
    ax.set_xlabel("N")
    ax.set_ylabel("Relative error")
    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.legend()
    fig.show()
    fig.savefig(
        "Relative Error - " + algorithmName + ".png",
        transparent=True,
        bbox_inches="tight",
    )
    return None


def runConvergence(
    model,
    distribution,
    sobol_estimator,
    list_sampleSize,
    ref_sampleSize=10000,
    ref_nrepetitions=1000,
):

    # compute empirical reference std
    ref_std_fo, ref_std_to = computeReferenceStandardDeviation(
        model, distribution, sobol_estimator, ref_sampleSize, ref_nrepetitions
    )

    list_absErrFirst = np.zeros((len(list_sampleSize), 1))
    list_absErrTotal = np.zeros((len(list_sampleSize), 1))
    list_relErrFirst = np.zeros((len(list_sampleSize), 1))
    list_relErrTotal = np.zeros((len(list_sampleSize), 1))
    for i, sampleSize in enumerate(list_sampleSize):
        # Compute errors
        (
            list_absErrFirst[i],
            list_absErrTotal[i],
            list_relErrFirst[i],
            list_relErrTotal[i],
        ) = computeErrors(
            model, distribution, sobol_estimator, sampleSize, ref_std_fo, ref_std_to
        )

    algorithmName = "%s - %s" % (model.getName(), sobol_estimator.__name__)
    plotAbsoluteError(
        algorithmName, list_sampleSize, list_absErrFirst, list_absErrTotal
    )
    plotRelativeError(
        algorithmName,
        list_sampleSize,
        list_relErrFirst,
        list_relErrTotal,
        ref_std_fo,
        ref_std_to,
    )


if __name__ == "__main__":
    # Sizes of simulation
    n_loops = 10
    list_sampleSize = np.logspace(1, 5, n_loops)  # n_loops valeurs de 1e1 à 1e5

    ref_sampleSize = 4e5
    ref_nrepetitions = 1000

    # GSOBOL

    a = array([0, 9, 99])
    # Distribution uniforme associée au cas-test GSobol
    distribution_gsobol = gsobolDistribution(len(a))
    model_gsobol = ot.PythonFunction(len(a), 1, func_sample=lambda X: gsobol(X, a))
    model_gsobol.setName("G-Sobol")

    # Saltelli
    runConvergence(
        model_gsobol,
        distribution_gsobol,
        SaltelliSensitivityAlgorithm,
        list_sampleSize,
        ref_sampleSize,
        ref_nrepetitions,
    )

    # Jansen
    runConvergence(
        model_gsobol,
        distribution_gsobol,
        JansenSensitivityAlgorithm,
        list_sampleSize,
        ref_sampleSize,
        ref_nrepetitions,
    )

    # Mauntz
    runConvergence(
        model_gsobol,
        distribution_gsobol,
        MauntzKucherenkoSensitivityAlgorithm,
        list_sampleSize,
        ref_sampleSize,
        ref_nrepetitions,
    )

    # Martinez
    runConvergence(
        model_gsobol,
        distribution_gsobol,
        MartinezSensitivityAlgorithm,
        list_sampleSize,
        ref_sampleSize,
        ref_nrepetitions,
    )

    # ISHIGAMI

    model_ishigami = ishigamiGSymbolic()
    model_ishigami.setName("Ishigami")
    distribution_ishigami = ishigamiDistribution()

    # Saltelli
    runConvergence(
        model_ishigami,
        distribution_ishigami,
        SaltelliSensitivityAlgorithm,
        list_sampleSize,
        ref_sampleSize,
        ref_nrepetitions,
    )

    # Jansen
    runConvergence(
        model_ishigami,
        distribution_ishigami,
        JansenSensitivityAlgorithm,
        list_sampleSize,
        ref_sampleSize,
        ref_nrepetitions,
    )

    # Mauntz
    runConvergence(
        model_ishigami,
        distribution_ishigami,
        MauntzKucherenkoSensitivityAlgorithm,
        list_sampleSize,
        ref_sampleSize,
        ref_nrepetitions,
    )

    # Martinez
    runConvergence(
        model_ishigami,
        distribution_ishigami,
        MartinezSensitivityAlgorithm,
        list_sampleSize,
        ref_sampleSize,
        ref_nrepetitions,
    )

    # POUTRE

    model_poutre = ot.SymbolicFunction(
        ["L", "b", "h", "E", "F"], ["F * L^3 / (48 * E * b * h^3 / 12)"]
    )
    model_poutre.setName("poutre")
    L = ot.LogNormal()
    L.setParameter(ot.LogNormalMuSigmaOverMu()([5.0, 0.02, 0.0]))
    b = ot.LogNormal()
    b.setParameter(ot.LogNormalMuSigmaOverMu()([0.2, 0.05, 0.0]))
    h = ot.LogNormal()
    h.setParameter(ot.LogNormalMuSigmaOverMu()([0.4, 0.05, 0.0]))
    E = ot.LogNormal()
    E.setParameter(ot.LogNormalMuSigmaOverMu()([3e4, 0.12, 0.0]))
    F = ot.LogNormal()
    F.setParameter(ot.LogNormalMuSigmaOverMu()([0.1, 0.20, 0.0]))
    distribution_poutre = ot.ComposedDistribution([L, b, h, E, F])

    # Saltelli
    runConvergence(
        model_poutre,
        distribution_poutre,
        SaltelliSensitivityAlgorithm,
        list_sampleSize,
        ref_sampleSize,
        ref_nrepetitions,
    )

    # Jansen
    runConvergence(
        model_poutre,
        distribution_poutre,
        JansenSensitivityAlgorithm,
        list_sampleSize,
        ref_sampleSize,
        ref_nrepetitions,
    )

    # Mauntz
    runConvergence(
        model_poutre,
        distribution_poutre,
        MauntzKucherenkoSensitivityAlgorithm,
        list_sampleSize,
        ref_sampleSize,
        ref_nrepetitions,
    )

    # Martinez
    runConvergence(
        model_poutre,
        distribution_poutre,
        MartinezSensitivityAlgorithm,
        list_sampleSize,
        ref_sampleSize,
        ref_nrepetitions,
    )
