//                                               -*- C++ -*-
/**
 *  @brief Jacobi polynomial factory
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_JACOBIFACTORY_HXX
#define OPENTURNS_JACOBIFACTORY_HXX

#include "openturns/OrthogonalUniVariatePolynomialFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class JacobiFactory
 *
 * Jacobi polynomial factory
 */

class OT_API JacobiFactory
  : public OrthogonalUniVariatePolynomialFactory
{
  CLASSNAME
public:


  /** Default constructor: (1, 1) order Jacobi polynomial associated with the default Beta() = Beta(2, 4, -1, 1) distribution which is equal to the Epanechnikov distribution */
  JacobiFactory();

  /** Parameter constructor: (alpha, beta) is the order of the Jacobi polynomial, associated with the Beta(beta + 1, alpha + beta + 2, -1, 1) distribution in the ANALYSIS parameter set or to the Beta(alpha, beta, -1, 1) distribution in the PROBABILITY parameter set */
  JacobiFactory(const Scalar alpha,
                const Scalar beta,
                const ParameterSet parameterization = ANALYSIS);

  /** Virtual constructor */
  JacobiFactory * clone() const override;

  /** Calculate the coefficients of recurrence a0n, a1n, a2n such that
      Pn+1(x) = (a0n * x + a1n) * Pn(x) + a2n * Pn-1(x) */
  Coefficients getRecurrenceCoefficients(const UnsignedInteger n) const override;

  /** Alpha accessor */
  Scalar getAlpha() const;

  /** Beta accessor */
  Scalar getBeta() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  /* First parameter of the Jacobi polynomial */
  Scalar alpha_;
  /* Second parameter of the Jacobi polynomial */
  Scalar beta_;

} ; /* class JacobiFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_JACOBIFACTORY_HXX */
