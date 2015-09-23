//                                               -*- C++ -*-
/**
 *  @brief A class which implements the ConditionedNormalProcess process
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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

#include "ConditionedNormalProcess.hxx"
#include "PersistentObjectFactory.hxx"
#include "Exception.hxx"
#include "Log.hxx"
#include "CovarianceMatrix.hxx"
#include "TriangularMatrix.hxx"
#include "DistFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ConditionedNormalProcess);

static const Factory<ConditionedNormalProcess> RegisteredFactory;

ConditionedNormalProcess::ConditionedNormalProcess()
  : TemporalNormalProcess()
  , krigingResult_()
  , trendEvaluationMesh_()
{
  // Nothing to do
}

ConditionedNormalProcess::ConditionedNormalProcess(const KrigingResult & result,
    const Mesh & mesh)
  : TemporalNormalProcess()
  , krigingResult_(result)
  , trendEvaluationMesh_()
{
  // set covariance model
  covarianceModel_ = result.getCovarianceModel();
  // Set the mesh & dimension
  setDimension(covarianceModel_.getDimension());
  if (covarianceModel_.getSpatialDimension() != mesh.getDimension())
    throw InvalidArgumentException(HERE) << "In ConditionedNormalProcess::ConditionedNormalProcess, process dimension incompatible with mesh dimension. Here, (process dimension= " << getDimension()
                                         << ", mesh dimension=" << mesh.getDimension() << ")";
  setMesh(mesh);
  // Initialize
  initialize();
}


/* Virtual constructor */
ConditionedNormalProcess * ConditionedNormalProcess::clone() const
{
  return new ConditionedNormalProcess(*this);
}

/* String converter */
String ConditionedNormalProcess::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ConditionedNormalProcess::GetClassName()
      << ", mesh=" << mesh_
      << ", trend=" << trend_
      << ", covariance=" << covarianceModel_
      << ", conditional covariance =" << choleskyFactorCovarianceMatrix_;
  return oss;
}

String ConditionedNormalProcess::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "ConditionedNormalProcess("
      << "mesh=" << mesh_.__str__(offset)
      << ", trend=" << trend_.__str__(offset)
      << ", covariance=" << covarianceModel_.__str__(offset)
      << ", conditional covariance =" << choleskyFactorCovarianceMatrix_.__str__(offset)
      << ")";
  return oss;
}

void ConditionedNormalProcess::initialize()
{
  if (isInitialized_) return;
  // Build the covariance factor
  NumericalSample vertices(mesh_.getVertices());
  // Build the covariance matrix
  CovarianceMatrix covarianceMatrix(krigingResult_.getConditionalCovariance(vertices));
  // Get the Cholesky factor
  LOGINFO(OSS(false) << "Evaluation of the Cholesky factor");
  choleskyFactorCovarianceMatrix_ = covarianceMatrix.computeCholesky();
  // Build the trend function
  LOGINFO(OSS(false) << "Build of the trend function");
  const NumericalMathFunction krigingEvaluation(krigingResult_.getMetaModel());
  // Evaluation of the trend part (evaluation once)
  trendEvaluationMesh_ = krigingEvaluation(mesh_.getVertices());
  // Set the trend function
  trend_ = TrendTransform(krigingEvaluation);
  // Set descriptions
  trend_.setInputDescription( Description::BuildDefault(getSpatialDimension(), "x"));
  trend_.setOutputDescription( Description::BuildDefault(getDimension(), "y"));
  // Set description
  setDescription(trend_.getOutputDescription());
  isInitialized_ = true;
}

/* Mesh accessor */
void ConditionedNormalProcess::setMesh(const Mesh & mesh)
{
  // Checking dimension coherancy between kriging result & mesh
  if (mesh.getDimension() != krigingResult_.getMetaModel().getInputDimension())
    throw InvalidArgumentException(HERE) << "In ConditionedNormalProcess::ConditionedNormalProcess, incompatible dimension between mesh & kriging. Here, mesh dimension = " << mesh.getDimension()
                                         << " and kriging input dimension = " <<  krigingResult_.getMetaModel().getInputDimension();
  // Set the mesh
  isInitialized_ = false;
  ProcessImplementation::setMesh(mesh);
  trendEvaluationMesh_ = NumericalSample(0, 0);
  initialize();
}

/* TimeGrid accessor */
void ConditionedNormalProcess::setTimeGrid(const RegularGrid & timeGrid)
{
  setMesh(timeGrid);
}

/** Set sampling method accessor */
void ConditionedNormalProcess::setSamplingMethod(const UnsignedInteger samplingMethod)
{
  // Nothing to do
  LOGWARN(OSS(false) << "Only the cholesky factor method is implemented. The method has no effect here");
}

/* Realization generator */
Field ConditionedNormalProcess::getRealization() const
{
  // 1) L.X product with L: cholesky factor, X the gaussian vector
  // Constantes values
  const UnsignedInteger fullSize(choleskyFactorCovarianceMatrix_.getDimension());
  NumericalPoint gaussianPoint(fullSize);
  // N independent gaussian realizations
  for (UnsignedInteger index = 0; index < fullSize; ++index) gaussianPoint[index] = DistFunc::rNormal();

  // 2) X <- LX
  gaussianPoint = choleskyFactorCovarianceMatrix_ * gaussianPoint;
  const UnsignedInteger size(getMesh().getVerticesNumber());
  NumericalSample values(size, getDimension());
  values.getImplementation()->setData(gaussianPoint);

  // 3) Add the trend part
  // This last one is evaluated thanks to the trend function
  // It is similar to trend_(Field(mesh_, values))
  values += trendEvaluationMesh_;
  values.setDescription(getDescription());
  return Field(mesh_, values);
}

/* Method save() stores the object through the StorageManager */
void ConditionedNormalProcess::save(Advocate & adv) const
{
  TemporalNormalProcess::save(adv);
  adv.saveAttribute("krigingResult_", krigingResult_);
  adv.saveAttribute("trendEvaluationMesh_", trendEvaluationMesh_);
}

/* Method load() reloads the object from the StorageManager */
void ConditionedNormalProcess::load(Advocate & adv)
{
  TemporalNormalProcess::load(adv);
  adv.loadAttribute("krigingResult_", krigingResult_);
  adv.loadAttribute("trendEvaluationMesh_", trendEvaluationMesh_);
}

END_NAMESPACE_OPENTURNS
