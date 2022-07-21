#! /usr/bin/env python
# -*- coding: utf-8 -*-
"""
@author: Michaël Baudin (EDF), Antoine Dumas (Phiméca)

Analyses the convergence of the standard deviation of the asymptotic
distribution of the Sobol' index estimator depending on the
sample size.
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


def computeVariance(model, distribution, sobol_estimator, sampleSize):
    """
    Compute the asymptotic standard deviation of the Sobol' indices estimator.

    Parameters
    ----------
    model : ot.Function
        The function.
    distribution : ot.Distribution
        The input distribution.
    sobol_estimator : ot.SobolSensitivityAlgorithm
        The estimator.
    sampleSize : int
        The sample size.

    Returns
    -------
    std_first_asymptotic : ot.Point(dimension)
        The componentwise standard deviation of the first order index.
    std_total_asymptotic : ot.Point(dimension)
        The componentwise standard deviation of the total order index.

    """

    sobolexperiment = ot.SobolIndicesExperiment(distribution, int(sampleSize), False)
    inputDesign = sobolexperiment.generate()
    outputDesign = model(inputDesign)
    sensitivity_algorithm = sobol_estimator(inputDesign, outputDesign, int(sampleSize))

    # Récupère les distributions asymptotiques
    distFirstCol = sensitivity_algorithm.getFirstOrderAsymptoticDistribution()
    distTotalCol = sensitivity_algorithm.getTotalOrderAsymptoticDistribution()
    std_first_asymptotic = ot.Point([d.getStandardDeviation()[0] for d in distFirstCol])
    std_total_asymptotic = ot.Point([d.getStandardDeviation()[0] for d in distTotalCol])

    return std_first_asymptotic, std_total_asymptotic


def plotVariance(
    algorithmName, list_sampleSize, list_std_first_asymptotic, list_std_total_asymptotic
):
    """
    Plots the standard deviation depending on the sample size.

    Each figure is saved into "Convergence_Variance-" + algorithmName + ".png".

    Parameters
    ----------
    algorithmName : str
        The name of the test case.
    list_sampleSize : list of ints
        The list of sample sizes.
    list_std_first_asymptotic : list of floats
        The list of standard deviations of the first order indices.
    list_std_total_asymptotic : TYPE
        The list of standard deviations of the total order indices.

    Returns
    -------
    None.

    """
    fig, ax = pl.subplots()
    ax.set_title(algorithmName)
    ax.plot(list_sampleSize, 1.0 / sqrt(list_sampleSize), "--", label=u"$1/\\sqrt{n}$")

    for i in range(list_std_first_asymptotic.getDimension()):
        ax.plot(
            list_sampleSize,
            np.hstack(list_std_first_asymptotic[:, i]),
            "o",
            label="$S_{%i}$" % i,
        )
        ax.plot(
            list_sampleSize,
            np.hstack(list_std_total_asymptotic[:, i]),
            "o",
            label="$T_{%i}$" % i,
        )
    ax.set_xlabel("N")
    ax.set_ylabel("Variance")
    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.legend(bbox_to_anchor=(1.0, 1.0))
    fig.show()
    fig.savefig(
        "Convergence_stddev-" + algorithmName + ".png",
        transparent=True,
        bbox_inches="tight",
    )
    return None


def runConvergence(
    model,
    distribution,
    sobol_estimator,
    list_sampleSize,
):
    """
    Performs the convergence analysis.

    Parameters
    ----------
    model : ot.Function
        The function.
    distribution : ot.Distribution
        The input distribution.
    sobol_estimator : ot.SobolSensitivityAlgorithm
        The estimator.
    list_sampleSize : list of ints
        The list of sample sizes.

    Returns
    -------
    None.

    """

    list_std_first_asymptotic = ot.Sample(
        len(list_sampleSize), distribution.getDimension()
    )
    list_std_total_asymptotic = ot.Sample(
        len(list_sampleSize), distribution.getDimension()
    )
    for i, sampleSize in enumerate(list_sampleSize):
        # Compute errors
        list_std_first_asymptotic[i], list_std_total_asymptotic[i] = computeVariance(
            model, distribution, sobol_estimator, sampleSize
        )

    algorithmName = "%s - %s" % (model.getName(), sobol_estimator.__name__)
    plotVariance(
        algorithmName,
        list_sampleSize,
        list_std_first_asymptotic,
        list_std_total_asymptotic,
    )


if __name__ == "__main__":
    # Sizes of simulation
    n_loops = 10
    list_sampleSize_float = np.logspace(1, 5, n_loops)  # n_loops valeurs de 1e1 à 1e5
    list_sampleSize = [int(v) for v in list_sampleSize_float]

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
    )

    # Jansen
    runConvergence(
        model_gsobol,
        distribution_gsobol,
        JansenSensitivityAlgorithm,
        list_sampleSize,
    )

    # Mauntz
    runConvergence(
        model_gsobol,
        distribution_gsobol,
        MauntzKucherenkoSensitivityAlgorithm,
        list_sampleSize,
    )

    # Martinez
    runConvergence(
        model_gsobol,
        distribution_gsobol,
        MartinezSensitivityAlgorithm,
        list_sampleSize,
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
    )

    # Jansen
    runConvergence(
        model_ishigami,
        distribution_ishigami,
        JansenSensitivityAlgorithm,
        list_sampleSize,
    )

    # Mauntz
    runConvergence(
        model_ishigami,
        distribution_ishigami,
        MauntzKucherenkoSensitivityAlgorithm,
        list_sampleSize,
    )

    # Martinez
    runConvergence(
        model_ishigami,
        distribution_ishigami,
        MartinezSensitivityAlgorithm,
        list_sampleSize,
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
    )

    # Jansen
    runConvergence(
        model_poutre,
        distribution_poutre,
        JansenSensitivityAlgorithm,
        list_sampleSize,
    )

    # Mauntz
    runConvergence(
        model_poutre,
        distribution_poutre,
        MauntzKucherenkoSensitivityAlgorithm,
        list_sampleSize,
    )

    # Martinez
    runConvergence(
        model_poutre,
        distribution_poutre,
        MartinezSensitivityAlgorithm,
        list_sampleSize,
    )
