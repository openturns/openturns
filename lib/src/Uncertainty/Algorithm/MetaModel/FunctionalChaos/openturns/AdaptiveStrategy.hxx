//                                               -*- C++ -*-
/**
 *  @brief This is the interface class for adaptive strategies
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
#ifndef OPENTURNS_ADAPTIVESTRATEGY_HXX
#define OPENTURNS_ADAPTIVESTRATEGY_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/AdaptiveStrategyImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class AdaptiveStrategy
 *
 * This is the interface class for adaptive strategies
 */

class OT_API AdaptiveStrategy
  : public TypedInterfaceObject<AdaptiveStrategyImplementation>
{
  CLASSNAME

public:

  typedef Collection<Function> FunctionCollection;

  /** Constructor from an orthogonal basis */
  AdaptiveStrategy();

  /** Constructor from an orthogonal basis */
  AdaptiveStrategy(const OrthogonalBasis & basis,
                   const UnsignedInteger maximumDimension);

  /** Constructor from implementation */
  AdaptiveStrategy(const AdaptiveStrategyImplementation & implementation);

  /** Maximum dimension accessor */
  void setMaximumDimension(const UnsignedInteger maximumDimension);
  UnsignedInteger getMaximumDimension() const;

  /** Compute initial basis for the approximation */
  void computeInitialBasis();

  /** Update the basis for the next iteration of approximation */
  void updateBasis(const Point & alpha_k,
                   const Scalar residual,
                   const Scalar relativeError);

  /** Basis accessor */
  OrthogonalBasis getBasis() const;

  /** Psi accessor */
  FunctionCollection getPsi() const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

protected:

private:

} ; /* class AdaptiveStrategy */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ADAPTIVESTRATEGY_HXX */
