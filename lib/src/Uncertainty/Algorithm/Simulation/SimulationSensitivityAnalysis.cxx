//                                               -*- C++ -*-
/**
 *  @brief Implementation of SimulationResult
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
#include "openturns/SimulationSensitivityAnalysis.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/MemoizeFunction.hxx"
#include "openturns/SobolIndicesAlgorithm.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SimulationSensitivityAnalysis)


static const Factory<SimulationSensitivityAnalysis> Factory_SimulationSensitivityAnalysis;

/* Default constructor */
SimulationSensitivityAnalysis::SimulationSensitivityAnalysis()
  : PersistentObject()
{
  // Nothing to do
}

/* Standard constructor */
SimulationSensitivityAnalysis::SimulationSensitivityAnalysis(const RandomVector & event,
    const Sample & inputSample,
    const Sample & outputSample)
  : PersistentObject(),
    inputSample_(inputSample),
    outputSample_(outputSample),
    event_(event)
{
  const UnsignedInteger inputSize = inputSample.getSize();
  const UnsignedInteger inputDimension = inputSample.getDimension();
  // Check if the given sample have compatible size
  if (inputSize != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: the input sample has a size=" << inputSize << " which is not equal to the output sample size=" << outputSample.getSize();
  // Check if the samples are not empty
  if (inputSize == 0) throw InvalidArgumentException(HERE) << "Error: cannot perform analysis based on empty samples.";
  // Check if the iso-probabilistic transformation is compatible with the input sample
  if (inputDimension != getTransformation().getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the given iso-probabilistic transformation has a dimension=" << getTransformation().getInputDimension() << " that is different from the input sample dimension=" << inputDimension;
  // Check if the output sample is uni dimensional
  if (outputSample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given output sample must have a dimension=1, here dimension=" << outputSample.getDimension();
}

/* Standard constructor */
SimulationSensitivityAnalysis::SimulationSensitivityAnalysis(const ProbabilitySimulationResult & result)
  : SimulationSensitivityAnalysis(result.getEvent())
{
  // Nothing to do
}

/* Standard constructor */
SimulationSensitivityAnalysis::SimulationSensitivityAnalysis(const RandomVector & event)
  : PersistentObject(),
    event_(event)
{
  // Inspect the event to see if it is a composite random vector based event
  if (!event.isEvent() || !event.isComposite()) throw InvalidArgumentException(HERE) << "Error: cannot perform a sensitivity analysis based on the given event. Check if it is based on a composite random vector.";
  // Get the input/output sample from the model
  MemoizeFunction model(event.getFunction());
  inputSample_  = model.getInputHistory();
  // Check if the samples are not empty
  if (inputSample_.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot perform analysis based on empty samples.";
  // We are sure that the output sample has the same size as the input sample
  outputSample_ = model.getOutputHistory();
  inputSample_.setDescription(event.getImplementation()->getAntecedent().getDistribution().getDescription());
}

/* Virtual constructor */
SimulationSensitivityAnalysis * SimulationSensitivityAnalysis::clone() const
{
  return new SimulationSensitivityAnalysis(*this);
}

/* Mean point in event domain computation */
Point SimulationSensitivityAnalysis::computeMeanPointInEventDomain(const Scalar threshold) const
{
  const UnsignedInteger inputSize = inputSample_.getSize();
  Sample filteredSample(0, inputSample_.getDimension());
  // Filter the input points with respect to the considered event
  for (UnsignedInteger i = 0; i < inputSize; ++i)
    if (getComparisonOperator()(outputSample_(i, 0), threshold)) filteredSample.add(inputSample_[i]);
  if (filteredSample.getSize() == 0) throw NotDefinedException(HERE) << "Error: cannont compute the mean point if no point is in the event domain.";
  return filteredSample.computeMean();
}

Point SimulationSensitivityAnalysis::computeMeanPointInEventDomain() const
{
  return computeMeanPointInEventDomain(getThreshold());
}

/* Importance factors computation */
PointWithDescription SimulationSensitivityAnalysis::computeImportanceFactors(const Scalar threshold) const
{
  PointWithDescription result(getTransformation()(computeMeanPointInEventDomain(threshold)).normalizeSquare());
  result.setDescription(inputSample_.getDescription());
  return result;
}

PointWithDescription SimulationSensitivityAnalysis::computeImportanceFactors() const
{
  return computeImportanceFactors(getThreshold());
}

PointWithDescription SimulationSensitivityAnalysis::computeEventProbabilitySensitivity() const
{
  const UnsignedInteger dimension = inputSample_.getDimension();
  const UnsignedInteger size = inputSample_.getSize();

  // only marginal parameters are handled here as the pdf gradient of copulas is not implemented
  Description description(event_.getImplementation()->getAntecedent().getDistribution().getParameterDescription());
  UnsignedInteger parameterDimension = 0;
  Collection<Distribution> marginals(dimension);
  for (UnsignedInteger j = 0; j < dimension; ++ j)
  {
    marginals[j] = event_.getImplementation()->getAntecedent().getDistribution().getMarginal(j);
    parameterDimension += marginals[j].getParameter().getSize();
  }
  // remove copula parameters
  description.erase(description.begin() + parameterDimension, description.end());
  Point sumGrad(parameterDimension);
  const ComparisonOperator op(getComparisonOperator());
  const Scalar threshold = getThreshold();
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    if (op(outputSample_(i, 0), threshold))
    {
      Point grad(parameterDimension);
      UnsignedInteger index = 0;
      for (UnsignedInteger j = 0; j < dimension; ++ j)
      {
        Point gradj(marginals[j].computeLogPDFGradient(Point(1, inputSample_(i, j))));
        std::copy(gradj.begin(), gradj.end(), grad.begin() + index);
        index += gradj.getDimension();
      }
      sumGrad += grad / size;
    }
  }
  PointWithDescription sensitivity(sumGrad);
  sensitivity.setDescription(description);
  return sensitivity;
}

/* Importance factors drawing */
Graph SimulationSensitivityAnalysis::drawImportanceFactors() const
{
  OSS oss;
  oss << "Importance Factors from Simulation - " << outputSample_.getDescription()[0];
  return SobolIndicesAlgorithm::DrawImportanceFactors(computeImportanceFactors(), oss.str());

}

Graph SimulationSensitivityAnalysis::drawImportanceFactorsRange(const Bool probabilityScale,
    const Scalar lower,
    const Scalar upper) const
{
  // Here we choose if we have to go forward or backward through the data
  // True if < or <=
  const Bool goForward = getComparisonOperator()(0.0, 1.0);
  // True if > or >=
  const Bool goBackward = getComparisonOperator()(1.0, 0.0);
  // If both are false, the comparison operator checks for equality, for which the method is not implemented
  if ( (!goForward) && (!goBackward) ) throw InternalException(HERE) << "Error: the drawImportanceFactorsRange is not implemented for an equality comparison operator.";
  // Load the preconized sample margin to avoid too noisy estimate of importance factors
  const UnsignedInteger sampleMargin = ResourceMap::GetAsUnsignedInteger("SimulationSensitivityAnalysis-DefaultSampleMargin");
  const UnsignedInteger size = inputSample_.getSize();
  if (sampleMargin >= size / 2) throw InternalException(HERE) << "Error: the default sample margin must be in less than half of the sample size, here sample margin=" << sampleMargin << " and sample size=" << size << ". Check the SimulationSensitivityAnalysis-DefaultSampleMargin key value in ResourceMap.";
  // iStart is the first index of the data to consider
  SignedInteger iStart = 0;
  // iStop is the first index outside of the data
  SignedInteger iStop = size;
  // iStartDrawing is the first index of the data to draw
  SignedInteger iStartDrawing = sampleMargin;
  // iStopDrawing is the first index the is not drawn
  SignedInteger iStopDrawing = size - iStartDrawing;
  SignedInteger step = 1;
  if (goBackward)
  {
    iStart = size - 1;
    iStop = -1;
    iStartDrawing = size - sampleMargin - 1;
    iStopDrawing = int(sampleMargin) - 1;
    step = -1;
  }
  // Here, we must take the ties into account in order to get an algorithm that is
  // both correct AND efficient.
  // The best way I found is to aggregate the input and output samples in order to sort all the data wrt the output value
  // Note on the memory management:
  // + We decided to store the data into two separate samples, one for the input (dimension d), one for the output (dimension 1)
  // + We want to produce a set of curve showing the evolution of each importance factor with respect either to a threshold value (for all the comparison operators) or to a probability (only for the weak or strict ordering operators)
  // + The algorithm must duplicate the data at least because of the iso-probabilistic transformation
  // + In fact, each curve embeds its data, so the input sample is duplicated and the output data is copied d times
  // + In the case of ties in the output sample, the data stored in the curves are shorter than the initial data
  const UnsignedInteger inputDimension = inputSample_.getDimension();
  Sample mergedSample(size, inputDimension + 1);
  // Use the loop to compute the number of points that compares favorably
  // to the internal threshold
  UnsignedInteger good = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    for (UnsignedInteger j = 0; j < inputDimension; ++j) mergedSample(i, j) = inputSample_(i, j);
    mergedSample[i][inputDimension] = outputSample_(i, 0);
    good += getComparisonOperator()(outputSample_(i, 0), getThreshold());
  }
  if ((good < sampleMargin) || (good >= size - sampleMargin)) LOGWARN(OSS() << "Warning: the default threshold does not correspond to well-estimated importance factors according to the default sample margin. The number of points defining the event is " << good << " and should be in [" << sampleMargin << ", " << size - sampleMargin - 1 << "] according to the SimulationSensitivityAnalysis-DefaulSampleMargin key value in ResourceMap.");
  // Sort the merged sample according to its last component
  mergedSample = mergedSample.sortAccordingToAComponent(inputDimension);
  // Prepare the data for the curves
  Collection<Sample> dataCollection(inputDimension, Sample(0, 2));
  // Now, we can go through the data and accumulate the importance factors. If we just call the computeImportanceFactors() method directly, the cost is O(size^2), which is too expensive for typical situations.
  // Aggregate the points in the event
  Point accumulator(inputDimension);
  // Here, we cannot use a simple loop as we have to deal with ties
  SignedInteger i = iStart;
  UnsignedInteger accumulated = 0;
  Bool mustDraw = false;
  while (i != iStopDrawing)
  {
    SignedInteger iThreshold = i;
    Scalar currentThreshold = mergedSample[iThreshold][inputDimension];
    // First, search for a valid threshold, ie one that needs to accumulate more points than the ones already accumulated
    while (!getComparisonOperator()(mergedSample[i][inputDimension], currentThreshold))
    {
      // Accumulate the current threshold candidate, as it will be accepted as soon as a valid threshold will be found
      const Point current(mergedSample[iThreshold]);
      for (UnsignedInteger j = 0; j < inputDimension; ++j) accumulator[j] += current[j];
      ++accumulated;
      iThreshold += step;
      // Exit if no other meaningful threshold is available
      if (iThreshold == iStopDrawing) break;
      currentThreshold = mergedSample[iThreshold][inputDimension];
    }
    // Here, either we have iThreshold == iStopDrawing, in which case there is no other point to add to the graph (for example, the largest values are all equal and we compare using <), or we found a valid new value for the threshold and the associated index iThreshold
    if (iThreshold == iStopDrawing) break;
    // The accumulator hes accumulated all the points that didn't compare with the previous threshold value, which means that there are no remaining points if the comparison operator is strict, or there can be additional points to accumulate if the operator is not strict. We have to accumulate all the points associated with a value equals to this threshold
    // i is the index associated with the last point having a value equal to the threshold. It is iThreshold if the comparison is strict.
    i = iThreshold;
    if (getComparisonOperator()(currentThreshold, currentThreshold))
    {
      SignedInteger iTies = iThreshold;
      while (getComparisonOperator()(mergedSample[iTies][inputDimension], currentThreshold))
      {
        // Accumulate the current threshold
        const Point current(mergedSample[iTies]);
        for (UnsignedInteger j = 0; j < inputDimension; ++j) accumulator[j] += current[j];
        ++accumulated;
        iTies += step;
        // Exit if no other point is available. We have to take into account points of index possibly larger than iStopDrawing because the current threshold could have been reached before index iStopDrawing but could stay the current value for indices after iStopDrawing
        if (iTies == iStop) break;
      } // Accumulate points associated with a value equal to the threshold
      // i is the index associated with the last point having a value equal to the threshold. It is iTies - 1 if the comparison is not strict.
      i = iTies;
    }
    // We must draw the point if the first index associated with the threshold value is equal or after (greater if step = 1, less if step = -1) iStartDrawing.
    mustDraw |= (iThreshold == iStartDrawing);
    if (mustDraw)
    {
      // Now, augmente the data in the collection
      // Initialize with values associated with probabilityScale == false
      Scalar xValue = currentThreshold;
      // Change the values if probabilityScale == true
      if (probabilityScale) xValue = Scalar(accumulated) / size;
      // Check if the point is in the exploration range
      if ((xValue >= lower) && (xValue <= upper))
      {
        Point importanceFactors;
        // Check if the importance factors are well-defined for the current threshold
        try
        {
          importanceFactors = getTransformation()(accumulator / accumulated).normalizeSquare();
          const Point ref(computeImportanceFactors(currentThreshold));
          for (UnsignedInteger j = 0; j < inputDimension; ++j)
          {
            Point point(2);
            point[0] = xValue;
            point[1] = 100.0 * importanceFactors[j];
            dataCollection[j].add(point);
          }
        }
        catch (...)
        {
          String msg("Warning! The importance factors associated with the");
          if (probabilityScale) msg += " probability level ";
          else msg += " threshold ";
          msg = String(OSS() << msg << xValue << " are not defined");
          LOGWARN(msg);
        }
      } // Within range
    } // mustDraw
  } // while (i != iStopDrawing)
  // Initialize with values associated with probabilityScale == false
  String xLabel("threshold");
  Scalar internalX = getThreshold();
  // Change the values if probabilityScale == true
  if (probabilityScale)
  {
    xLabel = "probability";
    internalX = static_cast<Scalar>(good) / size;
  }
  Graph graph("Importance factors range", xLabel, "Importance (%)", true, "topright");
  const Description colors(Drawable::BuildDefaultPalette(inputDimension));
  for (UnsignedInteger j = 0; j < inputDimension; ++j)
  {
    Curve curve(dataCollection[j]);
    curve.setColor(colors[j]);
    curve.setLegend(inputSample_.getDescription()[j]);
    graph.add(curve);
  }
  // Highlight the default threshold importance factors if stable enough
  if ((internalX >= lower) && (internalX <= upper) && (good >= sampleMargin) && (good < size - sampleMargin))
  {
    Sample data(2, 2);
    data(0, 0) = internalX;
    data(0, 1) = 0.0;
    data(1, 0) = internalX;
    data(1, 1) = 100.0;
    Curve curve(data);
    curve.setLineStyle("dashed");
    curve.setLineWidth(2);
    curve.setColor("red");
    curve.setLegend("current thres.");
    graph.add(curve);
  }
  return graph;
}

/* Input sample accessors */
Sample SimulationSensitivityAnalysis::getInputSample() const
{
  return inputSample_;
}

/* Output sample accessors */
Sample SimulationSensitivityAnalysis::getOutputSample() const
{
  return outputSample_;
}

/* Threshold accessors */
Scalar SimulationSensitivityAnalysis::getThreshold() const
{
  return event_.getThreshold();
}

/* Comparison operator accessors */
ComparisonOperator SimulationSensitivityAnalysis::getComparisonOperator() const
{
  return event_.getOperator();
}

/* Iso-probabilistic transformation accessor */
SimulationSensitivityAnalysis::IsoProbabilisticTransformation SimulationSensitivityAnalysis::getTransformation() const
{
  return event_.getImplementation()->getAntecedent().getDistribution().getIsoProbabilisticTransformation();
}

/* String converter */
String SimulationSensitivityAnalysis::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " inputSample=" << inputSample_
      << " outputSample=" << outputSample_
      << " event=" << event_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SimulationSensitivityAnalysis::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("inputSample_", inputSample_);
  adv.saveAttribute("outputSample_", outputSample_);
  adv.saveAttribute("event_", event_);
}

/* Method load() reloads the object from the StorageManager */
void SimulationSensitivityAnalysis::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("inputSample_", inputSample_);
  adv.loadAttribute("outputSample_", outputSample_);
  adv.loadAttribute("event_", event_);
}

END_NAMESPACE_OPENTURNS
