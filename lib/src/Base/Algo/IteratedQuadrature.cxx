//                                               -*- C++ -*-
/**
 *  @brief Implement an iterated quadrature based on a 1D quadrature
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
#include "openturns/IteratedQuadrature.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DatabaseFunction.hxx"
#include "openturns/FunctionImplementation.hxx"
#include "openturns/TBBImplementation.hxx"

#include <algorithm>

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class IteratedQuadrature
 */

CLASSNAMEINIT(IteratedQuadrature)

static const Factory<IteratedQuadrature> Factory_IteratedQuadrature;

/* Constructor without parameters */
IteratedQuadrature::IteratedQuadrature()
  : IntegrationAlgorithmImplementation()
    // It must be initialized here in order to avoid a recursive call to IteratedQuadrature::IteratedQuadrature()
  , algorithm_(GaussKronrod(ResourceMap::GetAsUnsignedInteger("IteratedQuadrature-MaximumSubIntervals"), ResourceMap::GetAsScalar("IteratedQuadrature-MaximumError"), GaussKronrod::GetRuleFromName(ResourceMap::GetAsString("IteratedQuadrature-Rule"))))
{
  // Nothing to do
}

/* Parameters constructor */
IteratedQuadrature::IteratedQuadrature(const IntegrationAlgorithm & algorithm)
  : IntegrationAlgorithmImplementation()
  , algorithm_(algorithm)
{
  // Nothing to do
}

/* Virtual constructor */
IteratedQuadrature * IteratedQuadrature::clone() const
{
  return new IteratedQuadrature(*this);
}


// Class to evaluate the original integrand on the complete points built from the
// fixed coordinates of the already integrated dimensions and the current one.
// It is used only for the innermost integration, in order to avoid nesting the
// integrand in a parametric function at each level of the recursion
class IteratedQuadratureLastFunction : public FunctionImplementation
{
public:
  /* Default constructor */
  IteratedQuadratureLastFunction(const Function & function,
                                 const Point & prefix)
    : FunctionImplementation()
    , function_(function)
    , prefix_(prefix)
  {
    // Nothing to do
  }

  Point operator()(const Point & point) const override
  {
    const UnsignedInteger prefixDimension = prefix_.getDimension();
    Point fullPoint(prefixDimension + 1);
    std::copy(prefix_.begin(), prefix_.end(), fullPoint.begin());
    fullPoint[prefixDimension] = point[0];
    return function_(fullPoint);
  }

  Sample operator()(const Sample & sample) const override
  {
    const UnsignedInteger sampleSize = sample.getSize();
    const UnsignedInteger prefixDimension = prefix_.getDimension();
    Sample fullSample(sampleSize, prefixDimension + 1);
    SampleImplementation & fullSampleImplementation = *fullSample.getImplementation();
    const UnsignedInteger fullDimension = prefixDimension + 1;
    for (UnsignedInteger i = 0; i < sampleSize; ++i)
    {
      std::copy(prefix_.begin(), prefix_.end(), fullSampleImplementation.data_begin() + i * fullDimension);
      fullSample(i, prefixDimension) = sample(i, 0);
    }
    return function_(fullSample);
  }

  // In-place update of the fixed coordinates, to allow the reuse of the
  // integrand over the points of a sample instead of allocating one instance
  // per point
  void setPrefix(const Point & prefix)
  {
    prefix_ = prefix;
  }

  IteratedQuadratureLastFunction * clone() const override
  {
    return new IteratedQuadratureLastFunction(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return function_.getOutputDimension();
  }

  Description getInputDescription() const override
  {
    return Description(1, "t");
  }

  Description getOutputDescription() const override
  {
    return function_.getOutputDescription();
  }

private:
  const Function & function_;
  Point prefix_;
}; // class IteratedQuadratureLastFunction

// Class to compute in a recursive way a multidimensional integral. It carries
// the fixed coordinates of the already integrated dimensions in a prefix point
// and evaluates the integrand on the complete points instead of wrapping it in
// a parametric function at each level, in order to limit the number of
// allocations in the recursion of IteratedQuadrature::integrate
class IteratedQuadraturePartialFunctionWrapper : public FunctionImplementation
{
  friend struct IteratedQuadratureIntegratePolicy;
public:
  /* Default constructor */
  IteratedQuadraturePartialFunctionWrapper(const IteratedQuadrature & quadrature,
      const Function & function,
      const Point & prefix,
      const IteratedQuadrature::FunctionCollection & lowerBounds,
      const IteratedQuadrature::FunctionCollection & upperBounds,
      const UnsignedInteger depth)
    : FunctionImplementation()
    , quadrature_(quadrature)
    , function_(function)
    , prefix_(prefix)
    , lowerBounds_(lowerBounds)
    , upperBounds_(upperBounds)
    , depth_(depth)
  {
    // Check the compatibility of the bound functions with the integrand:
    // this prevents any out-of-bounds access below when check=false was used
    // in the calling integrate method
    if (lowerBounds.getSize() != upperBounds.getSize()) throw InvalidArgumentException(HERE) << "Error: expected the same number of lower and upper bound functions, got " << lowerBounds.getSize() << " lower and " << upperBounds.getSize() << " upper bound functions";
    if (lowerBounds.getSize() + 1 != function.getInputDimension()) throw InvalidArgumentException(HERE) << "Error: expected " << function.getInputDimension() - 1 << " bound functions, got " << lowerBounds.getSize();
  }

