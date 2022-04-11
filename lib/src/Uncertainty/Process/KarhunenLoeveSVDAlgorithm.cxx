//                                               -*- C++ -*-
/**
 *  @brief This class implements the computation of the Karhunen-Loeve
 *         basis and eigenvalues of a given covariance model based on
 *         SVD decomposition of a process sample.
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
#include "openturns/KarhunenLoeveSVDAlgorithm.hxx"
#include "openturns/P1LagrangeEvaluation.hxx"
#include "openturns/PiecewiseLinearEvaluation.hxx"
#include "openturns/RankMCovarianceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DistFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KarhunenLoeveSVDAlgorithm
 */

CLASSNAMEINIT(KarhunenLoeveSVDAlgorithm)

static const Factory<KarhunenLoeveSVDAlgorithm> Algorithm_KarhunenLoeveSVDAlgorithm;

/* Constructor without parameters */
KarhunenLoeveSVDAlgorithm::KarhunenLoeveSVDAlgorithm()
  : KarhunenLoeveAlgorithmImplementation()
  , sample_()
  , centeredSample_(false)
  , verticesWeights_(0)
  , uniformVerticesWeights_(true)
  , sampleWeights_(0)
  , uniformSampleWeights_(true)
{
  // Nothing to do
}

/* Constructor with parameters */
KarhunenLoeveSVDAlgorithm::KarhunenLoeveSVDAlgorithm(const ProcessSample & sample,
    const Scalar threshold,
    const Bool centeredSample)
  : KarhunenLoeveAlgorithmImplementation(CovarianceModel(), threshold)
  , sample_(sample)
  , centeredSample_(centeredSample)
  , verticesWeights_(0)
  , uniformVerticesWeights_(false)
  , sampleWeights_(sample.getSize(), 1.0 / sample.getSize())
  , uniformSampleWeights_(true)
{
  // Set the vertices weights in order to check their uniformity and positivity
  setVerticesWeights(sample.getMesh().computeWeights());
}

/* Constructor with parameters */
KarhunenLoeveSVDAlgorithm::KarhunenLoeveSVDAlgorithm(const ProcessSample & sample,
    const Point & verticesWeights,
    const Scalar threshold,
    const Bool centeredSample)
  : KarhunenLoeveAlgorithmImplementation(CovarianceModel(), threshold)
  , sample_(sample)
  , centeredSample_(centeredSample)
  , verticesWeights_(0)
  , uniformVerticesWeights_(true)
  , sampleWeights_(sample.getSize(), 1.0 / sample.getSize())
  , uniformSampleWeights_(true)
{
  // Set the vertices weights in order to check their uniformity and positivity
  setVerticesWeights(verticesWeights);
}

/* Constructor with parameters */
KarhunenLoeveSVDAlgorithm::KarhunenLoeveSVDAlgorithm(const ProcessSample & sample,
    const Point & verticesWeights,
    const Point & sampleWeights,
    const Scalar threshold,
    const Bool centeredSample)
  : KarhunenLoeveAlgorithmImplementation(CovarianceModel(), threshold)
  , sample_(sample)
  , centeredSample_(centeredSample)
  , verticesWeights_(0)
  , uniformVerticesWeights_(true)
  , sampleWeights_(0)
  , uniformSampleWeights_(true)
{
  // Set the vertices weights in order to check their uniformity and positivity
  setVerticesWeights(verticesWeights);
  // Set the sample weights in order to check their uniformity and positivity
  setSampleWeights(sampleWeights);
}

/* Virtual constructor */
KarhunenLoeveSVDAlgorithm * KarhunenLoeveSVDAlgorithm::clone() const
{
  return new KarhunenLoeveSVDAlgorithm( *this );
}

