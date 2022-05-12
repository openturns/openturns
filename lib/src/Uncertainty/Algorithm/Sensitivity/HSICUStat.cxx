
//                                               -*- C++ -*-
/**
 * @brief HSICUStat implements the computation of an unbiased HSIC index.
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
Scalar HSICUStat::computeHSICIndex(const CovarianceMatrix & CovMat1,
                                   const CovarianceMatrix & CovMat2,
                                   const SquareMatrix & weightMatrix) const
{
  if(CovMat1.getDimension() != CovMat2.getDimension()) throw InvalidDimensionException(HERE) << "Input and Output covariance matrices must have the same size";

  Scalar hsic = 0.0;
  const SignedInteger n = weightMatrix.getNbColumns();

  const Point nullDiag(n);
  
  CovarianceMatrix CovMat1cp(CovMat1);
  CovarianceMatrix CovMat2cp(CovMat2);
  CovMat1cp.setDiagonal(nullDiag, 0);
  CovMat2cp.setDiagonal(nullDiag, 0);


  const SquareMatrix Kv = CovMat1cp * CovMat2cp;

  const Scalar trace = Kv.computeTrace();
  const Scalar sumKv = Kv.computeSumElements();
  const Scalar sumCov1 = CovMat1cp.computeSumElements();
  const Scalar SumCov2 = CovMat2cp.computeSumElements();

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
