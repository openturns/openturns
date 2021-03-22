//                                               -*- C++ -*-
/**
 *  @brief The CompositeDistribution distribution
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cstdlib>
#include <cmath>

#include "openturns/CompositeDistribution.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/Brent.hxx"
#include "openturns/SobolSequence.hxx"
#include "openturns/SymbolicFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CompositeDistribution)

static const Factory<CompositeDistribution> Factory_CompositeDistribution;

/* Default constructor */
CompositeDistribution::CompositeDistribution()
  : DistributionImplementation()
  , function_(SymbolicFunction("x", "x"))
  , antecedent_(Uniform(0.0, 1.0))
  , bounds_(0)
  , values_(0)
  , probabilities_(0)
  , increasing_(0)
  , solver_(Brent(ResourceMap::GetAsScalar("CompositeDistribution-SolverEpsilon"), ResourceMap::GetAsScalar("CompositeDistribution-SolverEpsilon"), ResourceMap::GetAsScalar("CompositeDistribution-SolverEpsilon")))
{
  // We don't know if the function is thread-safe and it could be called in parallel in computePDF()
  setParallel(false);
  setName("CompositeDistribution");
  setDimension(1);
  // Update the derivative attributes. It also recompute the range
  update();
}

/* Parameters constructor */
CompositeDistribution::CompositeDistribution(const Function & function,
    const Distribution & antecedent)
  : DistributionImplementation()
  , function_(function)
  , antecedent_(antecedent)
  , bounds_(0)
  , values_(0)
  , probabilities_(0)
  , increasing_(0)
  , solver_(Brent(ResourceMap::GetAsScalar("CompositeDistribution-SolverEpsilon") * (antecedent.getRange().getUpperBound()[0] - antecedent.getRange().getLowerBound()[0]), ResourceMap::GetAsScalar("CompositeDistribution-SolverEpsilon"), ResourceMap::GetAsScalar("CompositeDistribution-SolverEpsilon")))
{
  // We don't know if the function is thread-safe and it could be called in parallel in computePDF()
  setParallel(false);
  setName("CompositeDistribution");
  // This method check everything and call the update() method.
  setFunctionAndAntecedent(function, antecedent);
}

