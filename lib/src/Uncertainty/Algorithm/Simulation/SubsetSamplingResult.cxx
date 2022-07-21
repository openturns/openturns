//                                               -*- C++ -*-
/**
 *  @brief Subset sampling result
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/SubsetSamplingResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(SubsetSamplingResult)

static const Factory<SubsetSamplingResult> Factory_SubsetSamplingResult;


/* Default constructor */
SubsetSamplingResult::SubsetSamplingResult()
  : ProbabilitySimulationResult()
{
}


/* Constructor with parameters */
SubsetSamplingResult::SubsetSamplingResult(const RandomVector & event,
    const Scalar probabilityEstimate,
    const Scalar varianceEstimate,
    const UnsignedInteger outerSampling,
    const UnsignedInteger blockSize,
    const Scalar coefficientOfVariation)
  : ProbabilitySimulationResult(event, probabilityEstimate, varianceEstimate, outerSampling, blockSize),
    coefficientOfVariation_(coefficientOfVariation)
{
}


/* Virtual constructor */
SubsetSamplingResult * SubsetSamplingResult::clone() const
{
  return new SubsetSamplingResult(*this);
}


/* Coefficient of variation estimate accessor */
Scalar SubsetSamplingResult::getCoefficientOfVariation() const
{
  return coefficientOfVariation_;
}


/* String converter */
String SubsetSamplingResult::__repr__() const
{
  OSS oss;
  oss << ProbabilitySimulationResult::__repr__();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void SubsetSamplingResult::save(Advocate & adv) const
{
  ProbabilitySimulationResult::save(adv);
  adv.saveAttribute("coefficientOfVariation_", coefficientOfVariation_);
}


/* Method load() reloads the object from the StorageManager */
void SubsetSamplingResult::load(Advocate & adv)
{
  ProbabilitySimulationResult::load(adv);
  adv.loadAttribute("coefficientOfVariation_", coefficientOfVariation_);
}


END_NAMESPACE_OPENTURNS
