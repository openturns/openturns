//                                               -*- C++ -*-
/**
 * @brief HSICVStat implements the computation of a biased HSIC index.
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
Scalar HSICVStat::computeHSICIndex(const CovarianceMatrix & CovMat1,
                                   const CovarianceMatrix & CovMat2,
                                   const SquareMatrix & weightMatrix) const
{

  UnsignedInteger n = weightMatrix.getNbColumns();

  /* U = ones((n, n)) */
  const SquareMatrix U(n, Collection<Scalar>(n * n, 1.0));

  SquareMatrix H1(n);
  SquareMatrix H2(n);
  const Point diag(n, 1.0);
  H1.setDiagonal(diag);
  H2.setDiagonal(diag);

  H1 = H1 - U * weightMatrix / n ;
  H2 = H2 - weightMatrix * U / n ;

  const SquareMatrix M = weightMatrix * CovMat1 * weightMatrix * H1 * CovMat2 * H2 / n / n;
  const Scalar trace = M.computeTrace();

  return trace;
}

/* Compute the asymptotic p-value */
Scalar HSICVStat::computePValue(const Gamma &dist,
                                const UnsignedInteger n,
                                const Scalar HSIC_obs,
                                const Scalar) const
{
  return dist.computeComplementaryCDF(HSIC_obs * n);
}

/* Is compatible with a Conditional HSIC Estimator ? Yes! */
Bool HSICVStat::isCompatibleWithConditionalAnalysis() const
{
  return true;
}

END_NAMESPACE_OPENTURNS
