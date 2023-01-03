//                                               -*- C++ -*-
/**
 *  @brief  This class provides a naive implementation of search
 *  of neighbours.  It is only meant for debugging purpose.
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
#ifndef OPENTURNS_ENCLOSINGSIMPLEXMONOTONIC1D_HXX
#define OPENTURNS_ENCLOSINGSIMPLEXMONOTONIC1D_HXX

#include "openturns/EnclosingSimplexAlgorithmImplementation.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Indices.hxx"
#include "openturns/IndicesCollection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class EnclosingSimplexMonotonic1D
 */

class OT_API EnclosingSimplexMonotonic1D
  : public EnclosingSimplexAlgorithmImplementation
{

  CLASSNAME

public:
  /** Default constructor without parameters */
  EnclosingSimplexMonotonic1D();

  /** Parameter constructor */
  explicit EnclosingSimplexMonotonic1D(const Sample & vertices);

  /** Virtual copy constructor */
  EnclosingSimplexMonotonic1D * clone() const override;
#ifndef SWIG
  EnclosingSimplexMonotonic1D * emptyClone() const override;
#endif

  void setVerticesAndSimplices(const Sample & vertices, const IndicesCollection & simplices) override;

  /** Get the index of the enclosing simlex of the given point */
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

  /** Flag telling whether vertices are sorted in ascending or descending order */
  Bool increasing_;

} ; /* class EnclosingSimplexMonotonic1D */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ENCLOSINGSIMPLEXMONOTONIC1D_HXX */
