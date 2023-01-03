//                                               -*- C++ -*-
/**
 *  @brief  This class provides a point location algorithm on RegularGrid
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
#ifndef OPENTURNS_REGULARGRIDENCLOSINGSIMPLEX_HXX
#define OPENTURNS_REGULARGRIDENCLOSINGSIMPLEX_HXX

#include "openturns/EnclosingSimplexAlgorithmImplementation.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Indices.hxx"
#include "openturns/IndicesCollection.hxx"
#include "openturns/RegularGrid.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class RegularGridEnclosingSimplex
 */

class OT_API RegularGridEnclosingSimplex
  : public EnclosingSimplexAlgorithmImplementation
{

  CLASSNAME

public:
  /** Default constructor without parameters */
  RegularGridEnclosingSimplex();

  /** Parameter constructor */
  RegularGridEnclosingSimplex(const RegularGrid & grid);

  /** Virtual copy constructor */
  RegularGridEnclosingSimplex * clone() const override;
#ifndef SWIG
  RegularGridEnclosingSimplex * emptyClone() const override;
#endif

  void setVerticesAndSimplices(const Sample & vertices, const IndicesCollection & simplices) override;

  /** Get the index of the enclosing simplex of the given point */
  UnsignedInteger query(const Point & x) const override;
  using EnclosingSimplexAlgorithmImplementation::query;

  /** Get the index of the enclosing simplex of the given scalar */
  UnsignedInteger queryScalar(const Scalar x) const;

  /** Get the index of the enclosing simplex of the given scalars */
  Indices queryScalar(const Point & values) const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

private:

  /** Cached values taken from grid_ */
  Scalar start_;
  UnsignedInteger N_;
  Scalar step_;

} ; /* class RegularGridEnclosingSimplex */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_REGULARGRIDENCLOSINGSIMPLEX_HXX */
