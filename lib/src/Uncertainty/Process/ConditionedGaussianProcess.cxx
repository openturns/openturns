//                                               -*- C++ -*-
/**
 *  @brief A class which implements the ConditionedGaussianProcess process
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
{
  // Nothing to do
}

ConditionedGaussianProcess::ConditionedGaussianProcess(const KrigingResult & result,
    const Mesh & mesh)
  : GaussianProcess()
  , krigingResult_(result)
  , trendEvaluationMesh_()
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
      << ", conditional covariance =" << covarianceCholeskyFactor_;
  return oss;
}

String ConditionedGaussianProcess::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "ConditionedGaussianProcess("
      << "mesh=" << mesh_.__str__(offset)
      << ", trend=" << trend_.__str__(offset)
      << ", covariance=" << covarianceModel_.__str__(offset)
      << ", conditional covariance =" << covarianceCholeskyFactor_.__str__(offset)
      << ")";
  return oss;
}

void ConditionedGaussianProcess::initialize()
{
  if (isInitialized_) return;
  // Build the covariance factor
  Sample vertices(mesh_.getVertices());
  // Build the covariance matrix
  CovarianceMatrix covarianceMatrix(krigingResult_.getConditionalCovariance(vertices));
  // Get the Cholesky factor
  LOGINFO(OSS(false) << "Evaluation of the Cholesky factor");
  covarianceCholeskyFactor_ = covarianceMatrix.computeCholesky();
  // Build the trend function
  LOGINFO(OSS(false) << "Build of the trend function");
  const Function krigingEvaluation(krigingResult_.getMetaModel());
  // Evaluation of the trend part (evaluation once)
  trendEvaluationMesh_ = krigingEvaluation(mesh_.getVertices());
  // Set the trend function
  trend_ = TrendTransform(krigingEvaluation);
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
void ConditionedGaussianProcess::setSamplingMethod(const UnsignedInteger samplingMethod)
{
  // Nothing to do
  LOGWARN(OSS(false) << "Only the cholesky factor method is implemented. The method has no effect here");
}

/* Realization generator */
Field ConditionedGaussianProcess::getRealization() const
{
  // 1) L.X product with L: cholesky factor, X the gaussian vector
  // Constantes values
  const UnsignedInteger fullSize = covarianceCholeskyFactor_.getDimension();
  Point gaussianPoint(fullSize);
  // N independent gaussian realizations
  for (UnsignedInteger index = 0; index < fullSize; ++index) gaussianPoint[index] = DistFunc::rNormal();

  // 2) X <- LX
  gaussianPoint = covarianceCholeskyFactor_ * gaussianPoint;
  const UnsignedInteger size = getMesh().getVerticesNumber();
  Sample values(size, getOutputDimension());
  values.getImplementation()->setData(gaussianPoint);

  // 3) Add the trend part
  // This last one is evaluated thanks to the trend function
  // It is similar to trend_(Field(mesh_, values))
  values += trendEvaluationMesh_;
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
}

/* Method load() reloads the object from the StorageManager */
void ConditionedGaussianProcess::load(Advocate & adv)
{
  GaussianProcess::load(adv);
  adv.loadAttribute("krigingResult_", krigingResult_);
  adv.loadAttribute("trendEvaluationMesh_", trendEvaluationMesh_);
}

END_NAMESPACE_OPENTURNS
