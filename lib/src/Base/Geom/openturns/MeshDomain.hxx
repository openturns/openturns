//                                               -*- C++ -*-
/**
 *  @brief MeshDomain converts a Mesh into a DomainImplementation
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_MESHDOMAIN_HXX
#define OPENTURNS_MESHDOMAIN_HXX

#include "openturns/DomainImplementation.hxx"
#include "openturns/Mesh.hxx"
#include "openturns/EnclosingSimplexAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MeshDomain
 *
 * A class that converts a Mesh to a Domain
 */
class OT_API MeshDomain
  : public DomainImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  MeshDomain();

  /** Default constructor */
  explicit MeshDomain(const Mesh & mesh);

  /** Parameters constructor */
  MeshDomain(const Mesh & mesh, const EnclosingSimplexAlgorithm & enclosingSimplex);

  /** Mesh accessor */
  virtual Mesh getMesh() const;

  /** EnclosingSimplexAlgorithm accessor */
  virtual EnclosingSimplexAlgorithm getEnclosingSimplexAlgorithm() const;
  virtual void setEnclosingSimplexAlgorithm(const EnclosingSimplexAlgorithm enclosingSimplex);

  /** Virtual constructor method */
  MeshDomain * clone() const override;

  /** Check if the mesh contains a given point */
  Bool contains(const Point & point) const override;
  using DomainImplementation::contains;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Lower bound of the bounding box */
  virtual Point getLowerBound() const;

  /** Upper bound of the bounding box */
  virtual Point getUpperBound() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** The underlying mesh */
  Mesh mesh_;

  /** The algorithm for point location */
  EnclosingSimplexAlgorithm enclosingSimplex_;

}; /* class MeshDomain */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MESHDOMAIN_HXX */
