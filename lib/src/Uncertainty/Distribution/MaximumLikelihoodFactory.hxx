//                                               -*- C++ -*-
/**
 *  @brief Maximum likelihood estimation
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
#ifndef OPENTURNS_MAXIMUMLIKELIHOODFACTORY_HXX
#define OPENTURNS_MAXIMUMLIKELIHOODFACTORY_HXX

#include "DistributionFactoryImplementation.hxx"
#include "DistributionFactory.hxx"
#include "OptimizationSolver.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MaximumLikelihoodFactory
 */
class OT_API MaximumLikelihoodFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME;
public:
  MaximumLikelihoodFactory();

  MaximumLikelihoodFactory(const DistributionFactory & factory);

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

  void setParallel(const Bool parallel);
protected:
  DistributionFactory factory_;
  OptimizationSolver solver_;
  OptimizationProblem problem_;

  Bool isParallel_;
}; /* class MaximumLikelihoodFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MAXIMUMLIKELIHOODFACTORY_HXX */
