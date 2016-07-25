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
#include "openturns/RosenblattEvaluation.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(RosenblattEvaluation);


static const Factory<RosenblattEvaluation> Factory_RosenblattEvaluation;

/* Default constructor */
RosenblattEvaluation::RosenblattEvaluation()
  : NumericalMathEvaluationImplementation()
  , distribution_()
{
  // Nothing to do
}

/* Parameter constructor */
RosenblattEvaluation::RosenblattEvaluation(const Distribution & distribution)
  : NumericalMathEvaluationImplementation()
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
NumericalPoint RosenblattEvaluation::operator () (const NumericalPoint & inP) const
{
  const UnsignedInteger dimension(getOutputDimension());
  NumericalPoint result(dimension);
  NumericalPoint y(0);
  // Apply Phi^{-1} o conditional CDF over the components
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const NumericalScalar conditionalCDF(distribution_.computeConditionalCDF(inP[i], y));
    result[i] = DistFunc::qNormal(conditionalCDF);
    y.add(inP[i]);
  }
  ++callsNumber_;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}

/* Gradient according to the marginal parameters. */
Matrix RosenblattEvaluation::parameterGradient(const NumericalPoint & inP) const
{
  throw NotYetImplementedException(HERE) << "In RosenblattEvaluation::parameterGradient(const NumericalPoint & inP) const";
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

/* Method save() stores the object through the StorageManager */
void RosenblattEvaluation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute( "distribution_", distribution_ );
}

/* Method load() reloads the object from the StorageManager */
void RosenblattEvaluation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute( "distribution_", distribution_ );
}

END_NAMESPACE_OPENTURNS

