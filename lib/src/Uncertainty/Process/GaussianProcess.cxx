//                                               -*- C++ -*-
/**
 *  @brief A class which implements the Gaussian process
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

#include "openturns/GaussianProcess.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/TriangularMatrix.hxx"
#include "openturns/KPermutationsDistribution.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/HMatrixFactory.hxx"
#include "openturns/SymbolicFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GaussianProcess);

static const Factory<GaussianProcess> Factory_GaussianProcess;

GaussianProcess::GaussianProcess()
  : ProcessImplementation()
  , covarianceModel_()
  , covarianceCholeskyFactor_(0)
  , covarianceHMatrix_()
  , isInitialized_(false)
  , hasStationaryTrend_(true)
  , checkedStationaryTrend_(true)
  , trend_()
  , stationaryTrendValue_(0)
  , samplingMethod_(0)
{
#ifdef OPENTURNS_HAVE_MUPARSER
  trend_ = TrendTransform(SymbolicFunction(Description::BuildDefault(covarianceModel_.getSpatialDimension(), "x"), Description(getDimension(), "0.0")));
#else
  trend_ = TrendTransform(Function(Sample(1, covarianceModel_.getSpatialDimension()), Sample(1, getDimension())));
#endif
  setDescription(trend_.getOutputDescription());
}

/* Standard constructor  */
GaussianProcess::GaussianProcess(const TrendTransform & trend,
    const SecondOrderModel & model,
    const Mesh & mesh)
  : ProcessImplementation()
  , covarianceModel_(model.getCovarianceModel())
  , covarianceCholeskyFactor_(0)
  , covarianceHMatrix_()
  , isInitialized_(false)
  , hasStationaryTrend_(false)
  , checkedStationaryTrend_(false)
  , trend_(trend)
  , stationaryTrendValue_(trend.getOutputDimension())
  , samplingMethod_(0)
{
  if (trend.getInputDimension() != model.getSpatialDimension()) throw InvalidArgumentException(HERE) << "Error: the given trend has an input dimension=" << trend.getInputDimension() << " different from the second order model spatial dimension=" << model.getSpatialDimension();
  if (trend.getOutputDimension() != model.getDimension()) throw InvalidArgumentException(HERE) << "Error: the given trend has an output dimension=" << trend.getOutputDimension() << " different from the second order model dimension=" << model.getDimension();
  setMesh(mesh);
  setDimension(model.getDimension());
  setDescription(trend_.getOutputDescription());
}

/* Standard constructor  */
GaussianProcess::GaussianProcess(const TrendTransform & trend,
    const CovarianceModel & covarianceModel,
    const Mesh & mesh)
  : ProcessImplementation()
  , covarianceModel_(covarianceModel)
  , covarianceCholeskyFactor_(0)
  , covarianceHMatrix_()
  , isInitialized_(false)
  , hasStationaryTrend_(false)
  , checkedStationaryTrend_(false)
  , trend_(trend)
  , stationaryTrendValue_(trend.getOutputDimension())
  , samplingMethod_(0)
{
  if (trend.getInputDimension() != covarianceModel.getSpatialDimension()) throw InvalidArgumentException(HERE) << "Error: the given trend has an input dimension=" << trend.getInputDimension() << " different from the covariance model spatial dimension=" << covarianceModel.getSpatialDimension();
  if (trend.getOutputDimension() != covarianceModel.getDimension()) throw InvalidArgumentException(HERE) << "Error: the given trend has an output dimension=" << trend.getOutputDimension() << " different from the covariance model dimension=" << covarianceModel.getDimension();
  setMesh(mesh);
  setDimension(covarianceModel.getDimension());
  setDescription(trend_.getOutputDescription());
}

/* Standard constructor  */
GaussianProcess::GaussianProcess(const SecondOrderModel & model,
    const Mesh & mesh)
  : ProcessImplementation()
  , covarianceModel_(model.getCovarianceModel())
  , covarianceCholeskyFactor_(0)
  , covarianceHMatrix_()
  , isInitialized_(false)
  , hasStationaryTrend_(true)
  , checkedStationaryTrend_(true)
  , trend_()
  , stationaryTrendValue_(model.getDimension())
  , samplingMethod_(0)
{
  setMesh(mesh);
  setDimension(model.getDimension());
#ifdef OPENTURNS_HAVE_MUPARSER
  trend_ = TrendTransform(SymbolicFunction(Description::BuildDefault(getSpatialDimension(), "x"), Description(getDimension(), "0.0")));
#else
  trend_ = TrendTransform(Function(Sample(1, getSpatialDimension()), Sample(1, getDimension())));
#endif
  setDescription(trend_.getOutputDescription());
}

