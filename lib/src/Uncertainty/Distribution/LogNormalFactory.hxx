//                                               -*- C++ -*-
/**
 *  @brief Factory for LogNormal distribution
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
#ifndef OPENTURNS_LOGNORMALFACTORY_HXX
#define OPENTURNS_LOGNORMALFACTORY_HXX

#include "OTprivate.hxx"
#include "DistributionFactoryImplementation.hxx"
#include "LogNormal.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LogNormalFactory
 */
class OT_API LogNormalFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME;
public:

  /** Default constructor */
  LogNormalFactory();

  /** Virtual constructor */
  virtual LogNormalFactory * clone() const;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Implementation build(const NumericalSample & sample) const;
  Implementation build(const NumericalSample & sample,
                       const UnsignedInteger method) const;
  Implementation build(const NumericalPoint & parameters) const;
  Implementation build() const;
  LogNormal buildAsLogNormal(const NumericalSample & sample) const;
  LogNormal buildAsLogNormal(const NumericalSample & sample,
                             const UnsignedInteger method) const;
  LogNormal buildAsLogNormal(const NumericalPoint & parameters) const;
  LogNormal buildAsLogNormal() const;

  /** Algorithm associated with the method of moments */
  LogNormal buildMethodOfMoments(const NumericalSample & sample) const;

  /** Algoritm associated with the method of local likelihood maximization */
  LogNormal buildMethodOfLocalLikelihoodMaximization(const NumericalSample & sample) const;

  /** Algorithm associated with the method of modified moments */
  LogNormal buildMethodOfModifiedMoments(const NumericalSample & sample) const;

private:

}; /* class LogNormalFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LOGNORMALFACTORY_HXX */
