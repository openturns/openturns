//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformation evaluation for elliptical
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
  Point result(dimension);
  Point y(0);
  // Apply Phi^{-1} o conditional CDF over the components
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar conditionalCDF = distribution_.computeConditionalCDF(inP[i], y);
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

/* Evaluation */
Sample RosenblattEvaluation::operator () (const Sample & inSample) const
{
  const UnsignedInteger dimension = getOutputDimension();
  const UnsignedInteger size = inSample.getSize();
  if (inSample.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << dimension << ", got dimension=" << inSample.getDimension();
  Sample result(size, dimension);
  SampleImplementation & resultImpl(*result.getImplementation());
  Sample y(size, 0);
  // Apply Phi^{-1} o conditional CDF over the components
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Sample inSampleMarginal(inSample.getMarginal(i));
    const Point conditionalCDF(distribution_.computeConditionalCDF(inSampleMarginal.asPoint(), y));
    for (UnsignedInteger j = 0; j < size; ++j)
      resultImpl(j, i) = DistFunc::qNormal(conditionalCDF[j]);
    y.stack(inSampleMarginal);
  }
  result.setDescription(getOutputDescription());
  callsNumber_ += size;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inSample);
    outputStrategy_.store(result);
  }
  return result;
}

/* Gradient according to the marginal parameters. */
Matrix RosenblattEvaluation::parameterGradient(const Point & inP) const
{
  throw NotYetImplementedException(HERE) << "In RosenblattEvaluation::parameterGradient(const Point & inP) const";
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

String RosenblattEvaluation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << RosenblattEvaluation::GetClassName()
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

