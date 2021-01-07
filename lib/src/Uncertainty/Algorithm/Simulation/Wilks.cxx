//                                               -*- C++ -*-
/**
 *  @brief Wilks is a generic view of Wilks methods for computing
 * probabilities and related quantities by sampling and estimation
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
#include <cmath>
#include "openturns/Wilks.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Sample.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"

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

/* Sample size computation */
UnsignedInteger Wilks::ComputeSampleSize(const Scalar quantileLevel,
    const Scalar confidenceLevel,
    const UnsignedInteger marginIndex)
{
  if ((quantileLevel <= 0.0) || (quantileLevel >= 1.0)) throw InvalidArgumentException(HERE) << "Error: quantile level must be in ]0, 1[, here quantile level=" << quantileLevel;
  if ((confidenceLevel <= 0.0) || (confidenceLevel >= 1.0)) throw InvalidArgumentException(HERE) << "Error: confidence level must be in ]0, 1[, here confidence level=" << confidenceLevel;
  // Initial guess based on asymptotic normality of the empirical quantile
  Scalar x = DistFunc::qNormal(confidenceLevel);
  Scalar x2 = x * x;
  UnsignedInteger n = (UnsignedInteger)(ceil((quantileLevel * x2 + 2.0 * marginIndex + sqrt(quantileLevel * x2 * (quantileLevel * x2 + 4.0 * marginIndex))) / (2.0 * (1.0 - quantileLevel))));
  // Initialize cQuantileLevel to cQuantileLevel[i] = Binomial(n, n - marginIndex + i) * quantileLevel ^ (n - marginIndex + i) * (1 - quantileLevel) ^ (marginIndex - i)
  // For i = marginIndex, cQuantileLevel[i] = quantileLevel ^ n
  Point cQuantileLevel(marginIndex + 1, pow(quantileLevel, static_cast<int>(n)));
  // Descending recursion n! / (m - i)! / (n - m + i)! = n! / (m - (i - 1))! / (n - m + (i - 1))! * (m - (i - 1)) / (n - m + i)
  // -> Binomial(n, n - m + i - 1) = Binomial(n, n - m + i) * (n - m + i) / (m - i)
  for (UnsignedInteger i = marginIndex; i > 0; --i) cQuantileLevel[i - 1] = cQuantileLevel[i] * (1.0 - quantileLevel) / quantileLevel * (n - marginIndex + i) / (marginIndex - i + 1.0);
  Scalar cumulQuantileLevel = -1.0;
  do
  {
    cumulQuantileLevel = 0.0;
    ++n;
    // Update the terms in the sum
    // Binomial(n + 1, n + 1 - m + j) * quantileLevel ^ (n + 1 - m + j) * (1 - quantileLevel) ^ (m - j)= Binomial(n, n - m + j) * quantileLevel ^ (n - m + j) * (1 - quantileLevel) ^ (m - j) * quantileLevel * (n + 1) / (n + 1 - m + j)
    for (UnsignedInteger j = 0; j <= marginIndex; ++j)
    {
      cQuantileLevel[j] *= (n * quantileLevel) / (n - marginIndex + j);
      cumulQuantileLevel += cQuantileLevel[j];
    }
  }
  while (cumulQuantileLevel > 1.0 - confidenceLevel);
  return n;
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
