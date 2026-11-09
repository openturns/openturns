//                                               -*- C++ -*-
/**
 *  @brief Class for the InverseNataf transformation evaluation for elliptical
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
#include "openturns/InverseNatafEllipticalDistributionEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class InverseNatafEllipticalDistributionEvaluation
 *
 * This class offers an interface for the InverseNataf function for elliptical distributions
 */

CLASSNAMEINIT(InverseNatafEllipticalDistributionEvaluation)

static const Factory<InverseNatafEllipticalDistributionEvaluation> Factory_InverseNatafEllipticalDistributionEvaluation;

/* Default constructor */
InverseNatafEllipticalDistributionEvaluation::InverseNatafEllipticalDistributionEvaluation()
  : LinearEvaluation()
{
  // Nothing to do
}

/* Parameter constructor */
InverseNatafEllipticalDistributionEvaluation::InverseNatafEllipticalDistributionEvaluation(const Point & mean,
    const TriangularMatrix & cholesky)
  : LinearEvaluation(
      Point(mean.getDimension(), 0.0),
      mean,
      cholesky
    )
{
  // Nothing to do
}

/* Parameter constructor with recorded (mu, sigma) scales */
InverseNatafEllipticalDistributionEvaluation::InverseNatafEllipticalDistributionEvaluation(const Point & mean,
    const TriangularMatrix & cholesky,
    const Point & parameters)
  : LinearEvaluation(
      Point(mean.getDimension(), 0.0),
      mean,
      cholesky
    )
{
  EvaluationImplementation::setParameter(parameters);
}

/* Virtual constructor */
InverseNatafEllipticalDistributionEvaluation * InverseNatafEllipticalDistributionEvaluation::clone() const
{
  return new InverseNatafEllipticalDistributionEvaluation(*this);
}
/* Comparison operator */
Bool InverseNatafEllipticalDistributionEvaluation::operator ==(const InverseNatafEllipticalDistributionEvaluation &) const
{
  return true;
}
Bool InverseNatafEllipticalDistributionEvaluation::equals(const EvaluationImplementation & other) const
{
  const InverseNatafEllipticalDistributionEvaluation * p_other = dynamic_cast<const InverseNatafEllipticalDistributionEvaluation *>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String InverseNatafEllipticalDistributionEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << InverseNatafEllipticalDistributionEvaluation::GetClassName()
      << " mean=" << getCenter()
      << " cholesky=" << getLinear();

  return oss;
}

/* Gradient according to the marginal parameters.
 * For the InverseNataf transformation based on elliptical
 * distributions, the marginal parameters \theta are the mean
 * vector \mu and the marginal standard deviations \sigma. With
 * L the Cholesky factor of the covariance matrix, L = D C where D
 * is diagonal with the marginal scales and C the Cholesky factor of
 * the correlation matrix, the expression of the inverse transformation is:
 *
 * T^(-1)_i(U) = mu_i + \sum_{j} L_{ij}U_j
 *
 * thus we have:
 *
 * dT^(-1)_i/dmu_j = 1 for j=i,
 *                 = 0 otherwise
 * dT^(-1)_i/dsigma_j = (C U)_j for j=i,
 *                    = 0 otherwise
 *
 * where (C U)_j is the j-th component of the correlation-space vector.
 * The (i,j)th element of the gradient is dT^(-1)_j/dtheta_i.
 *
 */

