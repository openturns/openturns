//                                               -*- C++ -*-
/**
 *  @brief Gauss-LP quadrature rule builder
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

#ifndef OPENTURNS_GAUSSLPQUADRATURE_HXX
#define OPENTURNS_GAUSSLPQUADRATURE_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Function.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Sample.hxx"
#include "openturns/GaussLPQuadratureRule.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GaussLPQuadrature
 *
 * Build a minimal quadrature rule adapted to the measure of a finite set of
 * functions by a Gauss-LP approach (linear programming followed by local
 * refinement).
 *
 * References:
 * - Ryu, E.K. and Boyd, S.P., "Extensions of Gauss Quadrature Via Linear
 *   Programming", Foundations of Computational Mathematics, 15(4):953-971,
 *   2015. Linear programming formulation selecting a sparse set of positive
 *   weights that integrates the first n functions exactly.
 * - Jakeman, J.D. and Narayan, A., "Generation and application of
 *   multivariate polynomial quadrature rules", Computer Methods in Applied
 *   Mechanics and Engineering, 338:134-161, 2018. Multivariate extension,
 *   with a greedy clustering of the linear programming support down to n
 *   nodes.
 *
 * The builder is configured with a finite collection of scalar functions and
 * the measure against which their moments are computed. An optional sample of
 * anchor nodes can be supplied. When the anchors can reproduce the moments
 * on their own, the rule is the subset of anchors selected by the linear
 * program. Otherwise every anchor is kept with a positive weight, and the
 * linear program adds new nodes to complete the rule.
 *
 * This class is a rule builder, not an experiment: the number of nodes is an
 * output of the algorithm, while the number of functions to integrate is the
 * input. The immutable rule itself is represented by GaussLPQuadratureRule.
 */
class OT_API GaussLPQuadrature
  : public PersistentObject
{
  CLASSNAME
public:

  typedef Collection<Function>           FunctionCollection;
  typedef PersistentCollection<Function> FunctionPersistentCollection;

  GaussLPQuadrature();

  /** Constructor from the functions whose moments are reproduced and the
      measure against which they are integrated. */
  GaussLPQuadrature(const FunctionCollection & functions,
                    const Distribution & measure);

  /** Constructor from the functions, the measure and a sample of anchor
      nodes that must be retained in the rule. */
  GaussLPQuadrature(const FunctionCollection & functions,
                    const Distribution & measure,
                    const Sample & anchors);

  GaussLPQuadrature * clone() const override;

  /** Build a rule integrating the first n functions exactly. */
  Sample build(const UnsignedInteger n,
               Point & weightsOut) const;

  /** Build the rule and return it as an immutable weighted experiment. */
  GaussLPQuadratureRule buildRule(const UnsignedInteger n) const;

  /** Anchor nodes retained by the builder. */
  void setAnchors(const Sample & anchors);
  Sample getAnchors() const;

  /** Functions and measure used by the builder. The current anchors are
      preserved and revalidated against the new measure. */
  void setMeasureAndFunctions(const Distribution & measure,
                              const FunctionCollection & functions);
  FunctionCollection getFunctionsCollection() const;

  /** Number of functions available to the builder */
  UnsignedInteger getSize() const;

  /** Measure against which the moments are computed */
  Distribution getMeasure() const;

  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  void save(Advocate & adv) const override;
  void load(Advocate & adv) override;

private:

  FunctionPersistentCollection functions_;
  Distribution measure_;
  UnsignedInteger size_ = 0;
  Sample anchors_;

}; /* class GaussLPQuadrature */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GAUSSLPQUADRATURE_HXX */
