# -*- coding: utf-8 -*-
# Copyright (C) 2018 - Michael Baudin

import pylab as pl
import openturns as ot
import numpy as np
from openturns.viewer import View


'''
Une librairie pour l'assimilation de donnée.
'''


def plotModelVsData(theta,xobserved,yobserved,obsFunction):
    # Modele vs Donnees
    y=obsFunction(theta)
    pl.plot(xobserved,yobserved,"bo",label="Data")
    pl.plot(xobserved,y,"ro",label="Model")
    pl.legend(loc=2)
    return None

def plotModelVsDataBeforeAndAFter(thetaB,thetaStar,xobserved,yobserved,obsFunction):
    # Plot the fit
    # Plot observations
    pl.plot(xobserved,yobserved,"bo",label="Data")
    # Plot observations before
    obsFunction.setParameter(thetaB)
    yBefore=obsFunction(xobserved)
    pl.plot(xobserved,yBefore,"ro",label="Before DA")
    # Plot observations after
    obsFunction.setParameter(thetaStar)
    yAfter=obsFunction(xobserved)
    pl.plot(xobserved,yAfter,"go",label="After DA")
    pl.legend(loc=2)
    return None

def plotObservationsVsPredictions(theta,yobserved,obsFunction):
    # Plot the Ymodel vs Yobservations
    # Plot the diagonal
    pl.plot(yobserved, yobserved, "r-")
    # Compute the observations
    obsFunction.setParameter(theta)
    y=obsFunction(theta)
    pl.plot(yobserved, y, "bo")
    pl.xlabel("Observation")
    pl.ylabel("Prediction")
    pl.legend(loc=2)
    return None

def plotResiduals(theta,xobserved,yobserved,obsFunction,errorCovariance=None):
    # Plot the distribution of the residuals
    obsFunction.setParameter(theta)
    yPredicted = obsFunction(xobserved)
    r = yPredicted - yobserved
    myGraph = ot.VisualTest_DrawHistogram(r)
    # Loi normale ajustée
    fittedNormal = ot.NormalFactory().build(r)
    mu = fittedNormal.getParameter()[0]
    sigma = fittedNormal.getParameter()[1]
    ng = fittedNormal.drawPDF()
    ng.setColors(["blue"])
    myGraph.add(ng)
    # Loi normale sur les observations
    if not (errorCovariance is None):
        zero = ot.Point(1)
        obsDistr = ot.Normal(zero,errorCovariance)
        og = obsDistr.drawPDF()
        og.setColors(["green"])
        myGraph.add(og)
    #
    if not (errorCovariance is None):
        myGraph.setLegends(["Data","Normal data fit","Normal(0,%.4e)" % (errorCovariance[0,0])])
    else:
        myGraph.setLegends(["Data","Normal data fit"])
    myGraph.setTitle("Residuals analysis - Mu=%.4e, Sigma=%.4e" % (mu,sigma))
    myGraph.setXTitle("Residuals")
    myGraph.setYTitle("Probability distribution function")
    return myGraph

def plotObservationsVsPredictionsBeforeAfter(theta0,thetaStar,xobserved,yobserved,obsFunction):
    # Plot the Ymodel vs Yobservations
    # Plot the diagonal
    pl.plot(yobserved, yobserved, "b-")
    # Plot the predictions before
    obsFunction.setParameter(theta0)
    yBefore=obsFunction(xobserved)
    pl.plot(yobserved, yBefore, "ro", label="Before")
    # Plot the predictions after
    obsFunction.setParameter(thetaStar)
    yAfter=obsFunction(xobserved)
    pl.plot(yobserved, yAfter, "go", label="After")
    pl.xlabel("Observation")
    pl.ylabel("Prediction")    
    pl.legend(loc=2)
    return None

def plotThetaDistribution(thetaB,thetaStar,covariancePrior,distributionPosterior,labelsTheta=None):
    # Plot the distribution of theta
    dimCalage = len(thetaStar)
    for i in range(dimCalage):
        # Loi à posteriori
        thetaPosterior = distributionPosterior.getMarginal(i)
        myGraph = thetaPosterior.drawPDF()
        myGraph.setColors(["green"])
        # Loi à priori
        thetaPrior = ot.Normal(thetaB[i],np.sqrt(covariancePrior[i,i]))
        pg = thetaPrior.drawPDF()
        pg.setColors(["red"])
        myGraph.add(pg)
        #
        myGraph.setXTitle(labelsTheta[i])
        myGraph.setYTitle("PDF")
        myGraph.setLegends(["Posterior","Prior"])
        myGraph.setTitle("Theta PDF")
        View(myGraph)
    return None

