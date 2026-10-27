//                                               -*- C++ -*-
/**
 *  @brief This class implements the multivariate distribution based on a
 *         bipartite graph and a collection of distributions
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
  using DistributionImplementation::operator ==;
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

  /** Get the log-PDF of the CumulativeDistributionNetwork */
  using DistributionImplementation::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  /** Get the CDF of the CumulativeDistributionNetwork */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Get the CDF of Xi | X1, ..., Xi-1 */
  using DistributionImplementation::computeConditionalCDF;
  Scalar computeConditionalCDF(const Scalar xj, const Point & xcond) const override;

  /** Get the PDF of Xi | X1, ..., Xi-1 */
  using DistributionImplementation::computeConditionalPDF;
  Scalar computeConditionalPDF(const Scalar xj, const Point & xcond) const override;

  /** Get the quantile of Xi | X1, ..., Xi-1 */
  using DistributionImplementation::computeConditionalQuantile;
  Scalar computeConditionalQuantile(const Scalar p, const Point & xcond) const override;

  /** Get the i-th marginal distribution */
  Distribution getMarginal(const UnsignedInteger i) const override;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Distribution getMarginal(const Indices & indices) const override;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const override;

  /** Check if the distribution is continuous */
  Bool isContinuous() const override;

  /** Check if the distribution is discrete */
  Bool isDiscrete() const override;

  /** Check if the distribution is integral */
  Bool isIntegral() const override;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const override;

  /** Parameters accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;
  Description getParameterDescription() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** Rebuild the incidence caches from the collection and the graph */
  void updateStructure();

  /** Extract the components of a full dimension point to feed the index distribution */
  Point reducePoint(const Point & point,
                    const UnsignedInteger index) const;

  /** Compute the log of a local mixed partial derivative (or finite difference) */
  Scalar computeLocalPartialLog(const UnsignedInteger factorIndex,
                                const Point & localPoint,
                                const Indices & derivativePositions) const;

  /** Compute the log of a local CDF, factorised when possible */
  Scalar computeLocalLogCDF(const UnsignedInteger factorIndex,
                            const Point & localPoint) const;

  /** Compute the log of a CDF at a finite point, factorised when possible */
  Scalar computeFactorLogCDF(const Distribution & distribution,
                             const DistributionCollection & univariateMarginals,
                             const Point & localPoint) const;

  /** Compute the log of a mixed partial derivative at a finite point */
  Scalar computeFactorPartialLog(const UnsignedInteger factorIndex,
                                 const Distribution & distribution,
                                 const DistributionCollection & univariateMarginals,
                                 const Point & localPoint,
                                 const Indices & derivativePositions) const;

  /** Compute the log of a mixed partial derivative of the joint CDF */
  Scalar computePartialDerivativeLog(const Point & point,
                                     const Indices & derivativeIndices,
                                     const Bool useDifferences) const;

  /** Compute the log of a mixed difference of the joint CDF of integral factors */
  Scalar computeDiscretePartialLog(const Point & point,
                                   const Indices & derivativeIndices) const;

  /** Compute the log-PDF (continuous) or log-PMF (integral) */
  Scalar computeLogPDFKernel(const Point & point,
                             const Bool useDifferences) const;

  /** The collection of distribution of the cumulativeDistributionNetwork */
  DistributionPersistentCollection distributionCollection_;

  /** Bipartite graph corresponding to the network */
  BipartiteGraph graph_;

  /** For each factor, the global indices of its variables, in local order */
  Collection<Indices> factorVariables_;

  /** For each variable, the indices of the factors containing it */
  Collection<Indices> variableFactors_;

  /** For each variable, the number of factors containing it */
  Indices variableMultiplicity_;

  /** For each independent factor, the collection of its univariate marginals */
  Collection<DistributionCollection> factorUnivariateMarginals_;

  /** Whether the variables can be partitioned into independent factor blocks */
  Bool blockIndependent_ = false;

}; /* class CumulativeDistributionNetwork */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CUMULATIVEDISTRIBUTIONNETWORK_HXX */
