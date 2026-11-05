//                                               -*- C++ -*-
/**
 *  @brief Gauss-LP quadrature rule
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
#ifndef OPENTURNS_GAUSSLPQUADRATURERULE_HXX
#define OPENTURNS_GAUSSLPQUADRATURERULE_HXX

#include "openturns/WeightedExperimentImplementation.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Point.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GaussLPQuadratureRule
 *
 * Immutable weighted experiment holding the nodes and weights produced by
 * GaussLPQuadrature. Its size is known when it is constructed and
 * generateWithWeights() always returns the same rule. The distribution is the
 * measure against which the rule was built, so that ExperimentIntegration
 * maps its integration interval onto the support of the measure.
 */
class OT_API GaussLPQuadratureRule
  : public WeightedExperimentImplementation
{
  CLASSNAME
public:

  GaussLPQuadratureRule();

  GaussLPQuadratureRule(const Sample & nodes,
                        const Point & weights,
                        const Distribution & measure);

  GaussLPQuadratureRule * clone() const override;

  /** WeightedExperiment interface */
  Sample generateWithWeights(Point & weightsOut) const override;
  Bool hasUniformWeights() const override;
  Bool isRandom() const override;

  /** The rule is immutable: its size and its measure are fixed. */
  void setSize(const UnsignedInteger size) override;
  void setDistribution(const Distribution & distribution) override;

  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  void save(Advocate & adv) const override;
  void load(Advocate & adv) override;

private:

  Sample nodes_;
  Point weights_;

}; /* class GaussLPQuadratureRule */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GAUSSLPQUADRATURERULE_HXX */
