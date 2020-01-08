//                                               -*- C++ -*-
/**
 *  @brief Charlier polynomial factory
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
#ifndef OPENTURNS_CHARLIERFACTORY_HXX
#define OPENTURNS_CHARLIERFACTORY_HXX

#include "openturns/OrthogonalUniVariatePolynomialFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class CharlierFactory
 *
 * Charlier polynomial factory
 */

class OT_API CharlierFactory
  : public OrthogonalUniVariatePolynomialFactory
{
  CLASSNAME
public:


  /** Default constructor, associated with the default Poisson distribution of parameter 1. */
  CharlierFactory();

  /** Parameter constructor: lambda is the order of the generalized Charlier polynomial, associated with the Poisson(lambda) distribution */
  CharlierFactory(const Scalar lambda);

  /** Virtual constructor */
  virtual CharlierFactory * clone() const;

  /** Calculate the coefficients of recurrence a0n, a1n, a2n such that
      Pn+1(x) = (a0n * x + a1n) * Pn(x) + a2n * Pn-1(x) */
  Coefficients getRecurrenceCoefficients(const UnsignedInteger n) const;

  /** K accessor */
  Scalar getLambda() const;

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:
  /** Parameter of the Charlier polynomial */
  Scalar lambda_;

} ; /* class CharlierFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CHARLIERFACTORY_HXX */
