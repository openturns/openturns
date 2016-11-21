//                                               -*- C++ -*-
/**
 *  @brief Maximum likelihood estimation
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
#ifndef OPENTURNS_MAXIMUMLIKELIHOODFACTORY_HXX
#define OPENTURNS_MAXIMUMLIKELIHOODFACTORY_HXX

#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/DistributionFactory.hxx"
#include "openturns/OptimizationSolver.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MaximumLikelihoodFactory
 */
class OT_API MaximumLikelihoodFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME;
public:
  /** Default constructor */
  MaximumLikelihoodFactory();

  /** Parameters constructor */
  MaximumLikelihoodFactory(const Distribution & distribution);

  /** Virtual constructor */
  virtual MaximumLikelihoodFactory * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  using DistributionFactoryImplementation::build;

  /* Here is the interface that all derived class must implement */
  /** Build a distribution based on a sample */
  virtual Implementation build(const NumericalSample & sample) const;

  /** Build a distribution based on a set of parameters */
  virtual NumericalPoint buildParameter(const NumericalSample & sample) const;

  void setOptimizationSolver(const OptimizationSolver & solver);
  OptimizationSolver getOptimizationSolver() const;

  void setOptimizationProblem(const OptimizationProblem & problem);
  OptimizationProblem getOptimizationProblem() const;

protected:
  /* The underlying distribution */
  Distribution distribution_;

  /* Solver & optimization problem for log-likelihood maximization */
  OptimizationSolver solver_;
  OptimizationProblem problem_;

  /* Flag to tell if the log-likelihood can be computed in parallel */
  Bool isParallel_;
}; /* class MaximumLikelihoodFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MAXIMUMLIKELIHOODFACTORY_HXX */
