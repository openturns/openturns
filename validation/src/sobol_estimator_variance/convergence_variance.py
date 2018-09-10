# -*- coding: utf-8 -*-
"""
Created on Thu Jun 01 11:48:42 2017

@author: c61372, dumas
"""

#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
from gsobollib import (gsobolSAExact, 
                       gsobolDistribution, gsobol)
from ishigamilib import (ishigamiSAExact, ishigamiDistribution, 
                         ishigamiAB, ishigamiGSymbolic)
from numpy import zeros, sqrt, array
import numpy as np
import matplotlib as mpl
mpl.use('Agg') # to remove to show figure
import pylab as pl
from pylab import plot, show, xlabel, ylabel, xscale, yscale, legend, title, savefig
from sensitivity_confidence import SensitivityConfidenceTest
from sobol_variance_estimators import (SaltelliSensitivityAlgorithm,
                                       MartinezSensitivityAlgorithm,
                                       JansenSensitivityAlgorithm,
                                       MauntzKucherenkoSensitivityAlgorithm)
ot.Log.Show(ot.Log.ERROR)


def computeVariance(model, distribution, sobol_estimator, sampleSize):
               
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

def plotVariance(algorithmName, list_sampleSize, list_std_first_asymptotic, list_std_total_asymptotic):
    fig, ax = pl.subplots()
    ax.set_title(algorithmName)
    ax.plot(list_sampleSize, 1./sqrt(list_sampleSize),"--", label="1/sqrt(n)")
    ax.plot(list_sampleSize, (0.1)/sqrt(list_sampleSize),"--", label="0.1/sqrt(n)")
    ax.plot(list_sampleSize, (0.01)/sqrt(list_sampleSize),"--", label="0.01/sqrt(n)")

    for i in range(list_std_first_asymptotic.getDimension()):
        ax.plot(list_sampleSize, np.hstack(list_std_first_asymptotic[:,i]),"o", label="First order X%i"%i)
        ax.plot(list_sampleSize, np.hstack(list_std_total_asymptotic[:,i]),"o", label="Total order X%i"%i)
    ax.set_xlabel("N")
    ax.set_ylabel("Variance")
    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.legend()
    fig.show()
    fig.savefig("Convergence_Variance-" + algorithmName+'.png', transparent=True, bbox_inches="tight")
    return None

def runConvergence(model, distribution, sobol_estimator, list_sampleSize,
                   ref_sampleSize=10000, ref_nrepetitions=1000):

    
    list_std_first_asymptotic = ot.Sample(len(list_sampleSize), distribution.getDimension())
    list_std_total_asymptotic = ot.Sample(len(list_sampleSize), distribution.getDimension())
    for i, sampleSize in enumerate(list_sampleSize):
        # Compute errors
        list_std_first_asymptotic[i], \
        list_std_total_asymptotic[i] = computeVariance(model,
                                                       distribution,
                                                       sobol_estimator,
                                                       sampleSize)

    algorithmName = "%s - %s" % (model.getName(), sobol_estimator.__name__)
    plotVariance(algorithmName, list_sampleSize, list_std_first_asymptotic, list_std_total_asymptotic)


if __name__ == '__main__':
    #########################################
    # Sizes of simulation
    n_loops = 10
    list_sampleSize = np.logspace(1, 5, n_loops) # n_loops valeurs de 1e1 à 1e5

    ref_sampleSize = 4e5
    ref_nrepetitions = 1000

    ################################################################################
    #################                  GSOBOL                  #####################
    ################################################################################

    a = array([0,9,99])
    # Distribution uniforme associée au cas-test GSobol
    distribution_gsobol = gsobolDistribution(len(a))
    model_gsobol = ot.PythonFunction(len(a), 1, func_sample=lambda X: gsobol(X,a))
    model_gsobol.setName("G-Sobol")

    # Saltelli
    runConvergence(model_gsobol, distribution_gsobol, SaltelliSensitivityAlgorithm,
                   list_sampleSize, ref_sampleSize, ref_nrepetitions)

    # Jansen
    runConvergence(model_gsobol, distribution_gsobol, JansenSensitivityAlgorithm,
                   list_sampleSize, ref_sampleSize, ref_nrepetitions)

    # Mauntz
    runConvergence(model_gsobol, distribution_gsobol, MauntzKucherenkoSensitivityAlgorithm,
                   list_sampleSize, ref_sampleSize, ref_nrepetitions)

    # Martinez
    runConvergence(model_gsobol, distribution_gsobol, MartinezSensitivityAlgorithm,
                   list_sampleSize, ref_sampleSize, ref_nrepetitions)

    ################################################################################
    ################                  ISHIGAMI                  ####################
    ################################################################################

    model_ishigami = ishigamiGSymbolic()
    model_ishigami.setName("Ishigami")
    distribution_ishigami = ishigamiDistribution()

    # Saltelli
    runConvergence(model_ishigami, distribution_ishigami, SaltelliSensitivityAlgorithm,
                   list_sampleSize, ref_sampleSize, ref_nrepetitions)

    # Jansen
    runConvergence(model_ishigami, distribution_ishigami, JansenSensitivityAlgorithm,
                   list_sampleSize, ref_sampleSize, ref_nrepetitions)

    # Mauntz
    runConvergence(model_ishigami, distribution_ishigami, MauntzKucherenkoSensitivityAlgorithm,
                   list_sampleSize, ref_sampleSize, ref_nrepetitions)

    # Martinez
    runConvergence(model_ishigami, distribution_ishigami, MartinezSensitivityAlgorithm,
                   list_sampleSize, ref_sampleSize, ref_nrepetitions)

    ################################################################################
    #################                  POUTRE                  #####################
    ################################################################################


    model_poutre = ot.SymbolicFunction(['L', 'b', 'h', 'E', 'F'],
                                       ['F * L^3 / (48 * E * b * h^3 / 12)'])
    model_poutre.setName("poutre")
    L = ot.LogNormal()
    L.setParameter(ot.LogNormalMuSigmaOverMu()([5., .02, 0.]))
    b = ot.LogNormal()
    b.setParameter(ot.LogNormalMuSigmaOverMu()([.2, .05, 0.]))
    h = ot.LogNormal()
    h.setParameter(ot.LogNormalMuSigmaOverMu()([.4, .05, 0.]))
    E = ot.LogNormal()
    E.setParameter(ot.LogNormalMuSigmaOverMu()([3e4, .12, 0.]))
    F = ot.LogNormal()
    F.setParameter(ot.LogNormalMuSigmaOverMu()([.1, .20, 0.]))
    distribution_poutre = ot.ComposedDistribution([L, b, h, E, F])

    # Saltelli
    runConvergence(model_poutre, distribution_poutre, SaltelliSensitivityAlgorithm,
                   list_sampleSize, ref_sampleSize, ref_nrepetitions)

    # Jansen
    runConvergence(model_poutre, distribution_poutre, JansenSensitivityAlgorithm,
                   list_sampleSize, ref_sampleSize, ref_nrepetitions)

    # Mauntz
    runConvergence(model_poutre, distribution_poutre, MauntzKucherenkoSensitivityAlgorithm,
                   list_sampleSize, ref_sampleSize, ref_nrepetitions)

    # Martinez
    runConvergence(model_poutre, distribution_poutre, MartinezSensitivityAlgorithm,
                   list_sampleSize, ref_sampleSize, ref_nrepetitions)