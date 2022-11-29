//                                               -*- C++ -*-
/**
 *  @brief A class which implements the Gaussian process
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifdef OPENTURNS_HAVE_ANALYTICAL_PARSER
#include "openturns/SymbolicFunction.hxx"
#else
#include "openturns/DatabaseFunction.hxx"
#endif

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GaussianProcess)

static const Factory<GaussianProcess> Factory_GaussianProcess;

GaussianProcess::GaussianProcess()
  : ProcessImplementation()
  , covarianceModel_()
  , covarianceCholeskyFactor_(0)
  , covarianceHMatrix_()
  , hasStationaryTrend_(true)
  , checkedStationaryTrend_(true)
  , trend_()
  , stationaryTrendValue_(0.0)
{
#ifdef OPENTURNS_HAVE_ANALYTICAL_PARSER
  trend_ = TrendTransform(SymbolicFunction(Description::BuildDefault(covarianceModel_.getInputDimension(), "x"), Description(getOutputDimension(), "0.0")), getMesh());
#else
  trend_ = TrendTransform(DatabaseFunction(Sample(1, covarianceModel_.getInputDimension()), Sample(1, getOutputDimension())), getMesh());
#endif
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
  , hasStationaryTrend_(false)
  , checkedStationaryTrend_(false)
  , trend_(trend)
  , stationaryTrendValue_(trend.getOutputDimension())
{
  if (trend.getTrendFunction().getInputDimension() != covarianceModel.getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the given trend has an input dimension=" << trend.getInputDimension() << " different from the covariance model input dimension=" << covarianceModel.getInputDimension();
  if (trend.getOutputDimension() != covarianceModel.getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the given trend has an output dimension=" << trend.getOutputDimension() << " different from the covariance model dimension=" << covarianceModel.getOutputDimension();
  setMesh(mesh);
  setOutputDimension(covarianceModel.getOutputDimension());
  setDescription(trend_.getOutputDescription());
}

GaussianProcess::GaussianProcess(const CovarianceModel & covarianceModel,
                                 const Mesh & mesh)
  : ProcessImplementation()
  , covarianceModel_(covarianceModel)
  , covarianceCholeskyFactor_(0)
  , covarianceHMatrix_()
  , hasStationaryTrend_(true)
  , checkedStationaryTrend_(true)
  , trend_()
  , stationaryTrendValue_(covarianceModel.getOutputDimension())
{
  // We use the upper class accessor to prevent the reinitialization of the flags
  ProcessImplementation::setMesh(mesh);
  setOutputDimension(covarianceModel.getOutputDimension());
#ifdef OPENTURNS_HAVE_ANALYTICAL_PARSER
  trend_ = TrendTransform(SymbolicFunction(Description::BuildDefault(getInputDimension(), "x"), Description(getOutputDimension(), "0.0")), getMesh());
#else
  trend_ = TrendTransform(DatabaseFunction(Sample(1, getInputDimension()), Sample(1, getOutputDimension())), getMesh());
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
  // There is a specific regularization for h-matrices
  if (samplingMethod_ == SamplingMethod::HMAT)
  {
    HMatrixFactory hmatFactory;
    HMatrixParameters hmatrixParameters;
    LOGINFO(OSS() << "Assemble and factor the covariance matrix");
    covarianceHMatrix_ = hmatFactory.build(mesh_.getVertices(), covarianceModel_.getOutputDimension(), true, hmatrixParameters);
    if (covarianceModel_.getOutputDimension() == 1)
    {
      CovarianceAssemblyFunction simple(covarianceModel_, mesh_.getVertices());
      covarianceHMatrix_.assemble(simple, hmatrixParameters, 'L');
    }
    else
    {
      CovarianceBlockAssemblyFunction block(covarianceModel_, mesh_.getVertices());
      covarianceHMatrix_.assemble(block, hmatrixParameters, 'L');
    }
    covarianceHMatrix_.factorize(hmatrixParameters.getFactorizationMethod());
  }
  else
  {
    // LAPACK
    LOGINFO(OSS() << "Discretize the covariance model");
    CovarianceMatrix covarianceMatrix(covarianceModel_.discretize(mesh_));
    covarianceCholeskyFactor_ = covarianceMatrix.computeRegularizedCholesky();
  }

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

GaussianProcess::SamplingMethod GaussianProcess::getSamplingMethod() const
{
  const GaussianProcess::SamplingMethod output = static_cast<GaussianProcess::SamplingMethod>(samplingMethod_);
  return output;
}

/** Set sampling method accessor */
void GaussianProcess::setSamplingMethod(const SamplingMethod samplingMethod)
{
  if (samplingMethod > 2)
    throw InvalidArgumentException(HERE) << "Sampling method should be 0 (Cholesky), 1 (H-Matrix implementation) or 2 (Gibbs)";

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
  if ((getOutputDimension() == 1) && (samplingMethod_ == 2))
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
  return Field(mesh_, trend_(values));
}

