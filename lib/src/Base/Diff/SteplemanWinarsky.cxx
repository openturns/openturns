//                                             -*- C++ -*-
/**
 *  @brief Adaptive step search of Stepleman and Winarsky
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
 *  Reference:
 *  R. S. Stepleman and N. D. Winarsky, Adaptive Numerical Differentiation,
 *  Mathematics of Computation, 33(148), 1257-1264, 1979.
 *  The optimal finite-difference step balances the truncation error,
 *  which decreases with the step, against the round-off error, which
 *  grows as the step decreases. Successive steps h_{i+1} = alpha h_i
 *  with 0 < alpha < 1 are tried and centered quotients D(h) = (f(x+h)-f(x-h))/(2h) are
 *  compared: while truncation dominates the successive differences
 *  shrink geometrically, once round-off dominates they stagnate or grow,
 *  and the previous step is retained as optimal.
 *  Quotients that are not finite (e.g. from overflowing function values)
 *  carry no information and are skipped.
 */

#include "openturns/SteplemanWinarsky.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/AggregatedFunction.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/CenteredFiniteDifferenceGradient.hxx"
#include "openturns/ConstantStep.hxx"
#include <cmath>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SteplemanWinarsky)

static const Factory<SteplemanWinarsky> Factory_SteplemanWinarsky;

/* Default constructor */
SteplemanWinarsky::SteplemanWinarsky()
  : PersistentObject()
  , alpha_(ResourceMap::GetAsScalar("SteplemanWinarsky-DefaultAlpha"))
  , maximumIterationNumber_(ResourceMap::GetAsUnsignedInteger("SteplemanWinarsky-DefaultMaximumIterationNumber"))
{
  // Nothing to do
}

