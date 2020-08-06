//                                               -*- C++ -*-
/**
 *  @brief The class building chaos expansions
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_FUNCTIONALCHAOSALGORITHM_HXX
#define OPENTURNS_FUNCTIONALCHAOSALGORITHM_HXX

#include "openturns/MetaModelResult.hxx"
#include "openturns/MetaModelAlgorithm.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Point.hxx"
#include "openturns/Function.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/AdaptiveStrategy.hxx"
#include "openturns/ProjectionStrategy.hxx"
#include "openturns/FunctionalChaosResult.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class FunctionalChaosAlgorithm
 *
 * The class building chaos expansions
 */

class OT_API FunctionalChaosAlgorithm
  : public MetaModelAlgorithm
{
  CLASSNAME

public:
  /** Default constructor */
  FunctionalChaosAlgorithm();

  /** Constructor */
  FunctionalChaosAlgorithm(const Function & model,
                           const Distribution & distribution,
                           const AdaptiveStrategy & adaptiveStrategy,
                           const ProjectionStrategy & projectionStrategy);

  /** Constructor */
  FunctionalChaosAlgorithm(const Sample & inputSample,
                           const Sample & outputSample,
                           const Distribution & distribution,
                           const AdaptiveStrategy & adaptiveStrategy,
                           const ProjectionStrategy & projectionStrategy);

  /** Constructor */
  FunctionalChaosAlgorithm(const Sample & inputSample,
                           const Point & weights,
                           const Sample & outputSample,
                           const Distribution & distribution,
                           const AdaptiveStrategy & adaptiveStrategy,
                           const ProjectionStrategy & projectionStrategy);

  /** Constructor */
  FunctionalChaosAlgorithm(const Function & model,
                           const Distribution & distribution,
                           const AdaptiveStrategy & adaptiveStrategy);

  /** Constructor */
  FunctionalChaosAlgorithm(const Sample & inputSample,
                           const Sample & outputSample,
                           const Distribution & distribution,
                           const AdaptiveStrategy & adaptiveStrategy);

  /** Constructor */
  FunctionalChaosAlgorithm(const Sample & inputSample,
                           const Sample & outputSample);

  /** Constructor */
  FunctionalChaosAlgorithm(const Sample & inputSample,
                           const Point & weights,
                           const Sample & outputSample,
                           const Distribution & distribution,
                           const AdaptiveStrategy & adaptiveStrategy);

  /** Virtual constructor */
  FunctionalChaosAlgorithm * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Maximum residual accessors */
  void setMaximumResidual(Scalar residual);
  Scalar getMaximumResidual() const;

  /** Projection strategy accessor */
  void setProjectionStrategy(const ProjectionStrategy & projectionStrategy);
  ProjectionStrategy getProjectionStrategy() const;

  AdaptiveStrategy getAdaptiveStrategy() const;

  /** Computes the functional chaos */
  void run() override;

  /** Get the functional chaos result */
  FunctionalChaosResult getResult() const;

  /** Sample accessors */
  Sample getInputSample() const override;
  Sample getOutputSample() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Recover the distribution */
  static Distribution BuildDistribution(const Sample & inputSample);

private:

  /** Marginal computation */
  void runMarginal(const UnsignedInteger marginalIndex,
                   Indices & indices,
                   Point & coefficients,
                   Scalar & residual,
                   Scalar & relativeError);

  /** The isoprobabilistic transformation maps the distribution into the orthogonal measure */
  Function transformation_;

  /** The inverse isoprobabilistic transformation */
  Function inverseTransformation_;

  /** The composed model */
  Function composedModel_;

  /** The adaptive strategy */
  AdaptiveStrategy adaptiveStrategy_;

  /** The projection strategy */
  ProjectionStrategy projectionStrategy_;

  /** Maximum residual */
  Scalar maximumResidual_;

  /** Result of the projection */
  FunctionalChaosResult result_;

} ; /* class FunctionalChaosAlgorithm */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FUNCTIONALCHAOSALGORITHM_HXX */
