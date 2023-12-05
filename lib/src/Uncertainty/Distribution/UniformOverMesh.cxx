//                                               -*- C++ -*-
/**
 *  @brief The UniformOverMesh distribution
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <numeric>

#include "openturns/UniformOverMesh.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/RegularGrid.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/GaussLegendre.hxx"
#include "openturns/PersistentObjectFactory.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(UniformOverMesh)

static const Factory<UniformOverMesh> Factory_UniformOverMesh;

/* Default constructor */
UniformOverMesh::UniformOverMesh()
  : ContinuousDistribution()
{
  setName( "UniformOverMesh" );
  setMesh(RegularGrid(0.0, 1.0, 2));
}

/* Parameters constructor */
UniformOverMesh::UniformOverMesh(const Mesh & mesh)
  : ContinuousDistribution()
{
  setName( "UniformOverMesh" );
  setMesh(mesh);
}

/* Comparison operator */
Bool UniformOverMesh::operator ==(const UniformOverMesh & other) const
{
  if (this == &other) return true;
  return (mesh_ == other.mesh_);
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
  if (dimension == 1)
  {
    for (UnsignedInteger n = 0; n < size; ++n)
    {
      const UnsignedInteger index = indices[n];
      DistFunc::rUniformSegment(&vertices_(simplices_(index, 0), 0), &vertices_(simplices_(index, 1), 0), dimension, &result(n, 0));
    } // n
    return result;
  } // dimension == 1
  if (dimension == 2)
  {
    for (UnsignedInteger n = 0; n < size; ++n)
    {
      const UnsignedInteger index = indices[n];
      DistFunc::rUniformTriangle(&vertices_(simplices_(index, 0), 0), &vertices_(simplices_(index, 1), 0), &vertices_(simplices_(index, 2), 0), dimension, &result(n, 0));
    }
    return result;
  } // dimension == 2
  if (dimension == 3)
  {
    for (UnsignedInteger n = 0; n < size; ++n)
    {
      const UnsignedInteger index = indices[n];
      DistFunc::rUniformTetrahedron(&vertices_(simplices_(index, 0), 0), &vertices_(simplices_(index, 1), 0), &vertices_(simplices_(index, 2), 0), &vertices_(simplices_(index, 3), 0), dimension, &result(n, 0));
    }
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
  Scalar pdf = 0.0;
  if (meshDomain_.contains(point))
    pdf = 1.0 / meshVolume_;
  return pdf;
}


/* Get the CDF of the distribution, i.e. P(X <= point) = CDF(point) */
Scalar UniformOverMesh::computeCDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();
  Scalar cdf = 0.0;
  // Waiting for a better implementation
  const Interval quadrant(Point(getDimension(), -SpecFunc::MaxScalar), point);
  const Interval intersection(quadrant.intersect(getRange()));
  if (intersection == getRange()) cdf = 1.0;
  else if (!intersection.isEmpty())
    cdf = integrationAlgorithm_.integrate(PDFWrapper(this), intersection)[0];
  return cdf;
}


/* Compute the mean of the distribution */
void UniformOverMesh::computeMean() const
{
  const UnsignedInteger dimension = getDimension();
  mean_ = Point(dimension);
  for (UnsignedInteger i = 0; i < simplicesVolumes_.getSize(); ++i)
  {
    const Scalar factor = simplicesVolumes_[i] / (meshVolume_ * (1.0 + dimension));
    for (UnsignedInteger j = 0; j <= dimension; ++j)
    {
      const UnsignedInteger vertexIndex = simplices_(i, j);
      for (UnsignedInteger k = 0; k < dimension; ++k)
        mean_[k] += vertices_(vertexIndex, k) * factor;
    } // j
  } // i
  isAlreadyComputedMean_ = true;
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
  meshDomain_ = MeshDomain(mesh);
  simplicesVolumes_ = mesh.computeSimplicesVolume();
  meshVolume_ = std::accumulate(simplicesVolumes_.begin(), simplicesVolumes_.end(), 0.0);
  probabilities_ = simplicesVolumes_ / meshVolume_;
  DistFunc::rDiscreteSetup(probabilities_, base_, alias_);
  const UnsignedInteger maximumIntegrationNumber = ResourceMap::GetAsUnsignedInteger("UniformOverMesh-MaximumIntegrationNodesNumber");
  const UnsignedInteger maximumNumber = static_cast< UnsignedInteger > (round(std::pow(maximumIntegrationNumber, 1.0 / getDimension())));
  const UnsignedInteger candidateNumber = ResourceMap::GetAsUnsignedInteger("UniformOverMesh-MarginalIntegrationNodesNumber");
  if (candidateNumber > maximumNumber) LOGWARN(OSS() << "Warning! The requested number of marginal integration nodes=" << candidateNumber << " would lead to an excessive number of integration nodes=" << std::pow(candidateNumber, 1.0 * getDimension()) << ". It has been reduced to " << maximumNumber << ". You should increase the ResourceMap key \"UniformOverMesh-MaximumIntegrationNodesNumber\" or decrease the ResourceMap key \"UniformOverMesh-MarginalIntegrationNodesNumber\"");
  integrationAlgorithm_ = GaussLegendre(Indices(getDimension(), std::min(maximumNumber, candidateNumber)));
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computeRange();
}

Mesh UniformOverMesh::getMesh() const
{
  return mesh_;
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

/* Method save() stores the object through the StorageManager */
void UniformOverMesh::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "mesh_", mesh_ );
  adv.saveAttribute( "integrationAlgorithm_", integrationAlgorithm_ );
}

/* Method load() reloads the object from the StorageManager */
void UniformOverMesh::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  Mesh mesh;
  adv.loadAttribute( "mesh_", mesh );
  adv.loadAttribute( "integrationAlgorithm_", integrationAlgorithm_ );
  setMesh(mesh);
}

END_NAMESPACE_OPENTURNS
