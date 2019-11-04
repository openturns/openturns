//                                               -*- C++ -*-
/**
 *  @brief This class implements the computation of the Karhunen-Loeve
 *         basis and eigenvalues of a given covariance model based on
 *         P1 Lagrange approximation.
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/KarhunenLoeveP1Algorithm.hxx"
#include "openturns/SquareComplexMatrix.hxx"
#include "openturns/P1LagrangeEvaluation.hxx"
#include "openturns/PiecewiseLinearEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KarhunenLoeveP1Algorithm
 */

CLASSNAMEINIT(KarhunenLoeveP1Algorithm)

static const Factory<KarhunenLoeveP1Algorithm> Algorithm_KarhunenLoeveP1Algorithm;

/* Constructor without parameters */
KarhunenLoeveP1Algorithm::KarhunenLoeveP1Algorithm()
  : KarhunenLoeveAlgorithmImplementation()
  , mesh_()
{
  // Nothing to do
}

/* Constructor with parameters */
KarhunenLoeveP1Algorithm::KarhunenLoeveP1Algorithm(const Mesh & mesh,
    const CovarianceModel & covariance,
    const Scalar threshold)
  : KarhunenLoeveAlgorithmImplementation(covariance, threshold)
  , mesh_(mesh)
{
  // Nothing to do
}

/* Virtual constructor */
KarhunenLoeveP1Algorithm * KarhunenLoeveP1Algorithm::clone() const
{
  return new KarhunenLoeveP1Algorithm( *this );
}

