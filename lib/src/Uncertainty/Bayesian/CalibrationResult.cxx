//                                               -*- C++ -*-
/**
 *  @brief CalibrationResult stores the result of a CalibrationAlgorithmImplementation
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/CalibrationResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/KernelSmoothing.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/Curve.hxx"
#include "openturns/VisualTest.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CalibrationResult)

static const Factory<CalibrationResult> Factory_CalibrationResult;

/* Default constructor */
CalibrationResult::CalibrationResult()
  : PersistentObject()
{
  // Nothing to do
}

/* Parameter constructor */
CalibrationResult::CalibrationResult(const Distribution & parameterPrior,
                                     const Distribution & parameterPosterior,
                                     const Point & parameterMAP,
                                     const Distribution & observationsError,
                                     const Sample & inputObservations,
                                     const Sample & outputObservations,
                                     const Function & residualFunction,
                                     const Bool & bayesian
                                    )
  : PersistentObject()
  , parameterPrior_(parameterPrior)
  , parameterPosterior_(parameterPosterior)
  , parameterMAP_(parameterMAP)
  , observationsError_(observationsError)
  , inputObservations_(inputObservations)
  , outputObservations_(outputObservations)
  , residualFunction_(residualFunction)
  , bayesian_(bayesian)
{
  // compute output at prior/posterior mean using the fact that model(inputObs)|p = residualFunction(p) + outputObs as the model is not available
  SampleImplementation outputAtPriorMean(outputObservations_.getSize(), outputObservations_.getDimension());
  outputAtPriorMean.setData(residualFunction_(parameterPrior_.getMean()) + outputObservations_.getImplementation()->getData());
  outputAtPriorMean_ = outputAtPriorMean;

  SampleImplementation outputAtPosteriorMean(outputObservations_.getSize(), outputObservations_.getDimension());
  outputAtPosteriorMean.setData(residualFunction_(parameterPosterior_.getMean()) + outputObservations_.getImplementation()->getData());
  outputAtPosteriorMean_ = outputAtPosteriorMean;

  // Set default colors
  const Description colors = DrawableImplementation::BuildDefaultPalette(3);
  priorColor_ = colors[0];
  posteriorColor_ = colors[1];
  observationColor_ = colors[2];
}


/* Virtual constructor */
CalibrationResult * CalibrationResult::clone() const
{
  return new CalibrationResult(*this);
}

/* Parameter prior accessors */
Distribution CalibrationResult::getParameterPrior() const
{
  return parameterPrior_;
}

void CalibrationResult::setParameterPrior(const Distribution & parameterPrior)
{
  parameterPrior_ = parameterPrior;
}

/* Parameter posterior accessors */
Distribution CalibrationResult::getParameterPosterior() const
{
  return parameterPosterior_;
}

void CalibrationResult::setParameterPosterior(const Distribution & parameterPosterior)
{
  parameterPosterior_ = parameterPosterior;
}

/* Parameter MAP accessors */
Point CalibrationResult::getParameterMAP() const
{
  return parameterMAP_;
}

void CalibrationResult::setParameterMAP(const Point & parameterMAP)
{
  parameterMAP_ = parameterMAP;
}

/* Observations error accessors */
Distribution CalibrationResult::getObservationsError() const
{
  return observationsError_;
}

void CalibrationResult::setObservationsError(const Distribution & observationsError)
{
  observationsError_ = observationsError;
}

/* Input observations accessors */
Sample CalibrationResult::getInputObservations() const
{
  return inputObservations_;
}

void CalibrationResult::setInputObservations(const Sample & inputObservations)
{
  inputObservations_ = inputObservations;
}

/* Output observations accessors */
Sample CalibrationResult::getOutputObservations() const
{
  return outputObservations_;
}

void CalibrationResult::setOutputObservations(const Sample & outputObservations)
{
  outputObservations_ = outputObservations;
}

/* Residual function accessors */
Function CalibrationResult::getResidualFunction() const
{
  return residualFunction_;
}

void CalibrationResult::setResidualFunction(const Function & residualFunction)
{
  residualFunction_ = residualFunction;
}

