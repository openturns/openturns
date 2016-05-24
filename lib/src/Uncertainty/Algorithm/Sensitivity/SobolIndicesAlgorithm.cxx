//                                               -*- C++ -*-
/**
 *  @brief SobolIndicesAlgorithm provides capabilities for sensitivity algorithm
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/SobolIndicesAlgorithm.hxx"
#include "openturns/MartinezSensitivityAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SobolIndicesAlgorithm);

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
SobolIndicesAlgorithm::SobolIndicesAlgorithm(const NumericalSample & inputDesign,
                                             const NumericalSample & outputDesign,
                                             const UnsignedInteger size):
  TypedInterfaceObject<SobolIndicesAlgorithmImplementation>(new MartinezSensitivityAlgorithm(inputDesign, outputDesign, size))
{
  // Nothing to do
}

/** Constructor with parameters */
SobolIndicesAlgorithm::SobolIndicesAlgorithm(const Distribution & distribution,
                                             const UnsignedInteger size,
                                             const NumericalMathFunction & model,
                                             const Bool computeSecondOrder):
  TypedInterfaceObject<SobolIndicesAlgorithmImplementation>(new MartinezSensitivityAlgorithm(distribution, size, model, computeSecondOrder))
{
  // Nothing to do
}

/** Constructor with experiment / model parameters */
SobolIndicesAlgorithm::SobolIndicesAlgorithm(const WeightedExperiment & experiment,
                                             const NumericalMathFunction & model,
                                             const Bool computeSecondOrder):
  TypedInterfaceObject<SobolIndicesAlgorithmImplementation>(new MartinezSensitivityAlgorithm(experiment, model, computeSecondOrder))
{
  // Nothing to do
}

/* First order indices accessor */
NumericalPoint SobolIndicesAlgorithm::getFirstOrderIndices(const UnsignedInteger marginalIndex) const
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
NumericalPoint SobolIndicesAlgorithm::getTotalOrderIndices(const UnsignedInteger marginalIndex) const
{
  return getImplementation()->getTotalOrderIndices(marginalIndex);
}

/* Interval for the total order indices accessor */
Interval SobolIndicesAlgorithm::getTotalOrderIndicesInterval() const
{
  return getImplementation()->getTotalOrderIndicesInterval();
}

/* Aggregated first order indices accessor for multivariate samples */
NumericalPoint SobolIndicesAlgorithm::getAggregatedFirstOrderIndices() const
{
  return getImplementation()->getAggregatedFirstOrderIndices();
}

/* Aggregated total order indices accessor for multivariate samples */
NumericalPoint SobolIndicesAlgorithm::getAggregatedTotalOrderIndices() const
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

// Setter for bootstrap number
void SobolIndicesAlgorithm::setBootstrapSize(const UnsignedInteger bootstrapSize)
{
  copyOnWrite();
  getImplementation()->setBootstrapSize(bootstrapSize);
}

// Setter for bootstrap confidence level
void SobolIndicesAlgorithm::setBootstrapConfidenceLevel(const NumericalScalar confidenceLevel)
{
  copyOnWrite();
  getImplementation()->setBootstrapConfidenceLevel(confidenceLevel);
}

/* String converter */
String SobolIndicesAlgorithm::__repr__() const
{
  return getImplementation()->__repr__();
}

/* String converter */
String SobolIndicesAlgorithm::__str__(const String & offset) const
{
  return getImplementation()->__str__();
}


END_NAMESPACE_OPENTURNS
