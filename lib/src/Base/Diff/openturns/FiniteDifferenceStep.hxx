//                                               -*- C++ -*-
/**
 *  @brief Interface for finite difference steps
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

#ifndef OPENTURNS_FINITEDIFFERENCESTEP_HXX
#define OPENTURNS_FINITEDIFFERENCESTEP_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/FiniteDifferenceStepImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API FiniteDifferenceStep
  : public TypedInterfaceObject<FiniteDifferenceStepImplementation>
{
  CLASSNAME
public:

  /** Default constructor */
  FiniteDifferenceStep();

  /** Parameters constructor */
  explicit FiniteDifferenceStep(const Point & eps);

  /** Constructor from implementation */
  FiniteDifferenceStep(const FiniteDifferenceStepImplementation & implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  FiniteDifferenceStep(const Implementation & p_implementation);
#endif
  /** String converter */
  String __repr__() const override;

  /** Epsilon accessor */
  void setEpsilon(const Point & epsilon);
  Point getEpsilon() const;

  /** Compute step */
  Point operator()(const Point & inP) const;

};

END_NAMESPACE_OPENTURNS

#endif

