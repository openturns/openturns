//                                               -*- C++ -*-
/**
 *  @brief Simulation algorithm to estimate an expectation
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ExpectationSimulationAlgorithm.hxx"
#include "openturns/Log.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Point.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Normal.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class ExpectationSimulationAlgorithm
 */

CLASSNAMEINIT(ExpectationSimulationAlgorithm)

static const Factory<ExpectationSimulationAlgorithm> Factory_ExpectationSimulationAlgorithm;

/** For save/load mechanism */
ExpectationSimulationAlgorithm::ExpectationSimulationAlgorithm()
  : SimulationAlgorithm()
  , coefficientOfVariationCriterionType_(ResourceMap::GetAsString("ExpectationSimulationAlgorithm-DefaultCoefficientOfVariationCriterionType"))
  , standardDeviationCriterionType_(ResourceMap::GetAsString("ExpectationSimulationAlgorithm-DefaultStandardDeviationCriterionType"))
{
//   setVerbose(verbose);
}

/* Constructor with parameters */
ExpectationSimulationAlgorithm::ExpectationSimulationAlgorithm(const RandomVector & randomVector)
  : SimulationAlgorithm()
  , randomVector_(randomVector)
  , coefficientOfVariationCriterionType_(ResourceMap::GetAsString("ExpectationSimulationAlgorithm-DefaultCoefficientOfVariationCriterionType"))
  , standardDeviationCriterionType_(ResourceMap::GetAsString("ExpectationSimulationAlgorithm-DefaultStandardDeviationCriterionType"))
{
//   setVerbose(verbose);
}

/* Virtual constructor */
ExpectationSimulationAlgorithm * ExpectationSimulationAlgorithm::clone() const
{
  return new ExpectationSimulationAlgorithm(*this);
}

/*  RandomVector accessor */
RandomVector ExpectationSimulationAlgorithm::getRandomVector() const
{
  return randomVector_;
}

/* Result accessor */
void ExpectationSimulationAlgorithm::setResult(const ExpectationSimulationResult & result)
{
  result_ = result;
}

/* Result accessor */
ExpectationSimulationResult ExpectationSimulationAlgorithm::getResult() const
{
  return result_;
}

/* String converter */
String ExpectationSimulationAlgorithm::__repr__() const
{
  OSS oss;
  oss << "class=" << ExpectationSimulationAlgorithm::GetClassName()
      << " randomVector=" << randomVector_
      << " maximumOuterSampling=" << getMaximumOuterSampling()
      << " coefficientOfVariationCriterionType=" << getCoefficientOfVariationCriterionType()
      << " maximumCoefficientOfVariation=" << getMaximumCoefficientOfVariation()
      << " standardDeviationCriterionType=" << getStandardDeviationCriterionType()
      << " maximumStandardDeviation=" << getMaximumStandardDeviation()
      << " maximumStandardDeviationPerComponent=" << getMaximumStandardDeviationPerComponent()
      << " blockSize=" << getBlockSize();
  return oss;
}

