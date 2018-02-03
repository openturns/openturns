//                                               -*- C++ -*-
/**
 *  @brief Interval is defined as the cartesian product of n 1D intervalls ]low_1, up_1]x...x]low_n,up_n]
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
#ifndef OPENTURNS_MESHDOMAIN_HXX
#define OPENTURNS_MESHDOMAIN_HXX

#include "openturns/DomainImplementation.hxx"
#include "openturns/Mesh.hxx"

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

  /** Virtual constructor method */
  virtual MeshDomain * clone() const;

  /** Check if the mesh contains a given point */
  virtual Bool contains(const Point & point) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Lower bound of the bounding box */
  virtual Point getLowerBound() const;

  /** Upper bound of the bounding box */
  virtual Point getUpperBound() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  /** Get the numerical volume of the domain */
  void computeVolume() const;

  /** The underlying mesh */
  Mesh mesh_;

}; /* class MeshDomain */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MESHDOMAIN_HXX */
