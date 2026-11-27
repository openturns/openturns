//                                               -*- C++ -*-
/**
 *  @brief LowerConfidenceBound is the lower confidence bound criterion of the
 *  EGO algorithm
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
#include "openturns/LowerConfidenceBound.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/GaussianProcessConditionalCovariance.hxx"
#include "openturns/OSS.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LowerConfidenceBound)

static const Factory<LowerConfidenceBound> Factory_LowerConfidenceBound;

/**
 * @class LowerConfidenceBoundEvaluation
 *
 * The scalar criterion itself. It is a named class of this translation unit
 * rather than a lambda, for the same reason as ExpectedImprovementEvaluation:
 * the algorithm hands it to its inner solver through a Function, which needs a
 * clonable EvaluationImplementation and a class identity, not a bound callback.
 *
 * The criterion is built from the conditional mean and the conditional
 * standard deviation of the Gaussian process, which the conditional covariance
 * operator provides; the two terms of the bound, the predicted value and the
 * multiple of the predictive deviation, are therefore read from a single
 * conditional covariance object, created once per iteration with the objective
 * rather than once per evaluated point.
 */
class LowerConfidenceBoundEvaluation
  : public EvaluationImplementation
{
public:
  LowerConfidenceBoundEvaluation(const Scalar conservativeFactor,
                                 const GaussianProcessRegressionResult & gprResult,
                                 const Bool isMinimization)
    : EvaluationImplementation()
    , conservativeFactor_(conservativeFactor)
    , gprResult_(gprResult)
    , gprCov_(gprResult)
    , isMinimization_(isMinimization)
  {
    // Nothing to do
  }

  LowerConfidenceBoundEvaluation * clone() const override
  {
    return new LowerConfidenceBoundEvaluation(*this);
  }

  Point operator()(const Point & x) const override
  {
    return Point(1, computeAsScalar(x));
  }

  Scalar computeAsScalar(const Point & x) const
  {
    const Scalar mx = gprCov_.getConditionalMean(x)[0];
    const Scalar sk2 = gprCov_.getConditionalMarginalVariance(x);
    // the conditional variance is a square form, so round-off can make it
    // slightly negative; there the confidence bound is the conditional mean
    // itself, which is the measured value of the objective at a design point
    const Scalar sk = (sk2 > 0.0) ? std::sqrt(sk2) : 0.0;
    if (!std::isfinite(sk))
      return SpecFunc::LowestScalar;
    // the criterion is always maximized: a minimization uses the negated
    // confidence bound, and a maximization its mirror
    return isMinimization_ ? conservativeFactor_ * sk - mx : mx + conservativeFactor_ * sk;
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
  Scalar conservativeFactor_;
  GaussianProcessRegressionResult gprResult_;
  GaussianProcessConditionalCovariance gprCov_;

  // whether the global problem is a minimization (the criterion is always maximized)
  Bool isMinimization_ = true;
};

/* Default constructor */
LowerConfidenceBound::LowerConfidenceBound()
  : EfficientGlobalOptimizationCriterionImplementation()
  , conservativeFactor_(ResourceMap::GetAsScalar("LowerConfidenceBound-DefaultConservativeFactor"))
{
  // Nothing to do
}

/* Constructor with the conservative factor */
LowerConfidenceBound::LowerConfidenceBound(const Scalar conservativeFactor)
  : EfficientGlobalOptimizationCriterionImplementation()
  , conservativeFactor_(conservativeFactor)
{
  if (!std::isfinite(conservativeFactor) || !(conservativeFactor >= 0.0))
    throw InvalidArgumentException(HERE) << "The conservative factor must be finite and nonnegative, here conservativeFactor=" << conservativeFactor;
}

/* Virtual constructor */
LowerConfidenceBound * LowerConfidenceBound::clone() const
{
  return new LowerConfidenceBound(*this);
}

/* Conservative factor accessor */
void LowerConfidenceBound::setConservativeFactor(const Scalar conservativeFactor)
{
  if (!std::isfinite(conservativeFactor) || !(conservativeFactor >= 0.0))
    throw InvalidArgumentException(HERE) << "The conservative factor must be finite and nonnegative, here conservativeFactor=" << conservativeFactor;
  conservativeFactor_ = conservativeFactor;
}

Scalar LowerConfidenceBound::getConservativeFactor() const
{
  return conservativeFactor_;
}

/* The confidence bound to maximize, to be maximized by the algorithm */
Function LowerConfidenceBound::buildObjective() const
{
  if (gprResult_.getMetaModel().getInputDimension() == 0)
    throw InvalidArgumentException(HERE) << "LowerConfidenceBound needs a Gaussian process: set it with setGaussianProcessRegression() before building the objective";
  if (inputSample_.getSize() != gprResult_.getInputSample().getSize())
    throw InvalidArgumentException(HERE) << "LowerConfidenceBound design size " << inputSample_.getSize()
                                         << " disagrees with the Gaussian process design size " << gprResult_.getInputSample().getSize();
  // The bound is defined on the objective values of the design, so a noise
  // model is not needed for the criterion to be computable; whether it is
  // then a bound on the observed values or on the true objective is an
  // unresolved design question, so the configuration is refused rather than
  // computed with a meaning the benchmark itself left open.
  if (hasNoiseFunction())
    throw NotYetImplementedException(HERE) << "In LowerConfidenceBound::buildObjective(): the lower confidence bound is not defined for a noisy objective, it is unclear whether the bound should be on the observed values, where the predictive deviation already contains the noise, or on the true objective, which would need a noise variance discount";
  LOGINFO(OSS() << "Lower confidence bound conservative factor=" << conservativeFactor_);
  return Function(new LowerConfidenceBoundEvaluation(conservativeFactor_, gprResult_, minimization_));
}

/* String converter */
String LowerConfidenceBound::__repr__() const
{
  OSS oss;
  oss << "class=" << LowerConfidenceBound::GetClassName()
      << " name=" << getName()
      << " conservativeFactor=" << conservativeFactor_
      << " " << EfficientGlobalOptimizationCriterionImplementation::__repr__();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void LowerConfidenceBound::save(Advocate & adv) const
{
  EfficientGlobalOptimizationCriterionImplementation::save(adv);
  adv.saveAttribute("conservativeFactor_", conservativeFactor_);
}

/* Method load() reloads the object from the StorageManager */
void LowerConfidenceBound::load(Advocate & adv)
{
  EfficientGlobalOptimizationCriterionImplementation::load(adv);
  adv.loadAttribute("conservativeFactor_", conservativeFactor_);
}

END_NAMESPACE_OPENTURNS
