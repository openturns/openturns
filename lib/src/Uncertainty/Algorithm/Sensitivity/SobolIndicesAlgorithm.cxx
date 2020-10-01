//                                               -*- C++ -*-
/**
 *  @brief SobolIndicesAlgorithm provides capabilities for sensitivity algorithm
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
#include "openturns/SobolIndicesAlgorithm.hxx"
#include "openturns/JansenSensitivityAlgorithm.hxx"
#include "openturns/MartinezSensitivityAlgorithm.hxx"
#include "openturns/MauntzKucherenkoSensitivityAlgorithm.hxx"
#include "openturns/SaltelliSensitivityAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SobolIndicesAlgorithm)

/* Default constructor */
SobolIndicesAlgorithm::SobolIndicesAlgorithm() :
  TypedInterfaceObject<SobolIndicesAlgorithmImplementation>(new SobolIndicesAlgorithmImplementation())
{
  // Nothing to do
}

/* Constructor from an implementation */
SobolIndicesAlgorithm::SobolIndicesAlgorithm(const SobolIndicesAlgorithmImplementation & implementation) :
  TypedInterfaceObject<SobolIndicesAlgorithmImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from a Pointer to an implementation */
SobolIndicesAlgorithm::SobolIndicesAlgorithm(const Implementation & p_implementation) :
  TypedInterfaceObject<SobolIndicesAlgorithmImplementation>(p_implementation)
{
  // Nothing to do
}

/*
 * @brief  Standard constructor
 */
SobolIndicesAlgorithm::SobolIndicesAlgorithm(const Sample & inputDesign,
    const Sample & outputDesign,
    const UnsignedInteger size):
  TypedInterfaceObject<SobolIndicesAlgorithmImplementation>(new MartinezSensitivityAlgorithm(inputDesign, outputDesign, size))
{
  // Nothing to do
}

/** Constructor with parameters */
SobolIndicesAlgorithm::SobolIndicesAlgorithm(const Distribution & distribution,
    const UnsignedInteger size,
    const Function & model,
    const Bool computeSecondOrder):
  TypedInterfaceObject<SobolIndicesAlgorithmImplementation>(new MartinezSensitivityAlgorithm(distribution, size, model, computeSecondOrder))
{
  // Nothing to do
}

/** Constructor with experiment / model parameters */
SobolIndicesAlgorithm::SobolIndicesAlgorithm(const WeightedExperiment & experiment,
    const Function & model,
    const Bool computeSecondOrder):
  TypedInterfaceObject<SobolIndicesAlgorithmImplementation>(new MartinezSensitivityAlgorithm(experiment, model, computeSecondOrder))
{
  // Nothing to do
}

/* First order indices accessor */
Point SobolIndicesAlgorithm::getFirstOrderIndices(const UnsignedInteger marginalIndex) const
{
  return getImplementation()->getFirstOrderIndices(marginalIndex);
}

/* Interval for the first order indices accessor */
Interval SobolIndicesAlgorithm::getFirstOrderIndicesInterval() const
{
  return getImplementation()->getFirstOrderIndicesInterval();
}

/* Second order indices accessor */
SymmetricMatrix SobolIndicesAlgorithm::getSecondOrderIndices(const UnsignedInteger marginalIndex) const
{
  return getImplementation()->getSecondOrderIndices(marginalIndex);
}

/* Total order indices accessor */
Point SobolIndicesAlgorithm::getTotalOrderIndices(const UnsignedInteger marginalIndex) const
{
  return getImplementation()->getTotalOrderIndices(marginalIndex);
}

/* Interval for the total order indices accessor */
Interval SobolIndicesAlgorithm::getTotalOrderIndicesInterval() const
{
  return getImplementation()->getTotalOrderIndicesInterval();
}

/* Aggregated first order indices accessor for multivariate samples */
Point SobolIndicesAlgorithm::getAggregatedFirstOrderIndices() const
{
  return getImplementation()->getAggregatedFirstOrderIndices();
}

/* Aggregated total order indices accessor for multivariate samples */
Point SobolIndicesAlgorithm::getAggregatedTotalOrderIndices() const
{
  return getImplementation()->getAggregatedTotalOrderIndices();
}