/* Standard constructor  */
GaussianProcess::GaussianProcess(const CovarianceModel & covarianceModel,
    const Mesh & mesh)
  : ProcessImplementation()
  , covarianceModel_(covarianceModel)
  , covarianceCholeskyFactor_(0)
  , covarianceHMatrix_()
  , isInitialized_(false)
  , hasStationaryTrend_(true)
  , checkedStationaryTrend_(true)
  , trend_()
  , stationaryTrendValue_(covarianceModel.getDimension())
  , samplingMethod_(0)
{
  setMesh(mesh);
  setDimension(covarianceModel.getDimension());
#ifdef OPENTURNS_HAVE_MUPARSER
  trend_ = TrendTransform(SymbolicFunction(Description::BuildDefault(getSpatialDimension(), "x"), Description(getDimension(), "0.0")));
#else
  trend_ = TrendTransform(Function(Sample(1, getSpatialDimension()), Sample(1, getDimension())));
#endif
  setDescription(trend_.getOutputDescription());
}

/* Virtual constructor */
GaussianProcess * GaussianProcess::clone() const
{
  return new GaussianProcess(*this);
}

void GaussianProcess::initialize() const
{
  // Initialization of the process
  // Get the covariance matrix (its Cholesky factor)
  CovarianceMatrix covarianceMatrix;
  if (samplingMethod_ != 1)
  {
    LOGINFO(OSS() << "Discretize the covariance model");
    covarianceMatrix = CovarianceMatrix(covarianceModel_.discretize(mesh_));
  }
  // Boolean flag to tell if the regularization is enough
  Bool continuationCondition = true;
  // Scaling factor of the matrix : M-> M + \lambda I with \lambda very small
  // The regularization is needed for fast decreasing covariance models
  const NumericalScalar startingScaling = ResourceMap::GetAsNumericalScalar("GaussianProcess-StartingScaling");
  const NumericalScalar maximalScaling = ResourceMap::GetAsNumericalScalar("GaussianProcess-MaximalScaling");
  NumericalScalar cumulatedScaling = 0.0;
  NumericalScalar scaling = startingScaling;
  HMatrixFactory hmatFactory;
  HMatrixParameters hmatrixParameters;

  while (continuationCondition && (cumulatedScaling < maximalScaling))
  {
    // Unroll the regularization to optimize the computation
    if (samplingMethod_ == 1)
    {
      LOGINFO(OSS() << "Assemble and factor the covariance matrix");
      covarianceHMatrix_ = hmatFactory.build(mesh_.getVertices(), covarianceModel_.getDimension(), true, hmatrixParameters);
      if (covarianceModel_.getDimension() == 1)
      {
        CovarianceAssemblyFunction simple(covarianceModel_, mesh_.getVertices(), cumulatedScaling);
        covarianceHMatrix_.assemble(simple, 'L');
      }
      else
      {
        CovarianceBlockAssemblyFunction block(covarianceModel_, mesh_.getVertices(), cumulatedScaling);
        covarianceHMatrix_.assemble(block, 'L');
      }
      try
      {
        covarianceHMatrix_.factorize("LLt");
        continuationCondition = false;
      }
      catch (InternalException)
      {
        cumulatedScaling += scaling ;
        scaling *= 2.0;
        NumericalScalar assemblyEpsilon = hmatrixParameters.getAssemblyEpsilon() / 10.0;
        hmatrixParameters.setAssemblyEpsilon(assemblyEpsilon);
        NumericalScalar recompressionEpsilon = hmatrixParameters.getRecompressionEpsilon() / 10.0;
        hmatrixParameters.setRecompressionEpsilon(recompressionEpsilon);
        LOGDEBUG(OSS() <<  "Currently, scaling up to "  << cumulatedScaling << " to get an admissible covariance. Maybe compression & recompression factors are not adapted.");
        LOGDEBUG(OSS() <<  "Currently, assembly espilon = "  << assemblyEpsilon );
        LOGDEBUG(OSS() <<  "Currently, recompression epsilon "  <<  recompressionEpsilon);
      }
    }
    else
    {
      const UnsignedInteger fullSize = covarianceMatrix.getDimension();
      for (UnsignedInteger i = 0; i < fullSize; ++i) covarianceMatrix(i, i) += scaling;
      LOGINFO(OSS() << "Factor the covariance matrix");
      try
      {
        covarianceCholeskyFactor_ = covarianceMatrix.computeCholesky();
        continuationCondition = false;
      }
      catch (...)
      {
        cumulatedScaling += scaling ;
        scaling *= 2.0;
      }
    }
  } // While

  if (scaling >= maximalScaling)
    throw InvalidArgumentException(HERE) << "Error; Could not compute the Cholesky factor"
                                         << " Scaling up to "  << cumulatedScaling << " was not enough";

  if (cumulatedScaling > 0.0)
    LOGWARN(OSS() <<  "Warning! Scaling up to "  << cumulatedScaling << " was needed in order to get an admissible covariance. ");

  // The process has been initialized
  isInitialized_ = true;
}

