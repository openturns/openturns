//                                               -*- C++ -*-
/**
 *  @brief This class implements the computation of the Karhunen-Loeve
 *         basis and eigenvalues of a given covariance model based on
 *         quadrature approximation.
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
#include "openturns/KarhunenLoeveQuadratureAlgorithm.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/SquareComplexMatrix.hxx"
#include "openturns/Sample.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/ComposedDistribution.hxx"
#include "openturns/GaussProductExperiment.hxx"
#include "openturns/HyperbolicAnisotropicEnumerateFunction.hxx"
#include "openturns/OrthogonalProductPolynomialFactory.hxx"
#include "openturns/Basis.hxx"
#include "openturns/LegendreFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/LinearCombinationFunction.hxx"
#include "openturns/DualLinearCombinationFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KarhunenLoeveQuadratureAlgorithm
 */

CLASSNAMEINIT(KarhunenLoeveQuadratureAlgorithm)

static const Factory<KarhunenLoeveQuadratureAlgorithm> Algorithm_KarhunenLoeveQuadratureAlgorithm;

/* Constructor without parameters */
KarhunenLoeveQuadratureAlgorithm::KarhunenLoeveQuadratureAlgorithm()
  : KarhunenLoeveAlgorithmImplementation()
  , domain_()
  , experiment_()
  , basis_()
  , mustScale_(false)
{
  // Nothing to do
}

/* Constructor with parameters */
KarhunenLoeveQuadratureAlgorithm::KarhunenLoeveQuadratureAlgorithm(const Domain & domain,
    const Interval & domainBounds,
    const CovarianceModel & covariance,
    const WeightedExperiment & experiment,
    const FunctionCollection & basis,
    const Bool mustScale,
    const Scalar threshold)
  : KarhunenLoeveAlgorithmImplementation(covariance, threshold)
  , domain_(domain)
  , domainLowerBound_(domainBounds.getLowerBound())
  , domainUpperBound_(domainBounds.getUpperBound())
  , experiment_(experiment)
  , basis_(basis)
  , mustScale_(mustScale)
{
  // Check the arguments
  const UnsignedInteger dimension = domain.getDimension();
  const Distribution distribution(experiment.getDistribution());
  if (dimension != distribution.getDimension()) throw InvalidArgumentException(HERE) << "Error: the domain dimension=" << dimension << " does not match the distribution dimension=" << distribution.getDimension() << " of the weighted experiment";
  if (domainBounds.isNumericallyEmpty()) throw InvalidArgumentException(HERE) << "Error: the given domain is numerically empty.";
}

/* Constructor with parameters specialized to the case of Legendre polynomials and Gauss product quadrature */
KarhunenLoeveQuadratureAlgorithm::KarhunenLoeveQuadratureAlgorithm(const Domain & domain,
    const Interval & domainBounds,
    const CovarianceModel & covariance,
    const UnsignedInteger marginalDegree,
    const Scalar threshold)
  : KarhunenLoeveAlgorithmImplementation(covariance, threshold)
  , domain_(domain)
  , domainLowerBound_(domainBounds.getLowerBound())
  , domainUpperBound_(domainBounds.getUpperBound())
  , experiment_(GaussProductExperiment(ComposedDistribution(Collection<Distribution>(domain.getDimension(), Uniform())), Indices(domain.getDimension(), marginalDegree + 1)))
  , mustScale_(true)
{
  // Check the arguments
  const UnsignedInteger dimension = domain.getDimension();
  const Distribution distribution(experiment_.getDistribution());
  if (dimension != distribution.getDimension()) throw InvalidArgumentException(HERE) << "Error: the domain dimension=" << dimension << " does not match the distribution dimension=" << distribution.getDimension() << " of the weighted experiment";
  if (domainBounds.isNumericallyEmpty()) throw InvalidArgumentException(HERE) << "Error: the given domain is numerically empty.";


  // Here we have to use the double/double version of std::pow to make VC++ happy. Grrr.
  const UnsignedInteger basisSize = static_cast<UnsignedInteger>(std::floor(0.5 + std::pow(1.0 * marginalDegree, 1.0 * dimension)));
  const Basis basis(OrthogonalProductPolynomialFactory(Collection<OrthogonalUniVariatePolynomialFamily>(dimension, LegendreFactory()), HyperbolicAnisotropicEnumerateFunction(dimension, SpecFunc::MaxScalar)));
  for(UnsignedInteger i = 0; i < basisSize; ++i)
  {
    basis_.add(basis.build(i));
  }
}

