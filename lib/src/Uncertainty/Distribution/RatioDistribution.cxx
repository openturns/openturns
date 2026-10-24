//                                               -*- C++ -*-
/**
 *  @brief The RatioDistribution distribution
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
#include <algorithm>
#include <cmath>

#include "openturns/RatioDistribution.hxx"
#include "openturns/Brent.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Function.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/UniVariateFunction.hxx"
#include "openturns/UniVariateFunctionEvaluation.hxx"
#include "openturns/UniVariateFunctionImplementation.hxx"
#include "openturns/Uniform.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(RatioDistribution)

static const Factory<RatioDistribution> Factory_RatioDistribution;

/* Default constructor */
RatioDistribution::RatioDistribution()
  : DistributionImplementation()
  , p_left_(new Uniform(0.0, 1.0))
  , p_right_(new Uniform(1.0, 2.0))
  , algo_()
{
  setName("RatioDistribution");
  setDimension(1);
  // Adjust the truncation interval and the distribution range
  computeRange();
}

/* Parameters constructor */
RatioDistribution::RatioDistribution(const Distribution & left,
                                     const Distribution & right)
  : DistributionImplementation()
  , p_left_(left.getImplementation())
  , p_right_(right.getImplementation())
  , algo_()
{
  setName("RatioDistribution");
  setDimension(1);
  setLeft(left);
  setRight(right);
  computeRange();
}

/* Comparison operator */
Bool RatioDistribution::operator ==(const RatioDistribution & other) const
{
  if (this == &other) return true;
  return hasEqualBase(other) && (p_left_ == other.getLeft().getImplementation()) && (p_right_ == other.getRight().getImplementation());
}

Bool RatioDistribution::equals(const DistributionImplementation & other) const
{
  const RatioDistribution* p_other = dynamic_cast<const RatioDistribution*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String RatioDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << RatioDistribution::GetClassName()
      << " name=" << getName()
      << " left=" << p_left_->__repr__()
      << " right=" << p_right_->__repr__();
  return oss;
}

String RatioDistribution::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(" << p_left_->__str__() << " / " << p_right_->__str__() << ")";
  return oss;
}