/* Performs the actual computation. */
void ExpectationSimulationAlgorithm::run()
{
  const UnsignedInteger dimension = randomVector_.getDimension();

  // First, reset the convergence history
  convergenceStrategy_.setDimension(2 * dimension);

  UnsignedInteger outerSampling = 0;
  Point meanEstimate(dimension);
  Point varianceEstimate(dimension);
  const UnsignedInteger blockSize = getBlockSize();
  // Initialize the result. We use the accessors in order to preserve the exact nature of the result (SimulationResult or QuasiMonteCarloResult)
  // First, the invariant part
  // For the event, we have to access to the implementation as the interface does not provide the setEvent() method ON PURPOSE!
  result_.setRandomVector(randomVector_);
  result_.setBlockSize(blockSize_);
  // Second, the variant part
  result_.setExpectationEstimate(meanEstimate);
  result_.setVarianceEstimate(varianceEstimate);
  result_.setOuterSampling(outerSampling);

  Bool stop = false;
  // We loop if there remains some outer sampling and the coefficient of variation is greater than the limit or has not been computed yet.
  while ((outerSampling < getMaximumOuterSampling()) && !stop)
  {
    // Perform a block of simulation
    const Sample blockSample(randomVector_.getSample(blockSize));
    LOGDEBUG(OSS() << "ExpectationSimulationAlgorithm::run: blockSample=\n" << blockSample);
    ++ outerSampling;
    // Then, actualize the estimates
    const Point meanBlock(blockSample.computeMean());
    // avoid bias in the incremental function (not using computeCovariance)
    Point varianceBlock(blockSample.computeVariance() * (blockSize - 1.0) / blockSize);
    // Let Skp be the empirical variance of a sample of size k*p
    // Let Mkp be the empirical mean of a sample of size k*p
    // Let Sp be the empirical variance of a sample of size p
    // Let Mp be the empirical mean of a sample of size p
    // Then, the empirical variance of the concatenated sample of size (k+1)*p is
    // S(k+1)p = (Sp + k * Skp) / (k + 1) + k * (Mkp - Mp)^2 / (k + 1)^2
    // and the empirical mean of the concatenated sample of size (k+1)*p is
    // M(k+1)p = (Mp + k * Mkp) / (k + 1)
    // To avoid integer overflow and double loss of precision, the formulas have to be written the way they are
    const Scalar size = outerSampling;
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      varianceEstimate[j] = (varianceBlock[j] + (size - 1.0) * varianceEstimate[j]) / size + (1.0 - 1.0 / size) * (meanEstimate[j] - meanBlock[j]) * (meanEstimate[j] - meanBlock[j]) / size;
      meanEstimate[j] = (meanBlock[j] + (size - 1.0) * meanEstimate[j]) / size;
    }
    const Point reducedVarianceEstimate(varianceEstimate / (size * blockSize));

    // Update result
    result_.setExpectationEstimate(meanEstimate);
    result_.setVarianceEstimate(reducedVarianceEstimate);
    result_.setOuterSampling(outerSampling);

    // Display the result at each outer sample
    if (getVerbose()) LOGINFO(result_.__repr__());

    // compute criterion on standard deviation
    Point standardDeviation(result_.getStandardDeviation());
    const Scalar standardDeviationCriterion = computeCriterion(standardDeviationCriterionType_, standardDeviation);

    // compute criterion on coefficient of variation
    Point coefficientOfVariation(result_.getCoefficientOfVariation());
    Bool covOk = true;
    for (UnsignedInteger j = 0; (j < dimension) && covOk; ++ j)
      covOk = (coefficientOfVariation[j] > 0.0);
    const Scalar coefficientOfVariationCriterion = covOk ? computeCriterion(coefficientOfVariationCriterionType_, coefficientOfVariation) : SpecFunc::MaxScalar;

    // decide whether we should stop
    if (!stop && (coefficientOfVariationCriterion <= getMaximumCoefficientOfVariation()))
    {
      LOGINFO(OSS() << "Stopped due to maximum coefficient variation criterion:" << coefficientOfVariationCriterion);
      stop = true;
    }
    if (!stop && (standardDeviationCriterion <= getMaximumStandardDeviation()))
    {
      LOGINFO(OSS() << "Stopped due to maximum standard deviation criterion:" << standardDeviationCriterion);
      stop = true;
    }
    for (UnsignedInteger j = 0; j < maximumStandardDeviationPerComponent_.getDimension(); ++ j)
    {
      if (!stop && (standardDeviation[j] <= maximumStandardDeviationPerComponent_[j]))
      {
        LOGINFO(OSS() << "Stopped due to maximum standard deviation criterion on component j=" << j << " sigma=" << standardDeviation[j]);
        stop = true;
        break;
      }
    }

    // Update the history
    Point convergencePoint(meanEstimate);
    convergencePoint.add(Point(dimension, -1.0));
    // Get the variance estimate from the result in order to deal with simulation
    // methods that do not provide variance estimate (conventional value: -1.0)
    // It is checked using the value of the standard deviation
    for (UnsignedInteger j = 0; j < dimension; ++ j)
      if (standardDeviation[j] >= 0.0)
        convergencePoint[dimension + j] = reducedVarianceEstimate[j];
    convergenceStrategy_.store(convergencePoint);

    // callbacks
    if (progressCallback_.first)
    {
      progressCallback_.first((100.0 * outerSampling) / getMaximumOuterSampling(), progressCallback_.second);
    }
    if (!stop && stopCallback_.first)
    {
      stop = stopCallback_.first(stopCallback_.second);
      if (stop)
      {
        LOGINFO(OSS() << "Stopped due to user");
      }
    }
  }
}

Scalar ExpectationSimulationAlgorithm::computeCriterion(const String & criterionType, const Point & values)
{
  Scalar criterion = 0.0;
  const UnsignedInteger dimension = values.getDimension();

  if (criterionType == "NONE")
  {
    criterion = SpecFunc::MaxScalar;
  }
  else if (criterionType == "MAX")
  {
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      criterion = std::max(criterion, values[j]);
    }
  }
  else if (criterionType == "NORM1")
  {
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      criterion += std::abs(values[j]) / dimension;
    }
  }
  else if (criterionType == "NORM2")
  {
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      criterion += values[j] * values[j] / dimension;
    }
    criterion = std::sqrt(criterion);
  }
  else
    throw InvalidArgumentException(HERE) << "Invalid criterion:" << criterionType << ", should be one of NONE, MAX, NORM1 or NORM2";
  return criterion;
}


void ExpectationSimulationAlgorithm::setCoefficientOfVariationCriterionType(const String & criterionType)
{
  coefficientOfVariationCriterionType_ = criterionType;
}

