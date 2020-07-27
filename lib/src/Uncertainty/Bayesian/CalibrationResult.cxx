//                                               -*- C++ -*-
/**
 *  @brief CalibrationResult stores the result of a CalibrationAlgorithmImplementation
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
  for (UnsignedInteger j = 0; j < dimension; ++ j)
  {
    Graph graph("", getParameterPrior().getDescription()[j], "PDF", true, "topright");

    // prior
    Graph priorPDF(getParameterPrior().getMarginal(j).drawPDF());
    priorPDF.setLegends(Description(1, "Prior"));
    priorPDF.setColors(Description(1, "red"));
    graph.add(priorPDF);

    // posterior
    Graph postPDF(getParameterPosterior().getMarginal(j).drawPDF());
    postPDF.setLegends(Description(1, "Posterior"));
    postPDF.setColors(Description(1, "green"));
    graph.add(postPDF);

    // scale Diracs
    if (getParameterPrior().getMarginal(j).getImplementation()->getClassName() == "Dirac")
    {
      const Scalar postMax = postPDF.getDrawable(0).getData().getMax()[1];
      for (UnsignedInteger i = 0; i < priorPDF.getDrawable(0).getData().getSize(); ++ i)
        if (priorPDF.getDrawable(0).getData()(i, 1) == 1.0)
          priorPDF.getDrawable(0).getData().getImplementation()->operator()(i, 1) = postMax;
    }

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
  Sample priorResiduals(outputObservations_ - outputAtPriorMean_);
  Sample postResiduals(outputObservations_ - outputAtPosteriorMean_);
  for (UnsignedInteger j = 0; j < outputDimension; ++ j)
  {
    Graph graph("Residual analysis", outputObservations_.getDescription()[j] + " residuals", "PDF", true, "topright");

    // prior
    Graph priorPDF(KernelSmoothing().build(priorResiduals.getMarginal(j)).drawPDF());
    priorPDF.setLegends(Description(1, "Initial"));
    priorPDF.setColors(Description(1, "red"));
    graph.add(priorPDF);

    // posterior
    Graph postPDF(KernelSmoothing().build(postResiduals.getMarginal(j)).drawPDF());
    postPDF.setLegends(Description(1, "Calibrated"));
    postPDF.setColors(Description(1, "green"));
    graph.add(postPDF);

    // observation error
    Graph obsPDF(getObservationsError().getMarginal(j).drawPDF());
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

