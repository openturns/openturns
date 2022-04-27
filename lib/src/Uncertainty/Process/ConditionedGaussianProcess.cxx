//                                               -*- C++ -*-
/**
 *  @brief A class which implements the ConditionedGaussianProcess process
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

#include "openturns/ConditionedGaussianProcess.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/TriangularMatrix.hxx"
#include "openturns/DistFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ConditionedGaussianProcess)

static const Factory<ConditionedGaussianProcess> Factory_ConditionedGaussianProcess;

ConditionedGaussianProcess::ConditionedGaussianProcess()
  : GaussianProcess()
  , krigingResult_()
  , trendEvaluationMesh_()
  , knownValuesIndices_()
{
  // Nothing to do
}

ConditionedGaussianProcess::ConditionedGaussianProcess(const KrigingResult & result,
    const Mesh & mesh)
  : GaussianProcess()
  , krigingResult_(result)
  , trendEvaluationMesh_()
  , knownValuesIndices_()
{
  // set covariance model
  covarianceModel_ = result.getCovarianceModel();
  // Set the mesh & dimension
  setOutputDimension(covarianceModel_.getOutputDimension());
  if (covarianceModel_.getInputDimension() != mesh.getDimension())
    throw InvalidArgumentException(HERE) << "In ConditionedGaussianProcess::ConditionedGaussianProcess, process dimension incompatible with mesh dimension. Here, (process dimension= " << getOutputDimension() << ", mesh dimension=" << mesh.getDimension() << ")";
  setMesh(mesh);
  // Initialize
  initialize();
}


/* Virtual constructor */
ConditionedGaussianProcess * ConditionedGaussianProcess::clone() const
{
  return new ConditionedGaussianProcess(*this);
}

/* String converter */
String ConditionedGaussianProcess::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ConditionedGaussianProcess::GetClassName()
      << ", mesh=" << mesh_
      << ", trend=" << trend_
      << ", covariance=" << covarianceModel_
      << ", conditional covariance =" << covarianceCholeskyFactor_
      << ", knownValuesIndices =" << knownValuesIndices_;
  return oss;
}

String ConditionedGaussianProcess::__str__(const String & ) const
{
  OSS oss(false);
  oss << "ConditionedGaussianProcess("
      << "mesh=" << mesh_.__str__()
      << ", trend=" << trend_.__str__()
      << ", covariance=" << covarianceModel_.__str__()
      << ", conditional covariance =" << covarianceCholeskyFactor_.__str__()
      << ")";
  return oss;
}

void ConditionedGaussianProcess::initialize()
{
  if (isInitialized_) return;
  const Sample vertices(mesh_.getVertices());
  // Build the covariance matrix
  CovarianceMatrix covarianceMatrix(krigingResult_.getConditionalCovariance(vertices));
  // Now check if there is any point both in the input sample and in the mesh vertices.
  // They are characterized by
  // a zero cross: a null row and a null column which cross at a zero diagonal element.
  // The trick is to replace this value by the maximum marginal variance, then to
  // remember to set to zero the value at the corresponding index during the sampling phase
  Scalar maximumVariance = 0.0;
  for (UnsignedInteger i = 0; i < covarianceMatrix.getDimension(); ++i)
    maximumVariance = std::max(maximumVariance, covarianceMatrix(i, i));
  const Scalar startingScaling = ResourceMap::GetAsScalar("GaussianProcess-StartingScaling");
  const Scalar maximalScaling = ResourceMap::GetAsScalar("GaussianProcess-MaximalScaling");
  const Scalar epsilon = maximumVariance * startingScaling;
  for (UnsignedInteger i = 0; i < covarianceMatrix.getDimension(); ++i)
    if (covarianceMatrix(i, i) <= epsilon)
    {
      // Enforce a strict zero cross
      for (UnsignedInteger j = 0; j < i; ++j)
        covarianceMatrix(i, j) = 0.0;
      // Then put the maximum variance on the diagonal.
      // In theory any positive number should work but
      // this way the condition number should be greatly improved.
      covarianceMatrix(i, i) = maximumVariance;
      knownValuesIndices_.add(i);
    }
  // Get the Cholesky factor
  LOGINFO(OSS(false) << "Evaluation of the Cholesky factor");

  // Boolean flag to tell if the regularization is enough
  Bool continuationCondition = true;
  // Scaling factor of the matrix : M-> M + \lambda I with \lambda very small
  // The regularization is needed for fast decreasing covariance models
  Scalar maxEV = -1.0;
  Scalar cumulatedScaling = 0.0;
  Scalar scaling = startingScaling;
  while (continuationCondition)
  {
    try
    {
      covarianceCholeskyFactor_ = covarianceMatrix.computeCholesky();
      continuationCondition = false;
    }
    // If the factorization failed regularize the matrix
    // Here we use a generic exception as different exceptions may be thrown
    catch (Exception &)
    {
      // If the largest eigenvalue module has not been computed yet...
      if (maxEV < 0.0)
      {
        maxEV = covarianceMatrix.computeLargestEigenValueModule();
        LOGDEBUG(OSS() << "maxEV=" << maxEV);
        scaling *= maxEV;
      }
      cumulatedScaling += scaling ;
      LOGDEBUG(OSS() << "scaling=" << scaling << ", cumulatedScaling=" << cumulatedScaling);
      // Unroll the regularization to optimize the computation
      for (UnsignedInteger i = 0; i < covarianceMatrix.getDimension(); ++i) covarianceMatrix(i, i) += scaling;
      scaling *= 2.0;
      continuationCondition = scaling < maxEV * maximalScaling;
    }
  }
  if (maxEV > 0.0 && scaling >= maximalScaling * maxEV)
    throw InvalidArgumentException(HERE) << "In GaussianProcess, could not compute the Cholesky factor."
                                         << " Scaling up to " << cumulatedScaling << " was not enough";
  if (cumulatedScaling > 0.0)
    LOGWARN(OSS() <<  "Warning! Scaling up to "  << cumulatedScaling << " was needed in order to get an admissible covariance. ");
  // Build the trend function
  LOGINFO(OSS(false) << "Build of the trend function");
  const Function krigingEvaluation(krigingResult_.getMetaModel());
  // Evaluation of the trend over the mesh
  trendEvaluationMesh_ = krigingEvaluation(vertices);
  // Set the trend function
  trend_ = TrendTransform(krigingEvaluation, mesh_);
  // Set descriptions
  trend_.setInputDescription( Description::BuildDefault(getInputDimension(), "x"));
  trend_.setOutputDescription( Description::BuildDefault(getOutputDimension(), "y"));
  // Set description
  setDescription(trend_.getOutputDescription());
  isInitialized_ = true;
}

