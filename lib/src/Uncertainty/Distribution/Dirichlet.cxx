//                                               -*- C++ -*-
/**
 *  @brief The Dirichlet distribution
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
#include <limits>
#include <numeric>
#include "openturns/Indices.hxx"
#include "openturns/Dirichlet.hxx"
#include "openturns/Beta.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/JacobiFactory.hxx"
#include "openturns/LegendreFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/OSS.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Dirichlet)

static const Factory<Dirichlet> Factory_Dirichlet;

namespace
{

/* Clip a value to [0, 1].  SpecFunc::Clip01 already performs the clipping;
   the only addition is the NaN fallback.  A NaN cannot be compared with the
   bounds, and letting it through would propagate into the public CDF, so the
   neutral midpoint of the probability range is returned instead. */
Scalar clipProbability(const Scalar value)
{
  if (!(value == value)) return 0.5;
  return SpecFunc::Clip01(value);
}


/* Exponential of a log-probability.  Unlike std::exp, the result is
   guaranteed to lie in [0, 1]: a non-negative log-probability already means
   a probability of at least one, so it is saturated to 1 instead of
   overflowing above 709; very negative values are flushed to 0 before
   std::exp enters the denormal range.  NaNs are propagated so that the
   caller can apply its fallback. */
Scalar expProbability(const Scalar logValue)
{
  if (!(logValue == logValue)) return logValue;
  if (logValue >= 0.0) return 1.0;
  if (logValue < SpecFunc::LogMinScalar) return 0.0;
  return std::exp(logValue);
}


/* Normalize a positive vector by its largest component and update a log-scale. */
Bool normalizeVector(Point & values, Scalar & logScale)
{
  Scalar maximum = 0.0;
  for (const Scalar value : values) maximum = std::max(maximum, std::abs(value));
  if (!(maximum > 0.0) || !std::isfinite(maximum)) return false;
  for (Scalar & value : values) value /= maximum;
  logScale += std::log(maximum);
  return true;
}


/* Asymptotic normal approximation of the Dirichlet CDF, used when the
   total shape parameter is so large that direct quadrature cannot resolve
   the boundary layer. */
Scalar computeNormalApproximationCDF(const Point & x,
                                     const Point & theta,
                                     const Scalar beta)
{
  const UnsignedInteger n = x.getSize();
  Scalar totalTheta = beta;
  for (const Scalar value : theta) totalTheta += value;
  if (!(totalTheta > 0.0)) return -1.0;

  Point upper(n);
  Point ratio(n);
  CovarianceMatrix correlation(n);
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    ratio[i] = theta[i] / totalTheta;
    const Scalar variance = ratio[i] * (1.0 - ratio[i]) / (totalTheta + 1.0);
    if (!(variance > 0.0)) return -1.0;
    upper[i] = (x[i] - ratio[i]) / std::sqrt(variance);
    if (!(upper[i] == upper[i])) return -1.0;
    /* At eight standard deviations the marginal event is certain to
       double precision, and below minus thirty-eight it is negligible. */
    if (upper[i] > 8.0) upper[i] = 8.0;
    if (upper[i] < -38.0) return 0.0;
  }
  for (UnsignedInteger i = 0; i < n; ++i)
    for (UnsignedInteger j = i + 1; j < n; ++j)
    {
      const Scalar denominator = (1.0 - ratio[i]) * (1.0 - ratio[j]);
      if (!(denominator > 0.0)) return -1.0;
      const Scalar rho = -std::sqrt(ratio[i] * ratio[j] / denominator);
      correlation(i, j) = rho;
    }
  const Point lower(n, SpecFunc::LowestScalar);
  return DistFunc::pNormalND(lower, upper, correlation.computeCholesky());
}


/* Relative tolerance used to recognize an integer shape parameter.  It is
   set to five thousand times SpecFunc::Precision: this leaves room for the
   rounding error of a sum of shape parameters while staying far below any
   non-integer part chosen by the user. */
Bool isIntegerShapeParameter(const Scalar beta, const Scalar roundedBeta)
{
  return std::abs(beta - roundedBeta) <= 5.0e3 * SpecFunc::Precision * std::max(1.0, beta);
}


/* Exact CDF inside the simplex when beta is an integer >= 2.
   With beta = m + 1 and s = 1 - sum(x), expanding
   (s + sum(x_i (1 - t_i)))^m and integrating term by term yields a
   positive finite sum whose terms are convolutions. */
Bool computeIntegerBetaCDF(const Point & x,
                           const Point & theta,
                           const Scalar beta,
                           Scalar & value)
{
  const Scalar roundedBeta = std::round(beta);
  if (!isIntegerShapeParameter(beta, roundedBeta)) return false;
  if (roundedBeta < 2.0) return false;
  const UnsignedInteger m = static_cast<UnsignedInteger>(roundedBeta) - 1;
  if (m > ResourceMap::GetAsUnsignedInteger("Dirichlet-CDFMaximumIntegerBeta")) return false;
  const UnsignedInteger n = x.getSize();
  const Scalar slack = 1.0 - std::accumulate(x.begin(), x.end(), 0.0);
  if (slack < 0.0) return false;

  Point g(m + 1, 0.0);
  Point h(m + 1, 0.0);
  Point next(m + 1, 0.0);
  g[0] = 1.0;
  for (UnsignedInteger k = 1; k <= m; ++k) g[k] = g[k - 1] * slack / k;
  Scalar logScale = 0.0;
  if (!normalizeVector(g, logScale)) return false;

  for (UnsignedInteger i = 0; i < n; ++i)
  {
    h[0] = 1.0;
    for (UnsignedInteger k = 1; k <= m; ++k) h[k] = h[k - 1] * x[i] / (theta[i] + k);
    if (!normalizeVector(h, logScale)) return false;
    std::fill(next.begin(), next.end(), 0.0);
    for (UnsignedInteger q = 0; q <= m; ++q)
    {
      Scalar sum = 0.0;
      for (UnsignedInteger j = 0; j <= q; ++j) sum += g[j] * h[q - j];
      next[q] = sum;
    }
    if (!normalizeVector(next, logScale)) return false;
    std::swap(g, next);
  }
  /* Below this value the normalised convolution carries no significant
     digit and the logarithmic term would be dominated by roundoff. */
  if (!(g[m] > 1.0e-280)) return false;
  const Scalar totalTheta = beta + std::accumulate(theta.begin(), theta.end(), 0.0);
  Scalar logPre = SpecFunc::LogGamma(totalTheta);
  for (UnsignedInteger i = 0; i < n; ++i)
    logPre += theta[i] * std::log(x[i]) - SpecFunc::LogGamma(theta[i] + 1.0);
  const Scalar logValue = logPre + logScale + std::log(g[m]);
  value = expProbability(logValue);
  return true;
}


