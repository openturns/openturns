//                                               -*- C++ -*-
/**
 *  @brief Factory for LogNormal distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_LOGNORMALFACTORY_HXX
#define OPENTURNS_LOGNORMALFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/LogNormal.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LogNormalFactory
 */
class OT_API LogNormalFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  LogNormalFactory();

  /** Virtual constructor */
  virtual LogNormalFactory * clone() const;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Distribution build(const Sample & sample) const;
  Distribution build(const Sample & sample,
                     const UnsignedInteger method) const;
  Distribution build(const Point & parameters) const;
  Distribution build() const;
  LogNormal buildAsLogNormal(const Sample & sample) const;
  LogNormal buildAsLogNormal(const Sample & sample,
                             const UnsignedInteger method) const;
  LogNormal buildAsLogNormal(const Point & parameters) const;
  LogNormal buildAsLogNormal() const;

  /** Algorithm associated with the method of moments */
  LogNormal buildMethodOfMoments(const Sample & sample) const;

  /** Algoritm associated with the method of local likelihood maximization */
  LogNormal buildMethodOfLocalLikelihoodMaximization(const Sample & sample) const;

  /** Algorithm associated with the method of modified moments */
  LogNormal buildMethodOfModifiedMoments(const Sample & sample) const;

  /** Algorithm associated with the method of least-squares */
  LogNormal buildMethodOfLeastSquares(const Sample & sample) const;

private:
  LogNormal buildMethodOfLeastSquares(const Sample & sample, const Scalar gamma) const;

}; /* class LogNormalFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LOGNORMALFACTORY_HXX */
