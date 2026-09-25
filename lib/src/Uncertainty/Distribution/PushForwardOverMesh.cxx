//                                               -*- C++ -*-
/**
 *  @brief The PushForwardOverMesh distribution
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/PushForwardOverMesh.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/UniformOverMesh.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PushForwardOverMesh)

static const Factory<PushForwardOverMesh> Factory_PushForwardOverMesh;

/* Default constructor */
PushForwardOverMesh::PushForwardOverMesh()
  : DistributionImplementation()
  , antecedent_()
  , values_()
  , evaluation_()
  , weights_()
{
  // Trivial default: the identity P1 map over the mesh of the segment [0, 1]
  Sample vertices(2, Point(1, 0.0));
  vertices(1, 0) = 1.0;
  IndicesCollection simplices(1, 2);
  simplices(0, 1) = 1;
  const Mesh mesh(vertices, simplices);
  Sample values(2, Point(1, 0.0));
  values(1, 0) = 1.0;
  setAntecedentAndValues(UniformOverMesh(mesh), Field(mesh, values));
}

/* Parameters constructor */
PushForwardOverMesh::PushForwardOverMesh(const Field & values)
  : PushForwardOverMesh(UniformOverMesh(values.getMesh()), values)
{
  // The default antecedent is the uniform distribution over the reference mesh
}

/* Parameters constructor */
PushForwardOverMesh::PushForwardOverMesh(const Distribution & antecedent,
    const Field & values)
  : DistributionImplementation()
  , antecedent_()
  , values_()
  , evaluation_()
  , weights_()
{
  setAntecedentAndValues(antecedent, values);
}

/* Comparison operator */
Bool PushForwardOverMesh::operator ==(const PushForwardOverMesh & other) const
{
  if (this == &other) return true;
  return (values_ == other.values_) && (antecedent_ == other.antecedent_);
}

Bool PushForwardOverMesh::equals(const DistributionImplementation & other) const
{
  const PushForwardOverMesh* p_other = dynamic_cast<const PushForwardOverMesh*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String PushForwardOverMesh::__repr__() const
{
  OSS oss;
  oss << "class=" << PushForwardOverMesh::GetClassName()
      << " name=" << getName()
      << " antecedent=" << antecedent_
      << " values=" << values_;
  return oss;
}

String PushForwardOverMesh::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "=f(" << antecedent_.__str__()
      << ") with f the P1 map defined by " << values_;
  return oss;
}

/* Virtual constructor */
PushForwardOverMesh * PushForwardOverMesh::clone() const
{
  return new PushForwardOverMesh(*this);
}

