//                                               -*- C++ -*-
/**
 *  @brief Legendre polynomial factory
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
#ifndef OPENTURNS_LEGENDREFACTORY_HXX
#define OPENTURNS_LEGENDREFACTORY_HXX

#include "openturns/OrthogonalUniVariatePolynomialFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class LegendreFactory
 *
 * Legendre polynomial factory
 */

class OT_API LegendreFactory
  : public OrthogonalUniVariatePolynomialFactory
{
  CLASSNAME
public:


  /** Default constructor */
  LegendreFactory();

  /** Virtual constructor */
  LegendreFactory * clone() const override;

  /** Calculate the coefficients of recurrence a0n, a1n, a2n such that
      Pn+1(x) = (a0n * x + a1n) * Pn(x) + a2n * Pn-1(x) */
  Coefficients getRecurrenceCoefficients(const UnsignedInteger n) const override;

  /** String converter */
  String __repr__() const override;

  /** Roots of the polynomial of degree n */
  Point getRoots(const UnsignedInteger n) const override;

  /** Nodes and weights of the polynomial of degree n */
  Point getNodesAndWeights(const UnsignedInteger n,
                           Point & weightsOut) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

} ; /* class LegendreFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LEGENDREFACTORY_HXX */