/* String converter */
String CalibrationResult::__repr__() const
{
  OSS oss;
  oss << "class=" << CalibrationResult::GetClassName()
      << " parameter prior=" << parameterPrior_
      << " parameter posterior=" << parameterPosterior_
      << " parameter MAP=" << parameterMAP_
      << " observations error=" << observationsError_
      << " output observation=" << outputObservations_
      << " residual function=" << residualFunction_
      << " bayesian=" << bayesian_
      << " priorColor_=" << priorColor_
      << " posteriorColor_=" << posteriorColor_
      << " observationColor_=" << observationColor_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void CalibrationResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "parameterPrior_", parameterPrior_ );
  adv.saveAttribute( "parameterPosterior_", parameterPosterior_ );
  adv.saveAttribute( "parameterMAP_", parameterMAP_ );
  adv.saveAttribute( "observationsError_", observationsError_ );
  adv.saveAttribute( "inputObservations_", inputObservations_ );
  adv.saveAttribute( "outputObservations_", outputObservations_ );
  adv.saveAttribute( "residualFunction_", residualFunction_ );
  adv.saveAttribute( "outputAtPriorMean_", outputAtPriorMean_ );
  adv.saveAttribute( "outputAtPosteriorMean_", outputAtPosteriorMean_ );
  adv.saveAttribute( "bayesian_", bayesian_ );
  adv.saveAttribute( "priorColor_", priorColor_ );
  adv.saveAttribute( "posteriorColor_", posteriorColor_ );
  adv.saveAttribute( "observationColor_", observationColor_ );
}

/* Method load() reloads the object from the StorageManager */
void CalibrationResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "parameterPrior_", parameterPrior_ );
  adv.loadAttribute( "parameterPosterior_", parameterPosterior_ );
  adv.loadAttribute( "parameterMAP_", parameterMAP_ );
  adv.loadAttribute( "observationsError_", observationsError_ );
  adv.loadAttribute( "inputObservations_", inputObservations_ );
  adv.loadAttribute( "outputObservations_", outputObservations_ );
  adv.loadAttribute( "residualFunction_", residualFunction_ );
  adv.loadAttribute( "outputAtPriorMean_", outputAtPriorMean_ );
  adv.loadAttribute( "outputAtPosteriorMean_", outputAtPosteriorMean_ );
  if (adv.hasAttribute("bayesian_"))
  {
    adv.loadAttribute( "bayesian_", bayesian_ );
    adv.loadAttribute( "priorColor_", priorColor_ );
    adv.loadAttribute( "posteriorColor_", posteriorColor_ );
    adv.loadAttribute( "observationColor_", observationColor_ );
  }
  else
  {
    const CovarianceMatrix priorCovariance(parameterPrior_.getCovariance());
    bayesian_ = (priorCovariance(0, 0) < SpecFunc::MaxScalar);
    Description colors = DrawableImplementation::BuildDefaultPalette(3);
    priorColor_ = colors[0];
    posteriorColor_ = colors[1];
    observationColor_ = colors[2];
  }
}

void CalibrationResult::setOutputAtPriorAndPosteriorMean(const Sample & outputAtPrior, const Sample & outputAtPosterior)
{
  outputAtPriorMean_ = outputAtPrior;
  outputAtPosteriorMean_ = outputAtPosterior;
}

Sample CalibrationResult::getOutputAtPriorMean() const
{
  return outputAtPriorMean_;
}

Sample CalibrationResult::getOutputAtPosteriorMean() const
{
  return outputAtPosteriorMean_;
}