/* Set the antecedent and the field with check */
void PushForwardOverMesh::setAntecedentAndValues(const Distribution & antecedent,
    const Field & values)
{
  const Sample referenceVertices(values.getMesh().getVertices());
  const UnsignedInteger inputDimension = referenceVertices.getDimension();
  if (referenceVertices.getSize() == 0 || inputDimension == 0)
    throw InvalidArgumentException(HERE) << "Error: expected a non-empty reference mesh in the field";
  const Sample imageVertices(values.getValues());
  const UnsignedInteger outputDimension = imageVertices.getDimension();
  if (imageVertices.getSize() != referenceVertices.getSize())
    throw InvalidArgumentException(HERE) << "Error: the field must give the image of each reference vertex, here the reference mesh has " << referenceVertices.getSize() << " vertices and the field has " << imageVertices.getSize() << " values";
  if (imageVertices.getSize() == 0 || outputDimension == 0)
    throw InvalidArgumentException(HERE) << "Error: expected a non-empty field of values";
  if (antecedent.getDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "Error: the antecedent must have a dimension equal to the dimension of the reference mesh, here the dimension of the antecedent is " << antecedent.getDimension() << " and the dimension of the reference mesh is " << inputDimension;
  if (!antecedent.isContinuous())
    throw InvalidArgumentException(HERE) << "Error: the antecedent must be absolutely continuous, here isContinuous()=" << antecedent.isContinuous();
  if (outputDimension < inputDimension)
    throw NotYetImplementedException(HERE) << "Error: the case where the ambient dimension of the image is lower than the dimension of the reference domain (co-area formula) is not yet implemented, here reference dimension=" << inputDimension << " and ambient dimension=" << outputDimension;
  antecedent_ = antecedent;
  values_ = values;
  evaluation_ = P1LagrangeEvaluation(values_);
  epsilon_ = ResourceMap::GetAsScalar("PushForwardOverMesh-OnSimplexEpsilon");
  computeWeights();
  setDimension(outputDimension);
  // We don't know if the P1 evaluation is thread-safe and it could be called in parallel in computePDF()
  setParallel(false);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computeRange();
}

/* Compute the per-simplex weights */
void PushForwardOverMesh::computeWeights()
{
  const Mesh referenceMesh(values_.getMesh());
  const Sample referenceVertices(referenceMesh.getVertices());
  const IndicesCollection simplices(referenceMesh.getSimplices());
  const Sample imageVertices(values_.getValues());
  const UnsignedInteger dimension = referenceMesh.getDimension();
  const UnsignedInteger simplicesNumber = simplices.getSize();
  weights_ = Point(simplicesNumber, 0.0);
  for (UnsignedInteger i = 0; i < simplicesNumber; ++i)
  {
    // Gram matrices of the edges of the reference and of the image simplex
    SquareMatrix referenceGram(dimension);
    SquareMatrix imageGram(dimension);
    const UnsignedInteger index0 = simplices(i, 0);
    const Point referenceV0(referenceVertices[index0]);
    const Point imageV0(imageVertices[index0]);
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      const UnsignedInteger indexJ = simplices(i, j + 1);
      const Point referenceEdge(referenceVertices[indexJ] - referenceV0);
      const Point imageEdge(imageVertices[indexJ] - imageV0);
      for (UnsignedInteger k = j; k < dimension; ++k)
      {
        const UnsignedInteger indexK = simplices(i, k + 1);
        const Point referenceEdgeK(referenceVertices[indexK] - referenceV0);
        const Point imageEdgeK(imageVertices[indexK] - imageV0);
        const Scalar referenceValue = referenceEdge.dot(referenceEdgeK);
        const Scalar imageValue = imageEdge.dot(imageEdgeK);
        referenceGram(j, k) = referenceValue;
        referenceGram(k, j) = referenceValue;
        imageGram(j, k) = imageValue;
        imageGram(k, j) = imageValue;
      }
    }
    const Scalar referenceDeterminant = referenceGram.computeDeterminant();
    const Scalar imageDeterminant = imageGram.computeDeterminant();
    if ((referenceDeterminant > 0.0) && (imageDeterminant > 0.0))
      weights_[i] = std::sqrt(referenceDeterminant / imageDeterminant);
    // Else the simplex is degenerate and its contribution is discarded:
    // this is the integral of the density at a critical point where the
    // Jacobian of the map is singular
  }
}

/* Antecedent accessor */
void PushForwardOverMesh::setAntecedent(const Distribution & antecedent)
{
  setAntecedentAndValues(antecedent, values_);
}

Distribution PushForwardOverMesh::getAntecedent() const
{
  return antecedent_;
}

/* P1 field accessor */
void PushForwardOverMesh::setValues(const Field & values)
{
  setAntecedentAndValues(antecedent_, values);
}

Field PushForwardOverMesh::getValues() const
{
  return values_;
}

/* Image mesh accessor */
Mesh PushForwardOverMesh::getMesh() const
{
  return Mesh(values_.getValues(), values_.getMesh().getSimplices());
}

/* Intrinsic dimension accessor */
UnsignedInteger PushForwardOverMesh::getIntrinsicDimension() const
{
  return values_.getMesh().getDimension();
}

/* Get one realization of the distribution */
Point PushForwardOverMesh::getRealization() const
{
  return evaluation_(antecedent_.getRealization());
}

/* Compute the numerical range of the distribution given the parameters values */
void PushForwardOverMesh::computeRange()
{
  const Sample imageVertices(values_.getValues());
  setRange(Interval(imageVertices.getMin(), imageVertices.getMax()));
}

/* Check if the point belongs to the affine hull of the image of the simplex
   at the given index, and if so give its barycentric coordinates */
