//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformation evaluation for elliptical
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/InverseRosenblattEvaluation.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(InverseRosenblattEvaluation);


static const Factory<InverseRosenblattEvaluation> Factory_InverseRosenblattEvaluation;

/* Default constructor */
InverseRosenblattEvaluation::InverseRosenblattEvaluation()
  : NumericalMathEvaluationImplementation()
  , distribution_()
{
  // Nothing to do
}

/* Parameter constructor */
InverseRosenblattEvaluation::InverseRosenblattEvaluation(const Distribution & distribution)
  : NumericalMathEvaluationImplementation()
  , distribution_(distribution)
{
  Description description(Description::BuildDefault(distribution.getDimension(), "X"));
  description.add(distribution.getDescription());
  setDescription(description);
}

/* Virtual constructor */
InverseRosenblattEvaluation * InverseRosenblattEvaluation::clone() const
{
  return new InverseRosenblattEvaluation(*this);
}

/* Evaluation */
NumericalPoint InverseRosenblattEvaluation::operator () (const NumericalPoint & inP) const
{
  const UnsignedInteger dimension(getOutputDimension());
  if (inP.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << dimension << ", got dimension=" << inP.getDimension();
  NumericalPoint result(dimension);
  NumericalPoint y(0);
  // Apply conditional Quantile o Phi over the components
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    result[i] = distribution_.computeConditionalQuantile(DistFunc::pNormal(inP[i]), y);
    y.add(result[i]);
  }
  ++callsNumber_;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}

/*
 * Gradient according to the marginal parameters.
 * F(Q(y, p), p) = Id
 * F:RxRp->R
 * Q:RxRp->R
 * dF/dp = dF/dx(Q(y, p), p) . dQ/dp(y, p) + dF/dp(Q(y, p), p)
 *       = f(Q(y, p), p) . dQ/dp(y, p) + dF/dp(Q(y, p), p)
 *       = 0
 * => dQ/dp(y, p) = - dF/dp(Q(y, p), p) / f(Q(y, p), p)
 */
Matrix InverseRosenblattEvaluation::parameterGradient(const NumericalPoint & inP) const
{
  throw NotYetImplementedException(HERE) << "In InverseRosenblattEvaluation::parameterGradient(const NumericalPoint & inP) const";
}

/* Accessor for input point dimension */
UnsignedInteger InverseRosenblattEvaluation::getInputDimension() const
{
  return distribution_.getDimension();
}

/* Accessor for output point dimension */
UnsignedInteger InverseRosenblattEvaluation::getOutputDimension() const
{
  return distribution_.getDimension();
}

/* String converter */
String InverseRosenblattEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << InverseRosenblattEvaluation::GetClassName()
      << " description=" << getDescription()
      << " distribution=" << distribution_;
  return oss;
}

String InverseRosenblattEvaluation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << InverseRosenblattEvaluation::GetClassName()
      << "(Normal(" << distribution_.getDimension() << ")->"
      << distribution_ << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void InverseRosenblattEvaluation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute( "distribution_", distribution_ );
}

/* Method load() reloads the object from the StorageManager */
void InverseRosenblattEvaluation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute( "distribution_", distribution_ );
}

END_NAMESPACE_OPENTURNS

