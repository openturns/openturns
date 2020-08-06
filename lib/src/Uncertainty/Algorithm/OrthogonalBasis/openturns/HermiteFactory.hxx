//                                               -*- C++ -*-
/**
 *  @brief Hermite polynomial factory
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
#ifndef OPENTURNS_HERMITEFACTORY_HXX
#define OPENTURNS_HERMITEFACTORY_HXX

#include "openturns/OrthogonalUniVariatePolynomialFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class HermiteFactory
 *
 * Hermite polynomial factory
 */

class OT_API HermiteFactory
  : public OrthogonalUniVariatePolynomialFactory
{
  CLASSNAME
public:


  /** Default constructor */
  HermiteFactory();

  /** Virtual constructor */
  HermiteFactory * clone() const override;

  /** Calculate the coefficients of recurrence a0n, a1n, a2n such that
      Pn+1(x) = (a0n * x + a1n) * Pn(x) + a2n * Pn-1(x) */
  Coefficients getRecurrenceCoefficients(const UnsignedInteger n) const override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

} ; /* class HermiteFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HERMITEFACTORY_HXX */
