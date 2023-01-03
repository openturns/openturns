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
Scalar HSICVStat::computeHSICIndex(const Sample & inSample,
                                   const Sample & outSample,
                                   const CovarianceModel & inCovariance,
                                   const CovarianceModel & outCovariance,
                                   const SquareMatrix & weightMatrix) const
{
  if(inSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Input Sample must be of dimension 1";
  if(outSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Output Sample must be of dimension 1";
  if(inCovariance.getInputDimension() != 1) throw InvalidDimensionException(HERE) << "Input covariance input dimension must be 1";
  if(outCovariance.getInputDimension() != 1) throw InvalidDimensionException(HERE) << "Output covariance input dimension must be 1";
  if(inCovariance.getOutputDimension() != 1) throw InvalidDimensionException(HERE) << "Input covariance output dimension must be 1";
  if(outCovariance.getOutputDimension() != 1) throw InvalidDimensionException(HERE) << "Output covariance output dimension must be 1";
  if(inSample.getSize() != outSample.getSize()) throw InvalidDimensionException(HERE) << "Input and Output Samples must have the same size";

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

  const CovarianceMatrix Kv1(inCovariance.discretize(inSample));
  const CovarianceMatrix Kv2(outCovariance.discretize(outSample));

  const SquareMatrix M = weightMatrix * Kv1 * weightMatrix * H1 * Kv2 * H2 / n / n;
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