/* Virtual constructor */
RatioDistribution * RatioDistribution::clone() const
{
  return new RatioDistribution(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void RatioDistribution::computeRange()
{
  const Scalar a = p_left_->getRange().getLowerBound()[0];
  const Scalar b = p_left_->getRange().getUpperBound()[0];
  const Scalar c = p_right_->getRange().getLowerBound()[0];
  const Scalar d = p_right_->getRange().getUpperBound()[0];
  // If the denominator support contains zero, the ratio is unbounded.
  // Keep finite numerical bounds with the infinite flags unset so that the
  // generic algorithms can still evaluate the distribution far in the tails.
  if ((c <= 0.0) && (d >= 0.0))
  {
    if ((a == 0.0) && (b == 0.0))
    {
      setRange(Interval(0.0, 0.0));
      return;
    }
    setRange(Interval(Point(1, -SpecFunc::MaxScalar),
                      Point(1, SpecFunc::MaxScalar),
                      Interval::BoolCollection(1, false),
                      Interval::BoolCollection(1, false)));
    return;
  }
  const Scalar ac = a / c;
  const Scalar ad = a / d;
  const Scalar bc = b / c;
  const Scalar bd = b / d;
  setRange(Interval(std::min(std::min(ac, ad), std::min(bc, bd)),
                    std::max(std::max(ac, ad), std::max(bc, bd))));
}

/* Get one realization of the distribution */
Point RatioDistribution::getRealization() const
{
  return Point(1, p_left_->getRealization()[0] / p_right_->getRealization()[0]);
}

namespace
{
// Class used to wrap the kernel of the integral defining the PDF of the ratio
class PDFKernelRatioDistribution: public UniVariateFunctionImplementation
{
public:
  PDFKernelRatioDistribution(const Pointer<DistributionImplementation> & p_left,
                             const Pointer<DistributionImplementation> & p_right,
                             const Scalar z)
    : UniVariateFunctionImplementation()
    , p_left_(p_left)
    , p_right_(p_right)
    , z_(z)
  {
    // Nothing to do
  }

  PDFKernelRatioDistribution * clone() const override
  {
    return new PDFKernelRatioDistribution(*this);
  }

  Scalar operator() (const Scalar u) const override
  {
    const Scalar pdfRight = p_right_->computePDF(u);
    if (pdfRight == 0.0) return 0.0;
    const Scalar absU = std::abs(u);
    if (absU == 0.0) return 0.0;
    const Scalar argument = z_ * u;
    if (!std::isfinite(argument)) return 0.0;
    const Scalar pdfLeft = p_left_->computePDF(argument);
    if (pdfLeft == 0.0) return 0.0;
    return absU * pdfLeft * pdfRight;
  }

private:
  const Pointer<DistributionImplementation> p_left_;
  const Pointer<DistributionImplementation> p_right_;
  const Scalar z_;

}; // class PDFKernelRatioDistribution

// Class used to wrap the kernel of the integral defining the CDF of the ratio
class CDFKernelRatioDistribution: public UniVariateFunctionImplementation
{
public:
  CDFKernelRatioDistribution(const Pointer<DistributionImplementation> & p_left,
                             const Pointer<DistributionImplementation> & p_right,
                             const Scalar z,
                             const Bool negative)
    : UniVariateFunctionImplementation()
    , p_left_(p_left)
    , p_right_(p_right)
    , z_(z)
    , negative_(negative)
  {
    // Nothing to do
  }

  CDFKernelRatioDistribution * clone() const override
  {
    return new CDFKernelRatioDistribution(*this);
  }

  Scalar operator() (const Scalar u) const override
  {
    const Scalar pdfRight = p_right_->computePDF(u);
    if (pdfRight == 0.0) return 0.0;
    const Scalar argument = z_ * u;
    Scalar cdfLeft = 0.0;
    if (negative_)
    {
      // Use the survival function rather than 1 - CDF to avoid cancellation
      // when z < 0 and the argument is large and positive.
      if (std::isfinite(argument))
        cdfLeft = p_left_->computeSurvivalFunction(argument);
      else
        cdfLeft = (argument < 0.0 ? 1.0 : 0.0);
    }
    else
    {
      if (std::isfinite(argument))
        cdfLeft = p_left_->computeCDF(argument);
      else
        cdfLeft = (argument > 0.0 ? 1.0 : 0.0);
    }
    return pdfRight * cdfLeft;
  }

private:
  const Pointer<DistributionImplementation> p_left_;
  const Pointer<DistributionImplementation> p_right_;
  const Scalar z_;
  const Bool negative_;

}; // class CDFKernelRatioDistribution

// Class used to wrap the tail kernel of the ratio CDF.  The change of
// variable v = z * u concentrates the contribution of the denominator
// around zero, which is where the CDF transition is located when |z| is
// large.  The kernel is used both for positive and negative z.
class CDFTailKernelRatioDistribution: public UniVariateFunctionImplementation
{
public:
  CDFTailKernelRatioDistribution(const Pointer<DistributionImplementation> & p_left,
                                 const Pointer<DistributionImplementation> & p_right,
                                 const Scalar z)
    : UniVariateFunctionImplementation()
    , p_left_(p_left)
    , p_right_(p_right)
    , z_(z)
  {
    // Nothing to do
  }

  CDFTailKernelRatioDistribution * clone() const override
  {
    return new CDFTailKernelRatioDistribution(*this);
  }

  Scalar operator() (const Scalar v) const override
  {
    const Scalar u = v / z_;
    const Scalar pdfRight = p_right_->computePDF(u);
    if (pdfRight == 0.0) return 0.0;
    const Scalar cdfLeft = (v < 0.0
                            ? p_left_->computeCDF(v)
                            : p_left_->computeSurvivalFunction(v));
    return pdfRight * cdfLeft;
  }

private:
  const Pointer<DistributionImplementation> p_left_;
  const Pointer<DistributionImplementation> p_right_;
  const Scalar z_;

}; // class CDFTailKernelRatioDistribution

// Class used to evaluate the CDF as a univariate function of the quantile
class CDFRatioDistribution: public UniVariateFunctionImplementation
{
public:
  CDFRatioDistribution(const RatioDistribution * p_distribution,
                       const Bool complementary = false)
    : UniVariateFunctionImplementation()
    , p_distribution_(p_distribution)
    , complementary_(complementary)
  {
    // Nothing to do
  }

  CDFRatioDistribution * clone() const override
  {
    return new CDFRatioDistribution(*this);
  }

  Scalar operator() (const Scalar x) const override
  {
    return complementary_ ? p_distribution_->computeComplementaryCDF(x) : p_distribution_->computeCDF(x);
  }

private:
  const RatioDistribution * p_distribution_;
  const Bool complementary_;

}; // class CDFRatioDistribution

// Class used to wrap the kernel of the integral defining the characteristic function
class CFKernelRatioDistribution: public EvaluationImplementation
{
public:
  CFKernelRatioDistribution(const Pointer<DistributionImplementation> & p_left,
                            const Pointer<DistributionImplementation> & p_right,
                            const Scalar t)
    : EvaluationImplementation()
    , p_left_(p_left)
    , p_right_(p_right)
    , t_(t)
  {
    // Nothing to do
  }

  CFKernelRatioDistribution * clone() const override
  {
    return new CFKernelRatioDistribution(*this);
  }

  Point operator() (const Point & point) const override
  {
    const Scalar u = point[0];
    const Scalar pdf = p_right_->computePDF(u);
    if (pdf == 0.0 || u == 0.0) return Point(2);
    const Complex phi(p_left_->computeCharacteristicFunction(t_ / u));
    Point value(2);
    value[0] = pdf * phi.real();
    value[1] = pdf * phi.imag();
    return value;
  }

  UnsignedInteger getInputDimension() const override
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 2;
  }

private:
  const Pointer<DistributionImplementation> p_left_;
  const Pointer<DistributionImplementation> p_right_;
  const Scalar t_;

}; // class CFKernelRatioDistribution

// Class used to wrap the kernel of the integral defining an inverse moment
class InverseMomentKernelRatioDistribution: public UniVariateFunctionImplementation
{
public:
  InverseMomentKernelRatioDistribution(const Pointer<DistributionImplementation> & p_right,
                                       const UnsignedInteger order)
    : UniVariateFunctionImplementation()
    , p_right_(p_right)
    , order_(order)
  {
    // Nothing to do
  }

  InverseMomentKernelRatioDistribution * clone() const override
  {
    return new InverseMomentKernelRatioDistribution(*this);
  }

  Scalar operator() (const Scalar u) const override
  {
    if (u == 0.0) return 0.0;
    return p_right_->computePDF(u) / std::pow(u, static_cast<Scalar>(order_));
  }

private:
  const Pointer<DistributionImplementation> p_right_;
  const UnsignedInteger order_;

}; // class InverseMomentKernelRatioDistribution
} // anonymous namespace

/* Get the PDF of the distribution: PDF(z) = \int_R |u| PDF_left(z * u) * PDF_right(u) du */
Scalar RatioDistribution::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  return computePDF(point[0]);
}

Scalar RatioDistribution::computePDF(const Scalar z) const
{
  const Scalar lower = getRange().getLowerBound()[0];
  const Scalar upper = getRange().getUpperBound()[0];
  if ((z < lower) || (z > upper)) return 0.0;
  const Scalar c = p_right_->getRange().getLowerBound()[0];
  const Scalar d = p_right_->getRange().getUpperBound()[0];
  Scalar integrationLower = c;
  Scalar integrationUpper = d;
  // Restrict the integral to the values of the denominator for which z * u
  // belongs to the numerical support of the numerator.  This is essential
  // when the integrand is concentrated close to the boundary of the
  // denominator support (e.g. a ratio of uniform distributions close to its
  // upper bound).
  if (z != 0.0)
  {
    const Scalar a = p_left_->getRange().getLowerBound()[0];
    const Scalar b = p_left_->getRange().getUpperBound()[0];
    const Scalar first = (z > 0.0 ? a / z : b / z);
    const Scalar second = (z > 0.0 ? b / z : a / z);
    integrationLower = std::max(c, std::min(first, second));
    integrationUpper = std::min(d, std::max(first, second));
    if (!(integrationLower < integrationUpper)) return 0.0;
  }
  const PDFKernelRatioDistribution kernel(p_left_, p_right_, z);
  Scalar value = 0.0;
  if (integrationLower < 0.0) value += algo_.integrate(kernel, integrationLower, std::min(integrationUpper, 0.0));
  if (integrationUpper > 0.0) value += algo_.integrate(kernel, std::max(integrationLower, 0.0), integrationUpper);
  return value;
}