/* String converter */
String GaussianProcess::__repr__() const
{
  OSS oss(true);
  oss << "class=" << GaussianProcess::GetClassName();
  oss << " mesh=" << mesh_
      << " trend=" << trend_
      << " covarianceModel=" << covarianceModel_
      << " covarianceCholeskyFactor=" << covarianceCholeskyFactor_
      << " isInitialized=" << isInitialized_
      << " hasStationaryTrend=" << hasStationaryTrend_
      << " checkedStationaryTrend=" << checkedStationaryTrend_;
  return oss;
}

String GaussianProcess::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "GaussianProcess("
      << "trend=" << trend_.__str__(offset)
      << ", covariance=" << covarianceModel_.__str__(offset)
      << ")";
  return oss;
}

/* Mesh accessor */
void GaussianProcess::setMesh(const Mesh & mesh)
{
  checkedStationaryTrend_ = false;
  ProcessImplementation::setMesh(mesh);
  isInitialized_ = false;
  covarianceCholeskyFactor_ = TriangularMatrix();
  covarianceHMatrix_ = HMatrix();
}

/* TimeGrid accessor */
void GaussianProcess::setTimeGrid(const RegularGrid & timeGrid)
{
  setMesh(timeGrid);
}

/** Set sampling method accessor */
void GaussianProcess::setSamplingMethod(const UnsignedInteger samplingMethod)
{
  if (samplingMethod > 2)
    throw InvalidArgumentException(HERE) << "Sampling method should be 0 (Cholesky), 1 (H-Matrix implementation) or 2 (Gibbs, available only in dimension 1 ";
  if ((samplingMethod == 2) && getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Sampling method Gibbs is available only in dimension 1 ";
  // Set the sampling method
  if (samplingMethod != samplingMethod_)
  {
    samplingMethod_ = samplingMethod;
    isInitialized_ = false;
    covarianceCholeskyFactor_ = TriangularMatrix();
    covarianceHMatrix_ = HMatrix();
  }

}

/* Realization generator */
Field GaussianProcess::getRealization() const
{
  Sample values;
  if ((getDimension() == 1) && (samplingMethod_ == 2))
    values = getRealizationGibbs();
  else if (samplingMethod_ == 1)
    values = getRealizationHMatrix();
  else values = getRealizationCholesky();
  // If constant trend
  if (isTrendStationary())
  {
    // If nonzero trend
    if (stationaryTrendValue_.norm() != 0.0) values += stationaryTrendValue_;
    values.setDescription(getDescription());
    return Field(mesh_, values);
  }
  // else apply the trend
  values.setDescription(getDescription());
  return trend_(Field(mesh_, values));
}

Sample GaussianProcess::getRealizationGibbs() const
{
  const Sample vertices(getMesh().getVertices());
  const UnsignedInteger size = vertices.getSize();
  const UnsignedInteger nMax = std::max(static_cast<UnsignedInteger>(1), ResourceMap::GetAsUnsignedInteger("GaussianProcess-GibbsMaximumIteration"));

  Sample values(size, 1);
  Point diagonal(size);
  const KPermutationsDistribution permutationDistribution(size, size);
  for (UnsignedInteger n = 0; n < nMax; ++n)
  {
    LOGINFO(OSS() << "Gibbs sampler - start iteration " << n + 1 << " over " << nMax);
    const Point permutation(permutationDistribution.getRealization());
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const UnsignedInteger index = static_cast< UnsignedInteger >(permutation[i]);
      LOGDEBUG(OSS() << "Gibbs sampler - update " << i << " -> component " << index << " over " << size - 1);
      // Here we work on the normalized covariance, ie the correlation
      Sample covarianceRow(covarianceModel_.discretizeRow(vertices, index));
      diagonal[index] = covarianceRow[index][0];
      const Point delta(1, (DistFunc::rNormal() - values[index][0]) / diagonal[index]);
      values += covarianceRow * delta;
    }
  }
  // We have to rescale the realization
  for (UnsignedInteger i = 0; i < size; ++i) values[i] *= diagonal[i];
  return values;
}

