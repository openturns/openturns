//                                               -*- C++ -*-
/**
 *  @brief Factory for MultivariatePolya distribution
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_MULTIVARIATEPOLYAFACTORY_HXX
#define OPENTURNS_MULTIVARIATEPOLYAFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/MultivariatePolya.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MultivariatePolyaFactory
 *
 * The class that creates a MultivariatePolya distribution from a sample.
 */
class OT_API MultivariatePolyaFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  MultivariatePolyaFactory();

  /** Virtual constructor */
  MultivariatePolyaFactory * clone() const override;

  /** Here is the interface that all derived class must implement */
  /** Build the distribution from a sample */
  Distribution build(const Sample & sample) const override;

  /** Build the distribution from its default parameters */
  Distribution build() const override;

  /** Build the distribution from a sample */
  MultivariatePolya buildAsMultivariatePolya(const Sample & sample) const;

  /** Build the distribution from its default parameters */
  MultivariatePolya buildAsMultivariatePolya() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

}; /* class MultivariatePolyaFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MULTIVARIATEPOLYAFACTORY_HXX */