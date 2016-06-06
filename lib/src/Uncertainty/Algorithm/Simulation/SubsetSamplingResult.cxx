//                                               -*- C++ -*-
/**
 *  @brief Subset sampling result
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
#include "openturns/SubsetSamplingResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(SubsetSamplingResult);

static const Factory<SubsetSamplingResult> Factory_SubsetSamplingResult;


/* Default constructor */
SubsetSamplingResult::SubsetSamplingResult()
: SimulationResultImplementation()
{
}


/* Constructor with parameters */
SubsetSamplingResult::SubsetSamplingResult(const Event & event,
  const NumericalScalar probabilityEstimate,
  const NumericalScalar varianceEstimate,
  const UnsignedInteger outerSampling,
  const UnsignedInteger blockSize,
  const NumericalScalar coefficientOfVariation)
: SimulationResultImplementation(event, probabilityEstimate, varianceEstimate, outerSampling, blockSize),
  coefficientOfVariation_(coefficientOfVariation)
{
}


/* Virtual constructor */
SubsetSamplingResult * SubsetSamplingResult::clone() const
{
  return new SubsetSamplingResult(*this);
}


/* Coefficient of variation estimate accessor */
NumericalScalar SubsetSamplingResult::getCoefficientOfVariation() const
{
  return coefficientOfVariation_;
}


/* String converter */
String SubsetSamplingResult::__repr__() const
{
  OSS oss;
  oss << SimulationResultImplementation::__repr__();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void SubsetSamplingResult::save(Advocate & adv) const
{
  SimulationResultImplementation::save(adv);
  adv.saveAttribute("coefficientOfVariation_", coefficientOfVariation_);
}


/* Method load() reloads the object from the StorageManager */
void SubsetSamplingResult::load(Advocate & adv)
{
  SimulationResultImplementation::load(adv);
  adv.loadAttribute("coefficientOfVariation_", coefficientOfVariation_);
}


END_NAMESPACE_OPENTURNS
