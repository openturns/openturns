//                                               -*- C++ -*-
/**
 *  @brief Result of a Sobol indices simulation
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
#include "openturns/SobolSimulationResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Curve.hxx"
#include "openturns/SobolIndicesAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SobolSimulationResult)

static const Factory<SobolSimulationResult> Factory_SobolSimulationResult;

/* Default constructor */
SobolSimulationResult::SobolSimulationResult()
  : SimulationResult()
{
  // Nothing to do
}

/* Standard constructor */
SobolSimulationResult::SobolSimulationResult(const Distribution & firstOrderIndicesDistribution,
    const Distribution & totalOrderIndicesDistribution,
    const UnsignedInteger outerSampling,
    const UnsignedInteger blockSize)
  : SimulationResult(outerSampling, blockSize)
  , firstOrderIndicesDistribution_(firstOrderIndicesDistribution)
  , totalOrderIndicesDistribution_(totalOrderIndicesDistribution)
{
  // Nothing to do
}

/* Virtual constructor */
SobolSimulationResult * SobolSimulationResult::clone() const
{
  return new SobolSimulationResult(*this);
}

/* Variance estimate accessor */
Point SobolSimulationResult::getFirstOrderIndicesEstimate() const
{
  return firstOrderIndicesDistribution_.getMean();
}

Point SobolSimulationResult::getTotalOrderIndicesEstimate() const
{
  return totalOrderIndicesDistribution_.getMean();
}

/** Distribution of the expectation */
void SobolSimulationResult::setFirstOrderIndicesDistribution(const Distribution & firstOrderIndicesDistribution)
{
  firstOrderIndicesDistribution_ = firstOrderIndicesDistribution;
}

void SobolSimulationResult::setTotalOrderIndicesDistribution(const Distribution & totalOrderIndicesDistribution)
{
  totalOrderIndicesDistribution_ = totalOrderIndicesDistribution;
}

Distribution SobolSimulationResult::getFirstOrderIndicesDistribution() const
{
  return firstOrderIndicesDistribution_;
}

Distribution SobolSimulationResult::getTotalOrderIndicesDistribution() const
{
  return totalOrderIndicesDistribution_;
}

/* String converter */
String SobolSimulationResult::__repr__() const
{
  OSS oss;
  oss.setPrecision(6);
  oss << std::scientific
      << "firstOrderIndicesDistribution=" << firstOrderIndicesDistribution_
      << " totalOrderIndicesDistribution=" << totalOrderIndicesDistribution_
      << " outerSampling=" << outerSampling_
      << " blockSize=" << blockSize_;
  return oss;
}

Graph SobolSimulationResult::draw(const Scalar confidenceLevel) const
{
  Point fo(getFirstOrderIndicesEstimate());
  Point to(getTotalOrderIndicesEstimate());
  const Interval foInterval(getFirstOrderIndicesDistribution().computeBilateralConfidenceInterval(confidenceLevel));
  const Interval toInterval(getTotalOrderIndicesDistribution().computeBilateralConfidenceInterval(confidenceLevel));
  Graph graph(SobolIndicesAlgorithm::DrawSobolIndices(getFirstOrderIndicesDistribution().getDescription(), fo, to, foInterval, toInterval));
  graph.setTitle(OSS() << "Sobol' indices - " << getClassName());
  return graph;
}

/* Method save() stores the object through the StorageManager */
void SobolSimulationResult::save(Advocate & adv) const
{
  SimulationResult::save(adv);
  adv.saveAttribute("firstOrderIndicesDistribution_", firstOrderIndicesDistribution_);
  adv.saveAttribute("totalOrderIndicesDistribution_", totalOrderIndicesDistribution_);
}

/* Method load() reloads the object from the StorageManager */
void SobolSimulationResult::load(Advocate & adv)
{
  SimulationResult::load(adv);
  adv.loadAttribute("firstOrderIndicesDistribution_", firstOrderIndicesDistribution_);
  adv.loadAttribute("totalOrderIndicesDistribution_", totalOrderIndicesDistribution_);
}

END_NAMESPACE_OPENTURNS