/* Parameters constructor */
CompositeDistribution::CompositeDistribution(const Function & function,
    const Distribution & antecedent,
    const Point & bounds,
    const Point & values)
  : DistributionImplementation()
  , function_(function)
  , antecedent_(antecedent)
  , bounds_(bounds)
  , values_(values)
  , probabilities_(0)
  , increasing_(0)
  , solver_(Brent(ResourceMap::GetAsScalar("CompositeDistribution-SolverEpsilon") * (antecedent.getRange().getUpperBound()[0] - antecedent.getRange().getLowerBound()[0]), ResourceMap::GetAsScalar("CompositeDistribution-SolverEpsilon"), ResourceMap::GetAsScalar("CompositeDistribution-SolverEpsilon")))
{
  // We don't know if the function is thread-safe and it could be called in parallel in computePDF()
  setParallel(false);
  setName("CompositeDistribution");
  if (function.getInputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the function must have an input dimension equal to 1, here input dimension=" << function.getInputDimension();
  if (function.getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the function must have an output dimension equal to 1, here input dimension=" << function.getOutputDimension();
  if (antecedent.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the antecedent must have dimension 1. Here dimension=" << antecedent.getDimension();
  const UnsignedInteger size = bounds.getSize();
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: there must be at least two bounds.";
  if (size != values.getSize()) throw InvalidArgumentException(HERE) << "Error: the size of the bounds=" << bounds.getSize() << " is not equal to the size of the values=" << values.getSize();
  probabilities_ = Point(size, 0.0);
  // Compute the probabilities
  // We start at i=1 because the first bound is the lower bound of the range, where CDF == 0.0
  for (UnsignedInteger i = 1; i < size - 1; ++i) probabilities_[i] = antecedent.computeCDF(bounds[i]);
  probabilities_[size - 1] = 1.0;
  increasing_ = Indices(size - 1);
  // Compute the variations
  for (UnsignedInteger i = 0; i < size - 1; ++i) increasing_[i] = values_[i + 1] > values[i];
  // Compute the range
  Scalar xMin = values[0];
  Scalar xMax = xMin;
  for (UnsignedInteger i = 1; i < size; ++i)
  {
    xMin = std::min(xMin, values[i]);
    xMax = std::max(xMax, values[i]);
  }
  // Range based on interval arithmetic
  setRange(Interval(xMin, xMax));
}

/* Set the function and antecedent with check */
void CompositeDistribution::setFunctionAndAntecedent(const Function & function,
    const Distribution & antecedent)
{
  if (function.getInputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the function must have an input dimension equal to 1, here input dimension=" << function.getInputDimension();
  if (function.getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the function must have an output dimension equal to 1, here input dimension=" << function.getOutputDimension();
  if (antecedent.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the antecedent must have dimension 1. Here dimension=" << antecedent.getDimension();
  function_ = function;
  antecedent_ = antecedent;
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  update();
}

/* Compute all the derivative attributes */
void CompositeDistribution::update()
{
  // First, compute the roots of the gradient
  const Scalar xMin = antecedent_.getRange().getLowerBound()[0] + ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon");
  const Scalar xMax = antecedent_.getRange().getUpperBound()[0] - ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon");
  bounds_ = Point(1, xMin);
  try
  {
    values_ = function_(Point(1, xMin));
  }
  catch (...)
  {
    throw NotDefinedException(HERE) << "Error: cannot evaluate the function at x=" << xMin;
  }
  if (!SpecFunc::IsNormal(values_[0])) throw NotDefinedException(HERE) << "Error: cannot evaluate the function at x=" << xMin;
  probabilities_ = Point(1, antecedent_.computeCDF(xMin));
  increasing_ = Indices(0);
  Scalar fMin = values_[0];
  Scalar fMax = values_[0];
  const UnsignedInteger n = ResourceMap::GetAsUnsignedInteger("CompositeDistribution-StepNumber");
  const DerivativeWrapper derivativeWrapper(function_);
  const Function derivative(bindMethod<DerivativeWrapper, Point, Point>(derivativeWrapper, &DerivativeWrapper::computeDerivative, 1, 1));
  Scalar a = xMin;
  Scalar fpA = -1.0;
  try
  {
    fpA = derivative(Point(1, a))[0];
  }
  catch (...)
  {
    throw NotDefinedException(HERE) << "Error: cannot evaluate the derivative at x=" << a;
  }
  if (!SpecFunc::IsNormal(fpA)) throw NotDefinedException(HERE) << "Error: cannot evaluate the derivative at x=" << a;
  Scalar b = a;
  Scalar fpB = fpA;
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    a = b;
    fpA = fpB;
    b = ((i + 1) * xMax + (n - i) * xMin) / (n + 1);
    try
    {
      fpB = derivative(Point(1, b))[0];
    }
    catch (...)
    {
      throw NotDefinedException(HERE) << "Error: cannot evaluate the derivative at x=" << b;
    }
    if (!SpecFunc::IsNormal(fpB)) throw NotDefinedException(HERE) << "Error: cannot evaluate the derivative at x=" << b;
    try
    {
      const Scalar root = solver_.solve(derivative, 0.0, a, b, fpA, fpB);
      bounds_.add(root);
      Scalar value = -1.0;
      try
      {
        value = function_(Point(1, root))[0];
      }
      catch (...)
      {
        throw NotDefinedException(HERE) << "Error: cannot evaluate the function at x=" << root;
      }
      if (!SpecFunc::IsNormal(root)) throw NotDefinedException(HERE) << "Error: cannot evaluate the derivative at x=" << root;
      increasing_.add(value > values_[values_.getSize() - 1]);
      values_.add(value);
      probabilities_.add(antecedent_.computeCDF(root));
      fMin = std::min(value, fMin);
      fMax = std::max(value, fMax);
    }
    catch(...)
    {
      // Nothing to do
    }
  }
  bounds_.add(xMax);
  Scalar value = -1.0;
  try
  {
    value = function_(Point(1, xMax))[0];
  }
  catch (...)
  {
    throw NotDefinedException(HERE) << "Error: cannot evaluate the function at x=" << xMax;
  }
  if (!SpecFunc::IsNormal(value)) throw NotDefinedException(HERE) << "Error: cannot evaluate the function at x=" << xMax;
  increasing_.add(value > values_[values_.getSize() - 1]);
  values_.add(value);
  probabilities_.add(Point(1, antecedent_.computeCDF(xMax)));
  fMin = std::min(value, fMin);
  fMax = std::max(value, fMax);
  setRange(Interval(fMin, fMax));
}

/* Function accessors */
void CompositeDistribution::setFunction(const Function & function)
{
  if (!(function == function_)) setFunctionAndAntecedent(function, antecedent_);
}

Function CompositeDistribution::getFunction() const
{
  return function_;
}

/* Antecedent accessors */
void CompositeDistribution::setAntecedent(const Distribution & antecedent)
{
  if (antecedent != antecedent_) setFunctionAndAntecedent(function_, antecedent);
}

Distribution CompositeDistribution::getAntecedent() const
{
  return antecedent_;
}

/* Solver accessors */
void CompositeDistribution::setSolver(const Solver & solver)
{
  if (!(solver == solver_))
  {
    solver_ = solver;
    update();
  }
}

Solver CompositeDistribution::getSolver() const
{
  return solver_;
}

/* Comparison operator */
Bool CompositeDistribution::operator ==(const CompositeDistribution & other) const
{
  if (this == &other) return true;
  return (antecedent_ == other.antecedent_) && (function_ == other.function_);
}

Bool CompositeDistribution::equals(const DistributionImplementation & other) const
{
  const CompositeDistribution* p_other = dynamic_cast<const CompositeDistribution*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String CompositeDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << CompositeDistribution::GetClassName()
      << " name=" << getName()
      << " function=" << function_
      << " antecedent=" << antecedent_
      << " bounds=" << bounds_
      << " values=" << values_
      << " probabilities=" << probabilities_
      << " increasing=" << increasing_;
  return oss;
}

String CompositeDistribution::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "=f(" << antecedent_.__str__() << ") with f=" << function_.getEvaluation().__str__();
  return oss;
}

/* Virtual constructor */
CompositeDistribution * CompositeDistribution::clone() const
{
  return new CompositeDistribution(*this);
}

/* Get one realization of the distribution */
Point CompositeDistribution::getRealization() const
{
  return function_(antecedent_.getRealization());
}

/* Get the PDF of the distribution */
Scalar CompositeDistribution::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  Scalar pdf = 0.0;
  const Scalar x = point[0];
  Scalar a = bounds_[0];
  Scalar fA = values_[0];
  Scalar b = a;
  Scalar fB = fA;
  const UnsignedInteger size = bounds_.getSize();
  for (UnsignedInteger i = 1; i < size; ++i)
  {
    a = b;
    fA = fB;
    b = bounds_[i];
    fB = values_[i];
    const Bool mustSolve = (increasing_[i - 1] && (fA <= x) && (x < fB)) || (!increasing_[i - 1] && (fB <= x) && (x < fA));
    if (mustSolve)
    {
      const Point fInvX(1, solver_.solve(function_, x, a, b, fA, fB));
      const Scalar numerator = antecedent_.computePDF(fInvX);
      if (numerator > 0.0)
      {
        const Matrix gradient(function_.gradient(fInvX));
        if (!(gradient.getNbRows() == 1 && gradient.getNbColumns() == 1)) throw InternalException(HERE) << "Error: the given function has no actual gradient. Consider using finite differences.";
        const Scalar denominator = std::abs(function_.gradient(fInvX)(0, 0));
        if (SpecFunc::IsNormal(denominator)) pdf += numerator / denominator;
        LOGDEBUG(OSS() << "i=" << i << ", a=" << a << ", fA=" << fA << ", x=" << x << ", b=" << b << ", fB=" << fB << ", fInvX=" << fInvX << ", numerator=" << numerator << ", denominator=" << denominator << ", pdf=" << pdf);
      }
    }
  } // i
  LOGDEBUG(OSS() << "pdf=" << pdf);
  return pdf;
}


/* Get the CDF of the distribution */
Scalar CompositeDistribution::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  Scalar cdf = 0.0;
  const Scalar x = point[0];
  Scalar a = bounds_[0];
  Scalar fA = values_[0];
  Scalar b = a;
  Scalar fB = fA;
  for (UnsignedInteger i = 1; i < bounds_.getSize(); ++i)
  {
    a = b;
    fA = fB;
    b = bounds_[i];
    fB = values_[i];
    // The contribution of the current segment [a, b] to the probability P(X <= x) where X = f(antecedent) is
    // the probability of X\in f([a,b])\cap X <= x
    // If f is increasing, f([a, b]) = [f(a), f(b)] and the contribution is:
    // 0 if x <= f(a)
    // Fantecedent(b) - Fantecedent(a) if x >= f(b)
    // Fantecedent(t) - Fantecedent(a) if f(a) < x < f(b) where f(t) = x
    if (increasing_[i - 1])
    {
      LOGDEBUG(OSS() << "increasing");
      if (x >= fB)
      {
        cdf += probabilities_[i] - probabilities_[i - 1];
        LOGDEBUG(OSS() << "x >= fB, i=" << i << ", a=" << a << ", fA=" << fA << ", x=" << x << ", b=" << b << ", fB=" << fB << ", cdf=" << cdf);
      }
      else if (x > fA)
      {
        const Point fInvX(1, solver_.solve(function_, x, a, b, fA, fB));
        cdf += antecedent_.computeCDF(fInvX) - probabilities_[i - 1];
        LOGDEBUG(OSS() << "x < fA, i=" << i << ", a=" << a << ", fA=" << fA << ", x=" << x << ", b=" << b << ", fB=" << fB << ", fInvX=" << fInvX << ", cdf=" << cdf);
      }
      LOGDEBUG(OSS() << "x <= fA, no contribution");
    } // increasing
    // If f is decreasing, f([a, b]) = [f(b), f(a)] and the contribution is:
    // 0 if x <= f(b)
    // Fantecedent(b) - Fantecedent(a) if x >= f(a)
    // Fantecedent(b) - Fantecedent(t) if f(b) < x < f(a) where f(t) = x
    else
    {
      LOGDEBUG(OSS() << "decreasing");
      if (x >= fA)
      {
        cdf += probabilities_[i] - probabilities_[i - 1];
        LOGDEBUG(OSS() << "x >= fA, i=" << i << ", a=" << a << ", fA=" << fA << ", x=" << x << ", b=" << b << ", fB=" << fB << ", cdf=" << cdf);
      }
      else if (x > fB)
      {
        const Point fInvX(1, solver_.solve(function_, x, a, b, fA, fB));
        cdf += probabilities_[i] - antecedent_.computeCDF(fInvX);
        LOGDEBUG(OSS() << "x > fB, i=" << i << ", a=" << a << ", fA=" << fA << ", x=" << x << ", b=" << b << ", fB=" << fB << ", fInvX=" << fInvX << ", cdf=" << cdf);
      }
      LOGDEBUG(OSS() << "x <= fB, no contribution");
    } // decreasing
  } // i
  LOGDEBUG(OSS() << "cdf=" << cdf);
  return cdf;
}

/** Get the product minimum volume interval containing a given probability of the distribution */
Interval CompositeDistribution::computeMinimumVolumeIntervalWithMarginalProbability(const Scalar prob, Scalar & marginalProb) const
{
  return DistributionImplementation::computeUnivariateMinimumVolumeIntervalByOptimization(prob, marginalProb);
}

/** Get the minimum volume level set containing a given probability of the distribution */
LevelSet CompositeDistribution::computeMinimumVolumeLevelSetWithThreshold(const Scalar prob, Scalar & threshold) const
{
  Function minimumVolumeLevelSetFunction(MinimumVolumeLevelSetEvaluation(clone()).clone());
  minimumVolumeLevelSetFunction.setGradient(MinimumVolumeLevelSetGradient(clone()).clone());
  // As we are in 1D and as the function defining the composite distribution can have complex variations,
  // we use an improved sampling method to compute the quantile of the -logPDF(X) distribution
  const UnsignedInteger size = SpecFunc::NextPowerOfTwo(ResourceMap::GetAsUnsignedInteger("Distribution-MinimumVolumeLevelSetSamplingSize"));
  const Point q(SobolSequence(1).generate(size).getImplementation()->getData());
  const Sample sampleAntecedent(antecedent_.computeQuantile(q));
  const Sample minusLogPDFSample(computeLogPDF(function_(sampleAntecedent)) * Point(1, -1.0));
  const Scalar minusLogPDFThreshold = minusLogPDFSample.computeQuantile(prob)[0];
  threshold = std::exp(-minusLogPDFThreshold);

  return LevelSet(minimumVolumeLevelSetFunction, LessOrEqual(), minusLogPDFThreshold);
}

/* Get the PDF singularities inside of the range - 1D only */
Point CompositeDistribution::getSingularities() const
{
  if (values_.getSize() == 2) return Point(1, 0);
  // The singularities are at the extrema of f
  Point singularities(values_);
  std::sort(singularities.begin(), singularities.end());
  // Remove the end points
  singularities.erase(0);
  singularities.erase(singularities.getSize() - 1);
  return singularities;
}

/* Parameters value and description accessor */
CompositeDistribution::PointWithDescriptionCollection CompositeDistribution::getParametersCollection() const
{
  return antecedent_.getParametersCollection();
}

void CompositeDistribution::setParametersCollection(const PointCollection & parametersCollection)
{
  antecedent_.setParametersCollection(parametersCollection);
  setFunctionAndAntecedent(function_, antecedent_);
}

/* Parameters value accessor */
Point CompositeDistribution::getParameter() const
{
  return antecedent_.getParameter();
}

void CompositeDistribution::setParameter(const Point & parameter)
{
  antecedent_.setParameter(parameter);
  setFunctionAndAntecedent(function_, antecedent_);
}

/* Parameters description accessor */
Description CompositeDistribution::getParameterDescription() const
{
  return antecedent_.getParameterDescription();
}

/* Tell if the distribution is continuous */
Bool CompositeDistribution::isContinuous() const
{
  return antecedent_.isContinuous();
}

/* Tell if the distribution is integer valued */
Bool CompositeDistribution::isDiscrete() const
{
  return antecedent_.isDiscrete();
}

/* Compute the shifted moments of the distribution */
Point CompositeDistribution::computeShiftedMomentContinuous(const UnsignedInteger n,
    const Point & shift) const
{
  if (shift.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the shift dimension must match the distribution dimension.";
  if (n == 0) return Point(1, 1.0);
  Point moment(1);
  // For each component
  GaussKronrod algo;
  const CompositeDistributionShiftedMomentWrapper kernel(n, shift[0], this);
  const Function integrand(bindMethod<CompositeDistributionShiftedMomentWrapper, Point, Point>(kernel, &CompositeDistributionShiftedMomentWrapper::computeShiftedMomentKernel, 1, 1));
  const Scalar a = antecedent_.getRange().getLowerBound()[0];
  const Scalar b = antecedent_.getRange().getUpperBound()[0];
  moment[0] = algo.integrate(integrand, Interval(a, b))[0];
  return moment;
}

/* Method save() stores the object through the StorageManager */
void CompositeDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "function_", function_ );
  adv.saveAttribute( "antecedent_", antecedent_ );
  adv.saveAttribute( "bounds_", bounds_ );
  adv.saveAttribute( "values_", values_ );
  adv.saveAttribute( "probabilities_", probabilities_ );
  adv.saveAttribute( "increasing_", increasing_ );
  adv.saveAttribute( "solver_", solver_ );
}

/* Method load() reloads the object from the StorageManager */
void CompositeDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "function_", function_ );
  adv.loadAttribute( "antecedent_", antecedent_ );
  adv.loadAttribute( "bounds_", bounds_ );
  adv.loadAttribute( "values_", values_ );
  adv.loadAttribute( "probabilities_", probabilities_ );
  adv.loadAttribute( "solver_", solver_ );
  update();
}


END_NAMESPACE_OPENTURNS
