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
  , priorLineStyle_(ResourceMap::GetAsString( "GaussianResult-PriorLineStyle" ))
  , posteriorLineStyle_(ResourceMap::GetAsString( "GaussianResult-PosteriorLineStyle" ))
  , observationLineStyle_(ResourceMap::GetAsString( "GaussianResult-ObservationLineStyle" ))
{
  // compute output at prior/posterior mean using the fact that model(inputObs)|p = residualFunction(p) + outputObs as the model is not available
  SampleImplementation outputAtPriorMean(outputObservations_.getSize(), outputObservations_.getDimension());
  outputAtPriorMean.setData(residualFunction_(parameterPrior_.getMean()) + outputObservations_.getImplementation()->getData());
  outputAtPriorMean_ = outputAtPriorMean;

  SampleImplementation outputAtPosteriorMean(outputObservations_.getSize(), outputObservations_.getDimension());
  outputAtPosteriorMean.setData(residualFunction_(parameterPosterior_.getMean()) + outputObservations_.getImplementation()->getData());
  outputAtPosteriorMean_ = outputAtPosteriorMean;
  
  // Set default colors
  Description colors = DrawableImplementation::BuildDefaultPalette(3);
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
      << " residual function=" << residualFunction_;
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
  Log::Show(Log::ALL);
  LOGDEBUG(OSS() << "DEBUG:" << bayesian_);
  
  const Scalar xRangeMarginFactor = ResourceMap::GetAsScalar("GaussianResult-xRangeMarginFactor");

  const UnsignedInteger dimension = parameterMAP_.getDimension();
  GridLayout grid(1, dimension);
  const Point candidate(getParameterPrior().getMean());
  for (UnsignedInteger j = 0; j < dimension; ++ j)
  {
    LOGDEBUG(OSS() << "+ Input :" << j);
    Graph graph("", getParameterPrior().getDescription()[j], "PDF", true, "topright");

    // The graph must show:
    // + the full posterior PDF
    // + the full prior PDF if it does not shrink too much the posterior graph
    // + the candidate

    // Dry run: draw everything using the natural parameters
    // posterior
    Drawable postPDF(getParameterPosterior().getMarginal(j).drawPDF().getDrawable(0));

    const Scalar xMinPost = postPDF.getData().getMin()[0];
    const Scalar xMaxPost = postPDF.getData().getMax()[0];
    LOGDEBUG(OSS() << "DEBUG: xMinPost=" << xMinPost);
    LOGDEBUG(OSS() << "DEBUG: xMaxPost=" << xMaxPost);

    // candidate
    const Scalar xCandidate = candidate[j];
    LOGDEBUG(OSS() << "DEBUG: xCandidate=" << xCandidate);
    Sample data(1, 2);
    data(0, 0) = xCandidate;
    Cloud cloudCandidate(data);
    cloudCandidate.setColor(priorColor_);
    cloudCandidate.setPointStyle("star");
    cloudCandidate.setLegend("Candidate");

    // Compute min and max bounds of graphics
    Scalar xMin;
    Scalar xMax;
    if (bayesian_)
    {
        // prior
        Drawable priorPDF(getParameterPrior().getMarginal(j).drawPDF().getDrawable(0));

        const Scalar xMinPrior = priorPDF.getData().getMin()[0];
        const Scalar xMaxPrior = priorPDF.getData().getMax()[0];
        const Scalar xRange = std::max(xMaxPost, std::max(xMaxPrior, xCandidate)) - std::min(xMinPost, std::min(xMinPrior, xCandidate));
        LOGDEBUG(OSS() << "DEBUG: xMinPrior=" << xMinPrior);
        LOGDEBUG(OSS() << "DEBUG: xMaxPrior=" << xMaxPrior);
        LOGDEBUG(OSS() << "DEBUG: xRange=" << xRange);

        // Now, build the common range
        xMin = std::min(xCandidate, std::min(xMinPrior, xMinPost)) - xRangeMarginFactor * xRange;
        xMax = std::max(xCandidate, std::max(xMaxPrior, xMaxPost)) + xRangeMarginFactor * xRange;
    } else {
        // The prior is flat: ignore it to compute the bounds.
        // Now, build the common range
        const Scalar xRange = std::max(xMaxPost, xCandidate) - std::min(xMinPost, xCandidate);
        LOGDEBUG(OSS() << "DEBUG: xRange=" << xRange);
        xMin = std::min(xCandidate, xMinPost) - xRangeMarginFactor * xRange;
        xMax = std::max(xCandidate, xMaxPost) + xRangeMarginFactor * xRange;
    }
    LOGDEBUG(OSS() << "DEBUG: xMin=" << xMin);
    LOGDEBUG(OSS() << "DEBUG: xMax=" << xMax);
    
    // Now draw everything using the common range
    postPDF = getParameterPosterior().getMarginal(j).drawPDF(xMin, xMax).getDrawable(0);
    if (bayesian_)
    {
        postPDF.setLegend("Posterior");
    }
    else
    {
        postPDF.setLegend("After calibration");
    }
    postPDF.setColor(posteriorColor_);
    postPDF.setLineStyle(posteriorLineStyle_);

    Drawable priorPDF(getParameterPrior().getMarginal(j).drawPDF(xMin, xMax).getDrawable(0));
    if (bayesian_)
    {
        priorPDF.setLegend("Prior");
    }
    else
    {
        priorPDF.setLegend("Before calibration");
    }
    priorPDF.setColor(priorColor_);
    priorPDF.setLineStyle(priorLineStyle_);

    // assemble the graphs in the correct order
    graph.add(priorPDF);
    graph.add(postPDF);
    graph.add(cloudCandidate);

    grid.setGraph(0, j, graph);
  }
  return grid;
}

