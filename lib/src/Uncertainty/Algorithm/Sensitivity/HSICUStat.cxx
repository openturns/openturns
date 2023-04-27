
//                                               -*- C++ -*-
/**
 * @brief HSICUStat implements the computation of an unbiased HSIC index.
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
#include "openturns/HSICUStat.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HSICUStat)
/* Default constructor */
HSICUStat::HSICUStat()
{
  // Nothing
};

/* Virtual constructor */
HSICUStat* HSICUStat::clone() const
{
  return new HSICUStat(*this);
}

/* Compute the HSIC index for one marginal*/
Scalar HSICUStat::computeHSICIndex(const CovarianceMatrix & covarianceMatrix1,
                                   const CovarianceMatrix & covarianceMatrix2,
                                   const SquareMatrix & weightMatrix) const
{
  LOGWARN(OSS() << "computeHSICIndex(covarianceMatrix1, covarianceMatrix2, weightMatrix) is deprecated in favor of computeHSICIndex(covarianceMatrix1, covarianceMatrix2, weightPoint)");
  Scalar hsic = 0.0;
  const SignedInteger n = weightMatrix.getNbColumns();
  
  CovarianceMatrix covarianceMatrix1Copy(covarianceMatrix1);
  covarianceMatrix1Copy.setDiagonal(0.0, 0);
  CovarianceMatrix covarianceMatrix2Copy(covarianceMatrix2);
  covarianceMatrix2Copy.setDiagonal(0.0, 0);


  const SquareMatrix Kv(covarianceMatrix1Copy * covarianceMatrix2Copy);

  const Scalar trace = Kv.computeTrace();
  const Scalar sumKv = Kv.computeSumElements();
  const Scalar sumCov1 = covarianceMatrix1Copy.computeSumElements();
  const Scalar sumCov2 = covarianceMatrix2Copy.computeSumElements();

  hsic = trace - 2 * sumKv / (n - 2) + sumCov1 * sumCov2 / (n - 1) / (n - 2);
  hsic /= n * (n - 3);

  return hsic;
}

/* Compute the HSIC index for one marginal*/
Scalar HSICUStat::computeHSICIndex(const CovarianceMatrix &covarianceMatrix1,
                                   const CovarianceMatrix &covarianceMatrix2,
                                   const Point &weights) const
{
  /*
  The interest is to provide the U-stat, which is function of (Kx, Ky)
  If we define M as
    M = MKx * MKy
  with MKx (resp MKy)  matrices equal to Kx (resp Ky) outside diagonal, and 0
  on this last one
  Then u-stat defines as :
    hsic = trace(M) - 2 * sum(M) / (n - 2) + sum(MKx) * sum(MKy) / (n - 1) / (n - 2)
  sum(.) refers to the sum of elements of the matrix.

  If we create MKy and MKy as copies of Kx and Ky, we have some performance issues
  when n becomes high.
  Thus we propose an implementation where we only create small objects (vectors)
  and use blas operations to avoiding copies
  Indeed,
     Mkx = Kx - Dx
     MKy = Ky - Dy
  where Dx (respectively Dy) is a diagonal matrix containing the diagonal values of Kx
  (respectively Ky). Then
    M = (Kx - Dx) * (Ky - Dy)
      = Kx * Ky - Kx * Dy - Dx * Ky + Dx * Dy
  Having an eye on the previous formula,
    sum(MKx) = sum(Kx) - trace(Kx)
    sum(MKy) = sum(Ky) - trace(Ky)
  Now let us focus on trace(M), sum(M)
    trace(M) = trace(Kx * Ky - Kx * Dy - Dx * Ky + Dx * Dy)
    trace(M) = trace(Kx * Ky) - trace(Kx * Dy) - trace(Dx * Ky) + trace(Dx * Dy)

  It is easy to notice that
    (Kx * Dy)[i,j] = Kx[i,j] * Dy[j]
    (Dx * Ky)[i,j] = Ky[i,j] * Dx[i]
  Then
    trace(Kx * Dy) = \sum_i Kx[i,i] * Dy[i]
                   = \sum_i Kx[i,i] * Ky[i,i]
  One could notice that :
    trace(Dx * Ky) = \sum_i Kx[i,i] * Ky[i,i]
                   = trace(Kx * Dy)
                  =  trace(Dx * Dy)
  Thus
    trace(M) = trace(Kx * Ky) - trace(Dx * Dy)
  Focusing on the sum of elements:
    sum(Kx * Dy) = \sum_{i,j} Kx[i,j] * Dy[j]
                 = \sum_{i} KxDy[i]
  with KxDy = Kx * vect(Dy) (vect(.) is seeing . as a vector)
  By similarity, we get sum(Dx * Ky) (using sum(M) = sum(M^t))
  Finally
    sum(M) = sum(Kx * Ky) - \sum_{i} KxDy[i] - \sum_{i} KyDx[i] + <Dx, Dy>
  */

  Scalar hsic = 0.0;
  const SignedInteger n = weights.getDimension();
  const Point ones(n, 1.0);

  const SquareMatrix Kv(covarianceMatrix1 * covarianceMatrix2);

  // Compute \sum Kx * Dy
  const Point diagonalKy(covarianceMatrix2.getDiagonalAsPoint());
  const Point KxDy(covarianceMatrix1 * diagonalKy);
  const Scalar sumKxDy = KxDy.dot(ones);

  // Compute Ky * Dx
  const Point diagonalKx(covarianceMatrix1.getDiagonalAsPoint());
  const Point KyDx(covarianceMatrix2 * diagonalKx);
  const Scalar sumKyDx = KyDx.dot(ones);

  const Scalar sumDxDy = diagonalKx.dot(diagonalKy);

  const Scalar trace = Kv.computeTrace() - sumDxDy;
  const Scalar sumKv = Kv.computeSumElements() - sumKyDx - sumKxDy + sumDxDy;

  const Scalar sumCov1 = covarianceMatrix1.computeSumElements() - diagonalKx.dot(ones);
  const Scalar sumCov2 = covarianceMatrix2.computeSumElements() - diagonalKy.dot(ones);

  hsic = trace - 2 * sumKv / (n - 2) + sumCov1 * sumCov2 / (n - 1) / (n - 2);
  hsic /= n * (n - 3);
  return hsic;
}

/* Compute the asymptotic p-value */
Scalar HSICUStat::computePValue(const Gamma &dist,
                                const UnsignedInteger n,
                                const Scalar HSIC_obs,
                                const Scalar mHSIC) const
{
  return dist.computeComplementaryCDF(HSIC_obs * n + mHSIC * n);
}

/* Is compatible with a Conditional HSIC Estimator ? No! */
Bool HSICUStat::isCompatibleWithConditionalAnalysis() const
{
  return false;
}

END_NAMESPACE_OPENTURNS
