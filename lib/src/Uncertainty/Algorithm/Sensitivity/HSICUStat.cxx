
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

  Scalar hsic = 0.0;
  const SignedInteger n = weightMatrix.getNbColumns();

  const Point nullDiag(n);
  
  CovarianceMatrix covarianceMatrix1Copy(covarianceMatrix1);
  CovarianceMatrix covarianceMatrix2Copy(covarianceMatrix2);
  covarianceMatrix1Copy.setDiagonal(nullDiag, 0);
  covarianceMatrix2Copy.setDiagonal(nullDiag, 0);


  const SquareMatrix Kv(covarianceMatrix1Copy * covarianceMatrix2Copy);

  const Scalar trace = Kv.computeTrace();
  const Scalar sumKv = Kv.computeSumElements();
  const Scalar sumCov1 = covarianceMatrix1Copy.computeSumElements();
  const Scalar SumCov2 = covarianceMatrix2Copy.computeSumElements();

  hsic = trace - 2 * sumKv / (n - 2) + sumCov1 * SumCov2 / (n - 1) / (n - 2);
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