/* Parameters constructor */
SteplemanWinarsky::SteplemanWinarsky(const Function & function,
                                     const Point & x,
                                     const Point & initialStep)
  : PersistentObject()
  , function_(function)
  , x_(x)
  , initialStep_(initialStep)
  , alpha_(ResourceMap::GetAsScalar("SteplemanWinarsky-DefaultAlpha"))
  , maximumIterationNumber_(ResourceMap::GetAsUnsignedInteger("SteplemanWinarsky-DefaultMaximumIterationNumber"))
{
  const UnsignedInteger inputDimension = function_.getInputDimension();
  if (x_.getDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "Invalid point dimension " << x_.getDimension() << ", expected " << inputDimension;
  if (initialStep_.getDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "Invalid initial step dimension " << initialStep_.getDimension() << ", expected " << inputDimension;
  for (UnsignedInteger j = 0; j < inputDimension; ++ j)
    if (!(std::abs(initialStep_[j]) > 0.0))
      throw InvalidArgumentException(HERE) << "Null initial step component " << j;
  if (!(alpha_ > 0.0) || !(alpha_ < 1.0))
    throw InvalidArgumentException(HERE) << "Invalid alpha " << alpha_ << ", expected in ]0, 1[";
  // Validate the ResourceMap entry with the same rule as the setter
  setMaximumIterationNumber(maximumIterationNumber_);
}

/* Virtual constructor */
SteplemanWinarsky * SteplemanWinarsky::clone() const
{
  return new SteplemanWinarsky(*this);
}

/* String converter */
String SteplemanWinarsky::__repr__() const
{
  OSS oss;
  oss << "class=" << SteplemanWinarsky::GetClassName()
       << " function=" << function_.__repr__()
       << " x=" << x_
       << " initialStep=" << initialStep_
       << " alpha=" << alpha_
       << " maximumIterationNumber=" << maximumIterationNumber_
       << " result=" << result_.__repr__();
  return oss;
}

/* Comparison operator */
Bool SteplemanWinarsky::operator ==(const SteplemanWinarsky & other) const
{
  if (this == &other) return true;
  return (function_ == other.function_) && (x_ == other.x_) && (initialStep_ == other.initialStep_) && (alpha_ == other.alpha_) && (maximumIterationNumber_ == other.maximumIterationNumber_);
}

/* Run the adaptive search, populates result_ */
void SteplemanWinarsky::run()
{
  const UnsignedInteger inputDimension = function_.getInputDimension();
  const UnsignedInteger outputDimension = function_.getOutputDimension();
  if (x_.getDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "Invalid point dimension " << x_.getDimension() << ", expected " << inputDimension;
  if (initialStep_.getDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "Invalid initial step dimension " << initialStep_.getDimension() << ", expected " << inputDimension;
  if (outputDimension < 1)
    throw InvalidArgumentException(HERE) << "Invalid output dimension " << outputDimension << ", expected at least 1";
  if (!(alpha_ > 0.0) || !(alpha_ < 1.0))
    throw InvalidArgumentException(HERE) << "Invalid alpha " << alpha_ << ", expected in ]0, 1[";
  if (maximumIterationNumber_ < 1)
    throw InvalidArgumentException(HERE) << "Invalid maximum iterations " << maximumIterationNumber_ << ", expected at least 1";
  for (UnsignedInteger j = 0; j < inputDimension; ++ j)
    if (!(std::abs(initialStep_[j]) > 0.0))
      throw InvalidArgumentException(HERE) << "Null initial step component " << j;

  Sample optimalSteps(0, inputDimension);
  Collection<Function> collection;
  for (UnsignedInteger i = 0; i < outputDimension; ++ i)
  {
    const Function marginalI(outputDimension == 1 ? function_ : function_.getMarginal(i));
    Point optimalStep(inputDimension);
    for (UnsignedInteger j = 0; j < inputDimension; ++ j)
    {
      Scalar hPrevious = std::abs(initialStep_[j]);
      // First centered quotient D(h) with actual denominator to account for round-off in x +/- h
      Point xPlus(x_);
      Point xMinus(x_);
      xPlus[j] += hPrevious;
      xMinus[j] -= hPrevious;
      Scalar denominator = xPlus[j] - xMinus[j];
      if (!(denominator > 0.0)) throw InternalException(HERE) << "Null denominator for component " << j << " with initial step " << hPrevious;
      Point fPlus(marginalI(xPlus));
      Point fMinus(marginalI(xMinus));
      // Scalar division: Point/Scalar division overflows via its reciprocal for tiny denominators
      Scalar dPrevious = (fPlus[0] - fMinus[0]) / denominator;
      Scalar hCurrent = hPrevious * alpha_;
      // Second quotient to initialize the successive differences
      Scalar bestH = hPrevious;
      Scalar bestDelta = SpecFunc::Infinity;
      Bool hasFinite = std::isfinite(dPrevious);
      if (hCurrent >= SpecFunc::MinScalar)
      {
        xPlus = x_;
        xMinus = x_;
        xPlus[j] += hCurrent;
        xMinus[j] -= hCurrent;
        denominator = xPlus[j] - xMinus[j];
        if (denominator > 0.0)
        {
          fPlus = marginalI(xPlus);
          fMinus = marginalI(xMinus);
          const Scalar dCurrent = (fPlus[0] - fMinus[0]) / denominator;
          const Scalar delta = std::abs(dCurrent - dPrevious);
          if (std::isfinite(delta))
          {
            bestDelta = delta;
            bestH = hCurrent;
            hPrevious = hCurrent;
            dPrevious = dCurrent;
            hasFinite = true;
          }
        }
        else
        {
          bestH = hPrevious;
          bestDelta = 0.0;
        }
      }
      else
      {
        bestH = hPrevious;
        bestDelta = 0.0;
      }
      // Coincident initial quotients: the estimate is already stable
      Bool hasConverged = (bestDelta == 0.0);
      for (UnsignedInteger k = 2; k < maximumIterationNumber_; ++ k)
      {
        const Scalar h = hPrevious * alpha_;
        if (!(h >= SpecFunc::MinScalar))
        {
          hasConverged = true;
          break;
        }
        xPlus = x_;
        xMinus = x_;
        xPlus[j] += h;
        xMinus[j] -= h;
        denominator = xPlus[j] - xMinus[j];
        if (!(denominator > 0.0))
        {
          hasConverged = true;
          break;
        }
        fPlus = marginalI(xPlus);
        fMinus = marginalI(xMinus);
        const Scalar dCurrent = (fPlus[0] - fMinus[0]) / denominator;
        if (!std::isfinite(dCurrent))
        {
          // Non-finite quotients carry no information: shrink further
          hPrevious = h;
          continue;
        }
        if (!std::isfinite(dPrevious))
        {
          // No usable previous quotient: restart the differences from here
          dPrevious = dCurrent;
          hPrevious = h;
          continue;
        }
        const Scalar delta = std::abs(dCurrent - dPrevious);
        // Round-off dominates as soon as successive differences grow again
        if (delta > bestDelta)
        {
          hasConverged = true;
          break;
        }
        bestDelta = delta;
        bestH = h;
        hPrevious = h;
        dPrevious = dCurrent;
        hasFinite = true;
        // Successive quotients coincide: the estimate is stable, keep the current step
        if (bestDelta == 0.0)
        {
          hasConverged = true;
          break;
        }
      }
      if (!hasFinite) throw InternalException(HERE) << "SteplemanWinarsky could not compute a finite quotient for component " << j << " of marginal " << i << ": the function values may overflow, check the function scaling";
      if (!hasConverged) throw InternalException(HERE) << "SteplemanWinarsky did not converge for component " << j << " of marginal " << i << " within " << maximumIterationNumber_ << " iterations, the successive differences still decrease: consider increasing the maximum number of iterations";
      optimalStep[j] = bestH;
    } // for j

    Function marginalIWithOptimalStep(marginalI);
    marginalIWithOptimalStep.setGradient(CenteredFiniteDifferenceGradient(ConstantStep(optimalStep), marginalI.getEvaluation()));
    collection.add(marginalIWithOptimalStep);
    optimalSteps.add(optimalStep);
  } // for i

  const Function aggregated = (outputDimension == 1) ? collection[0] : AggregatedFunction(collection);
  result_ = SteplemanWinarskyResult(optimalSteps, aggregated);
}

/* Result accessor */
SteplemanWinarskyResult SteplemanWinarsky::getResult() const
{
  return result_;
}

/* Function accessor */
void SteplemanWinarsky::setFunction(const Function & function)
{
  function_ = function;
}

Function SteplemanWinarsky::getFunction() const
{
  return function_;
}

/* Point accessor */
void SteplemanWinarsky::setX(const Point & x)
{
  x_ = x;
}

Point SteplemanWinarsky::getX() const
{
  return x_;
}

/* Initial step accessor */
void SteplemanWinarsky::setInitialStep(const Point & initialStep)
{
  initialStep_ = initialStep;
}

Point SteplemanWinarsky::getInitialStep() const
{
  return initialStep_;
}

/* Alpha accessor */
void SteplemanWinarsky::setAlpha(const Scalar alpha)
{
  if (!(alpha > 0.0) || !(alpha < 1.0))
    throw InvalidArgumentException(HERE) << "Invalid alpha " << alpha << ", expected in ]0, 1[";
  alpha_ = alpha;
}

Scalar SteplemanWinarsky::getAlpha() const
{
  return alpha_;
}

/* Maximum iterations accessor */
void SteplemanWinarsky::setMaximumIterationNumber(const UnsignedInteger maximumIterationNumber)
{
  if (maximumIterationNumber < 1)
    throw InvalidArgumentException(HERE) << "Invalid maximum iterations " << maximumIterationNumber << ", expected at least 1";
  maximumIterationNumber_ = maximumIterationNumber;
}

UnsignedInteger SteplemanWinarsky::getMaximumIterationNumber() const
{
  return maximumIterationNumber_;
}

/* Method save() stores the object through the StorageManager */
void SteplemanWinarsky::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("function_", function_);
  adv.saveAttribute("x_", x_);
  adv.saveAttribute("initialStep_", initialStep_);
  adv.saveAttribute("alpha_", alpha_);
  adv.saveAttribute("maximumIterationNumber_", maximumIterationNumber_);
  adv.saveAttribute("result_", result_);
}

/* Method load() reloads the object from the StorageManager */
void SteplemanWinarsky::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("function_", function_);
  adv.loadAttribute("x_", x_);
  adv.loadAttribute("initialStep_", initialStep_);
  adv.loadAttribute("alpha_", alpha_);
  adv.loadAttribute("maximumIterationNumber_", maximumIterationNumber_);
  adv.loadAttribute("result_", result_);
}

END_NAMESPACE_OPENTURNS
