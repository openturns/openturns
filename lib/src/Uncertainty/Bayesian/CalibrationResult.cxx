//                                               -*- C++ -*-
/**
 *  @brief CalibrationResult stores the result of a CalibrationAlgorithmImplementation
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
                                     const Function & residualFunction)
  : PersistentObject()
  , parameterPrior_(parameterPrior)
  , parameterPosterior_(parameterPosterior)
  , parameterMAP_(parameterMAP)
  , observationsError_(observationsError)
  , inputObservations_(inputObservations)
  , outputObservations_(outputObservations)
  , residualFunction_(residualFunction)
{
  // Nothing to do
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
  const UnsignedInteger dimension = parameterMAP_.getDimension();
  GridLayout grid(1, dimension);
  const Point candidate(getParameterPrior().getMean());
  for (UnsignedInteger j = 0; j < dimension; ++ j)
  {
    Graph graph("", getParameterPrior().getDescription()[j], "PDF", true, "topright");

    // The graphmust show:
    // + the full posterior PDF
    // + the full prior PDF if it does not shrink too much the posterior graph
    // + the candidate

    // Dry run: draw everything using the natural parameters
    // posterior
    Graph postPDF(getParameterPosterior().getMarginal(j).drawPDF());

    const Scalar xMinPost = postPDF.getDrawable(0).getData().getMin()[0];
    const Scalar xMaxPost = postPDF.getDrawable(0).getData().getMax()[0];
    const Scalar deltaPost = xMaxPost - xMinPost;

    // candidate
    const Scalar xCandidate = candidate[j];
    Sample data(1, 2);
    data(0, 0) = xCandidate;
    Cloud cloudCandidate(data);
    cloudCandidate.setColor("red");
    cloudCandidate.setPointStyle("star");
    cloudCandidate.setLegend("Candidate");

    // prior
    Graph priorPDF(getParameterPrior().getMarginal(j).drawPDF());

    const Scalar xMinPrior = priorPDF.getDrawable(0).getData().getMin()[0];
    const Scalar xMaxPrior = priorPDF.getDrawable(0).getData().getMax()[0];

    // Now, build the common range
    Scalar xMin = std::min(xMinPost, std::max(xMinPrior, xMinPost - 0.5 * deltaPost));
    Scalar xMax = std::max(xMaxPost, std::min(xMaxPrior, xMaxPost + 0.5 * deltaPost));
    const Scalar lowXCandidate = xCandidate - 0.1 * deltaPost;
    if (lowXCandidate < xMin)
    {
      xMin = lowXCandidate;
      xMax = std::max(xMaxPost, xMax - (xMin - lowXCandidate));
    }

    const Scalar highXCandidate = xCandidate + 0.1 * deltaPost;
    if (highXCandidate > xMax)
    {
      xMax = highXCandidate;
      xMin = std::min(xMinPost, xMin - (highXCandidate - xMax));
    }

    // Now draw everything using the common range
    postPDF = getParameterPosterior().getMarginal(j).drawPDF(xMin, xMax);
    postPDF.setLegends(Description(1, "Posterior"));
    postPDF.setColors(Description(1, "green"));

    priorPDF.setLegends(Description(1, "Prior"));
    priorPDF.setColors(Description(1, "red"));
    priorPDF = getParameterPrior().getMarginal(j).drawPDF(xMin, xMax);

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
    Graph priorPDF(KernelSmoothing().build(priorResiduals.getMarginal(j)).drawPDF(xMin, xMax));
    priorPDF.setLegends(Description(1, "Initial"));
    priorPDF.setColors(Description(1, "red"));
    graph.add(priorPDF);

    // posterior
    Graph postPDF(KernelSmoothing().build(postResiduals.getMarginal(j)).drawPDF(xMin, xMax));
    postPDF.setLegends(Description(1, "Calibrated"));
    postPDF.setColors(Description(1, "green"));
    graph.add(postPDF);

    // observation error
    Graph obsPDF(errorJ.drawPDF(xMin, xMax));
    obsPDF.setLegends(Description(1, "Observation errors"));
    obsPDF.setColors(Description(1, "blue"));
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
      obsCloud.setColor("blue");
      graph.add(obsCloud);

      // model outputs before calibration
      Cloud priorCloud(inputObservations_j, outputAtPriorMean_.getMarginal(i));
      priorCloud.setLegend("Initial");
      priorCloud.setColor("red");
      graph.add(priorCloud);

      // model outputs after calibration
      Cloud postCloud(inputObservations_j, outputAtPosteriorMean_.getMarginal(i));
      postCloud.setLegend("Calibrated");
      postCloud.setColor("green");
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
    diagonal.setColor("blue");
    graph.add(diagonal);

    // predictions before
    Cloud priorCloud(outputObservations_j, outputAtPriorMean_.getMarginal(j));
    priorCloud.setLegend("Initial");
    priorCloud.setColor("red");
    graph.add(priorCloud);

    // predictions after
    Cloud postCloud(outputObservations_j, outputAtPosteriorMean_.getMarginal(j));
    postCloud.setLegend("Calibrated");
    postCloud.setColor("green");
    graph.add(postCloud);

    grid.setGraph(0, j, graph);
  }
  return grid;
}



END_NAMESPACE_OPENTURNS

