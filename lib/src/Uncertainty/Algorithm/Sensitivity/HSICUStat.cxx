
//                                               -*- C++ -*-
/**
 * @brief HSICUStat implements the computation of an unbiased HSIC index.
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
#include "openturns/Pointer.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/HSICUStat.hxx"
BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HSICUStat)
/** Default constructor */
HSICUStat::HSICUStat()
{
  // Nothing
};

/** Virtual constructor */
HSICUStat* HSICUStat::clone() const
{
  return new HSICUStat(*this);
}

/** Compute the HSIC index for one marginal*/
Scalar HSICUStat::computeHSICIndex(const Sample & inSample, const Sample & outSample, const CovarianceModel & inCovariance, const CovarianceModel & outCovariance, const SquareMatrix & weightMatrix) const
{
  if(inSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Input Sample must be of dimension 1";
  if(outSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Output Sample must be of dimension 1";
  if(inCovariance.getInputDimension() != 1) throw InvalidDimensionException(HERE) << "Input covariance input dimension must be 1";
  if(outCovariance.getInputDimension() != 1) throw InvalidDimensionException(HERE) << "Output covariance input dimension must be 1";
  if(inCovariance.getOutputDimension() != 1) throw InvalidDimensionException(HERE) << "Input covariance output dimension must be 1";
  if(outCovariance.getOutputDimension() != 1) throw InvalidDimensionException(HERE) << "Output covariance output dimension must be 1";
  if(inSample.getSize() != outSample.getSize()) throw InvalidDimensionException(HERE) << "Input and Output Samples must have the same size";

  Scalar hsic = 0.0;
  SignedInteger n = weightMatrix.getNbColumns();

  CovarianceMatrix Kv1 = inCovariance.discretize(inSample);
  CovarianceMatrix Kv2 = outCovariance.discretize(outSample);

  Point nullDiag(n);
  Kv1.setDiagonal(nullDiag, 0);
  Kv2.setDiagonal(nullDiag, 0);


  SquareMatrix Kv = Kv1 * Kv2;

  Scalar trace = Kv.computeTrace();
  Scalar sumKv = Kv.computeSumElements();
  Scalar sumKv1 = Kv1.computeSumElements();
  Scalar sumKv2 = Kv2.computeSumElements();

  hsic = trace - 2 * sumKv / (n - 2) + sumKv1 * sumKv2 / (n - 1) / (n - 2);
  hsic /= n * (n - 3);

  return hsic;
}

/** Compute the asymptotic p-value */
Scalar HSICUStat::computePValue(const Gamma &dist, const UnsignedInteger n, const Scalar HSIC_obs, const Scalar mHSIC) const
{
  return dist.computeComplementaryCDF(HSIC_obs * n + mHSIC * n);
}

/** Is compatible with a Conditional HSIC Estimator ? No! */
Bool HSICUStat::isCompatibleWithConditionalAnalysis() const
{
  return false;
}

END_NAMESPACE_OPENTURNS