GridLayout CalibrationResult::drawParameterDistributions() const
{
  const Scalar xRangeMarginFactor = ResourceMap::GetAsScalar("CalibrationResult-xRangeMarginFactor");
  const UnsignedInteger dimension = parameterMAP_.getDimension();
  GridLayout grid(1, dimension);
  const Point initialPoint(getParameterPrior().getMean());
  for (UnsignedInteger j = 0; j < dimension; ++ j)
  {
    const Bool upperRightGraph = (j == dimension - 1);
    Graph graph("", getParameterPrior().getDescription()[j], "", true, "topright");
    if (j == 0)
    {
      // Show the Y title only for the first graph
      graph.setYTitle("PDF");
    }

    // The graph must show:
    // + the full posterior PDF
    // + the full prior PDF if it does not shrink too much the posterior graph
    // + the initial point

    // Dry run: draw everything using the natural parameters
    // posterior
    Drawable postPDF(getParameterPosterior().getMarginal(j).drawPDF().getDrawable(0));
    const Scalar xMinPost = postPDF.getData().getMin()[0];
    const Scalar xMaxPost = postPDF.getData().getMax()[0];

    // initialPoint
    const Scalar xInitialPoint = initialPoint[j];

    // Compute min and max bounds of graphics
    Scalar xMin;
    Scalar xMax;
    if (bayesian_)
    {
      // In the Bayesian framework, only the prior and posterior matters.
      // prior
      Drawable priorPDF(getParameterPrior().getMarginal(j).drawPDF().getDrawable(0));
      const Scalar xMinPrior = priorPDF.getData().getMin()[0];
      const Scalar xMaxPrior = priorPDF.getData().getMax()[0];

      // Now, build the common range
      xMin = std::min(xMinPrior, xMinPost);
      xMax = std::max(xMaxPrior, xMaxPost);
    }
    else
    {
      // In the Least Squares framework, only the initial point and posterior matters.
      // The initial point is just a point: this is why we need a margin here.
      // The prior is flat: ignore it to compute the bounds.
      // Now, build the common range
      const Scalar xMinRaw = std::min(xMinPost, xInitialPoint);
      const Scalar xMaxRaw = std::max(xMaxPost, xInitialPoint);
      const Scalar xScaledRange = xRangeMarginFactor * (xMaxRaw - xMinRaw);
      xMin = xMinRaw - xScaledRange;
      xMax = xMaxRaw + xScaledRange;
    }

    if (bayesian_)
    {
      Drawable priorPDF(getParameterPrior().getMarginal(j).drawPDF(xMin, xMax).getDrawable(0));
      if (upperRightGraph)
      {
        priorPDF.setLegend("Prior");
      }
      else
      {
        priorPDF.setLegend("");
      }
      priorPDF.setColor(priorColor_);
      priorPDF.setLineStyle(ResourceMap::GetAsString("CalibrationResult-PriorLineStyle"));
      graph.add(priorPDF);
    }
    else
    {
      Sample data(1, 2);
      data(0, 0) = xInitialPoint;
      Cloud cloudStartingPoint;
      cloudStartingPoint = Cloud(data);
      cloudStartingPoint.setColor(priorColor_);
      cloudStartingPoint.setPointStyle(ResourceMap::GetAsString("CalibrationResult-PriorPointStyle" ));
      if (upperRightGraph)
      {
        cloudStartingPoint.setLegend("Starting point");
      }
      else
      {
        cloudStartingPoint.setLegend("");
      }
      graph.add(cloudStartingPoint);
    }

    // Now draw everything using the common range
    postPDF = getParameterPosterior().getMarginal(j).drawPDF(xMin, xMax).getDrawable(0);
    if (upperRightGraph)
    {
      if (bayesian_)
      {
        postPDF.setLegend("Posterior");
      }
      else
      {
        postPDF.setLegend("Calibrated");
      }
    }
    else
    {
      postPDF.setLegend("");
    }
    postPDF.setColor(posteriorColor_);
    postPDF.setLineStyle(ResourceMap::GetAsString("CalibrationResult-PosteriorLineStyle"));
    graph.add(postPDF);

    grid.setGraph(0, j, graph);
  }
  return grid;
}

