//                                               -*- C++ -*-
/**
 *  @brief This class allows to compute integrals of a function over an interval
 *         using GaussKronrod method for 1D scalar function
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_GAUSSKRONROD_HXX
#define OPENTURNS_GAUSSKRONROD_HXX

#include "openturns/IntegrationAlgorithmImplementation.hxx"
#include "openturns/GaussKronrodRule.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GaussKronrod
 */

class OT_API GaussKronrod
  : public IntegrationAlgorithmImplementation
{

  CLASSNAME;

public:

  /** Default constructor without parameters */
  GaussKronrod();

  /** Parameter constructor */
  GaussKronrod(const UnsignedInteger maximumSubIntervals,
               const NumericalScalar maximumError,
               const GaussKronrodRule & rule);

  /** Virtual copy constructor */
  virtual GaussKronrod * clone() const;

  /** Compute an approximation of \int_{[a,b]}f(x)dx, where [a,b]
   * is an 1D interval
   */
  using IntegrationAlgorithmImplementation::integrate;
#ifndef SWIG
  virtual NumericalPoint integrate(const Function & function,
                                   const Interval & interval,
                                   NumericalScalar & error) const;

  // This method allows to get the estimated integration error as a scalar
  virtual NumericalPoint integrate(const Function & function,
                                   const NumericalScalar a,
                                   const NumericalScalar b,
                                   NumericalScalar & error,
                                   NumericalPoint & ai,
                                   NumericalPoint & bi,
                                   Sample & fi,
                                   NumericalPoint & ei) const;

#endif
  // This method allows to get the estimated integration error as a NumericalPoint,
  // needed by Python
  virtual NumericalPoint integrate(const Function & function,
                                   const NumericalScalar a,
                                   const NumericalScalar b,
                                   NumericalPoint & error,
                                   NumericalPoint & ai,
                                   NumericalPoint & bi,
                                   Sample & fi,
                                   NumericalPoint & ei) const;

  /** Maximum sub-intervals accessor */
  UnsignedInteger getMaximumSubIntervals() const;
  void setMaximumSubIntervals(const UnsignedInteger maximumSubIntervals);

  /** Maximum error accessor */
  NumericalScalar getMaximumError() const;
  void setMaximumError(const NumericalScalar maximumError);

  /** Rule accessor */
  GaussKronrodRule getRule() const;
  void setRule(const GaussKronrodRule & rule);

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

private:

  /** Compute the local GaussKronrod rule over [a, b] */
  NumericalPoint computeRule(const Function & function,
                             const NumericalScalar a,
                             const NumericalScalar b,
                             NumericalScalar & localError) const;

  /* Maximum number of sub-intervals */
  UnsignedInteger maximumSubIntervals_;

  /* Maximum estimated error */
  NumericalScalar maximumError_;

  /* Local integration rule */
  GaussKronrodRule rule_;

} ; /* class GaussKronrod */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GAUSSKRONROD_HXX */
