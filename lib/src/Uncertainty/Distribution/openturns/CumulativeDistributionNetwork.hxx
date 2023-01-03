//                                               -*- C++ -*-
/**
 *  @brief This class implements the multivariate distribution based on a
 *         bipartite graph and a collection of distributions
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
#ifndef OPENTURNS_CUMULATIVEDISTRIBUTIONNETWORK_HXX
#define OPENTURNS_CUMULATIVEDISTRIBUTIONNETWORK_HXX

#include "openturns/Distribution.hxx"
#include "openturns/DistributionImplementation.hxx"
#include "openturns/Collection.hxx"
#include "openturns/BipartiteGraph.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CumulativeDistributionNetwork
 *
 * This class implements the multivariate distribution based on a
 * bipartite graph and a collection of distributions
 */
class OT_API CumulativeDistributionNetwork
  : public DistributionImplementation
{
  CLASSNAME
public:

  /** A type for distribution collection */
  typedef Collection<Distribution>           DistributionCollection;
  typedef PersistentCollection<Distribution> DistributionPersistentCollection;

  /** Default constructor */
  CumulativeDistributionNetwork();

  /** Parameters constructor */
  CumulativeDistributionNetwork(const DistributionCollection & coll,
                                const BipartiteGraph & graph);

  /** Comparison operator */
  Bool operator ==(const CumulativeDistributionNetwork & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;


  /** Distribution collection accessor */
  void setDistributionCollection(const DistributionCollection & coll);
  DistributionCollection getDistributionCollection() const;

  /** Graph accessor */
  void setGraph(const BipartiteGraph & graph);
  BipartiteGraph getGraph() const;

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  CumulativeDistributionNetwork * clone() const override;

  /** Get one realization of the CumulativeDistributionNetwork */
  Point getRealization() const override;

  /** Get many realizations of the CumulativeDistributionNetwork */
  Sample getSample(const UnsignedInteger size) const override;

  /** Get the PDF of the CumulativeDistributionNetwork */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the CumulativeDistributionNetwork */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Get the i-th marginal distribution */
  Distribution getMarginal(const UnsignedInteger i) const override;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Distribution getMarginal(const Indices & indices) const override;

  /** Check if the distribution is continuous */
  Bool isContinuous() const override;

  /** Check if the distribution is discrete */
  Bool isDiscrete() const override;

  /** Check if the distribution is integral */
  Bool isIntegral() const override;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** Extract the components of a full dimension point to feed the index distribution */
  Point reducePoint(const Point & point,
                    const UnsignedInteger index) const;

  /** The collection of distribution of the cumulativeDistributionNetwork */
  DistributionPersistentCollection distributionCollection_;

  /** Bipartite graph corresponding to the network */
  BipartiteGraph graph_;

}; /* class CumulativeDistributionNetwork */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CUMULATIVEDISTRIBUTIONNETWORK_HXX */
