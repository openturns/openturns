//                                               -*- C++ -*-
/**
 *  @brief Result of an expectation simulation
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
#include "openturns/ExpectationSimulationResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SimulationSensitivityAnalysis.hxx"
#include "openturns/Log.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Normal.hxx"
#include "openturns/ComposedDistribution.hxx"
#include "openturns/Dirac.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ExpectationSimulationResult)

static const Factory<ExpectationSimulationResult> Factory_ExpectationSimulationResult;

/* Default constructor */
ExpectationSimulationResult::ExpectationSimulationResult()
  : SimulationResult()
  , expectationEstimate_(0.0)
  , varianceEstimate_(0.0)
{
  // Nothing to do
}

/* Standard constructor */
ExpectationSimulationResult::ExpectationSimulationResult(const RandomVector & randomVector,
    const Point expectationEstimate,
    const UnsignedInteger outerSampling,
    const UnsignedInteger blockSize)
  : SimulationResult(outerSampling, blockSize)
  , randomVector_(randomVector)
  , expectationEstimate_(expectationEstimate)
  , varianceEstimate_(0.0)
{
  // Nothing to do
}

/* Virtual constructor */
ExpectationSimulationResult * ExpectationSimulationResult::clone() const
{
  return new ExpectationSimulationResult(*this);
}

/* Event accessor */
RandomVector ExpectationSimulationResult::getRandomVector() const
{
  return randomVector_;
}

void ExpectationSimulationResult::setRandomVector(const RandomVector & randomVector)
{
  randomVector_ = randomVector;
}

/* Probability estimate accessor */
Point ExpectationSimulationResult::getExpectationEstimate() const
{
  return expectationEstimate_;
}

void ExpectationSimulationResult::setExpectationEstimate(const Point expectationEstimate)
{
  expectationEstimate_ = expectationEstimate;
}

/* Variance estimate accessor */
Point ExpectationSimulationResult::getVarianceEstimate() const
{
  return varianceEstimate_;
}

void ExpectationSimulationResult::setVarianceEstimate(const Point varianceEstimate)
{
  varianceEstimate_ = varianceEstimate;
}

Point ExpectationSimulationResult::getStandardDeviation() const
{
  Point standardDeviation(varianceEstimate_.getDimension(), -1.0);
  for (UnsignedInteger j = 0; j < varianceEstimate_.getDimension(); ++ j)
    if (varianceEstimate_[j] > 0.0)
      standardDeviation[j] = sqrt(varianceEstimate_[j]);
  return standardDeviation;
}

Point ExpectationSimulationResult::getCoefficientOfVariation() const
{
  Point coefficientOfVariation(varianceEstimate_.getDimension(), -1.0);
  for (UnsignedInteger j = 0; j < varianceEstimate_.getDimension(); ++ j)
    if ((varianceEstimate_[j] > 0.0) && (std::abs(expectationEstimate_[j]) > SpecFunc::Precision))
      coefficientOfVariation[j] = sqrt(varianceEstimate_[j]) / std::abs(expectationEstimate_[j]);
  return coefficientOfVariation;
}

/** Distribution of the expectation */
Distribution ExpectationSimulationResult::getExpectationDistribution() const
{
  Scalar minVariance = SpecFunc::MaxScalar;
  for (UnsignedInteger j = 0; j < varianceEstimate_.getDimension(); ++ j)
    if (varianceEstimate_[j] < minVariance)
      minVariance = varianceEstimate_[j];
  Distribution result;
  if (minVariance > 0.0)
  {
    result = Normal(expectationEstimate_, getStandardDeviation(), CorrelationMatrix(varianceEstimate_.getDimension()));
  }
  else
  {
    ComposedDistribution::DistributionCollection coll(varianceEstimate_.getDimension());
    for (UnsignedInteger j = 0; j < varianceEstimate_.getDimension(); ++ j)
      if (varianceEstimate_[j] > 0.0)
        coll[j] = Normal(expectationEstimate_[j], sqrt(varianceEstimate_[j]));
      else
        coll[j] = Dirac(expectationEstimate_[j]);
    result = ComposedDistribution(coll);
  }
  return result;
}

/* String converter */
String ExpectationSimulationResult::__repr__() const
{
  OSS oss;
  oss.setPrecision(6);
  oss << std::scientific
      << "expectationEstimate=" << expectationEstimate_
      << " varianceEstimate=" << varianceEstimate_
      << " outerSampling=" << outerSampling_
      << " blockSize=" << blockSize_;
  return oss;
}


/* Method save() stores the object through the StorageManager */
void ExpectationSimulationResult::save(Advocate & adv) const
{
  SimulationResult::save(adv);
  adv.saveAttribute("randomVector_", randomVector_);
  adv.saveAttribute("expectationEstimate_", expectationEstimate_);
  adv.saveAttribute("varianceEstimate_", varianceEstimate_);
}

/* Method load() reloads the object from the StorageManager */
void ExpectationSimulationResult::load(Advocate & adv)
{
  SimulationResult::load(adv);
  adv.loadAttribute("randomVector_", randomVector_);
  adv.loadAttribute("expectationEstimate_", expectationEstimate_);
  adv.loadAttribute("varianceEstimate_", varianceEstimate_);
}

END_NAMESPACE_OPENTURNS
