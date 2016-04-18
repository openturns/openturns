//                                               -*- C++ -*-
/**
 *  @brief This class implements the multivariate distribution based on a
 *         bipartite graph and a collection of distributions
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
  CLASSNAME;
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

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;


  /** Distribution collection accessor */
  void setDistributionCollection(const DistributionCollection & coll);
  const DistributionCollection & getDistributionCollection() const;

  /** Graph accessor */
  void setGraph(const BipartiteGraph & graph);
  const BipartiteGraph & getGraph() const;

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  virtual CumulativeDistributionNetwork * clone() const;

  /** Get one realization of the CumulativeDistributionNetwork */
  NumericalPoint getRealization() const;

  /** Get many realizations of the CumulativeDistributionNetwork */
  NumericalSample getSample(const UnsignedInteger size) const;

  /** Get the PDF of the CumulativeDistributionNetwork */
  using DistributionImplementation::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;

  /** Get the CDF of the CumulativeDistributionNetwork */
  using DistributionImplementation::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;

  /** Get the i-th marginal distribution */
  Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Implementation getMarginal(const Indices & indices) const;

  /** Check if the distribution is continuous */
  Bool isContinuous() const;

  /** Check if the distribution is discrete */
  Bool isDiscrete() const;

  /** Check if the distribution is integral */
  Bool isIntegral() const;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:


private:

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** Extract the components of a full dimension point to feed the index distribution */
  NumericalPoint reducePoint(const NumericalPoint & point,
                             const UnsignedInteger index) const;

  /** The collection of distribution of the cumulativeDistributionNetwork */
  DistributionPersistentCollection distributionCollection_;

  /** Bipartite graph corresponding to the network */
  BipartiteGraph graph_;

}; /* class CumulativeDistributionNetwork */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CUMULATIVEDISTRIBUTIONNETWORK_HXX */
