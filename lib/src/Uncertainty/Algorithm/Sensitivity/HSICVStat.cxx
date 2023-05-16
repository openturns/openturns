//                                               -*- C++ -*-
/**
 * @brief HSICVStat implements the computation of a biased HSIC index.
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Pointer.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/HSICVStat.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HSICVStat)
/* Default constructor */
HSICVStat::HSICVStat()
{
  // Nothing
}

/* Virtual constructor */
HSICVStat* HSICVStat::clone() const
{
  return new HSICVStat(*this);
}

/* Compute the HSIC index for one marginal*/
Scalar HSICVStat::computeHSICIndex(const CovarianceMatrix &CovMat1,
                                   const CovarianceMatrix &CovMat2,
                                   const Point &weights) const
{
  /*
  The interest is to provide the V stat, which is the trace of
  M = (W * Kx * W) * (H1 * Ky * H2)
  Left side involves only Kx, the second only Ky
  The first block is easy to compute
  (W * Kx * W)_[i,j] = W[i] * Kx[i,j] * W[j]
  However we will not "build" a full matrix, as we intend
  to perform computations without using blas/lapack
  The second block needs more computations.
  If we drop the 1/n factor (in H1 / H2), we get
  (H1 * Ky * H2) = (I - U * W/n) * Ky * (I - W * U/n)
                 = (Ky - Ky * W * U / n - U * W * Ky / n + U * W * Ky * W * U / n / n)
  Looking more closely,
  (Ky * W * U)[i,j] = \sum_k Ky[i,k] * W[k]
  (U * Ky * W)[i,j] = \sum_k Ky[k,j] * W[k]
  Thus summing the two provides :
  1/n * (Ky * W * U + U * Ky * W)[i,j] = 1/n \sum_k (Ky[i,k] + Ky[k,j]) * W[k]
                                       = 1/n \sum_k (Ky[i,k] + Ky[j,k]) * W[k]
  We define the weightedSumRows to compute these sums.
  One could notice that:
  weightedSumRows[l] = \sum{k} Ky[l,k] * W[k]
  weightedSumRows[l] = Ky * W
  It a standard matrix-vector product.
  For the last part of the second block, we need to compute (U * W * Ky * W * U)
  W * K * W is an easy task (same as left block but replacing Kx with Ky).
  Multiplying the latter by U on left and right allows one to sum all the elements
  (U M U)[i,j] = \sum_{k, l} M[k,l] for all i, j
  The result is a square matrix with values \sum_{k,l} Ky[k,l] * W[k] * W[l]
  We define weightedSumElements to compute this
  weightedSumElements =  \sum_{k, l} M[k,l]
                      =  \sum_{k, l} Ky[k,k] * W[k] * W[l]
                      =  \sum_{k} W[k] * (Ky[k,l] * W[l])
                      =  \sum_{k} W[k] * weightedSumRows[k]
  weightedSumElements is an inner product between W and weightedSumRows
  Finally we get for the right side block a matrix that writes as
  (H1 * Ky * H2)[i,j] = Ky[i,j] - \sum_k (Ky[i,k] + Ky[j,k]) * W[k] / n + \sum_{k,l} Ky[k,l] * W[k] * W[l] /n /n
                      = Ky[i,j] -  (weightedSumElements[i] + weightedSumElements[j]) + weightedSumElements
  One has to note that both left & right side blocks are symmetric.
  Finally the trace is computed manually. Indeed it is a O(n^2) algorithm
  whereas building matrices (left/right), computing left * right then the trace is O(n^3)
  trace(left * right) = \sum{i,j} left[i,j] right[j,i]
                      = \sum{i,j} left[i,j] right[i,j] (because of symmetry)
  */
  // Compute weighted sum \sum_k (Ky[i,k] + Ky[j,k]) * W[k]
  const UnsignedInteger n = weights.getDimension();
  Point weightedSumRows(CovMat2 * weights);
  // weightedSumRows scaled by 1/n
  weightedSumRows /= n;
  // We compute the sum of the weighted matrix
  Scalar weightedSumElements = weightedSumRows.dot(weights);
  // weighted sum elements of K is to be scaled by 1/ n / n
  // As weightedSumRows is already scaled, we scale with 1/n
  weightedSumElements /= n;

  // Compute the trace : the algorithm is O(n^2)
  // If we compute left * right and then the trace, the algorithm is O(n^3)
  Scalar trace = 0.0;
  for (UnsignedInteger j = 0; j < n; ++j)
  {
    const Scalar wj = weights[j];
    const Scalar wsrJ = weightedSumRows[j];
    const Scalar cxJJ = CovMat1(j, j);
    const Scalar cyJJ = CovMat2(j, j);
    trace += (wj * cxJJ * wj) * (cyJJ + weightedSumElements - 2 * wsrJ);
    for (UnsignedInteger i = j + 1; i < n; ++i)
    {
      const Scalar wi = weights[i];
      const Scalar wsrI = weightedSumRows[i];
      const Scalar cxIJ = CovMat1(i, j);
      const Scalar cyIJ = CovMat2(i, j);
      trace += 2.0 * (wi * cxIJ * wj) * (cyIJ + weightedSumElements - wsrI - wsrJ);
    }
  }
  return trace / n / n;
}

/* Compute the asymptotic p-value */
Scalar HSICVStat::computePValue(const Gamma & distribution,
                                const UnsignedInteger n,
                                const Scalar HSICObs,
                                const Scalar) const
{
  return distribution.computeComplementaryCDF(HSICObs * n);
}

/* Is compatible with a Conditional HSIC Estimator ? Yes! */
Bool HSICVStat::isCompatibleWithConditionalAnalysis() const
{
  return true;
}

END_NAMESPACE_OPENTURNS
