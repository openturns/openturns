//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an monteCarloExperiment plane
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
  explicit GaussProductExperiment(const Indices & marginalDegrees);

  /** Parameters constructor */
  explicit GaussProductExperiment(const Distribution & distribution);

  /** Parameters constructor */
  GaussProductExperiment(const Distribution & distribution,
                         const Indices & marginalDegrees);

  /** Virtual constructor */
  virtual GaussProductExperiment * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Uniform weights ? */
  virtual Bool hasUniformWeights() const;

  /** Sample generation */
  Sample generateWithWeights(Point & weightsOut) const;

  /** Marginal degrees accessor */
  void setMarginalDegrees(const Indices & marginalDegrees);
  Indices getMarginalDegrees() const;

  /** Distribution accessor */
  void setDistribution(const Distribution & distribution);

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);
protected:

private:
  // Compute the tensor product nodes and weights
  void computeNodesAndWeights() const;

  // Distribution and marginal degrees accessor
  void setDistributionAndMarginalDegrees(const Distribution & distribution,
                                         const Indices & marginalDegrees);

  // Marginal orthogonal univariate polynomial family collection
  OrthogonalUniVariatePolynomialFamilyPersistentCollection collection_;

  // Marginal degrees
  Indices marginalDegrees_;

  // Integration nodes; weights
  mutable Sample nodes_;
  mutable Point weights_;

  // Flag to manage the computation of nodes and weights
  mutable Bool isAlreadyComputedNodesAndWeights_;

}; /* class GaussProductExperiment */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GAUSSPRODUCTEXPERIMENT_HXX */