/* Exton-Lauricella F_A representation of the Dirichlet CDF.  Only valid
   inside the simplex. */
Bool computeExtonCDF(const Point & x,
                     const Point & theta,
                     const Scalar beta,
                     Scalar & value,
                     Scalar & cancellation)
{
  const UnsignedInteger n = x.getSize();
  const Scalar sumX = std::accumulate(x.begin(), x.end(), 0.0);
  if (sumX >= 1.0) return false;
  const Scalar totalTheta = beta + std::accumulate(theta.begin(), theta.end(), 0.0);
  Scalar logPre = SpecFunc::LogGamma(totalTheta) - SpecFunc::LogGamma(beta);
  for (UnsignedInteger i = 0; i < n; ++i)
    logPre += theta[i] * std::log(x[i]) - SpecFunc::LogGamma(theta[i] + 1.0);

  const Scalar a = 1.0 - beta;
  /* For large beta the alternating Lauricella series loses too many digits
     through cancellation, and the box or upper methods are both cheaper and
     more accurate.  The threshold 256 keeps the cancellation bounded. */
  if ((a != 0.0) && (beta > 256.0)) return false;

  Point c(n);
  for (UnsignedInteger i = 0; i < n; ++i) c[i] = theta[i] + 1.0;
  Scalar logFunction = 0.0;
  Scalar sign = 1.0;
  if (!SpecFunc::LogLauricellaF_A(a, theta, c, x, logFunction, cancellation, sign)) return false;

  value = sign * expProbability(logPre + logFunction);
  return true;
}


/* Tensor Gauss-Jacobi quadrature of the box [0,x_1] x ... x [0,x_n]
   when x is inside the simplex. */
Bool computeBoxCDF(const Point & x,
                   const Point & theta,
                   const Scalar beta,
                   const UnsignedInteger order,
                   Scalar & value)
{
  const UnsignedInteger n = x.getSize();
  UnsignedInteger numberOfNodes = 1;
  const UnsignedInteger maxUnsigned = std::numeric_limits<UnsignedInteger>::max();
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    if (numberOfNodes > maxUnsigned / order) return false;
    numberOfNodes *= order;
  }
  /* Bound the memory and the number of terms of the tensor product. */
  if (numberOfNodes > ResourceMap::GetAsUnsignedInteger("Dirichlet-CDFMaximumTensorNodes")) return false;

  Matrix nodes(n, order);
  Matrix weights(n, order);
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    Point marginalWeights;
    const Point marginalNodes(JacobiFactory(theta[i], 1.0).getNodesAndWeights(order, marginalWeights));
    for (UnsignedInteger j = 0; j < order; ++j)
    {
      nodes(i, j) = 0.5 * (marginalNodes[j] + 1.0);
      weights(i, j) = marginalWeights[j];
    }
  }

  const Scalar totalTheta = beta + std::accumulate(theta.begin(), theta.end(), 0.0);
  Scalar logPre = SpecFunc::LogGamma(totalTheta) - SpecFunc::LogGamma(beta);
  for (UnsignedInteger i = 0; i < n; ++i)
    logPre += theta[i] * std::log(x[i]) - SpecFunc::LogGamma(theta[i] + 1.0);

  /* For large shape parameters logPre is large and the weighted sum
     underflows: accumulate it in logarithmic scale in that case.  The
     switch is set at 650, below log(max double) ~ 709, so that the final
     exponentiation cannot overflow, while staying well above the range
     where the direct sum is accurate. */
  const Bool useLog = (logPre > 650.0);
  Matrix logWeights;
  if (useLog)
  {
    logWeights = Matrix(n, order);
    for (UnsignedInteger i = 0; i < n; ++i)
      for (UnsignedInteger j = 0; j < order; ++j)
        logWeights(i, j) = (weights(i, j) > 0.0 ? std::log(weights(i, j)) : -std::numeric_limits<Scalar>::infinity());
  }

  Scalar total = 0.0;
  Scalar maxLogTerm = -std::numeric_limits<Scalar>::infinity();
  Scalar logTotal = 0.0;
  Indices index(n, 0);
  while (true)
  {
    Scalar sum = 0.0;
    Scalar weight = 1.0;
    Scalar logWeight = 0.0;
    for (UnsignedInteger i = 0; i < n; ++i)
    {
      sum += x[i] * nodes(i, index[i]);
      if (useLog) logWeight += logWeights(i, index[i]);
      else weight *= weights(i, index[i]);
    }
    if (sum < 1.0)
    {
      if (useLog)
      {
        const Scalar logIntegrand = (beta == 1.0 ? 0.0 : (beta - 1.0) * std::log1p(-sum));
        const Scalar logTerm = logWeight + logIntegrand;
        if (std::isfinite(logTerm))
        {
          if (logTerm > maxLogTerm)
          {
            logTotal = logTotal * std::exp(maxLogTerm - logTerm) + 1.0;
            maxLogTerm = logTerm;
          }
          else logTotal += std::exp(logTerm - maxLogTerm);
        }
      }
      else if (beta == 1.0) total += weight;
      else total += weight * std::pow(1.0 - sum, beta - 1.0);
    }
    UnsignedInteger position = 0;
    while (position < n)
    {
      ++index[position];
      if (index[position] < order) break;
      index[position] = 0;
      ++position;
    }
    if (position == n) break;
  }

  if (useLog)
  {
    if (!(logTotal > 0.0) || !std::isfinite(maxLogTerm)) value = 0.0;
    else
    {
      const Scalar logValue = logPre + maxLogTerm + std::log(logTotal);
      value = expProbability(logValue);
    }
  }
  else if (!(total > 0.0)) value = 0.0;
  else
  {
    const Scalar logValue = logPre + std::log(total);
    value = expProbability(logValue);
  }
  return true;
}


Bool computeBoxCDFAdaptive(const Point & x,
                           const Point & theta,
                           const Scalar beta,
                           const UnsignedInteger maximumOrder,
                           Scalar & value)
{
  const UnsignedInteger n = x.getSize();
  const Scalar tolerance = ResourceMap::GetAsScalar("Dirichlet-CDFBoxTolerance");
  /* Start from order 8, or 6 from dimension 6 where one tensor-product
     evaluation is already expensive, then refine until two successive
     orders agree or the user maximum order is reached. */
  UnsignedInteger order = std::min(maximumOrder, (n >= 6 ? 6UL : 8UL));
  Scalar previous = 0.0;
  Bool hasPrevious = false;
  while (true)
  {
    Scalar current = 0.0;
    if (!computeBoxCDF(x, theta, beta, order, current)) return false;
    if (!(current == current) || !std::isfinite(current)) return false;
    if (hasPrevious && (std::abs(current - previous) <= tolerance * std::max(current, SpecFunc::MinScalar)))
    {
      value = current;
      return true;
    }
    previous = current;
    hasPrevious = true;
    if (order >= maximumOrder)
    {
      value = previous;
      return false;
    }
    /* Refine by 4 or 8 so that convergence is reached in a few iterations;
       use 2 in dimension 6 and above, where each evaluation is expensive. */
    UnsignedInteger step = (order < 16 ? 4 : 8);
    if (n >= 6) step = 2;
    order = std::min(maximumOrder, order + step);
  }
}


