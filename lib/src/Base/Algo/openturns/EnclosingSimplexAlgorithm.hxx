//                                               -*- C++ -*-
/**
 *  @brief  This class provides fast point location.
 *  This is the interface class.
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_ENCLOSINGSIMPLEX_HXX
#define OPENTURNS_ENCLOSINGSIMPLEX_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/EnclosingSimplexAlgorithmImplementation.hxx"
#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Indices.hxx"
#include "openturns/IndicesCollection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class EnclosingSimplexAlgorithm
 */

class OT_API EnclosingSimplexAlgorithm
  : public TypedInterfaceObject<EnclosingSimplexAlgorithmImplementation>
{

  CLASSNAME

public:

  /** Default onstructor */
  EnclosingSimplexAlgorithm();

  /** Parameter constructor */
  EnclosingSimplexAlgorithm(const Sample & vertices, const IndicesCollection & simplices);

  /** Copy constructors */
  EnclosingSimplexAlgorithm(const EnclosingSimplexAlgorithmImplementation & implementation);

  /** Constructor from implementation */
  EnclosingSimplexAlgorithm(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  EnclosingSimplexAlgorithm(EnclosingSimplexAlgorithmImplementation * p_implementation);
#endif

  /** Points accessor */
  Sample getVertices() const;

  /** Simplices accessor (flattened view) */
  IndicesCollection getSimplices() const;

  void setVerticesAndSimplices(const Sample & vertices, const IndicesCollection & simplices);

  /** Get the index of the enclosing simplex */
  virtual UnsignedInteger query(const Point & x) const;

  /** Get the indices of the enclosing simplex of the given points */
  virtual Indices query(const Sample & sample) const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;
} ; /* class EnclosingSimplexAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ENCLOSINGSIMPLEX_HXX */
