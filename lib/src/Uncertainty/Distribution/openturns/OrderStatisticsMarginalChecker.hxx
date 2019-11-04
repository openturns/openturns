//                                               -*- C++ -*-
/**
 *  @brief OrderStatisticsMarginalChecker class
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
#ifndef OPENTURNS_ORDERSTATISTICSMARGINALCHECKER_HXX
#define OPENTURNS_ORDERSTATISTICSMARGINALCHECKER_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/OptimizationAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class OrderStatisticsMarginalChecker
 * @brief Checks for ordered marginals in the context of the maximum order statistics copula
 */
class OT_API OrderStatisticsMarginalChecker
  : public Object
{
  CLASSNAME
public:

  typedef Collection<Distribution> DistributionCollection;

  /** Parameters constructor */
  OrderStatisticsMarginalChecker(const DistributionCollection & collection);

  /** Is the collection of marginals compatible ? */
  Bool isCompatible() const;

  /** Throws an exception if incompatible margins */
  void check() const;

  /** Returns the indices of disjoints marginals */
  Indices buildPartition() const;

  /** Optimization solver accessor */
  OptimizationAlgorithm getOptimizationAlgorithm() const;
  void setOptimizationAlgorithm(const OptimizationAlgorithm & solver);

protected:

  /** Optimization solver */
  mutable OptimizationAlgorithm  solver_;

private:
  DistributionCollection collection_;
}; /* class OrderStatisticsMarginalChecker */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ORDERSTATISTICSMARGINALCHECKER_HXX */
