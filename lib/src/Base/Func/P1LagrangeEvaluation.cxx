//                                               -*- C++ -*-
/**
 *  @brief P1 Lagrange piecewise linear function.
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/P1LagrangeEvaluation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/TBBImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(P1LagrangeEvaluation)

static const Factory<P1LagrangeEvaluation> Factory_P1LagrangeEvaluation;


/* Default constructor */
P1LagrangeEvaluation::P1LagrangeEvaluation()
  : EvaluationImplementation()
  , nearestNeighbour_()
  , enclosingSimplex_()
{
  // Nothing to do
}

/* Parameters constructor */
P1LagrangeEvaluation::P1LagrangeEvaluation(const Field & field)
  : EvaluationImplementation()
  , nearestNeighbour_(field.getMesh().getVertices())
  , enclosingSimplex_(field.getMesh().getVertices(), field.getMesh().getSimplices())
{
  setField(field);
}


/* Parameters constructor */
P1LagrangeEvaluation::P1LagrangeEvaluation(const ProcessSample & sample)
  : EvaluationImplementation()
  , nearestNeighbour_(sample.getMesh().getVertices())
  , enclosingSimplex_(sample.getMesh().getVertices(), sample.getMesh().getSimplices())
{
  const Mesh mesh(sample.getMesh());
  const UnsignedInteger length = mesh.getVerticesNumber();
  if (!(length > 0)) throw InvalidArgumentException(HERE) << "Error: expected a non-empty ProcessSample";
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger dimension = sample.getDimension();
  // Copy values in expected order
  values_ = Sample(length, size * dimension);
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    const Sample dataI(sample[i]);
    for(UnsignedInteger l = 0; l < length; ++l)
      std::copy(&dataI(l, 0), &dataI(l, 0) + dimension, &values_(l, i * dimension));
  }
  // To check for pending vertices
  setMesh(mesh);
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
      << offset << "field :" << Os::GetEndOfLine()
      << offset << getField().__str__(offset);
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
  const UnsignedInteger nrVertices = mesh.getVerticesNumber();
  if (nrVertices != values_.getSize()) throw InvalidArgumentException(HERE) << "Error: expected a mesh with =" << values_.getSize() << " vertices, got " << nrVertices << " vertices";
  mesh_ = mesh;
  nearestNeighbour_.setSample(mesh_.getVertices());
  enclosingSimplex_.setVerticesAndSimplices(mesh_.getVertices(), mesh_.getSimplices());

  // Check for pending vertices
  Interval::BoolCollection seenVertices(nrVertices);
  const UnsignedInteger nrSimplices = mesh_.getSimplicesNumber();
  const IndicesCollection simplices(mesh_.getSimplices());
  // Iterate over simplices
  for(UnsignedInteger i = 0; i < nrSimplices; ++i)
  {
    // Iterate over vertices
    for(IndicesCollection::const_iterator vertexIt = simplices.cbegin_at(i), vertexGuard = simplices.cend_at(i); vertexIt != vertexGuard; ++vertexIt)
    {
      const UnsignedInteger vertexIndex = (*vertexIt);
      if (!(vertexIndex < nrVertices)) throw InvalidArgumentException(HERE) << "Error: found a vertex index of " << vertexIndex << " for a total vertex number of " << nrVertices;
      seenVertices[vertexIndex] = 1;
    }
  }
  Indices pendingVertices;
  for (UnsignedInteger i = 0; i < nrVertices; ++i)
    if (seenVertices[i] == 0)
      pendingVertices.add(i);
  if (pendingVertices.getSize() != 0)
  {
    LOGWARN(OSS() << "There are " << pendingVertices.getSize() << " pending vertices. Check the simplices of the mesh");
    LOGDEBUG(OSS() << "The pending vertices indices are " << pendingVertices);
  }
}

Mesh P1LagrangeEvaluation::getMesh() const
{
  return mesh_;
}

/* Values accessor */
void P1LagrangeEvaluation::setValues(const Sample & values)
{
  if (values.getSize() != mesh_.getVerticesNumber()) throw InvalidArgumentException(HERE) << "Error: expected a sample of size=" << mesh_.getVerticesNumber() << ", got size=" << values.getSize();
  values_ = values;
}

