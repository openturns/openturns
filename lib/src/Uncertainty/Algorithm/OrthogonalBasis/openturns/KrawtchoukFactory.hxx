//                                               -*- C++ -*-
/**
 *  @brief Krawtchouk polynomial factory
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
#ifndef OPENTURNS_KRAWTCHOUKFACTORY_HXX
#define OPENTURNS_KRAWTCHOUKFACTORY_HXX

#include "openturns/OrthogonalUniVariatePolynomialFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class KrawtchoukFactory
 *
 * Krawtchouk polynomial factory
 */

class OT_API KrawtchoukFactory
  : public OrthogonalUniVariatePolynomialFactory
{
  CLASSNAME
public:


  /** Default constructor: (1, 0.5) order Krawtchouk polynomial associated with the default Binomial() = Binomial(1, 0.5) distribution which is equal to the Bernoulli(0.5) distribution */
  KrawtchoukFactory();

  /** Parameter constructor: (n, p) is the order of the Krawtchouk polynomial, associated with the Binomial(n, p) distribution */
  KrawtchoukFactory(const UnsignedInteger n,
                    const Scalar p);

  /** Virtual constructor */
  KrawtchoukFactory * clone() const override;

  /** Calculate the coefficients of recurrence a0n, a1n, a2n such that
      Pn+1(x) = (a0n * x + a1n) * Pn(x) + a2n * Pn-1(x) */
  Coefficients getRecurrenceCoefficients(const UnsignedInteger n) const override;

  /** N accessor */
  UnsignedInteger getN() const;

  /** P accessor */
  Scalar getP() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  /* First parameter of the Krawtchouk polynomial */
  UnsignedInteger n_;
  /* Second parameter of the Krawtchouk polynomial */
  Scalar p_;

} ; /* class KrawtchoukFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KRAWTCHOUKFACTORY_HXX */
