//                                               -*- C++ -*-
/**
 *  @brief MeshDomain converts a Mesh into a DomainImplementation
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/MeshDomain.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/EnclosingSimplexAlgorithm.hxx"
#include "openturns/TBB.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MeshDomain)

static const Factory<MeshDomain> Factory_MeshDomain;


/* Default constructor */
MeshDomain::MeshDomain()
  : DomainImplementation()
  , mesh_()
  , enclosingSimplex_()
{
  // Nothing to do
}

/* Default constructor */
MeshDomain::MeshDomain(const Mesh & mesh)
  : DomainImplementation(mesh.getDimension())
  , mesh_(mesh)
  , enclosingSimplex_(mesh.getVertices(), mesh.getSimplices())
{
  // Nothing to do
}

/* Default constructor */
MeshDomain::MeshDomain(const Mesh & mesh, const EnclosingSimplexAlgorithm & enclosingSimplex)
  : DomainImplementation(mesh.getDimension())
  , mesh_(mesh)
  , enclosingSimplex_(enclosingSimplex)
{
  // Nothing to do
}

/* Clone method */
MeshDomain * MeshDomain::clone() const
{
  return new MeshDomain(*this);
}

/** Mesh accessor */
Mesh MeshDomain::getMesh() const
{
  return mesh_;
}

/* EnclosingSimplexAlgorithm accessor */
EnclosingSimplexAlgorithm MeshDomain::getEnclosingSimplexAlgorithm() const
{
  return enclosingSimplex_;
}

void MeshDomain::setEnclosingSimplexAlgorithm(const EnclosingSimplexAlgorithm enclosingSimplex)
{
  EnclosingSimplexAlgorithm emptyClone(enclosingSimplex.getImplementation()->emptyClone());
  enclosingSimplex_.swap(emptyClone);
  enclosingSimplex_.setVerticesAndSimplices(mesh_.getVertices(), mesh_.getSimplices());
}

/* String converter */
String MeshDomain::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " name=" << getName()
         << " mesh=" << mesh_
         << " enclosingSimplex=" << enclosingSimplex_;

}

String MeshDomain::__str__(const String & ) const
{
  return OSS(false) << __repr__();
}

/* Check if the given point is inside of the domain */
Bool MeshDomain::contains(const Point & point) const
{
  return enclosingSimplex_.query(point) < mesh_.getSimplicesNumber();
}

/* Lower bound of the bounding box */
Point MeshDomain::getLowerBound() const
{
  return mesh_.getLowerBound();
}

/* Upper bound of the bounding box */
Point MeshDomain::getUpperBound() const
{
  return mesh_.getUpperBound();
}

/* Method save() stores the object through the StorageManager */
void MeshDomain::save(Advocate & adv) const
{
  DomainImplementation::save(adv);
  adv.saveAttribute("mesh_", mesh_);
  adv.saveAttribute("enclosingSimplex_", enclosingSimplex_);
}

/* Method load() reloads the object from the StorageManager */
void MeshDomain::load(Advocate & adv)
{
  DomainImplementation::load(adv);
  adv.loadAttribute("mesh_", mesh_);
  adv.loadAttribute("enclosingSimplex_", enclosingSimplex_);
}

END_NAMESPACE_OPENTURNS
