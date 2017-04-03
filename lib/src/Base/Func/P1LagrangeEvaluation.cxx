//                                               -*- C++ -*-
/**
 *  @brief P1 Lagrange piecewise linear function.
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/P1LagrangeEvaluation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Description.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(P1LagrangeEvaluation);

static const Factory<P1LagrangeEvaluation> Factory_P1LagrangeEvaluation;


/* Default constructor */
P1LagrangeEvaluation::P1LagrangeEvaluation()
  : EvaluationImplementation()
{
  // Nothing to do
}

/* Default constructor */
P1LagrangeEvaluation::P1LagrangeEvaluation(const Field & field)
  : EvaluationImplementation()
{
  setField(field);
}


/* Virtual constructor */
P1LagrangeEvaluation * P1LagrangeEvaluation::clone() const
{
  return new P1LagrangeEvaluation(*this);
}


/* Comparison operator */
Bool P1LagrangeEvaluation::operator ==(const P1LagrangeEvaluation & other) const
{
  if (this == &other) return true;
  return (mesh_ == other.mesh_) && (values_ == other.values_);
}


/* String converter */
String P1LagrangeEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << P1LagrangeEvaluation::GetClassName()
      << " name=" << getName()
      << " mesh=" << mesh_
      << " values=" << values_;
  return oss;
}

String P1LagrangeEvaluation::__str__( const String & offset ) const
{
  OSS oss(false);
  oss << P1LagrangeEvaluation::GetClassName() << Os::GetEndOfLine()
      << offset << "field :" << Os::GetEndOfLine() << getField().__str__(offset);
  return oss;
}

/* Field accessor */
void P1LagrangeEvaluation::setField(const Field & field)
{
  values_ = field.getValues();
  // To check for pending vertices
  setMesh(field.getMesh());
}

Field P1LagrangeEvaluation::getField() const
{
  return Field(mesh_, values_);
}

/* Mesh accessor */
void P1LagrangeEvaluation::setMesh(const Mesh & mesh)
{
  if (mesh.getVerticesNumber() != values_.getSize()) throw InvalidArgumentException(HERE) << "Error: expected a mesh with =" << values_.getSize() << " vertices, got " << mesh_.getVerticesNumber() << " vertices";
  mesh_ = mesh;
  // Check for pending vertices
  Mesh::IndicesCollection verticesToSimplices(mesh_.getVerticesToSimplicesMap());
  Indices pendingVertices(0);
  for (UnsignedInteger i = 0; i < verticesToSimplices.getSize(); ++i)
    if (verticesToSimplices[i].getSize() == 0) pendingVertices.add(i);
  if (pendingVertices.getSize() > 0)
    {
      LOGWARN(OSS() << "There are " << pendingVertices.getSize() << " pending vertices. Check the simplices of the mesh");
      LOGDEBUG(OSS() << "The pending vertices indices are " << pendingVertices);
    }
}

Mesh P1LagrangeEvaluation::getMesh() const
{
  return mesh_;
}

/* Vertices accessor */
void P1LagrangeEvaluation::setVertices(const NumericalSample & vertices)
{
  mesh_.setVertices(vertices);
}

NumericalSample P1LagrangeEvaluation::getVertices() const
{
  return mesh_.getVertices();
}

/* Simplices accessor */
void P1LagrangeEvaluation::setSimplices(const IndicesCollection & simplices)
{
  mesh_.setSimplices(simplices);
}

P1LagrangeEvaluation::IndicesCollection P1LagrangeEvaluation::getSimplices() const
{
  return mesh_.getSimplices();
}

/* Values accessor */
void P1LagrangeEvaluation::setValues(const NumericalSample & values)
{
  if (values.getSize() != mesh_.getVerticesNumber()) throw InvalidArgumentException(HERE) << "Error: expected a sample of size=" << mesh_.getVerticesNumber() << ", got size=" << values.getSize();
  values_ = values;
}

NumericalSample P1LagrangeEvaluation::getValues() const
{
  return values_;
}

/* Here is the interface that all derived class must implement */

/* Operator () */
NumericalPoint P1LagrangeEvaluation::operator()( const NumericalPoint & inP ) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  NumericalPoint result;
  CacheKeyType inKey(inP.getCollection());

  if ( isCacheEnabled() && p_cache_->hasKey(inKey) )
  {
    result = NumericalPoint::ImplementationType( p_cache_->find(inKey) );
  }
  else
  {
    result = evaluate(inP);
  }
  ++ callsNumber_;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}

/* Evaluation method */
NumericalPoint P1LagrangeEvaluation::evaluate( const NumericalPoint & inP ) const
{
  NumericalPoint coordinates(0);
  const Indices vertexAndSimplexIndices(mesh_.getNearestVertexAndSimplexIndicesWithCoordinates(inP, coordinates));
  // Here, perform the P1 interpolation
  // First get the index of the nearest vertex
  const UnsignedInteger nearestIndex = vertexAndSimplexIndices[0];
  // As a first guess, take the value at the nearest index. It will be the final value if no simplex contains the point
  NumericalPoint result(values_[nearestIndex]);
  if (coordinates.getSize() > 0)
    {
      const Indices simplex(mesh_.getSimplex(vertexAndSimplexIndices[1]));
      result = values_[simplex[0]] * coordinates[0];
      for (UnsignedInteger j = 1; j < simplex.getSize(); ++j)
	result += values_[simplex[j]] * coordinates[j];
    }
  return result;
}

NumericalSample P1LagrangeEvaluation::operator()( const NumericalSample & inS ) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inS.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inS.getDimension();
  const UnsignedInteger size = inS.getSize();
  NumericalSample result(size, values_.getDimension());
  if (size == 0) return result;
  if (inS == mesh_.getVertices()) result = values_;
  else
  {
    const P1LagrangeEvaluationComputeSamplePolicy policy( inS, result, *this );
    TBB::ParallelFor( 0, size, policy );
  } // The input sample is different from
  callsNumber_ += size;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inS);
    outputStrategy_.store(result);
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger P1LagrangeEvaluation::getInputDimension() const
{
  return mesh_.getDimension();
}


/* Accessor for output point dimension */
UnsignedInteger P1LagrangeEvaluation::getOutputDimension() const
{
  return values_.getDimension();
}


/* Method save() stores the object through the StorageManager */
void P1LagrangeEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute("mesh_", mesh_);
  adv.saveAttribute("values_", values_);
}

/* Method load() reloads the object from the StorageManager */
void P1LagrangeEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute("mesh_", mesh_);
  adv.loadAttribute("values_", values_);
}

END_NAMESPACE_OPENTURNS