  Point operator()(const Point & point) const override
  {
    // Build the full point of the local integration problem
    const UnsignedInteger prefixDimension = prefix_.getDimension();
    Point fullPoint(prefixDimension + 1);
    std::copy(prefix_.begin(), prefix_.end(), fullPoint.begin());
    fullPoint[prefixDimension] = point[0];
    const Scalar a = lowerBounds_[depth_](fullPoint)[0];
    const Scalar b = upperBounds_[depth_](fullPoint)[0];
    // Compute the local integral, the last dimension being integrated directly
    // on the original integrand
    const Bool last = (depth_ == function_.getInputDimension() - 2);
    const Function partialFunction(last ?
        static_cast<FunctionImplementation *>(new IteratedQuadratureLastFunction(function_, fullPoint)) :
        static_cast<FunctionImplementation *>(new IteratedQuadraturePartialFunctionWrapper(quadrature_, function_, fullPoint, lowerBounds_, upperBounds_, depth_ + 1)));
    const Point value(quadrature_.getAlgorithm().integrate(partialFunction, Interval(a, b)));
    for (UnsignedInteger i = 0; i < value.getDimension(); ++i)
      if (!std::isfinite(value[i])) throw InternalException(HERE) << "Error: NaN or Inf produced for x=" << point << " while integrating " << function_;
    return value;
  }

  Sample operator()(const Sample & sample) const override;

  // In-place update of the fixed coordinates, to allow the reuse of the
  // integrand over the points of a sample instead of allocating one instance
  // per point
  void setPrefix(const Point & prefix)
  {
    prefix_ = prefix;
  }

  IteratedQuadraturePartialFunctionWrapper * clone() const override
  {
    return new IteratedQuadraturePartialFunctionWrapper(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return function_.getOutputDimension();
  }

  Description getInputDescription() const override
  {
    return Description(1, "t");
  }

  Description getOutputDescription() const override
  {
    return function_.getOutputDescription();
  }

private:
  const IteratedQuadrature & quadrature_;
  const Function & function_;
  Point prefix_;
  const IteratedQuadrature::FunctionCollection & lowerBounds_;
  const IteratedQuadrature::FunctionCollection & upperBounds_;
  const UnsignedInteger depth_;
}; // class IteratedQuadraturePartialFunctionWrapper


// Body of the parallel evaluation of the local integrals over the sample
// points. Each thread gets its own copy of the body, hence its own integrand
// with a dedicated prefix, so that the prefix-based reuse does not introduce
// any data race
struct IteratedQuadratureIntegratePolicy
{
  const IteratedQuadrature & quadrature_;
  const Function & function_;
  const IteratedQuadrature::FunctionCollection & lowerBounds_;
  const IteratedQuadrature::FunctionCollection & upperBounds_;
  const UnsignedInteger depth_;
  const Bool last_;
  const IntegrationAlgorithm algorithm_;
  const Sample & fullSample_;
  const Sample & sampleA_;
  const Sample & sampleB_;
  const Sample & sample_;
  Sample & result_;
  mutable Function partialFunction_;
  mutable IteratedQuadraturePartialFunctionWrapper * wrapper_;
  mutable IteratedQuadratureLastFunction * lastFunction_;
  // Per-thread workspace of the underlying GaussKronrod algorithm, reused
  // across the local integrals of this thread, and fast path to its
  // allocation-free integrate method (nullptr for the other algorithms)
  mutable Point ai_;
  mutable Point bi_;
  mutable Point ei_;
  mutable Sample fi_;
  mutable Sample x_;
  mutable Sample y_;
  const GaussKronrod * gk_;

  IteratedQuadratureIntegratePolicy(const IteratedQuadraturePartialFunctionWrapper & outer,
                                    const Sample & fullSample,
                                    const Sample & sampleA,
                                    const Sample & sampleB,
                                    const Sample & sample,
                                    Sample & result)
    : quadrature_(outer.quadrature_)
    , function_(outer.function_)
    , lowerBounds_(outer.lowerBounds_)
    , upperBounds_(outer.upperBounds_)
    , depth_(outer.depth_)
    , last_(outer.depth_ == outer.function_.getInputDimension() - 2)
    , algorithm_(outer.quadrature_.getAlgorithm())
    , fullSample_(fullSample)
    , sampleA_(sampleA)
    , sampleB_(sampleB)
    , sample_(sample)
    , result_(result)
    , partialFunction_()
    , wrapper_(nullptr)
    , lastFunction_(nullptr)
    , gk_(dynamic_cast<const GaussKronrod *>(algorithm_.getImplementation().get()))
  {
    createFunction();
  }

