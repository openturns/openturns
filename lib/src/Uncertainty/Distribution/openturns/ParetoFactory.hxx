//                                               -*- C++ -*-
/**
 *  @brief Factory for Pareto distribution
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
#ifndef OPENTURNS_PARETOFACTORY_HXX
#define OPENTURNS_PARETOFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/Pareto.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ParetoFactory
 */
class OT_API ParetoFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  ParetoFactory();

  /** Virtual constructor */
  ParetoFactory * clone() const override;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Distribution build(const Sample & sample) const override;
  Distribution build(const Point & parameters) const override;
  Distribution build() const override;
  Pareto buildAsPareto(const Sample & sample) const;
  Pareto buildAsPareto(const Point & parameters) const;
  Pareto buildAsPareto() const;

  /** Specific algorithms */
  Pareto buildMethodOfMoments(const Sample & sample) const;
  Pareto buildMethodOfLikelihoodMaximization(const Sample & sample) const;
  Pareto buildMethodOfLeastSquares(const Sample & sample) const;

}; /* class ParetoFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PARETOFACTORY_HXX */
