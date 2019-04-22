# -*- coding: utf-8 -*-
"""
A collection of graphics functions for calibration.
"""

import pylab as pl
import openturns as ot

CalibrationGraphicsConstants_ObservationColor = "blue"
CalibrationGraphicsConstants_PriorColor = "red"
CalibrationGraphicsConstants_PosteriorColor = "green"

def drawModelVsObservations(theta,xobserved,yobserved,obsFunction):
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
    cloud.setColor(CalibrationGraphicsConstants_ObservationColor)
    graph.add(cloud)
    # Model outputs
    obsFunction.setParameter(theta)
    y=obsFunction(xobserved)
    yDim = y.getDimension()
    if yDim > 1:
        raise TypeError('Expected Y observations in 1 dimension.')
    cloud = ot.Cloud(xobserved,y)
    cloud.setColor(CalibrationGraphicsConstants_PosteriorColor)
    cloud.setLegend("Model outputs")
    graph.add(cloud)
    return graph

def drawObservationsVsPredictions(theta,xobserved,yobserved,obsFunction):
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
    cloud.setColor(CalibrationGraphicsConstants_PriorColor)
    graph.add(cloud)
    # Compute the observations
    obsFunction.setParameter(theta)
    y=obsFunction(xobserved)
    cloud = ot.Cloud(yobserved, y)
    cloud.setColor(CalibrationGraphicsConstants_ObservationColor)
    graph.add(cloud)
    return graph

def drawModelVsObservationsBeforeAndAFter(thetaPrior,thetaPosterior,xobserved,yobserved,obsFunction):
    """
    Plots the observed output of the model depending 
    on the observed input before and after calibration.

    Parameters
    ----------
    thetaPrior : :class:`~openturns.Point`
        The value of the parameters before calibration.

    thetaPosterior : :class:`~openturns.Point`
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
    cloud.setColor(CalibrationGraphicsConstants_ObservationColor)
    cloud.setLegend("Observations")
    graph.add(cloud)
    # Model outputs before calibration
    obsFunction.setParameter(thetaPrior)
    yAtPrior=obsFunction(xobserved)
    yDim = yAtPrior.getDimension()
    if yDim > 1:
        raise TypeError('Expected Y observations in 1 dimension.')
    cloud = ot.Cloud(xobserved,yAtPrior)
    cloud.setColor(CalibrationGraphicsConstants_PriorColor)
    cloud.setLegend("Before")
    graph.add(cloud)
    # Model outputs after calibration
    obsFunction.setParameter(thetaPosterior)
    yAtPosterior=obsFunction(xobserved)
    cloud = ot.Cloud(xobserved,yAtPosterior)
    cloud.setColor(CalibrationGraphicsConstants_PosteriorColor)
    cloud.setLegend("After")
    graph.add(cloud)
    return graph

def drawResiduals(theta,xobserved,yobserved,obsFunction,errorCovariance=None):
    """
    Plot the distribution of the residuals. 

    Parameters
    ----------
    theta : :class:`~openturns.Point`
        The value of the parameters.

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

def drawObservationsVsPredictionsBeforeAfter(thetaPrior,thetaPosterior,xobserved,yobserved,obsFunction):
    """
    Plots the output of the model depending 
    on the output observations before and after calibration.

    Parameters
    ----------
    thetaPrior : :class:`~openturns.Point`
        The value of the parameters before calibration.

    thetaPosterior : :class:`~openturns.Point`
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
    cloud.setColor(CalibrationGraphicsConstants_ObservationColor)
    graph.add(cloud)
    # Plot the predictions before
    obsFunction.setParameter(thetaPrior)
    yAtPrior=obsFunction(xobserved)
    cloud = ot.Cloud(yobserved, yAtPrior)
    cloud.setColor(CalibrationGraphicsConstants_PriorColor)
    cloud.setLegend("Before")
    graph.add(cloud)
    # Plot the predictions after
    obsFunction.setParameter(thetaPosterior)
    yAtPosterior=obsFunction(xobserved)
    cloud = ot.Cloud(yobserved, yAtPosterior)
    cloud.setColor(CalibrationGraphicsConstants_PosteriorColor)
    cloud.setLegend("After")
    graph.add(cloud)
    return graph

def drawThetaDistribution(calibrationResult):
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
        By default, no label is set.
    """
    thetaPrior = calibrationResult.getParameterPrior()
    thetaDescription = thetaPrior.getDescription()
    thetaPosterior = calibrationResult.getParameterPosterior()
    thetaDim = thetaPosterior.getDimension()
    fig = pl.figure(figsize=(12, 4))
    for i in range(thetaDim):
        graph = ot.Graph("",thetaDescription[i],"PDF",True,"topright")
        # Prior distribution
        thetaPrior_i = thetaPrior.getMarginal(i)
        priorPDF = thetaPrior_i.drawPDF()
        priorPDF.setColors([CalibrationGraphicsConstants_PriorColor])
        priorPDF.setLegends(["Prior"])
        graph.add(priorPDF)
        # Posterior distribution
        thetaPosterior_i = thetaPosterior.getMarginal(i)
        postPDF = thetaPosterior_i.drawPDF()
        postPDF.setColors([CalibrationGraphicsConstants_PosteriorColor])
        postPDF.setLegends(["Posterior"])
        graph.add(postPDF)
        '''
        If the prior is a Dirac, set the vertical axis bounds to the posterior. 
        Otherwise, the Dirac set to [0,1], where the 1 can be much larger 
        than the maximum PDF of the posterior.
        '''
        if (thetaPrior_i.getName()=="Dirac"):
            # The vertical (PDF) bounds of the posterior
            postbb = postPDF.getBoundingBox()
            pdf_upper = postbb.getUpperBound()[1]
            pdf_lower = postbb.getLowerBound()[1]
            # Set these bounds to the graph
            bb = graph.getBoundingBox()
            graph_upper = bb.getUpperBound()
            graph_upper[1] = pdf_upper
            bb.setUpperBound(graph_upper)
            graph_lower = bb.getLowerBound()
            graph_lower[1] = pdf_lower
            bb.setLowerBound(graph_lower)
            graph.setBoundingBox(bb)
        # Add it to the graphics
        ax = fig.add_subplot(1, thetaDim, i+1)
        _ = ot.viewer.View(graph, figure=fig, axes=[ax])
    return fig
