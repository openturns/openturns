//                                               -*- C++ -*-
/**
 *  @brief This class allows one to compute integrals of a function over an
 *         interval. It implements a tensorized Fejer & Clenshaw-Curtis
 *         quadrature
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
#ifndef OPENTURNS_FEJERALGORITHM_HXX
#define OPENTURNS_FEJERALGORITHM_HXX

#include "openturns/IntegrationAlgorithmImplementation.hxx"
#include "openturns/IntegrationAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FejerAlgorithm
 */

class OT_API FejerAlgorithm
  : public IntegrationAlgorithmImplementation
{

  CLASSNAME

public:
  enum IntegrationMethod {FEJERTYPE1 = 0, FEJERTYPE2 = 1, CLENSHAWCURTIS};

  /** Default constructor */

  explicit FejerAlgorithm(const UnsignedInteger dimension = 1,
                          const IntegrationMethod method = CLENSHAWCURTIS);

  /** Parameter constructor */
  explicit FejerAlgorithm(const Indices & discretization,
                          const IntegrationMethod method = CLENSHAWCURTIS);

  /** Virtual copy constructor */
  FejerAlgorithm *clone() const override;

  /** Compute an approximation of \int_a^b f(x_1,\dots,x_n)dx_1\dotsdx_n, where [a,b] is an nD interval.*/
  using IntegrationAlgorithmImplementation::integrate;
  Point integrate(const Function &function,
                  const Interval &interval) const override;
  Point integrateWithNodes(const Function &function,
                           const Interval &interval,
                           Sample &adaptedNodesOut) const;

  /** Discretization accessor */
  Indices getDiscretization() const;

  /** Nodes accessor */
  Sample getNodes() const;

  /** Weights accessor */
  Point getWeights() const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

private:
  /* Generate nodes and weights */
  void generateNodesAndWeights(const IntegrationMethod method);

  // Generate nodes and weights for ClenshawCurtis
  void generateNodesAndWeightsClenshawCurtis(Collection<Point> & marginalNodes, Collection<Point> & marginalWeights);
  // Generate nodes and weights for Fejer Type 1
  void generateNodesAndWeightsFejerType1(Collection<Point> & marginalNodes, Collection<Point> & marginalWeights);
  // Generate nodes and weights for Fejer Type 2
  void generateNodesAndWeightsFejerType2(Collection<Point> & marginalNodes, Collection<Point> & marginalWeights);

  /* Discretization of the tensorized rule */
  Indices discretization_;

  /* Integration nodes */
  Sample nodes_;

  /* Integration weights */
  Point weights_;
} ; /* class FejerAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FEJERALGORITHM_HXX */