Bool PushForwardOverMesh::isOnSimplex(const Point & point,
                                      const UnsignedInteger index,
                                      Point & barycentricCoordinates) const
{
  const Mesh referenceMesh(values_.getMesh());
  const IndicesCollection simplices(referenceMesh.getSimplices());
  const Sample imageVertices(values_.getValues());
  const UnsignedInteger dimension = referenceMesh.getDimension();
  const UnsignedInteger index0 = simplices(index, 0);
  const Point v0(imageVertices[index0]);
  // Gram matrix of the image edges and the right hand side
  const Point q(point - v0);
  Point rhs(dimension);
  SquareMatrix gram(dimension);
  Scalar maxEdgeSquared = 0.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const UnsignedInteger indexI = simplices(index, i + 1);
    const Point edgeI(imageVertices[indexI] - v0);
    maxEdgeSquared = std::max(maxEdgeSquared, edgeI.normSquare());
    rhs[i] = edgeI.dot(q);
    for (UnsignedInteger j = i; j < dimension; ++j)
    {
      const UnsignedInteger indexJ = simplices(index, j + 1);
      const Point edgeJ(imageVertices[indexJ] - v0);
      const Scalar value = edgeI.dot(edgeJ);
      gram(i, j) = value;
      gram(j, i) = value;
    }
  }
  if (!(maxEdgeSquared > 0.0)) return false;
  const Scalar lengthScale = std::sqrt(maxEdgeSquared);
  const Scalar tolerance = epsilon_ * lengthScale;
  // The image simplex is degenerate
  if (!(gram.computeDeterminant() > 0.0)) return false;
  const Point edgeCoordinates(gram.solveLinearSystem(rhs));
  // Rebuild the projection of the point onto the affine hull of the image simplex
  const UnsignedInteger dimensionPoint = point.getDimension();
  Point projection(v0);
  Scalar firstCoordinate = 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    firstCoordinate -= edgeCoordinates[i];
    const UnsignedInteger indexI = simplices(index, i + 1);
    const Point vertexI(imageVertices[indexI]);
    for (UnsignedInteger j = 0; j < dimensionPoint; ++j)
      projection[j] += edgeCoordinates[i] * (vertexI[j] - v0[j]);
  }
  if ((point - projection).norm() > tolerance) return false;
  // The barycentric coordinates must lie in [0, 1] within the tolerance
  if ((firstCoordinate < -epsilon_) || (firstCoordinate > 1.0 + epsilon_)) return false;
  barycentricCoordinates = Point(dimension + 1);
  barycentricCoordinates[0] = firstCoordinate;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if ((edgeCoordinates[i] < -epsilon_) || (edgeCoordinates[i] > 1.0 + epsilon_)) return false;
    barycentricCoordinates[i + 1] = edgeCoordinates[i];
  }
  return true;
}