UnsignedInteger upperQuadratureOrder(const UnsignedInteger subsetDimension,
                                     const UnsignedInteger maximumOrder,
                                     const Bool singular)
{
  /* Quadrature orders of an upper corner of dimension k.  They are chosen
     so that the Gauss-Jacobi rule resolves the smooth part of the
     integrand: singular shapes need a higher order because of the
     integrable power behaviour near the corner, while the cost grows like
     order^k.  The result is capped by the user maximum order and floored
     at 3 to keep at least three nodes in the rule. */
  UnsignedInteger order = 12;
  if (singular)
  {
    if (subsetDimension <= 2) order = 40;
    else if (subsetDimension == 3) order = 32;
    else if (subsetDimension == 4) order = 20;
    else if (subsetDimension == 5) order = 14;
    else if (subsetDimension == 6) order = 10;
    else order = 8;
  }
  else
  {
    if (subsetDimension <= 2) order = 16;
    else if (subsetDimension == 3) order = 12;
    else if (subsetDimension == 4) order = 8;
    else if (subsetDimension == 5) order = 5;
    else order = 4;
  }
  return std::max(3UL, std::min(maximumOrder, order));
}


/* Integral of the upper corner {X_i > x_i, i in subset}.  The case of a
   singleton is exact and computed with the beta survival function. */
Bool computeUpperCornerIntegral(const Point & x,
                                const Point & theta,
                                const Scalar beta,
                                const Indices & subset,
                                const UnsignedInteger order,
                                Scalar & value)
{
  const UnsignedInteger k = subset.getSize();
  const Scalar totalTheta = beta + std::accumulate(theta.begin(), theta.end(), 0.0);
  if (k == 1)
  {
    const UnsignedInteger i = subset[0];
    value = DistFunc::pBeta(theta[i], totalTheta - theta[i], x[i], true);
    return true;
  }
  Scalar subsetTheta = 0.0;
  Scalar subsetX = 0.0;
  for (const UnsignedInteger i : subset)
  {
    subsetTheta += theta[i];
    subsetX += x[i];
  }
  const Scalar slack = 1.0 - subsetX;
  if (!(slack > 0.0)) return false;
  const Scalar betaSubset = totalTheta - subsetTheta;

  Matrix nodes(k, order);
  Matrix weights(k, order);
  for (UnsignedInteger j = 0; j < k; ++j)
  {
    const Scalar alpha = betaSubset - 1.0 + static_cast<Scalar>(k - 1 - j);
    const Scalar shape = alpha + 1.0;
    /* Above one million the Jacobi nodes become expensive to compute and
       the large-shape normal approximation is both cheaper and accurate. */
    if (!(shape > 0.0) || !std::isfinite(shape) || (shape > 1.0e6)) return false;
    Point marginalWeights;
    const Point marginalNodes(JacobiFactory(1.0, shape).getNodesAndWeights(order, marginalWeights));
    for (UnsignedInteger q = 0; q < order; ++q)
    {
      nodes(j, q) = 0.5 * (marginalNodes[q] + 1.0);
      weights(j, q) = marginalWeights[q] / shape;
    }
  }

  Scalar logC = SpecFunc::LogGamma(totalTheta) - SpecFunc::LogGamma(betaSubset);
  for (const UnsignedInteger i : subset) logC -= SpecFunc::LogGamma(theta[i]);
  const Scalar logScale = logC + (betaSubset + k - 1.0) * std::log(slack);

  /* For large shape parameters the weighted sum underflows: accumulate it
     in logarithmic scale in that case. */
  /* Same log-scale switch as in the box quadrature: 650 is below
     log(max double) ~ 709 to keep the final exponentiation finite. */
  const Bool useLog = (logScale > 650.0);
  Matrix logWeights;
  if (useLog)
  {
    logWeights = Matrix(k, order);
    for (UnsignedInteger j = 0; j < k; ++j)
      for (UnsignedInteger q = 0; q < order; ++q)
        logWeights(j, q) = (weights(j, q) > 0.0 ? std::log(weights(j, q)) : -std::numeric_limits<Scalar>::infinity());
  }

  Scalar total = 0.0;
  Scalar maxLogTerm = -std::numeric_limits<Scalar>::infinity();
  Scalar logTotal = 0.0;
  Indices index(k, 0);
  Point z(k, 0.0);
  while (true)
  {
    Scalar running = 1.0;
    Scalar weight = 1.0;
    Scalar logWeight = 0.0;
    for (UnsignedInteger j = 0; j < k; ++j)
    {
      const Scalar node = nodes(j, index[j]);
      z[j] = running * node;
      running *= 1.0 - node;
      if (useLog) logWeight += logWeights(j, index[j]);
      else weight *= weights(j, index[j]);
    }
    Scalar product = 1.0;
    Scalar logProduct = 0.0;
    for (UnsignedInteger j = 0; j < k; ++j)
    {
      const UnsignedInteger i = subset[j];
      const Scalar base = x[i] + slack * z[j];
      if (!(base > 0.0)) return false;
      if (useLog) logProduct += (theta[i] - 1.0) * std::log(base);
      else product *= std::pow(base, theta[i] - 1.0);
    }
    if (useLog)
    {
      const Scalar logTerm = logWeight + logProduct;
      if (std::isfinite(logTerm))
      {
        if (logTerm > maxLogTerm)
        {
          logTotal = logTotal * std::exp(maxLogTerm - logTerm) + 1.0;
          maxLogTerm = logTerm;
        }
        else logTotal += std::exp(logTerm - maxLogTerm);
      }
    }
    else total += weight * product;
    UnsignedInteger position = 0;
    while (position < k)
    {
      ++index[position];
      if (index[position] < order) break;
      index[position] = 0;
      ++position;
    }
    if (position == k) break;
  }

  Scalar logValue = 0.0;
  if (useLog)
  {
    if (!(logTotal > 0.0) || !std::isfinite(maxLogTerm)) return false;
    logValue = logScale + maxLogTerm + std::log(logTotal);
  }
  else
  {
    if (!(total > 0.0) || !std::isfinite(total)) return false;
    logValue = logScale + std::log(total);
  }
  value = expProbability(logValue);
  return true;
}


