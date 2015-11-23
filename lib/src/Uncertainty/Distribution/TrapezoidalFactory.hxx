//                                               -*- C++ -*-
/**
 *  @brief Factory for Trapezoidal distribution
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
#ifndef OPENTURNS_TRAPEZOIDALFACTORY_HXX
#define OPENTURNS_TRAPEZOIDALFACTORY_HXX

#include "OTprivate.hxx"
#include "DistributionImplementationFactory.hxx"
#include "Trapezoidal.hxx"
#include "OptimizationSolver.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class TrapezoidalFactory
 */
class OT_API TrapezoidalFactory
  : public DistributionImplementationFactory
{
  CLASSNAME;
public:

  /** Default constructor */
  TrapezoidalFactory();

  /** Virtual constructor */
  TrapezoidalFactory * clone() const;

  /* Here is the interface that all derived class must implement */
  using DistributionImplementationFactory::build;

  Implementation build( const NumericalSample & sample ) const;
  Implementation build(const NumericalPoint & parameters) const;
  Implementation build() const;
  Trapezoidal buildAsTrapezoidal( const NumericalSample & sample ) const;
  Trapezoidal buildAsTrapezoidal(const NumericalPoint & parameters) const;
  Trapezoidal buildAsTrapezoidal() const;

  /** Optimization solver accessor */
  OptimizationSolver getOptimizationSolver() const;
  void setOptimizationSolver(const OptimizationSolver & solver);

protected:
  /** Likelihood constraint accessor */
  NumericalMathFunction getLogLikelihoodInequalityConstraint() const;

  /** likelihood estimate */
  NumericalPoint computeLogLikelihoodInequalityConstraint( const NumericalPoint & x ) const;

  /** only used to pass data to be used in computeLogLikeliHood */
  mutable NumericalSample sample_;

  /**   OptimizationSolver   */ 
  mutable OptimizationSolver solver_;

}; /* class TrapezoidalFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TRAPEZOIDALFACTORY_HXX */
