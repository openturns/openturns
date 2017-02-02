//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all SamplerImplementation
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/SamplerImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(SamplerImplementation);

static const Factory<SamplerImplementation> Factory_SamplerImplementation;

/* Default constructor */
SamplerImplementation::SamplerImplementation()
  : PersistentObject()
  , verbose_(false)
{

}

/* String converter */
String SamplerImplementation::__repr__() const
{
  return OSS() << "class=" << SamplerImplementation::GetClassName()
         << " name=" << getName();
}


SamplerImplementation* SamplerImplementation::clone() const
{
  return new SamplerImplementation(*this);
}


void SamplerImplementation::setVerbose(const Bool verbose)
{
  verbose_ = verbose;
}


Bool SamplerImplementation::getVerbose() const
{
  return verbose_;
}

/* Here is the interface that all derived class must implement */


UnsignedInteger SamplerImplementation::getDimension() const
{
  throw NotYetImplementedException(HERE) << "In SamplerImplementation::getDimension() const";
}


NumericalPoint SamplerImplementation::getRealization() const
{
  throw NotYetImplementedException(HERE) << "In SamplerImplementation::getRealization() const";
}


NumericalSample SamplerImplementation::getSample(const UnsignedInteger size) const
{
  NumericalSample sample(size, getDimension());
  for (UnsignedInteger i = 0; i < size; ++ i)
    sample[i] = getRealization();
  return sample;
}


/* Method save() stores the object through the StorageManager */
void SamplerImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("verbose_", verbose_);
}


/* Method load() reloads the object from the StorageManager */
void SamplerImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("verbose_", verbose_);
}


END_NAMESPACE_OPENTURNS
