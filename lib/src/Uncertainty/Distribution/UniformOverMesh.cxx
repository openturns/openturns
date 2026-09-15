//                                               -*- C++
/**
 *  @brief The UniformOverMesh distribution
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
#include <cmath>
#include <numeric>

#include "openturns/UniformOverMesh.hxx"
#include "openturns/RegularGrid.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/GaussLegendre.hxx"
#include "openturns/IntervalMesher.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/ResourceMap.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(UniformOverMesh)

static const Factory<UniformOverMesh> Factory_UniformOverMesh;

/* Default constructor */
UniformOverMesh::UniformOverMesh()
  : DistributionImplementation()
{
  setName( "UniformOverMesh" );
  setMesh(RegularGrid(0.0, 1.0, 2));
}

/* Parameters constructor */
UniformOverMesh::UniformOverMesh(const Mesh & mesh)
  : DistributionImplementation()
{
  setName( "UniformOverMesh" );
  setMesh(mesh);
}

/* Comparison operator */
Bool UniformOverMesh::operator ==(const UniformOverMesh & other) const
{
  if (this == &other) return true;
  return hasEqualBase(other) && (mesh_ == other.mesh_);
}

Bool UniformOverMesh::equals(const DistributionImplementation & other) const
{
  const UniformOverMesh* p_other = dynamic_cast<const UniformOverMesh*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String UniformOverMesh::__repr__() const
{
  OSS oss(true);
  oss << "class=" << UniformOverMesh::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " intrinsicDimension=" << intrinsicDimension_
      << " mesh=" << mesh_
      << " meshDomain=" << meshDomain_
      << " simplicesVolumes=" << simplicesVolumes_
      << " meshVolume=" << meshVolume_
      << " probabilities=" << probabilities_
      << " integrationAlgorithm=" << integrationAlgorithm_;
  return oss;
}

String UniformOverMesh::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(mesh = " << mesh_ << ")";
  return oss;
}

