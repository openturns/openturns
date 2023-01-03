//                                               -*- C++ -*-
/**
 *  @brief P1 Lagrange interpolation between two meshes.
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
#ifndef OPENTURNS_P1LAGRANGEINTERPOLATION_HXX
#define OPENTURNS_P1LAGRANGEINTERPOLATION_HXX

#include "openturns/FieldFunctionImplementation.hxx"
#include "openturns/Mesh.hxx"
#include "openturns/Field.hxx"
#include "openturns/IndicesCollection.hxx"
#include "openturns/NearestNeighbourAlgorithm.hxx"
#include "openturns/EnclosingSimplexAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class P1LagrangeInterpolation
 * Provided a field, compute the P1 piecewise-linear Lagrange interpolation.
 */
class OT_API P1LagrangeInterpolation
  : public FieldFunctionImplementation
{
  CLASSNAME

public:

  /** Default constructor */
  P1LagrangeInterpolation();

  /** Parameters constructor */
  P1LagrangeInterpolation(const Mesh & inputMesh,
                          const Mesh & outputMesh,
                          const UnsignedInteger dimension);

  /** Parameters constructor */
  P1LagrangeInterpolation(const Mesh & inputMesh,
                          const Mesh & outputMesh,
                          const UnsignedInteger dimension,
                          const EnclosingSimplexAlgorithm & enclosingSimplex,
                          const NearestNeighbourAlgorithm & nearestNeighbour);

  /** Virtual constructor */
  P1LagrangeInterpolation * clone() const override;

  /** Field dimension accessor */
  void setDimension(const UnsignedInteger dimension);

  /** Enclosing simplex algorithm accessor */
  EnclosingSimplexAlgorithm getEnclosingSimplexAlgorithm() const;

  /** Nearest neighbour algorithm accessor */
  NearestNeighbourAlgorithm getNearestNeighbourAlgorithm() const;

  /** Comparison operator */
  Bool operator ==(const P1LagrangeInterpolation & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__( const String & offset = "") const override;

  /** Operator () */
  Sample operator()(const Sample & field) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


private:
  void computeProjection();

  /* EnclosingSimplexAlgorithm to speed-up point location */
  EnclosingSimplexAlgorithm enclosingSimplex_;

  /* NearestNeighbourAlgorithm to speed-up nearest-neighbour search */
  NearestNeighbourAlgorithm nearestNeighbour_;

  /* Each point from sample_ is projected onto (1+mesh_.getDimension()) points
   * Projection matrix is sparse, we store barycentric coordinates and the list
   * of neighbours.
   */
  Sample barycentricCoordinates_;
  IndicesCollection neighbours_;

}; /* class P1LagrangeInterpolation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_P1LAGRANGEINTERPOLATION_HXX */
