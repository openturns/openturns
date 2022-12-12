//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an TensorProductExperiment
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
#ifndef OPENTURNS_TENSORPRODUCTEXPERIMENT_HXX
#define OPENTURNS_TENSORPRODUCTEXPERIMENT_HXX

#include "openturns/WeightedExperimentImplementation.hxx"
#include "openturns/WeightedExperiment.hxx"
#include "openturns/Indices.hxx"
#include "openturns/PersistentCollection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class TensorProductExperiment
 *
 * The class describes the probabilistic concept of TensorProductExperiment
 */
class OT_API TensorProductExperiment
  : public WeightedExperimentImplementation
{
  CLASSNAME
public:

  typedef Collection<WeightedExperiment> WeightedExperimentCollection;
  typedef PersistentCollection<WeightedExperiment> WeightedExperimentPersistentCollection;

  /** Default constructor */
  TensorProductExperiment();

  /** Parameters constructor */
  explicit TensorProductExperiment(const WeightedExperimentCollection & experimentCollection);

  /** Virtual constructor */
  TensorProductExperiment * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Uniform weights ? */
  Bool hasUniformWeights() const override;

  /** Sample generation */
  Sample generateWithWeights(Point & weightsOut) const override;

  /** Distribution collection accessor */
  void setWeightedExperimentCollection(const WeightedExperimentCollection & coll);
  WeightedExperimentCollection getWeightedExperimentCollection() const;

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

  // Integration nodes; weights
  mutable Sample nodes_;
  mutable Point weights_;

}; /* class TensorProductExperiment */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TENSORPRODUCTEXPERIMENT_HXX */
