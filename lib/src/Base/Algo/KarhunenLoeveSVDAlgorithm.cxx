//                                               -*- C++ -*-
/**
 *  @brief This class implements the computation of the Karhunen-Loeve
 *         basis and eigenvalues of a given covariance model based on
 *         SVD decomposition of a process sample.
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
#include "openturns/KarhunenLoeveSVDAlgorithm.hxx"
#include "openturns/P1LagrangeEvaluationImplementation.hxx"
#include "openturns/PiecewiseLinearEvaluationImplementation.hxx"
#include "openturns/RankMCovarianceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KarhunenLoeveSVDAlgorithm
 */

CLASSNAMEINIT(KarhunenLoeveSVDAlgorithm);

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
    const NumericalScalar threshold,
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
    const NumericalPoint & verticesWeights,
    const NumericalScalar threshold,
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
    const NumericalPoint & verticesWeights,
    const NumericalPoint & sampleWeights,
    const NumericalScalar threshold,
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
  NumericalPoint mean;
  if (!centeredSample_)
    {
      LOGINFO("Noncentered sample: compute mean");
      const NumericalScalar unbiasedRatio = size / (size - 1.0);
      mean = NumericalPoint(augmentedDimension);
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
    const NumericalScalar coeff = std::sqrt(verticesWeights_[0]);
    for (UnsignedInteger i = 0; i < kTilde; ++i)
    {
      NumericalPoint data = sample_[i].getImplementation()->getData();
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
    NumericalPoint coeffs(verticesNumber);
    for (UnsignedInteger j = 0; j < verticesNumber; ++j)
      coeffs[j] = std::sqrt(verticesWeights_[j]);
    UnsignedInteger shift = 0;
    for (UnsignedInteger i = 0; i < kTilde; ++i)
    {
      const NumericalScalar wI = std::sqrt(sampleWeights_[i]);
      const NumericalSample currentSample(sample_[i]);
      for (UnsignedInteger j = 0; j < verticesNumber; ++j)
      {
        const NumericalScalar wJ = coeffs[j];
	const NumericalPoint currentPoint(currentSample[j]);
        for (UnsignedInteger k = 0; k < dimension; ++k)
	  {
	    designMatrix[shift] = wI * wJ * currentPoint[k];
	    ++shift;
	  } // k
      } // j
    } // i
  } // !uniformWeights
  LOGDEBUG(OSS(false) << "Design matrix=" << designMatrix);
  // Compute the SVD decomposition of the design matrix
  MatrixImplementation U;
  MatrixImplementation Vt;
  const NumericalPoint svd(designMatrix.computeSVD(U, Vt));
  LOGDEBUG(OSS(false) << "U=\n" << U << ", singular values=" << svd);
  NumericalPoint eigenValues(svd.getDimension());
  for (UnsignedInteger i = 0; i < svd.getDimension(); ++i)
    eigenValues[i] = svd[i] * svd[i];
  LOGINFO("Extract the relevant eigenpairs");
  UnsignedInteger K = 0;
  const NumericalScalar lowerBound = threshold_ * std::abs(eigenValues[0]);
  // Find the cut-off in the eigenvalues
  while ((K < std::min(kTilde, verticesNumber)) && (eigenValues[K] >= lowerBound)) ++K;
  LOGDEBUG(OSS() << "Eigenvalues lower bound=" << lowerBound << ", K=" << K);
  LOGINFO("Create eigenmodes values");
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
	const NumericalScalar coefficient = 1.0 / std::sqrt(verticesWeights_[i]);
	for (UnsignedInteger k = 0; k < dimension; ++k)
	  {
	    eigenModesValues[index] *= coefficient;
	    ++index;
	  } // k
      } // i
    } // j
  } // !uniformVerticesWeights_
  // Reduce and rescale the eigenvectors
  MatrixImplementation transposedProjection(augmentedDimension, K);
  NumericalPoint selectedEV(K);
  Basis modes(0);
  ProcessSample modesAsProcessSample(sample_.getMesh(), 0, dimension);
  const UnsignedInteger meshDimension = sample_.getMesh().getDimension();
  NumericalSampleImplementation values(verticesNumber, dimension);
  UnsignedInteger index = 0;
  LOGINFO("Create modes and projection");
  for (UnsignedInteger k = 0; k < K; ++k)
  {
    selectedEV[k] = eigenValues[k];
    MatrixImplementation a(eigenModesValues.getColumn(k));
    const NumericalScalar factor = a[0] < 0.0 ? -1.0 : 1.0;
    // Store the eigen modes in two forms
    values.setData(a * factor);
    modesAsProcessSample.add(values);
    if (meshDimension == 1)
      modes.add(PiecewiseLinearEvaluationImplementation(sample_.getMesh().getVertices().getImplementation()->getData(), values));
    else
      modes.add(P1LagrangeEvaluationImplementation(modesAsProcessSample.getField(k)));
    // Build the relevant column of the transposed projection matrix
    // \vect{\alpha}=\diag{1/\sqrt{\lambda}}[(\sqrt{W}^{-1}U)^tW]F
    //              =\diag{1/\sqrt{\lambda}}[(W.eigenModesValues)^t]F
    // so M^t=[W.eigenModesValues.\diag{1/\sqrt{\lambda}}]^t
    if (uniformVerticesWeights_)
      {
	a *= (factor * verticesWeights_[0] / sqrt(selectedEV[k]));
	std::copy(a.begin(), a.end(), transposedProjection.begin() + index);
	index += augmentedDimension;
      } // uniformVerticesWeights_
    else
      {
	const NumericalScalar inverseSqrtLambda = factor / sqrt(selectedEV[k]);
	UnsignedInteger shift = 0;
	for (UnsignedInteger i = 0; i < verticesNumber; ++i)
	  {
	    const NumericalScalar coefficient = verticesWeights_[i] * inverseSqrtLambda;
	    for (UnsignedInteger j = 0; j < dimension; ++j)
	      {
		transposedProjection[index] = coefficient * a[shift];
		++shift;
		++index;
	      } // j
	  } // i
      } // !uniformVerticesWeights_
  } // k
  LOGINFO("Create KL result");
  covariance_ = RankMCovarianceModel(selectedEV, modes);
  result_ = KarhunenLoeveResultImplementation(covariance_, threshold_, selectedEV, modes, modesAsProcessSample, transposedProjection.transpose());
}

