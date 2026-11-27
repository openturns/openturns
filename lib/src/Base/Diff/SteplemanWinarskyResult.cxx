//                                             -*- C++ -*-
/**
 *  @brief SteplemanWinarskyResult stores the result of SteplemanWinarsky
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/SteplemanWinarskyResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SteplemanWinarskyResult)

static const Factory<SteplemanWinarskyResult> Factory_SteplemanWinarskyResult;

/* Default constructor */
SteplemanWinarskyResult::SteplemanWinarskyResult()
  : PersistentObject()
{
  // Nothing to do
}

/* Parameters constructor */
SteplemanWinarskyResult::SteplemanWinarskyResult(const Sample & optimalStepCollection,
    const Function & functionWithOptimalStep)
  : PersistentObject()
  , optimalStepCollection_(optimalStepCollection)
  , functionWithOptimalStep_(functionWithOptimalStep)
{
  // Nothing to do
}

/* Virtual constructor */
SteplemanWinarskyResult * SteplemanWinarskyResult::clone() const
{
  return new SteplemanWinarskyResult(*this);
}

/* String converter */
String SteplemanWinarskyResult::__repr__() const
{
  OSS oss;
  oss << "class=" << SteplemanWinarskyResult::GetClassName()
       << " optimalStepCollection=" << optimalStepCollection_
      << " functionWithOptimalStep=" << functionWithOptimalStep_.__repr__();
  return oss;
}

/* Comparison operator */
Bool SteplemanWinarskyResult::operator ==(const SteplemanWinarskyResult & other) const
{
  if (this == &other) return true;
  return (optimalStepCollection_ == other.optimalStepCollection_) && (functionWithOptimalStep_ == other.functionWithOptimalStep_);
}

/* Optimal step accessor */
void SteplemanWinarskyResult::setOptimalStepCollection(const Sample & optimalStepCollection)
{
  optimalStepCollection_ = optimalStepCollection;
}

Sample SteplemanWinarskyResult::getOptimalStepCollection() const
{
  return optimalStepCollection_;
}

Point SteplemanWinarskyResult::getOptimalStep(const UnsignedInteger marginalIndex) const
{
  if (marginalIndex >= optimalStepCollection_.getSize())
    throw InvalidDimensionException(HERE) << "Marginal index " << marginalIndex << " must be less than the output dimension " << optimalStepCollection_.getSize();
  return optimalStepCollection_[marginalIndex];
}

/* Function with optimal finite-difference gradient accessor */
Function SteplemanWinarskyResult::getFunctionWithOptimalStep() const
{
  return functionWithOptimalStep_;
}

/* Method save() stores the object through the StorageManager */
void SteplemanWinarskyResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("optimalStepCollection_", optimalStepCollection_);
  adv.saveAttribute("functionWithOptimalStep_", functionWithOptimalStep_);
}

/* Method load() reloads the object from the StorageManager */
void SteplemanWinarskyResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("optimalStepCollection_", optimalStepCollection_);
  adv.loadAttribute("functionWithOptimalStep_", functionWithOptimalStep_);
}

END_NAMESPACE_OPENTURNS
