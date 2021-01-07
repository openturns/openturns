//                                               -*- C++ -*-
/**
 *  @brief The class SphereUniformNormImplementation implements walker on a regular grid
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SPHEREUNIFORMNORMIMPLEMENTATION_HXX
#define OPENTURNS_SPHEREUNIFORMNORMIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/SphereUniformNormImplementation.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS


class OT_API SphereUniformNormImplementation
  : public PersistentObject
{
  CLASSNAME

public:

  typedef Pointer<SphereUniformNormImplementation> Implementation;

  /** Default constructor */
  SphereUniformNormImplementation(const Point & step, const Bool symmetric);

  /** Virtual constructor */
  SphereUniformNormImplementation * clone() const override;

  /** Symmettric accessor */
  Bool isSymmetric() const
  {
    return symmetric_;
  }

  /** Get all points at a certain distance */
  virtual Sample getPoints(const UnsignedInteger distance) const;

  /** Get index offset, needed by RandomMixture cache */
  virtual UnsignedInteger getOffsetLevel(const UnsignedInteger distance) const;

#ifndef SWIG
  /** Constructor from grid steps */
  static Implementation GetFromGridSteps(const Point & step, const Bool symmetric);
#endif

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  /** Disable default constructor */
  SphereUniformNormImplementation();

private:

  /** Grid steps */
  Point step_;

  /** Symmetric grid? */
  Bool symmetric_;

};


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPHEREUNIFORMNORMIMPLEMENTATION_HXX */
