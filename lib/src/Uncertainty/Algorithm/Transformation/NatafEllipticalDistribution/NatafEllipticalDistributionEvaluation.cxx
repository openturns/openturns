//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformation evaluation for elliptical
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
      inverseCholesky
    )
{
  // Nothing to do
}

/* Parameter constructor with recorded (mu, sigma) scales */
NatafEllipticalDistributionEvaluation::NatafEllipticalDistributionEvaluation(const Point & mean,
    const TriangularMatrix & inverseCholesky,
    const Point & parameters)
  : LinearEvaluation(
      mean,
      Point(mean.getDimension(), 0.0),
      inverseCholesky
    )
{
  EvaluationImplementation::setParameter(parameters);
}

/* Virtual constructor */
NatafEllipticalDistributionEvaluation * NatafEllipticalDistributionEvaluation::clone() const
{
  return new NatafEllipticalDistributionEvaluation(*this);
}
/* Comparison operator */
Bool NatafEllipticalDistributionEvaluation::operator ==(const NatafEllipticalDistributionEvaluation &) const
{
  return true;
}
Bool NatafEllipticalDistributionEvaluation::equals(const EvaluationImplementation & other) const
{
  const NatafEllipticalDistributionEvaluation * p_other = dynamic_cast<const NatafEllipticalDistributionEvaluation *>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String NatafEllipticalDistributionEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << NatafEllipticalDistributionEvaluation::GetClassName()
      << " mean=" << getCenter()
      << " inverseCholesky=" << getLinear();

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
  Matrix linear(getLinear().transpose()); // \Gamma^t, transposed inverse Cholesky
  UnsignedInteger inputDimension = getInputDimension();
  Matrix result(2 * inputDimension, inputDimension);
  // dT_j/dmu_i
  for (UnsignedInteger i = 0; i < inputDimension; ++i)
    for (UnsignedInteger j = i; j < inputDimension; ++j) result(i, j) = -linear(i, j);
  // dTj/dsigma_j
  for (UnsignedInteger i = 0; i < inputDimension; ++i) result(inputDimension + i, i) = -linear(i, i) * linear(i, i) * (inP[i] - center[i]);
  return result;
}

/* Parameters value accessor, as (mu, sigma).
 * The recorded values are returned if any, else they are recovered from the
 * stored map. The recovery is exact for uncorrelated maps; for correlated
 * maps the true marginal scales cannot be recovered from the map alone, in
 * which case the conditional standard deviations are returned instead.
 * The dependence parameters are intentionally excluded, as the Nataf
 * derivative with respect to them is not implemented.
 */
Point NatafEllipticalDistributionEvaluation::getParameter() const
{
  const Point recorded(EvaluationImplementation::getParameter());
  if (recorded.getDimension() > 0) return recorded;
  const UnsignedInteger dimension = getInputDimension();
  const Point center(getCenter());
  const Matrix linear(getLinear());
  Point parameter(2 * dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    parameter[i] = center[i];
    parameter[dimension + i] = (linear(i, i) != 0.0 ? 1.0 / linear(i, i) : 0.0);
  }
  return parameter;
}

void NatafEllipticalDistributionEvaluation::setParameter(const Point & parameter)
{
  const UnsignedInteger dimension = getInputDimension();
  if (parameter.getDimension() != 2 * dimension) throw InvalidArgumentException(HERE) << "Required parameter of dimension " << 2 * dimension << ", provided " << parameter.getDimension();
  Point newMean(dimension);
  Point newSigma(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    newMean[i] = parameter[i];
    newSigma[i] = parameter[dimension + i];
    if (!(newSigma[i] >= 0.0)) throw InvalidArgumentException(HERE) << "The sigma parameter must be non-negative, got sigma[" << i << "]=" << newSigma[i];
  }
  // Update the inverse Cholesky factor Gamma from the new sigma values.
  // Gamma writes Gamma = C^{-1} D^{-1} with D = diag(sigma) and C^{-1} the
  // inverse Cholesky factor of the correlation matrix, which does not depend
  // on sigma, so each column j is rescaled by sigma_old[j] / sigma_new[j],
  // with the old values read on the recorded parameter vector.
  Matrix newLinear(getLinear());
  const Point recorded(EvaluationImplementation::getParameter());
  if (recorded.getDimension() > 0)
  {
    if (recorded.getDimension() != 2 * dimension) throw InvalidArgumentException(HERE) << "Recorded parameter of dimension " << recorded.getDimension() << " is inconsistent with dimension " << dimension;
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      const Scalar oldSigma = recorded[dimension + j];
      Scalar factor = 1.0;
      if (newSigma[j] != oldSigma)
      {
        if (!(oldSigma > 0.0)) throw InvalidArgumentException(HERE) << "Cannot update a degenerate transformation, please rebuild it";
        if (!(newSigma[j] > 0.0)) throw InvalidArgumentException(HERE) << "Cannot update to a degenerate transformation, please rebuild it";
        factor = oldSigma / newSigma[j];
      }
      for (UnsignedInteger i = 0; i < dimension; ++i)
        newLinear(i, j) = newLinear(i, j) * factor;
    }
  }
  else
  {
    // No recorded scales (direct construction): the true marginal scales
    // cannot be recovered from a correlated map, so an explicit record is
    // required. For a diagonal map the recovery is exact and the map is
    // rescaled from the recovered scales.
    Bool diagonal = true;
    for (UnsignedInteger i = 0; i < dimension; ++i)
      for (UnsignedInteger j = 0; j < dimension; ++j)
        if ((i != j) && (newLinear(i, j) != 0.0)) diagonal = false;
    if (!diagonal) throw InvalidArgumentException(HERE) << "Cannot update the parameters of a directly constructed correlated transformation without recorded scales, please rebuild it from a distribution";
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      const Scalar oldSigma = (newLinear(j, j) != 0.0 ? 1.0 / newLinear(j, j) : 0.0);
      if (newSigma[j] != oldSigma)
      {
        if (!(oldSigma > 0.0)) throw InvalidArgumentException(HERE) << "Cannot update a degenerate transformation, please rebuild it";
        if (!(newSigma[j] > 0.0)) throw InvalidArgumentException(HERE) << "Cannot update to a degenerate transformation, please rebuild it";
        const Scalar factor = oldSigma / newSigma[j];
        for (UnsignedInteger i = 0; i < dimension; ++i)
          newLinear(i, j) = newLinear(i, j) * factor;
      }
    }
  }
  setCenter(newMean);
  setLinear(newLinear);
  EvaluationImplementation::setParameter(parameter);
}

/* Parameters description accessor */
Description NatafEllipticalDistributionEvaluation::getParameterDescription() const
{
  if (parameterDescription_.getSize() > 0) return parameterDescription_;
  const UnsignedInteger dimension = getInputDimension();
  Description description(Description::BuildDefault(dimension, "mu_"));
  description.add(Description::BuildDefault(dimension, "sigma_"));
  return description;
}

/* Accessor for parameters dimension */
UnsignedInteger NatafEllipticalDistributionEvaluation::getParameterDimension() const
{
  return 2 * getInputDimension();
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
