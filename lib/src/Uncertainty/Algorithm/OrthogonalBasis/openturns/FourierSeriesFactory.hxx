//                                               -*- C++ -*-
/**
 *  @brief Fourier function factory
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
#ifndef OPENTURNS_FOURIERSERIESFACTORY_HXX
#define OPENTURNS_FOURIERSERIESFACTORY_HXX

#include "openturns/OrthogonalUniVariateFunctionFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class FourierSeriesFactory
 *
 * Fourier function factory
 */

class OT_API FourierSeriesFactory
  : public OrthogonalUniVariateFunctionFactory
{
  CLASSNAME
public:


  /** Default constructor */
  FourierSeriesFactory();

  /** Virtual constructor */
  FourierSeriesFactory * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** The method to get the function of any order. */
  UniVariateFunction build(const UnsignedInteger order) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

} ; /* class FourierSeriesFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FOURIERSERIESFACTORY_HXX */
