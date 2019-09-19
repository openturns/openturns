//                                               -*- C++ -*-
/**
 *  @brief SobolIndices provides data structure for Sobol indices
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/SobolIndices.hxx"
#include "openturns/MartinezSobolIndices.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SobolIndices)

/*
 * @brief  Default constructor
 */
SobolIndices::SobolIndices() :
  TypedInterfaceObject<SobolIndicesImplementation>(new MartinezSobolIndices())
{
  // Nothing to do
}

/*
 * @brief  Standard constructor
 */
SobolIndices::SobolIndices(const UnsignedInteger modelInputDimension,
                           const UnsignedInteger modelOutputDimension) :
  TypedInterfaceObject<SobolIndicesImplementation>(new MartinezSobolIndices(modelInputDimension, modelOutputDimension))
{
  // Nothing to do
}

/* Constructor from an implementation */
SobolIndices::SobolIndices(const SobolIndicesImplementation & implementation) :
  TypedInterfaceObject<SobolIndicesImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from a Pointer to an implementation */
SobolIndices::SobolIndices(const Implementation & p_implementation) :
  TypedInterfaceObject<SobolIndicesImplementation>(p_implementation)
{
  // Nothing to do
}

void SobolIndices::SetModelDimensions(const UnsignedInteger modelInputDimension,
                                      const UnsignedInteger modelOutputDimension)
{
  return getImplementation()->SetModelDimensions(modelInputDimension, modelOutputDimension);
}

UnsignedInteger SobolIndices::GetModelInputDimensions()
{
  return getImplementation()->GetModelInputDimensions();
}

UnsignedInteger SobolIndices::GetModelOutputDimensions()
{
  return getImplementation()->GetModelOutputDimensions();
}

void SobolIndices::computeIndices(const Sample inputSample)
{
  return getImplementation()->computeIndices(inputSample);
}

void SobolIndices::incrementIndices(const Sample inputSample)
{
  return getImplementation()->incrementIndices(inputSample);
}

Point SobolIndices::getMean() const
{
  return getImplementation()->getMean();
}

Point SobolIndices::getVariance() const
{
  return getImplementation()->getVariance();
}

Point SobolIndices::getFirstOrderIndices(const UnsignedInteger marginalIndex) const
{
  return getImplementation()->getFirstOrderIndices(marginalIndex);
}

Point SobolIndices::getTotalOrderIndices(const UnsignedInteger marginalIndex) const
{
  return getImplementation()->getTotalOrderIndices(marginalIndex);
}

Interval SobolIndices::getFirstOrderIndicesInterval() const
{
  return getImplementation()->getFirstOrderIndicesInterval();
}

Interval SobolIndices::getTotalOrderIndicesInterval() const
{
  return getImplementation()->getTotalOrderIndicesInterval();
}

Point SobolIndices::getAggregatedFirstOrderIndices() const
{
  return getImplementation()->getAggregatedFirstOrderIndices();
}

Point SobolIndices::getAggregatedTotalOrderIndices() const
{
  return getImplementation()->getAggregatedTotalOrderIndices();
}

/* String converter */
String SobolIndices::__repr__() const
{
  return getImplementation()->__repr__();
}

/* String converter */
String SobolIndices::__str__(const String & ) const
{
  return getImplementation()->__str__();
}

END_NAMESPACE_OPENTURNS