Sample P1LagrangeEvaluation::getValues() const
{
  return values_;
}

/* Nearest neighbour algorithm accessor */
void P1LagrangeEvaluation::setNearestNeighbourAlgorithm(const NearestNeighbourAlgorithm & nearestNeighbour)
{
  NearestNeighbourAlgorithm emptyClone(nearestNeighbour.getImplementation()->emptyClone());
  nearestNeighbour_.swap(emptyClone);
  nearestNeighbour_.setSample(mesh_.getVertices());
}

NearestNeighbourAlgorithm P1LagrangeEvaluation::getNearestNeighbourAlgorithm() const
{
  return nearestNeighbour_;
}

/* EnclosingSimplexAlgorithm to speed-up point location */
void P1LagrangeEvaluation::setEnclosingSimplexAlgorithm(const EnclosingSimplexAlgorithm & enclosingSimplex)
{
  EnclosingSimplexAlgorithm emptyClone(enclosingSimplex.getImplementation()->emptyClone());
  enclosingSimplex_.swap(emptyClone);
  enclosingSimplex_.setVerticesAndSimplices(mesh_.getVertices(), mesh_.getSimplices());
}

EnclosingSimplexAlgorithm P1LagrangeEvaluation::getEnclosingSimplexAlgorithm() const
{
  return enclosingSimplex_;
}

/* Here is the interface that all derived class must implement */

/* Operator () */
Point P1LagrangeEvaluation::operator()( const Point & inP ) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  Point result(evaluate(inP));
  callsNumber_.increment();
  return result;
}

class P1LagrangeEvaluationComputeSamplePolicy
{
  const Sample & input_;
  Sample & output_;
  const P1LagrangeEvaluation & lagrange_;

public:
  P1LagrangeEvaluationComputeSamplePolicy(const Sample & input,
                                          Sample & output,
                                          const P1LagrangeEvaluation & lagrange)
    : input_(input)
    , output_(output)
    , lagrange_(lagrange)
  {
    // Nothing to do
  }

  inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
      output_[i] = lagrange_.evaluate(input_[i]);
  } // operator ()
};  // class P1LagrangeEvaluationComputeSamplePolicy

/* Evaluation method */
Point P1LagrangeEvaluation::evaluate( const Point & inP ) const
{
  const UnsignedInteger simplexIndex = enclosingSimplex_.query(inP);
  if (simplexIndex >= mesh_.getSimplicesNumber())
  {
    // No enclosing simplex.   Take value at the nearest point
    return values_[nearestNeighbour_.query(inP)];
  }

  // Compute barycentric coordinates
  Point coordinates(0);
  if (!mesh_.checkPointInSimplexWithCoordinates(inP, simplexIndex, coordinates))
  {
    // Hmmm, should not happen
    return values_[nearestNeighbour_.query(inP)];
  }
  // Here, perform the P1 interpolation
  const Indices simplex(mesh_.getSimplex(simplexIndex));
  Point result(values_[simplex[0]] * coordinates[0]);
  for (UnsignedInteger j = 1; j < simplex.getSize(); ++j)
    result += values_[simplex[j]] * coordinates[j];
  return result;
}

Sample P1LagrangeEvaluation::operator()( const Sample & inS ) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inS.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inS.getDimension();
  const UnsignedInteger size = inS.getSize();
  Sample result(size, values_.getDimension());
  if (size == 0) return result;
  if (inS == mesh_.getVertices()) result = values_;
  else
  {
    const P1LagrangeEvaluationComputeSamplePolicy policy( inS, result, *this );
    TBBImplementation::ParallelFor( 0, size, policy );
  } // The input sample is different from
  callsNumber_.fetchAndAdd(size);
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
  adv.saveAttribute("nearestNeighbour_", nearestNeighbour_);
  adv.saveAttribute("enclosingSimplex_", enclosingSimplex_);
}

/* Method load() reloads the object from the StorageManager */
void P1LagrangeEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute("mesh_", mesh_);
  adv.loadAttribute("values_", values_);
  adv.loadAttribute("nearestNeighbour_", nearestNeighbour_);
  adv.loadAttribute("enclosingSimplex_", enclosingSimplex_);
}

END_NAMESPACE_OPENTURNS