/* Inclusion-exclusion over the upper corners. */
Bool computeUpperCDF(const Point & x,
                     const Point & theta,
                     const Scalar beta,
                     const UnsignedInteger maximumOrder,
                     Scalar & value,
                     Scalar & absoluteSum)
{
  const UnsignedInteger n = x.getSize();
  if (n > ResourceMap::GetAsUnsignedInteger("Dirichlet-CDFMaximumQuadratureDimension")) return false;
  Bool singular = (beta < 1.0);
  for (const Scalar shape : theta) singular = singular || (shape < 1.0);
  /* High-order tensor quadrature is too expensive for singular shapes
     beyond the box dimension. */
  if (singular && (n > ResourceMap::GetAsUnsignedInteger("Dirichlet-CDFMaximumBoxDimension"))) return false;
  Scalar sum = 0.0;
  absoluteSum = 0.0;
  Indices subset;
  /* The subsets are enumerated with a bit mask: protect the shift when a
     user raises the maximum dimension above the word size. */
  if (n >= static_cast<UnsignedInteger>(std::numeric_limits<UnsignedInteger>::digits)) return false;
  const UnsignedInteger numberOfSubsets = (static_cast<UnsignedInteger>(1) << n);
  for (UnsignedInteger mask = 1; mask < numberOfSubsets; ++mask)
  {
    subset.clear();
    Scalar subsetX = 0.0;
    for (UnsignedInteger i = 0; i < n; ++i)
      if (mask & (static_cast<UnsignedInteger>(1) << i))
      {
        subset.add(i);
        subsetX += x[i];
      }
    if (subsetX >= 1.0) continue;
    const UnsignedInteger k = subset.getSize();
    const UnsignedInteger order = upperQuadratureOrder(k, maximumOrder, singular);
    Scalar integral = 0.0;
    if (!computeUpperCornerIntegral(x, theta, beta, subset, order, integral)) return false;
    const Scalar signedIntegral = ((k % 2) == 1 ? integral : -integral);
    sum += signedIntegral;
    absoluteSum += integral;
  }
  value = 1.0 - sum;
  return true;
}


/* Recursive conditional integration.  It is slower than the other methods
   but remains positive and is used for very small probabilities or when
   inclusion-exclusion suffers from cancellation. */
Scalar computeRecursiveCDF(Point x,
                           Point theta,
                           Scalar beta,
                           const UnsignedInteger order)
{
  Point activeX;
  Point activeTheta;
  for (UnsignedInteger i = 0; i < x.getSize(); ++i)
  {
    if (x[i] <= 0.0) return 0.0;
    if (x[i] < 1.0)
    {
      activeX.add(x[i]);
      activeTheta.add(theta[i]);
    }
    else beta += theta[i];
  }
  const UnsignedInteger n = activeX.getSize();
  if (n == 0) return 1.0;
  if (n == 1) return DistFunc::pBeta(activeTheta[0], beta, activeX[0]);

  UnsignedInteger pivot = 0;
  for (UnsignedInteger i = 1; i < n; ++i)
    if (activeTheta[i] > activeTheta[pivot]) pivot = i;
  const Scalar x0 = activeX[pivot];
  const Scalar theta0 = activeTheta[pivot];
  Scalar sumTheta = beta;
  for (const Scalar value : activeTheta) sumTheta += value;
  const Scalar s = sumTheta - theta0;
  const Scalar logPre = -SpecFunc::LogBeta(theta0, s)
                        + std::log(x0 / theta0) + (theta0 - 1.0) * std::log(x0);
  const Scalar inverseTheta0 = 1.0 / theta0;

  Point otherX;
  Point otherTheta;
  for (UnsignedInteger i = 0; i < n; ++i)
    if (i != pivot)
    {
      otherX.add(activeX[i]);
      otherTheta.add(activeTheta[i]);
    }

  Point cuts;
  cuts.add(0.0);
  cuts.add(1.0);
  for (const Scalar xx : otherX)
  {
    const Scalar tCut = 1.0 - xx;
    if ((tCut > 0.0) && (tCut < x0))
    {
      const Scalar uCut = std::pow(tCut / x0, theta0);
      if ((uCut > 0.0) && (uCut < 1.0)) cuts.add(uCut);
    }
  }
  std::sort(cuts.begin(), cuts.end());
  cuts.erase(std::unique(cuts.begin(), cuts.end()), cuts.end());

  Point legendreWeights;
  const Point legendreNodes(LegendreFactory().getNodesAndWeights(order, legendreWeights));
  /* The nested conditional integrals use a slightly lower order than the
     outer one to keep the total cost polynomial in the order. */
  const UnsignedInteger subOrder = (order > 16 ? order - 8 : 12);
  Scalar total = 0.0;
  for (UnsignedInteger segment = 0; segment + 1 < cuts.getSize(); ++segment)
  {
    const Scalar lower = cuts[segment];
    const Scalar upper = cuts[segment + 1];
    if (!(upper > lower)) continue;
    const Scalar halfWidth = 0.5 * (upper - lower);
    for (UnsignedInteger q = 0; q < order; ++q)
    {
      const Scalar u = lower + halfWidth * (legendreNodes[q] + 1.0);
      const Scalar t = x0 * std::pow(u, inverseTheta0);
      const Scalar oneMinusT = 1.0 - t;
      if (!(oneMinusT > 0.0)) continue;
      Point z(otherX.getSize());
      for (UnsignedInteger i = 0; i < otherX.getSize(); ++i)
        z[i] = std::min(otherX[i] / oneMinusT, 1.0);
      const Scalar inner = computeRecursiveCDF(z, otherTheta, beta, subOrder);
      total += (upper - lower) * legendreWeights[q] * std::pow(oneMinusT, s - 1.0) * inner;
    }
  }
  if (!(total > 0.0)) return 0.0;
  const Scalar logValue = logPre + std::log(total);
  return clipProbability(expProbability(logValue));
}


