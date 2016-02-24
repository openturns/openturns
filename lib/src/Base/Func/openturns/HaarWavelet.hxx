//                                               -*- C++ -*-
/**
 *  @brief Haar wavelet function implementation
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_HAARWAVELET_HXX
#define OPENTURNS_HAARWAVELET_HXX

#include "openturns/UniVariateFunctionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class HaarWavelet
 *
 * Haar wavelet function implementation
 */

class OT_API HaarWavelet
  : public UniVariateFunctionImplementation
{
  CLASSNAME;
public:

  /** Parameter constructor */
  explicit HaarWavelet(const UnsignedInteger j = 0,
                               const UnsignedInteger k = 0,
			       const Bool isScaling = true);

  /** Virtual constructor */
  virtual HaarWavelet * clone() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** HaarWavelet are evaluated as functors */
  NumericalScalar operator() (const NumericalScalar x) const;

  /** HaarWavelet gradient */
  NumericalScalar gradient(const NumericalScalar x) const;

  /** HaarWavelet hessian */
  NumericalScalar hessian(const NumericalScalar x) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:
  UnsignedInteger j_;
  UnsignedInteger k_;
  Bool isScaling_;

} ; /* Class HaarWavelet */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HAARWAVELET_HXX */
