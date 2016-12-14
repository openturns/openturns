//                                               -*- C++ -*-
/**
 *  @brief This class implements the computation of the Karhunen-Loeve
 *         basis and eigenvalues of a given covariance model based on
 *         quadrature approximation of the integral equation.
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
#include "openturns/KarhunenLoeveQuadratureFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/SquareComplexMatrix.hxx"
#include "openturns/NumericalSample.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/ComposedFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KarhunenLoeveQuadratureFactory
 */

CLASSNAMEINIT(KarhunenLoeveQuadratureFactory);

static const Factory<KarhunenLoeveQuadratureFactory> Factory_KarhunenLoeveQuadratureFactory;

/* Constructor without parameters */
KarhunenLoeveQuadratureFactory::KarhunenLoeveQuadratureFactory()
  : PersistentObject()
  , domain_()
  , coll_(0)
  , threshold_(0.0)
  , nodes_(0, 0)
  , weights_(0)
  , theta_(0, 0)
  , cholesky_(0)
{
  // Nothing to do
}

/* Constructor with parameters */
KarhunenLoeveQuadratureFactory::KarhunenLoeveQuadratureFactory(const Domain & domain,
    const WeightedExperiment & experiment,
    const Basis & basis,
    const UnsignedInteger basisSize,
    const Bool mustScale,
    const NumericalScalar threshold)
  : PersistentObject()
  , domain_(domain)
  , coll_(basisSize)
  , threshold_(threshold)
  , nodes_(0, 0)
  , weights_(0)
  , theta_(0, 0)
  , cholesky_(0)
{
  // Check the arguments
  const UnsignedInteger dimension = domain.getDimension();
  const Distribution distribution(experiment.getDistribution());
  if (dimension != distribution.getDimension()) throw InvalidArgumentException(HERE) << "Error: the domain dimension=" << dimension << " does not match the distribution dimension=" << distribution.getDimension() << " of the weighted experiment";
  // First thing to do: build a linear transformation that maps the range of the distribution associated with the weighted experiment to the bounding box of the domain
  const NumericalPoint domainLowerBound(domain.getLowerBound());
  const NumericalPoint domainUpperBound(domain.getUpperBound());
  if (Interval(domainLowerBound, domainUpperBound).isNumericallyEmpty()) throw InvalidArgumentException(HERE) << "Error: the given domain is numerically empty.";
  const NumericalPoint distributionLowerBound(distribution.getRange().getLowerBound());
  const NumericalPoint distributionUpperBound(distribution.getRange().getUpperBound());
  const Bool hasSameBounds = (domainLowerBound == distributionLowerBound) && (domainUpperBound == distributionUpperBound);
  // The function scaling maps points in the range of the distribution into the domain
  NumericalMathFunction scaling;
  NumericalMathFunction inverseScaling;
  // Normalization factor takes into account the fact that we map the range of the distribution defining the weighted experiment with the bounding box of the domain
  NumericalScalar normalizationFactor = 1.0;
  if (!hasSameBounds)
  {
    TriangularMatrix T(dimension);
    TriangularMatrix inverseT(dimension);
    NumericalPoint center((distributionUpperBound + distributionLowerBound) * 0.5);
    NumericalPoint constant((domainUpperBound + domainLowerBound) * 0.5);
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      T(i, i) = (domainUpperBound[i] - domainLowerBound[i]) / (distributionUpperBound[i] - distributionLowerBound[i]);
      normalizationFactor *= T(i, i);
      inverseT(i, i) = 1.0 / T(i, i);
    }
    scaling = LinearFunction(center, constant, T);
    inverseScaling = LinearFunction(constant, center, inverseT);
  }
  // Here we set the collection of functions
  for (UnsignedInteger i = 0; i < basisSize; ++i)
    if (!hasSameBounds && mustScale) coll_[i] = ComposedFunction(basis.build(i), inverseScaling);
    else coll_[i] = basis.build(i);
  // Compute the integration nodes and weights
  NumericalPoint rawWeights;
  WeightedExperiment experimentCopy(experiment);
  LOGINFO("Generate the weighted experiment");
  NumericalSample rawNodes(experimentCopy.generateWithWeights(rawWeights));
  LOGINFO(OSS(false) << "Initial number of integration nodes=" << rawNodes.getSize());
  LOGINFO("Generate the pdf");
  const NumericalSample pdf(distribution.computePDF(rawNodes));
  if (!hasSameBounds) rawNodes = scaling(rawNodes);
  // Update the weights in order to match Lebesgue distribution on the domain
  // We keep only the nodes inside of the domain
  nodes_ = NumericalSample(0, dimension);
  LOGINFO("Filter the integration nodes");
  for (UnsignedInteger i = 0; i < rawWeights.getDimension(); ++i)
  {
    const Bool isInside = domain.contains(rawNodes[i]);
    if (isInside)
    {
      nodes_.add(rawNodes[i]);
      weights_.add(std::sqrt(normalizationFactor * rawWeights[i] / pdf[i][0]));
    }
  }
  // Now we compute the basis functions over the nodes, properly scaled
  const UnsignedInteger nodesNumber = nodes_.getSize();
  LOGINFO(OSS(false) << "Final number of integration nodes=" << nodesNumber);
  if (nodesNumber == 0) throw InternalException(HERE) << "Error: cannot compute a Karhunen Loeve decomposition with zero integration node.";
  LOGINFO("Compute the design matrix");
  theta_ = Matrix(nodesNumber, basisSize);
  for (UnsignedInteger j = 0; j < basisSize; ++j)
  {
    const NumericalPoint thetaj(coll_[j](nodes_).getImplementation()->getData());
    for (UnsignedInteger i = 0; i < nodesNumber; ++i)
      theta_(i, j) = weights_[i] * thetaj[i];
  }
  // Compute the Cholesky factor of \theta^t\theta
  LOGINFO("Compute the Cholesky factor of the Gram matrix");
  CovarianceMatrix gram(theta_.computeGram(true));
  const NumericalScalar epsilon = ResourceMap::GetAsNumericalScalar("KarhunenLoeveQuadratureFactory-RegularizationFactor");
  if (epsilon > 0.0)
    for (UnsignedInteger i = 0; i < gram.getDimension(); ++i) gram(i, i) += epsilon;
  cholesky_ = gram.computeCholesky(false);
}