Sample GaussianProcess::getRealizationCholesky() const
{
  if (!isInitialized_) initialize();
  // Constantes values
  const UnsignedInteger size = getMesh().getVerticesNumber();
  const UnsignedInteger fullSize = covarianceCholeskyFactor_.getDimension();
  const Point gaussianPoint(DistFunc::rNormal(fullSize));

  SampleImplementation values(size, dimension_);
  const Point rawResult(covarianceCholeskyFactor_ * gaussianPoint);
  LOGINFO(OSS() << "In GaussianProcess::getRealizationCholesky(), size=" << size << ", fullSize=" << fullSize << ", gaussianPoint dimension=" << gaussianPoint.getDimension() << ", rawResult dimension=" << rawResult.getDimension());
  values.setData(rawResult);
  return values;
}

Sample GaussianProcess::getRealizationHMatrix() const
{
  if (!isInitialized_) initialize();
  const UnsignedInteger size = getMesh().getVerticesNumber();
  const UnsignedInteger fullSize = covarianceHMatrix_.getNbRows();
  const Point gaussianPoint(DistFunc::rNormal(fullSize));

  Point y(fullSize);
  covarianceHMatrix_.gemv('N', 1.0, gaussianPoint, 0.0, y);
  Sample values(size, dimension_);
  values.getImplementation()->setData(y);
  return values;
}

/* Covariance model accessor */
CovarianceModel GaussianProcess::getCovarianceModel() const
{
  return covarianceModel_;
}

/* Trend accessor */
TrendTransform GaussianProcess::getTrend() const
{
  return trend_;
}

/* Check if the process is stationary */
Bool GaussianProcess::isStationary() const
{
  return covarianceModel_.isStationary() && isTrendStationary();
}

/* Tell if the process is trend stationary */
Bool GaussianProcess::isTrendStationary() const
{
  if (!checkedStationaryTrend_) checkStationaryTrend();
  return hasStationaryTrend_;
}

/* Check if the process is trend stationary */
void GaussianProcess::checkStationaryTrend() const
{
  hasStationaryTrend_ = true;
  checkedStationaryTrend_ = true;
  const UnsignedInteger n = mesh_.getVerticesNumber();
  if (n == 0) return;
  stationaryTrendValue_ = (*trend_.getEvaluation())(mesh_.getVertices()[0]);
  for (UnsignedInteger i = 1; i < n; ++i)
  {
    if ((*trend_.getEvaluation())(mesh_.getVertices()[i]) != stationaryTrendValue_)
    {
      hasStationaryTrend_ = false;
      return;
    }
  }
  return;
}

/* Check if the process is Normal */
Bool GaussianProcess::isNormal() const
{
  return true;
}


/* Method save() stores the object through the StorageManager */
void GaussianProcess::save(Advocate & adv) const
{
  ProcessImplementation::save(adv);
  adv.saveAttribute("covarianceModel_", covarianceModel_);
  adv.saveAttribute("covarianceCholeskyFactor_", covarianceCholeskyFactor_);
  adv.saveAttribute("isInitialized_", isInitialized_);
  adv.saveAttribute("hasStationaryTrend_", hasStationaryTrend_);
  adv.saveAttribute("checkedStationaryTrend_", checkedStationaryTrend_);
  adv.saveAttribute("trend_", trend_);
  adv.saveAttribute("stationaryTrendValue_", stationaryTrendValue_);
  adv.saveAttribute("samplingMethod_", samplingMethod_);
}

/* Method load() reloads the object from the StorageManager */
void GaussianProcess::load(Advocate & adv)
{
  ProcessImplementation::load(adv);
  adv.loadAttribute("covarianceModel_", covarianceModel_);
  adv.loadAttribute("covarianceCholeskyFactor_", covarianceCholeskyFactor_);
  adv.loadAttribute("isInitialized_", isInitialized_);
  adv.loadAttribute("hasStationaryTrend_", hasStationaryTrend_);
  adv.loadAttribute("checkedStationaryTrend_", checkedStationaryTrend_);
  adv.loadAttribute("trend_", trend_);
  adv.loadAttribute("stationaryTrendValue_", stationaryTrendValue_);
  adv.loadAttribute("samplingMethod_", samplingMethod_);
}

END_NAMESPACE_OPENTURNS