/* Sample accessor */
ProcessSample KarhunenLoeveSVDAlgorithm::getSample() const
{
  return sample_;
}

/* Vertices weights accessor */
NumericalPoint KarhunenLoeveSVDAlgorithm::getVerticesWeights() const
{
  return verticesWeights_;
}

void KarhunenLoeveSVDAlgorithm::setVerticesWeights(const NumericalPoint & verticesWeights)
{
  const UnsignedInteger verticesNumber = sample_.getMesh().getVerticesNumber();
  if (!(verticesWeights.getSize() == verticesNumber)) throw InvalidArgumentException(HERE) << "Error: expected vertices weights of dimension=" << verticesNumber << ", got dimension=" << verticesWeights.getSize();
  const NumericalScalar weight0 = verticesWeights[0];
  for (UnsignedInteger i = 0; i < verticesNumber; ++i)
    {
      if (verticesWeights[i] <= 0.0) throw InvalidArgumentException(HERE) << "Error: expected positive vertices weights, here weights[" << i << "]=" << verticesWeights[i];
      uniformVerticesWeights_ = uniformVerticesWeights_ && (verticesWeights[i] == weight0);
    }
  verticesWeights_ = verticesWeights;
}

/* Sample weights accessor */
NumericalPoint KarhunenLoeveSVDAlgorithm::getSampleWeights() const
{
  return sampleWeights_;
}

void KarhunenLoeveSVDAlgorithm::setSampleWeights(const NumericalPoint & sampleWeights)
{
  const UnsignedInteger sampleSize = sample_.getSize();
  if (!(sampleWeights.getSize() == sampleSize)) throw InvalidArgumentException(HERE) << "Error: expected sample weights of dimension=" << sampleSize << ", got dimension=" << sampleWeights.getSize();
  const NumericalScalar weight0 = sampleWeights[0];
  NumericalScalar weightSum = 0.0;
  for (UnsignedInteger i = 0; i < sampleSize; ++i)
    {
      if (sampleWeights[i] <= 0.0) throw InvalidArgumentException(HERE) << "Error: expected positive sample weights, here weights[" << i << "]=" << sampleWeights[i];
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
void KarhunenLoeveSVDAlgorithm::setCovarianceModel(const CovarianceModel & covariance)
{
  // Nothing to do
}

/* String converter */
String KarhunenLoeveSVDAlgorithm::__repr__() const
{
  OSS oss(true);
  oss << "class=" << KarhunenLoeveSVDAlgorithm::GetClassName()
      << ", sample=" << sample_
      << ", centered=" << std::boolalpha << centeredSample_
      << ", weights=" << verticesWeights_;
  return oss;
}

/* String converter */
String KarhunenLoeveSVDAlgorithm::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << KarhunenLoeveSVDAlgorithm::GetClassName()
      << ", sample=" << sample_
      << ", centered=" << std::boolalpha << centeredSample_
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
