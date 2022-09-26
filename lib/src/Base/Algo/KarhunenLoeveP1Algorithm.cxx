//                                               -*- C++ -*-
/**
 *  @brief This class implements the computation of the Karhunen-Loeve
 *         basis and eigenvalues of a given covariance model based on
 *         P1 Lagrange approximation.
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/SpecFunc.hxx"
#include "openturns/SparseMatrix.hxx"
#include "openturns/HMatrix.hxx"
#include <algorithm>
#ifdef OPENTURNS_HAVE_SPECTRA
#include <Spectra/GenEigsSolver.h>
#endif



BEGIN_NAMESPACE_OPENTURNS

typedef Collection<Complex> ComplexCollection;


/** Defining parent class KLGenMatProd **/
class KLGenMatProd
{
public:
  using Scalar = double;

  KLGenMatProd()
  {
    // Nothing to do
  }

  virtual ~KLGenMatProd()
  {
    // Nothing to do
  }

  virtual int rows() const
  {
    throw NotYetImplementedException(HERE) << "method 'rows' not yet implemented";
  }

  virtual int cols() const
  {
    throw NotYetImplementedException(HERE) << "method 'cols' not yet implemented";
  }

  virtual void perform_op(const Scalar *, Scalar *) const
  {
    throw NotYetImplementedException(HERE) << "method 'perform_op' not yet implemented";
  }

  virtual CovarianceMatrix getC() const
  {
    throw NotYetImplementedException(HERE) << "method 'getC' not yet implemented";
  }
};


/** Defining class KLMatProdLapack **/
class KLMatProdLapack
  : public KLGenMatProd
{
public:

  KLMatProdLapack(const CovarianceMatrix & C,
                  const SparseMatrix & G)
    : C_(C)
    , G_(G)
    , rows_(C_.getNbRows())
    , cols_(C_.getNbColumns())
  {
    // Nothing to do
  }

  int rows() const
  {
    return rows_;
  }

  int cols() const
  {
    return cols_;
  }

  // Matrix/vector product operator
  void perform_op(const Scalar * x_in, Scalar * y_out) const
  {
    // Convert double array to Eigen::VectorXd
    Point u(rows_);
    std::copy(x_in, x_in + rows_, u.begin());

    // Compute product with sparse matrix G
    Point v(G_ * u);

    // Compute product with dense matrix C
    Point w(C_ * v);

    // Output double array
    std::copy(w.begin(), w.end(), y_out);
  }

  CovarianceMatrix getC() const
  {
    return C_;
  }

private:
  CovarianceMatrix C_;
  SparseMatrix G_;
  int rows_;
  int cols_;
};

/** Defining class KLMatProdHMat **/
class KLMatProdHMat
  : public KLGenMatProd
{
public:

  KLMatProdHMat(  const HMatrix & C,
                  const SparseMatrix & G)
    : C_(C)
    , G_(G)
    , rows_(C_.getNbRows())
    , cols_(C_.getNbColumns())
  {
    // Nothing to do
  }

  int rows() const
  {
    return rows_;
  }

  int cols() const
  {
    return cols_;
  }

  // Matrix/vector product operator
  void perform_op(const Scalar * x_in, Scalar * y_out) const
  {
    // Convert double array to Eigen::VectorXd
    Point u(rows_);
    std::copy(x_in, x_in + rows_, u.begin());

    // Compute product with sparse matrix G
    Point v(G_ * u);

    // Compute product with dense hmatrix C
    Point w(C_.getNbRows());
    C_.gemv('N', 1.0, v, 0.0, w);

    // Output double array
    std::copy(w.begin(), w.end(), y_out);
  }

private:
  HMatrix C_;
  SparseMatrix G_;
  int rows_;
  int cols_;
};


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
KarhunenLoeveP1Algorithm::KarhunenLoeveP1Algorithm( const Mesh & mesh,
    const CovarianceModel & covariance,
    const Scalar threshold
                                                  )
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


/* Compute P1 gram as a SparseMatrix */
static SparseMatrix ComputeSparseAugmentedP1Gram(const Mesh & mesh, const UnsignedInteger covarianceDimension)
{
  // If no simplex, the P1 gram matrix is null
  if (mesh.getSimplicesNumber() == 0)
    return SparseMatrix();

  const UnsignedInteger nbVertices  = mesh.getVerticesNumber();
  const UnsignedInteger nbSimplices = mesh.getSimplicesNumber();
  const UnsignedInteger verticesDim = mesh.getVertices().getDimension();
  const UnsignedInteger simplexSize = verticesDim + 1;
  const UnsignedInteger augmentedDimension = nbVertices * covarianceDimension;

  SquareMatrix elementaryGram(simplexSize, Point(simplexSize * simplexSize, 1.0 / (simplexSize * (simplexSize + 1.0))));
  for (UnsignedInteger i = 0; i < simplexSize; ++i) elementaryGram(i, i) *= 2.0;
  const Point simplexVolume(mesh.computeSimplicesVolume());
  Point values;
  Indices rowIndices;
  Indices columnIndices;
  for (UnsignedInteger i = 0; i < nbSimplices; ++i)
  {
    const Indices simplex(mesh.getSimplex(i));
    const Scalar delta = simplexVolume[i];

    if (delta != 0.0)
      for (UnsignedInteger j = 0; j < simplexSize; ++j)
        for (UnsignedInteger k = 0; k < simplexSize; ++k)
          for (UnsignedInteger l = 0; l < covarianceDimension; ++l)
          {
            rowIndices.add(simplex[j] * covarianceDimension + l);
            columnIndices.add(simplex[k] * covarianceDimension + l);
            values.add(delta * elementaryGram(j, k));
          }
  } // Loop over simplices
  SparseMatrix augmentedGram(augmentedDimension, augmentedDimension, rowIndices, columnIndices, values);
  return augmentedGram;
}


