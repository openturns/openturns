//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformation evaluation for elliptical
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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

/* Evaluation */
Point RosenblattEvaluation::operator () (const Point & inP) const
{
  const UnsignedInteger dimension = getOutputDimension();
  if (inP.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << dimension << ", got dimension=" << inP.getDimension();
  callsNumber_.increment();
  const Point q(distribution_.computeSequentialConditionalCDF(inP));
  const Point y(DistFunc::qNormal(q));
  return y;
}

/* Gradient according to the marginal parameters. */
Matrix RosenblattEvaluation::parameterGradient(const Point & ) const
{
  Matrix grad(0, getOutputDimension());
  return grad;
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
      << "->Normal(" << distribution_.getDimension() << ")";
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