/* Here we discretize the following Fredholm problem:
   \int_{\Omega}C(s,t)\phi_n(s)ds=\lambda_n\phi_n(t)
   using the empirical estimate for C and the SVD decomposition of the design matrix
*/
void KarhunenLoeveSVDAlgorithm::run()
{
  // Compute the design matrix of the sample
  LOGINFO("Build the Design matrix");
  const UnsignedInteger size = sample_.getSize();
  const UnsignedInteger kTilde = centeredSample_ ? size : size - 1;
  const UnsignedInteger verticesNumber = sample_.getMesh().getVerticesNumber();
  const UnsignedInteger dimension = sample_.getDimension();
  const UnsignedInteger augmentedDimension = verticesNumber * dimension;
  MatrixImplementation designMatrix(augmentedDimension, kTilde);
  // Compute the empirical mean if the sample is not centered
  Point mean;
  if (!centeredSample_)
  {
    LOGINFO("Noncentered sample: compute mean");
    const Scalar unbiasedRatio = size / (size - 1.0);
    mean = Point(augmentedDimension);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      mean += sampleWeights_[i] * sample_[i].getImplementation()->getData();
      sampleWeights_[i] *= unbiasedRatio;
    }
  }
  if (uniformVerticesWeights_)
  {
    LOGINFO("Uniform vertices weights");
    UnsignedInteger shift = 0;
    const Scalar coeff = std::sqrt(verticesWeights_[0]);
    for (UnsignedInteger i = 0; i < kTilde; ++i)
    {
      Point data = sample_[i].getImplementation()->getData();
      if (!centeredSample_) data -= mean;
      data *= coeff * std::sqrt(sampleWeights_[i]);
      std::copy(data.begin(), data.end(), designMatrix.begin() + shift);
      shift += augmentedDimension;
    }
  } // uniformVerticesWeights
  else
    // Take the vertices weights into account
  {
    LOGINFO("Non-uniform vertices weights");
    Point coeffs(verticesNumber);
    for (UnsignedInteger j = 0; j < verticesNumber; ++j)
      coeffs[j] = std::sqrt(verticesWeights_[j]);
    UnsignedInteger shift = 0;
    for (UnsignedInteger i = 0; i < kTilde; ++i)
    {
      const Scalar wI = std::sqrt(sampleWeights_[i]);
      Point data = sample_[i].getImplementation()->getData();
      if (!centeredSample_) data -= mean;
      UnsignedInteger baseIndex = 0;
      for (UnsignedInteger j = 0; j < verticesNumber; ++j)
      {
        const Scalar wJ = coeffs[j];
        for (UnsignedInteger k = 0; k < dimension; ++k)
        {
          designMatrix[shift] = wI * wJ * data[baseIndex];
          ++shift;
          ++baseIndex;
        } // k
      } // j
    } // i
  } // !uniformWeights
  LOGDEBUG(OSS(false) << "Design matrix=" << designMatrix);
  // Compute the SVD decomposition of the design matrix
  MatrixImplementation U;
  MatrixImplementation Vt;
  Point svd;
  // Do we use an approximate random algorithm?
  if (ResourceMap::GetAsBool("KarhunenLoeveSVDAlgorithm-UseRandomSVD"))
  {
    LOGINFO("Use random SVD");
    // Maximum number of singular values to compute
    MatrixImplementation & A = designMatrix;
    const UnsignedInteger m = A.getNbRows();
    const UnsignedInteger n = A.getNbColumns();
    const UnsignedInteger rank = std::min(std::min(ResourceMap::GetAsUnsignedInteger("KarhunenLoeveSVDAlgorithm-RandomSVDMaximumRank"), A.getNbColumns()), A.getNbRows());
    LOGINFO(OSS() << "Maximum rank=" << rank);
    LOGINFO(OSS() << "A=" << A.getNbRows() << "x" << A.getNbColumns());
    if (ResourceMap::GetAsString("KarhunenLoeveSVDAlgorithm-RandomSVDVariant") == "Halko2010")
    {
      // Here we use the algorithm described in:
      // Nathan Halko, Per-Gunnar Martinsson, Joel A. Tropp, "Finding structure with randomness: Probabilistic algorithms for constructing approximate matrix decompositions", https://arxiv.org/pdf/0909.4061.pdf
      LOGINFO("Use Halko2010 variant");
      // Create a Gaussian mixing matrix to apply to A = designMatrix
      LOGINFO("Create a Gaussian mixing matrix to apply to A = designMatrix");
      const MatrixImplementation Omega(m, rank, DistFunc::rNormal(m * rank));
      LOGINFO(OSS() << "Omega=" << Omega.getNbRows() << "x" << Omega.getNbColumns());
      // Y = A'.Omega
      LOGINFO("Create Y = A'.Omega");
      MatrixImplementation Y(A.genProd(Omega, true, false));
      LOGINFO(OSS() << "Y=" << Y.getNbRows() << "x" << Y.getNbColumns());
      // Orthonormalize Y columns
      LOGINFO("Orthonormalize Y columns");
      MatrixImplementation R;
      Y = Y.computeQR(R, false, false);
      LOGINFO(OSS() << "R=" << R.getNbRows() << "x" << R.getNbColumns());
      LOGINFO(OSS() << "Y=" << Y.getNbRows() << "x" << Y.getNbColumns());
      // B = A.Y
      LOGINFO("Create B = A.Y");
      MatrixImplementation B(A.genProd(Y, false, false));
      LOGINFO(OSS() << "B=" << B.getNbRows() << "x" << B.getNbColumns());
      // Create a Gaussian mixing matrix to apply to B
      LOGINFO("Create a Gaussian mixing matrix to apply to B");
      MatrixImplementation P(B.getNbColumns(), rank, DistFunc::rNormal(B.getNbColumns() * rank));
      LOGINFO(OSS() << "P=" << P.getNbRows() << "x" << P.getNbColumns());
      // Z = B.P
      LOGINFO("Create Z = B.P");
      MatrixImplementation Z(B.genProd(P, false, false));
      // Orthonormalize Z columns
      LOGINFO("Orthonormalize Z columns");
      Z = Z.computeQR(R, false, false);
      LOGINFO(OSS() << "R=" << R.getNbRows() << "x" << R.getNbColumns());
      LOGINFO(OSS() << "Z=" << Z.getNbRows() << "x" << Z.getNbColumns());
      // C = Z'.B
      LOGINFO("Create C = Z'.B");
      MatrixImplementation C(Z.genProd(B, true, false));
      LOGINFO(OSS() << "C=" << C.getNbRows() << "x" << C.getNbColumns());
      // Compute the SVD of C
      LOGINFO("Compute the SVD of C");
      MatrixImplementation Uc;
      MatrixImplementation VTc;
      svd = C.computeSVD(Uc, VTc, false, false);
      LOGINFO(OSS() << "Uc=" << Uc.getNbRows() << "x" << Uc.getNbColumns());
      LOGINFO(OSS() << "VTc=" << VTc.getNbRows() << "x" << VTc.getNbColumns());
      // Restore A singular vectors
      LOGINFO("Restore A singular vectors");
      U = Z.genProd(Uc, false, false);
      LOGINFO(OSS() << "U=" << U.getNbRows() << "x" << U.getNbColumns());
      // Vt is not needed for the algorithm
      // Vt = Y * Vt;
    }
    else
    {
      // Here we use the algorithm described in:
      // Nathan Halko, Per-Gunnar Martisson, Yoel Shkolnisky and Mark Tygert, "An algorithm for the principal component analysis of large data sets", arXiv:1007.5510v2
      LOGINFO("Use Halko2011 variant");
      const UnsignedInteger l = rank + ResourceMap::GetAsUnsignedInteger("KarhunenLoeveSVDAlgorithm-Halko2011Margin");
      const UnsignedInteger iMax = ResourceMap::GetAsUnsignedInteger("KarhunenLoeveSVDAlgorithm-Halko2011Iterations");
      // Create a Gaussian mixing matrix to apply to A = designMatrix
      LOGINFO("Create a Gaussian mixing matrix to apply to A = designMatrix");
      const MatrixImplementation G(n, l, DistFunc::rNormal(n * l));
      LOGINFO(OSS() << "G=" << G.getNbRows() << "x" << G.getNbColumns());
      LOGINFO("Create AGi = A.G");
      MatrixImplementation AGi(A.genProd(G, false, false));
      LOGINFO(OSS() << "AGi=" << AGi.getNbRows() << "x" << AGi.getNbColumns());
      LOGINFO("Create H");
      MatrixImplementation H(m, (iMax + 1) * l);
      LOGINFO(OSS() << "H=" << H.getNbRows() << "x" << H.getNbColumns());
      std::copy(AGi.begin(), AGi.end(), H.begin());
      for (UnsignedInteger i = 1; i <= iMax; ++i)
      {
        AGi = A.genProd(A.genProd(AGi, true, false), false, false);
        std::copy(AGi.begin(), AGi.end(), H.begin() + i * l);
        LOGINFO(OSS() << "H=" << H.getNbRows() << "x" << H.getNbColumns());
      }
      LOGINFO("Create QR decomposition of H");
      MatrixImplementation R;
      MatrixImplementation Q(H.computeQR(R, false));
      LOGINFO(OSS() << "R=" << R.getNbRows() << "x" << R.getNbColumns());
      LOGINFO(OSS() << "Q=" << Q.getNbRows() << "x" << Q.getNbColumns());
      LOGINFO("Create T = A.Q");
      MatrixImplementation T(A.genProd(Q, true, false));
      LOGINFO(OSS() << "T=" << T.getNbRows() << "x" << T.getNbColumns());
      LOGINFO("Create SVD of T");
      MatrixImplementation Uc;
      MatrixImplementation VTc;
      svd = T.computeSVD(Uc, VTc, false, false);
      LOGINFO(OSS() << "Uc=" << Uc.getNbRows() << "x" << Uc.getNbColumns());
      LOGINFO(OSS() << "VTc=" << VTc.getNbRows() << "x" << VTc.getNbColumns());
      LOGINFO("Create U");
      MatrixImplementation W(VTc.getNbColumns(), rank);
      LOGINFO(OSS() << "W=" << W.getNbRows() << "x" << W.getNbColumns());
      for (UnsignedInteger i = 0; i < VTc.getNbColumns(); ++i)
        for (UnsignedInteger j = 0; j < rank; ++j)
          W(i, j) = VTc(j, i);
      U = Q.genProd(W, false, false);
      LOGINFO(OSS() << "U=" << U.getNbRows() << "x" << U.getNbColumns());
    }
  }
  else
  {
    LOGINFO("Use LAPACK SVD");
    // The singular values are given in decreasing order
    // Last two arguments are:
    //   * fullSVD = false, we only want kTilde columns of U
    //   * keepIntact = false, designMatrix is no more used afterwards
    svd = designMatrix.computeSVD(U, Vt,  false, false);
  }
  LOGDEBUG(OSS(false) << "U=\n" << U << ", singular values=" << svd);
  Scalar cumulatedVariance = 0.0;
  Point eigenValues(svd.getDimension());
  for (UnsignedInteger i = 0; i < svd.getDimension(); ++i)
  {
    eigenValues[i] = svd[i] * svd[i];
    cumulatedVariance += eigenValues[i];
  }

  // Find the cut-off in the eigenvalues
  UnsignedInteger K = 0;
  Scalar selectedVariance = 0.0; // sum of eigenvalues selected after cut-off is applied
  const UnsignedInteger nbModesMax = std::min(eigenValues.getSize(), getNbModes());
  do
  {
    selectedVariance += eigenValues[K];
    ++ K;
  }
  while ((K < nbModesMax) && (selectedVariance < (1.0 - threshold_) * cumulatedVariance));
  LOGINFO(OSS() << "Selected " << K << " eigenvalues out of " << eigenValues.getSize() << " computed");

  // Stores the eigenmodes values in-place to avoid wasting memory
  MatrixImplementation & eigenModesValues = U;
  if (uniformVerticesWeights_) eigenModesValues *= 1.0 / std::sqrt(verticesWeights_[0]);
  else
  {
    UnsignedInteger index = 0;
    for (UnsignedInteger j = 0; j < K; ++j)
    {
      for (UnsignedInteger i = 0; i < verticesNumber; ++i)
      {
        const Scalar coefficient = 1.0 / std::sqrt(verticesWeights_[i]);
        for (UnsignedInteger k = 0; k < dimension; ++k)
        {
          eigenModesValues[index] *= coefficient;
          ++index;
        } // k
      } // i
    } // j
  } // !uniformVerticesWeights_
  // Reduce and rescale the eigenvectors
  MatrixImplementation projection(K, augmentedDimension);
  Point selectedEV(K);
  Collection<Function> modes(0);
  ProcessSample modesAsProcessSample(sample_.getMesh(), 0, dimension);
  const UnsignedInteger meshDimension = sample_.getMesh().getDimension();
  SampleImplementation values(verticesNumber, dimension);
  Pointer<PiecewiseLinearEvaluation> evaluation1D;
  Pointer<P1LagrangeEvaluation> evaluationXD;
  if (meshDimension == 1)
    evaluation1D = new PiecewiseLinearEvaluation(sample_.getMesh().getVertices().getImplementation()->getData(), values);
  else
    evaluationXD = new P1LagrangeEvaluation(Field(sample_.getMesh(), dimension));
  LOGINFO("Create modes and projection");
  for (UnsignedInteger k = 0; k < K; ++k)
  {
    selectedEV[k] = eigenValues[k];
    MatrixImplementation a(eigenModesValues.getColumn(k));
    if (a[0] < 0.0) a *= -1.0;
    // Store the eigen modes in two forms
    values.setData(a);
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
    // Build the relevant column of the transposed projection matrix
    // \vect{\alpha}=\diag{1/\sqrt{\lambda}}[(\sqrt{W}^{-1}U)^tW]F
    //              =\diag{1/\sqrt{\lambda}}[(W.eigenModesValues)^t]F
    // so M^t=[W.eigenModesValues.\diag{1/\sqrt{\lambda}}]^t
    if (uniformVerticesWeights_)
    {
      a *= verticesWeights_[0] / sqrt(selectedEV[k]);
      // Build the relevant row of the projection matrix
      for(UnsignedInteger i = 0; i < augmentedDimension; ++i)
        projection(k, i) = a[i];
    } // uniformVerticesWeights_
    else
    {
      const Scalar inverseSqrtLambda = 1.0 / sqrt(selectedEV[k]);
      for (UnsignedInteger i = 0; i < verticesNumber; ++i)
      {
        const Scalar coefficient = verticesWeights_[i] * inverseSqrtLambda;
        for (UnsignedInteger j = 0; j < dimension; ++j)
        {
          projection(k, i * dimension + j) = coefficient * a[i * dimension + j];
        } // j
      } // i
    } // !uniformVerticesWeights_
  } // k
  LOGINFO("Create KL result");
  covariance_ = RankMCovarianceModel(selectedEV, modes);
  result_ = KarhunenLoeveResultImplementation(covariance_, threshold_, selectedEV, modes, modesAsProcessSample, projection);
}

