//                                               -*- C++ -*-
/**
 *  @brief This is a abstract class for adaptive strategy implementations
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
#ifndef OPENTURNS_ADAPTIVESTRATEGYIMPLEMENTATION_HXX
#define OPENTURNS_ADAPTIVESTRATEGYIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/OrthogonalBasis.hxx"
#include "openturns/Point.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class AdaptiveStrategyImplementation
 *
 * This is a abstract class for adaptive strategy implementations
 */

class OT_API AdaptiveStrategyImplementation
  : public PersistentObject
{
  CLASSNAME
  // Make the FunctionalChaosAlgorithm class a friend of the AdaptiveStrategyImplementation class
  // as this last class is in charge of managing data that are part of the FunctionalChaosAlgorithm
  // state
  friend class FunctionalChaosAlgorithm;
  // Make the ProjectionStrategyImplementation class a friend of the AdaptiveStrategyImplementation class
  // as this last class is in charge of data needed by the ProjectionStrategyImplementation to update the state of the FunctionalChaosAlgorithm class
  friend class ProjectionStrategyImplementation;

public:

  typedef Collection<Function> FunctionCollection;

  /** Default constructor */
  AdaptiveStrategyImplementation();

  /** Constructor from an orthogonal basis */
  AdaptiveStrategyImplementation(const OrthogonalBasis & basis,
                                 const UnsignedInteger maximumDimension);

  /** Virtual constructor */
  AdaptiveStrategyImplementation * clone() const override;


  /** Maximum dimension accessor */
  void setMaximumDimension(const UnsignedInteger maximumDimension);
  UnsignedInteger getMaximumDimension() const;

  /** Compute initial basis for the approximation */
  virtual void computeInitialBasis();

  /** Update the basis for the next iteration of approximation */
  virtual void updateBasis(const Point & alpha_k,
                           const Scalar residual,
                           const Scalar relativeError);

  /** String converter */
  String __repr__() const override;

  /** Basis accessor */
  OrthogonalBasis getBasis() const;

  /** Psi accessor */
  FunctionCollection getPsi() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** The orthogonal basis */
  OrthogonalBasis basis_;

  /** Maximum dimension of the basis to be generated */
  UnsignedInteger maximumDimension_;

  /** The collection of indices of the vectors in the partial basis with respect to the orthogonal basis */
  Indices I_p_;

  /** The ranks of the added vectors in the current partial basis */
  Indices addedPsi_k_ranks_;

  /** The ranks of the vectors common between the previous partial basis and the current partial basis, as found in the previous partial basis */
  Indices conservedPsi_k_ranks_;

  /** The ranks of the removed vectors in the previous partial basis */
  Indices removedPsi_k_ranks_;

  /** The collection of vectors upon which we project the (composed) model */
  FunctionCollection Psi_k_p_;

  /** The full basis */
  FunctionCollection Psi_;
private:

} ; /* class AdaptiveStrategyImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ADAPTIVESTRATEGYIMPLEMENTATION_HXX */
