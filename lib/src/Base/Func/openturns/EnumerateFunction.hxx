//                                               -*- C++ -*-
/**
 *  @brief The bijective function to select polynomials in the orthogonal basis
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
#ifndef OPENTURNS_ENUMERATEFUNCTION_HXX
#define OPENTURNS_ENUMERATEFUNCTION_HXX

#include "openturns/EnumerateFunctionImplementation.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class EnumerateFunction
 *
 * The bijective function to select polynomials in the orthogonal basis
 */
class OT_API EnumerateFunction
  : public TypedInterfaceObject<EnumerateFunctionImplementation>
{
  CLASSNAME
public:

  typedef Pointer<EnumerateFunctionImplementation> Implementation;

  /** Default constructor */
  EnumerateFunction();

  /** Constructor with parameters */
  EnumerateFunction(const EnumerateFunctionImplementation & implementation);

  /** Constructor with parameters */
  EnumerateFunction(Implementation & p_implementation);

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** The bijective association between an integer and a set of indices */
  Indices operator() (const UnsignedInteger index) const;

  /** The inverse of the association */
  UnsignedInteger inverse(const Indices & indices) const;

  /** The cardinal of the given strata */
  UnsignedInteger getStrataCardinal(const UnsignedInteger strataIndex) const;

  /** The cardinal of the cumulated strata above or equal to the given strata */
  UnsignedInteger getStrataCumulatedCardinal(const UnsignedInteger strataIndex) const;

  /** The index of the strata of degree max <= maximumDegree */
  UnsignedInteger getMaximumDegreeStrataIndex(const UnsignedInteger maximumDegree) const;

  /** The cardinal of indices of degree max <= maximumDegree */
  UnsignedInteger getMaximumDegreeCardinal(const UnsignedInteger maximumDegree) const;

  /** Basis size from degree */
  UnsignedInteger getBasisSizeFromTotalDegree(const UnsignedInteger maximumDegree) const;

  /** Dimension accessor */
  void setDimension(const UnsignedInteger dimension);
  UnsignedInteger getDimension() const;

  /** Upper bound accessor */
  void setUpperBound(const Indices & upperBound);
  Indices getUpperBound() const;

protected:

private:


} ; /* class EnumerateFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ENUMERATEFUNCTION_HXX */