GridLayout CalibrationResult::drawResiduals() const
{
  //
  const UnsignedInteger outputDimension = outputObservations_.getDimension();
  GridLayout grid(1, outputDimension);
  grid.setTitle("Residual analysis");
  const Sample priorResiduals(outputObservations_ - outputAtPriorMean_);
  const Sample postResiduals(outputObservations_ - outputAtPosteriorMean_);
  for (UnsignedInteger j = 0; j < outputDimension; ++ j)
  {
    const Bool upperRightGraph = (j == outputDimension - 1);
    Graph graph("", outputObservations_.getDescription()[j] + " residuals", "PDF", true, "topright");

    // Get the distributions
    const Distribution errorJ(getObservationsError().getMarginal(j));
    const Distribution priorResidualsJ(KernelSmoothing().build(priorResiduals.getMarginal(j)));
    const Distribution postResidualsJ(KernelSmoothing().build(postResiduals.getMarginal(j)));
    // Dry run
    // prior
    Drawable priorPDF(priorResidualsJ.drawPDF().getDrawable(0));
    const Scalar xMinPrior = priorPDF.getData().getMin()[0];
    const Scalar xMaxPrior = priorPDF.getData().getMax()[0];
    // posterior
    Drawable postPDF(postResidualsJ.drawPDF().getDrawable(0));
    const Scalar xMinPost = postPDF.getData().getMin()[0];
    const Scalar xMaxPost = postPDF.getData().getMax()[0];
    // normal
    Drawable obsPDF(errorJ.drawPDF().getDrawable(0));
    const Scalar xMinObs = obsPDF.getData().getMin()[0];
    const Scalar xMaxObs = obsPDF.getData().getMax()[0];
    // Now, build the common range
    const Scalar xMin = std::min(xMinPrior, std::min(xMinPost, xMinObs));
    const Scalar xMax = std::max(xMaxPrior, std::max(xMaxPost, xMaxObs));

    // Now draw everything using the common range
    // observation error
    obsPDF = errorJ.drawPDF(xMin, xMax).getDrawable(0);
    if (upperRightGraph)
    {
      if (bayesian_)
      {
        obsPDF.setLegend("Normal, hypothesis");
      }
      else
      {
        obsPDF.setLegend("Normal, estimated");
      }
    }
    else
    {
      obsPDF.setLegend("");
    }
    obsPDF.setColor(observationColor_);
    obsPDF.setLineStyle(ResourceMap::GetAsString("CalibrationResult-ObservationLineStyle"));
    graph.add(obsPDF);

    // prior
    priorPDF = priorResidualsJ.drawPDF(xMin, xMax).getDrawable(0);
    if (upperRightGraph)
    {
      priorPDF.setLegend("Initial (kernel smoothing)");
    }
    else
    {
      priorPDF.setLegend("");
    }
    priorPDF.setColor(priorColor_);
    priorPDF.setLineStyle(ResourceMap::GetAsString("CalibrationResult-PriorLineStyle"));
    graph.add(priorPDF);

    // posterior
    postPDF = postResidualsJ.drawPDF(xMin, xMax).getDrawable(0);
    if (upperRightGraph)
    {
      postPDF.setLegend("Calibrated (kernel smoothing)");
    }
    else
    {
      postPDF.setLegend("");
    }
    postPDF.setColor(posteriorColor_);
    postPDF.setLineStyle(ResourceMap::GetAsString("CalibrationResult-PosteriorLineStyle"));
    graph.add(postPDF);

    grid.setGraph(0, j, graph);
  }
  return grid;
}


