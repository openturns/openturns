//                                               -*- C++ -*-
/**
 *  @brief This class allows to compute integrals of a function over an interval
 *         using GaussKronrod method for 1D scalar function
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
#ifndef OPENTURNS_GAUSSKRONROD_HXX
#define OPENTURNS_GAUSSKRONROD_HXX

#include "openturns/IntegrationAlgorithmImplementation.hxx"
#include "openturns/UniVariateFunction.hxx"
#include "openturns/GaussKronrodRule.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GaussKronrod
 */

class OT_API GaussKronrod
  : public IntegrationAlgorithmImplementation
{

  CLASSNAME

public:

  /** Default constructor without parameters */
  GaussKronrod();

  /** Parameter constructor */
  GaussKronrod(const UnsignedInteger maximumSubIntervals,
               const Scalar maximumError,
               const GaussKronrodRule & rule);

  /** Virtual copy constructor */
  virtual GaussKronrod * clone() const;

  /** Compute an approximation of \int_{[a,b]}f(x)dx, where [a,b]
   * is an 1D interval
   */
  using IntegrationAlgorithmImplementation::integrate;
#ifndef SWIG
  virtual Scalar integrate(const UniVariateFunction & function,
                           const Scalar a,
                           const Scalar b) const;

  virtual Point integrate(const Function & function,
                          const Interval & interval,
                          Scalar & error) const;

  // This method allows to get the estimated integration error as a scalar
  virtual Point integrate(const Function & function,
                          const Scalar a,
                          const Scalar b,
                          Scalar & error,
                          Point & ai,
                          Point & bi,
                          Sample & fi,
                          Point & ei) const;

#endif
  // This method allows to get the estimated integration error as a Point,
  // needed by Python
  virtual Point integrate(const Function & function,
                          const Scalar a,
                          const Scalar b,
                          Point & error,
                          Point & ai,
                          Point & bi,
                          Sample & fi,
                          Point & ei) const;

  /** Maximum sub-intervals accessor */
  UnsignedInteger getMaximumSubIntervals() const;
  void setMaximumSubIntervals(const UnsignedInteger maximumSubIntervals);

  /** Maximum error accessor */
  Scalar getMaximumError() const;
  void setMaximumError(const Scalar maximumError);

  /** Rule accessor */
  GaussKronrodRule getRule() const;
  void setRule(const GaussKronrodRule & rule);

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

private:

  /** Compute the local GaussKronrod rule over [a, b] */
  Point computeRule(const Function & function,
                    const Scalar a,
                    const Scalar b,
                    Scalar & localError) const;

  Scalar computeScalarRule(const UniVariateFunction & function,
                           const Scalar a,
                           const Scalar b,
                           Scalar & localError) const;

  /* Maximum number of sub-intervals */
  UnsignedInteger maximumSubIntervals_;

  /* Maximum estimated error */
  Scalar maximumError_;

  /* Local integration rule */
  GaussKronrodRule rule_;

} ; /* class GaussKronrod */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GAUSSKRONROD_HXX */