/* Virtual constructor */
KarhunenLoeveQuadratureAlgorithm * KarhunenLoeveQuadratureAlgorithm::clone() const
{
  return new KarhunenLoeveQuadratureAlgorithm( *this );
}

/* Solve the Fredholm eigenvalues problem:
   \int_{\Omega}C(s,t)\phi_n(s)ds=\lambda_n\phi_n(t)
   using a quadrature approximation of the integrals over a given basis:
   \int_{\Omega}C(s,t)\phi_n(s)ds=\sum_{l=1}^Lw_lC(\xi_l,t)\phi_n(\xi_l)1_{\Omega}(\xi_n)/p(\xi_n)
   where (w_n,\xi_n) is the quadrature rule associated with the density p, p being such that p>0 over \Omega.
   The eignefunctions \phi_n are projected onto the basis \theta_k
   \phi_n(t)=\sum_k\alpha_k^n\theta_k(t)
   leading to:
   \forall t\in\Omega,
   \sum_{l=1}^Lw_lC(\xi_l,t)\sum_k\alpha_k^n\theta(\xi_l)1_{\Omega}(\xi_l)/p(\xi_l)=\lambda_n\sum_k\alpha_k^n\theta(t)
   then we write that the dot product wrt any \theta_j of both sides are equal:
   \sum_{l'=1}^Lw_l'[\sum_{l=1}^Lw_lC(\xi_l,\xi_l')\sum_k\alpha_k^n\theta(\xi_l)1_{\Omega}(\xi_l)/p(\xi_l)]\theta_j'(\xi_l')1_{\Omega}(\xi_l')/p(\xi_l')=\lambda_n\sum_k\alpha_k^n\sum_{l'=1}^Lw_l'\theta_k(\xi_l')\theta_j(\xi_l')1_{\Omega}(\xi_l')/p(\xi_l')
   introducing \eta_l=\sqrt{1_{\Omega}(\xi_l)w_l(\xi_n)/p(\xi_n)} it writes:
   \sum_k\alpha_k^n\sum_{l'=1}^L\sum_{l=1}^L[\eta_l\theta(\xi_l)]\eta_lC(\xi_l,\xi_l')\eta_l'[\eta_l'\theta_j'(\xi_l')]=\lambda_n\sum_k\alpha_k^n\sum_{l'=1}^L\eta_l'\theta_k(\xi_l')\eta_l'\theta_j(\xi_l')
   In a block-matrix form we get:
   [C(\xi_1,\xi_1) ... C(\xi_1,\xi_L)][K_11 ... K_1N][\alpha_1]             [\alpha_1]
   [    ...                  ...     ][ ...      ...][   ...  ] = \lambda_n [   ...  ]
   [C(\xi_N,\xi_1) ... C(\xi_N,\xi_N)][K_N1 ... K_NN][\alpha_N]             [\alpha_N]
   Where:
   K_ij = \int_{\Omega}\theta_i(s)\theta_j(s)ds I
   with I the dxd identity matrix
*/
void KarhunenLoeveQuadratureAlgorithm::run()
{
  const UnsignedInteger domainDimension = domain_.getDimension();
  const Distribution distribution(experiment_.getDistribution());
  // First thing to do: build a linear transformation that maps the range of the distribution associated with the weighted experiment to the bounding box of the domain
  const Point distributionLowerBound(distribution.getRange().getLowerBound());
  const Point distributionUpperBound(distribution.getRange().getUpperBound());
  const Bool hasSameBounds = (domainLowerBound_ == distributionLowerBound) && (domainUpperBound_ == distributionUpperBound);
  // The function scaling maps points in the range of the distribution into the domain
  Function scaling;
  Function inverseScaling;
  // Normalization factor takes into account the fact that we map the range of the distribution defining the weighted experiment with the bounding box of the domain
  Scalar normalizationFactor = 1.0;
  if (!hasSameBounds)
  {
    TriangularMatrix T(domainDimension);
    TriangularMatrix inverseT(domainDimension);
    const Point center((distributionUpperBound + distributionLowerBound) * 0.5);
    const Point constant((domainUpperBound_ + domainLowerBound_) * 0.5);
    for (UnsignedInteger i = 0; i < domainDimension; ++i)
    {
      T(i, i) = (domainUpperBound_[i] - domainLowerBound_[i]) / (distributionUpperBound[i] - distributionLowerBound[i]);
      normalizationFactor *= T(i, i);
      inverseT(i, i) = 1.0 / T(i, i);
    }
    scaling = LinearFunction(center, constant, T);
    inverseScaling = LinearFunction(constant, center, inverseT);
  }
  // Here we set the collection of functions
  Collection<Function> coll(basis_.getSize());
  for (UnsignedInteger i = 0; i < basis_.getSize(); ++i)
    if (!hasSameBounds && mustScale_) coll[i] = ComposedFunction(basis_[i], inverseScaling);
    else coll[i] = basis_[i];
  // Compute the integration nodes and weights
  Point rawWeights;
  WeightedExperiment experimentCopy(experiment_);
  LOGINFO("Generate the weighted experiment");
  Sample rawNodes(experimentCopy.generateWithWeights(rawWeights));
  LOGINFO(OSS(false) << "Initial number of integration nodes=" << rawNodes.getSize());
  LOGINFO("Generate the pdf");
  const Sample pdf(distribution.computePDF(rawNodes));
  if (!hasSameBounds) rawNodes = scaling(rawNodes);
  // Update the weights in order to match Lebesgue distribution on the domain
  // We keep only the nodes inside of the domain
  Sample nodes(0, domainDimension);
  Point weights(0);
  LOGINFO("Filter the integration nodes");
  for (UnsignedInteger i = 0; i < rawWeights.getDimension(); ++i)
  {
    const Bool isInside = domain_.contains(rawNodes[i]);
    if (isInside)
    {
      nodes.add(rawNodes[i]);
      weights.add(std::sqrt(normalizationFactor * rawWeights[i] / pdf(i, 0)));
    }
  }
  const UnsignedInteger nodesNumber = nodes.getSize();
  // Reduce the scope of quadrature to free the data as soon as possible
  {
    Sample quadrature(nodes);
    SampleImplementation weightsAsSample(nodesNumber, 1);
    weightsAsSample.setData(weights);
    quadrature.stack(weightsAsSample);
    quadrature = quadrature.sortAccordingToAComponent(0);
    Indices full(domainDimension);
    full.fill();
    nodes = quadrature.getMarginal(full);
    weights = quadrature.getMarginal(domainDimension).getImplementation()->getData();
  }
  // Now we compute the basis functions over the nodes, properly scaled
  LOGINFO(OSS(false) << "Final number of integration nodes=" << nodesNumber);
  if (nodesNumber == 0) throw InternalException(HERE) << "Error: cannot compute a Karhunen Loeve decomposition with zero integration node.";
  LOGINFO("Compute the design matrix");
  MatrixImplementation scaledTheta(nodesNumber, basis_.getSize());
  UnsignedInteger indexTheta = 0;
  // scaledTheta(i,j)=w_i\theta_j(\xi_i)
  for (UnsignedInteger j = 0; j < basis_.getSize(); ++j)
  {
    const Point thetaj(coll[j](nodes).getImplementation()->getData());
    for (UnsignedInteger i = 0; i < nodesNumber; ++i)
    {
      scaledTheta[indexTheta] = thetaj[i] * weights[i];
      ++indexTheta;
    }
  }
  // Compute the Cholesky factor L of \theta^t\diag(w_i)^2\theta, ie LL^t=\theta^t\diag(w_i)^2\theta
  LOGINFO("Compute the Cholesky factor of the Gram matrix");
  CovarianceMatrix gram(scaledTheta.computeGram(true));
  const Scalar epsilon = ResourceMap::GetAsScalar("KarhunenLoeveQuadratureAlgorithm-RegularizationFactor");
  if (epsilon > 0.0)
    for (UnsignedInteger i = 0; i < gram.getDimension(); ++i) gram(i, i) += epsilon;
  TriangularMatrix cholesky(gram.computeCholesky(false));

  // Discretize the covariance model
  LOGINFO("Discretize the covariance matrix");
  const UnsignedInteger dimension = covariance_.getOutputDimension();
  CovarianceMatrix C(covariance_.discretize(nodes));
  // Prepare the generalized eigenvalues problem
  // Step 1: scale C by the weights
  LOGINFO("Scale the covariance matrix");
  if (dimension == 1)
  {
    for (UnsignedInteger j = 0; j < nodesNumber; ++j)
      for (UnsignedInteger i = j; i < nodesNumber; ++i)
      {
        const Scalar factor = weights[i] * weights[j];
        C(i, j) *= factor;
      } // i
  } // dimension == 1
  else
  {
    for (UnsignedInteger j = 0; j < nodesNumber; ++j)
      for (UnsignedInteger i = j; i < nodesNumber; ++i)
      {
        const Scalar factor = weights[i] * weights[j];
        for (UnsignedInteger m = 0; m < dimension; ++m)
          for (UnsignedInteger n = 0; n < dimension; ++n)
            C(m + i * dimension, n + j * dimension) *= factor;
      } // i
  } // dimension > 1
  // Step 2: take theta into account
  Matrix omega;
  LOGINFO("Scale the design matrix");
  // If dimension == 1 we can use scaledTheta directly
  if (dimension == 1) omega = scaledTheta;
  // Here we have to expand scaledTheta if dimension > 1
  else
  {
    omega = Matrix(nodesNumber * dimension, basis_.getSize() * dimension);
    for (UnsignedInteger j = 0; j < basis_.getSize(); ++j)
      for (UnsignedInteger i = 0; i < nodesNumber; ++i)
      {
        const Scalar value = scaledTheta(i, j);
        for (UnsignedInteger k = 0; k < dimension; ++k)
          omega(k + i * dimension, k + j * dimension) = value;
      }
  }
  // M.genProd(N, f1, f2) =
  // M  .N   si f1=false, f2=false
  // M^t.N   si f1=true,  f2=false
  // M  .N^t si f1=false, f2=true
  // M^t.N^t si f1=true,  f2=true
  LOGINFO("Build the lhs matrix of the generalized eigenvalues problem");
  C = omega.getImplementation()->genProd(*(C * omega).getImplementation(), true, false);
  // Cholesky decomposition of scaledTheta
  LOGINFO("Expand the Cholesky factor");
  TriangularMatrix choleskyBlock;
  // If dimension == 1 we can use cholesky directly
  if (dimension == 1) choleskyBlock = cholesky;
  // Here we have to expand cholesky if dimension > 1
  else
  {
    choleskyBlock = TriangularMatrix(basis_.getSize() * dimension);
    for (UnsignedInteger j = 0; j < basis_.getSize(); ++j)
      for (UnsignedInteger i = j; i < basis_.getSize(); ++i)
      {
        const Scalar value = cholesky(i, j);
        for (UnsignedInteger k = 0; k < dimension; ++k)
          choleskyBlock(k + i * dimension, k + j * dimension) = value;
      }
  }
  // Step 3: reduction to a standard eigenvalues problem
  // A = L^{-1}CL^{-t}
  //   = L^{-1}((CL^{-t})^t)^t as (A^t)^t=A
  //   = L^{-1}(L^{-1}C^t)^t as (MN)^t = N^tM^t
  //   = L^{-1}(L^{-1}C)^t as C^t = C
  LOGINFO("Reduce the generalized eigenvalue problem to a standard eigenvalues problem");
  C = choleskyBlock.solveLinearSystem((choleskyBlock.solveLinearSystem(C)).transpose()).getImplementation();
  SquareMatrix eigenVectors;
  // Last time we need C, so we can overwrite it by eigenVectors
  LOGINFO("Solve the standard eigenvalues problem");
  Point eigenValues(C.computeEV(eigenVectors, false));
  const UnsignedInteger augmentedDimension = eigenVectors.getDimension();
  // Transform the eigenvectors to the generalized ones
  // Last time we need cholesky, so we can overwrite it by eigenVectors
  LOGINFO("Get the generalized eigenvectors");
  eigenVectors = choleskyBlock.transpose().solveLinearSystemInPlace(eigenVectors).getImplementation();
  LOGINFO("Post-process the eigenvalue problem");
  Collection< std::pair<Scalar, UnsignedInteger> > eigenPairs(augmentedDimension);
  for (UnsignedInteger i = 0; i < augmentedDimension; ++i)
    eigenPairs[i] = std::pair<Scalar, UnsignedInteger>(-eigenValues[i], i);
  // Sort the eigenvalues in decreasing order
  std::sort(eigenPairs.begin(), eigenPairs.end());
  Scalar cumulatedVariance = 0.0;
  for (UnsignedInteger i = 0; i < augmentedDimension; ++i)
  {
    // the eigenvalue can be close to zero but numerically negative
    eigenValues[i] = std::max(-eigenPairs[i].first, 0.0);
    cumulatedVariance += eigenValues[i];
  }
  LOGDEBUG(OSS(false) << "eigenValues=" << eigenValues);

  // Find the cut-off in the eigenvalues
  UnsignedInteger K = 0;
  Scalar selectedVariance = 0.0; // sum of eigenvalues selected after cut-off is applied
  const UnsignedInteger nbModesMax = std::min(eigenValues.getSize(), getNbModes());
  do
  {
    selectedVariance += eigenValues[K];
    ++ K;
  }
  while ((K < nbModesMax) && (selectedVariance <= (1.0 - threshold_) * cumulatedVariance));
  LOGINFO(OSS() << "Selected " << K << " eigenvalues out of " << eigenValues.getSize() << " computed");

  // Reduce and rescale the eigenvectors
  MatrixImplementation projection(K, nodesNumber * dimension);
  Point selectedEV(K);
  Collection<Function> modes;
  ProcessSample modesAsProcessSample(Mesh(nodes), 0, dimension);
  SampleImplementation values(nodesNumber, dimension);
  Point a(augmentedDimension, 1);
  Point modeValues(nodesNumber * dimension);
  for (UnsignedInteger k = 0; k < K; ++k)
  {
    selectedEV[k] = eigenValues[k];
    const UnsignedInteger initialColumn = eigenPairs[k].second;
    std::copy(eigenVectors.getImplementation()->begin() + initialColumn * augmentedDimension, eigenVectors.getImplementation()->begin() + (initialColumn + 1) * augmentedDimension, a.begin());
    // Store the eigen modes in two forms
    // modeValues = omega.a
    const Point omegaA(omega * a);
    const Scalar norm = omegaA.norm();
    const Scalar factor = omegaA[0] < 0.0 ? -1.0 / norm : 1.0 / norm;
    // Scale a
    a *= factor;
    // Compute the values of the mode
    // modeValues = \epsilon diag(1/w) omega.a / ||omega.a||
    // Build the relevant column of the transposed projection matrix
    // For k=1,...,K K is the number of selected modes
    // M_{k,:}[f(\xi_j)]=1/\sqrt{\lambda_k}\int_{\Omega}f(x)\phi_k(x)dx
    //                =1/\sqrt{\lambda_k}\sum_{j=1}^N w_jf(\xi_j)\phi_k(\xi_j)
    //                =1/\sqrt{\lambda_k}\sum_{j=1}^N w_jf(\xi_j)\sum_{i=1}^P a_i^k\theta_i(\xi_j)
    // So M is KxNd, \omega is NdxPd, eigenVectors is PdxK
    // M^t=\omega * eigenVectors
    // b = \epsilon diag(1/\sqrt{\lambda}) omega.a / ||omega.a||
    UnsignedInteger index = 0;
    const Scalar alphaK = factor / std::sqrt(selectedEV[k]);
    for (UnsignedInteger i = 0; i < nodesNumber; ++i)
    {
      const Scalar wA = factor / weights[i];
      const Scalar wB = weights[i] * alphaK;
      for (UnsignedInteger j = 0; j < dimension; ++j)
      {
        modeValues[index] = omegaA[index] * wA;
        projection(k, i * dimension + j) = omegaA[index] * wB;
        ++index;
      } // for j
    } // for i
    values.setData(modeValues);
    modesAsProcessSample.add(values);
    if (dimension == 1)
      modes.add(LinearCombinationFunction(coll, a));
    else
    {
      SampleImplementation aSample(basis_.getSize(), dimension);
      // aSample = diag(1/w) omega.a / ||omega.a||^2
      aSample.setData(a);
      modes.add(DualLinearCombinationFunction(coll, aSample));
    }
  }
  result_ = KarhunenLoeveResultImplementation(covariance_, threshold_, selectedEV, modes, modesAsProcessSample, projection, selectedVariance / cumulatedVariance);
}

