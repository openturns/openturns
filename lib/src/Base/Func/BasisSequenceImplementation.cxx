//                                               -*- C++ -*-
/**
 *  @brief A sequence of numerical math function collection
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

#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/BasisSequenceImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



TEMPLATE_CLASSNAMEINIT( PersistentCollection<Indices> );
static const Factory<PersistentCollection<Indices> > Factory_PersistentCollection_Indices;



CLASSNAMEINIT(BasisSequenceImplementation);

static const Factory<BasisSequenceImplementation> Factory_BasisSequenceImplementation;

/* Default constructor */
BasisSequenceImplementation::BasisSequenceImplementation()
  : PersistentCollection<Indices>()
{
  // Nothing to do
}

/* Default constructor */
BasisSequenceImplementation::BasisSequenceImplementation(const Basis & masterBasis)
  : PersistentCollection<Indices>(),
    masterBasis_(masterBasis)
{
  // Nothing to do
}

/* Virtual constructor */
BasisSequenceImplementation * BasisSequenceImplementation::clone() const
{
  return new BasisSequenceImplementation( *this );
}


/* String converter */
String BasisSequenceImplementation::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " collection=" << PersistentCollection<Indices>::__repr__()
         << " masterBasis=" << masterBasis_;
}

String BasisSequenceImplementation::__str__(const String & offset) const
{
  return OSS(false) << offset << __repr__();
}


/* Dimension accessor */
UnsignedInteger BasisSequenceImplementation::getDimension() const
{
  return masterBasis_.getDimension();
}

/* Returns the master basis */
Basis BasisSequenceImplementation::getMasterBasis() const
{
  return masterBasis_;
}

Basis BasisSequenceImplementation::getBasis(const UnsignedInteger index) const
{
  Basis result;
  Indices subBasisIndices(operator[](index));
  for (UnsignedInteger i = 0; i < subBasisIndices.getSize(); ++ i) result.add(masterBasis_[subBasisIndices[i]]);
  return result;
}

Indices BasisSequenceImplementation::getIndices(const UnsignedInteger index) const
{
  return operator[](index);
}

/* Method save() stores the object through the StorageManager */
void BasisSequenceImplementation::save(Advocate & adv) const
{
  PersistentCollection<Indices>::save( adv );
  adv.saveAttribute( "masterBasis_", masterBasis_ );
}


/* Method load() reloads the object from the StorageManager */
void BasisSequenceImplementation::load(Advocate & adv)
{
  PersistentCollection<Indices>::load( adv );
  adv.loadAttribute( "masterBasis_", masterBasis_ );
}


END_NAMESPACE_OPENTURNS