/* Get the CDF of the distribution */
Scalar RatioDistribution::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  return computeCDF(point[0]);
}

Scalar RatioDistribution::computeCDF(const Scalar z) const
{
  const Scalar lower = getRange().getLowerBound()[0];
  const Scalar upper = getRange().getUpperBound()[0];
  if (z <= lower) return 0.0;
  if (z >= upper) return 1.0;
  const Scalar c = p_right_->getRange().getLowerBound()[0];
  const Scalar d = p_right_->getRange().getUpperBound()[0];
  // When the denominator support contains zero, the CDF transition around
  // u = 0 has a width of order 1 / |z|.  For large |z|, integrating directly
  // in u loses this narrow contribution and the result saturates at 1 or 0.
  // Use the equivalent representation obtained by the change of variable
  // v = z * u in this regime.
  if ((c <= 0.0) && (d >= 0.0) && (std::abs(z) >= 1.0))
    return computeCDFTail(z);
  const CDFKernelRatioDistribution negativeKernel(p_left_, p_right_, z, true);
  const CDFKernelRatioDistribution positiveKernel(p_left_, p_right_, z, false);
  Scalar value = 0.0;
  if (c < 0.0) value += algo_.integrate(negativeKernel, c, std::min(d, 0.0));
  if (d > 0.0) value += algo_.integrate(positiveKernel, std::max(c, 0.0), d);
  return std::max(0.0, std::min(1.0, value));
}

/* Get the complementary CDF of the distribution */
Scalar RatioDistribution::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  return computeComplementaryCDF(point[0]);
}

Scalar RatioDistribution::computeComplementaryCDF(const Scalar z) const
{
  const Scalar lower = getRange().getLowerBound()[0];
  const Scalar upper = getRange().getUpperBound()[0];
  if (z <= lower) return 1.0;
  if (z >= upper) return 0.0;
  const Scalar c = p_right_->getRange().getLowerBound()[0];
  const Scalar d = p_right_->getRange().getUpperBound()[0];
  if ((c <= 0.0) && (d >= 0.0))
  {
    if (z >= 1.0) return computeCDFTailComplement(z);
    if (z <= -1.0) return std::max(0.0, std::min(1.0, 1.0 - computeCDFTail(z)));
  }
  return 0.5 + (0.5 - computeCDF(z));
}

/* Compute the tail integral used by both the CDF and the complementary CDF.
   With v = z * u, the CDF complement or the CDF itself is a factor 1 / |z|
   times this integral.  This keeps the transition region around the vanishing
   denominator resolved. */