/* Call to spectra routines to compute EV */
static void ComputeEVWithSpectra(const UnsignedInteger augmentedDimension,
                                 const int nev,
                                 const int ncv,
                                 Pointer<KLGenMatProd> op,
                                 Point & eigenvalues,
                                 Matrix & eigenvectors)
{
#ifdef OPENTURNS_HAVE_SPECTRA
  Spectra::GenEigsSolver<KLGenMatProd> solver(*op, nev, ncv);
  solver.init();
  solver.compute(Spectra::SortRule::LargestMagn);

  if(solver.info() != Spectra::CompInfo::Successful)
    throw InternalException(HERE) << "Error while computing the eigenvalues (nev=" << nev << ", ncv=" << ncv << ", solver.info()=" << static_cast<int>(solver.info()) << ")";

  LOGINFO("Post-process the eigenvalue problem");

  Eigen::VectorXd eigenValuesEigen = solver.eigenvalues().real();
  Eigen::MatrixXd eigenVectorsEigen = solver.eigenvectors().real();

  // Post-process eigenvalues and eigenvectors
  std::copy(eigenValuesEigen.data(), eigenValuesEigen.data() + nev, eigenvalues.begin());
  std::copy(eigenVectorsEigen.data(), eigenVectorsEigen.data() + nev * augmentedDimension, &eigenvectors(0, 0));
#else
  (void)augmentedDimension;
  (void)nev;
  (void)ncv;
  (void)op;
  (void)eigenvalues;
  (void)eigenvectors;
  throw InternalException(HERE) << "SPECTRA is not available";
#endif
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
  const String eigenValuesSolver(ResourceMap::GetAsString("KarhunenLoeveP1Algorithm-EigenvaluesSolver"));
  const String covarianceMatrixStorage(ResourceMap::GetAsString("KarhunenLoeveP1Algorithm-CovarianceMatrixStorage"));

  // Retrieve relevant dimensions and constants
  const UnsignedInteger meshDimension       = mesh_.getDimension();
  const UnsignedInteger meshSize            = mesh_.getVerticesNumber();
  const UnsignedInteger dimension           = covariance_.getOutputDimension();
  const UnsignedInteger augmentedDimension  = dimension * meshSize;
  const Scalar epsilon = ResourceMap::GetAsScalar("KarhunenLoeveP1Algorithm-RegularizationFactor");

  // Define maximum number of modes and the number of modes to compute
  UnsignedInteger nbModesMax = (eigenValuesSolver == "SPECTRA") ? augmentedDimension - 2 : augmentedDimension;
  nbModesMax = std::min(getNbModes(), nbModesMax);
  const UnsignedInteger nev = (eigenValuesSolver == "SPECTRA") ? nbModesMax : augmentedDimension;

  // Declare some useful variables
  Matrix eigenVectors(augmentedDimension, nev);
  Point eigenValues(nev);
  Scalar computedVariance = 0.0;    // i.e. sum of /computed/ eigenvalues
  Scalar cumulatedVariance = 0.0;   // i.e. sum of /all/ eigenvalues

  // Compute the extended Gram matrix of the mesh
  LOGINFO("Build the Gram matrix");
  SparseMatrix G(ComputeSparseAugmentedP1Gram(mesh_, dimension));
  // Add epsilon to the diagonal of extended Gram matrix
  if (epsilon > 0.0)
    for (UnsignedInteger i = 0; i < augmentedDimension; ++i)
      G(i, i) += epsilon;

  if (eigenValuesSolver == "SPECTRA")
  {
    // Prepare matrix/vector product operator
    LOGINFO("Discretize the covariance model");
    Pointer<KLGenMatProd> op;

    if (covarianceMatrixStorage == "DENSE")
    {
      CovarianceMatrix C(covariance_.discretize(mesh_.getVertices()));
      op = new KLMatProdLapack(C, G);
    }
    else if (covarianceMatrixStorage == "HMAT")
    {
      HMatrix C(covariance_.discretizeHMatrix(mesh_, HMatrixParameters()));
      op = new KLMatProdHMat(C, G);
    }
    else
      throw InternalException(HERE) << "unknown covariance matrix storage format: " << covarianceMatrixStorage;

    // Define convergence index
    const UnsignedInteger ncv = std::min(2 * nev + 1, augmentedDimension);

    // Solve EV problem
    LOGINFO("Solve the eigenvalue problem");
    ComputeEVWithSpectra( augmentedDimension,
                          nev,
                          ncv,
                          op,
                          eigenValues,
                          eigenVectors);

    LOGDEBUG(OSS(false) << "eigenValues=" << eigenValues);

    // Compute computedVariance (i.e sum of computed eigenvalues)
    LOGINFO("Post-process the eigenvalue problem");
    for (UnsignedInteger k = 0; k < eigenValues.getSize(); ++k)
      computedVariance += eigenValues[k];

    // Here, cumulatedVariance is estimated from the computed EV, assuming
    // that all the following EV are equal to the last one computed
    cumulatedVariance = computedVariance + (augmentedDimension - eigenValues.getSize()) * eigenValues[eigenValues.getSize() - 1];
    LOGDEBUG(OSS(false) << "eigenValues=" << eigenValues);
  }
  else if (eigenValuesSolver == "LAPACK")
  {
    // LAPACK cannot solve EV problem based on HMatrix
    if (covarianceMatrixStorage == "HMAT")
      throw NotYetImplementedException(HERE) << "LAPACK cannot solve EV problem based on HMAT matrix storage";
    else if (covarianceMatrixStorage != "DENSE")
      throw InternalException(HERE) << "unknown covariance matrix storage format: " << covarianceMatrixStorage;

    // Declare complex eigenvectors variable
    SquareComplexMatrix eigenVectorsComplex;

    // Discretize covariance model
    LOGINFO("Discretize the covariance model");
    CovarianceMatrix C(covariance_.discretize(mesh_.getVertices()));

    // Prepare matrix M = C*G, compute EV
    SquareMatrix denseG(G.asDenseMatrix().getImplementation());
    SquareMatrix M(C * denseG);
    SquareMatrix::ComplexCollection eigenValuesComplex(M.computeEV(eigenVectorsComplex, false));

    // Format results, sort eigenvalues by decreasing order
    LOGINFO("Post-process the eigenvalue problem");

    Collection< std::pair<Scalar, UnsignedInteger> > eigenPairs(augmentedDimension);
    for (UnsignedInteger i = 0; i < augmentedDimension; ++i)
      eigenPairs[i] = std::pair<Scalar, UnsignedInteger>(-eigenValuesComplex[i].real(), i);

    std::sort(eigenPairs.begin(), eigenPairs.end());

    // Store eigenVectors in real matrix
    for (UnsignedInteger j = 0; j < augmentedDimension; ++j)
      for (UnsignedInteger i = 0; i < augmentedDimension; ++i)
        eigenVectors(i, j) = eigenVectorsComplex(i, eigenPairs[j].second).real();

    // Compute cumulatedVariance (i.e. sum of eigenvalues)
    for (UnsignedInteger i = 0; i < augmentedDimension; ++i)
    {
      // the eigenvalue can be close to zero but numerically negative
      eigenValues[i] = std::max(-eigenPairs[i].first, 0.0);
      cumulatedVariance += eigenValues[i];
    }
    LOGDEBUG(OSS(false) << "eigenValues=" << eigenValues);
  }
  else
  {
    throw InternalException(HERE) << "unknown eigen-values solver: " << eigenValuesSolver;
  }

  // Find the cut-off in the eigenvalues
  UnsignedInteger K = 0;
  Scalar selectedVariance = 0.0; // sum of eigenvalues selected after cut-off is applied
  do
  {
    selectedVariance += eigenValues[K];
    ++ K;
  }
  while ((K < nbModesMax) && (selectedVariance <= (1.0 - threshold_) * cumulatedVariance));
  LOGINFO(OSS() << "Selected " << K << " eigenvalues out of " << eigenValues.getSize() << " computed");

  // Reduce and rescale the eigenvectors
  MatrixImplementation projection(K, augmentedDimension);
  Point selectedEV(K);
  std::copy(eigenValues.begin(), eigenValues.begin() + K, selectedEV.begin());
  Collection<Function> modes(0);
  ProcessSample modesAsProcessSample(mesh_, 0, dimension);
  SampleImplementation values(meshSize, dimension);
  Pointer<PiecewiseLinearEvaluation> evaluation1D;
  Pointer<P1LagrangeEvaluation> evaluationXD;
  if (meshDimension == 1)
    evaluation1D = new PiecewiseLinearEvaluation(mesh_.getVertices().getImplementation()->getData(), values);
  else
    evaluationXD = new P1LagrangeEvaluation(Field(mesh_, dimension));

  Point a(augmentedDimension);
  for (UnsignedInteger k = 0; k < K; ++k)
  {
    std::copy(eigenVectors.getImplementation()->begin() + k * augmentedDimension,
              eigenVectors.getImplementation()->begin() + (k + 1)*augmentedDimension,
              a.begin());

    const Point Ga(G * a);                                                  // TODO: useful?
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
  result_ = KarhunenLoeveResultImplementation(covariance_, threshold_, selectedEV, modes, modesAsProcessSample, projection, selectedVariance/cumulatedVariance);
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
