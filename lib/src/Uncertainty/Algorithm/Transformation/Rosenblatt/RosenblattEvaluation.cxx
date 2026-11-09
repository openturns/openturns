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
#include "openturns/RosenblattEvaluation.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include <cmath>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(RosenblattEvaluation)


static const Factory<RosenblattEvaluation> Factory_RosenblattEvaluation;

/* Default constructor */
RosenblattEvaluation::RosenblattEvaluation()
  : EvaluationImplementation()
  , distribution_()
{
  // Nothing to do
}

/* Parameter constructor */
RosenblattEvaluation::RosenblattEvaluation(const Distribution & distribution)
  : EvaluationImplementation()
  , distribution_(distribution)
{
  Description description(distribution.getDescription());
  description.add(Description::BuildDefault(distribution.getDimension(), "Y"));
  setDescription(description);
}

/* Virtual constructor */
RosenblattEvaluation * RosenblattEvaluation::clone() const
{
  return new RosenblattEvaluation(*this);
}
/* Comparison operator */
Bool RosenblattEvaluation::operator ==(const RosenblattEvaluation & other) const
{
  if (this == &other) return true;
  return (distribution_ == other.distribution_);
}
Bool RosenblattEvaluation::equals(const EvaluationImplementation & other) const
{
  const RosenblattEvaluation * p_other = dynamic_cast<const RosenblattEvaluation *>(&other);
  return p_other && (*this == *p_other);
}

/* Evaluation */
Point RosenblattEvaluation::operator () (const Point & inP) const
{
  const UnsignedInteger dimension = getInputDimension();
  if (inP.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << dimension << ", got dimension=" << inP.getDimension();
  callsNumber_.increment();
  const Point q(distribution_.computeSequentialConditionalCDF(inP));
  const Point y(DistFunc::qNormal(q));
  return y;
}

/* Gradient according to the marginal parameters.
 * No analytical derivative of the Rosenblatt map with respect to the
 * distribution parameters is available, so it is computed numerically
 * by central finite differences.
 */
Matrix RosenblattEvaluation::parameterGradient(const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << inputDimension << ", got dimension=" << inP.getDimension();
  // No per-parameter continuity information is available: each parameter is
  // perturbed in turn, and a parameter that cannot be perturbed in either
  // direction (e.g. an integer parameter) raises NotYetImplemented below
  const Point parameter(distribution_.getParameter());
  const UnsignedInteger parameterDimension = parameter.getDimension();
  const UnsignedInteger outputDimension = getOutputDimension();
  Matrix gradient(parameterDimension, outputDimension);
  if (parameterDimension == 0) return gradient;
  const Scalar epsilon = ResourceMap::GetAsScalar("Evaluation-ParameterEpsilon");
  const Point nominalValue(operator()(inP));
  for (UnsignedInteger i = 0; i < parameterDimension; ++i)
  {
    const Scalar step = epsilon * (1.0 + std::abs(parameter[i]));
    // Only an invalid perturbed parameter value falls back to a one-sided
    // difference: evaluation errors propagate instead of being hidden behind
    // a degraded gradient
    RosenblattEvaluation evaluationPlus(*this);
    Point parameterPlus(parameter);
    parameterPlus[i] += step;
    Bool plusOk = true;
    try
    {
      evaluationPlus.distribution_.setParameter(parameterPlus);
    }
    catch (const InvalidArgumentException &)
    {
      plusOk = false;
    }
    const Point valuePlus(plusOk ? evaluationPlus.operator()(inP) : nominalValue);
    RosenblattEvaluation evaluationMinus(*this);
    Point parameterMinus(parameter);
    parameterMinus[i] -= step;
    Bool minusOk = true;
    try
    {
      evaluationMinus.distribution_.setParameter(parameterMinus);
    }
    catch (const InvalidArgumentException &)
    {
      minusOk = false;
    }
    const Point valueMinus(minusOk ? evaluationMinus.operator()(inP) : nominalValue);
    if (plusOk && minusOk)
    {
      for (UnsignedInteger j = 0; j < outputDimension; ++j)
        gradient(i, j) = (valuePlus[j] - valueMinus[j]) / (2.0 * step);
    }
    else if (plusOk)
    {
      for (UnsignedInteger j = 0; j < outputDimension; ++j)
        gradient(i, j) = (valuePlus[j] - nominalValue[j]) / step;
    }
    else if (minusOk)
    {
      for (UnsignedInteger j = 0; j < outputDimension; ++j)
        gradient(i, j) = (nominalValue[j] - valueMinus[j]) / step;
    }
    else throw NotYetImplementedException(HERE) << "In RosenblattEvaluation::parameterGradient: cannot perturb parameter " << i << " in either direction";
  }
  return gradient;
}

/* Parameters value accessor */
Point RosenblattEvaluation::getParameter() const
{
  return distribution_.getParameter();
}

void RosenblattEvaluation::setParameter(const Point & parameter)
{
  distribution_.setParameter(parameter);
}

UnsignedInteger RosenblattEvaluation::getParameterDimension() const
{
  return distribution_.getParameterDimension();
}

/* Parameters description accessor */
Description RosenblattEvaluation::getParameterDescription() const
{
  // Keep the description consistent with the parameter vector: fall back to
  // default names when the distribution provides none (custom distributions
  // may throw NotYetImplementedException and the Python wrapper returns an
  // empty description), and fail fast on a short non-empty description
  const UnsignedInteger dimension = distribution_.getParameterDimension();
  Description description;
  try { description = distribution_.getParameterDescription(); }
  catch (const NotYetImplementedException &) { /* fall through to the default below */ }
  if (description.getSize() != dimension)
  {
    if (description.getSize() != 0) throw InvalidArgumentException(HERE) << "Error: the parameter description of dimension " << description.getSize() << " does not match the parameter dimension " << dimension;
    description = Description::BuildDefault(dimension, "p");
  }
  return description;
}

/* Accessor for input point dimension */
UnsignedInteger RosenblattEvaluation::getInputDimension() const
{
  return distribution_.getDimension();
}

/* Accessor for output point dimension */
UnsignedInteger RosenblattEvaluation::getOutputDimension() const
{
  return distribution_.getDimension();
}

/* Is it safe to call in parallel? */
Bool RosenblattEvaluation::isParallel() const
{
  return distribution_.getImplementation()->isParallel();
}

/* String converter */
String RosenblattEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << RosenblattEvaluation::GetClassName()
      << " description=" << getDescription()
      << " distribution=" << distribution_;
  return oss;
}

String RosenblattEvaluation::__str__(const String & ) const
{
  OSS oss(false);
  oss << RosenblattEvaluation::GetClassName()
      << "(" << distribution_
      << "->Normal(" << distribution_.getDimension() << "))";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void RosenblattEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "distribution_", distribution_ );
}

/* Method load() reloads the object from the StorageManager */
void RosenblattEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "distribution_", distribution_ );
}

END_NAMESPACE_OPENTURNS

