//                                               -*- C++ -*-
/**
 *  @brief The TruncatedOverMesh distribution
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/TruncatedOverMesh.hxx"
#include "openturns/Domain.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/RegularGrid.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/GaussLegendre.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SimplicialCubature.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TruncatedOverMesh)

static const Factory<TruncatedOverMesh> Factory_TruncatedOverMesh;

/* Default constructor */
TruncatedOverMesh::TruncatedOverMesh()
  : ContinuousDistribution()
{
  setName("TruncatedOverMesh");
  setMesh(RegularGrid(0.0, 1.0, 2));
}

/* Parameters constructor */
TruncatedOverMesh::TruncatedOverMesh(const Distribution & distribution, const Mesh & mesh)
  : ContinuousDistribution()
  , distribution_(distribution)
{
  setName("TruncatedOverMesh");
  setMesh(mesh);
}

/* Comparison operator */
Bool TruncatedOverMesh::operator ==(const TruncatedOverMesh & other) const
{
  if (this == &other) return true;
  return (distribution_ == other.distribution_) && (mesh_ == other.mesh_);
}

Bool TruncatedOverMesh::equals(const DistributionImplementation & other) const
{
  const TruncatedOverMesh* p_other = dynamic_cast<const TruncatedOverMesh*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String TruncatedOverMesh::__repr__() const
{
  OSS oss(true);
  oss << "class=" << TruncatedOverMesh::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " distribution=" << distribution_
      << " mesh=" << mesh_;
  return oss;
}

String TruncatedOverMesh::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(distribution = " << distribution_.__str__() << ", mesh = " << mesh_.__str__() << ")";
  return oss;
}

/* Virtual constructor */
TruncatedOverMesh * TruncatedOverMesh::clone() const
{
  return new TruncatedOverMesh(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void TruncatedOverMesh::computeRange()
{
  const Interval range(Interval(mesh_.getLowerBound(), mesh_.getUpperBound()).intersect(distribution_.getRange()));
  setRange(range);
}


/** Build the sample of vertices for i-th simplex */
Sample TruncatedOverMesh::getSimplexVertices(const UnsignedInteger index) const
{
  const Indices simplex(mesh_.getSimplex(index));
  Sample simplexVertices(0, mesh_.getDimension());
  for (UnsignedInteger j = 0; j < simplex.getSize(); ++ j)
    simplexVertices.add(mesh_.getVertex(simplex[j]));
  return simplexVertices;
}


// maps the unit hypercube to a simplex
class TruncatedOverMeshSimplexTransformationEvaluation: public EvaluationImplementation
{
public:
  explicit TruncatedOverMeshSimplexTransformationEvaluation(const Sample & vertices)
    : EvaluationImplementation()
    , vertices_(vertices)
  {
    // Nothing to do
  }

  TruncatedOverMeshSimplexTransformationEvaluation * clone() const override
  {
    return new TruncatedOverMeshSimplexTransformationEvaluation(*this);
  }

  Point operator() (const Point & point) const override
  {
    // xref "Simplex-stochastic collocation method with improved scalability", Appendix D. Proof of uniform distribution
    const UnsignedInteger dimension = getInputDimension();
    Point result(vertices_[0]);
    if (dimension == 1)
      result += SpecFunc::Clip01(point[0]) * (vertices_[1] - vertices_[0]);
    else
    {
      for (UnsignedInteger i = 1; i < dimension + 1; ++ i)
      {
        Scalar prod = 1.0;
        for (UnsignedInteger j = 1; j < i + 1; ++ j)
          prod *= std::pow(SpecFunc::Clip01(point[dimension - j], 1.0), 1.0 / (dimension - j + 1));
        result += prod * (vertices_[i] - vertices_[i - 1]);
      }
    }
    return result;
  }

  UnsignedInteger getInputDimension() const override
  {
    return vertices_.getDimension();
  }

  UnsignedInteger getOutputDimension() const override
  {
    return vertices_.getDimension();
  }

private:
  Sample vertices_;
};

/* Get one realization of the distribution */
Point TruncatedOverMesh::getRealization() const
{
  // pick a simplex
  const UnsignedInteger index = DistFunc::rDiscrete(base_, alias_);
  const Sample simplexVertices(getSimplexVertices(index));

  Point x;
  Scalar y = 0.0;

  // xref "A Guide to Simulation", P. Bratley, 5.2.6: Generalized rejection
  do
  {
    // draw x uniformly in the simplex
    x = DistFunc::rUniformSimplex(simplexVertices);

    // draw y in [0, pdfsup]
    y = RandomGenerator::Generate() * pdfSup_[index];    

  } while (y > distribution_.computePDF(x));

  return x;
}

/* Get a sample of the distribution */
Sample TruncatedOverMesh::getSample(const UnsignedInteger size) const
{
  return ContinuousDistribution::getSample(size);
}


/* Get the PDF of the distribution, i.e. P(point < X < point+dx) = PDF(point)dx + o(dx) */
Scalar TruncatedOverMesh::computePDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();
  Scalar pdf = 0.0;
  if (meshDomain_.contains(point))
    pdf = distribution_.computePDF(point) * normalizationFactor_;
  return pdf;
}


/* Get the CDF of the distribution, i.e. P(X <= point) = CDF(point) */
Scalar TruncatedOverMesh::computeCDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  Scalar cdf = 0.0;
  const Interval quadrant(Point(getDimension(), -SpecFunc::MaxScalar), point);
#if 1
  // Waiting for a better implementation
  const Interval intersection(quadrant.intersect(getRange()));
  if (intersection == getRange()) cdf = 1.0;
  else if (!intersection.isEmpty())
    cdf = integrationAlgorithm_.integrate(PDFWrapper(this), intersection)[0];
#else
  // integrate pdf over simplices inside the [-inf, X] quadrant
  SimplicialCubature integrationAlgorithm;
  const UnsignedInteger simplicesNumber = mesh_.getSimplicesNumber();
  for (UnsignedInteger i = 0; i < simplicesNumber; ++ i)
  {
    const Sample simplexVertices(getSimplexVertices(i));
    // TODO: split simplices that have vertices both inside & outside the quadrant
    if (!quadrant.contains(simplexVertices.getMax()))
      continue;
    Indices simplexIndices(simplexVertices.getSize());
    simplexIndices.fill();
    const Mesh simplexMesh(simplexVertices, IndicesCollection(Collection<Indices>(1, simplexIndices)));
    cdf += integrationAlgorithm.integrate(PDFWrapper(distribution_.getImplementation()->clone()), simplexMesh)[0];
  }
#endif
  return cdf;
}


