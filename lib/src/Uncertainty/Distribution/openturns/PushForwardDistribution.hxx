//                                               -*- C++ -*-
/**
 *  @brief The PushForwardDistribution distribution
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
#ifndef OPENTURNS_PUSHFORWARDDISTRIBUTION_HXX
#define OPENTURNS_PUSHFORWARDDISTRIBUTION_HXX

#include "openturns/DistributionImplementation.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Function.hxx"
#include "openturns/Interval.hxx"
#include "openturns/Solver.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PushForwardDistribution
 *
 * The push-forward distribution distribution is the distribution of
 * Y = f(X) where X is an absolutely continuous random vector and f is
 * a deterministic function.
 */
class OT_API PushForwardDistribution
  : public DistributionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  PushForwardDistribution();

  /** Parameters constructor */
  PushForwardDistribution(const Function & function,
                          const Distribution & antecedent);

  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const PushForwardDistribution & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  PushForwardDistribution * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const override;
  using DistributionImplementation::setParametersCollection;
  void setParametersCollection(const PointCollection & parametersCollection) override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /* Interface specific to PushForwardDistribution */

  /** Function accessor */
  void setFunction(const Function & function);
  Function getFunction() const;

  /** Antecedent accessor */
  void setAntecedent(const Distribution & antecedent);
  Distribution getAntecedent() const;

  /** Tell if the distribution is continuous */
  Bool isContinuous() const override;

  /** Tell if the distribution is discrete */
  Bool isDiscrete() const override;

  /** Set the solver used to invert the function */
  void setSolver(const Solver & solver);
  Solver getSolver() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Compute the mean of the distribution */
  void computeMean() const override;

  /** Compute the covariance of the distribution */
  void computeCovariance() const override;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

private:

  /** Set the function and antecedent with check */
  void setFunctionAndAntecedent(const Function & function,
                                const Distribution & antecedent);

  /** Build the finite search box in the support of the antecedent used to
      detect the preimages: the range when its bounds are finite, else
      quantile-clipped bounds */
  Interval buildSearchInterval() const;

  /** Find all the preimages of the given point, i.e. all the roots of the
      residual function r(u) = f(u) - point, by a damped Newton solver with
      step control started from a decomposition of the search box: sign
      changes and local minima of the residual in dimension one,
      low-discrepancy multi-start in higher dimensions */
  PointCollection findPreimages(const Function & residualFunction) const;

  /** Solve r(u) = 0 by damped Newton with step control from the given
      starting point; returns the root if converged within tolerance, an
      empty point otherwise */
  Point dampedNewton(const Function & residualFunction,
                     const Point & startingPoint) const;

  /** The function through which the antecedent is pushed */
  Function function_;

  /** The antecedent distribution */
  Distribution antecedent_;

  /** Solver used to invert the function */
  Solver solver_;
}; /* class PushForwardDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PUSHFORWARDDISTRIBUTION_HXX */