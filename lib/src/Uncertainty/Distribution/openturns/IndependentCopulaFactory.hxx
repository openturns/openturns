//                                               -*- C++ -*-
/**
 *  @brief Factory for IndependentCopula distribution
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_INDEPENDENTCOPULAFACTORY_HXX
#define OPENTURNS_INDEPENDENTCOPULAFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/IndependentCopula.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class IndependentCopulaFactory
 */
class OT_API IndependentCopulaFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  IndependentCopulaFactory();

  /** Virtual constructor */
  IndependentCopulaFactory * clone() const override;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Distribution build(const Sample & sample) const override;
  Distribution build(const Point & parameters) const override;
  Distribution build() const override;
  IndependentCopula buildAsIndependentCopula(const Sample & sample) const;
  IndependentCopula buildAsIndependentCopula(const Point & parameters) const;
  IndependentCopula buildAsIndependentCopula() const;

}; /* class IndependentCopulaFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INDEPENDENTCOPULAFACTORY_HXX */
