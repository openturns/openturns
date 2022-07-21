//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for the trend transform
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/TrendTransform.hxx"
#include "openturns/TrendEvaluation.hxx"
#include "openturns/InverseTrendTransform.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TrendTransform)

static const Factory<TrendTransform> Factory_TrendTransform;

/* Default constructor */
TrendTransform::TrendTransform()
  : VertexValueFunction()
{
  // Nothing to do
}

/* Parameter constructor */
TrendTransform::TrendTransform(const Function & function, const Mesh & mesh)
  : VertexValueFunction(TrendEvaluation(function), mesh)
{
  // Nothing to do
}

/* Parameter constructor */
TrendTransform::TrendTransform(const Evaluation & evaluation, const Mesh & mesh)
  : VertexValueFunction(TrendEvaluation(Function(evaluation)), mesh)
{
  // Nothing to do
}

/* Parameter constructor */
TrendTransform::TrendTransform(const EvaluationImplementation & evaluation, const Mesh & mesh)
  : VertexValueFunction(TrendEvaluation(evaluation), mesh)
{
  // Nothing to do
}

/* Virtual constructor */
TrendTransform * TrendTransform::clone() const
{
  return new TrendTransform(*this);
}

/* Comparison operator */
Bool TrendTransform::operator ==(const TrendTransform & other) const
{
  if (this == &other) return true;
  return VertexValueFunction::operator==(other);
}

/* String converter */
String TrendTransform::__repr__() const
{
  OSS oss(true);
  oss << "class=" << TrendTransform::GetClassName()
      << " inherited from " << VertexValueFunction::__repr__();
  return oss;
}

/* String converter */
String TrendTransform::__str__(const String & offset) const
{
  return VertexValueFunction::__str__(offset);
}

/* Inverse accessor */
InverseTrendTransform TrendTransform::getInverse() const
{
  return InverseTrendTransform(getTrendFunction(), getInputMesh());
}

/* Underlying trend function accessor */
Function TrendTransform::getTrendFunction() const
{
  const TrendEvaluation * p_evaluation = dynamic_cast<const TrendEvaluation*>(getFunction().getEvaluation().getImplementation().get());
  if (p_evaluation) return p_evaluation->getFunction();
  throw InternalException(HERE) << "Error: cannot extract the evaluation of the internal function as a TrendEvaluation";
}

/* Method save() stores the object through the StorageManager */
void TrendTransform::save(Advocate & adv) const
{
  VertexValueFunction::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void TrendTransform::load(Advocate & adv)
{
  VertexValueFunction::load(adv);
}

END_NAMESPACE_OPENTURNS
