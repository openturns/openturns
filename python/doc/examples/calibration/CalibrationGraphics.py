# -*- coding: utf-8 -*-
"""
A collection of graphics functions for calibration.
"""

import pylab as pl
import openturns as ot
import numpy as np
from openturns.viewer import View

'''
Une librairie pour l'assimilation de donnée.
'''

CalibrationGraphicsConstants_observationColor = "blue"
CalibrationGraphicsConstants_beforeColor = "red"
CalibrationGraphicsConstants_afterColor = "green"

def plotModelVsObservations(theta,xobserved,yobserved,obsFunction):
    """
    Plots the observed output of the model at point theta depending 
    on the observed input.

    Parameters
    ----------
    theta : :class:`~openturns.Point`
        The parameters to calibrate.

    xobserved : :class:`~openturns.Sample`
        The input observations.

    yobserved : :class:`~openturns.Sample`
        The output observations.

    obsFunction : :class:`~openturns.Function`
        The model.
    """
    xDim = xobserved.getDimension()
    if xDim > 1:
        raise TypeError('Expected X observations in 1 dimension.')
    yDim = yobserved.getDimension()
    if yDim > 1:
        raise TypeError('Expected Y observations in 1 dimension.')
    xdescription = xobserved.getDescription()
    ydescription = yobserved.getDescription()
    graph = ot.Graph("",xdescription[0],ydescription[0],True,"topright")
    # Observations
    cloud = ot.Cloud(xobserved,yobserved)
    cloud.setLegend("Observations")
    cloud.setColor(CalibrationGraphicsConstants_observationColor)
    graph.add(cloud)
    # Model outputs
    obsFunction.setParameter(theta)
    y=obsFunction(xobserved)
    yDim = y.getDimension()
    if yDim > 1:
        raise TypeError('Expected Y observations in 1 dimension.')
    cloud = ot.Cloud(xobserved,y)
    cloud.setColor(CalibrationGraphicsConstants_afterColor)
    cloud.setLegend("Model outputs")
    graph.add(cloud)
    return graph

def plotModelVsObservationsBeforeAndAFter(thetaReference,thetaStar,xobserved,yobserved,obsFunction):
    """
    Plots the observed output of the model depending 
    on the observed input before and after calibration.

    Parameters
    ----------
    thetaReference : :class:`~openturns.Point`
        The value of the parameters before calibration.

    thetaStar : :class:`~openturns.Point`
        The value of the optimal parameters (i.e. after calibration).

    xobserved : :class:`~openturns.Sample`
        The input observations.

    yobserved : :class:`~openturns.Sample`
        The output observations.

    obsFunction : :class:`~openturns.Function`
        The calibrated model.
    """
    xDim = xobserved.getDimension()
    if xDim > 1:
        raise TypeError('Expected X observations in 1 dimension.')
    yDim = yobserved.getDimension()
    if yDim > 1:
        raise TypeError('Expected Y observations in 1 dimension.')
    xdescription = xobserved.getDescription()
    ydescription = yobserved.getDescription()
    graph = ot.Graph("",xdescription[0],ydescription[0],True,"topright")
    # Observations
    cloud = ot.Cloud(xobserved,yobserved)
    cloud.setColor(CalibrationGraphicsConstants_observationColor)
    cloud.setLegend("Observations")
    graph.add(cloud)
    # Model outputs before calibration
    obsFunction.setParameter(thetaReference)
    y=obsFunction(xobserved)
    yDim = y.getDimension()
    if yDim > 1:
        raise TypeError('Expected Y observations in 1 dimension.')
    cloud = ot.Cloud(xobserved,y)
    cloud.setColor(CalibrationGraphicsConstants_beforeColor)
    cloud.setLegend("Before")
    graph.add(cloud)
    # Model outputs after calibration
    obsFunction.setParameter(thetaStar)
    y=obsFunction(xobserved)
    cloud = ot.Cloud(xobserved,y)
    cloud.setColor(CalibrationGraphicsConstants_afterColor)
    cloud.setLegend("After")
    graph.add(cloud)
    return graph

def plotObservationsVsPredictions(theta,xobserved,yobserved,obsFunction):
    """
    Plots the output of the model (i.e. the predictions) depending 
    on the observed outputs.

    Parameters
    ----------
    theta : :class:`~openturns.Point`
        The value of the calibrated parameters.

    yobserved : :class:`~openturns.Sample`
        The output observations.

    obsFunction : :class:`~openturns.Function`
        The calibrated model.
    """
    yDim = yobserved.getDimension()
    if yDim > 1:
        raise TypeError('Expected Y observations in 1 dimension.')
    graph = ot.Graph("","Observations","Predictions",True)
    # Plot the diagonal
    cloud = ot.Curve(yobserved, yobserved)
    cloud.setColor(CalibrationGraphicsConstants_beforeColor)
    graph.add(cloud)
    # Compute the observations
    obsFunction.setParameter(theta)
    y=obsFunction(xobserved)
    cloud = ot.Cloud(yobserved, y)
    cloud.setColor(CalibrationGraphicsConstants_observationColor)
    graph.add(cloud)
    return graph

