//                                               -*- C++ -*-
/**
 *  @brief An implementation returning the set of polynomials in sequence
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
#ifndef OPENTURNS_CLEANINGSTRATEGY_HXX
#define OPENTURNS_CLEANINGSTRATEGY_HXX

#include "openturns/AdaptiveStrategyImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class CleaningStrategy
 *
 * An implementation returning the set of polynomials in sequence
 */

class OT_API CleaningStrategy
  : public AdaptiveStrategyImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  CleaningStrategy();

  /** Constructor from an orthogonal basis */
  CleaningStrategy(const OrthogonalBasis & basis,
                   const UnsignedInteger maximumDimension);

  /** Constructor from an orthogonal basis */
  CleaningStrategy(const OrthogonalBasis & basis,
                   const UnsignedInteger maximumDimension,
                   const UnsignedInteger maximumSize,
                   const Scalar significanceFactor);

  /** Virtual constructor */
  CleaningStrategy * clone() const override;

  /** Compute initial basis for the approximation */
  void computeInitialBasis() override;

  /** Update the basis for the next iteration of approximation */
  void updateBasis(const Point & alpha_k,
                   const Scalar residual,
                   const Scalar relativeError) override;

  /** String converter */
  String __repr__() const override;

  /** Current vector index accessor */
  UnsignedInteger getCurrentVectorIndex() const;

  /** Maximum size accessor */
  UnsignedInteger getMaximumSize() const;
  void setMaximumSize(const UnsignedInteger maximumSize);

  /** Significance factor */
  Scalar getSignificanceFactor() const;
  void setSignificanceFactor(const Scalar significanceFactor);

  /** @deprecated Verbose accessor */
  Bool getVerbose() const;
  void setVerbose(const Bool verbose);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  // Index of the next vector to be generated
  UnsignedInteger currentVectorIndex_ = 0;

  // Maximum size of the current basis
  UnsignedInteger maximumSize_ = 0;

  // Relative significance factor of a vector
  Scalar significanceFactor_ = 0.0;

} ; /* class CleaningStrategy */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CLEANINGSTRATEGY_HXX */
