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
 * marginal standard deviations \sigma. With Gamma the inverse Cholesky
 * factor of the covariance matrix, Gamma = C^{-1} D^{-1} where D is
 * diagonal with the marginal scales and C^{-1} the inverse Cholesky
 * factor of the correlation matrix, the expression of the transformation is:
 *
 * T_i(X) = \sum_{j} \Gamma_{ij}(X_j - mu_j)
 *
 * thus we have:
 *
 * dT_i/dmu_j = -Gamma_{ij}
 * dT_i/dsigma_j = -Gamma_{ij}(X_j - mu_j) / sigma_j
 *
 * The (i,j)th element of the gradient is dT_j/dtheta_i.
 *
 */
Matrix NatafEllipticalDistributionEvaluation::parameterGradient(const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << inputDimension << ", got dimension=" << inP.getDimension();
  Point center(getCenter()); // \mu
  const Matrix gamma(getLinear()); // \Gamma, inverse Cholesky factor
  const Matrix linear(gamma.transpose()); // \Gamma^t, transposed inverse Cholesky
  Matrix result(2 * inputDimension, inputDimension);
  // dT_j/dmu_i
  for (UnsignedInteger i = 0; i < inputDimension; ++i)
    for (UnsignedInteger j = i; j < inputDimension; ++j) result(i, j) = -linear(i, j);
  // True marginal scales, recorded at construction when available.
  // Without record the recovery below is exact for diagonal maps only.
  const Point recorded(EvaluationImplementation::getParameter());
  Point sigma(inputDimension);
  if (recorded.getDimension() > 0)
  {
    for (UnsignedInteger j = 0; j < inputDimension; ++j) sigma[j] = recorded[inputDimension + j];
  }
  else
  {
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
      for (UnsignedInteger j = 0; j < inputDimension; ++j)
        if ((i != j) && (gamma(i, j) != 0.0)) throw InvalidArgumentException(HERE) << "Cannot compute the parameter gradient of a directly constructed correlated transformation without recorded scales, please rebuild it from a distribution";
    for (UnsignedInteger j = 0; j < inputDimension; ++j) sigma[j] = (gamma(j, j) != 0.0 ? 1.0 / gamma(j, j) : 0.0);
  }
  // dT_i/dsigma_j, only the j-th input component matters
  for (UnsignedInteger j = 0; j < inputDimension; ++j)
  {
    if (!(sigma[j] > 0.0)) continue;
    const Scalar factor = -(inP[j] - center[j]) / sigma[j];
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
      result(inputDimension + j, i) = gamma(i, j) * factor;
  }
  return result;
}

/* Parameters value accessor, as (mu, sigma).
 * The recorded values are returned if any, else they are recovered from the
 * stored map. The recovery is exact for uncorrelated maps; for correlated
 * maps the true marginal scales cannot be recovered from the map alone, in
 * which case an exception is thrown instead of returning the conditional
 * standard deviations, consistently with setParameter()/parameterGradient().
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
  for (UnsignedInteger i = 0; i < dimension; ++i)
    for (UnsignedInteger j = 0; j < dimension; ++j)
      if ((i != j) && (linear(i, j) != 0.0)) throw InvalidArgumentException(HERE) << "Cannot recover the parameters of a directly constructed correlated transformation without recorded scales, please rebuild it from a distribution";
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