/* Mesh accessor */
void ConditionedGaussianProcess::setMesh(const Mesh & mesh)
{
  // Checking dimension coherancy between kriging result & mesh
  if (mesh.getDimension() != krigingResult_.getMetaModel().getInputDimension())
    throw InvalidArgumentException(HERE) << "In ConditionedGaussianProcess::ConditionedGaussianProcess, incompatible dimension between mesh & kriging. Here, mesh dimension = " << mesh.getDimension()
                                         << " and kriging input dimension = " <<  krigingResult_.getMetaModel().getInputDimension();
  // Set the mesh
  isInitialized_ = false;
  ProcessImplementation::setMesh(mesh);
  trendEvaluationMesh_ = Sample(0, 0);
  initialize();
}

/* TimeGrid accessor */
void ConditionedGaussianProcess::setTimeGrid(const RegularGrid & timeGrid)
{
  setMesh(timeGrid);
}

/** Set sampling method accessor */
void ConditionedGaussianProcess::setSamplingMethod(const UnsignedInteger )
{
  // Nothing to do
  LOGWARN(OSS(false) << "Only the cholesky factor method is implemented. The method has no effect here");
}

/* Realization generator */
Field ConditionedGaussianProcess::getRealization() const
{
  // Sample the corresponding Gaussian vector mu+L.x
  // where mu is the value of the trend function over the mesh
  // L the Cholesky factor of the covariance discretized over the mesh
  // x an iid sequence of standard normal random variables
  const UnsignedInteger fullSize = covarianceCholeskyFactor_.getDimension();
  Sample values(fullSize, getOutputDimension());
  Point deviation(covarianceCholeskyFactor_ * DistFunc::rNormal(fullSize));
  // Set to zero the deviations at known positions
  for (UnsignedInteger i = 0; i < knownValuesIndices_.getSize(); ++i)
    deviation[knownValuesIndices_[i]] = 0.0;
  values.getImplementation()->setData(trendEvaluationMesh_.getImplementation()->getData() + deviation);
  // Add the description
  values.setDescription(getDescription());
  return Field(mesh_, values);
}

/* Is the underlying a gaussian process ? */
Bool ConditionedGaussianProcess::isNormal() const
{
  return true;
}

/* Is the underlying a stationary process ? */
Bool ConditionedGaussianProcess::isStationary() const
{
  // False since the conditioned variance is null at conditioning points
  return false;
}

/* Method save() stores the object through the StorageManager */
void ConditionedGaussianProcess::save(Advocate & adv) const
{
  GaussianProcess::save(adv);
  adv.saveAttribute("krigingResult_", krigingResult_);
  adv.saveAttribute("trendEvaluationMesh_", trendEvaluationMesh_);
  adv.saveAttribute("knownValuesIndices_", knownValuesIndices_);
}

/* Method load() reloads the object from the StorageManager */
void ConditionedGaussianProcess::load(Advocate & adv)
{
  GaussianProcess::load(adv);
  adv.loadAttribute("krigingResult_", krigingResult_);
  adv.loadAttribute("trendEvaluationMesh_", trendEvaluationMesh_);
  adv.loadAttribute("knownValuesIndices_", knownValuesIndices_);
}

END_NAMESPACE_OPENTURNS
