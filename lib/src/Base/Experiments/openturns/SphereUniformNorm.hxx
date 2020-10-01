//                                               -*- C++ -*-
/**
 *  @brief Implementation of a generic grid iterator.
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
#ifndef OPENTURNS_SPHEREUNIFORMNORM_HXX
#define OPENTURNS_SPHEREUNIFORMNORM_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/SphereUniformNormImplementation.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS


class OT_API SphereUniformNorm
  : public TypedInterfaceObject<SphereUniformNormImplementation>
{
  CLASSNAME

public:

  typedef Pointer<SphereUniformNormImplementation> Implementation;

  /** Default constructor */
  SphereUniformNorm();

  /** Default constructor */
  SphereUniformNorm(const Point step, const Bool symmetric);

  /** Copy constructor */
  SphereUniformNorm(const SphereUniformNormImplementation & implementation);

  /** Constructor from implementation */
  SphereUniformNorm(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  SphereUniformNorm(SphereUniformNormImplementation * p_implementation);
#endif

  /** Symmettric accessor */
  Bool isSymmetric() const;

  /** Get all points at a certain distance */
  virtual Sample getPoints(const UnsignedInteger distance) const;

  /** Get index offset, needed by RandomMixture cache */
  virtual UnsignedInteger getOffsetLevel(const UnsignedInteger distance) const;

  /** Constructor from grid steps */
  static SphereUniformNorm GetFromGridSteps(const Point & step, const Bool symmetric);

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

};


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPHEREUNIFORMNORM_HXX */
