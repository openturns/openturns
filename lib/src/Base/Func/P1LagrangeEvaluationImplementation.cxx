//                                               -*- C++ -*-
/**
 *  @brief P1 Lagrange piecewise linear function.
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
#include "openturns/P1LagrangeEvaluationImplementation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Description.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(P1LagrangeEvaluationImplementation);

static const Factory<P1LagrangeEvaluationImplementation> RegisteredFactory;


/* Default constructor */
P1LagrangeEvaluationImplementation::P1LagrangeEvaluationImplementation()
  : NumericalMathEvaluationImplementation()
{
  // Nothing to do
}

/* Default constructor */
P1LagrangeEvaluationImplementation::P1LagrangeEvaluationImplementation(const Field & field)
  : NumericalMathEvaluationImplementation()
{
  setField(field);
}


/* Virtual constructor */
P1LagrangeEvaluationImplementation * P1LagrangeEvaluationImplementation::clone() const
{
  return new P1LagrangeEvaluationImplementation(*this);
}


/* Comparison operator */
Bool P1LagrangeEvaluationImplementation::operator ==(const P1LagrangeEvaluationImplementation & other) const
{
  if (this == &other) return true;
  return (field_ == other.field_);
}


/* String converter */
String P1LagrangeEvaluationImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << P1LagrangeEvaluationImplementation::GetClassName()
      << " name=" << getName()
      << " field=" << field_;
  return oss;
}

String P1LagrangeEvaluationImplementation::__str__( const String & offset ) const
{
  OSS oss(false);
  oss << P1LagrangeEvaluationImplementation::GetClassName() << Os::GetEndOfLine()
      << offset << "field :" << Os::GetEndOfLine() << field_.__str__(offset);
  return oss;
}

/* Field accessor */
void P1LagrangeEvaluationImplementation::setField(const Field & field)
{
  field_ = field;
  verticesToSimplices_ = field.getMesh().getVerticesToSimplicesMap();
}

Field P1LagrangeEvaluationImplementation::getField() const
{
  return field_;
}

/* Vertices accessor */
void P1LagrangeEvaluationImplementation::setVertices(const NumericalSample & vertices)
{
  Mesh mesh(field_.getMesh());
  mesh.setVertices(vertices);
  field_ = Field(mesh, field_.getValues());
}

NumericalSample P1LagrangeEvaluationImplementation::getVertices() const
{
  return field_.getMesh().getVertices();
}

/* Values accessor */
void P1LagrangeEvaluationImplementation::setValues(const NumericalSample & values)
{
  field_.setValues(values);
}

NumericalSample P1LagrangeEvaluationImplementation::getValues() const
{
  return field_.getValues();
}

/* Here is the interface that all derived class must implement */

/* Operator () */
NumericalPoint P1LagrangeEvaluationImplementation::operator()( const NumericalPoint & inP ) const
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
    // Here, perform the P1 interpolation
    // First get the index of the nearest vertex
    const UnsignedInteger nearestIndex(field_.getMesh().getNearestVertexIndex(inP));
    const Indices simplicesCandidates(verticesToSimplices_[nearestIndex]);
    const Mesh mesh(field_.getMesh());
    const NumericalSample vertices(mesh.getVertices());
    const Mesh::IndicesCollection simplices(mesh.getSimplices());
    const NumericalSample values(field_.getValues());
    // As a first guess, take the value at the nearest index. It will be the final value if no simplex contains the point
    result = values[nearestIndex];
    NumericalPoint coordinates;
    for (UnsignedInteger i = 0; i < simplicesCandidates.getSize(); ++i)
    {
      const UnsignedInteger simplexIndex(simplicesCandidates[i]);
      if (mesh.checkPointInSimplexWithCoordinates(inP, simplexIndex, coordinates))
      {
        const Indices simplex(simplices[simplexIndex]);
        result = values[simplex[0]] * coordinates[0];
        for (UnsignedInteger j = 1; j < simplex.getSize(); ++j)
          result += values[simplex[j]] * coordinates[j];
        break;
      }
    } // Loop over the simplices candidates
  }
  ++ callsNumber_;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}

NumericalSample P1LagrangeEvaluationImplementation::operator()( const NumericalSample & inS ) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inS.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inS.getDimension();
  NumericalSample result;
  if (inS == field_.getMesh().getVertices()) result = field_.getValues();
  else result = NumericalMathEvaluationImplementation::operator()(inS);
  callsNumber_ += inS.getSize();
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inS);
    outputStrategy_.store(result);
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger P1LagrangeEvaluationImplementation::getInputDimension() const
{
  return field_.getSpatialDimension();
}


/* Accessor for output point dimension */
UnsignedInteger P1LagrangeEvaluationImplementation::getOutputDimension() const
{
  return field_.getDimension();
}


/* Method save() stores the object through the StorageManager */
void P1LagrangeEvaluationImplementation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute("field_", field_);
}

/* Method load() reloads the object from the StorageManager */
void P1LagrangeEvaluationImplementation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute("field_", field_);
}

END_NAMESPACE_OPENTURNS
