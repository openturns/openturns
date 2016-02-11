//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all distributions parameters
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_DISTRIBUTIONPARAMETERS_HXX
#define OPENTURNS_DISTRIBUTIONPARAMETERS_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/DistributionParametersImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DistributionParameters
 *
 */
class OT_API DistributionParameters
  : public TypedInterfaceObject<DistributionParametersImplementation>
{
  CLASSNAME;
public:
  typedef Collection<NumericalPointWithDescription> NumericalPointWithDescriptionCollection;

  /** Default constructor */
  DistributionParameters();

  /** Default constructor */
  DistributionParameters(const DistributionParametersImplementation & implementation);

  /** Constructor from implementation */
  DistributionParameters(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  DistributionParameters(DistributionParametersImplementation * p_implementation);
#endif

  /** Build a distribution based on a set of native parameters */
  Distribution getDistribution() const;

  /** Compute jacobian / native parameters */
  Matrix gradient() const;

  /** Conversion operator */
  NumericalPoint operator () (const NumericalPoint & inP) const;

  /** Conversion from native parameters */
  NumericalPoint inverse(const NumericalPoint & inP) const;

  /** Parameters value and description accessor */
  void setValues(const NumericalPoint & values);
  NumericalPoint getValues() const;
  Description getDescription() const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;


}; /* class DistributionParameters */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DISTRIBUTIONPARAMETERS_HXX */