String ExpectationSimulationAlgorithm::getCoefficientOfVariationCriterionType() const
{
  return coefficientOfVariationCriterionType_;
}

void ExpectationSimulationAlgorithm::setStandardDeviationCriterionType(const String & criterionType)
{
  standardDeviationCriterionType_ = criterionType;
}

String ExpectationSimulationAlgorithm::getStandardDeviationCriterionType() const
{
  return standardDeviationCriterionType_;
}

/* Maximum of standard deviation on each component */
void ExpectationSimulationAlgorithm::setMaximumStandardDeviationPerComponent(const Point & maximumStandardDeviation)
{
  if (maximumStandardDeviation.getDimension() != randomVector_.getDimension())
    throw InvalidDimensionException(HERE) << "The maximum standard deviation dimension must match the random vector dimension";
  maximumStandardDeviationPerComponent_ = maximumStandardDeviation;
}

Point ExpectationSimulationAlgorithm::getMaximumStandardDeviationPerComponent() const
{
  return maximumStandardDeviationPerComponent_;
}

/* Draw the probability convergence at the given level */
Graph ExpectationSimulationAlgorithm::drawExpectationConvergence(const UnsignedInteger marginalIndex,
    const Scalar level) const
{
  // Check if the given level is in ]0, 1[
  if ((level <= 0.0) || (level >= 1.0))
    throw InvalidArgumentException(HERE) << "Confidence level must be in ]0, 1[";

  const Sample convergenceSample(convergenceStrategy_.getSample());
  const UnsignedInteger dimension = convergenceSample.getDimension() / 2;
  if (!(marginalIndex < dimension))
    throw InvalidDimensionException (HERE) << "Marginal index must be <" << dimension;
  const UnsignedInteger size = convergenceSample.getSize();
  Sample dataEstimate(size, 2);
  Sample dataLowerBound(0, 2);
  Sample dataUpperBound(0, 2);
  for (UnsignedInteger i = 0; i < size; i++)
  {
    const Scalar expectationEstimate = convergenceSample(i, marginalIndex);
    const Scalar varianceEstimate = convergenceSample(i, dimension + marginalIndex);
    dataEstimate(i, 0) = i + 1;
    dataEstimate(i, 1) = expectationEstimate;
    // The bounds are drawn only if there is a useable variance estimate
    if (varianceEstimate >= 0.0)
    {
      // The probability estimate is asymptotically normal
      const Scalar xq = DistFunc::qNormal(0.5 + 0.5 * level);
      const Scalar confidenceLength = 2.0 * xq * sqrt(varianceEstimate);

      Point pt(2);
      pt[0] = i + 1;
      pt[1] = expectationEstimate - 0.5 * confidenceLength;
      dataLowerBound.add(pt);
      pt[1] = expectationEstimate + 0.5 * confidenceLength;
      dataUpperBound.add(pt);
    }
  }
  const Curve estimateCurve(dataEstimate, "red", "solid", 2, "expectation estimate");
  OSS oss;
  oss << "Expectation convergence graph at level " << level;
  Graph convergenceGraph(oss, "outer iteration", "estimate", true, "topright");
  convergenceGraph.add(estimateCurve);
  const Curve lowerBoundCurve(dataLowerBound, "green", "solid", 1, "bounds");
  const Curve upperBoundCurve(dataUpperBound, "green", "solid", 1, "");
  convergenceGraph.add(lowerBoundCurve);
  convergenceGraph.add(upperBoundCurve);
  return convergenceGraph;
}

/* Method save() stores the object through the StorageManager */
void ExpectationSimulationAlgorithm::save(Advocate & adv) const
{
  SimulationAlgorithm::save(adv);
  adv.saveAttribute("randomVector_", randomVector_);
  adv.saveAttribute("coefficientOfVariationCriterionType_", coefficientOfVariationCriterionType_);
  adv.saveAttribute("standardDeviationCriterionType_", standardDeviationCriterionType_);
  adv.saveAttribute("maximumStandardDeviationPerComponent_", maximumStandardDeviationPerComponent_);
  adv.saveAttribute("result_", result_);
}

/* Method load() reloads the object from the StorageManager */
void ExpectationSimulationAlgorithm::load(Advocate & adv)
{
  SimulationAlgorithm::load(adv);
  adv.loadAttribute("randomVector_", randomVector_);
  adv.loadAttribute("coefficientOfVariationCriterionType_", coefficientOfVariationCriterionType_);
  adv.loadAttribute("standardDeviationCriterionType_", standardDeviationCriterionType_);
  adv.loadAttribute("maximumStandardDeviationPerComponent_", maximumStandardDeviationPerComponent_);
  adv.loadAttribute("result_", result_);
}


END_NAMESPACE_OPENTURNS
