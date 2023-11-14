//                                               -*- C++ -*-
/**
 *  @brief Boundary extraction algorithm for meshes
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
#ifndef OPENTURNS_BOUNDARYMESHER_HXX
#define OPENTURNS_BOUNDARYMESHER_HXX

#include "openturns/LevelSet.hxx"
#include "openturns/Interval.hxx"
#include "openturns/Mesh.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/AbdoRackwitz.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BoundaryMesher
 */
class OT_API BoundaryMesher
  : public PersistentObject
{
  CLASSNAME
public:

  /** Default constructor */
  BoundaryMesher();

  /** Virtual constructor */
  BoundaryMesher * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /* Here is the interface that all derived class must implement */
  /** Build the boundary of the given mesh */
  virtual Mesh build(const Mesh & mesh,
                     const Scalar offset = 0.0) const;

protected:

private:

  /* Discretization in each dimension */
  Indices discretization_;

  /* Optimization solver used to project the vertices */
  OptimizationAlgorithm solver_;

}; /* class BoundaryMesher */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BOUNDARYMESHER_HXX */

