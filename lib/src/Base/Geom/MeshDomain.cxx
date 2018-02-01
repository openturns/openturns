//                                               -*- C++ -*-
/**
 *  @brief MeshDomain converts a Mesh into a DomainImplementation
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/MeshDomain.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MeshDomain)

static const Factory<MeshDomain> Factory_MeshDomain;


/* Default constructor */
MeshDomain::MeshDomain()
  : DomainImplementation()
  , mesh_()
{
  // Nothing to do
}

/* Default constructor */
MeshDomain::MeshDomain(const Mesh & mesh)
  : DomainImplementation(mesh.getDimension())
  , mesh_(mesh)
{
  // Nothing to do
}

/* Clone method */
MeshDomain * MeshDomain::clone() const
{
  return new MeshDomain(*this);
}

/* String converter */
String MeshDomain::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " name=" << getName()
         << " mesh=" << mesh_;

}

String MeshDomain::__str__(const String & offset) const
{
  return OSS(false) << offset << __repr__();
}

/* Check if the given point is inside of the domain */
Bool MeshDomain::contains(const Point & point) const
{
  return mesh_.contains(point);
}

/* TBB functor to speed-up volume computation */
struct VolumeFunctor
{
  const Mesh & mesh_;
  Scalar accumulator_;

  VolumeFunctor(const Mesh & mesh)
    : mesh_(mesh), accumulator_(0.0) {}

  VolumeFunctor(const VolumeFunctor & other, TBB::Split)
    : mesh_(other.mesh_), accumulator_(0.0) {}

  void operator() (const TBB::BlockedRange<UnsignedInteger> & r)
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i) accumulator_ += mesh_.computeSimplexVolume(i);
  }

  void join(const VolumeFunctor & other)
  {
    accumulator_ += other.accumulator_;
  }

}; /* end struct VolumeFunctor */

/* Compute the volume of the mesh */
void MeshDomain::computeVolume() const
{
  VolumeFunctor functor( mesh_ );
  TBB::ParallelReduce( 0, mesh_.getSimplicesNumber(), functor );
  volume_ = functor.accumulator_;
  isAlreadyComputedVolume_ = true;
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
}

/* Method load() reloads the object from the StorageManager */
void MeshDomain::load(Advocate & adv)
{
  DomainImplementation::load(adv);
  adv.loadAttribute("mesh_", mesh_);
}

END_NAMESPACE_OPENTURNS