GridLayout CalibrationResult::drawObservationsVsInputs() const
{
  const UnsignedInteger inputDimension = inputObservations_.getDimension();
  const UnsignedInteger outputDimension = outputObservations_.getDimension();
  GridLayout grid(outputDimension, inputDimension);
  const Description xDescription(inputObservations_.getDescription());
  const Description yDescription(outputObservations_.getDescription());
  for (UnsignedInteger i = 0; i < outputDimension; ++ i)
  {
    for (UnsignedInteger j = 0; j < inputDimension; ++ j)
    {
      const Bool upperRightGraph = (i == 0 && j == inputDimension - 1);
      // Only the last row
      String xTitle = (i == outputDimension - 1) ? xDescription[j] : "";
      // Only the first column
      String yTitle = (j == 0) ? yDescription[i] : "";
      Graph graph("", xTitle, yTitle, true, "topright");
      const Sample inputObservations_j(inputObservations_.getMarginal(j));

      // observation
      Cloud obsCloud(inputObservations_j, outputObservations_.getMarginal(i));
      if (upperRightGraph)
      {
        // Set the legend only for the first graph
        obsCloud.setLegend("Observations");
      }
      obsCloud.setColor(observationColor_);
      obsCloud.setPointStyle(ResourceMap::GetAsString("CalibrationResult-ObservationPointStyle" ));
      graph.add(obsCloud);

      // model outputs before calibration
      Cloud priorCloud(inputObservations_j, outputAtPriorMean_.getMarginal(i));
      if (upperRightGraph)
      {
        priorCloud.setLegend("Initial");
      }
      priorCloud.setColor(priorColor_);
      priorCloud.setPointStyle(ResourceMap::GetAsString("CalibrationResult-PriorPointStyle" ));
      graph.add(priorCloud);

      // model outputs after calibration
      Cloud postCloud(inputObservations_j, outputAtPosteriorMean_.getMarginal(i));
      if (upperRightGraph)
      {
        postCloud.setLegend("Calibrated");
      }
      postCloud.setColor(posteriorColor_);
      postCloud.setPointStyle(ResourceMap::GetAsString("CalibrationResult-PosteriorPointStyle" ));
      graph.add(postCloud);

      grid.setGraph(i, j, graph);
    }
  }
  return grid;
}


GridLayout CalibrationResult::drawObservationsVsPredictions() const
{
  const UnsignedInteger outputDimension = outputObservations_.getDimension();
  GridLayout grid(1, outputDimension);
  const Description yDescription(outputObservations_.getDescription());
  for (UnsignedInteger j = 0; j < outputDimension; ++ j)
  {
    Graph graph("", yDescription[j] + " observations", yDescription[j] + " predictions", true, "topleft");
    const Sample outputObservations_j(outputObservations_.getMarginal(j));

    const Bool upperRightGraph = (j == outputDimension - 1);
    // observation diagonal
    Sample diagonalPoints(2, 2);
    diagonalPoints(0, 0) = outputObservations_j.getMin()[0];
    diagonalPoints(0, 1) = diagonalPoints(0, 0);
    diagonalPoints(1, 0) = outputObservations_j.getMax()[0];
    diagonalPoints(1, 1) = diagonalPoints(1, 0);
    Curve diagonal(diagonalPoints);
    diagonal.setColor(observationColor_);
    graph.add(diagonal);

    // predictions before
    Cloud priorCloud(outputObservations_j, outputAtPriorMean_.getMarginal(j));
    if (upperRightGraph)
    {
      priorCloud.setLegend("Initial");
    }
    priorCloud.setColor(priorColor_);
    priorCloud.setPointStyle(ResourceMap::GetAsString("CalibrationResult-PriorPointStyle" ));
    graph.add(priorCloud);

    // predictions after
    Cloud postCloud(outputObservations_j, outputAtPosteriorMean_.getMarginal(j));
    if (upperRightGraph)
    {
      postCloud.setLegend("Calibrated");
    }
    postCloud.setColor(posteriorColor_);
    postCloud.setPointStyle(ResourceMap::GetAsString("CalibrationResult-PosteriorPointStyle" ));
    graph.add(postCloud);

    grid.setGraph(0, j, graph);
  }
  return grid;
}


GridLayout CalibrationResult::drawResidualsNormalPlot() const
{
  const UnsignedInteger outputDimension = outputObservations_.getDimension();
  GridLayout grid(1, outputDimension);
  const Sample postResiduals(outputObservations_ - outputAtPosteriorMean_);
  for (UnsignedInteger j = 0; j < outputDimension; ++ j)
  {
    Graph graph(VisualTest::DrawHenryLine(postResiduals.getMarginal(j)));
    if (j < outputDimension - 1)
    {
      graph.setLegendPosition("");
    }
    if (j > 0)
    {
      graph.setYTitle("");
    }
    graph.setXTitle("Residuals");
    graph.setTitle("");
    grid.setGraph(0, j, graph);
  }
  grid.setTitle("Residuals after calibration vs Gaussian hypothesis");
  return grid;
}


END_NAMESPACE_OPENTURNS