/* Main deterministic CDF dispatcher. */
Scalar computeDirichletCDF(const Point & point,
                           const Point & theta,
                           Scalar beta,
                           const UnsignedInteger maximumOrder)
{
  Point x;
  Point activeTheta;
  for (UnsignedInteger i = 0; i < point.getSize(); ++i)
  {
    if (point[i] <= 0.0) return 0.0;
    if (point[i] < 1.0)
    {
      x.add(point[i]);
      activeTheta.add(theta[i]);
    }
    else beta += theta[i];
  }
  const UnsignedInteger n = x.getSize();
  if (n == 0) return 1.0;
  if (n == 1) return DistFunc::pBeta(activeTheta[0], beta, x[0]);

  const Scalar totalTheta = beta + std::accumulate(activeTheta.begin(), activeTheta.end(), 0.0);
  Point marginalCDF(n);
  for (UnsignedInteger i = 0; i < n; ++i)
    marginalCDF[i] = DistFunc::pBeta(activeTheta[i], totalTheta - activeTheta[i], x[i]);
  Scalar upper = marginalCDF[0];
  Scalar sumSurvival = 0.0;
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    upper = std::min(upper, marginalCDF[i]);
    sumSurvival += 1.0 - marginalCDF[i];
  }
  Scalar lower = std::max(0.0, 1.0 - sumSurvival);
  if (lower > upper)
  {
    const Scalar middle = 0.5 * (lower + upper);
    lower = middle;
    upper = middle;
  }
  if (upper <= 0.0) return 0.0;
  /* upper is a minimum of marginal CDFs, so its roundoff is a few ulps:
     a complement below 1e-15 is indistinguishable from zero here. */
  if (1.0 - upper <= 1.0e-15) return 1.0;

  const Scalar sumX = std::accumulate(x.begin(), x.end(), 0.0);
  const auto finish = [&](const Scalar candidate)
  {
    if (!(candidate == candidate)) return clipProbability(0.5 * (lower + upper));
    return clipProbability(std::max(lower, std::min(upper, candidate)));
  };
  const auto warnFrechetMidpoint = [&]()
  {
    LOGWARN(OSS() << "Dirichlet::computeCDF: only Frechet bounds are available in dimension " << n << ", returning their midpoint [" << lower << ", " << upper << "]");
  };

  const UnsignedInteger maximumBoxDimension = ResourceMap::GetAsUnsignedInteger("Dirichlet-CDFMaximumBoxDimension");
  const UnsignedInteger maximumQuadratureDimension = ResourceMap::GetAsUnsignedInteger("Dirichlet-CDFMaximumQuadratureDimension");
  const UnsignedInteger maximumIntegerBeta = ResourceMap::GetAsUnsignedInteger("Dirichlet-CDFMaximumIntegerBeta");
  const Scalar extonMaximumCancellation = ResourceMap::GetAsScalar("Dirichlet-CDFExtonMaximumCancellation");
  const Scalar upperMaximumCancellation = ResourceMap::GetAsScalar("Dirichlet-CDFUpperMaximumCancellation");
  const Scalar normalMinimumProbability = ResourceMap::GetAsScalar("Dirichlet-CDFNormalMinimumProbability");
  /* Relative slack granted above the Frechet upper bound when accepting a
     candidate estimate.  Five hundred thousand times SpecFunc::Precision is
     1e-10 with the default precision: it absorbs the roundoff accumulated by
     the quadrature and inclusion-exclusion sums while still rejecting a
     violation of the bound. */
  const Scalar upperBoundTolerance = 5.0e5 * SpecFunc::Precision;
  Scalar minShape = beta;
  for (const Scalar value : activeTheta) minShape = std::min(minShape, value);
  const Bool normalEligible = (n <= maximumQuadratureDimension)
                              && (totalTheta >= ResourceMap::GetAsScalar("Dirichlet-CDFNormalMinimumTotal"))
                              && (minShape >= ResourceMap::GetAsScalar("Dirichlet-CDFNormalMinimumShape"));

  if (sumX < 1.0)
  {
    const Scalar roundedBeta = std::round(beta);
    if (isIntegerShapeParameter(beta, roundedBeta)
        && (roundedBeta >= 2.0) && (roundedBeta <= maximumIntegerBeta))
    {
      Scalar value = 0.0;
      if (computeIntegerBetaCDF(x, activeTheta, beta, value)) return finish(value);
    }

    if (beta == 1.0)
    {
      Scalar value = 0.0;
      Scalar cancellation = 1.0;
      if (computeExtonCDF(x, activeTheta, beta, value, cancellation)) return finish(value);
    }

    /* Near the upper boundary the complement is small: inclusion-exclusion
       gives a much better relative accuracy than the box integral.  Below a
       complement of 1e-3 the cancellation error of the inclusion-exclusion
       stays acceptable, above it the box integral is preferred. */
    if ((n <= maximumQuadratureDimension) && (1.0 - upper < 1.0e-3))
    {
      Scalar value = 0.0;
      Scalar absoluteSum = 0.0;
      /* A candidate above the marginal upper bound by more than
         upperBoundTolerance is rejected. */
      if (computeUpperCDF(x, activeTheta, beta, maximumOrder, value, absoluteSum)
          && (value > 0.0) && (value <= upper * (1.0 + upperBoundTolerance))) return finish(value);
    }

    Scalar boxCandidate = 0.0;
    Bool hasBoxCandidate = false;
    if (n <= maximumBoxDimension)
    {
      UnsignedInteger boxOrder = maximumOrder;
      if (n == 4) boxOrder = std::min(maximumOrder, 28UL);
      else if (n == 5) boxOrder = std::min(maximumOrder, 16UL);
      else if (n == 6) boxOrder = std::min(maximumOrder, 12UL);
      else if (n >= 7) boxOrder = std::min(maximumOrder, 10UL);
      Scalar value = 0.0;
      if (computeBoxCDFAdaptive(x, activeTheta, beta, boxOrder, value)) return finish(value);
      if ((value > 0.0) && std::isfinite(value))
      {
        boxCandidate = value;
        hasBoxCandidate = true;
      }
    }

    {
      Scalar value = 0.0;
      Scalar cancellation = 0.0;
      /* The Exton series is only accurate to about 1e-8 here, hence the
         looser relative acceptance tolerance. */
      if (computeExtonCDF(x, activeTheta, beta, value, cancellation)
          && (cancellation < extonMaximumCancellation)
          && (value > 0.0) && (value <= upper * (1.0 + 1.0e-8))) return finish(value);
    }

    if (n <= maximumQuadratureDimension)
    {
      Scalar value = 0.0;
      Scalar absoluteSum = 0.0;
      if (computeUpperCDF(x, activeTheta, beta, maximumOrder, value, absoluteSum)
          && (value > 0.0) && (value <= upper * (1.0 + upperBoundTolerance))
          && (absoluteSum < upperMaximumCancellation * value)) return finish(value);
    }

    if (normalEligible)
    {
      const Scalar value = computeNormalApproximationCDF(x, activeTheta, beta);
      if ((value > normalMinimumProbability) && std::isfinite(value)) return finish(value);
    }

    if (n <= 5)
    {
      /* The recursive conditional integration is the last resort.  Its cost
         grows quickly with the order, so cap the order at 48 in dimension 3
         and below and at 32 in dimensions 4 and 5, unless the user maximum
         order is smaller. */
      const UnsignedInteger fallbackOrder = std::min(maximumOrder, (n <= 3 ? 48UL : 32UL));
      const Scalar value = computeRecursiveCDF(x, activeTheta, beta, fallbackOrder);
      if (value > 0.0) return finish(value);
    }
    if (hasBoxCandidate) return finish(boxCandidate);
    warnFrechetMidpoint();
    return finish(0.5 * (lower + upper));
  }

  /* Outside the simplex, inclusion-exclusion is the primary method.  When
     the event is so rare that the complement suffers from cancellation,
     switch to the positive recursive integration. */
  if (n <= maximumQuadratureDimension)
  {
    Scalar value = 0.0;
    Scalar absoluteSum = 0.0;
    if (computeUpperCDF(x, activeTheta, beta, maximumOrder, value, absoluteSum)
        && (value > 0.0) && (value <= upper * (1.0 + upperBoundTolerance))
        && (absoluteSum < upperMaximumCancellation * value)) return finish(value);
  }

  if (normalEligible)
  {
    const Scalar value = computeNormalApproximationCDF(x, activeTheta, beta);
    if ((value > normalMinimumProbability) && std::isfinite(value)) return finish(value);
  }

  if (n <= 5)
  {
    /* Same last-resort recursive order as in the inside-simplex branch. */
    const UnsignedInteger fallbackOrder = std::min(maximumOrder, (n <= 3 ? 48UL : 32UL));
    const Scalar value = computeRecursiveCDF(x, activeTheta, beta, fallbackOrder);
    return finish(value);
  }
  warnFrechetMidpoint();
  return finish(0.5 * (lower + upper));
}

} /* anonymous namespace */


