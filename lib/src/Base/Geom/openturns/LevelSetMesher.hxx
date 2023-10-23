//                                               -*- C++ -*-
/**
 *  @brief Meshing algorithm for levelSets
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_LEVELSETMESHER_HXX
#define OPENTURNS_LEVELSETMESHER_HXX

#include "openturns/LevelSet.hxx"
#include "openturns/Interval.hxx"
#include "openturns/Mesh.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/AbdoRackwitz.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LevelSetMesher
 */
class OT_API LevelSetMesher
  : public PersistentObject
{
  CLASSNAME
public:

  /** Default constructor */
  LevelSetMesher();

  /** Default constructor */
  explicit LevelSetMesher(const Indices & discretization,
                          const OptimizationAlgorithm & solver = AbdoRackwitz());

  /** Virtual constructor */
  LevelSetMesher * clone() const override;

  /** Discretization accessors */
  void setDiscretization(const Indices & discretization);
  Indices getDiscretization() const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Optimization solver accessor */
  void setOptimizationAlgorithm(const OptimizationAlgorithm & solver);
  OptimizationAlgorithm getOptimizationAlgorithm() const;

  /* Here is the interface that all derived class must implement */
  /** Build a mesh based on a domain */
  virtual Mesh build(const LevelSet & levelSet,
                     const Bool project = true) const;
  virtual Mesh build(const LevelSet & levelSet,
                     const Interval & boundingBox,
                     const Bool project = true) const;

protected:

private:

  /* Discretization in each dimension */
  Indices discretization_;

  /* Optimization solver used to project the vertices */
  OptimizationAlgorithm solver_;

}; /* class LevelSetMesher */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LEVELSETMESHER_HXX */

