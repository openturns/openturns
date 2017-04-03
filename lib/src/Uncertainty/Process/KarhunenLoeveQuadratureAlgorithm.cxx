//                                               -*- C++ -*-
/**
 *  @brief This class implements the computation of the Karhunen-Loeve
 *         basis and eigenvalues of a given covariance model based on
 *         quadrature approximation.
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
#include "openturns/KarhunenLoeveQuadratureAlgorithm.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/SquareComplexMatrix.hxx"
#include "openturns/NumericalSample.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/ComposedDistribution.hxx"
#include "openturns/GaussProductExperiment.hxx"
#include "openturns/EnumerateFunction.hxx"
#include "openturns/OrthogonalProductPolynomialFactory.hxx"
#include "openturns/LegendreFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/LinearCombinationFunction.hxx"
#include "openturns/DualLinearCombinationFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KarhunenLoeveQuadratureAlgorithm
 */

CLASSNAMEINIT(KarhunenLoeveQuadratureAlgorithm);

static const Factory<KarhunenLoeveQuadratureAlgorithm> Algorithm_KarhunenLoeveQuadratureAlgorithm;

/* Constructor without parameters */
KarhunenLoeveQuadratureAlgorithm::KarhunenLoeveQuadratureAlgorithm()
  : KarhunenLoeveAlgorithmImplementation()
  , domain_()
  , experiment_()
  , basis_()
  , basisSize_(0)
  , mustScale_(false)
{
  // Nothing to do
}

/* Constructor with parameters */
KarhunenLoeveQuadratureAlgorithm::KarhunenLoeveQuadratureAlgorithm(const Domain & domain,
    const CovarianceModel & covariance,
    const WeightedExperiment & experiment,
    const Basis & basis,
    const UnsignedInteger basisSize,
    const Bool mustScale,
    const NumericalScalar threshold)
  : KarhunenLoeveAlgorithmImplementation(covariance, threshold)
  , domain_(domain)
  , experiment_(experiment)
  , basis_(basis)
  , basisSize_(basisSize)
  , mustScale_(mustScale)
{
  // Check the arguments
  const UnsignedInteger dimension = domain.getDimension();
  const Distribution distribution(experiment.getDistribution());
  if (dimension != distribution.getDimension()) throw InvalidArgumentException(HERE) << "Error: the domain dimension=" << dimension << " does not match the distribution dimension=" << distribution.getDimension() << " of the weighted experiment";
  const NumericalPoint domainLowerBound(domain.getLowerBound());
  const NumericalPoint domainUpperBound(domain.getUpperBound());
  if (Interval(domainLowerBound, domainUpperBound).isNumericallyEmpty()) throw InvalidArgumentException(HERE) << "Error: the given domain is numerically empty.";
}

