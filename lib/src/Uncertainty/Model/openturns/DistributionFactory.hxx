//                                               -*- C++ -*-
/**
 * @brief Top-level class for all distribution factories
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

#ifndef OPENTURNS_DISTRIBUTIONFACTORY_HXX
#define OPENTURNS_DISTRIBUTIONFACTORY_HXX

#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/DistributionFactoryResult.hxx"
#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class DistributionFactory
 */
class OT_API DistributionFactory
  : public TypedInterfaceObject<DistributionFactoryImplementation>
{
  CLASSNAME
public:

  typedef Collection<Point>                PointCollection;
  typedef Collection<DistributionFactory>           DistributionFactoryCollection;

  /** Default constructor */
  explicit DistributionFactory(const UnsignedInteger bootstrapSize = ResourceMap::GetAsUnsignedInteger("DistributionFactory-DefaultBootstrapSize"));

  /** Parameter constructor */
  DistributionFactory(const DistributionFactoryImplementation & factory);

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /* Here is the interface that all derived class must implement */

  virtual Distribution build(const Sample & sample) const;
  virtual Distribution build(const Point & parameters) const;
  virtual Distribution build() const;

  /** Build the distribution and the parameter distribution */
  virtual DistributionFactoryResult buildEstimator(const Sample & sample) const;

  /** Build the distribution and the parameter distribution in a new parametrization */
  virtual DistributionFactoryResult buildEstimator(const Sample & sample,
      const DistributionParameters & parameters) const;

  /** Catalog of factories */
  static DistributionFactoryCollection GetContinuousUniVariateFactories();
  static DistributionFactoryCollection GetContinuousMultiVariateFactories();
  static DistributionFactoryCollection GetDiscreteUniVariateFactories();
  static DistributionFactoryCollection GetDiscreteMultiVariateFactories();
  static DistributionFactoryCollection GetUniVariateFactories();
  static DistributionFactoryCollection GetMultiVariateFactories();

private:

}; /* class DistributionFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DISTRIBUTIONFACTORY_HXX */