/* Virtual constructor */
UniformOverMesh * UniformOverMesh::clone() const
{
  return new UniformOverMesh(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void UniformOverMesh::computeRange()
{
  setRange(Interval(mesh_.getLowerBound(), mesh_.getUpperBound()));
}


/* Get one realization of the distribution */
Point UniformOverMesh::getRealization() const
{
  const UnsignedInteger index = DistFunc::rDiscrete(base_, alias_);
  const UnsignedInteger dimension = getDimension();
  Point result(dimension);
  if (intrinsicDimension_ < dimension)
  {
    // Copy the vertices of the simplex in a contiguous buffer
    Sample vertexSample(intrinsicDimension_ + 1, dimension);
    for (UnsignedInteger j = 0; j <= intrinsicDimension_; ++j)
      for (UnsignedInteger k = 0; k < dimension; ++k)
        vertexSample(j, k) = vertices_(simplices_(index, j), k);
    DistFunc::rUniformSimplex(&vertexSample(0, 0), dimension, intrinsicDimension_ + 1, &result[0]);
    return result;
  }
  if (dimension == 1)
  {
    DistFunc::rUniformSegment(&vertices_(simplices_(index, 0), 0), &vertices_(simplices_(index, 1), 0), dimension, &result[0]);
    return result;
  }
  if (dimension == 2)
  {
    DistFunc::rUniformTriangle(&vertices_(simplices_(index, 0), 0), &vertices_(simplices_(index, 1), 0), &vertices_(simplices_(index, 2), 0), dimension, &result[0]);
    return result;
  }
  if (dimension == 3)
  {
    DistFunc::rUniformTetrahedron(&vertices_(simplices_(index, 0), 0), &vertices_(simplices_(index, 1), 0), &vertices_(simplices_(index, 2), 0), &vertices_(simplices_(index, 3), 0), dimension, &result[0]);
    return result;
  }
  Sample vertices(dimension + 1, dimension);
  for (UnsignedInteger i = 0; i <= dimension; ++i)
  {
    const UnsignedInteger vertexIndex = simplices_(index, i);
    for (UnsignedInteger j = 0; j < dimension; ++j)
      vertices(i, j) = vertices_(vertexIndex, j);
  } // i
  DistFunc::rUniformSimplex(&vertices(0, 0), dimension, dimension + 1, &result[0]);
  return result;
}


/* Get a sample of the distribution */
Sample UniformOverMesh::getSample(const UnsignedInteger size) const
{
  const Indices indices(DistFunc::rDiscrete(base_, alias_, size));
  const UnsignedInteger dimension = getDimension();
  Sample result(size, dimension);
  if (intrinsicDimension_ < dimension)
  {
    Sample vertexSample(intrinsicDimension_ + 1, dimension);
    for (UnsignedInteger n = 0; n < size; ++n)
    {
      const UnsignedInteger index = indices[n];
      // Copy the vertices of the simplex in a contiguous buffer
      for (UnsignedInteger j = 0; j <= intrinsicDimension_; ++j)
        for (UnsignedInteger k = 0; k < dimension; ++k)
          vertexSample(j, k) = vertices_(simplices_(index, j), k);
      DistFunc::rUniformSimplex(&vertexSample(0, 0), dimension, intrinsicDimension_ + 1, &result(n, 0));
    } // n
    result.setDescription(getDescription());
    return result;
  }
  if (dimension == 1)
  {
    for (UnsignedInteger n = 0; n < size; ++n)
    {
      const UnsignedInteger index = indices[n];
      DistFunc::rUniformSegment(&vertices_(simplices_(index, 0), 0), &vertices_(simplices_(index, 1), 0), dimension, &result(n, 0));
    } // n
    result.setDescription(getDescription());
    return result;
  } // dimension == 1
  if (dimension == 2)
  {
    for (UnsignedInteger n = 0; n < size; ++n)
    {
      const UnsignedInteger index = indices[n];
      DistFunc::rUniformTriangle(&vertices_(simplices_(index, 0), 0), &vertices_(simplices_(index, 1), 0), &vertices_(simplices_(index, 2), 0), dimension, &result(n, 0));
    }
    result.setDescription(getDescription());
    return result;
  } // dimension == 2
  if (dimension == 3)
  {
    for (UnsignedInteger n = 0; n < size; ++n)
    {
      const UnsignedInteger index = indices[n];
      DistFunc::rUniformTetrahedron(&vertices_(simplices_(index, 0), 0), &vertices_(simplices_(index, 1), 0), &vertices_(simplices_(index, 2), 0), &vertices_(simplices_(index, 3), 0), dimension, &result(n, 0));
    }
    result.setDescription(getDescription());
    return result;
  } // dimension == 3
  Sample vertices(dimension + 1, dimension);
  for (UnsignedInteger n = 0; n < size; ++n)
  {
    const UnsignedInteger index = indices[n];
    for (UnsignedInteger i = 0; i <= dimension; ++i)
    {
      const UnsignedInteger vertexIndex = simplices_(index, i);
      for (UnsignedInteger j = 0; j < dimension; ++j)
        vertices(i, j) = vertices_(vertexIndex, j);
    } // i
    DistFunc::rUniformSimplex(&vertices(0, 0), dimension, dimension + 1, &result(n, 0));
  } // n
  result.setDescription(getDescription());
  return result;
}


/* Get the PDF of the distribution, i.e. P(point < X < point+dx) = PDF(point)dx + o(dx) */
Scalar UniformOverMesh::computePDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();
  if (intrinsicDimension_ < getDimension())
    return (isOnMesh(point) ? (1.0 / meshVolume_) : 0.0);
  if (meshDomain_.contains(point))
    return 1.0 / meshVolume_;
  return 0.0;
}


/* Get the probability content of an interval */
Scalar UniformOverMesh::computeProbabilityContinuous(const Interval & interval) const
{
  if (interval.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "UniformOverMesh interval must have dimension " << getDimension() << ", got " << interval.getDimension();
  Scalar probability = 0.0;
  const Interval interval2(interval.intersect(getRange()));
  if (interval2.isEmpty())
    probability = 0.0;
  else if (interval2 == getRange())
    probability = 1.0;
  else if (intrinsicDimension_ < getDimension())
    throw NotYetImplementedException(HERE) << "In UniformOverMesh::computeProbabilityContinuous() const: for a mesh embedded in an ambient space of larger dimension, the probability of an interval is not yet implemented";
  else
  {
    try
    {
      const Mesh intervalMesh(IntervalMesher(Indices(getDimension(), 1)).build(interval2));
      const Mesh intersectionMesh(mesh_.intersect(intervalMesh));
      probability = intersectionMesh.getVolume() / meshVolume_;
    }
    catch (const NotYetImplementedException &)
    {
      // no boost support
      probability = integrationAlgorithm_.integrate(getPDF(), interval2)[0];
    }
  }
  return probability;
}


/* Compute the mean of the distribution */
void UniformOverMesh::computeMean() const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger verticesPerSimplex = intrinsicDimension_ + 1;
  mean_ = Point(dimension);
  for (UnsignedInteger i = 0; i < simplicesVolumes_.getSize(); ++i)
  {
    const Scalar factor = simplicesVolumes_[i] / (meshVolume_ * verticesPerSimplex);
    for (UnsignedInteger j = 0; j < verticesPerSimplex; ++j)
    {
      const UnsignedInteger vertexIndex = simplices_(i, j);
      for (UnsignedInteger k = 0; k < dimension; ++k)
        mean_[k] += vertices_(vertexIndex, k) * factor;
    } // j
  } // i
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void UniformOverMesh::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger verticesPerSimplex = intrinsicDimension_ + 1;
  // For a simplex with vertices v_0,...,v_d, the uniform distribution has
  // second moments E[X X^T] = (sum_{i,j} v_i v_j^T + sum_i v_i v_i^T) / ((d+1)(d+2)).
  const Scalar denominator = static_cast<Scalar>(verticesPerSimplex * (verticesPerSimplex + 1));
  SquareMatrix secondMoments(dimension);
  const UnsignedInteger simplicesNumber = simplices_.getSize();
  for (UnsignedInteger i = 0; i < simplicesNumber; ++i)
  {
    SquareMatrix contribution(dimension);
    for (UnsignedInteger a = 0; a < verticesPerSimplex; ++a)
    {
      const UnsignedInteger vertexA = simplices_(i, a);
      for (UnsignedInteger r = 0; r < dimension; ++r)
        for (UnsignedInteger c = 0; c < dimension; ++c)
          contribution(r, c) += vertices_(vertexA, r) * vertices_(vertexA, c);
      for (UnsignedInteger b = 0; b < verticesPerSimplex; ++b)
      {
        const UnsignedInteger vertexB = simplices_(i, b);
        for (UnsignedInteger r = 0; r < dimension; ++r)
          for (UnsignedInteger c = 0; c < dimension; ++c)
            contribution(r, c) += vertices_(vertexA, r) * vertices_(vertexB, c);
      }
    }
    for (UnsignedInteger r = 0; r < dimension; ++r)
      for (UnsignedInteger c = 0; c < dimension; ++c)
        secondMoments(r, c) += probabilities_[i] * contribution(r, c) / denominator;
  }
  const Point mean(getMean());
  for (UnsignedInteger r = 0; r < dimension; ++r)
    for (UnsignedInteger c = 0; c < dimension; ++c)
      secondMoments(r, c) -= mean[r] * mean[c];
  covariance_ = CovarianceMatrix(secondMoments.getImplementation());
  isAlreadyComputedCovariance_ = true;
}

/* Mesh accessor */
void UniformOverMesh::setMesh(const Mesh & mesh)
{
  const UnsignedInteger dimension = mesh.getDimension();
  if (dimension == 0) throw InvalidArgumentException(HERE) << "Error: expected a mesh of dimension>0";
  setDimension(dimension);
  mesh_ = mesh;
  vertices_ = mesh.getVertices();
  simplices_ = mesh.getSimplices();
  const UnsignedInteger simplicesNumber = simplices_.getSize();
  if (simplicesNumber == 0)
    throw InvalidArgumentException(HERE) << "Error: expected a mesh with at least one simplex.";
  const UnsignedInteger intrinsicDimension = mesh.getIntrinsicDimension();
  if (intrinsicDimension == 0)
    throw InvalidArgumentException(HERE) << "Error: expected simplices with more than one vertex.";
  if (intrinsicDimension > dimension)
    throw InvalidArgumentException(HERE) << "Error: the intrinsic dimension=" << intrinsicDimension << " of the mesh must be lower than or equal to the ambient dimension=" << dimension << ".";
  intrinsicDimension_ = intrinsicDimension;
  // Compute the volume of each simplex
  if (intrinsicDimension == dimension)
  {
    simplicesVolumes_ = mesh.computeSimplicesVolume();
  }
  else
  {
    // The intrinsic volume of a simplex embedded in an ambient space of
    // larger dimension is sqrt(det(G)) / d! where G is the Gram matrix of
    // its edge vectors
    Scalar dFactorial = 1.0;
    for (UnsignedInteger i = 2; i <= intrinsicDimension; ++i) dFactorial *= static_cast<Scalar>(i);
    SquareMatrix gram(intrinsicDimension);
    simplicesVolumes_ = Point(simplicesNumber);
    for (UnsignedInteger i = 0; i < simplicesNumber; ++i)
    {
      const UnsignedInteger index0 = simplices_(i, 0);
      const Point v0(vertices_[index0]);
      for (UnsignedInteger j = 0; j < intrinsicDimension; ++j)
      {
        const UnsignedInteger indexJ = simplices_(i, j + 1);
        const Point edgeJ(vertices_[indexJ] - v0);
        for (UnsignedInteger k = j; k < intrinsicDimension; ++k)
        {
          const UnsignedInteger indexK = simplices_(i, k + 1);
          const Point edgeK(vertices_[indexK] - v0);
          const Scalar value = edgeJ.dot(edgeK);
          gram(j, k) = value;
          gram(k, j) = value;
        }
      }
      const Scalar determinant = gram.computeDeterminant();
      simplicesVolumes_[i] = (determinant > 0.0) ? std::sqrt(determinant) / dFactorial : 0.0;
    }
  }
  meshVolume_ = std::accumulate(simplicesVolumes_.begin(), simplicesVolumes_.end(), 0.0);
  if (!(meshVolume_ > 0.0)) throw InvalidArgumentException(HERE) << "Error: expected a mesh of positive volume.";
  probabilities_ = simplicesVolumes_ / meshVolume_;
  DistFunc::rDiscreteSetup(probabilities_, base_, alias_);
  epsilon_ = ResourceMap::GetAsScalar("UniformOverMesh-OnManifoldEpsilon");
  if (intrinsicDimension == dimension)
    meshDomain_ = MeshDomain(mesh);
  const UnsignedInteger maximumIntegrationNumber = ResourceMap::GetAsUnsignedInteger("UniformOverMesh-MaximumIntegrationNodesNumber");
  const UnsignedInteger maximumNumber = static_cast< UnsignedInteger > (round(std::pow(maximumIntegrationNumber, 1.0 / getDimension())));
  const UnsignedInteger candidateNumber = ResourceMap::GetAsUnsignedInteger("UniformOverMesh-MarginalIntegrationNodesNumber");
  if (candidateNumber > maximumNumber) LOGWARN(OSS() << "The requested number of marginal integration nodes=" << candidateNumber << " would lead to an excessive number of integration nodes=" << std::pow(candidateNumber, 1.0 * getDimension()) << ". It has been reduced to " << maximumNumber << ". You should increase the ResourceMap key \"UniformOverMesh-MaximumIntegrationNodesNumber\" or decrease the ResourceMap key \"UniformOverMesh-MarginalIntegrationNodesNumber\"");
  integrationAlgorithm_ = GaussLegendre(Indices(getDimension(), std::min(maximumNumber, candidateNumber)));
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computeRange();
}

Mesh UniformOverMesh::getMesh() const
{
  return mesh_;
}

UnsignedInteger UniformOverMesh::getIntrinsicDimension() const
{
  return intrinsicDimension_;
}

Scalar UniformOverMesh::getVolume() const
{
  return meshVolume_;
}

/* Integration algorithm accessor */
void UniformOverMesh::setIntegrationAlgorithm(const IntegrationAlgorithm & integrationAlgorithm)
{
  integrationAlgorithm_ = integrationAlgorithm;
}

IntegrationAlgorithm UniformOverMesh::getIntegrationAlgorithm() const
{
  return integrationAlgorithm_;
}

Point UniformOverMesh::getParameter() const
{
  return {};
}

void UniformOverMesh::setParameter(const Point & parameter)
{
  if (parameter.getDimension())
    throw InvalidArgumentException(HERE) << "UniformOverMesh parameter must be of dimension 0";
}

Description UniformOverMesh::getParameterDescription() const
{
  return {};
}

/* Check if the point belongs to the affine hull of the simplex at the
   given index, and if so give its barycentric coordinates */
Bool UniformOverMesh::isOnSimplex(const Point & point,
                                  const UnsignedInteger index,
                                  Point & barycentricCoordinates) const
{
  const UnsignedInteger dimension = intrinsicDimension_;
  const Point v0(vertices_[simplices_(index, 0)]);
  // Compute the Gram matrix of the edges and the right hand side
  const Point q(point - v0);
  Point rhs(dimension);
  SquareMatrix gram(dimension);
  Scalar maxEdgeSquared = 0.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const UnsignedInteger indexI = simplices_(index, i + 1);
    const Point edgeI(vertices_[indexI] - v0);
    maxEdgeSquared = std::max(maxEdgeSquared, edgeI.normSquare());
    rhs[i] = edgeI.dot(q);
    for (UnsignedInteger j = i; j < dimension; ++j)
    {
      const UnsignedInteger indexJ = simplices_(index, j + 1);
      const Point edgeJ(vertices_[indexJ] - v0);
      const Scalar value = edgeI.dot(edgeJ);
      gram(i, j) = value;
      gram(j, i) = value;
    }
  }
  if (!(maxEdgeSquared > 0.0)) return false;
  const Scalar lengthScale = std::sqrt(maxEdgeSquared);
  const Scalar tolerance = epsilon_ * lengthScale;
  // The simplex is degenerate
  if (!(gram.computeDeterminant() > 0.0)) return false;
  const Point edgeCoordinates(gram.solveLinearSystem(rhs));
  // Rebuild the projection of the point onto the affine hull of the simplex
  const UnsignedInteger dimensionPoint = point.getDimension();
  Point projection(v0);
  Scalar firstCoordinate = 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    firstCoordinate -= edgeCoordinates[i];
    const UnsignedInteger indexI = simplices_(index, i + 1);
    const Point vertexI(vertices_[indexI]);
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

/* Check if the point belongs to the mesh */
Bool UniformOverMesh::isOnMesh(const Point & point) const
{
  const UnsignedInteger simplicesNumber = simplices_.getSize();
  for (UnsignedInteger i = 0; i < simplicesNumber; ++i)
  {
    Point coordinates;
    if (isOnSimplex(point, i, coordinates)) return true;
  }
  return false;
}

/* Method save() stores the object through the StorageManager */
void UniformOverMesh::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "mesh_", mesh_ );
  adv.saveAttribute( "integrationAlgorithm_", integrationAlgorithm_ );
}

/* Method load() reloads the object from the StorageManager */
void UniformOverMesh::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  Mesh mesh;
  adv.loadAttribute( "mesh_", mesh );
  adv.loadAttribute( "integrationAlgorithm_", integrationAlgorithm_ );
  setMesh(mesh);
}

END_NAMESPACE_OPENTURNS