/* Compute the PDF of the distribution */
Scalar PushForwardOverMesh::computePDF(const Point & point) const
{
  const UnsignedInteger outputDimension = getDimension();
  if (point.getDimension() != outputDimension)
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << outputDimension << ", here dimension=" << point.getDimension();
  // A point outside the bounding box of the image mesh cannot be in the support
  if (!getRange().contains(point)) return 0.0;
  const Mesh referenceMesh(values_.getMesh());
  const Sample referenceVertices(referenceMesh.getVertices());
  const IndicesCollection simplices(referenceMesh.getSimplices());
  const UnsignedInteger dimension = referenceMesh.getDimension();
  const UnsignedInteger simplicesNumber = simplices.getSize();
  // The intrinsic density at the point is the sum of the contributions of
  // all the simplices whose image contains the point, each contribution
  // being the density of the antecedent at the preimage divided by the
  // volume stretching factor sqrt(det(J J^T)) of the piecewise-affine map.
  // The images of two distinct simplices may genuinely overlap (fold) and
  // then the whole sum is taken; on the other hand a point lying exactly
  // on the common boundary of two images belongs to a set of measure zero,
  // and it is attributed to a single simplex, the first one met in the
  // scan, so that it is not counted twice
  Scalar pdf = 0.0;
  UnsignedInteger boundaryS = simplicesNumber;
  Bool hasBoundaryMatch = false;
  Point boundaryBarycentricCoordinates;
  for (UnsignedInteger s = 0; s < simplicesNumber; ++s)
  {
    Point barycentricCoordinates;
    if (!isOnSimplex(point, s, barycentricCoordinates)) continue;
    if (!(weights_[s] > 0.0)) continue;
    // Check if the point is strictly inside the image of the simplex
    Bool strictlyInside = true;
    for (UnsignedInteger i = 0; i <= dimension; ++i)
    {
      if (!((barycentricCoordinates[i] > epsilon_) &&
            (barycentricCoordinates[i] < 1.0 - epsilon_)))
      {
        strictlyInside = false;
        break;
      }
    }
    if (!strictlyInside)
    {
      // A point on the boundary of the image: keep the first match only
      if (!hasBoundaryMatch)
      {
        boundaryS = s;
        boundaryBarycentricCoordinates = barycentricCoordinates;
        hasBoundaryMatch = true;
      }
      continue;
    }
    // Preimage of the point in the reference domain
    Point preimage(dimension, 0.0);
    for (UnsignedInteger i = 0; i <= dimension; ++i)
    {
      const Point referenceVertex(referenceVertices[simplices(s, i)]);
      for (UnsignedInteger j = 0; j < dimension; ++j)
        preimage[j] += barycentricCoordinates[i] * referenceVertex[j];
    }
    const Scalar numerator = antecedent_.computePDF(preimage);
    if (!(numerator > 0.0)) continue;
    pdf += numerator * weights_[s];
  }
  if (hasBoundaryMatch)
  {
    Point preimage(dimension, 0.0);
    for (UnsignedInteger i = 0; i <= dimension; ++i)
    {
      const Point referenceVertex(referenceVertices[simplices(boundaryS, i)]);
      for (UnsignedInteger j = 0; j < dimension; ++j)
        preimage[j] += boundaryBarycentricCoordinates[i] * referenceVertex[j];
    }
    const Scalar numerator = antecedent_.computePDF(preimage);
    if (numerator > 0.0) pdf += numerator * weights_[boundaryS];
  }
  return pdf;
}

/* Parameters value and description accessor */
PushForwardOverMesh::PointWithDescriptionCollection PushForwardOverMesh::getParametersCollection() const
{
  return antecedent_.getParametersCollection();
}

void PushForwardOverMesh::setParametersCollection(const PointCollection & parametersCollection)
{
  antecedent_.setParametersCollection(parametersCollection);
  setAntecedentAndValues(antecedent_, values_);
}

/* Parameters value accessors */
void PushForwardOverMesh::setParameter(const Point & parameter)
{
  antecedent_.setParameter(parameter);
  setAntecedentAndValues(antecedent_, values_);
}

Point PushForwardOverMesh::getParameter() const
{
  return antecedent_.getParameter();
}

/* Parameters description accessor */
Description PushForwardOverMesh::getParameterDescription() const
{
  return antecedent_.getParameterDescription();
}

/* Tell if the distribution is continuous */
Bool PushForwardOverMesh::isContinuous() const
{
  return antecedent_.isContinuous();
}

/* Tell if the distribution is discrete */
Bool PushForwardOverMesh::isDiscrete() const
{
  return antecedent_.isDiscrete();
}

/* Compute the mean of the distribution */
void PushForwardOverMesh::computeMean() const
{
  const UnsignedInteger sampleSize = ResourceMap::GetAsUnsignedInteger("PushForwardOverMesh-SampleSize");
  const Sample sample(evaluation_(antecedent_.getSample(sampleSize)));
  mean_ = sample.computeMean();
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void PushForwardOverMesh::computeCovariance() const
{
  const UnsignedInteger sampleSize = ResourceMap::GetAsUnsignedInteger("PushForwardOverMesh-SampleSize");
  const Sample sample(evaluation_(antecedent_.getSample(sampleSize)));
  covariance_ = sample.computeCovariance();
  isAlreadyComputedCovariance_ = true;
}

/* Method save() stores the object through the StorageManager */
void PushForwardOverMesh::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute("antecedent_", antecedent_);
  adv.saveAttribute("values_", values_);
}

/* Method load() reloads the object from the StorageManager */
void PushForwardOverMesh::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute("antecedent_", antecedent_);
  adv.loadAttribute("values_", values_);
  setAntecedentAndValues(antecedent_, values_);
}

END_NAMESPACE_OPENTURNS