Scalar RatioDistribution::computeCDFTailIntegral(const Scalar z) const
{
  const Scalar a = p_left_->getRange().getLowerBound()[0];
  const Scalar b = p_left_->getRange().getUpperBound()[0];
  const Scalar c = p_right_->getRange().getLowerBound()[0];
  const Scalar d = p_right_->getRange().getUpperBound()[0];
  const CDFTailKernelRatioDistribution kernel(p_left_, p_right_, z);
  Scalar value = 0.0;
  if (z > 0.0)
  {
    // F_Z(z) = 1 - [int_{z c}^{0} f_Y(v / z) F_X(v) dv
    //               + int_{0}^{z d} f_Y(v / z) (1 - F_X(v)) dv] / z
    if (c < 0.0)
    {
      const Scalar lower = std::max(z * c, a);
      if (lower < 0.0) value += algo_.integrate(kernel, lower, 0.0);
    }
    if (d > 0.0)
    {
      const Scalar upper = std::min(z * d, b);
      if (upper > 0.0) value += algo_.integrate(kernel, 0.0, upper);
    }
  }
  else
  {
    // z < 0:
    // F_Z(z) = [int_{0}^{z c} f_Y(v / z) (1 - F_X(v)) dv
    //           + int_{z d}^{0} f_Y(v / z) F_X(v) dv] / |z|
    if (c < 0.0)
    {
      const Scalar upper = std::min(z * c, b);
      if (upper > 0.0) value += algo_.integrate(kernel, 0.0, upper);
    }
    if (d > 0.0)
    {
      const Scalar lower = std::max(z * d, a);
      if (lower < 0.0) value += algo_.integrate(kernel, lower, 0.0);
    }
  }
  return value;
}

/* Compute the CDF in the far tails when the denominator support contains zero. */
Scalar RatioDistribution::computeCDFTail(const Scalar z) const
{
  const Scalar absZ = std::abs(z);
  const Scalar value = computeCDFTailIntegral(z);
  if (z > 0.0) return std::max(0.0, std::min(1.0, 1.0 - value / absZ));
  return std::max(0.0, std::min(1.0, value / absZ));
}

/* Compute the complementary CDF in the far upper tail.  This form avoids the
   cancellation of 1 - CDF when the requested probability is close to one. */
Scalar RatioDistribution::computeCDFTailComplement(const Scalar z) const
{
  if (z <= 0.0) return 1.0 - computeCDFTail(z);
  return std::max(0.0, std::min(1.0, computeCDFTailIntegral(z) / z));
}

/* Compute the probability content of an interval */
Scalar RatioDistribution::computeProbability(const Interval & interval) const
{
  return computeProbabilityContinuous(interval);
}

