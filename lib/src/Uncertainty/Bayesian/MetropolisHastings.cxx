//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all MetropolisHastings
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
#include "openturns/MetropolisHastings.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(MetropolisHastings)

/* Default constructor */
MetropolisHastings::MetropolisHastings()
  : TypedInterfaceObject<MetropolisHastingsImplementation>(new MetropolisHastingsImplementation)
{
  // Nothing to do
}

/* Constructor from implementation */
MetropolisHastings::MetropolisHastings(const MetropolisHastingsImplementation & op)
  : TypedInterfaceObject<MetropolisHastingsImplementation>(op.clone())
{
  // Nothing to do
}

/* Constructor from implementation pointer */
MetropolisHastings::MetropolisHastings(MetropolisHastingsImplementation * p_implementation)
  : TypedInterfaceObject<MetropolisHastingsImplementation>(p_implementation)
{
  // Nothing to do
}

/* Get a realization */
Point MetropolisHastings::getRealization() const
{
  return getImplementation()->getRealization();
}

Distribution MetropolisHastings::getTargetDistribution() const
{
  return getImplementation()->getTargetDistribution();
}

Function MetropolisHastings::getTargetLogPDF() const
{
  return getImplementation()->getTargetLogPDF();
}

Domain MetropolisHastings::getTargetLogPDFSupport() const
{
  return getImplementation()->getTargetLogPDFSupport();
}

Point MetropolisHastings::getInitialState() const
{
  return getImplementation()->getInitialState();
}

Indices MetropolisHastings::getMarginalIndices() const
{
  return getImplementation()->getMarginalIndices();
}

UnsignedInteger MetropolisHastings::getDimension() const
{
  return getImplementation()->getDimension();
}

void MetropolisHastings::setHistory(const HistoryStrategy & history)
{
  copyOnWrite();
  getImplementation()->setHistory(history);
}


HistoryStrategy MetropolisHastings::getHistory() const
{
  return getImplementation()->getHistory();
}

/* Likelihood accessor */
void MetropolisHastings::setLikelihood(const Distribution & conditional,
                                       const Sample & observations,
                                       const Function & model,
                                       const Sample & covariates)
{
  copyOnWrite();
  getImplementation()->setLikelihood(conditional, observations, model, covariates);
}


Distribution MetropolisHastings::getConditional() const
{
  return getImplementation()->getConditional();
}


Function MetropolisHastings::getLinkFunction() const
{
  return getImplementation()->getLinkFunction();
}


Sample MetropolisHastings::getObservations() const
{
  return getImplementation()->getObservations();
}


Sample MetropolisHastings::getCovariates() const
{
  return getImplementation()->getCovariates();
}

/* Compute the log-likelihood */
Scalar MetropolisHastings::computeLogLikelihood(const Point & state) const
{
  return getImplementation()->computeLogLikelihood(state);
}

Scalar MetropolisHastings::computeLogPosterior(const Point & state) const
{
  return getImplementation()->computeLogPosterior(state);
}

Scalar MetropolisHastings::getAcceptanceRate() const
{
  return getImplementation()->getAcceptanceRate();
}

END_NAMESPACE_OPENTURNS
