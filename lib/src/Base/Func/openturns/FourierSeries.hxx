//                                               -*- C++ -*-
/**
 *  @brief Fourier series function implementation
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_FOURIERSERIES_HXX
#define OPENTURNS_FOURIERSERIES_HXX

#include "openturns/UniVariateFunctionImplementation.hxx"
#include "openturns/UniVariatePolynomial.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class FourierSeries
 *
 * Fourier series function implementation
 */

class OT_API FourierSeries
  : public UniVariateFunctionImplementation
{
  CLASSNAME
public:

  /** Parameter constructor */
  explicit FourierSeries(const Bool isCosine = true,
                         const UnsignedInteger k = 0);

  /** Virtual constructor */
  FourierSeries * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** FourierSeries are evaluated as functors */
  Scalar operator() (const Scalar x) const override;

  /** FourierSeries gradient */
  Scalar gradient(const Scalar x) const override;

  /** FourierSeries hessian */
  Scalar hessian(const Scalar x) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  Bool isCosine_;
  UnsignedInteger k_;

} ; /* Class FourierSeries */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FOURIERSERIES_HXX */