/* Sample accessor */
ProcessSample KarhunenLoeveSVDAlgorithm::getSample() const
{
  return sample_;
}

/* Vertices weights accessor */
Point KarhunenLoeveSVDAlgorithm::getVerticesWeights() const
{
  return verticesWeights_;
}

void KarhunenLoeveSVDAlgorithm::setVerticesWeights(const Point & verticesWeights)
{
  const UnsignedInteger verticesNumber = sample_.getMesh().getVerticesNumber();
  if (!(verticesWeights.getSize() == verticesNumber)) throw InvalidArgumentException(HERE) << "Error: expected vertices weights of dimension=" << verticesNumber << ", got dimension=" << verticesWeights.getSize();
  const Scalar weight0 = verticesWeights[0];
  uniformVerticesWeights_ = true;
  for (UnsignedInteger i = 0; i < verticesNumber; ++i)
  {
    if (!(verticesWeights[i] > 0.0)) throw InvalidArgumentException(HERE) << "Error: expected positive vertices weights, here weights[" << i << "]=" << verticesWeights[i];
    uniformVerticesWeights_ = uniformVerticesWeights_ && (verticesWeights[i] == weight0);
  }
  verticesWeights_ = verticesWeights;
}

/* Sample weights accessor */
Point KarhunenLoeveSVDAlgorithm::getSampleWeights() const
{
  return sampleWeights_;
}

