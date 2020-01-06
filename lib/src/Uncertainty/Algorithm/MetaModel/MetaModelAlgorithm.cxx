//                                               -*- C++ -*-
/**
 *  @brief Implementation for metamodel algorithms
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/MetaModelAlgorithm.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MetaModelAlgorithm)

static const Factory<MetaModelAlgorithm> Factory_MetaModelAlgorithm;

/* Default constructor */
MetaModelAlgorithm::MetaModelAlgorithm()
  : PersistentObject()
{
  // Nothing to do
}

/* Constructor with parameters */
MetaModelAlgorithm::MetaModelAlgorithm(const Distribution & distribution,
                                       const Function & model)
  : PersistentObject()
  , distribution_(distribution)
  , model_(model)
{
  // Nothing to do
}

/* Virtual constructor */
MetaModelAlgorithm * MetaModelAlgorithm::clone() const
{
  return new MetaModelAlgorithm(*this);
}

/* String converter */
String MetaModelAlgorithm::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName();
  return oss;
}



/* Distribution accessor */
void MetaModelAlgorithm::setDistribution(const Distribution & distribution)
{
  distribution_ = distribution;
}

Distribution MetaModelAlgorithm::getDistribution() const
{
  return distribution_;
}

/* Response surface computation */
void MetaModelAlgorithm::run()
{
  throw NotYetImplementedException(HERE) << "In MetaModelAlgorithm::run()";
}


Sample MetaModelAlgorithm::getInputSample() const
{
  throw NotYetImplementedException(HERE) << "In MetaModelAlgorithm::getInputSample() const";
}


Sample MetaModelAlgorithm::getOutputSample() const
{
  throw NotYetImplementedException(HERE) << "In MetaModelAlgorithm::getOutputSample() const";
}


/* Method save() stores the object through the StorageManager */
void MetaModelAlgorithm::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "distribution_", distribution_ );
  adv.saveAttribute( "model_", model_ );

}

/* Method load() reloads the object from the StorageManager */
void MetaModelAlgorithm::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "distribution_", distribution_ );
  adv.loadAttribute( "model_", model_ );
}

END_NAMESPACE_OPENTURNS
