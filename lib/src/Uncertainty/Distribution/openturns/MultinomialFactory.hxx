//                                               -*- C++ -*-
/**
 *  @brief Factory for Multinomial distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_MULTINOMIALFACTORY_HXX
#define OPENTURNS_MULTINOMIALFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/Multinomial.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MultinomialFactory
 */
class OT_API MultinomialFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  MultinomialFactory();

  /** Virtual constructor */
  virtual MultinomialFactory * clone() const;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Distribution build(const Sample & sample) const;
  Distribution build() const;
  Multinomial buildAsMultinomial(const Sample & sample) const;
  Multinomial buildAsMultinomial() const;

}; /* class MultinomialFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MULTINOMIALFACTORY_HXX */
