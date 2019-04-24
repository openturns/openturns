# -*- coding: utf-8 -*-
"""
A graphics class to analyze the results of calibration.
"""

import pylab as pl
import openturns as ot

class PostCalibrationResult:
    def __init__(self,calibrationResult,model,inputObservations,outputObservations):
        """
        Plots the prior and posterior distribution of the calibrated parameter theta.
    
        Parameters
        ----------
        calibrationResult : :class:`~openturns.calibrationResult`
            The result of a calibration.
    
        model : 2-d sequence of float
            The function to calibrate.
    
        inputObservations : :class:`~openturns.Sample`
            The sample of input values of the model linked to the observations.
    
        outputObservations : 2-d sequence of float
            The sample of output values of the model linked to the observations.
        """
        self.model = model
        self.outputAtPrior = None
        self.outputAtPosterior = None
        self.calibrationResult = calibrationResult
        self.observationColor = "blue"
        self.priorColor = "red"
        self.posteriorColor = "green"
        self.inputObservations = inputObservations
        self.outputObservations = outputObservations
        # Compute yAtPrior
        meanPrior = calibrationResult.getParameterPrior().getMean()
        model.setParameter(meanPrior)
        self.outputAtPrior=model(inputObservations)
        # Compute yAtPosterior
        meanPosterior = calibrationResult.getParameterPosterior().getMean()
        model.setParameter(meanPosterior)
        self.outputAtPosterior=model(inputObservations)
        return None

    def drawThetaDistribution(self):
        """
        Plots the prior and posterior distribution of the calibrated parameter theta.
        """
        thetaPrior = self.calibrationResult.getParameterPrior()
        thetaDescription = thetaPrior.getDescription()
        thetaPosterior = self.calibrationResult.getParameterPosterior()
        thetaDim = thetaPosterior.getDimension()
        fig = pl.figure(figsize=(12, 4))
        for i in range(thetaDim):
            graph = ot.Graph("",thetaDescription[i],"PDF",True,"topright")
            # Prior distribution
            thetaPrior_i = thetaPrior.getMarginal(i)
            priorPDF = thetaPrior_i.drawPDF()
            priorPDF.setColors([self.priorColor])
            priorPDF.setLegends(["Prior"])
            graph.add(priorPDF)
            # Posterior distribution
            thetaPosterior_i = thetaPosterior.getMarginal(i)
            postPDF = thetaPosterior_i.drawPDF()
            postPDF.setColors([self.posteriorColor])
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
    
    def drawObservationsVsPredictions(self):
        """
        Plots the output of the model depending 
        on the output observations before and after calibration.
        """
        xDim = self.inputObservations.getDimension()
        if xDim > 1:
            raise TypeError('Expected X observations in 1 dimension.')
        yDim = self.outputObservations.getDimension()
        if yDim > 1:
            raise TypeError('Expected Y observations in 1 dimension.')
        graph = ot.Graph("","Observations","Predictions",True,"topleft")
        # Plot the diagonal
        cloud = ot.Curve(self.outputObservations, self.outputObservations)
        cloud.setColor(self.observationColor)
        graph.add(cloud)
        # Plot the predictions before
        cloud = ot.Cloud(self.outputObservations, self.outputAtPrior)
        cloud.setColor(self.priorColor)
        cloud.setLegend("Before")
        graph.add(cloud)
        # Plot the predictions after
        cloud = ot.Cloud(self.outputObservations, self.outputAtPosterior)
        cloud.setColor(self.posteriorColor)
        cloud.setLegend("After")
        graph.add(cloud)
        return graph

    def drawResiduals(self):
        """
        Plot the distribution of the residuals and 
        the distribution of the observation errors. 
        """    
        # Plot the distribution of the residuals
        sampleResiduals = self.outputObservations - self.outputAtPosterior
        kernel = ot.KernelSmoothing()
        fittedDist = kernel.build(sampleResiduals)
        myGraph = fittedDist.drawPDF()
        myGraph.setColors(["blue"])
        # Plot the distribution of the observation errors
        obserrgraph = self.calibrationResult.getObservationsError().drawPDF()
        obserrgraph.setColors(["green"])
        myGraph.add(obserrgraph)
        #
        myGraph.setLegends(["Residuals","Observation errors"])
        myGraph.setTitle("Residuals analysis")
        myGraph.setXTitle("Residuals")
        myGraph.setYTitle("Probability distribution function")
        return myGraph

    def drawModelVsObservations(self):
        """
        Plots the observed output of the model depending 
        on the observed input before and after calibration.
        """
        xDim = self.inputObservations.getDimension()
        if xDim > 1:
            raise TypeError('Expected X observations in 1 dimension.')
        yDim = self.outputObservations.getDimension()
        if yDim > 1:
            raise TypeError('Expected Y observations in 1 dimension.')
        xdescription = self.inputObservations.getDescription()
        ydescription = self.outputObservations.getDescription()
        graph = ot.Graph("",xdescription[0],ydescription[0],True,"topright")
        # Observations
        cloud = ot.Cloud(self.inputObservations,self.outputObservations)
        cloud.setColor(self.observationColor)
        cloud.setLegend("Observations")
        graph.add(cloud)
        # Model outputs before calibration
        yDim = self.outputAtPrior.getDimension()
        if yDim > 1:
            raise TypeError('Expected Y observations in 1 dimension.')
        cloud = ot.Cloud(self.inputObservations,self.outputAtPrior)
        cloud.setColor(self.priorColor)
        cloud.setLegend("Before")
        graph.add(cloud)
        # Model outputs after calibration
        cloud = ot.Cloud(self.inputObservations,self.outputAtPosterior)
        cloud.setColor(self.posteriorColor)
        cloud.setLegend("After")
        graph.add(cloud)
        return graph
    