/** Method that draw (plot) the sensitivity graph */
Graph SobolIndicesAlgorithm::draw() const
{
  return getImplementation()->draw();
}

/** Method that draw the sensitivity graph of a fixed marginal */
Graph SobolIndicesAlgorithm::draw(UnsignedInteger marginalIndex) const
{
  return getImplementation()->draw(marginalIndex);
}

// Getter for bootstrap size
UnsignedInteger SobolIndicesAlgorithm::getBootstrapSize() const
{
  return getImplementation()->getBootstrapSize();
}

// Setter for bootstrap size
void SobolIndicesAlgorithm::setBootstrapSize(const UnsignedInteger bootstrapSize)
{
  copyOnWrite();
  getImplementation()->setBootstrapSize(bootstrapSize);
}

// Getter for bootstrap confidence level
Scalar SobolIndicesAlgorithm::getConfidenceLevel() const
{
  return getImplementation()->getConfidenceLevel();
}

// Setter for bootstrap confidence level
void SobolIndicesAlgorithm::setConfidenceLevel(const Scalar confidenceLevel)
{
  copyOnWrite();
  getImplementation()->setConfidenceLevel(confidenceLevel);
}

void SobolIndicesAlgorithm::setUseAsymptoticDistribution(Bool useAsymptoticDistribution)
{
  copyOnWrite();
  getImplementation()->setUseAsymptoticDistribution(useAsymptoticDistribution);
}

Bool SobolIndicesAlgorithm::getUseAsymptoticDistribution() const
{
  return getImplementation()->getUseAsymptoticDistribution();
}

Distribution SobolIndicesAlgorithm::getFirstOrderIndicesDistribution() const
{
  return getImplementation()->getFirstOrderIndicesDistribution();
}

Distribution SobolIndicesAlgorithm::getTotalOrderIndicesDistribution() const
{
  return getImplementation()->getTotalOrderIndicesDistribution();
}

/* String converter */
String SobolIndicesAlgorithm::__repr__() const
{
  return getImplementation()->__repr__();
}

/* String converter */
String SobolIndicesAlgorithm::__str__(const String & ) const
{
  return getImplementation()->__str__();
}

void SobolIndicesAlgorithm::setDesign(const Sample & inputDesign,
                                      const Sample & outputDesign,
                                      const UnsignedInteger size)
{
  copyOnWrite();
  getImplementation()->setDesign(inputDesign, outputDesign, size);
}


Graph SobolIndicesAlgorithm::DrawSobolIndices(const Description & inputDescription,
    const Point & firstOrderIndices,
    const Point & totalOrderIndices,
    const Interval & firstOrderConfidenceInterval,
    const Interval & totalOrderConfidenceInterval)
{
  return SobolIndicesAlgorithmImplementation::DrawSobolIndices(inputDescription, firstOrderIndices, totalOrderIndices, firstOrderConfidenceInterval, totalOrderConfidenceInterval);
}

Graph SobolIndicesAlgorithm::DrawImportanceFactors(const PointWithDescription & importanceFactors,
    const String & title)
{
  return SobolIndicesAlgorithmImplementation::DrawImportanceFactors(importanceFactors, title);
}

/* ImportanceFactors graph */
Graph SobolIndicesAlgorithm::DrawImportanceFactors(const Point & values,
    const Description & names,
    const String & title)
{
  return SobolIndicesAlgorithmImplementation::DrawImportanceFactors(values, names, title);
}

Graph SobolIndicesAlgorithm::DrawCorrelationCoefficients(const PointWithDescription & correlationCoefficients,
    const String & title)
{
  return SobolIndicesAlgorithmImplementation::DrawCorrelationCoefficients(correlationCoefficients, title);
}

Graph SobolIndicesAlgorithm::DrawCorrelationCoefficients(const Point & values,
    const Description & names,
    const String & title)
{
  return SobolIndicesAlgorithmImplementation::DrawCorrelationCoefficients(values, names, title);
}

END_NAMESPACE_OPENTURNS
