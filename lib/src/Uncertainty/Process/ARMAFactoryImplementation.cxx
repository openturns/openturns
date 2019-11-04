//                                               -*- C++ -*-
/**
 *  @brief An interface for all implementation class of process
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ARMAFactoryImplementation.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ARMAFactoryImplementation)

static const Factory<ARMAFactoryImplementation> Factory_ARMAFactoryImplementation;


/* Default constructor */
ARMAFactoryImplementation::ARMAFactoryImplementation()
  : PersistentObject()
  , p_(0)
  , currentP_(0)
  , q_(0)
  , currentQ_(0)
  , invertible_(true)
{
  // Nothing to do
}

/* Parameter constructor */
ARMAFactoryImplementation::ARMAFactoryImplementation(const UnsignedInteger p,
    const UnsignedInteger q,
    const Bool invertible)
  : PersistentObject()
  , p_(1, p)
  , currentP_(p)
  , q_(1, q)
  , currentQ_(q)
  , invertible_(invertible)
{
  // Nothing to do
}

/* Parameter constructor */
ARMAFactoryImplementation::ARMAFactoryImplementation(const Indices & p,
    const Indices & q,
    const Bool invertible)
  : PersistentObject()
  , p_(p)
  , currentP_(0)
  , q_(q)
  , currentQ_(0)
  , invertible_(invertible)
{
  // Nothing to do
}

/* Virtual constructor */
ARMAFactoryImplementation * ARMAFactoryImplementation::clone() const
{
  return new ARMAFactoryImplementation(*this);
}

/* P accessor */
Indices ARMAFactoryImplementation::getP() const
{
  return p_;
}
void ARMAFactoryImplementation::setP(const Indices & ARIndices)
{
  p_ = ARIndices;
}


UnsignedInteger ARMAFactoryImplementation::getCurrentP() const
{
  return currentP_;
}
void ARMAFactoryImplementation::setCurrentP(const UnsignedInteger p) const
{
  currentP_ = p;
}

/* Q accessor */
Indices ARMAFactoryImplementation::getQ() const
{
  return q_;
}
void ARMAFactoryImplementation::setQ(const Indices & MAIndices)
{
  q_ = MAIndices;
}

UnsignedInteger ARMAFactoryImplementation::getCurrentQ() const
{
  return currentQ_;
}
void ARMAFactoryImplementation::setCurrentQ(const UnsignedInteger q) const
{
  currentQ_ = q;
}

/* Invertibility accessor */
Bool ARMAFactoryImplementation::getInvertible() const
{
  return invertible_;
}

void ARMAFactoryImplementation::setInvertible(const Bool invertible)
{
  invertible_ = invertible;
}

/* String converter */
String ARMAFactoryImplementation::__repr__() const
{
  OSS oss;
  oss << "class= " << ARMAFactoryImplementation::GetClassName()
      << " p=" << p_
      << " current p=" << currentP_
      << " q=" << q_
      << " current q=" << currentQ_
      << " invertible=" << invertible_;
  return oss;
}

/* String converter */
String ARMAFactoryImplementation::__str__(const String & ) const
{
  OSS oss;
  oss << "class= " << ARMAFactoryImplementation::GetClassName()
      << " p=" << p_
      << " q=" << q_
      << " invertible=" << invertible_;
  return oss;
}

/* Build method */
ARMA ARMAFactoryImplementation::build(const TimeSeries & ) const
{
  throw NotYetImplementedException(HERE) << "In ARMAFactoryImplementation::build(const TimeSeries & timeSeries) const";
}

/* Build method */
ARMA ARMAFactoryImplementation::build(const ProcessSample & ) const
{
  throw NotYetImplementedException(HERE) << "In ARMAFactoryImplementation::build(const ProcessSample & sample) const";
}

/* Method save() stores the object through the StorageManager */
void ARMAFactoryImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "p_", p_ );
  adv.saveAttribute( "q_", q_ );
  adv.saveAttribute( "invertible_", invertible_ );
}

/* Method load() reloads the object from the StorageManager */
void ARMAFactoryImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "p_", p_ );
  adv.loadAttribute( "q_", q_ );
  adv.loadAttribute( "invertible_", invertible_ );
}

END_NAMESPACE_OPENTURNS
