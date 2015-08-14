//                                               -*- C++ -*-
/**
 *  @brief The class building chaos expansions
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_FUNCTIONALCHAOSALGORITHM_HXX
#define OPENTURNS_FUNCTIONALCHAOSALGORITHM_HXX

#include "MetaModelResult.hxx"
#include "MetaModelAlgorithm.hxx"
#include "Indices.hxx"
#include "NumericalPoint.hxx"
#include "NumericalMathFunction.hxx"
#include "Distribution.hxx"
#include "AdaptiveStrategy.hxx"
#include "ProjectionStrategy.hxx"
#include "FunctionalChaosResult.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class FunctionalChaosAlgorithm
 *
 * The class building chaos expansions
 */

class OT_API FunctionalChaosAlgorithm
  : public MetaModelAlgorithm
{
  CLASSNAME;

public:


  /** Constructor */
  FunctionalChaosAlgorithm(const NumericalMathFunction & model,
                           const Distribution & distribution,
                           const AdaptiveStrategy & adaptiveStrategy,
                           const ProjectionStrategy & projectionStrategy);

  /** Constructor */
  FunctionalChaosAlgorithm(const NumericalSample & inputSample,
                           const NumericalSample & outputSample,
                           const Distribution & distribution,
                           const AdaptiveStrategy & adaptiveStrategy,
                           const ProjectionStrategy & projectionStrategy);

  /** Constructor */
  FunctionalChaosAlgorithm(const NumericalSample & inputSample,
                           const NumericalPoint & weights,
                           const NumericalSample & outputSample,
                           const Distribution & distribution,
                           const AdaptiveStrategy & adaptiveStrategy,
                           const ProjectionStrategy & projectionStrategy);

  /** Constructor */
  FunctionalChaosAlgorithm(const NumericalMathFunction & model,
                           const Distribution & distribution,
                           const AdaptiveStrategy & adaptiveStrategy);

  /** Constructor */
  FunctionalChaosAlgorithm(const NumericalSample & inputSample,
                           const NumericalSample & outputSample,
                           const Distribution & distribution,
                           const AdaptiveStrategy & adaptiveStrategy);

  /** Constructor */
  FunctionalChaosAlgorithm(const NumericalSample & inputSample,
                           const NumericalSample & outputSample);

  /** Constructor */
  FunctionalChaosAlgorithm(const NumericalSample & inputSample,
                           const NumericalPoint & weights,
                           const NumericalSample & outputSample,
                           const Distribution & distribution,
                           const AdaptiveStrategy & adaptiveStrategy);

  /** Virtual constructor */
  virtual FunctionalChaosAlgorithm * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Maximum residual accessors */
  void setMaximumResidual(NumericalScalar residual);
  NumericalScalar getMaximumResidual() const;

  /** Projection strategy accessor */
  void setProjectionStrategy(const ProjectionStrategy & projectionStrategy);
  ProjectionStrategy getProjectionStrategy() const;

  /** Computes the functional chaos */
  void run();

  /** Get the functional chaos result */
  FunctionalChaosResult getResult() const;

  /** Sample accessors */
  NumericalSample getInputSample() const;
  NumericalSample getOutputSample() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);


protected:

  friend class Factory<FunctionalChaosAlgorithm>;

  /** Default constructor */
  FunctionalChaosAlgorithm();

private:

  /** Marginal computation */
  void runMarginal(const UnsignedInteger marginalIndex,
                   Indices & indices,
                   NumericalPoint & coefficients,
                   NumericalScalar & residual,
                   NumericalScalar & relativeError);

  /** The isoprobabilistic transformation maps the distribution into the orthogonal measure */
  NumericalMathFunction transformation_;

  /** The inverse isoprobabilistic transformation */
  NumericalMathFunction inverseTransformation_;

  /** The composed model */
  NumericalMathFunction composedModel_;

  /** The adaptive strategy */
  AdaptiveStrategy adaptiveStrategy_;

  /** The projection strategy */
  ProjectionStrategy projectionStrategy_;

  /** Maximum residual */
  NumericalScalar maximumResidual_;

  /** Result of the projection */
  FunctionalChaosResult result_;

} ; /* class FunctionalChaosAlgorithm */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FUNCTIONALCHAOSALGORITHM_HXX */