/* Get the quantile of the distribution */
Scalar RatioDistribution::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  if (!((prob >= 0.0) && (prob <= 1.0)))
    throw InvalidArgumentException(HERE) << "computeScalarQuantile expected prob to belong to [0,1], but is " << prob;
  const Scalar q = (tail ? 1.0 - prob : prob);
  const Scalar lowerBound = getRange().getLowerBound()[0];
  const Scalar upperBound = getRange().getUpperBound()[0];
  if (q <= 0.0) return lowerBound;
  if (q >= 1.0) return upperBound;
  const Scalar c = p_right_->getRange().getLowerBound()[0];
  const Scalar d = p_right_->getRange().getUpperBound()[0];
  const Scalar cdfZero = computeCDF(0.0);
  // In the upper tail, solving for the complementary CDF avoids the loss of
  // accuracy of 1 - CDF when q is close to one.  This is the range where the
  // ratio CDF has its narrow transition at a vanishing denominator.
  if ((c <= 0.0) && (d >= 0.0) && (q > cdfZero) && (q > 0.5))
  {
    const Scalar targetTail = (tail ? prob : 1.0 - q);
    Scalar lower = 0.0;
    Scalar upper = 1.0;
    Scalar tailLower = computeComplementaryCDF(lower);
    Scalar tailUpper = computeComplementaryCDF(upper);
    UnsignedInteger iteration = 0;
    while ((tailUpper > targetTail) && (iteration < quantileIterations_))
    {
      if (upper > 0.5 * SpecFunc::MaxScalar) break;
      upper *= 2.0;
      tailUpper = computeComplementaryCDF(upper);
      ++iteration;
    }
    // A target tail slightly above the complementary CDF at zero can only be
    // a rounding effect: fall back to the CDF bracket rather than returning
    // the far end of the expansion.
    if (tailLower >= targetTail)
    {
      if (!(tailUpper <= targetTail))
      {
        LOGWARN(OSS() << "RatioDistribution::computeScalarQuantile could not bracket the upper tail probability " << targetTail
                << ", returning the last finite bracket bound=" << upper);
        return upper;
      }
      // Scale the residual tolerance with the tail probability: the default
      // absolute CDF tolerance is far too large when the target tail is tiny.
      const Scalar residualError = std::max(SpecFunc::MinScalar, std::min(cdfEpsilon_, targetTail * quantileEpsilon_));
      const Brent solver(quantileEpsilon_, quantileEpsilon_, residualError, quantileIterations_);
      const UniVariateFunction complementaryCDFFunction(new CDFRatioDistribution(this, true));
      return solver.solve(complementaryCDFFunction, targetTail, lower, upper, tailLower, tailUpper);
    }
  }
  // Bracket the quantile around zero, which is a natural starting point for
  // ratio distributions and avoids the very large initial interval that the
  // generic implementation would use for an unbounded range.  The expansion
  // is capped and checked against overflow so that a saturated CDF cannot
  // lead to an infinite bracket.
  Scalar lower = 0.0;
  Scalar upper = 0.0;
  Scalar cdfLower = cdfZero;
  Scalar cdfUpper = cdfZero;
  Bool hasBracket = true;
  if (q < cdfLower)
  {
    lower = -1.0;
    cdfLower = computeCDF(lower);
    UnsignedInteger iteration = 0;
    while ((cdfLower > q) && (iteration < quantileIterations_))
    {
      if (lower < -0.5 * SpecFunc::MaxScalar) break;
      lower *= 2.0;
      cdfLower = computeCDF(lower);
      ++iteration;
    }
    hasBracket = (cdfLower <= q);
  }
  else if (q > cdfLower)
  {
    upper = 1.0;
    cdfUpper = computeCDF(upper);
    UnsignedInteger iteration = 0;
    while ((cdfUpper < q) && (iteration < quantileIterations_))
    {
      if (upper > 0.5 * SpecFunc::MaxScalar) break;
      upper *= 2.0;
      cdfUpper = computeCDF(upper);
      ++iteration;
    }
    hasBracket = (cdfUpper >= q);
  }
  else return 0.0;
  if (!hasBracket)
  {
    // The requested probability is below or above the numerical resolution
    // of the CDF.  Return the finite end of the explored bracket rather than
    // an infinite value.
    LOGWARN(OSS() << "RatioDistribution::computeScalarQuantile could not bracket the quantile for p=" << q
            << ", returning the last finite bracket bound=" << (q < cdfLower ? lower : upper));
    return (q < cdfLower ? lower : upper);
  }
  // Brent is the standard scalar solver used by the distribution layer.  The
  // bracket found above is finite and the CDF is monotone.  The residual
  // tolerance is scaled with the target probability for small lower tails.
  const Scalar residualError = std::max(SpecFunc::MinScalar, std::min(cdfEpsilon_, q * quantileEpsilon_));
  const Brent solver(quantileEpsilon_, quantileEpsilon_, residualError, quantileIterations_);
  const UniVariateFunction cdfFunction(new CDFRatioDistribution(this));
  return solver.solve(cdfFunction, q, lower, upper, cdfLower, cdfUpper);
}

/* Get the characteristic function of the distribution */
Complex RatioDistribution::computeCharacteristicFunction(const Scalar t) const
{
  if (t == 0.0) return Complex(1.0);
  const Scalar c = p_right_->getRange().getLowerBound()[0];
  const Scalar d = p_right_->getRange().getUpperBound()[0];
  const CFKernelRatioDistribution kernel(p_left_, p_right_, t);
  Scalar negativeError = 0.0;
  Scalar positiveError = 0.0;
  Point value(2);
  if (c < 0.0)
  {
    const Point negativePart(algo_.integrate(kernel, Interval(c, std::min(d, 0.0)), negativeError));
    value[0] += negativePart[0];
    value[1] += negativePart[1];
  }
  if (d > 0.0)
  {
    const Point positivePart(algo_.integrate(kernel, Interval(std::max(c, 0.0), d), positiveError));
    value[0] += positivePart[0];
    value[1] += positivePart[1];
  }
  return Complex(value[0], value[1]);
}

/* Compute the inverse moment of the denominator */
Scalar RatioDistribution::computeInverseMoment(const UnsignedInteger n) const
{
  if (n == 0) return 1.0;
  const Scalar c = p_right_->getRange().getLowerBound()[0];
  const Scalar d = p_right_->getRange().getUpperBound()[0];
  // A support strictly containing zero crosses the pole of 1 / u^n, so the
  // inverse moment is not defined.  A support only touching zero at an
  // endpoint may still give a finite integral, for example when the density
  // vanishes fast enough there.  Integrate it and rely on the Gauss-Kronrod
  // error estimate to detect the divergent cases.
  if ((c < 0.0) && (d > 0.0))
    throw NotDefinedException(HERE) << "Error: the inverse moments of the denominator are not defined when its support contains zero in its interior";
  const InverseMomentKernelRatioDistribution kernel(p_right_, n);
  const UniVariateFunction function(kernel);
  const Function integrand{UniVariateFunctionEvaluation(function)};
  Scalar error = 0.0;
  const Point value(algo_.integrate(integrand, Interval(c, d), error));
  // An endpoint singularity integrable in the mathematical sense can leave a
  // non-negligible quadrature error.  A relative error above 1e-3 indicates a
  // divergence (or a singularity too strong for the adaptive rule), whereas
  // the finite cases encountered in practice stay well below this threshold.
  if (!std::isfinite(value[0]) || !(error <= 1.0e-3 * (1.0 + std::abs(value[0]))))
    throw NotDefinedException(HERE) << "Error: unable to compute the inverse moment of order " << n
                                     << " of the denominator, the integral is likely divergent";
  return value[0];
}

/* Compute the mean of the distribution */
void RatioDistribution::computeMean() const
{
  mean_ = getMoment(1);
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void RatioDistribution::computeCovariance() const
{
  const Scalar mean = getMoment(1)[0];
  const Scalar moment2 = getMoment(2)[0];
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = moment2 - mean * mean;
  isAlreadyComputedCovariance_ = true;
}

/* Get the skewness of the distribution */
Point RatioDistribution::getSkewness() const
{
  const Scalar mean = getMoment(1)[0];
  const Scalar moment2 = getMoment(2)[0];
  const Scalar moment3 = getMoment(3)[0];
  const Scalar variance = moment2 - mean * mean;
  if (!(variance > 0.0)) throw NotDefinedException(HERE) << "Error: the skewness is not defined for a distribution with zero variance";
  const Scalar centralMoment3 = moment3 - 3.0 * mean * moment2 + 2.0 * mean * mean * mean;
  return Point(1, centralMoment3 / std::pow(variance, 1.5));
}

/* Get the kurtosis of the distribution */
Point RatioDistribution::getKurtosis() const
{
  const Scalar mean = getMoment(1)[0];
  const Scalar moment2 = getMoment(2)[0];
  const Scalar moment3 = getMoment(3)[0];
  const Scalar moment4 = getMoment(4)[0];
  const Scalar variance = moment2 - mean * mean;
  if (!(variance > 0.0)) throw NotDefinedException(HERE) << "Error: the kurtosis is not defined for a distribution with zero variance";
  const Scalar mean2 = mean * mean;
  const Scalar centralMoment4 = moment4 - 4.0 * mean * moment3 + 6.0 * mean2 * moment2 - 3.0 * mean2 * mean2;
  return Point(1, centralMoment4 / (variance * variance));
}

/* Get the raw moments of the distribution */
Point RatioDistribution::getMoment(const UnsignedInteger n) const
{
  if (n == 0) return Point(1, 1.0);
  return Point(1, p_left_->getMoment(n)[0] * computeInverseMoment(n));
}

/* Parameters value accessor */
Point RatioDistribution::getParameter() const
{
  Point point(p_left_->getParameter());
  point.add(p_right_->getParameter());
  return point;
}

void RatioDistribution::setParameter(const Point & parameter)
{
  const UnsignedInteger leftSize = p_left_->getParameterDimension();
  const UnsignedInteger rightSize = p_right_->getParameterDimension();
  if (parameter.getSize() != leftSize + rightSize)
    throw InvalidArgumentException(HERE) << "Error: expected " << leftSize + rightSize << " values, got " << parameter.getSize();
  Point newLeftParameters(leftSize);
  Point newRightParameters(rightSize);
  std::copy(parameter.begin(), parameter.begin() + leftSize, newLeftParameters.begin());
  std::copy(parameter.begin() + leftSize, parameter.end(), newRightParameters.begin());
  Distribution newLeft(p_left_);
  Distribution newRight(p_right_);
  newLeft.setParameter(newLeftParameters);
  newRight.setParameter(newRightParameters);
  *this = RatioDistribution(newLeft, newRight);
}

/* Parameters description accessor */
Description RatioDistribution::getParameterDescription() const
{
  Description description(p_left_->getParameterDescription());
  description.add(p_right_->getParameterDescription());
  return description;
}

/* Check if the distribution is elliptical */
Bool RatioDistribution::isElliptical() const
{
  return false;
}

/* Left accessor */
void RatioDistribution::setLeft(const Distribution & left)
{
  if (left.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the numerator must have dimension=1, here dimension=" << left.getDimension();
  if (!left.isContinuous()) throw InvalidArgumentException(HERE) << "Error: the numerator must be a continuous distribution";
  p_left_ = left.getImplementation();
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  isAlreadyCreatedGeneratingFunction_ = false;
  setParallel(p_left_->isParallel() && p_right_->isParallel());
  computeRange();
}

Distribution RatioDistribution::getLeft() const
{
  return p_left_;
}

/* Right accessor */
void RatioDistribution::setRight(const Distribution & right)
{
  if (right.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the denominator must have dimension=1, here dimension=" << right.getDimension();
  if (!right.isContinuous()) throw InvalidArgumentException(HERE) << "Error: the denominator must be a continuous distribution";
  p_right_ = right.getImplementation();
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  isAlreadyCreatedGeneratingFunction_ = false;
  setParallel(p_left_->isParallel() && p_right_->isParallel());
  computeRange();
}

Distribution RatioDistribution::getRight() const
{
  return p_right_;
}

Bool RatioDistribution::isContinuous() const
{
  return p_left_->isContinuous() && p_right_->isContinuous();
}

/* Tell if the distribution is discrete */
Bool RatioDistribution::isDiscrete() const
{
  return false;
}

/* Tell if the distribution is integer valued */
Bool RatioDistribution::isIntegral() const
{
  return false;
}

/* Get the PDF singularities inside of the range - 1D only */
Point RatioDistribution::getSingularities() const
{
  return Point(0);
}

/* Method save() stores the object through the StorageManager */
void RatioDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute("left_", *p_left_);
  adv.saveAttribute("right_", *p_right_);
}

/* Method load() reloads the object from the StorageManager */
void RatioDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  Distribution left;
  adv.loadAttribute("left_", left);
  p_left_ = left.getImplementation();
  Distribution right;
  adv.loadAttribute("right_", right);
  p_right_ = right.getImplementation();
  computeRange();
}

END_NAMESPACE_OPENTURNS