def plotResiduals(theta,xobserved,yobserved,obsFunction,errorCovariance=None):
    """
    Plot the distribution of the residuals. 

    Parameters
    ----------
    theta : :class:`~openturns.Point`
        The value of the parameters to be calibrated.

    xobserved : :class:`~openturns.Sample`
        The input observations.

    yobserved : :class:`~openturns.Sample`
        The output observations.

    obsFunction : :class:`~openturns.Function`
        The calibrated model.

    errorCovariance : :class:`~openturns.CovarianceMatrix`
        The output covariance matrix. 
        By default, the covariance matrix is not plotted.
    """

    # Plot the distribution of the residuals
    obsFunction.setParameter(theta)
    yPredicted = obsFunction(xobserved)
    r = yPredicted - yobserved
    myGraph = ot.VisualTest_DrawHistogram(r)
    # Fit a gaussian on the residual sample
    fittedNormal = ot.NormalFactory().build(r)
    mu = fittedNormal.getParameter()[0]
    sigma = fittedNormal.getParameter()[1]
    ng = fittedNormal.drawPDF()
    ng.setColors(["blue"])
    myGraph.add(ng)
    # Plot the gaussian distribution of the output covariance
    if not (errorCovariance is None):
        zero = ot.Point(1)
        obsDistr = ot.Normal(zero,errorCovariance)
        og = obsDistr.drawPDF()
        og.setColors(["green"])
        myGraph.add(og)
    #
    if not (errorCovariance is None):
        myGraph.setLegends(["Data","Normal residual fit","Normal output"])
    else:
        myGraph.setLegends(["Data","Normal residual fit"])
    myGraph.setTitle("Residuals analysis")
    myGraph.setXTitle("Residuals")
    myGraph.setYTitle("Probability distribution function")
    return myGraph

def plotObservationsVsPredictionsBeforeAfter(thetaReference,thetaStar,xobserved,yobserved,obsFunction):
    """
    Plots the output of the model depending 
    on the output observations before and after calibration.

    Parameters
    ----------
    thetaReference : :class:`~openturns.Point`
        The value of the parameters before calibration.

    thetaStar : :class:`~openturns.Point`
        The value of the optimal parameters (i.e. after calibration).

    xobserved : :class:`~openturns.Sample`
        The input observations.

    yobserved : :class:`~openturns.Sample`
        The output observations.

    obsFunction : :class:`~openturns.Function`
        The calibrated model.
    """

    # Plot the Ymodel vs Yobservations
    xDim = xobserved.getDimension()
    if xDim > 1:
        raise TypeError('Expected X observations in 1 dimension.')
    yDim = yobserved.getDimension()
    if yDim > 1:
        raise TypeError('Expected Y observations in 1 dimension.')
    graph = ot.Graph("","Observations","Predictions",True,"topleft")
    # Plot the diagonal
    cloud = ot.Curve(yobserved, yobserved)
    cloud.setColor(CalibrationGraphicsConstants_observationColor)
    graph.add(cloud)
    # Plot the predictions before
    obsFunction.setParameter(thetaReference)
    yBefore=obsFunction(xobserved)
    cloud = ot.Cloud(yobserved, yBefore)
    cloud.setColor(CalibrationGraphicsConstants_beforeColor)
    cloud.setLegend("Before")
    graph.add(cloud)
    # Plot the predictions after
    obsFunction.setParameter(thetaStar)
    yAfter=obsFunction(xobserved)
    cloud = ot.Cloud(yobserved, yAfter)
    cloud.setColor(CalibrationGraphicsConstants_afterColor)
    cloud.setLegend("After")
    graph.add(cloud)
    return graph

def plotThetaDistribution(thetaPrior,covariancePrior,distributionPosterior,labelsTheta=None):
    """
    Plots the prior and posterior distribution of the calibrated parameter theta.

    Parameters
    ----------
    thetaPrior : :class:`~openturns.Point`
        The mean of the gaussian prior of the parameters.

    covariancePrior : :class:`~openturns.CovarianceMatrix`
        The prior covariance matrix.

    distributionPosterior : :class:`~openturns.Distribution`
        The posterior distribution of theta.

    labelsTheta : a p-tuple of strings, the description of theta.
    """
    # Plot the distribution of theta
    fig = pl.figure(figsize=(12, 4))
    thetaDim = len(thetaPrior)
    for i in range(thetaDim):
        # Loi à posteriori
        thetaPosterior = distributionPosterior.getMarginal(i)
        myGraph = thetaPosterior.drawPDF()
        myGraph.setColors(["green"])
        # Loi à priori
        sigma_i = np.sqrt(covariancePrior[i,i])
        thetaGaussianPrior = ot.Normal(thetaPrior[i],sigma_i)
        pg = thetaGaussianPrior.drawPDF()
        pg.setColors(["red"])
        myGraph.add(pg)
        #
        myGraph.setXTitle(labelsTheta[i])
        if (i==0):
            myGraph.setYTitle("PDF")
        myGraph.setLegends(["Posterior","Prior"])
        myGraph.setTitle("Theta PDF")
        # Add it to the graphics
        ax = fig.add_subplot(1, thetaDim, i+1)
        _ = View(myGraph, figure=fig, axes=[ax])
    return fig