  IteratedQuadratureIntegratePolicy(const IteratedQuadratureIntegratePolicy & other)
    : quadrature_(other.quadrature_)
    , function_(other.function_)
    , lowerBounds_(other.lowerBounds_)
    , upperBounds_(other.upperBounds_)
    , depth_(other.depth_)
    , last_(other.last_)
    , algorithm_(other.algorithm_)
    , fullSample_(other.fullSample_)
    , sampleA_(other.sampleA_)
    , sampleB_(other.sampleB_)
    , sample_(other.sample_)
    , result_(other.result_)
    , partialFunction_()
    , wrapper_(nullptr)
    , lastFunction_(nullptr)
    , gk_(dynamic_cast<const GaussKronrod *>(algorithm_.getImplementation().get()))
  {
    createFunction();
  }

  void createFunction() const
  {
    if (last_)
    {
      lastFunction_ = new IteratedQuadratureLastFunction(function_, Point(fullSample_[0]));
      partialFunction_ = Function(lastFunction_);
    }
    else
    {
      wrapper_ = new IteratedQuadraturePartialFunctionWrapper(quadrature_, function_, Point(fullSample_[0]), lowerBounds_, upperBounds_, depth_ + 1);
      partialFunction_ = Function(wrapper_);
    }
  }

  void operator()(const TBBImplementation::BlockedRange<UnsignedInteger> & range) const
  {
    for (UnsignedInteger k = range.begin(); k < range.end(); ++k)
    {
      // The prefix is updated for every point of the range, including the first
      // one, since the initial prefix of the thread-local integrand may not
      // correspond to this thread
      const Point fullPoint(fullSample_[k]);
      if (last_) lastFunction_->setPrefix(fullPoint);
      else wrapper_->setPrefix(fullPoint);
      const Scalar a = sampleA_(k, 0);
      const Scalar b = sampleB_(k, 0);
      if (gk_)
      {
        // Fast path for a GaussKronrod inner algorithm: reuse the per-thread
        // workspace so that the local integrals do not allocate
        Scalar error = 0.0;
        result_[k] = gk_->integrate(partialFunction_, a, b, error, ai_, bi_, fi_, ei_, x_, y_);
      }
      else
      {
        result_[k] = algorithm_.integrate(partialFunction_, Interval(a, b));
      }
      const UnsignedInteger outputDimension = result_[k].getDimension();
      for (UnsignedInteger i = 0; i < outputDimension; ++i)
        if (!std::isfinite(result_(k, i))) throw InternalException(HERE) << "Error: NaN or Inf produced for x=" << sample_[k] << " while integrating " << function_;
    }
  }
}; // struct IteratedQuadratureIntegratePolicy

Sample IteratedQuadraturePartialFunctionWrapper::operator()(const Sample & sample) const
{
  const UnsignedInteger sampleSize = sample.getSize();
  const UnsignedInteger outputDimension = function_.getOutputDimension();
  Sample result(sampleSize, outputDimension);
  if (sampleSize == 0) return result;
  // Build the full sample of the local integration problem in order to
  // evaluate the bound functions with a single batch call
  const UnsignedInteger prefixDimension = prefix_.getDimension();
  Sample fullSample(sampleSize, prefixDimension + 1);
  SampleImplementation & fullSampleImplementation = *fullSample.getImplementation();
  const UnsignedInteger fullDimension = prefixDimension + 1;
  for (UnsignedInteger i = 0; i < sampleSize; ++i)
  {
    std::copy(prefix_.begin(), prefix_.end(), fullSampleImplementation.data_begin() + i * fullDimension);
    fullSample(i, prefixDimension) = sample(i, 0);
  }
  const Sample sampleA(lowerBounds_[depth_](fullSample));
  const Sample sampleB(upperBounds_[depth_](fullSample));
  // Evaluate the local integrals over the sample points, using one integrand
  // per thread in order to keep the prefix-based reuse while being safe to
  // call concurrently when the original integrand allows parallel evaluation
  const IteratedQuadratureIntegratePolicy policy(*this, fullSample, sampleA, sampleB, sample, result);
  TBBImplementation::ParallelForIf(sampleSize > 1 && TBBImplementation::GetThreadsNumber() > 1 && function_.getImplementation()->isParallel(), 0, sampleSize, policy);
  return result;
}


/* Compute an approximation of \int_a^b\int_{L_1(x_1)}^{U_1(x_1)}\int_{L_1(x_1,x_2)}^{U_2(x_1,x_2)}\dots\int_{L_1(x_1,\dots,x_{n-1})}^{U_2(x_1,\dots,x_{n-1})} f(x_1,\dots,x_n)dx_1\dotsdx_n, where [a,b] is an 1D interval, L_k and U_k are functions from R^k into R.
 */
Point IteratedQuadrature::integrate(const Function & function,
                                    const Scalar a,
                                    const Scalar b,
                                    const FunctionCollection & lowerBounds,
                                    const FunctionCollection & upperBounds,
                                    const Bool check) const
{
  const UnsignedInteger inputDimension = function.getInputDimension();
  if (check)
  {
    // Check the compatibility of the lower bound functions and the upper bound functions
    // First, check the number of such functions
    if (lowerBounds.getSize() + 1 != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected " << inputDimension - 1 << " lower bound functions, got " << lowerBounds.getSize();
    if (upperBounds.getSize() + 1 != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected " << inputDimension - 1 << " upper bound functions, got " << upperBounds.getSize();
    // Second, check the input and output dimensions of each bound function
    for (UnsignedInteger i = 1; i < inputDimension; ++i)
    {
      const Function lower(lowerBounds[i - 1]);
      if (lower.getInputDimension() != i) throw InvalidArgumentException(HERE) << "Error: expected a lower bound function of rank=" << i - 1 << " to be of input dimension=" << i << ", got " << lower.getInputDimension();
      if (lower.getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected a lower bound function of rank=" << i - 1 << " to be of output dimension=1, got " << lower.getOutputDimension();
      const Function upper(upperBounds[i - 1]);
      if (upper.getInputDimension() != i) throw InvalidArgumentException(HERE) << "Error: expected an upper bound function of rank=" << i - 1 << " to be of input dimension=" << i << ", got " << upper.getInputDimension();
      if (upper.getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected an upper bound function of rank=" << i - 1 << " to be of output dimension=1, got " << upper.getOutputDimension();
    } // bounds dimensions
  } // check
  if (inputDimension == 1) return algorithm_.integrate(function, Interval(a, b));
  // Prepare the integrand using an IteratedQuadraturePartialFunctionWrapper
  const Function partialFunction(new IteratedQuadraturePartialFunctionWrapper(*this, function, Point(), lowerBounds, upperBounds, 0));
  return algorithm_.integrate(partialFunction, Interval(a, b));
}

Point IteratedQuadrature::integrate(const Function & function,
                                    const Interval & interval) const
{
  const UnsignedInteger inputDimension = function.getInputDimension();
  if (interval.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected an interval of dimension=" << inputDimension << ", got dimension=" << interval.getDimension();
  if (interval.getDimension() == 1) return algorithm_.integrate(function, interval);
  // Build the bound functions associated with the interval
  const Point lower(interval.getLowerBound());
  const Point upper(interval.getUpperBound());
  Scalar a = lower[0];
  Scalar b = upper[0];
  FunctionCollection lowerBounds(inputDimension - 1);
  FunctionCollection upperBounds(inputDimension - 1);
  for (UnsignedInteger i = 1; i < inputDimension; ++i)
  {
    lowerBounds[i - 1] = DatabaseFunction(Sample(1, i), Sample(1, Point(1, lower[i])));
    upperBounds[i - 1] = DatabaseFunction(Sample(1, i), Sample(1, Point(1, upper[i])));
  }
  return integrate(function, a, b, lowerBounds, upperBounds, false);
}

/** Accessors to the underlying integration algorithm */
IntegrationAlgorithm IteratedQuadrature::getAlgorithm() const
{
  return algorithm_;
}

void IteratedQuadrature::setAlgorithm(const IntegrationAlgorithm & algorithm)
{
  algorithm_ = algorithm;
}

/* String converter */
String IteratedQuadrature::__repr__() const
{
  OSS oss(true);
  oss << "class=" << IteratedQuadrature::GetClassName()
      << ", algorithm=" << algorithm_;
  return oss;
}

/* String converter */
String IteratedQuadrature::__str__(const String & ) const
{
  OSS oss(false);
  oss << IteratedQuadrature::GetClassName()
      << "(algorithm=" << algorithm_
      << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void IteratedQuadrature::save(Advocate & adv) const
{
  IntegrationAlgorithmImplementation::save(adv);
  adv.saveAttribute("algorithm_", algorithm_);
}

/* Method load() reloads the object from the StorageManager */
void IteratedQuadrature::load(Advocate & adv)
{
  IntegrationAlgorithmImplementation::load(adv);
  adv.loadAttribute("algorithm_", algorithm_);
}

END_NAMESPACE_OPENTURNS