/* Domain accessor */
Domain KarhunenLoeveQuadratureAlgorithm::getDomain() const
{
  return domain_;
}

/* Experiment accessor */
WeightedExperiment KarhunenLoeveQuadratureAlgorithm::getExperiment() const
{
  return experiment_;
}

/* Basis accessor */
KarhunenLoeveQuadratureAlgorithm::FunctionCollection KarhunenLoeveQuadratureAlgorithm::getBasis() const
{
  return basis_;
}

/* MustScale accessor */
Bool KarhunenLoeveQuadratureAlgorithm::getMustScale() const
{
  return mustScale_;
}

/* String converter */
String KarhunenLoeveQuadratureAlgorithm::__repr__() const
{
  OSS oss(true);
  oss << "class=" << KarhunenLoeveQuadratureAlgorithm::GetClassName()
      << ", domain=" << domain_
      << ", domainLowerBound=" << domainLowerBound_
      << ", domainUpperBound=" << domainUpperBound_;
  return oss;
}

/* String converter */
String KarhunenLoeveQuadratureAlgorithm::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << KarhunenLoeveQuadratureAlgorithm::GetClassName()
      << ", domain=" << domain_
      << ", domainLowerBound=" << domainLowerBound_
      << ", domainUpperBound=" << domainUpperBound_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void KarhunenLoeveQuadratureAlgorithm::save(Advocate & adv) const
{
  KarhunenLoeveAlgorithmImplementation::save(adv);
  adv.saveAttribute( "domain_", domain_ );
  adv.saveAttribute( "domainLowerBound_", domainLowerBound_ );
  adv.saveAttribute( "domainUpperBound_", domainUpperBound_ );
  adv.saveAttribute( "experiment_", experiment_ );
  adv.saveAttribute( "basis_", basis_ );
  adv.saveAttribute( "mustScale_", mustScale_ );
}

/* Method load() reloads the object from the StorageManager */
void KarhunenLoeveQuadratureAlgorithm::load(Advocate & adv)
{
  KarhunenLoeveAlgorithmImplementation::load(adv);
  adv.loadAttribute( "domain_", domain_ );
  adv.loadAttribute( "domainLowerBound_", domainLowerBound_ );
  adv.loadAttribute( "domainUpperBound_", domainUpperBound_ );
  adv.loadAttribute( "experiment_", experiment_ );
  adv.loadAttribute( "basis_", basis_ );
  adv.loadAttribute( "mustScale_", mustScale_ );
}

END_NAMESPACE_OPENTURNS

