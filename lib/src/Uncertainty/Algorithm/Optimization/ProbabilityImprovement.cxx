//                                               -*- C++ -*-
/**
 *  @brief ProbabilityImprovement is the probability of improvement criterion
 *  of the EGO algorithm
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
#include "openturns/ProbabilityImprovement.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/GaussianProcessConditionalCovariance.hxx"
#include "openturns/OSS.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ProbabilityImprovement)

static const Factory<ProbabilityImprovement> Factory_ProbabilityImprovement;

/**
 * @class ProbabilityImprovementEvaluation
 *
 * The scalar criterion itself: the probability, under the conditional law of
 * the Gaussian process, that a new evaluation beats the best value observed
 * so far. It is a named class of this translation unit rather than a lambda,
 * for the same reason as ExpectedImprovementEvaluation: the algorithm hands
 * it to its inner solver through a Function, which needs a clonable
 * EvaluationImplementation and a class identity, not a bound callback.
 */
class ProbabilityImprovementEvaluation
  : public EvaluationImplementation
{
public:
  ProbabilityImprovementEvaluation(const Scalar optimalValue,
                                   const GaussianProcessRegressionResult & gprResult,
                                   const Bool isMinimization)
    : EvaluationImplementation()
    , optimalValue_(optimalValue)
    , gprResult_(gprResult)
    , gprCov_(gprResult)
    , isMinimization_(isMinimization)
  {
    // Nothing to do
  }

  ProbabilityImprovementEvaluation * clone() const override
  {
    return new ProbabilityImprovementEvaluation(*this);
  }

  Point operator()(const Point & x) const override
  {
    return Point(1, computeAsScalar(x));
  }

  Scalar computeAsScalar(const Point & x) const
  {
    const Scalar mx = gprCov_.getConditionalMean(x)[0];
    // the improvement margin, positive when the surrogate predicts a better
    // point than the best one observed so far, whatever the direction of the
    // problem: the criterion is always maximized
    const Scalar margin = isMinimization_ ? optimalValue_ - mx : mx - optimalValue_;
    const Scalar sk2 = gprCov_.getConditionalMarginalVariance(x);
    // where the predictive variance vanishes, i.e. at the design points, the
    // probability of beating the best observed value is degenerate: the
    // enrichment adds nothing there, and the criterion is zero as the
    // expected improvement is, so the inner solver stays away from points
    // already evaluated
    if (sk2 <= 0.0) return 0.0;
    const Scalar sk = std::sqrt(sk2);
    if (!std::isfinite(sk))
      return SpecFunc::LowestScalar;
    return DistFunc::pNormal(margin / sk);
  }

  Sample operator()(const Sample & theta) const override
  {
    const UnsignedInteger size = theta.getSize();
    // avoid creating size points
    Point theta_i(theta.getDimension());
    Sample outS(size, 1);
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      for (UnsignedInteger j = 0; j < theta_i.getSize(); ++ j)
        theta_i[j] = theta(i, j);
      outS(i, 0) = computeAsScalar(theta_i);
    }
    return outS;
  }

  UnsignedInteger getInputDimension() const override
  {
    return gprResult_.getMetaModel().getInputDimension();
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

protected:
  Scalar optimalValue_;
  GaussianProcessRegressionResult gprResult_;
  GaussianProcessConditionalCovariance gprCov_;

  // whether the global problem is a minimization (the criterion is always maximized)
  Bool isMinimization_ = true;
};

/* Default constructor */
ProbabilityImprovement::ProbabilityImprovement()
  : EfficientGlobalOptimizationCriterionImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
ProbabilityImprovement * ProbabilityImprovement::clone() const
{
  return new ProbabilityImprovement(*this);
}

/* The probability of improvement, to be maximized by the algorithm */
Function ProbabilityImprovement::buildObjective() const
{
  if (gprResult_.getMetaModel().getInputDimension() == 0)
    throw InvalidArgumentException(HERE) << "ProbabilityImprovement needs a Gaussian process: set it with setGaussianProcessRegression() before building the objective";
  if (inputSample_.getSize() != gprResult_.getInputSample().getSize())
    throw InvalidArgumentException(HERE) << "ProbabilityImprovement design size " << inputSample_.getSize()
                                         << " disagrees with the Gaussian process design size " << gprResult_.getInputSample().getSize();
  // The probability is computed with respect to the best value *observed* on
  // the design, which a noise model does not provide: a noisy variant of the
  // criterion has not been established, so the configuration is refused rather
  // than silently computed against a value that is not the one the definition
  // refers to. The augmented expected improvement, ExpectedImprovement with a
  // noise function, is the established noisy rule.
  if (hasNoiseFunction())
    throw NotYetImplementedException(HERE) << "In ProbabilityImprovement::buildObjective(): the probability of improvement is not defined for a noisy objective, the reference level being the best value observed on the design, which a noise model does not provide";
  LOGINFO(OSS() << "Probability of improvement reference value=" << optimalValue_);
  return Function(new ProbabilityImprovementEvaluation(optimalValue_, gprResult_, minimization_));
}

/* String converter */
String ProbabilityImprovement::__repr__() const
{
  OSS oss;
  oss << "class=" << ProbabilityImprovement::GetClassName()
      << " name=" << getName()
      << " " << EfficientGlobalOptimizationCriterionImplementation::__repr__();
  return oss;
}

END_NAMESPACE_OPENTURNS
