//                                               -*- C++ -*-
/**
 *  @brief Factory for InverseGamma distribution
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
#ifndef OPENTURNS_INVERSEGAMMAFACTORY_HXX
#define OPENTURNS_INVERSEGAMMAFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/InverseGamma.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class InverseGammaFactory
 */
class OT_API InverseGammaFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  InverseGammaFactory();

  /** Virtual constructor */
  InverseGammaFactory * clone() const override;

  using DistributionFactoryImplementation::build;

  Distribution build(const Sample & sample) const override;
  Distribution build(const Point & parameters) const override;
  Distribution build() const override;

  InverseGamma buildAsInverseGamma(const Sample & sample) const;
  InverseGamma buildAsInverseGamma(const Point & parameters) const;
  InverseGamma buildAsInverseGamma() const;

private:

  /** Method of moments estimator */
  InverseGamma buildMethodOfMoments(const Sample & sample) const;

}; /* class InverseGammaFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSEGAMMAFACTORY_HXX */
