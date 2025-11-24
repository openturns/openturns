//                                               -*- C++ -*-
/**
 *  @brief Wilks is a generic view of Wilks methods for computing
 * probabilities and related quantities by sampling and estimation
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
#include <cmath>
#include "openturns/Wilks.hxx"
#include "openturns/QuantileConfidence.hxx"

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
  LOGWARN("Wilks is deprecated in favor of QuantileConfidence");
}


/* Sample size computation */
UnsignedInteger Wilks::ComputeSampleSize(const Scalar quantileLevel,
    const Scalar confidenceLevel,
    const UnsignedInteger marginIndex)
{
  const QuantileConfidence algo(quantileLevel, confidenceLevel);
  return algo.computeUnilateralMinimumSampleSize(marginIndex, true);
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
