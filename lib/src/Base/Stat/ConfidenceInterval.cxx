//                                               -*- C++ -*-
/**
 *  @brief ConfidenceInterval implements the confidence interval
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "ConfidenceInterval.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(ConfidenceInterval);

static const Factory<ConfidenceInterval> RegisteredFactory;


/* Default constructor */
ConfidenceInterval::ConfidenceInterval(const NumericalScalar firstPt,
                                       const NumericalScalar lastPt)
  : PersistentObject(),
    first_(firstPt),
    last_(lastPt)
{
  if (firstPt > lastPt) throw InvalidArgumentException(HERE);
}

/* Virtual constructor */
ConfidenceInterval * ConfidenceInterval::clone() const
{
  return new ConfidenceInterval(*this);
}

/* Description Accessor */
void ConfidenceInterval::setDescription(const Description & description)
{
  description_ = description;
}

Description ConfidenceInterval::getDescription() const
{
  return description_;
}

/* String converter */
String ConfidenceInterval::__repr__() const
{
  return OSS() << " [" << first_ << "; "
         << last_ << "]";
}

/* Elements accessors */
void ConfidenceInterval::setValues(const NumericalScalar firstPt,
                                   const NumericalScalar lastPt)
{
  if (firstPt > lastPt) throw InvalidArgumentException(HERE);
  first_ = firstPt;
  last_ = lastPt;
}

const NumericalScalar & ConfidenceInterval::getFirst() const
{
  return first_;
}

const NumericalScalar & ConfidenceInterval::getLast() const
{
  return last_;
}

/* Method save() stores the object through the StorageManager */
void ConfidenceInterval::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "first_", first_);
  adv.saveAttribute( "last_",  last_);
  adv.saveAttribute( "description_", description_ );
}

/* Method load() reloads the object from the StorageManager */
void ConfidenceInterval::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "first_", first_);
  adv.loadAttribute( "last_",  last_);
  adv.loadAttribute( "description_", description_ );
}



/* Comparison operator */
Bool ConfidenceInterval::operator ==(const ConfidenceInterval & rhs) const
{
  return ((this->getFirst() == rhs.getFirst()) && (this->getLast() == rhs.getLast()));
}

END_NAMESPACE_OPENTURNS