void KarhunenLoeveSVDAlgorithm::setSampleWeights(const Point & sampleWeights)
{
  const UnsignedInteger sampleSize = sample_.getSize();
  if (!(sampleWeights.getSize() == sampleSize)) throw InvalidArgumentException(HERE) << "Error: expected sample weights of dimension=" << sampleSize << ", got dimension=" << sampleWeights.getSize();
  const Scalar weight0 = sampleWeights[0];
  Scalar weightSum = 0.0;
  uniformSampleWeights_ = true;
  for (UnsignedInteger i = 0; i < sampleSize; ++i)
  {
    if (!(sampleWeights[i] > 0.0)) throw InvalidArgumentException(HERE) << "Error: expected positive sample weights, here weights[" << i << "]=" << sampleWeights[i];
    uniformSampleWeights_ = uniformSampleWeights_ && (sampleWeights[i] == weight0);
    weightSum += sampleWeights[i];
  }
  // Normalize the sample weights to have an unbiased estimator of the mean
  sampleWeights_ = sampleWeights / weightSum;
}

/* Covariance model accessors */
CovarianceModel KarhunenLoeveSVDAlgorithm::getCovarianceModel() const
{
  // If run() has already been called, it gives the tensor form of the covariance
  // else it is the default covariance model
  return getResult().getCovarianceModel();
}