Sample GaussianProcess::getRealizationGibbs() const
{
  const Sample vertices(getMesh().getVertices());
  const UnsignedInteger outputDimension = getOutputDimension();
  const UnsignedInteger size = vertices.getSize();
  const UnsignedInteger fullSize = size * outputDimension;
  const UnsignedInteger nMax = std::max(static_cast<UnsignedInteger>(1), ResourceMap::GetAsUnsignedInteger("GaussianProcess-GibbsMaximumIteration"));

  Sample values(fullSize, 1);
  const KPermutationsDistribution permutationDistribution(fullSize, fullSize);
  const Sample permutationSample(permutationDistribution.getSample(nMax));
  for (UnsignedInteger n = 0; n < nMax; ++n)
  {
    LOGINFO(OSS() << "Gibbs sampler - start iteration " << n + 1 << " over " << nMax);
    for (UnsignedInteger i = 0; i < fullSize; ++i)
    {
      const UnsignedInteger index = static_cast< UnsignedInteger >(permutationSample(n, i));
      LOGDEBUG(OSS() << "Gibbs sampler - update " << i << " -> component " << index << " over " << fullSize - 1);
      // Here we implement equation (6) of Arroyo and Emery (2020) with rho=0 and J={j}
      const Sample covarianceRow(covarianceModel_.discretizeRow(vertices, index));
      const Scalar diagonalIndex = covarianceRow(index, 0);
      const Point delta(1, (std::sqrt(diagonalIndex) * DistFunc::rNormal() - values(index, 0)) / diagonalIndex);
      values += covarianceRow * delta;
    }
  }
  // for output dim > 1 we need to reshape data
  if (outputDimension == 1) return values;
  Sample outputValues(size, outputDimension);
  const Point rawData(values.getImplementation()->getData());
  outputValues.getImplementation()->setData(rawData);
  return outputValues;
}

Sample GaussianProcess::getRealizationCholesky() const
{
  if (!isInitialized_) initialize();
  // Constantes values
  const UnsignedInteger size = getMesh().getVerticesNumber();
  const UnsignedInteger fullSize = covarianceCholeskyFactor_.getDimension();
  const Point gaussianPoint(DistFunc::rNormal(fullSize));

  SampleImplementation values(size, getOutputDimension());
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
  Sample values(size, getOutputDimension());
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
  const Function trendFunction(trend_.getTrendFunction());
  if (trendFunction.getInputDimension() == 0) return;
  stationaryTrendValue_ = trendFunction(mesh_.getVertices()[0]);
  for (UnsignedInteger i = 1; i < n; ++i)
  {
    if (!(trendFunction(mesh_.getVertices()[i]) == stationaryTrendValue_))
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

/* Get the random vector corresponding to the i-th marginal component */
Process GaussianProcess::getMarginal(const Indices & indices) const
{
  const UnsignedInteger outputDimension = getOutputDimension();
  if (!indices.check(outputDimension)) throw InvalidArgumentException(HERE) << "Error: the index must be less than the output dimension";
  if (outputDimension == 1) return clone();

  TrendTransform trend(getTrend().getTrendFunction().getMarginal(indices), getMesh());
  GaussianProcess result(trend, getCovarianceModel().getMarginal(indices), getMesh());
  result.hasStationaryTrend_ = hasStationaryTrend_;
  result.checkedStationaryTrend_ = checkedStationaryTrend_;
  if (hasStationaryTrend_)
  {
    Point stationaryTrendValue(indices.getSize());
    for (UnsignedInteger i = 0; i < indices.getSize(); ++i)
      stationaryTrendValue[i] = stationaryTrendValue_[indices[i]];
    result.stationaryTrendValue_ = stationaryTrendValue;
  }
  return result.clone();
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
