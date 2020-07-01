//                                               -*- C++ -*-
/**
 *  @brief Factory for WeibullMin distribution
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
#ifndef OPENTURNS_WEIBULLMINFACTORY_HXX
#define OPENTURNS_WEIBULLMINFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/WeibullMin.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class WeibullMinFactory
 */
class OT_API WeibullMinFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  WeibullMinFactory();

  /** Virtual constructor */
  virtual WeibullMinFactory * clone() const;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Distribution build(const Sample & sample) const;
  Distribution build(const Point & parameters) const;
  Distribution build() const;
  WeibullMin buildAsWeibullMin(const Sample & sample) const;
  WeibullMin buildAsWeibullMin(const Point & parameters) const;
  WeibullMin buildAsWeibullMin() const;

  /** Algorithm associated with the method of moments */
  WeibullMin buildMethodOfMoments(const Sample & sample) const;
  WeibullMin buildMethodOfLikelihoodMaximization(const Sample & sample) const;

}; /* class WeibullMinFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_WEIBULLMINFACTORY_HXX */
