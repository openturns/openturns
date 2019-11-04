//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformation evaluation for elliptical
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OTprivate.hxx"
#include "openturns/NatafEllipticalDistributionEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class NatafEllipticalDistributionEvaluation
 *
 * This class offers an interface for the Nataf function for elliptical distributions
 */

CLASSNAMEINIT(NatafEllipticalDistributionEvaluation)

static const Factory<NatafEllipticalDistributionEvaluation> Factory_NatafEllipticalDistributionEvaluation;

/* Default constructor */
NatafEllipticalDistributionEvaluation::NatafEllipticalDistributionEvaluation():
  LinearEvaluation()
{
  // Nothing to do
}

/* Parameter constructor */
NatafEllipticalDistributionEvaluation::NatafEllipticalDistributionEvaluation(const Point & mean,
    const TriangularMatrix & inverseCholesky)
  : LinearEvaluation(
      mean,
      Point(mean.getDimension(), 0.0),
      inverseCholesky.transpose()
    )
{
  // Nothing to do
}

/* Virtual constructor */
NatafEllipticalDistributionEvaluation * NatafEllipticalDistributionEvaluation::clone() const
{
  return new NatafEllipticalDistributionEvaluation(*this);
}

/* String converter */
String NatafEllipticalDistributionEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << NatafEllipticalDistributionEvaluation::GetClassName()
      << " mean=" << getCenter()
      << " inverseCholesky=" << getLinear().transpose();

  return oss;
}

/* Gradient according to the marginal parameters.
 * For the Nataf transformation based on elliptical distributions,
 * the marginal parameters \theta are the mean vector \mu and the
 * marginal standard deviations \sigma, which are the reciprocal
 * of the diagonal terms of the inverse Cholesky factor \Gamma of
 * the covariance matrix: \theta = (\mu, \sigma)
 * The expression of the transformation is:
 *
 * T_i(X) = \sum_{j=1}^i \Gamma_{ij}(X_j - mu_j)
 *
 * thus we have:
 *
 * dT_i/dmu_j = -Gamma_{ij} for j>=i,
 *            = 0 otherwise
 * dT_i/dsigma_j = -Gamma_{ii}^2(X_i - mu_i) for j=i,
 *               = 0 otherwise
 *
 * The (i,j)th element of the gradient is dT_j/dtheta_i.
 *
 */

Matrix NatafEllipticalDistributionEvaluation::parameterGradient(const Point & inP) const
{
  Point center(getCenter()); // \mu
  Matrix linear(getLinear()); // \Gamma^t, transposed inverse Cholesky
  UnsignedInteger inputDimension = getInputDimension();
  Matrix result(2 * inputDimension, inputDimension);
  // dT_j/dmu_i
  for (UnsignedInteger i = 0; i < inputDimension; ++i)
    for (UnsignedInteger j = i; j < inputDimension; ++j) result(i, j) = -linear(i, j);
  // dTj/dsigma_j
  for (UnsignedInteger i = 0; i < inputDimension; ++i) result(inputDimension + i, i) = -linear(i, i) * linear(i, i) * (inP[i] - center[i]);
  return result;
}

/* Method save() stores the object through the StorageManager */
void NatafEllipticalDistributionEvaluation::save(Advocate & adv) const
{
  LinearEvaluation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void NatafEllipticalDistributionEvaluation::load(Advocate & adv)
{
  LinearEvaluation::load(adv);
}

END_NAMESPACE_OPENTURNS
