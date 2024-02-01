//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all MetropolisHastings
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_METROPOLISHASTINGS_HXX
#define OPENTURNS_METROPOLISHASTINGS_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/MetropolisHastingsImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MetropolisHastings
 *
 * @brief The class describes the concept of a calibration strategy.
 *
 *
 * The implementation defines how the
 * @see MetropolisHastingsImplementation
 */
class OT_API MetropolisHastings
  : public TypedInterfaceObject<MetropolisHastingsImplementation>
{
  CLASSNAME
public:

  /** Default constructor */
  MetropolisHastings();

  /** Constructor from implementation */
  MetropolisHastings(const MetropolisHastingsImplementation & implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  MetropolisHastings(MetropolisHastingsImplementation * p_implementation);
#endif

  /** Get a realization */
  Point getRealization() const;

  /** Target distribution accessor */
  Distribution getTargetDistribution() const;

  /** Target log-pdf accessor */
  Function getTargetLogPDF() const;
  Domain getTargetLogPDFSupport() const;

  /** Initial state accessor */
  Point getInitialState() const;

  /** Marginal indices accessor */
  virtual Indices getMarginalIndices() const;

  /** History accessor */
  void setHistory(const HistoryStrategy & strategy);
  HistoryStrategy getHistory() const;

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /** Likelihood accessor */
  void setLikelihood(const Distribution & conditional,
                     const Sample & observations,
                     const Function & model = Function(),
                     const Sample & covariates = Sample(0, 0));

  /** Conditional accessor */
  Distribution getConditional() const;

  /** Link function accessor */
  Function getLinkFunction() const;

  /** Obervations accessor */
  Sample getObservations() const;

  /** Covariates accessor */
  Sample getCovariates() const;

  /** Compute the log-likelihood */
  Scalar computeLogLikelihood(const Point & state) const;

  /** Compute the log-likelihood */
  Scalar computeLogPosterior(const Point & currentState) const;

  /** Acceptance rate accessor*/
  Scalar getAcceptanceRate() const;

}; /* class MetropolisHastings */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_METROPOLISHASTINGS_HXX */