/* Virtual constructor */
KarhunenLoeveQuadratureFactory * KarhunenLoeveQuadratureFactory::clone() const
{
  return new KarhunenLoeveQuadratureFactory( *this );
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
Basis KarhunenLoeveQuadratureFactory::build(const CovarianceModel & covarianceModel,
    NumericalPoint & selectedEV) const
{
  const UnsignedInteger nodesNumber = nodes_.getSize();
  const UnsignedInteger dimension = covarianceModel.getDimension();
  // Discretize the covariance model
  LOGINFO("Discretize the covariance matrix");
  CovarianceMatrix C(covarianceModel.discretize(nodes_));
  // Prepare the generalized eigenvalues problem
  // Step 1: scale C by the weights
  LOGINFO("Scale the covariance matrix");
  if (dimension == 1)
  {
    for (UnsignedInteger j = 0; j < nodesNumber; ++j)
      for (UnsignedInteger i = j; i < nodesNumber; ++i)
      {
        const NumericalScalar factor = weights_[i] * weights_[j];
        C(i, j) *= factor;
      } // i
  } // dimension == 1
  else
  {
    for (UnsignedInteger j = 0; j < nodesNumber; ++j)
      for (UnsignedInteger i = j; i < nodesNumber; ++i)
      {
        const NumericalScalar factor = weights_[i] * weights_[j];
        for (UnsignedInteger m = 0; m < dimension; ++m)
          for (UnsignedInteger n = 0; n < dimension; ++n)
            C(m + i * dimension, n + j * dimension) *= factor;
      } // i
  } // dimension > 1
  // Step 2: take theta into account
  const UnsignedInteger basisSize = coll_.getSize();
  Matrix omega;
  LOGINFO("Scale the design matrix");
  // If dimension == 1 we can use theta_ directly
  if (dimension == 1) omega = theta_;
  // Here we have to expand theta if dimension > 1
  else
  {
    omega = Matrix(nodesNumber * dimension, basisSize * dimension);
    for (UnsignedInteger j = 0; j < basisSize; ++j)
      for (UnsignedInteger i = 0; i < nodesNumber; ++i)
      {
        const NumericalScalar value = theta_(i, j);
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
  // Cholesky decomposition of theta
  LOGINFO("Expand the Cholesky factor");
  TriangularMatrix cholesky;
  // If dimension == 1 we can use cholesky_ directly
  if (dimension == 1) cholesky = cholesky_;
  // Here we have to expand cholesky if dimension > 1
  else
  {
    cholesky = TriangularMatrix(basisSize * dimension);
    for (UnsignedInteger j = 0; j < basisSize; ++j)
      for (UnsignedInteger i = j; i < basisSize; ++i)
      {
        const NumericalScalar value = cholesky_(i, j);
        for (UnsignedInteger k = 0; k < dimension; ++k)
          cholesky(k + i * dimension, k + j * dimension) = value;
      }
  }
  // Step 3: reduction to a standard eigenvalues problem
  // A = L^{-1}CL^{-t}
  //   = L^{-1}((CL^{-t})^t)^t as (A^t)^t=A
  //   = L^{-1}(L^{-1}C^t)^t as (MN)^t = N^tM^t
  //   = L^{-1}(L^{-1}C)^t as C^t = C
  LOGINFO("Reduce the generalized eigenvalue problem to a standard eigenvalu problem");
  C = cholesky.solveLinearSystem((cholesky.solveLinearSystem(C)).transpose()).getImplementation();
  SquareMatrix eigenVectors;
  // Last time we need C, so we can overwrite it by eigenVectors
  LOGINFO("Solve the standard eigenvalue problem");
  NumericalPoint eigenValues(C.computeEV(eigenVectors, false));
  const UnsignedInteger eigenDimension = eigenVectors.getDimension();
  // Transform the eigenvectors to the generalizd ones
  // Last time we need cholesky, so we can overwrite it by eigenVectors
  LOGINFO("Get the generalized eigenvectors");
  eigenVectors = cholesky.transpose().solveLinearSystem(eigenVectors, false).getImplementation();
  LOGINFO("Sort the eigenvectors by decreasing eigenvalues");
  NumericalSample eigenPairs(eigenDimension, eigenDimension + 1);
  for (UnsignedInteger i = 0; i < eigenDimension; ++i)
  {
    for (UnsignedInteger j = 0; j < eigenDimension; ++j) eigenPairs[i][j] = eigenVectors(j, i);
    eigenPairs[i][eigenDimension] = -eigenValues[i];
  }
  eigenPairs = eigenPairs.sortAccordingToAComponent(eigenDimension);
  for (UnsignedInteger i = 0; i < eigenDimension; ++i)
  {
    for (UnsignedInteger j = 0; j < eigenDimension; ++j) eigenVectors(i, j) = eigenPairs[j][i];
    eigenValues[i] = -eigenPairs[i][eigenDimension];
  }
  selectedEV = NumericalPoint(0);
  UnsignedInteger j = 0;
  NumericalMathFunctionCollection resultBasis(0);
  LOGINFO("Keep only the relevant eigen pairs");
  while ((j < eigenDimension) && (eigenValues[j] > threshold_ * std::abs(eigenValues[0])))
  {
    selectedEV.add(eigenValues[j]);
    const NumericalPoint a(*eigenVectors.getColumn(j).getImplementation());
    const NumericalScalar norm = (omega * a).norm();
    if (dimension == 1)
      resultBasis.add(NumericalMathFunction(coll_, a / norm));
    else
    {
      NumericalSampleImplementation aSample(basisSize, dimension);
      aSample.setData(a / norm);
      resultBasis.add(NumericalMathFunction(coll_, aSample));
    }
    ++j;
  }
  return resultBasis;
}

/* String converter */
String KarhunenLoeveQuadratureFactory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << KarhunenLoeveQuadratureFactory::GetClassName()
      << ", domain=" << domain_
      << ", coll=" << coll_
      << ", threshold=" << threshold_
      << ", nodes=" << nodes_
      << ", weights=" << weights_
      << ", theta=" << theta_
      << ", cholesky=" << cholesky_;
  return oss;
}

/* String converter */
String KarhunenLoeveQuadratureFactory::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << KarhunenLoeveQuadratureFactory::GetClassName()
      << ", domain=" << domain_
      << ", coll=" << coll_
      << ", threshold=" << threshold_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void KarhunenLoeveQuadratureFactory::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "domain_", domain_ );
  adv.saveAttribute( "coll_", coll_ );
  adv.saveAttribute( "threshold_", threshold_ );
  adv.saveAttribute( "nodes_", nodes_ );
  adv.saveAttribute( "weights_", weights_ );
  adv.saveAttribute( "theta_", theta_ );
  adv.saveAttribute( "cholesky_", cholesky_ );
}

/* Method load() reloads the object from the StorageManager */
void KarhunenLoeveQuadratureFactory::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "domain_", domain_ );
  adv.loadAttribute( "coll_", coll_ );
  adv.loadAttribute( "threshold_", threshold_ );
  adv.loadAttribute( "nodes_", nodes_ );
  adv.loadAttribute( "weights_", weights_ );
  adv.loadAttribute( "theta_", theta_ );
  adv.loadAttribute( "cholesky_", cholesky_ );
}

END_NAMESPACE_OPENTURNS
