//                                               -*- C++ -*-
/**
 *  @brief Estimation by matching quantiles
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
#ifndef OPENTURNS_QUANTILEMATCHINGFACTORY_HXX
#define OPENTURNS_QUANTILEMATCHINGFACTORY_HXX

#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/DistributionFactory.hxx"
#include "openturns/OptimizationAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class QuantileMatchingFactory
 */
class OT_API QuantileMatchingFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:
  /** Default constructor */
  QuantileMatchingFactory();

  /** Parameters constructor */
  explicit QuantileMatchingFactory(const Distribution & distribution,
                                   const Point & probabilities = Point(),
                                   const Interval & optimizationBounds = Interval());

  /** Virtual constructor */
  QuantileMatchingFactory * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  using DistributionFactoryImplementation::build;

  /* Here is the interface that all derived class must implement */

  /** Build a distribution based on a sample */
  Distribution build(const Sample & sample) const override;

  /** Build a distribution based on a set of parameters */
  Distribution build(const Point & parameter) const override;

  /** Build a distribution using its default constructor */
  Distribution build() const override;

  /** Build a distribution from its quantiles */
  Distribution buildFromQuantiles(const Point & quantiles) const;

  /** Solver accessor */
  void setOptimizationAlgorithm(const OptimizationAlgorithm & solver);
  OptimizationAlgorithm getOptimizationAlgorithm() const;

  /** Accessor to optimization bounds */
  void setOptimizationBounds(const Interval & optimizationBounds);
  Interval getOptimizationBounds() const;

  /** Accessor to known parameter */
  void setKnownParameter(const Point & values, const Indices & positions);
  Point getKnownParameterValues() const;
  Indices getKnownParameterIndices() const;

  /** Accessor to fractiles */
  void setProbabilities(const Point & probabilities);
  Point getProbabilities() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /* The underlying distribution */
  Distribution distribution_;
  /* The list of probabilities */
  Point probabilities_;

  /* Solver & optimization problem for log-likelihood maximization */
  OptimizationAlgorithm solver_;

  /* Bounds used for parameter optimization */
  Interval optimizationBounds_;

  /* Known parameter */
  Point knownParameterValues_;
  Indices knownParameterIndices_;

}; /* class QuantileMatchingFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_QUANTILEMATCHINGFACTORY_HXX */
