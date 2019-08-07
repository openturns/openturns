//                                               -*- C++ -*-
/**
 *  @brief Multiple FORM result
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
#include "openturns/MultiFORMResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MultiFORMResult);

static const Factory<PersistentCollection<FORMResult> > Factory_PersistentCollection_FORMResult;
static Factory<MultiFORMResult> Factory_MultiFORMResult;


MultiFORMResult::MultiFORMResult()
  : PersistentObject()
  , eventProbability_(-1.0)
{
  // Nothing to do
}

MultiFORMResult::MultiFORMResult(const FORMResultCollection & collection)
  : PersistentObject()
  , eventProbability_(-1.0)
  , formResultCollection_(collection)
{
  // Nothing to do
}

/* Virtual constructor */
MultiFORMResult * MultiFORMResult::clone() const
{
  return new MultiFORMResult(*this);
}


/* EventProbability accessor */
void MultiFORMResult::setEventProbability(const Scalar eventProbability)
{
  eventProbability_ = eventProbability;
}

/* Generalized reliability index */
Scalar MultiFORMResult::getGeneralisedReliabilityIndex() const
{
  if (formResultCollection_.getSize() == 1)
    return formResultCollection_[0].getGeneralisedReliabilityIndex();
  else
    throw NotDefinedException(HERE) << "Beta is not defined";
}

Scalar MultiFORMResult::getEventProbability() const
{
  return eventProbability_;
}

MultiFORMResult::FORMResultCollection MultiFORMResult::getFORMResultCollection() const
{
  return formResultCollection_;
}

/* String converter */
String MultiFORMResult::__repr__() const
{
  OSS oss;
  oss << "class=" << MultiFORMResult::GetClassName()
      << " eventProbability=" << eventProbability_;
  return oss;
}


/* Method save() stores the object through the StorageManager */
void MultiFORMResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("eventProbability_", eventProbability_);
}


/* Method load() reloads the object from the StorageManager */
void MultiFORMResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("eventProbability_", eventProbability_);
}


END_NAMESPACE_OPENTURNS
