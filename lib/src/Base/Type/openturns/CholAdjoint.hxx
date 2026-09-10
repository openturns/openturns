//                                               -*- C++ -*-
/**
 *  @brief Adjoint of the lower Cholesky factorization
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
#ifndef OPENTURNS_CHOLADJOINT_HXX
#define OPENTURNS_CHOLADJOINT_HXX

#include "openturns/TriangularMatrix.hxx"
#include "openturns/Matrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

// Adjoint of the lower Cholesky factorization A = L L^T. Given L = chol(A)
// and LBar = dJ/dL for a scalar objective J, compute ABar = dJ/dA by
// reversing the in-place forward elimination algorithm (see e.g. Murray,
// 2016, "Differentiation of the Cholesky decomposition"). Only the lower
// triangle of ABar is filled, as A is symmetric and the factorization uses
// its lower triangle.
inline Matrix cholAdjoint(const TriangularMatrix & L,
                          const Matrix & LBar)
{
  const UnsignedInteger n = L.getDimension();
  Matrix ABar(n, n);
  Matrix work(LBar);
  for (UnsignedInteger j = n; j-- > 0;)
  {
    // Reverse of the inner loop over the rows below the diagonal
    for (UnsignedInteger i = j + 1; i < n; ++i)
    {
      const Scalar NBar = work(i, j) / L(j, j);
      work(j, j) -= NBar * L(i, j);
      ABar(i, j) += NBar;
      for (UnsignedInteger k = 0; k < j; ++k)
      {
        work(i, k) -= NBar * L(j, k);
        work(j, k) -= NBar * L(i, k);
      }
    }
    // Reverse of the diagonal square-root step
    const Scalar sBar = work(j, j) / (2.0 * L(j, j));
    ABar(j, j) += sBar;
    for (UnsignedInteger k = 0; k < j; ++k)
      work(j, k) -= 2.0 * sBar * L(j, k);
  }
  return ABar;
}

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CHOLADJOINT_HXX */
