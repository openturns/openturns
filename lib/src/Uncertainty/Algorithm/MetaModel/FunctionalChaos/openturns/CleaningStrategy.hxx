//                                               -*- C++ -*-
/**
 *  @brief An implementation returning the set of polynomials in sequence
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
  CLASSNAME;
public:


  /** Constructor from an orthogonal basis */
  CleaningStrategy(const OrthogonalBasis & basis,
                   const UnsignedInteger maximumDimension,
                   const Bool verbose = false);

  /** Constructor from an orthogonal basis */
  CleaningStrategy(const OrthogonalBasis & basis,
                   const UnsignedInteger maximumDimension,
                   const UnsignedInteger maximumSize,
                   const NumericalScalar significanceFactor,
                   const Bool verbose = false);

  /** Virtual constructor */
  virtual CleaningStrategy * clone() const;

  /** Compute initial basis for the approximation */
  void computeInitialBasis();

  /** Update the basis for the next iteration of approximation */
  void updateBasis(const NumericalPoint & alpha_k,
                   const NumericalScalar residual,
                   const NumericalScalar relativeError);

  /** String converter */
  virtual String __repr__() const;

  /** Current vector index accessor */
  UnsignedInteger getCurrentVectorIndex() const;

  /** Maximum size accessor */
  UnsignedInteger getMaximumSize() const;
  void setMaximumSize(const UnsignedInteger maximumSize);

  /** Significance factor */
  NumericalScalar getSignificanceFactor() const;
  void setSignificanceFactor(const NumericalScalar significanceFactor);

  /** Verbose accessor */
  Bool getVerbose() const;
  void setVerbose(const Bool verbose);

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);


protected:

  friend class Factory<CleaningStrategy>;

  /** Default constructor */
  CleaningStrategy();

private:
  // Index of the next vector to be generated
  UnsignedInteger currentVectorIndex_;

  // Maximum size of the curent basis
  UnsignedInteger maximumSize_;

  // Relative significance factor of a vector
  NumericalScalar significanceFactor_;

  // Verbose
  Bool verbose_;

} ; /* class CleaningStrategy */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CLEANINGSTRATEGY_HXX */