/* Hide the covariance model accessor as it has no meaning in the SVD context */
void KarhunenLoeveSVDAlgorithm::setCovarianceModel(const CovarianceModel & )
{
  // Nothing to do
}

/* String converter */
String KarhunenLoeveSVDAlgorithm::__repr__() const
{
  OSS oss(true);
  oss << "class=" << KarhunenLoeveSVDAlgorithm::GetClassName()
      << ", sample=" << sample_
      << ", centered=" << centeredSample_
      << ", weights=" << verticesWeights_;
  return oss;
}

/* String converter */
String KarhunenLoeveSVDAlgorithm::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << KarhunenLoeveSVDAlgorithm::GetClassName()
      << ", sample=" << sample_
      << ", centered=" << centeredSample_
      << ", weights=" << verticesWeights_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void KarhunenLoeveSVDAlgorithm::save(Advocate & adv) const
{
  KarhunenLoeveAlgorithmImplementation::save(adv);
  adv.saveAttribute( "sample_", sample_ );
  adv.saveAttribute( "centeredSample_", centeredSample_ );
  adv.saveAttribute( "verticesWeights_", verticesWeights_ );
  adv.saveAttribute( "uniformVerticesWeights_", uniformVerticesWeights_ );
}

/* Method load() reloads the object from the StorageManager */
void KarhunenLoeveSVDAlgorithm::load(Advocate & adv)
{
  KarhunenLoeveAlgorithmImplementation::load(adv);
  adv.loadAttribute( "sample_", sample_ );
  adv.loadAttribute( "centeredSample_", centeredSample_ );
  adv.loadAttribute( "verticesWeights_", verticesWeights_ );
  adv.loadAttribute( "uniformVerticesWeights_", uniformVerticesWeights_ );
}

END_NAMESPACE_OPENTURNS
