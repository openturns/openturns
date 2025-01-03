//                                               -*- C++ -*-
/**
 * @brief Class for identity evaluation.
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/VertexEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(VertexEvaluation)

static const Factory<VertexEvaluation> Factory_VertexEvaluation;

/* Default constructor */
VertexEvaluation::VertexEvaluation()
  : EvaluationImplementation()
  , indices_({0})
{
  // nothing to do
}

/* Parameter constructor */
VertexEvaluation::VertexEvaluation(const PointToFieldFunction & function,
                                   const Indices & indices)
  : EvaluationImplementation()
  , function_(function)
  , indices_(indices)
{
  if (indices.getSize() != 1)
    throw InvalidArgumentException(HERE) << "VertexEvaluation indices must not be empty";
  if (!indices.check(function.getOutputMesh().getVerticesNumber()))
    throw InvalidArgumentException(HERE) << "VertexEvaluation indices must not be in [0, " << function.getOutputMesh().getVerticesNumber() << "[ , got " << indices;
  setInputDescription(function.getInputDescription());
  setOutputDescription(function.getOutputDescription());
}

/* Virtual constructor */
VertexEvaluation * VertexEvaluation::clone() const
{
  return new VertexEvaluation(*this);
}

/* Comparison operator */
Bool VertexEvaluation::operator ==(const VertexEvaluation & other) const
{
  return function_ == other.function_ && indices_ == other.indices_;
}

/* String converter */
String VertexEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << VertexEvaluation::GetClassName()
      << " name=" << getName()
      << " function=" << function_
      << " indices=" << indices_;
  return oss;
}

String VertexEvaluation::__str__(const String & ) const
{
  OSS oss(false);
  oss << "VertexEvaluation(function=" << function_ << ", indices=" << indices_ << ")";
  return oss;
}

/* Here is the interface that all derived class must implement */

/* Operator () */
Point VertexEvaluation::operator() (const Point & inP) const
{
  const Sample values(function_(inP));
  return values[indices_[0]];
}

/* Accessor for input point dimension */
UnsignedInteger VertexEvaluation::getInputDimension() const
{
  return function_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger VertexEvaluation::getOutputDimension() const
{
  return function_.getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void VertexEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "function_", function_ );
  adv.saveAttribute( "indices_", indices_ );
}

/* Method load() reloads the object from the StorageManager */
void VertexEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "function_", function_ );
  adv.loadAttribute( "indices_", indices_ );
}

Indices VertexEvaluation::getIndices() const
{
  return indices_;
}

PointToFieldFunction VertexEvaluation::getPointToFieldFunction() const
{
  return function_;
}

END_NAMESPACE_OPENTURNS
