//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all SamplerImplementation
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
#ifndef OPENTURNS_SAMPLERIMPLEMENTATION_HXX
#define OPENTURNS_SAMPLERIMPLEMENTATION_HXX

#include "OTprivate.hxx"
#include "PersistentObject.hxx"
#include "ResourceMap.hxx"
#include "NumericalSample.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class SamplerImplementation
 *
 * @brief The class implements the concept of comparison operator defined in Sampler.
 *
 * This class is abstract so it can not be instanciated. It must be derived.
 * @see Sampler
 */
class OT_API SamplerImplementation
  : public PersistentObject
{
  CLASSNAME;
public:

  /** Default constructor */
  SamplerImplementation();

  /** Verbosity accessor */
  void setVerbose(const Bool verbose);
  Bool getVerbose() const;

  /** String converter */
  virtual String __repr__() const;

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  virtual SamplerImplementation * clone() const;

  /** @copydoc Sampler::getDimension() const */
  virtual UnsignedInteger getDimension() const;

  /** @copydoc Sampler::getRealization() const */
  virtual NumericalPoint getRealization() const;

  /** @copydoc Sampler::getSample() const */
  virtual NumericalSample getSample(const UnsignedInteger size) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:
  Bool verbose_;


}; /* class SamplerImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SAMPLERIMPLEMENTATION_HXX */