/* Compute the mean of the distribution */
void TruncatedOverMesh::computeMean() const
{
  // integrate x*f(x) using the cubature on mesh
  Point mean(dimension_);
  const SimplicialCubature algo;
  for(UnsignedInteger component = 0; component < dimension_; ++component)
  {
    const Implementation marginalDistribution(distribution_.getMarginal(component).getImplementation());
    const ShiftedMomentWrapper integrand(1, 0.0, marginalDistribution);
    mean[component] = algo.integrate(integrand, mesh_)[0];
  }
  mean_ = mean;
  isAlreadyComputedMean_ = true;
}


/* Mesh accessor */
void TruncatedOverMesh::setMesh(const Mesh & mesh)
{
  const UnsignedInteger dimension = mesh.getDimension();
  if (dimension != distribution_.getDimension())
    throw InvalidArgumentException(HERE) << "Error: expected a mesh of dimension " << distribution_.getDimension();
  setDimension(dimension);
  mesh_ = mesh;
  meshDomain_ = MeshDomain(mesh);

  // compute the weighted probability of the simplices
  const UnsignedInteger simplicesNumber = mesh.getSimplicesNumber();
  probabilities_.resize(simplicesNumber);
  pdfSup_.resize(simplicesNumber);
  for (UnsignedInteger i = 0; i < simplicesNumber; ++ i)
  {
    // integrate the pdf over the simplex via the unit hypercube transformation
    const Sample simplexVertices(getSimplexVertices(i));
    Indices simplexIndices(simplexVertices.getSize());
    simplexIndices.fill();
    const SimplicialCubature integrationAlgorithm;
    const Mesh simplexMesh(simplexVertices, IndicesCollection(Collection<Indices>(1, simplexIndices)));
    probabilities_[i] = integrationAlgorithm.integrate(PDFWrapper(distribution_.getImplementation()->clone()), simplexMesh)[0];

    // look for pdf maxima over the simplex in the same way
    const Function simplexTransform(new TruncatedOverMeshSimplexTransformationEvaluation(simplexVertices));
    const ComposedFunction pdfUnitCube(PDFWrapper(distribution_.getImplementation()->clone()), simplexTransform);
    OptimizationProblem problem(pdfUnitCube);
    problem.setMinimization(false);
    problem.setBounds(Interval(dimension));
    const String solverName = ResourceMap::GetAsString("TruncatedOverMesh-OptimizationAlgorithm");
    OptimizationAlgorithm solver(OptimizationAlgorithm::Build(solverName));
    solver.setProblem(problem);
    solver.setStartingPoint(Point(dimension, 0.5)); // start from median
    solver.run();
    OptimizationResult result(solver.getResult());
    pdfSup_[i] = result.getOptimalValue()[0];
  }
  normalizationFactor_ = 1.0 / std::accumulate(probabilities_.begin(), probabilities_.end(), 0.0);
  probabilities_ *= normalizationFactor_;

  DistFunc::rDiscreteSetup(probabilities_, base_, alias_);

  const UnsignedInteger maximumIntegrationNumber = ResourceMap::GetAsUnsignedInteger("TruncatedOverMesh-MaximumIntegrationNodesNumber");
  const UnsignedInteger maximumNumber = static_cast< UnsignedInteger > (round(std::pow(maximumIntegrationNumber, 1.0 / getDimension())));
  const UnsignedInteger candidateNumber = ResourceMap::GetAsUnsignedInteger("TruncatedOverMesh-MarginalIntegrationNodesNumber");
  if (candidateNumber > maximumNumber) LOGWARN(OSS() << "Warning! The requested number of marginal integration nodes=" << candidateNumber << " would lead to an excessive number of integration nodes=" << std::pow(candidateNumber, 1.0 * getDimension()) << ". It has been reduced to " << maximumNumber << ". You should increase the ResourceMap key \"TruncatedOverMesh-MaximumIntegrationNodesNumber\" or decrease the ResourceMap key \"TruncatedOverMesh-MarginalIntegrationNodesNumber\"");
  integrationAlgorithm_ = GaussLegendre(Indices(getDimension(), std::min(maximumNumber, candidateNumber)));
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computeRange();
}

Mesh TruncatedOverMesh::getMesh() const
{
  return mesh_;
}

/* Method save() stores the object through the StorageManager */
void TruncatedOverMesh::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "distribution_", distribution_ );
  adv.saveAttribute( "mesh_", mesh_ );
}

/* Method load() reloads the object from the StorageManager */
void TruncatedOverMesh::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  Mesh mesh;
  adv.loadAttribute( "distribution_", distribution_ );
  adv.loadAttribute( "mesh_", mesh );
  setMesh(mesh);
}

END_NAMESPACE_OPENTURNS
