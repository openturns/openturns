//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of a Smolyak experiment
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SMOLYAKEXPERIMENT_HXX
#define OPENTURNS_SMOLYAKEXPERIMENT_HXX

#include "openturns/WeightedExperimentImplementation.hxx"
#include "openturns/WeightedExperiment.hxx"
#include "openturns/Collection.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class SmolyakExperiment
 *
 * The class describes the probabilistic concept of Smolyak experiment
 */
class OT_API SmolyakExperiment
  : public WeightedExperimentImplementation
{
  CLASSNAME
public:

  typedef Collection<WeightedExperiment> WeightedExperimentCollection;
  typedef PersistentCollection<WeightedExperiment> WeightedExperimentPersistentCollection;


  /** Default constructor */
  SmolyakExperiment();

  /** Parameters constructor */
  explicit SmolyakExperiment(const WeightedExperimentCollection & experimentCollection, const UnsignedInteger level);

  /** Virtual constructor */
  SmolyakExperiment * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Uniform weights ? */
  Bool hasUniformWeights() const override;

  /** Sample generation */
  Sample generateWithWeights(Point & weightsOut) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;
protected:

private:
  // Compute the tensor product nodes and weights
  void computeNodesAndWeights() const;

  // Marginal experiment collection
  WeightedExperimentPersistentCollection collection_;

  // Minimum of a multiindex
  UnsignedInteger indicesMinimum(Indices indices) const;
  
  // Combination technique multiindex set
  IndicesCollection computeCombination() const;

  // Level
  mutable UnsignedInteger level_;

  // Integration nodes; weights
  mutable Sample nodes_;
  mutable Point weights_;

}; /* class SmolyakExperiment */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SMOLYAKEXPERIMENT_HXX */