Matrix InverseNatafEllipticalDistributionEvaluation::parameterGradient(const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << inputDimension << ", got dimension=" << inP.getDimension();
  const Matrix linear(getLinear()); // L, Cholesky factor
  Matrix result(2 * inputDimension, inputDimension);
  // dT_j/dmu_i
  for (UnsignedInteger i = 0; i < inputDimension; ++i) result(i, i) = 1.0;
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
        if ((i != j) && (linear(i, j) != 0.0)) throw InvalidArgumentException(HERE) << "Cannot compute the parameter gradient of a directly constructed correlated transformation without recorded scales, please rebuild it from a distribution";
    for (UnsignedInteger j = 0; j < inputDimension; ++j) sigma[j] = linear(j, j);
  }
  // dT_i/dsigma_j = delta_ij (C U)_j with C = D^{-1} L
  for (UnsignedInteger j = 0; j < inputDimension; ++j)
  {
    if (!(sigma[j] > 0.0)) continue;
    Scalar cu = 0.0;
    for (UnsignedInteger k = 0; k < inputDimension; ++k) cu += linear(j, k) * inP[k];
    result(inputDimension + j, j) = cu / sigma[j];
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
Point InverseNatafEllipticalDistributionEvaluation::getParameter() const
{
  const Point recorded(EvaluationImplementation::getParameter());
  if (recorded.getDimension() > 0) return recorded;
  const UnsignedInteger dimension = getInputDimension();
  const Point constant(getConstant());
  const Matrix linear(getLinear());
  for (UnsignedInteger i = 0; i < dimension; ++i)
    for (UnsignedInteger j = 0; j < dimension; ++j)
      if ((i != j) && (linear(i, j) != 0.0)) throw InvalidArgumentException(HERE) << "Cannot recover the parameters of a directly constructed correlated transformation without recorded scales, please rebuild it from a distribution";
  Point parameter(2 * dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    parameter[i] = constant[i];
    parameter[dimension + i] = linear(i, i);
  }
  return parameter;
}

void InverseNatafEllipticalDistributionEvaluation::setParameter(const Point & parameter)
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
  // Update the Cholesky factor L from the new sigma values. L writes
  // L = D L_c with D = diag(sigma) and L_c the Cholesky factor of the
  // correlation matrix, which does not depend on sigma, so each row i is
  // rescaled by sigma_new[i] / sigma_old[i], with the old values read on
  // the recorded parameter vector.
  Matrix newLinear(getLinear());
  const Point recorded(EvaluationImplementation::getParameter());
  if (recorded.getDimension() > 0)
  {
    if (recorded.getDimension() != 2 * dimension) throw InvalidArgumentException(HERE) << "Recorded parameter of dimension " << recorded.getDimension() << " is inconsistent with dimension " << dimension;
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      const Scalar oldSigma = recorded[dimension + i];
      Scalar factor = 1.0;
      if (newSigma[i] != oldSigma)
      {
        if (!(oldSigma > 0.0)) throw InvalidArgumentException(HERE) << "Cannot update a degenerate transformation, please rebuild it";
        if (!(newSigma[i] > 0.0)) throw InvalidArgumentException(HERE) << "Cannot update to a degenerate transformation, please rebuild it";
        factor = newSigma[i] / oldSigma;
      }
      for (UnsignedInteger j = 0; j < dimension; ++j)
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
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      const Scalar oldSigma = newLinear(i, i);
      if (newSigma[i] != oldSigma)
      {
        if (!(oldSigma > 0.0)) throw InvalidArgumentException(HERE) << "Cannot update a degenerate transformation, please rebuild it";
        if (!(newSigma[i] > 0.0)) throw InvalidArgumentException(HERE) << "Cannot update to a degenerate transformation, please rebuild it";
        const Scalar factor = newSigma[i] / oldSigma;
        for (UnsignedInteger j = 0; j < dimension; ++j)
          newLinear(i, j) = newLinear(i, j) * factor;
      }
    }
  }
  setConstant(newMean);
  setLinear(newLinear);
  EvaluationImplementation::setParameter(parameter);
}

/* Parameters description accessor */
Description InverseNatafEllipticalDistributionEvaluation::getParameterDescription() const
{
  if (parameterDescription_.getSize() > 0) return parameterDescription_;
  const UnsignedInteger dimension = getInputDimension();
  Description description(Description::BuildDefault(dimension, "mu_"));
  description.add(Description::BuildDefault(dimension, "sigma_"));
  return description;
}

/* Accessor for parameters dimension */
UnsignedInteger InverseNatafEllipticalDistributionEvaluation::getParameterDimension() const
{
  return 2 * getInputDimension();
}

/* Method save() stores the object through the StorageManager */
void InverseNatafEllipticalDistributionEvaluation::save(Advocate & adv) const
{
  LinearEvaluation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void InverseNatafEllipticalDistributionEvaluation::load(Advocate & adv)
{
  LinearEvaluation::load(adv);
}

END_NAMESPACE_OPENTURNS
