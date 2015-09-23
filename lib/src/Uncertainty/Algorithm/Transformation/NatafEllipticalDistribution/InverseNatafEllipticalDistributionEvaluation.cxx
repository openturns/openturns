//                                               -*- C++ -*-
/**
 *  @brief Class for the InverseNataf transformation evaluation for elliptical
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "OTprivate.hxx"
#include "InverseNatafEllipticalDistributionEvaluation.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class InverseNatafEllipticalDistributionEvaluation
 *
 * This class offers an interface for the InverseNataf function for elliptical distributions
 */

CLASSNAMEINIT(InverseNatafEllipticalDistributionEvaluation);

static const Factory<InverseNatafEllipticalDistributionEvaluation> RegisteredFactory;

/* Default constructor */
InverseNatafEllipticalDistributionEvaluation::InverseNatafEllipticalDistributionEvaluation()
  : LinearNumericalMathEvaluationImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
InverseNatafEllipticalDistributionEvaluation::InverseNatafEllipticalDistributionEvaluation(const NumericalPoint & mean,
    const TriangularMatrix & cholesky)
  : LinearNumericalMathEvaluationImplementation(
    NumericalPoint(mean.getDimension(), 0.0),
    mean,
    cholesky.transpose()
  )
{
  // Nothing to do
}

/* Virtual constructor */
InverseNatafEllipticalDistributionEvaluation * InverseNatafEllipticalDistributionEvaluation::clone() const
{
  return new InverseNatafEllipticalDistributionEvaluation(*this);
}

/* String converter */
String InverseNatafEllipticalDistributionEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << InverseNatafEllipticalDistributionEvaluation::GetClassName()
      << " mean=" << getCenter()
      << " cholesky=" << getLinear().transpose();

  return oss;
}

/* Gradient according to the marginal parameters.
 * For the InverseNataf transformation based on elliptical
 * distributions, the marginal parameters \theta are the mean
 * vector \mu and the marginal standard deviations \sigma,
 * which are the diagonal terms of the Cholesky factor L
 * of the covariance matrix: \theta = (\mu, \sigma)
 * The expression of the inverse transformation is:
 *
 * T^(-1)_i(U) = mu_i + \sum_{j=1}^i L_{ij}U_j
 *
 * thus we have:
 *
 * dT^(-1)_i/dmu_j = 1 for j=i,
 *                 = 0 otherwise
 * dT^(-1)_i/dsigma_j = U_i for j=i,
 *                    = 0 otherwise
 *
 * The (i,j)th element of the gradient is dT^(-1)_j/dtheta_i.
 *
 */

Matrix InverseNatafEllipticalDistributionEvaluation::parametersGradient(const NumericalPoint & inP) const
{
  UnsignedInteger inputDimension(getInputDimension());
  Matrix result(2 * inputDimension, inputDimension);
  // dT_j/dmu_i
  for (UnsignedInteger i = 0; i < inputDimension; ++i) result(i, i) = 1.0;
  // dTj/dsigma_j
  for (UnsignedInteger i = 0; i < inputDimension; ++i) result(inputDimension + i, i) = inP[i];
  return result;
}

/* Method save() stores the object through the StorageManager */
void InverseNatafEllipticalDistributionEvaluation::save(Advocate & adv) const
{
  LinearNumericalMathEvaluationImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void InverseNatafEllipticalDistributionEvaluation::load(Advocate & adv)
{
  LinearNumericalMathEvaluationImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