/* Here we discretize the following Fredholm problem:
   \int_{\Omega}C(s,t)\phi_n(s)ds=\lambda_n\phi_n(t)
   using a P1 approximation of C and \phi_n:
   C(s,t)=\sum_{i,j}C(s_i,s_j)\theta_i(s)\theta_j(t)
   \phi_n(t)=\sum_k\alpha_k^n\theta_k(t)
   where s_i,s_j are vertices of the mesh, C(s_i,s_j)\in\cS^+_d(\R), \alpha_k^n\in\R^d
   leading to:
   \forall t\in\Omega, \sum_{i,j}C(s_i,s_j)\theta_j(t)\int_{\Omega}\theta_i(s)\sum_k\alpha_k^n\theta_k(s)=\lambda_n\sum_l\alpha_k^l\theta_l(t)
   For each value of n we get d equations in \alpha^n. We write these equations for t=s_1,...,s_N the N vertices of the mesh:
   \sum_{i,j}C(s_i,s_j)\theta_j(s_m)\int_{\Omega}\theta_i(s)\sum_k\alpha_k^n\theta_k(s)=\lambda_n\sum_l\alpha_l^n\theta_l(s_m)
   ie:
   \sum_i C(s_i,s_m)\int_{\Omega}\theta_i(s)\sum_k\alpha_k^n\theta_k(s)=\lambda_n\alpha_m^n\theta_m(s_m)
   In a block-matrix form we get:
   [C(s_1,s_1) ... C(s_1,s_N)][K_11 ... K_1N][\alpha_1]             [\alpha_1]
   [    ...            ...   ][ ...      ...][   ...  ] = \lambda_n [   ...  ]
   [C(s_N,s_1) ... C(s_N,s_N)][K_N1 ... K_NN][\alpha_N]             [\alpha_N]
   Where:
   K_ij = \int_{\Omega}\theta_i(s)\theta_j(s)ds I
   with I the dxd identity matrix
*/
void KarhunenLoeveP1Algorithm::run()
{
  // Compute the gram of the mesh
  LOGINFO("Build the Gram matrix");
  CovarianceMatrix gram(mesh_.computeP1Gram());
  const UnsignedInteger numVertices = mesh_.getVerticesNumber();
  if (!(gram.getDimension() == numVertices)) throw InternalException(HERE) << "Error: the P1 Gram matrix of the mesh has a dimension=" << gram.getDimension() << " different from the number of vertices=" << numVertices;
  const Scalar epsilon = ResourceMap::GetAsScalar("KarhunenLoeveP1Algorithm-RegularizationFactor");
  if (epsilon > 0.0)
    for (UnsignedInteger i = 0; i < gram.getDimension(); ++i) gram(i, i) += epsilon;
  // Extend the Gram matrix of the mesh
  const UnsignedInteger dimension = covariance_.getOutputDimension();
  const UnsignedInteger augmentedDimension = dimension * numVertices;
  CovarianceMatrix G;
  if (dimension == 1) G = gram;
  else
  {
    G = CovarianceMatrix(augmentedDimension);
    for (UnsignedInteger i = 0; i < numVertices; ++i)
    {
      for (UnsignedInteger j = 0; j <= i; ++j)
      {
        const Scalar gij = gram(i, j);
        for (UnsignedInteger k = 0; k < dimension; ++k)
          G(i * dimension + k, j * dimension + k) = gij;
      } // Loop over j
    } // Loop over i
  }
  // Discretize the covariance model
  LOGINFO("Discretize the covariance model");
  CovarianceMatrix C(covariance_.discretize(mesh_.getVertices()));
  LOGINFO("Discretize the Fredholm equation");
  SquareMatrix M((C * G).getImplementation());
  LOGINFO("Solve the eigenvalue problem");
  SquareComplexMatrix eigenVectorsComplex;
  SquareMatrix::ComplexCollection eigenValuesComplex(M.computeEV(eigenVectorsComplex, false));
  LOGINFO("Post-process the eigenvalue problem");
  Collection< std::pair<Scalar, UnsignedInteger> > eigenPairs(augmentedDimension);
  for (UnsignedInteger i = 0; i < augmentedDimension; ++i)
    eigenPairs[i] = std::pair<Scalar, UnsignedInteger>(-eigenValuesComplex[i].real(), i);
  // Sort the eigenvalues in decreasing order
  std::sort(eigenPairs.begin(), eigenPairs.end());
  Point eigenValues(augmentedDimension);
  Scalar cumulatedVariance = 0.0;
  for (UnsignedInteger i = 0; i < augmentedDimension; ++i)
  {
    eigenValues[i] = -eigenPairs[i].first;
    cumulatedVariance += eigenValues[i];
  }
  LOGDEBUG(OSS(false) << "eigenValues=" << eigenValues);
  LOGINFO("Extract the relevant eigenpairs");
  // Start at 0 if the given threshold is large (eg greater than 1)
  UnsignedInteger K = 0;
  // Find the cut-off in the eigenvalues
  while ((K < eigenValues.getSize()) && (eigenValues[K] >= threshold_ * cumulatedVariance)) ++K;
  LOGINFO(OSS() << "Selected " << K << " eigenvalues");
  // Reduce and rescale the eigenvectors
  MatrixImplementation projection(K, augmentedDimension);
  Point selectedEV(K);
  Collection<Function> modes(0);
  ProcessSample modesAsProcessSample(mesh_, 0, dimension);
  const UnsignedInteger meshDimension = mesh_.getDimension();
  SampleImplementation values(numVertices, dimension);
  Pointer<PiecewiseLinearEvaluation> evaluation1D;
  Pointer<P1LagrangeEvaluation> evaluationXD;
  if (meshDimension == 1)
    evaluation1D = new PiecewiseLinearEvaluation(mesh_.getVertices().getImplementation()->getData(), values);
  else
    evaluationXD = new P1LagrangeEvaluation(Field(mesh_, dimension));
  Point a(augmentedDimension);
  for (UnsignedInteger k = 0; k < K; ++k)
  {
    selectedEV[k] = eigenValues[k];
    const UnsignedInteger initialColumn = eigenPairs[k].second;
    for (UnsignedInteger i = 0; i < augmentedDimension; ++i)
      a[i] = eigenVectorsComplex(i, initialColumn).real();
    const Point Ga(G * a);
    const Scalar norm = std::sqrt(a.dot(Ga));
    const Scalar factor = a[0] < 0.0 ? -1.0 / norm : 1.0 / norm;
    // Store the eigen modes in two forms
    values.setData(a * factor);
    modesAsProcessSample.add(values);
    if (meshDimension == 1)
    {
      evaluation1D->setValues(values);
      modes.add(*evaluation1D);
    }
    else
    {
      evaluationXD->setValues(values);
      modes.add(*evaluationXD);
    }

    // Build the relevant row of the projection matrix
    const Point b(Ga * (factor / sqrt(selectedEV[k])));
    for(UnsignedInteger i = 0; i < augmentedDimension; ++i)
      projection(k, i) = b[i];
  }
  result_ = KarhunenLoeveResultImplementation(covariance_, threshold_, selectedEV, modes, modesAsProcessSample, projection);
}

/* Mesh accessor */
Mesh KarhunenLoeveP1Algorithm::getMesh() const
{
  return mesh_;
}

/* String converter */
String KarhunenLoeveP1Algorithm::__repr__() const
{
  OSS oss(true);
  oss << "class=" << KarhunenLoeveP1Algorithm::GetClassName()
      << ", mesh=" << mesh_;
  return oss;
}

/* String converter */
String KarhunenLoeveP1Algorithm::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << KarhunenLoeveP1Algorithm::GetClassName()
      << ", mesh=" << mesh_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void KarhunenLoeveP1Algorithm::save(Advocate & adv) const
{
  KarhunenLoeveAlgorithmImplementation::save(adv);
  adv.saveAttribute( "mesh_", mesh_ );
}

/* Method load() reloads the object from the StorageManager */
void KarhunenLoeveP1Algorithm::load(Advocate & adv)
{
  KarhunenLoeveAlgorithmImplementation::load(adv);
  adv.loadAttribute( "mesh_", mesh_ );
}

END_NAMESPACE_OPENTURNS
