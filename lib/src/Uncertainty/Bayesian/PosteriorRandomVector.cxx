//                                               -*- C++ -*-
/**
 *  @brief An implementation class for random vectors based on samplers
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/PosteriorRandomVector.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(PosteriorRandomVector);

static const Factory<PosteriorRandomVector> RegisteredFactory;

/* Default constructor */
PosteriorRandomVector::PosteriorRandomVector(const Sampler & sampler)
  : RandomVectorImplementation()
  , sampler_(sampler)
{
  // Nothing to do
}


/* Virtual constructor */
PosteriorRandomVector * PosteriorRandomVector::clone() const
{
  return new PosteriorRandomVector(*this);
}

/* String converter */
String PosteriorRandomVector::__repr__() const
{
  OSS oss;
  oss << "class=" << PosteriorRandomVector::GetClassName()
      << " sampler=" << sampler_;
  return oss;
}


/* Sampler accessor */
Sampler PosteriorRandomVector::getSampler() const
{
  return sampler_;
}


UnsignedInteger PosteriorRandomVector::getDimension() const
{
  return sampler_.getDimension();
}


NumericalPoint PosteriorRandomVector::getRealization() const
{
  return sampler_.getRealization();
}


NumericalSample PosteriorRandomVector::getSample(const UnsignedInteger size) const
{
  return sampler_.getSample(size);
}

/* Method save() stores the object through the StorageManager */
void PosteriorRandomVector::save(Advocate & adv) const
{
  RandomVectorImplementation::save(adv);
  adv.saveAttribute( "sampler_", sampler_ );
}


/* Method load() reloads the object from the StorageManager */
void PosteriorRandomVector::load(Advocate & adv)
{
  RandomVectorImplementation::load(adv);
  adv.loadAttribute( "sampler_", sampler_ );
}


END_NAMESPACE_OPENTURNS
