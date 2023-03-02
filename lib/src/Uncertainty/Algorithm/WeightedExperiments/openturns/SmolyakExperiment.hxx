//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of a Smolyak experiment
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

  /** Compare two points approximately, internal use only */
  static Bool ComparePointsApproximately(const Point & x, const Point & y);

  /** Default constructor */
  SmolyakExperiment();

  /** Parameters constructor */
  SmolyakExperiment(const WeightedExperimentCollection & experimentCollection,
                    const UnsignedInteger level);

  /** Virtual constructor */
  SmolyakExperiment * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Uniform weights ? */
  Bool hasUniformWeights() const override;

  /** Nodes and weights generation */
  Sample generateWithWeights(Point & weightsOut) const override;

  /** Nodes generation */
  Sample generate() const override;

  /** Distribution collection accessor */
  void setExperimentCollection(const WeightedExperimentCollection & coll);
  WeightedExperimentCollection getExperimentCollection() const;

  /** Level accessor */
  void setLevel(const UnsignedInteger level);
  UnsignedInteger getLevel() const;

  /** Size accessor */
  void setSize(const UnsignedInteger size) override;
  UnsignedInteger getSize() const override;

  /** Combination technique multiindex set, internal use only */
  IndicesCollection computeCombination() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  // Merge (potentially) duplicated nodes
  void mergeNodesAndWeights(Sample & nodes, Point & weights) const;

  // Minimum of a multiindex
  UnsignedInteger indicesMinimum(const Indices & indices) const;

  // Marginal experiment collection
  WeightedExperimentPersistentCollection collection_;

  // Level
  UnsignedInteger level_ = 0;

}; /* class SmolyakExperiment */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SMOLYAKEXPERIMENT_HXX */
