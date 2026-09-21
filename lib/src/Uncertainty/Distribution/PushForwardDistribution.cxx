//                                               -*- C++
/**
 *  @brief The PushForwardDistribution distribution
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
#include "openturns/PushForwardDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ConstantFunction.hxx"
#include "openturns/IdentityFunction.hxx"
#include "openturns/LeastSquaresEquationsSolver.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Sample.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/Uniform.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PushForwardDistribution)

static const Factory<PushForwardDistribution> Factory_PushForwardDistribution;

/* Default constructor */
PushForwardDistribution::PushForwardDistribution()
  : DistributionImplementation()
  , function_(IdentityFunction(1))
  , antecedent_(Uniform(0.0, 1.0))
  , solver_(LeastSquaresEquationsSolver(ResourceMap::GetAsScalar("Solver-DefaultAbsoluteError"),
                                        ResourceMap::GetAsScalar("Solver-DefaultRelativeError"),
                                        ResourceMap::GetAsScalar("PushForwardDistribution-SolverResidualTolerance"),
                                        ResourceMap::GetAsUnsignedInteger("Solver-DefaultMaximumFunctionEvaluation")))
{
  setFunctionAndAntecedent(function_, antecedent_);
}

/* Parameters constructor */
PushForwardDistribution::PushForwardDistribution(const Function & function,
    const Distribution & antecedent)
  : DistributionImplementation()
  , function_()
  , antecedent_()
  , solver_(LeastSquaresEquationsSolver(ResourceMap::GetAsScalar("Solver-DefaultAbsoluteError"),
                                        ResourceMap::GetAsScalar("Solver-DefaultRelativeError"),
                                        ResourceMap::GetAsScalar("PushForwardDistribution-SolverResidualTolerance"),
                                        ResourceMap::GetAsUnsignedInteger("Solver-DefaultMaximumFunctionEvaluation")))
{
  setFunctionAndAntecedent(function, antecedent);
}

/* Comparison operator */
Bool PushForwardDistribution::operator ==(const PushForwardDistribution & other) const
{
  if (this == &other) return true;
  return (function_ == other.function_) && (antecedent_ == other.antecedent_);
}

Bool PushForwardDistribution::equals(const DistributionImplementation & other) const
{
  const PushForwardDistribution* p_other = dynamic_cast<const PushForwardDistribution*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String PushForwardDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << PushForwardDistribution::GetClassName()
      << " name=" << getName()
      << " function=" << function_
      << " antecedent=" << antecedent_
      << " solver=" << solver_;
  return oss;
}

String PushForwardDistribution::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "=f(" << antecedent_.__str__() << ") with f=" << function_.getEvaluation().__str__();
  return oss;
}

/* Virtual constructor */
PushForwardDistribution * PushForwardDistribution::clone() const
{
  return new PushForwardDistribution(*this);
}

/* Set the function and antecedent with check */
void PushForwardDistribution::setFunctionAndAntecedent(const Function & function,
    const Distribution & antecedent)
{
  const UnsignedInteger inputDimension = function.getInputDimension();
  const UnsignedInteger outputDimension = function.getOutputDimension();
  if (inputDimension != antecedent.getDimension())
    throw InvalidArgumentException(HERE) << "Error: the function must have an input dimension equal to the dimension of the antecedent, here input dimension=" << inputDimension << " and dimension of the antecedent=" << antecedent.getDimension();
  if (!antecedent.isContinuous())
    throw InvalidArgumentException(HERE) << "Error: the antecedent must be absolutely continuous, here isContinuous()=" << antecedent.isContinuous();
  if (outputDimension < inputDimension)
    throw NotYetImplementedException(HERE) << "Error: the case where the output dimension of the function is lower than the input dimension (co-area formula) is not yet implemented, here input dimension=" << inputDimension << " and output dimension=" << outputDimension;
  function_ = function;
  antecedent_ = antecedent;
  setDimension(outputDimension);
  // We don't know if the function is thread-safe and it could be called in parallel in computePDF()
  setParallel(false);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computeRange();
}

