# -*- coding: utf-8 -*-
"""
A graphics class to analyze the results of calibration.
"""

import pylab as pl
import openturns as ot

class CalibrationAnalysis:
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
        ySize = self.outputObservations.getSize()
        graph = ot.Graph("","Observations","Predictions",True,"topleft")
        # Plot the diagonal
        for i in range(ySize):
            cloud = ot.Curve(self.outputObservations[i], self.outputObservations[i])
            cloud.setColor(self.observationColor)
            graph.add(cloud)
        # Plot the predictions before
        yPriorSize = self.outputAtPrior.getSize()
        if (ySize != yPriorSize):
            raise TypeError('Y observations and Y predictions do not have the same size.')
        for i in range(ySize):
            cloud = ot.Cloud(self.outputObservations[i], self.outputAtPrior[i])
            cloud.setColor(self.priorColor)
            cloud.setLegend("Before")
            graph.add(cloud)
        # Plot the predictions after
        yPosteriorSize = self.outputAtPosterior.getSize()
        if (ySize != yPosteriorSize):
            raise TypeError('Y observations and Y predictions do not have the same size.')
        for i in range(ySize):
            cloud = ot.Cloud(self.outputObservations[i], self.outputAtPosterior[i])
            cloud.setColor(self.posteriorColor)
            cloud.setLegend("After")
            graph.add(cloud)
        return graph

    def drawResiduals(self):
        """
        Plot the distribution of the residuals and 
        the distribution of the observation errors. 
        """    
        graph = ot.Graph("Residuals analysis","Residuals","Probability distribution function",True,"topleft")
        ySize = self.outputObservations.getSize()
        yPriorSize = self.outputAtPrior.getSize()
        if (ySize != yPriorSize):
            raise TypeError('Y observations and Y predictions do not have the same size.')
        for i in range(ySize):
            sampleResiduals = self.outputObservations[i] - self.outputAtPosterior[i]
            kernel = ot.KernelSmoothing()
            fittedDist = kernel.build(ot.Sample(sampleResiduals,1))
            residualPDF = fittedDist.drawPDF()
            residualPDF.setColors(["blue"])
            residualPDF.setLegends(["Residuals"])
            graph.add(residualPDF)
        # Plot the distribution of the observation errors
        observationsError = self.calibrationResult.getObservationsError()
        if (observationsError.getDimension()==1):
            obserrgraph = observationsError.drawPDF()
            obserrgraph.setColors(["green"])
            obserrgraph.setLegends(["Observation errors"])
            graph.add(obserrgraph)
        return graph

    def drawObservationsVsInputs(self):
        """
        Plots the observed output of the model depending 
        on the observed input before and after calibration.
        """
        xSize = self.inputObservations.getSize()
        ySize = self.outputObservations.getSize()
        xDim = self.inputObservations.getDimension()
        yDim = self.outputObservations.getDimension()
        xdescription = self.inputObservations.getDescription()
        ydescription = self.outputObservations.getDescription()
        graph = ot.Graph("",xdescription[0],ydescription[0],True,"topright")
        # Observations
        if (xDim==1) and (yDim==1):
            cloud = ot.Cloud(self.inputObservations,self.outputObservations)
            cloud.setColor(self.observationColor)
            cloud.setLegend("Observations")
            graph.add(cloud)
        elif (xSize==1) and (ySize==1):
            cloud = ot.Cloud(self.inputObservations[0],self.outputObservations[0])
            cloud.setColor(self.observationColor)
            cloud.setLegend("Observations")
            graph.add(cloud)
        else:
            raise TypeError('X observations and Y predictions do not fit in size or dimension.')
        # Model outputs before calibration
        yPriorSize = self.outputAtPrior.getSize()
        yPriorDim = self.outputAtPrior.getDimension()
        if (xDim==1) and (yPriorDim==1):
            cloud = ot.Cloud(self.inputObservations,self.outputAtPrior)
            cloud.setColor(self.priorColor)
            cloud.setLegend("Before")
            graph.add(cloud)
        elif (xSize==1) and (yPriorSize==1):
            cloud = ot.Cloud(self.inputObservations[0],self.outputAtPrior[0])
            cloud.setColor(self.priorColor)
            cloud.setLegend("Before")
            graph.add(cloud)
        else:
            raise TypeError('X observations and Y predictions do not fit in size or dimension.')
        # Model outputs after calibration
        yPosteriorSize = self.outputAtPosterior.getSize()
        yPosteriorDim = self.outputAtPosterior.getDimension()
        if (xDim==1) and (yPosteriorDim==1):
            cloud = ot.Cloud(self.inputObservations,self.outputAtPosterior)
            cloud.setColor(self.posteriorColor)
            cloud.setLegend("After")
            graph.add(cloud)
        elif (xSize==1) and (yPosteriorSize==1):        
            cloud = ot.Cloud(self.inputObservations[0],self.outputAtPosterior[0])
            cloud.setColor(self.posteriorColor)
            cloud.setLegend("After")
            graph.add(cloud)
        else:
            raise TypeError('X observations and Y predictions do not fit in size or dimension.')
        return graph