/* Default constructor */
Dirichlet::Dirichlet()
  : DistributionImplementation()
{
  setName("Dirichlet");
  setTheta(Point(2, 1.0));
  isParallel_ = false;
}

/* Parameters constructor */
Dirichlet::Dirichlet(const Point & theta)
  : DistributionImplementation()
{
  setName("Dirichlet");
  setTheta(theta);
  isParallel_ = false;
}

/* Comparison operator */
Bool Dirichlet::operator ==(const Dirichlet & other) const
{
  if (this == &other) return true;
  return hasEqualBase(other) && theta_ == other.theta_;
}

Bool Dirichlet::equals(const DistributionImplementation & other) const
{
  const Dirichlet* p_other = dynamic_cast<const Dirichlet*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Dirichlet::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Dirichlet::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " theta=" << theta_;
  return oss;
}

String Dirichlet::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(theta = " << theta_.__str__() << ")";
  return oss;
}

/* Virtual constructor */
Dirichlet * Dirichlet::clone() const
{
  return new Dirichlet(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void Dirichlet::computeRange()
{
  setRange(Interval(Point(theta_.getSize() - 1, 0.0), Point(theta_.getSize() - 1, 1.0)));
}

/* Get one realization of the distribution. We use the representation:
 * X_k = Y_k / (Y_1 + ... + Y_{d+1}) where the Y_i are independent and
 * Y_i has a Gamma(theta_i) distribution
 */
Point Dirichlet::getRealization() const
{
  const UnsignedInteger dimension = getDimension();
  if (dimension == 1) return Point(1, DistFunc::rBeta(theta_[0], theta_[1]));
  Point realization(dimension);
  Scalar sum = DistFunc::rGamma(theta_[dimension]);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar yI = DistFunc::rGamma(theta_[i]);
    sum += yI;
    realization[i] = yI;
  }
  return realization * (1.0 / sum);
}

/* Get the PDF of the distribution */
Scalar Dirichlet::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Scalar logPDF = computeLogPDF(point);
  return std::exp(logPDF);
}

Scalar Dirichlet::computeLogPDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  Scalar sum = 0.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar xI = point[i];
    // Also rejects NaN.
    if (!(xI >= 0.0)) return SpecFunc::LowestScalar;
    sum += xI;
  }
  if (sum > 1.0) return SpecFunc::LowestScalar;

  /* Evaluate the continuous extension of the density on the closed simplex
     when it is finite: a zero component with a positive exponent contributes
     a zero factor, while a unit exponent contributes a unit factor. Singular
     exponents keep the zero convention of the Beta distribution. */
  Scalar logPDF = normalizationFactor_;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar exponent = theta_[i] - 1.0;
    if (point[i] > 0.0) logPDF += exponent * std::log(point[i]);
    else if (exponent != 0.0) return SpecFunc::LowestScalar;
  }
  const Scalar slack = 1.0 - sum;
  const Scalar lastExponent = theta_[dimension] - 1.0;
  if (slack > 0.0) logPDF += lastExponent * log1p(-sum);
  else if (lastExponent != 0.0) return SpecFunc::LowestScalar;
  return logPDF;
}

/* Get the CDF of the distribution */
Scalar Dirichlet::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Point theta(theta_.begin(), theta_.begin() + dimension);
  const Scalar beta = theta_[dimension];
  /* A quadrature rule needs at least three nodes, and a zero order would
     divide by zero in the tensor-product node count. */
  const UnsignedInteger maximumOrder = std::max(3UL, ResourceMap::GetAsUnsignedInteger("Dirichlet-DefaultIntegrationSize"));
  return computeDirichletCDF(point, theta, beta, maximumOrder);
}

