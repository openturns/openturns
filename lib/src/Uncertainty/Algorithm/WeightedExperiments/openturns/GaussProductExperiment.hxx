//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an GaussProductExperiment
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
#ifndef OPENTURNS_GAUSSPRODUCTEXPERIMENT_HXX
#define OPENTURNS_GAUSSPRODUCTEXPERIMENT_HXX

#include "openturns/WeightedExperimentImplementation.hxx"
#include "openturns/OrthogonalUniVariatePolynomialFamily.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Indices.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class GaussProductExperiment
 *
 * The class describes the probabilistic concept of monteCarloExperiment plan
 */
class OT_API GaussProductExperiment
  : public WeightedExperimentImplementation
{
  CLASSNAME
public:

  typedef Collection<OrthogonalUniVariatePolynomialFamily> OrthogonalUniVariatePolynomialFamilyCollection;
  typedef PersistentCollection<OrthogonalUniVariatePolynomialFamily> OrthogonalUniVariatePolynomialFamilyPersistentCollection;

  /** Default constructor */
  GaussProductExperiment();

  /** Parameters constructor */
  explicit GaussProductExperiment(const Indices & marginalSizes);

  /** Parameters constructor */
  explicit GaussProductExperiment(const Distribution & distribution);

  /** Parameters constructor */
  GaussProductExperiment(const Distribution & distribution,
                         const Indices & marginalSizes);

  /** Virtual constructor */
  GaussProductExperiment * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Uniform weights ? */
  Bool hasUniformWeights() const override;

  /** Sample generation */
  Sample generateWithWeights(Point & weightsOut) const override;

  /** Return isRandom flag */
  Bool isRandom() const override;

  /** Marginal sizes accessor */
  void setMarginalSizes(const Indices & marginalSizes);
  Indices getMarginalSizes() const;

  /** Distribution accessor */
  void setDistribution(const Distribution & distribution) override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;
protected:

private:
  // Compute the tensor product nodes and weights
  void computeNodesAndWeights() const;

  // Distribution and marginal sizes accessor
  void setDistributionAndMarginalSizes(const Distribution & distribution,
                                       const Indices & marginalSizes);

  // Marginal orthogonal univariate polynomial family collection
  OrthogonalUniVariatePolynomialFamilyPersistentCollection collection_;

  // Marginal sizes
  Indices marginalSizes_;

  // Integration nodes; weights
  mutable Sample nodes_;
  mutable Point weights_;

  // Flag to manage the computation of nodes and weights
  mutable Bool isAlreadyComputedNodesAndWeights_;

}; /* class GaussProductExperiment */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GAUSSPRODUCTEXPERIMENT_HXX */
