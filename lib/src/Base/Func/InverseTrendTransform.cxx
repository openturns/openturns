//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for inverse trend function
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
#include "openturns/InverseTrendTransform.hxx"
#include "openturns/InverseTrendEvaluation.hxx"
#include "openturns/TrendTransform.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(InverseTrendTransform)

static const Factory<InverseTrendTransform> Factory_InverseTrendTransform;

/* Default constructor */
InverseTrendTransform::InverseTrendTransform()
  : VertexValueFunction()
{
  // Nothing to do
}

/* Parameter constructor */
InverseTrendTransform::InverseTrendTransform(const Function & function, const Mesh & mesh)
  : VertexValueFunction(InverseTrendEvaluation(function), mesh)
{
  // Nothing to do
}

/* Parameter constructor */
InverseTrendTransform::InverseTrendTransform(const Evaluation & evaluation, const Mesh & mesh)
  : VertexValueFunction(InverseTrendEvaluation(Function(evaluation)), mesh)
{
  // Nothing to do
}

/* Parameter constructor */
InverseTrendTransform::InverseTrendTransform(const EvaluationImplementation & evaluation, const Mesh & mesh)
  : VertexValueFunction(InverseTrendEvaluation(Function(evaluation)), mesh)
{
  // Nothing to do
}

/* Virtual constructor */
InverseTrendTransform * InverseTrendTransform::clone() const
{
  return new InverseTrendTransform(*this);
}

/* String converter */
String InverseTrendTransform::__repr__() const
{
  OSS oss(true);
  oss << "class=" << TrendTransform::GetClassName()
      << " inherited from " << VertexValueFunction::__repr__();
  return oss;
}

/* Inverse accessor */
TrendTransform InverseTrendTransform::getInverse() const
{
  return TrendTransform(getTrendFunction(), getInputMesh());
}

/* Underlying trend function accessor */
Function InverseTrendTransform::getTrendFunction() const
{
  const InverseTrendEvaluation * p_evaluation = dynamic_cast<const InverseTrendEvaluation*>(getFunction().getEvaluation().getImplementation().get());
  if (p_evaluation) return p_evaluation->getFunction();
  throw InternalException(HERE) << "Error: cannot extract the evaluation of the internal function as an InverseTrendEvaluation";
}

/* Method save() stores the object through the StorageManager */
void InverseTrendTransform::save(Advocate & adv) const
{
  VertexValueFunction::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void InverseTrendTransform::load(Advocate & adv)
{
  VertexValueFunction::load(adv);
}

END_NAMESPACE_OPENTURNS