/* Get the quantile of the distribution */
Scalar Dirichlet::computeScalarQuantile(const Scalar prob,
                                        const Bool tail) const
{
  if (!((prob >= 0.0) && (prob <= 1.0)))
    throw InvalidArgumentException(HERE) << "computeScalarQuantile expected prob to belong to [0,1], but is " << prob;
  return DistFunc::qBeta(theta_[0], theta_[1], prob, tail);
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar Dirichlet::computeConditionalPDF(const Scalar xj, const Point & xcond) const
{
  const UnsignedInteger conditioningDimension = xcond.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  const Scalar r = theta_[conditioningDimension];
  Scalar s = sumTheta_ - r;
  if (conditioningDimension == 0) return DistFunc::dBeta(r, s, xj);
  Scalar sumThetaConditioning = 0.0;
  Scalar sumY = 0.0;
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
  {
    if (xcond[i] < 0.0) return 0.0;
    sumThetaConditioning += theta_[i];
    sumY += xcond[i];
  }
  if (sumY >= 1.0) return 0.0;
  s -= sumThetaConditioning;
  const Scalar z = xj / (1.0 - sumY);
  if (!((z > 0.0) && (z < 1.0))) return 0.0;
  return DistFunc::dBeta(r, s, z) / (1.0 - sumY);
}

Point Dirichlet::computeSequentialConditionalPDF(const Point & x) const
{
  const UnsignedInteger dimension = getDimension();
  if (x.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: cannot compute a sequential conditional PDF at a point of dimension=" << x.getDimension() << " not equal to the distribution dimension=" << dimension;
  Point result(dimension);
  Scalar sumY = 0.0;
  Scalar r = theta_[0];
  Scalar s = sumTheta_ - r;
  result[0] = DistFunc::dBeta(r, s, x[0]);
  for (UnsignedInteger conditioningDimension = 1; conditioningDimension < dimension; ++conditioningDimension)
  {
    sumY += x[conditioningDimension - 1];
    if (sumY < 0.0) return result;
    if (sumY >= 1.0) return result;
    r = theta_[conditioningDimension];
    s -= r;
    const Scalar z = x[conditioningDimension] / (1.0 - sumY);
    if (!((z > 0.0) && (z < 1.0))) break;
    result[conditioningDimension] = DistFunc::dBeta(r, s, z) / (1.0 - sumY);
  }
  return result;
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar Dirichlet::computeConditionalCDF(const Scalar xj, const Point & xcond) const
{
  const UnsignedInteger conditioningDimension = xcond.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  const Scalar r = theta_[conditioningDimension];
  Scalar s = sumTheta_ - r;
  if (conditioningDimension == 0) return DistFunc::pBeta(r, s, xj);
  Scalar sumThetaConditioning = 0.0;
  Scalar sumY = 0.0;
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
  {
    if (xcond[i] < 0.0) return 0.0;
    sumThetaConditioning += theta_[i];
    sumY += xcond[i];
  }
  if (!(sumY < 1.0)) return 1.0;
  s -= sumThetaConditioning;
  return DistFunc::pBeta(r, s, xj / (1.0 - sumY));
}

Point Dirichlet::computeSequentialConditionalCDF(const Point & x) const
{
  const UnsignedInteger dimension = getDimension();
  if (x.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: cannot compute a sequential conditional CDF at a point of dimension=" << x.getDimension() << " not equal to the distribution dimension=" << dimension;
  Point result(dimension);
  Scalar sumY = 0.0;
  Scalar r = theta_[0];
  Scalar s = sumTheta_ - r;
  result[0] = DistFunc::pBeta(r, s, x[0]);
  for (UnsignedInteger conditioningDimension = 1; conditioningDimension < dimension; ++conditioningDimension)
  {
    sumY += x[conditioningDimension - 1];
    if (sumY < 0.0) return result;
    if (sumY >= 1.0)
    {
      for (UnsignedInteger i = conditioningDimension; i < dimension; ++i) result[i] = 1.0;
      return result;
    }
    r = theta_[conditioningDimension];
    s -= r;
    const Scalar z = x[conditioningDimension] / (1.0 - sumY);
    result[conditioningDimension] = DistFunc::pBeta(r, s, z);
  }
  return result;
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
Scalar Dirichlet::computeConditionalQuantile(const Scalar p, const Point & xcond) const
{
  const UnsignedInteger conditioningDimension = xcond.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if (!((p >= 0.0) && (p <= 1.0))) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level q=" << p << " outside of [0, 1]";
  Scalar sumThetaConditioning = 0.0;
  Scalar sumY = 0.0;
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
  {
    if (!((xcond[i] >= 0.0) && (xcond[i] <= 1.0))) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a conditioning point outside of the conditioning distribution range";
    sumThetaConditioning += theta_[i];
    sumY += xcond[i];
  }
  if (sumY >= 1.0) return 0.0;
  const Scalar r = theta_[conditioningDimension];
  const Scalar s = sumTheta_ - sumThetaConditioning - r;
  return (1.0 - sumY) * DistFunc::qBeta(r, s, p);
}

Point Dirichlet::computeSequentialConditionalQuantile(const Point & q) const
{
  const UnsignedInteger dimension = getDimension();
  if (q.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: cannot compute a sequential conditional PDF at a quantile level vector of dimension=" << q.getDimension() << " not equal to the distribution dimension=" << dimension;
  Point result(dimension);
  Scalar sumY = 0.0;
  Scalar r = theta_[0];
  Scalar s = sumTheta_ - r;
  result[0] = DistFunc::qBeta(r, s, q[0]);
  for (UnsignedInteger conditioningDimension = 1; conditioningDimension < dimension; ++conditioningDimension)
  {
    sumY += result[conditioningDimension - 1];
    if (sumY < 0.0) return result;
    if (sumY >= 1.0) return result;
    r = theta_[conditioningDimension];
    s -= r;
    result[conditioningDimension] = (1.0 - sumY) * DistFunc::qBeta(r, s, q[conditioningDimension]);
  }
  return result;
}

/* Compute the mean of the distribution */
void Dirichlet::computeMean() const
{
  const UnsignedInteger dimension = getDimension();
  mean_ = Point(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) mean_[i] = theta_[i] / sumTheta_;
  isAlreadyComputedMean_ = true;
}

/* Get the roughness, i.e. the L2-norm of the PDF */
Scalar Dirichlet::getRoughness() const
{
  const UnsignedInteger size = theta_.getSize();
  Scalar logNumerator = 0.0;
  Scalar sumTheta = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (theta_[i] <= 0.5) throw NotDefinedException(HERE) << "Error: the roughness is not defined for theta <= 0.5.";
    logNumerator += SpecFunc::LogGamma(2.0 * theta_[i] - 1.0);
    sumTheta += theta_[i];
  }
  Scalar logDenominator = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i) logDenominator += SpecFunc::LogGamma(theta_[i]);
  return std::exp(logNumerator - SpecFunc::LogGamma(2.0 * sumTheta - size) - 2.0 * (logDenominator - SpecFunc::LogGamma(sumTheta)));
}

/* Compute the entropy of the distribution */
Scalar Dirichlet::computeEntropy() const
{
  const UnsignedInteger dimension = getDimension();
  Scalar entropy = -normalizationFactor_ - (dimension + 1 - sumTheta_) * SpecFunc::Psi(sumTheta_);
  for (UnsignedInteger i = 0; i <= dimension; ++i)
    entropy -= (theta_[i] - 1.0) * SpecFunc::Psi(theta_[i]);
  return entropy;
}

/* Get the standard deviation of the distribution */
Point Dirichlet::getStandardDeviation() const
{
  const UnsignedInteger dimension = getDimension();
  Point sigma(dimension);
  Scalar factor = 1.0 / (sumTheta_ * std::sqrt(1.0 + sumTheta_));
  for (UnsignedInteger i = 0; i < dimension; ++i) sigma[i] = std::sqrt(theta_[i] * (sumTheta_ - theta_[i])) * factor;
  return sigma;
}

/* Get the skewness of the distribution */
Point Dirichlet::getSkewness() const
{
  const UnsignedInteger dimension = getDimension();
  Point skewness(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar thetaI = theta_[i];
    skewness[i] = 2.0 * (sumTheta_ - 2.0 * thetaI) / (sumTheta_ + 2.0) * std::sqrt((sumTheta_ + 1.0) / (thetaI * (sumTheta_ - thetaI)));
  }
  return skewness;
}

/* Get the kurtosis of the distribution */
Point Dirichlet::getKurtosis() const
{
  const UnsignedInteger dimension = getDimension();
  Point kurtosis(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar thetaI = theta_[i];
    kurtosis[i] = 3.0 * (sumTheta_ + 1.0) * (2.0 * sumTheta_ * sumTheta_ + thetaI * (sumTheta_ - 6.0) * (sumTheta_ - thetaI)) / (thetaI * (sumTheta_ - thetaI) * (3.0 + sumTheta_) * (2.0 + sumTheta_));
  }
  return kurtosis;
}

/* Compute the covariance of the distribution */
void Dirichlet::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  covariance_ = CovarianceMatrix(dimension);
  Scalar factor = 1.0 / (sumTheta_ * sumTheta_ * (1.0 + sumTheta_));
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar thetaI = theta_[i];
    covariance_(i, i) = factor * (thetaI * (sumTheta_ - thetaI));
    for (UnsignedInteger j = 0; j < i; ++j) covariance_(i, j) = -factor * thetaI * theta_[j];
  }
  isAlreadyComputedCovariance_ = true;
}

/* Theta accessor */
void Dirichlet::setTheta(const Point & theta)
{
  const UnsignedInteger size = theta.getSize();
  if (size <= 1) throw InvalidArgumentException(HERE) << "Error: the parameter theta must be of size at least 2.";
  for (UnsignedInteger i = 0; i < size; ++i) if (!(theta[i] > 0.0)) throw InvalidArgumentException(HERE) << "Error: all the components of theta must be positive";
  theta_ = theta;
  sumTheta_ = 0.0;
  normalizationFactor_ = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar thetaI = theta[i];
    if (!(thetaI > 0.0)) throw InvalidArgumentException(HERE) << "Error: the vector theta must have positive components, here theta(" << i << ")=" << thetaI;
    normalizationFactor_ -= SpecFunc::LogGamma(thetaI);
    sumTheta_ += thetaI;
  }
  normalizationFactor_ += SpecFunc::LogGamma(sumTheta_);
  setDimension(size - 1);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computeRange();
}

Point Dirichlet::getTheta() const
{
  return theta_;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution Dirichlet::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  if (dimension == 1) return clone();
  const UnsignedInteger outputDimension = indices.getSize();
  if (outputDimension == 1)
  {
    const UnsignedInteger index_i = indices[0];
    Beta marginal(theta_[index_i], sumTheta_ - theta_[index_i], 0.0, 1.0);
    marginal.setDescription({getDescription()[index_i]});
    return marginal;
  }
  Point thetaMarginal(outputDimension + 1);
  Scalar sumMarginal = 0.0;
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
  {
    const UnsignedInteger index_i = indices[i];
    const Scalar thetaI = theta_[index_i];
    sumMarginal += thetaI;
    thetaMarginal[i] = thetaI;
  }
  thetaMarginal[outputDimension] = sumTheta_ - sumMarginal;
  Dirichlet* marginal(new Dirichlet(thetaMarginal));
  marginal->setDescription(getDescription().select(indices));
  return marginal;
} // getMarginal(Indices)

/* Tell if the distribution has independent marginals */
Bool Dirichlet::hasIndependentCopula() const
{
  return getDimension() == 1;
}

/* Tell if the distribution has an elliptical copula */
Bool Dirichlet::hasEllipticalCopula() const
{
  return hasIndependentCopula();
}

DistributionImplementation::PointWithDescriptionCollection Dirichlet::getParametersCollection() const
{
  const UnsignedInteger dimension = getDimension();
  PointWithDescriptionCollection parameters(dimension);
  const Description description(getDescription());
  for (UnsignedInteger marginalIndex = 0; marginalIndex < dimension; ++marginalIndex)
  {
    PointWithDescription point(2);
    Description marginalDescription(point.getDimension());
    point[0] = theta_[marginalIndex];
    point[1] = sumTheta_ - theta_[marginalIndex];
    marginalDescription[0] = "theta";
    marginalDescription[1] = "sum theta";
    point.setDescription(marginalDescription);
    point.setName(description[marginalIndex]);
    parameters[marginalIndex] = point;
  } // marginalIndex
  return parameters;
}


void Dirichlet::setParametersCollection(const PointCollection & parametersCollection)
{
  const UnsignedInteger size = parametersCollection.getSize();
  const UnsignedInteger dimension = size;
  Point theta(dimension + 1);
  if ( size == 0 ) throw InvalidArgumentException(HERE) << "The collection is empty.";
  if ( parametersCollection[0].getSize() < 2 ) throw InvalidArgumentException(HERE) << "The collection is too small.";
  Scalar lastTheta = parametersCollection[0][0] + parametersCollection[0][1];// sum of all thetas
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    theta[i] = parametersCollection[i][0];
    lastTheta -= theta[i]; // subtract each theta except the last one
  }
  theta[dimension] = lastTheta;
  setTheta(theta);
}

Point Dirichlet::getParameter() const
{
  return theta_;
}

void Dirichlet::setParameter(const Point & parameter)
{
  setTheta(parameter);
}

Description Dirichlet::getParameterDescription() const
{
  const UnsignedInteger dimension = getDimension();
  Description description(dimension + 1);
  for (UnsignedInteger i = 0; i <= dimension; ++ i)
  {
    description[i] = (OSS() << "theta_" << i);
  }
  return description;
}

/* Method save() stores the object through the StorageManager */
void Dirichlet::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "theta_", theta_ );
  adv.saveAttribute( "sumTheta_", sumTheta_ );
  adv.saveAttribute( "normalizationFactor_", normalizationFactor_ );
}

/* Method load() reloads the object from the StorageManager */
void Dirichlet::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "theta_", theta_ );
  adv.loadAttribute( "sumTheta_", sumTheta_ );
  adv.loadAttribute( "normalizationFactor_", normalizationFactor_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
