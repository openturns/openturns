//                                               -*- C++ -*-
/**
 *  @brief ExpectedImprovement is the expected improvement criterion of the EGO
 *  algorithm, with its augmented variant for noisy objectives
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
#include "openturns/ExpectedImprovement.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/GaussianProcessConditionalCovariance.hxx"
#include "openturns/OSS.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ExpectedImprovement)

static const Factory<ExpectedImprovement> Factory_ExpectedImprovement;

/**
 * @class ExpectedImprovementEvaluation
 *
 * The scalar criterion itself. It is a named class of this translation unit
 * rather than a lambda: the algorithm hands it to its inner solver through a
 * Function, which needs a clonable EvaluationImplementation and a class
 * identity, not a bound callback.
 */
class ExpectedImprovementEvaluation
  : public EvaluationImplementation
{
public:
  ExpectedImprovementEvaluation(const Scalar optimalValue,
                                const GaussianProcessRegressionResult & gprResult,
                                const Function & noiseFunction,
                                const Bool isMinimization)
    : EvaluationImplementation()
    , optimalValue_(optimalValue)
    , gprResult_(gprResult)
    , noiseFunction_(noiseFunction)
    , gprCov_(gprResult)
    , isMinimization_(isMinimization)
  {
    // Nothing to do
  }

  ExpectedImprovementEvaluation * clone() const override
  {
    return new ExpectedImprovementEvaluation(*this);
  }

  Point operator()(const Point & x) const override
  {
    return Point(1, computeAsScalar(x));
  }

  Scalar computeAsScalar(const Point & x) const
  {
    const Scalar mx = gprCov_.getConditionalMean(x)[0];
    const Scalar fmMk = isMinimization_ ? optimalValue_ - mx : mx - optimalValue_;
    const Scalar sk2 = gprCov_.getConditionalMarginalVariance(x);
    if (sk2 <= 0.0) return 0.0;
    const Scalar sk = std::sqrt(sk2);
    if (!std::isfinite(sk))
      return SpecFunc::LowestScalar;
    const Scalar ratio = fmMk / sk;
    Scalar ei = fmMk * DistFunc::pNormal(ratio) + sk * DistFunc::dNormal(ratio);
    if (noiseFunction_.getOutputDimension() == 1) // if provided
    {
      const Scalar noiseVariance = noiseFunction_(x)[0];
      if (!(noiseVariance >= 0.0)) throw InvalidArgumentException(HERE) << "Noise-induced variance must be nonnegative, but is " << noiseVariance << " at point " << x;
      if (sk2 > 0.0)
        ei *= (1.0 - std::sqrt(noiseVariance) / std::sqrt(noiseVariance + sk2));
      else
        ei = 0.0;
    }
    return ei;
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
  Function noiseFunction_;
  GaussianProcessConditionalCovariance gprCov_;

  // whether the global problem is a minimization (the criterion is always maximized)
  Bool isMinimization_ = true;
};

/* Default constructor */
ExpectedImprovement::ExpectedImprovement()
  : EfficientGlobalOptimizationCriterionImplementation()
  , aeiTradeoff_(ResourceMap::GetAsScalar("EfficientGlobalOptimization-DefaultAEITradeoff"))
{
  // Nothing to do
}

/* Constructor with the AEI tradeoff constant */
ExpectedImprovement::ExpectedImprovement(const Scalar aeiTradeoff)
  : EfficientGlobalOptimizationCriterionImplementation()
  , aeiTradeoff_(aeiTradeoff)
{
  if (!std::isfinite(aeiTradeoff))
    throw InvalidArgumentException(HERE) << "The AEI tradeoff must be finite, here aeiTradeoff=" << aeiTradeoff;
}

/* Virtual constructor */
ExpectedImprovement * ExpectedImprovement::clone() const
{
  return new ExpectedImprovement(*this);
}

/* AEI tradeoff constant accessor */
void ExpectedImprovement::setAEITradeoff(const Scalar aeiTradeoff)
{
  if (!std::isfinite(aeiTradeoff))
    throw InvalidArgumentException(HERE) << "The AEI tradeoff must be finite, here aeiTradeoff=" << aeiTradeoff;
  aeiTradeoff_ = aeiTradeoff;
}

Scalar ExpectedImprovement::getAEITradeoff() const
{
  return aeiTradeoff_;
}

/* Reference level of the improvement, computed on the current state */
Scalar ExpectedImprovement::computeReferenceLevel() const
{
  // Without noise the best observed value is the true reference
  if (!hasNoiseFunction())
    return optimalValue_;

  // With noise the true best value is unknown, so use a quantile of the
  // conditional prediction over the current design, as proposed for the
  // augmented expected improvement. The Gaussian process must be the current
  // one: using an earlier surrogate would compare the new candidate against a
  // reference that the last iterations have already invalidated.
  if (inputSample_.getSize() == 0)
    throw InvalidArgumentException(HERE) << "ExpectedImprovement needs the current design to compute the noisy reference level";
  const GaussianProcessConditionalCovariance gprCov(gprResult_);
  const Sample conditionalMean(gprCov.getConditionalMean(inputSample_));
  Scalar referenceLevel = minimization_ ? SpecFunc::Infinity : SpecFunc::LowestScalar;
  for (UnsignedInteger i = 0; i < inputSample_.getSize(); ++ i)
  {
    const Scalar sk2 = gprCov.getConditionalMarginalVariance(inputSample_[i]);
    if (!(sk2 >= 0.0))
      throw InvalidArgumentException(HERE) << "The conditional marginal variance must be nonnegative at design point " << inputSample_[i] << ", got " << sk2;
    const Scalar quantile = conditionalMean(i, 0) + aeiTradeoff_ * std::sqrt(sk2);
    if ((minimization_ && (quantile < referenceLevel))
        || (!minimization_ && (quantile > referenceLevel)))
      referenceLevel = quantile;
  }
  return referenceLevel;
}

/* The improvement criterion, to be maximized by the algorithm */
Function ExpectedImprovement::buildObjective() const
{
  if (gprResult_.getMetaModel().getInputDimension() == 0)
    throw InvalidArgumentException(HERE) << "ExpectedImprovement needs a Gaussian process: set it with setGaussianProcessRegression() before building the objective";
  if (inputSample_.getSize() != gprResult_.getInputSample().getSize())
    throw InvalidArgumentException(HERE) << "ExpectedImprovement design size " << inputSample_.getSize()
                                         << " disagrees with the Gaussian process design size " << gprResult_.getInputSample().getSize();
  const Scalar referenceLevel = computeReferenceLevel();
  LOGINFO(OSS() << "Expected improvement reference level=" << referenceLevel
          << " noisy=" << (hasNoiseFunction() ? "true" : "false"));
  return Function(new ExpectedImprovementEvaluation(referenceLevel, gprResult_, noiseFunction_, minimization_));
}

/* String converter */
String ExpectedImprovement::__repr__() const
{
  OSS oss;
  oss << "class=" << ExpectedImprovement::GetClassName()
      << " name=" << getName()
      << " aeiTradeoff=" << aeiTradeoff_
      << " " << EfficientGlobalOptimizationCriterionImplementation::__repr__();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void ExpectedImprovement::save(Advocate & adv) const
{
  EfficientGlobalOptimizationCriterionImplementation::save(adv);
  adv.saveAttribute("aeiTradeoff_", aeiTradeoff_);
}

/* Method load() reloads the object from the StorageManager */
void ExpectedImprovement::load(Advocate & adv)
{
  EfficientGlobalOptimizationCriterionImplementation::load(adv);
  adv.loadAttribute("aeiTradeoff_", aeiTradeoff_);
}

END_NAMESPACE_OPENTURNS