/* Constructor with parameters specialized to the case of Legendre polynomials and Gauss product quadrature */
KarhunenLoeveQuadratureAlgorithm::KarhunenLoeveQuadratureAlgorithm(const Domain & domain,
    const CovarianceModel & covariance,
    const UnsignedInteger marginalDegree,
    const NumericalScalar threshold)
  : KarhunenLoeveAlgorithmImplementation(covariance, threshold)
  , domain_(domain)
  , experiment_(GaussProductExperiment(ComposedDistribution(Collection<Distribution>(domain.getDimension(), Uniform())), Indices(domain.getDimension(), marginalDegree + 1)))
    // Here we have to use the double/double version of std::pow to make VC++ happy. Grrr.
  , basis_(OrthogonalProductPolynomialFactory(Collection<OrthogonalUniVariatePolynomialFamily>(domain.getDimension(), LegendreFactory()), EnumerateFunction(domain.getDimension(), SpecFunc::MaxNumericalScalar))), basisSize_(static_cast<UnsignedInteger>(std::floor(0.5 + std::pow(1.0*marginalDegree, 1.0*domain.getDimension()))))
  , mustScale_(true)
{
  // Check the arguments
  const UnsignedInteger dimension = domain.getDimension();
  const Distribution distribution(experiment_.getDistribution());
  if (dimension != distribution.getDimension()) throw InvalidArgumentException(HERE) << "Error: the domain dimension=" << dimension << " does not match the distribution dimension=" << distribution.getDimension() << " of the weighted experiment";
  const NumericalPoint domainLowerBound(domain.getLowerBound());
  const NumericalPoint domainUpperBound(domain.getUpperBound());
  if (Interval(domainLowerBound, domainUpperBound).isNumericallyEmpty()) throw InvalidArgumentException(HERE) << "Error: the given domain is numerically empty.";
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
  const NumericalPoint domainLowerBound(domain_.getLowerBound());
  const NumericalPoint domainUpperBound(domain_.getUpperBound());
  const NumericalPoint distributionLowerBound(distribution.getRange().getLowerBound());
  const NumericalPoint distributionUpperBound(distribution.getRange().getUpperBound());
  const Bool hasSameBounds = (domainLowerBound == distributionLowerBound) && (domainUpperBound == distributionUpperBound);
  // The function scaling maps points in the range of the distribution into the domain
  Function scaling;
  Function inverseScaling;
  // Normalization factor takes into account the fact that we map the range of the distribution defining the weighted experiment with the bounding box of the domain
  NumericalScalar normalizationFactor = 1.0;
  if (!hasSameBounds)
  {
    TriangularMatrix T(domainDimension);
    TriangularMatrix inverseT(domainDimension);
    NumericalPoint center((distributionUpperBound + distributionLowerBound) * 0.5);
    NumericalPoint constant((domainUpperBound + domainLowerBound) * 0.5);
    for (UnsignedInteger i = 0; i < domainDimension; ++i)
    {
      T(i, i) = (domainUpperBound[i] - domainLowerBound[i]) / (distributionUpperBound[i] - distributionLowerBound[i]);
      normalizationFactor *= T(i, i);
      inverseT(i, i) = 1.0 / T(i, i);
    }
    scaling = LinearFunction(center, constant, T);
    inverseScaling = LinearFunction(constant, center, inverseT);
  }
  // Here we set the collection of functions
  Collection<Function> coll(basisSize_);
  for (UnsignedInteger i = 0; i < basisSize_; ++i)
    if (!hasSameBounds && mustScale_) coll[i] = ComposedFunction(basis_.build(i), inverseScaling);
    else coll[i] = basis_.build(i);
  // Compute the integration nodes and weights
  NumericalPoint rawWeights;
  WeightedExperiment experimentCopy(experiment_);
  LOGINFO("Generate the weighted experiment");
  NumericalSample rawNodes(experimentCopy.generateWithWeights(rawWeights));
  LOGINFO(OSS(false) << "Initial number of integration nodes=" << rawNodes.getSize());
  LOGINFO("Generate the pdf");
  const NumericalSample pdf(distribution.computePDF(rawNodes));
  if (!hasSameBounds) rawNodes = scaling(rawNodes);
  // Update the weights in order to match Lebesgue distribution on the domain
  // We keep only the nodes inside of the domain
  NumericalSample nodes(0, domainDimension);
  NumericalPoint weights(0);
  LOGINFO("Filter the integration nodes");
  for (UnsignedInteger i = 0; i < rawWeights.getDimension(); ++i)
  {
    const Bool isInside = domain_.contains(rawNodes[i]);
    if (isInside)
    {
      nodes.add(rawNodes[i]);
      weights.add(std::sqrt(normalizationFactor * rawWeights[i] / pdf[i][0]));
    }
  }
  const UnsignedInteger nodesNumber = nodes.getSize();
  // Reduce the scope of quadrature to free the data as soon as possible
  {
    NumericalSample quadrature(nodes);
    NumericalSampleImplementation weightsAsSample(nodesNumber, 1);
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
  MatrixImplementation scaledTheta(nodesNumber, basisSize_);
  UnsignedInteger indexTheta = 0;
  // scaledTheta(i,j)=w_i\theta_j(\xi_i)
  for (UnsignedInteger j = 0; j < basisSize_; ++j)
  {
    const NumericalPoint thetaj(coll[j](nodes).getImplementation()->getData());
    for (UnsignedInteger i = 0; i < nodesNumber; ++i)
    {
      scaledTheta[indexTheta] = thetaj[i] * weights[i];
      ++indexTheta;
    }
  }
  // Compute the Cholesky factor L of \theta^t\diag(w_i)^2\theta, ie LL^t=\theta^t\diag(w_i)^2\theta
  LOGINFO("Compute the Cholesky factor of the Gram matrix");
  CovarianceMatrix gram(scaledTheta.computeGram(true));
  const NumericalScalar epsilon = ResourceMap::GetAsNumericalScalar("KarhunenLoeveQuadratureFactory-RegularizationFactor");
  if (epsilon > 0.0)
    for (UnsignedInteger i = 0; i < gram.getDimension(); ++i) gram(i, i) += epsilon;
  TriangularMatrix cholesky(gram.computeCholesky(false));

  // Discretize the covariance model
  LOGINFO("Discretize the covariance matrix");
  const UnsignedInteger dimension = covariance_.getDimension();
  CovarianceMatrix C(covariance_.discretize(nodes));
  // Prepare the generalized eigenvalues problem
  // Step 1: scale C by the weights
  LOGINFO("Scale the covariance matrix");
  if (dimension == 1)
  {
    for (UnsignedInteger j = 0; j < nodesNumber; ++j)
      for (UnsignedInteger i = j; i < nodesNumber; ++i)
      {
        const NumericalScalar factor = weights[i] * weights[j];
        C(i, j) *= factor;
      } // i
  } // dimension == 1
  else
  {
    for (UnsignedInteger j = 0; j < nodesNumber; ++j)
      for (UnsignedInteger i = j; i < nodesNumber; ++i)
      {
        const NumericalScalar factor = weights[i] * weights[j];
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
    omega = Matrix(nodesNumber * dimension, basisSize_ * dimension);
    for (UnsignedInteger j = 0; j < basisSize_; ++j)
      for (UnsignedInteger i = 0; i < nodesNumber; ++i)
      {
        const NumericalScalar value = scaledTheta(i, j);
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
    choleskyBlock = TriangularMatrix(basisSize_ * dimension);
    for (UnsignedInteger j = 0; j < basisSize_; ++j)
      for (UnsignedInteger i = j; i < basisSize_; ++i)
      {
        const NumericalScalar value = cholesky(i, j);
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
  NumericalPoint eigenValues(C.computeEV(eigenVectors, false));
  const UnsignedInteger augmentedDimension = eigenVectors.getDimension();
  // Transform the eigenvectors to the generalizd ones
  // Last time we need cholesky, so we can overwrite it by eigenVectors
  LOGINFO("Get the generalized eigenvectors");
  eigenVectors = choleskyBlock.transpose().solveLinearSystem(eigenVectors, false).getImplementation();
  LOGINFO("Sort the eigenvectors by decreasing eigenvalues");
  NumericalSample eigenPairs(augmentedDimension, augmentedDimension + 1);
  for (UnsignedInteger i = 0; i < augmentedDimension; ++i)
  {
    for (UnsignedInteger j = 0; j < augmentedDimension; ++j) eigenPairs[i][j] = eigenVectors(j, i);
    eigenPairs[i][augmentedDimension] = -eigenValues[i];
  }
  eigenPairs = eigenPairs.sortAccordingToAComponent(augmentedDimension);
  for (UnsignedInteger i = 0; i < augmentedDimension; ++i)
  {
    for (UnsignedInteger j = 0; j < augmentedDimension; ++j) eigenVectors(i, j) = eigenPairs[j][i];
    eigenValues[i] = -eigenPairs[i][augmentedDimension];
  }
  UnsignedInteger K = 0;
  NumericalScalar cumulatedVariance = std::abs(eigenValues[0]);
  // Find the cut-off in the eigenvalues
  while ((K < eigenValues.getSize()) && (eigenValues[K] >= threshold_ * cumulatedVariance))
    {
      cumulatedVariance += eigenValues[K];
      ++K;
    }
  // Reduce and rescale the eigenvectors
  MatrixImplementation transposedProjection(nodesNumber, K);
  NumericalPoint selectedEV(K);
  Basis modes(0);
  ProcessSample modesAsProcessSample(Mesh(nodes), 0, dimension);
  NumericalSampleImplementation values(nodesNumber, dimension);
  UnsignedInteger indexProjection = 0;
  for (UnsignedInteger k = 0; k < K; ++k)
  {
    selectedEV[k] = eigenValues[k];
    const MatrixImplementation a(*eigenVectors.getColumn(k).getImplementation());
    const NumericalScalar norm = (omega * NumericalPoint(a)).norm();
    // Store the eigen modes in two forms
    NumericalPoint modeValues(omega.getImplementation()->genProd(a));
    const NumericalScalar factor = modeValues[0] < 0.0 ? -1.0 / norm : 1.0 / norm;
    // Unscale the values
    for (UnsignedInteger i = 0; i < nodesNumber; ++i) modeValues[i] *= factor / weights[i];
    values.setData(modeValues);
    modesAsProcessSample.add(values);
    if (dimension == 1)
      modes.add(LinearCombinationFunction(coll, a * factor));
    else
    {
      NumericalSampleImplementation aSample(basisSize_, dimension);
      aSample.setData(a * factor);
      modes.add(DualLinearCombinationFunction(coll, aSample));
    }
    // Build the relevant column of the transposed projection matrix
    // For k=1,...,K K is the number of selected modes
    // M_{k,:}[f(\xi_j)]=1/\sqrt{\lambda_k}\int_{\Omega}f(x)\phi_k(x)dx
    //                =1/\sqrt{\lambda_k}\sum_{j=1}^N w_jf(\xi_j)\phi_k(\xi_j)
    //                =1/\sqrt{\lambda_k}\sum_{j=1}^N w_jf(\xi_j)\sum_{i=1}^P a_i^k\theta_i(\xi_j)
    // So M is KxNd, \omega is NdxPd, eigenVectors is PdxK
    // M^t=\omega * eigenVectors
    MatrixImplementation b(omega.getImplementation()->genProd(a * (factor / sqrt(selectedEV[k]))));
    UnsignedInteger indexB = 0;
    for (UnsignedInteger i = 0; i < nodesNumber; ++i)
    {
      const NumericalScalar wI = weights[i];
      for (UnsignedInteger j = 0; j < dimension; ++j)
      {
        b[indexB] *= wI;
        ++indexB;
      } // for j
    } // for i
    std::copy(b.begin(), b.end(), transposedProjection.begin() + indexProjection);
    indexProjection += b.getSize();
  }
  result_ = KarhunenLoeveResultImplementation(covariance_, threshold_, selectedEV, modes, modesAsProcessSample, transposedProjection.transpose());
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
Basis KarhunenLoeveQuadratureAlgorithm::getBasis() const
{
  return basis_;
}

/* BasisSize accessor */
UnsignedInteger KarhunenLoeveQuadratureAlgorithm::getBasisSize() const
{
  return basisSize_;
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
      << ", domain=" << domain_;
  return oss;
}

/* String converter */
String KarhunenLoeveQuadratureAlgorithm::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << KarhunenLoeveQuadratureAlgorithm::GetClassName()
      << ", domain=" << domain_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void KarhunenLoeveQuadratureAlgorithm::save(Advocate & adv) const
{
  KarhunenLoeveAlgorithmImplementation::save(adv);
  adv.saveAttribute( "domain_", domain_ );
  adv.saveAttribute( "experiment_", experiment_ );
  adv.saveAttribute( "basis_", basis_ );
  adv.saveAttribute( "basisSize_", basisSize_ );
  adv.saveAttribute( "mustScale_", mustScale_ );
}

/* Method load() reloads the object from the StorageManager */
void KarhunenLoeveQuadratureAlgorithm::load(Advocate & adv)
{
  KarhunenLoeveAlgorithmImplementation::load(adv);
  adv.loadAttribute( "domain_", domain_ );
  adv.loadAttribute( "experiment_", experiment_ );
  adv.loadAttribute( "basis_", basis_ );
  adv.loadAttribute( "basisSize_", basisSize_ );
  adv.loadAttribute( "mustScale_", mustScale_ );
}

END_NAMESPACE_OPENTURNS