/* Function accessors */
void PushForwardDistribution::setFunction(const Function & function)
{
  setFunctionAndAntecedent(function, antecedent_);
}

Function PushForwardDistribution::getFunction() const
{
  return function_;
}

/* Antecedent accessor */
void PushForwardDistribution::setAntecedent(const Distribution & antecedent)
{
  setFunctionAndAntecedent(function_, antecedent);
}

Distribution PushForwardDistribution::getAntecedent() const
{
  return antecedent_;
}

/* Set the solver used to invert the function */
void PushForwardDistribution::setSolver(const Solver & solver)
{
  solver_ = solver;
}

Solver PushForwardDistribution::getSolver() const
{
  return solver_;
}

/* Get one realization of the distribution */
Point PushForwardDistribution::getRealization() const
{
  return function_(antecedent_.getRealization());
}

/* Compute the numerical range of the distribution given the parameters values */
void PushForwardDistribution::computeRange()
{
  const UnsignedInteger outputDimension = function_.getOutputDimension();
  const UnsignedInteger sampleSize = ResourceMap::GetAsUnsignedInteger("PushForwardDistribution-SampleSize");
  const Sample sample(function_(antecedent_.getSample(sampleSize)));
  const Point lowerBound(sample.getMin());
  const Point upperBound(sample.getMax());
  Point marginedLowerBound(outputDimension);
  Point marginedUpperBound(outputDimension);
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
  {
    // Add a margin of 5% of each marginal span to avoid rejecting points
    // lying on the edge of the support
    const Scalar halfSpan = 0.025 * (upperBound[i] - lowerBound[i]);
    marginedLowerBound[i] = lowerBound[i] - halfSpan;
    marginedUpperBound[i] = upperBound[i] + halfSpan;
  }
  const Interval::BoolCollection finiteLowerBound(outputDimension, true);
  const Interval::BoolCollection finiteUpperBound(outputDimension, true);
  setRange(Interval(marginedLowerBound, marginedUpperBound, finiteLowerBound, finiteUpperBound));
}

