//                                               -*- C++ -*-
/**
 *  @brief WatsonBarnesCriterion is the second Watson and Barnes infill
 *  criterion of the EGO algorithm
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
#include "openturns/WatsonBarnesCriterion.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/GaussianProcessConditionalCovariance.hxx"
#include "openturns/OSS.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(WatsonBarnesCriterion)

static const Factory<WatsonBarnesCriterion> Factory_WatsonBarnesCriterion;

/**
 * @class WatsonBarnesEvaluation
 *
 * The scalar criterion itself: the expected improvement over the best value
 * observed so far, summed with the negative conditional mean of the Gaussian
 * process in a minimization (its opposite in a maximization). It is a named
 * class of this translation unit rather than a lambda, for the same reason as
 * ExpectedImprovementEvaluation: the algorithm hands it to its inner solver
 * through a Function, which needs a clonable EvaluationImplementation and a
 * class identity, not a bound callback.
 *
 * The expected improvement closed form below is the same formula as the one of
 * ExpectedImprovement.cxx, without the noise discount of the augmented variant,
 * because the criterion refuses a noise function. It is repeated here rather
 * than composed with the expected improvement objective: composing would
 * evaluate the conditional covariance twice per point of the inner multi-start
 * of the algorithm, and WB2 needs the conditional mean of the same point for
 * its exploitation term anyway, so a single conditional evaluation per point is
 * the cheapest form of the criterion.
 */
class WatsonBarnesEvaluation
  : public EvaluationImplementation
{
public:
  WatsonBarnesEvaluation(const Scalar optimalValue,
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

  WatsonBarnesEvaluation * clone() const override
  {
    return new WatsonBarnesEvaluation(*this);
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
    const Scalar delta = isMinimization_ ? optimalValue_ - mx : mx - optimalValue_;
    const Scalar sk2 = gprCov_.getConditionalMarginalVariance(x);
    // where the predictive variance vanishes, i.e. at the measured points of
    // an interpolating surrogate, the expected improvement is zero as in
    // ExpectedImprovement.cxx, and the criterion is then the exploitation term
    // alone, so a measured point can win the maximization: the difficulty is
    // documented on the class, and deduplicating the enrichment point belongs
    // to the algorithm
    if (sk2 <= 0.0) return isMinimization_ ? -mx : mx;
    const Scalar sk = std::sqrt(sk2);
    if (!std::isfinite(sk))
      return SpecFunc::LowestScalar;
    const Scalar ratio = delta / sk;
    // the expected improvement: the first moment of the positive part of the
    // improvement, cf. Jones et al. (1998) and ExpectedImprovement.cxx
    const Scalar ei = delta * DistFunc::pNormal(ratio) + sk * DistFunc::dNormal(ratio);
    // the exploitation term: the opposite of the conditional mean in a
    // minimization, its mirror in a maximization
    return isMinimization_ ? ei - mx : ei + mx;
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
WatsonBarnesCriterion::WatsonBarnesCriterion()
  : EfficientGlobalOptimizationCriterionImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
WatsonBarnesCriterion * WatsonBarnesCriterion::clone() const
{
  return new WatsonBarnesCriterion(*this);
}

/* The Watson and Barnes criterion, to be maximized by the algorithm */
Function WatsonBarnesCriterion::buildObjective() const
{
  if (gprResult_.getMetaModel().getInputDimension() == 0)
    throw InvalidArgumentException(HERE) << "WatsonBarnesCriterion needs a Gaussian process: set it with setGaussianProcessRegression() before building the objective";
  if (inputSample_.getSize() != gprResult_.getInputSample().getSize())
    throw InvalidArgumentException(HERE) << "WatsonBarnesCriterion design size " << inputSample_.getSize()
                                         << " disagrees with the Gaussian process design size " << gprResult_.getInputSample().getSize();
  // WB2 combines the expected improvement, whose noisy variant is established
  // (the augmented expected improvement), with the conditional mean term,
  // whose noisy counterpart has not been established: a discount of the
  // predictive variance applied to the exploitation term would change the
  // balance of the two terms without a source to justify it. The configuration
  // is therefore refused rather than computed with an invented noisy variant.
  if (hasNoiseFunction())
    throw NotYetImplementedException(HERE) << "In WatsonBarnesCriterion::buildObjective(): the Watson and Barnes criterion is not defined for a noisy objective, the reference level being the best value observed on the design, which a noise model does not provide";
  LOGINFO(OSS() << "Watson and Barnes criterion reference value=" << optimalValue_);
  return Function(new WatsonBarnesEvaluation(optimalValue_, gprResult_, minimization_));
}

/* String converter */
String WatsonBarnesCriterion::__repr__() const
{
  OSS oss;
  oss << "class=" << WatsonBarnesCriterion::GetClassName()
      << " name=" << getName()
      << " " << EfficientGlobalOptimizationCriterionImplementation::__repr__();
  return oss;
}

END_NAMESPACE_OPENTURNS
