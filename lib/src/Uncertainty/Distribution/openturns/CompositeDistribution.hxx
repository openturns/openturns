//                                               -*- C++ -*-
/**
 *  @brief The CompositeDistribution distribution
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
#ifndef OPENTURNS_COMPOSITEDISTRIBUTION_HXX
#define OPENTURNS_COMPOSITEDISTRIBUTION_HXX

#include "openturns/DistributionImplementation.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Function.hxx"
#include "openturns/Solver.hxx"
#include "openturns/GaussKronrod.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CompositeDistribution
 *
 * The CompositeDistribution distribution.
 */
class OT_API CompositeDistribution
  : public DistributionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  CompositeDistribution();

  /** Parameters constructor */
  CompositeDistribution(const Function & function,
                        const Distribution & antecedent);

  /** Parameters constructor */
  CompositeDistribution(const Function & function,
                        const Distribution & antecedent,
                        const Point & bounds,
                        const Point & values);

  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const CompositeDistribution & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  CompositeDistribution * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the distribution */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Get the product minimum volume interval containing a given probability of the distribution */
  Interval computeMinimumVolumeIntervalWithMarginalProbability(const Scalar prob, Scalar & marginalProbOut) const override;

  /** Get the minimum volume level set containing a given probability of the distribution */
  LevelSet computeMinimumVolumeLevelSetWithThreshold(const Scalar prob, Scalar & thresholdOut) const override;

  /** Get the PDF singularities inside of the range - 1D only */
  Point getSingularities() const override;

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const override;
  using DistributionImplementation::setParametersCollection;
  void setParametersCollection(const PointCollection & parametersCollection) override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /* Interface specific to CompositeDistribution */

  /** Function accessor */
  void setFunction(const Function & function);
  Function getFunction() const;

  /** Antecedent accessor */
  void setAntecedent(const Distribution & antecedent);
  Distribution getAntecedent() const;

  /** Tell if the distribution is continuous */
  Bool isContinuous() const override;

  /** Check if the distribution is discrete */
  Bool isDiscrete() const override;

  /** Set the solver used to perform the different computations */
  void setSolver(const Solver & solver);
  Solver getSolver() const;

  /** Compute the shifted moments of the distribution */
  Point computeShiftedMomentContinuous(const UnsignedInteger n, const Point & shift) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

private:

  friend class CompositeDistributionShiftedMomentWrapper;

  // Structure used to compute shifted moments
  struct CompositeDistributionShiftedMomentWrapper
  {
    CompositeDistributionShiftedMomentWrapper(const UnsignedInteger n,
        const Scalar shift,
        const CompositeDistribution * p_distribution):
      n_(n),
      shift_(shift),
      p_distribution_(p_distribution) {};

    Point computeShiftedMomentKernel(const Point & point) const
    {
      const Scalar y = p_distribution_->function_(point)[0];
      const Scalar power = std::pow(y - shift_, static_cast<Scalar>(n_));
      const Scalar pdf = p_distribution_->antecedent_.computePDF(point);
      const Scalar value = power * pdf;
      return Point(1, value);
    };

    const UnsignedInteger n_;
    const Scalar shift_;
    const CompositeDistribution * p_distribution_;
  }; // struct CompositeDistributionShiftedMomentWrapper

  // Structure used to wrap the gradient of the function into a Function
  struct DerivativeWrapper
  {
    const Function & function_;

    DerivativeWrapper(const Function & function)
      : function_(function)
    {}

    Point computeDerivative(const Point & point) const
    {
      Point value(1, function_.gradient(point)(0, 0));
      return value;
    }

  };

  /** update all the derivative attributes */
  void update();

  /** Set the function and antecedent with check */
  void setFunctionAndAntecedent(const Function & function,
                                const Distribution & antecedent);

  /** The main parameter set of the distribution */
  Function function_;
  Distribution antecedent_;

  /** Useful quantities */
  Point bounds_;
  Point values_;
  Point probabilities_;
  Indices increasing_;

  /** Solver used to invert the function and to find the zeros of its derivative */
  Solver solver_;
}; /* class CompositeDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPOSITEDISTRIBUTION_HXX */
