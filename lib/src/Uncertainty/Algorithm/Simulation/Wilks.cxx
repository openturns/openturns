//                                               -*- C++ -*-
/**
 *  @brief Wilks is a generic view of Wilks methods for computing
 * probabilities and related quantities by sampling and estimation
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cmath>
#include "openturns/Wilks.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Sample.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Brent.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS




/*
 * @class Wilks
 */

/* Constructor */
Wilks::Wilks(const RandomVector & vector)
  : Object()
  , vector_()
{
  // Check if the given vector is 1D as no theory has been made so far (2011) to define a quantile in higher dimension
  if (vector.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given vector must be 1D.";
  vector_ = vector;
}

namespace
{
class WilksEvaluation: public EvaluationImplementation
{
public:
  WilksEvaluation(const Scalar quantileLevel,
                  const UnsignedInteger marginIndex)
    : EvaluationImplementation()
    , quantileLevel_(quantileLevel)
    , marginIndex_(marginIndex)
  {
    // Nothing to do
  }

  WilksEvaluation * clone() const
  {
    return new WilksEvaluation(*this);
  }

  Point operator() (const Point & point) const
  {
    if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension 1, got dimension=" << point.getDimension();
    return Point(1, DistFunc::pBeta(marginIndex_ + 1, point[0] - marginIndex_, 1.0 - quantileLevel_));
  }

  UnsignedInteger getInputDimension() const
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const
  {
    return 1;
  }

private:
  const Scalar quantileLevel_;
  const UnsignedInteger marginIndex_;
}; // class WilksEvaluation
}; // Anonymous namespace

/* Sample size computation */
UnsignedInteger Wilks::ComputeSampleSize(const Scalar quantileLevel,
    const Scalar confidenceLevel,
    const UnsignedInteger marginIndex)
{
  if ((quantileLevel <= 0.0) || (quantileLevel >= 1.0)) throw InvalidArgumentException(HERE) << "Error: quantile level must be in ]0, 1[, here quantile level=" << quantileLevel;
  if ((confidenceLevel <= 0.0) || (confidenceLevel >= 1.0)) throw InvalidArgumentException(HERE) << "Error: confidence level must be in ]0, 1[, here confidence level=" << confidenceLevel;
  // Here we have to find the minimal value of N such that
  // 1-\sum_{i=N-r}^N Binomial(i, N)\alpha^i(1-\alpha)^{N-i}>=\beta
  // where:
  //   \alpha = quantileLevel
  //   \beta  = confidenceLevel
  //   r      = marginIndex
  // It rewrites F_{N,alpha}(N-r-1)>=beta where F_{N,alpha} is the CDF of the
  // Binomial(N, alpha) ditribution.
  // Easy case: marginIndex=0, the quantile bound is given by the largest upper statistics. The equation to solve is N=\min{n|1-\alpha^n>=\beta}
  const Function wilksConstraint(WilksEvaluation(quantileLevel, marginIndex).clone());
  Scalar nApprox = 0.0;
  if (marginIndex == 0)
    nApprox = std::log1p(-confidenceLevel) / std::log(quantileLevel);
  else
  {
    // Search for the minimal sample size
    // First, search for an upper bound
    // Here we use the relation F_{N,alpha}(N - r - 1) = pBeta(N - k, k + 1, 1 - alpha)
    // where k = N - r - 1
    // We compute a reasonable guess for n using a Normal approximation:
    // n*alpha+q_beta*sqrt(n*alpha*(1-alpha))=n-r:
    const Scalar aBeta = DistFunc::qNormal(confidenceLevel);
    UnsignedInteger nMax = static_cast<UnsignedInteger>((marginIndex + 0.5 * (quantileLevel * aBeta * aBeta + std::abs(aBeta) * std::sqrt(quantileLevel * (4.0 * marginIndex + quantileLevel * aBeta * aBeta)))) / (1.0 - quantileLevel));
    // This loop must end as wilksConstraint->1 when n->inf
    while (wilksConstraint(Point(1, nMax))[0] < confidenceLevel)
      // At the beginning of the loop nMax is >= 1 so it increases strictly
      nMax += nMax;
    nApprox = Brent().solve(wilksConstraint, confidenceLevel, marginIndex, nMax);
  } // marginIndex > 0
  // Here, nApprox can be very close to an integer (in the sense of: the value of the constraint evaluated at the nearest integer is very close to confidenceLevel), in which case the correct answer is round(nApprox), or the answer is the next integer value
  const UnsignedInteger nInf = static_cast<UnsignedInteger>(std::round(std::max(1.0 * marginIndex, nApprox)));
  const Scalar constraintInf = wilksConstraint(Point(1, nInf))[0];
  if (std::abs(constraintInf - confidenceLevel) < std::sqrt(SpecFunc::Precision)) return nInf;
  return static_cast<UnsignedInteger>(std::ceil(nApprox));
}

/* Estimate an upper bound of the quantile of the random vector for the given quantile level and confidence level, using the marginIndex upper statistics */
Point Wilks::computeQuantileBound(const Scalar quantileLevel,
                                  const Scalar confidenceLevel,
                                  const UnsignedInteger marginIndex) const
{
  // Compute the needed sample size
  const UnsignedInteger size = ComputeSampleSize(quantileLevel, confidenceLevel, marginIndex);
  // Generate a sorted sample of the needed size
  const Sample sample(vector_.getSample(size).sort(0));
  // The upper bound is the marginIndex upper statistics
  return sample[size - 1 - marginIndex];
}

END_NAMESPACE_OPENTURNS