Scalar PushForwardDistribution::computePDF(const Point & point) const
{
  const UnsignedInteger inputDimension = antecedent_.getDimension();
  const UnsignedInteger outputDimension = function_.getOutputDimension();
  if (point.getDimension() != outputDimension)
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << outputDimension << ", here dimension=" << point.getDimension();
  // Build the residual function r(u) = f(u) - point and look for a preimage
  // u such that r(u) = 0 using a least squares solver.
  // Only one preimage is searched for, starting from the mean of the
  // antecedent. This is fragile for non-monotone functions with several
  // preimages. A Newton solver with step control is planned as an
  // improvement.
  const Function residualFunction(function_ - ConstantFunction(inputDimension, point));
  const Interval antecedentRange(antecedent_.getRange());
  const Interval::BoolCollection finiteLowerBound(antecedentRange.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpperBound(antecedentRange.getFiniteUpperBound());
  Bool hasFiniteBounds = true;
  for (UnsignedInteger i = 0; i < inputDimension; ++i)
    hasFiniteBounds = hasFiniteBounds && finiteLowerBound[i] && finiteUpperBound[i];
  Point preimage(0);
  try
  {
    if (hasFiniteBounds)
      preimage = solver_.solve(residualFunction, antecedent_.getMean(), antecedentRange);
    else
      preimage = solver_.solve(residualFunction, antecedent_.getMean());
  }
  catch (const InternalException &)
  {
    // No preimage found within the tolerance: the point is not in the support
    return 0.0;
  }
  // Check that the solved point is an actual preimage
  const Scalar residualNorm = (function_(preimage) - point).norm();
  if (residualNorm > ResourceMap::GetAsScalar("PushForwardDistribution-SolverResidualTolerance"))
    return 0.0;
  if (!antecedentRange.contains(preimage))
    return 0.0;
  // Change of variables formula
  const Scalar numerator = antecedent_.computePDF(preimage);
  if (!(numerator > 0.0))
    return 0.0;
  const Matrix gradientMatrix(function_.gradient(preimage));
  if ((gradientMatrix.getNbRows() != inputDimension) || (gradientMatrix.getNbColumns() != outputDimension))
    throw InternalException(HERE) << "Error: the given function has no actual gradient. Consider using finite differences.";
  // In the square case the denominator is |det(J)|, in the intrinsic case
  // it is sqrt(det(J^T J)) where J is the Jacobian matrix of the function.
  // The gradient matrix returned by the function is the transpose of J.
  Scalar determinant = 0.0;
  if (outputDimension == inputDimension)
  {
    const SquareMatrix jacobianSquareMatrix(*gradientMatrix.getImplementation());
    determinant = jacobianSquareMatrix.computeDeterminant();
  }
  else
  {
    // Here J^T J = gradient * gradient^T
    SquareMatrix gramMatrix(inputDimension);
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
      for (UnsignedInteger j = 0; j < inputDimension; ++j)
      {
        Scalar value = 0.0;
        for (UnsignedInteger k = 0; k < outputDimension; ++k)
          value += gradientMatrix(i, k) * gradientMatrix(j, k);
        gramMatrix(i, j) = value;
      }
    determinant = gramMatrix.computeDeterminant();
  }
  const Scalar denominator = (outputDimension == inputDimension) ? std::abs(determinant) : std::sqrt(determinant);
  if (!std::isfinite(denominator) || !(denominator > 0.0))
    return 0.0;
  const Scalar pdf = numerator / denominator;
  return pdf;
}

/* Parameters value and description accessor */
PushForwardDistribution::PointWithDescriptionCollection PushForwardDistribution::getParametersCollection() const
{
  return antecedent_.getParametersCollection();
}

void PushForwardDistribution::setParametersCollection(const PointCollection & parametersCollection)
{
  antecedent_.setParametersCollection(parametersCollection);
  setFunctionAndAntecedent(function_, antecedent_);
}

/* Parameters value accessors */
void PushForwardDistribution::setParameter(const Point & parameter)
{
  antecedent_.setParameter(parameter);
  setFunctionAndAntecedent(function_, antecedent_);
}

Point PushForwardDistribution::getParameter() const
{
  return antecedent_.getParameter();
}

/* Parameters description accessor */
Description PushForwardDistribution::getParameterDescription() const
{
  return antecedent_.getParameterDescription();
}

/* Tell if the distribution is continuous */
Bool PushForwardDistribution::isContinuous() const
{
  return antecedent_.isContinuous();
}

/* Tell if the distribution is discrete */
Bool PushForwardDistribution::isDiscrete() const
{
  return antecedent_.isDiscrete();
}

/* Compute the mean of the distribution */
void PushForwardDistribution::computeMean() const
{
  const UnsignedInteger sampleSize = ResourceMap::GetAsUnsignedInteger("PushForwardDistribution-SampleSize");
  const Sample sample(function_(antecedent_.getSample(sampleSize)));
  mean_ = sample.computeMean();
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void PushForwardDistribution::computeCovariance() const
{
  const UnsignedInteger sampleSize = ResourceMap::GetAsUnsignedInteger("PushForwardDistribution-SampleSize");
  const Sample sample(function_(antecedent_.getSample(sampleSize)));
  covariance_ = sample.computeCovariance();
  isAlreadyComputedCovariance_ = true;
}

/* Method save() stores the object through the StorageManager */
void PushForwardDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute("function_", function_);
  adv.saveAttribute("antecedent_", antecedent_);
  adv.saveAttribute("solver_", solver_);
}

/* Method load() reloads the object from the StorageManager */
void PushForwardDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute("function_", function_);
  adv.loadAttribute("antecedent_", antecedent_);
  adv.loadAttribute("solver_", solver_);
}

END_NAMESPACE_OPENTURNS