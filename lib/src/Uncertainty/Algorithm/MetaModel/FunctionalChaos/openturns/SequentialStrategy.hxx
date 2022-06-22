//                                               -*- C++ -*-
/**
 *  @brief An implementation returning the set of polynomials in sequence
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SEQUENTIALSTRATEGY_HXX
#define OPENTURNS_SEQUENTIALSTRATEGY_HXX

#include "openturns/AdaptiveStrategyImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class SequentialStrategy
 * @deprecated
 *
 * An implementation returning the set of polynomials in sequence
 */

class OT_API SequentialStrategy
  : public AdaptiveStrategyImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  SequentialStrategy();

  /** Constructor from an orthogonal basis */
  SequentialStrategy(const OrthogonalBasis & basis,
                     const UnsignedInteger maximumDimension,
                     const Bool verbose = false);

  /** Virtual constructor */
  SequentialStrategy * clone() const override;

  /** Compute initial basis for the approximation */
  void computeInitialBasis() override;

  /** Update the basis for the next iteration of approximation */
  void updateBasis(const Point & alpha_k,
                   const Scalar residual,
                   const Scalar relativeError) override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  // Index of the next vector to be generated
  UnsignedInteger currentVectorIndex_;

  // Verbose
  Bool verbose_;

} ; /* class SequentialStrategy */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SEQUENTIALSTRATEGY_HXX */
