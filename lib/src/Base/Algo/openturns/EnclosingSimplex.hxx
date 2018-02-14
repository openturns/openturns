//                                               -*- C++ -*-
/**
 *  @brief  This class provides fast point location.
 *  This is the interface class.
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
#ifndef OPENTURNS_ENCLOSINGSIMPLEX_HXX
#define OPENTURNS_ENCLOSINGSIMPLEX_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/EnclosingSimplexImplementation.hxx"
#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Indices.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class EnclosingSimplex
 */

class OT_API EnclosingSimplex
  : public TypedInterfaceObject<EnclosingSimplexImplementation>
{

  CLASSNAME

public:

  typedef Pointer<EnclosingSimplexImplementation>    Implementation;
  typedef EnclosingSimplexImplementation::IndicesCollection IndicesCollection;

  /** Default onstructor */
  EnclosingSimplex();

  /** Parameter constructor */
  EnclosingSimplex(const Sample & vertices, const IndicesCollection & simplices);

  /** Copy constructors */
  EnclosingSimplex(const EnclosingSimplexImplementation & implementation);

  /** Constructor from implementation */
  EnclosingSimplex(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  EnclosingSimplex(EnclosingSimplexImplementation * p_implementation);
#endif

  /** Get the index of the enclosing simplex */
  virtual UnsignedInteger getEnclosingSimplexIndex(const Point & x) const;

  /** Get the indices of the enclosing simplex of the given points */
  virtual Indices getEnclosingSimplexIndex(const Sample & sample) const;

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;
} ; /* class EnclosingSimplex */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ENCLOSINGSIMPLEX_HXX */