GridLayout CalibrationResult::drawResiduals() const
{
  if (!outputAtPriorMean_.getDimension())
    throw NotDefinedException(HERE) << "Output at prior not available";

  const UnsignedInteger outputDimension = outputObservations_.getDimension();
  GridLayout grid(1, outputDimension);
  const Sample priorResiduals(outputObservations_ - outputAtPriorMean_);
  const Sample postResiduals(outputObservations_ - outputAtPosteriorMean_);
  const Point priorMin(priorResiduals.getMin());
  const Point priorMax(priorResiduals.getMax());
  const Point postMin(postResiduals.getMin());
  const Point postMax(postResiduals.getMax());
  for (UnsignedInteger j = 0; j < outputDimension; ++ j)
  {
    Graph graph("Residual analysis", outputObservations_.getDescription()[j] + " residuals", "PDF", true, "topright");

    const Distribution errorJ(getObservationsError().getMarginal(j));
    const Scalar errorMin = errorJ.computeQuantile(ResourceMap::GetAsScalar("Distribution-QMin"))[0];
    const Scalar errorMax = errorJ.computeQuantile(ResourceMap::GetAsScalar("Distribution-QMax"))[0];
    const Scalar delta = 2.0 * (errorMax - errorMin) * (1.0 - 0.5 * (ResourceMap::GetAsScalar("Distribution-QMax" ) - ResourceMap::GetAsScalar("Distribution-QMin")));
    const Scalar xMin = std::min(priorMin[j], std::min(postMin[j], errorMin - delta));
    const Scalar xMax = std::max(priorMax[j], std::max(postMax[j], errorMax + delta));
    // prior
    Drawable priorPDF(KernelSmoothing().build(priorResiduals.getMarginal(j)).drawPDF(xMin, xMax).getDrawable(0));
    priorPDF.setLegend("Initial");
    priorPDF.setColor(priorColor_);
    priorPDF.setLineStyle(priorLineStyle_);
    graph.add(priorPDF);

    // posterior
    Drawable postPDF(KernelSmoothing().build(postResiduals.getMarginal(j)).drawPDF(xMin, xMax).getDrawable(0));
    postPDF.setLegend("Calibrated");
    postPDF.setColor(posteriorColor_);
    postPDF.setLineStyle(posteriorLineStyle_);
    graph.add(postPDF);

    // observation error
    Drawable obsPDF(errorJ.drawPDF(xMin, xMax).getDrawable(0));
    obsPDF.setLegend("Observation errors");
    obsPDF.setColor(observationColor_);
    obsPDF.setLineStyle(observationLineStyle_);
    graph.add(obsPDF);

    grid.setGraph(0, j, graph);
  }
  return grid;
}


GridLayout CalibrationResult::drawObservationsVsInputs() const
{
  if (!outputAtPriorMean_.getDimension())
    throw NotDefinedException(HERE) << "Output at prior not available";

  const UnsignedInteger inputDimension = inputObservations_.getDimension();
  const UnsignedInteger outputDimension = outputObservations_.getDimension();
  GridLayout grid(outputDimension, inputDimension);
  const Description xDescription(inputObservations_.getDescription());
  const Description yDescription(outputObservations_.getDescription());
  for (UnsignedInteger i = 0; i < outputDimension; ++ i)
  {
    for (UnsignedInteger j = 0; j < inputDimension; ++ j)
    {
      Graph graph("", xDescription[j], yDescription[i], true, "topright");
      const Sample inputObservations_j(inputObservations_.getMarginal(j));

      // observation
      Cloud obsCloud(inputObservations_j, outputObservations_.getMarginal(i));
      obsCloud.setLegend("Observations");
      obsCloud.setColor(observationColor_);
      graph.add(obsCloud);

      // model outputs before calibration
      Cloud priorCloud(inputObservations_j, outputAtPriorMean_.getMarginal(i));
      priorCloud.setLegend("Initial");
      priorCloud.setColor(priorColor_);
      graph.add(priorCloud);

      // model outputs after calibration
      Cloud postCloud(inputObservations_j, outputAtPosteriorMean_.getMarginal(i));
      postCloud.setLegend("Calibrated");
      postCloud.setColor(posteriorColor_);
      graph.add(postCloud);

      grid.setGraph(i, j, graph);
    }
  }
  return grid;
}


GridLayout CalibrationResult::drawObservationsVsPredictions() const
{
  if (!outputAtPriorMean_.getDimension())
    throw NotDefinedException(HERE) << "Output at prior not available";

  const UnsignedInteger outputDimension = outputObservations_.getDimension();
  GridLayout grid(1, outputDimension);
  const Description yDescription(outputObservations_.getDescription());
  const String & priorColor_ = ResourceMap::GetAsString( "GaussianResult-priorColor_" );
  const String & posteriorColor_ = ResourceMap::GetAsString( "GaussianResult-posteriorColor_" );
  const String & observationColor_ = ResourceMap::GetAsString( "GaussianResult-observationColor_" );
  for (UnsignedInteger j = 0; j < outputDimension; ++ j)
  {
    Graph graph("", yDescription[j] + " observations", yDescription[j] + " predictions", true, "topleft");
    const Sample outputObservations_j(outputObservations_.getMarginal(j));

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
    priorCloud.setLegend("Initial");
    priorCloud.setColor(priorColor_);
    graph.add(priorCloud);

    // predictions after
    Cloud postCloud(outputObservations_j, outputAtPosteriorMean_.getMarginal(j));
    postCloud.setLegend("Calibrated");
    postCloud.setColor(posteriorColor_);
    graph.add(postCloud);

    grid.setGraph(0, j, graph);
  }
  return grid;
}



END_NAMESPACE_OPENTURNS

