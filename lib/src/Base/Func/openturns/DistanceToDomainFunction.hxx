//                                               -*- C++ -*-
/**
 *  @brief The class that implements DistanceToDomain functions.
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
#ifndef OPENTURNS_DISTANCETODOMAINFUNCTION_HXX
#define OPENTURNS_DISTANCETODOMAINFUNCTION_HXX

#include "openturns/Function.hxx"
#include "openturns/Domain.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class DistanceToDomainFunction
 *
 */
class OT_API DistanceToDomainFunction
  : public Function
{
  CLASSNAME
public:
  /* Default constructor */
  DistanceToDomainFunction();

  /** Parameter constructor */
  DistanceToDomainFunction(const Domain & domain);

  /** Comparison operator */
  Bool operator ==(const DistanceToDomainFunction & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

}; /* class QuadraticFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DISTANCETODOMAINFUNCTION_HXX */
