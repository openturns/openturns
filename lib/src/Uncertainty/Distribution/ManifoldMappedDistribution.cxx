//                                               -*- C++ -*-
/**
 *  @brief The push-forward of a Euclidean distribution onto a Riemannian manifold
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ManifoldMappedDistribution.hxx"
#include "openturns/Normal.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Exception.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Interval.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include <cmath>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ManifoldMappedDistribution)

static const Factory<ManifoldMappedDistribution> Factory_ManifoldMappedDistribution;

/* Default constructor */
ManifoldMappedDistribution::ManifoldMappedDistribution()
  : DistributionImplementation()
  , background_()
  , manifold_()
  , chartCenter_()
  , tangentBasis_(0, 0)
{
  // The default manifold is the 2-sphere in R^3 charted at the north pole
  Point northPole(3);
  northPole[2] = 1.0;
  setBackgroundManifoldAndChartCenter(Normal(2), manifold_, northPole);
}

/* Parameters constructor */
ManifoldMappedDistribution::ManifoldMappedDistribution(const Distribution & background,
    const Manifold & manifold,
    const Point & chartCenter)
  : DistributionImplementation()
  , background_()
  , manifold_()
  , chartCenter_()
  , tangentBasis_(0, 0)
{
  setBackgroundManifoldAndChartCenter(background, manifold, chartCenter);
}

/* Comparison operator */
Bool ManifoldMappedDistribution::operator ==(const ManifoldMappedDistribution & other) const
{
  if (this == &other) return true;
  return equals(other);
}

Bool ManifoldMappedDistribution::equals(const DistributionImplementation & other) const
{
  const ManifoldMappedDistribution * otherDistribution = dynamic_cast<const ManifoldMappedDistribution *>(&other);
  if (!otherDistribution) return false;
  return (background_ == otherDistribution->background_) && (manifold_ == otherDistribution->manifold_) && (chartCenter_ == otherDistribution->chartCenter_);
}

/* String converter */
String ManifoldMappedDistribution::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " name=" << getName()
         << " background=" << background_.__repr__()
         << " manifold=" << manifold_.__repr__()
         << " chartCenter=" << chartCenter_.__repr__();
}

String ManifoldMappedDistribution::__str__(const String & offset) const
{
  return OSS() << offset << getClassName()
         << "(background=" << background_.__str__()
         << ", manifold=" << manifold_.__str__()
         << ", chartCenter=" << chartCenter_.__str__() << ")";
}

/* Virtual constructor */
ManifoldMappedDistribution * ManifoldMappedDistribution::clone() const
{
  return new ManifoldMappedDistribution(*this);
}

/* Set the background, manifold and chart center with check */
void ManifoldMappedDistribution::setBackgroundManifoldAndChartCenter(const Distribution & background,
    const Manifold & manifold,
    const Point & chartCenter)
{
  const UnsignedInteger intrinsicDimension = manifold.getIntrinsicDimension();
  if (background.getDimension() != intrinsicDimension) throw InvalidArgumentException(HERE) << "Error: the background distribution must have a dimension equal to the intrinsic dimension of the manifold, here background dimension=" << background.getDimension() << " and intrinsic dimension=" << intrinsicDimension;
  background_ = background;
  manifold_ = manifold;
  chartCenter_ = manifold_.projectToManifold(chartCenter);
  tangentBasis_ = manifold_.getTangentBasis(chartCenter_);
  setDimension(manifold_.getAmbientDimension());
  setParallel(false);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computeRange();
}

/* Background distribution accessor */
void ManifoldMappedDistribution::setBackground(const Distribution & background)
{
  setBackgroundManifoldAndChartCenter(background, manifold_, chartCenter_);
}

Distribution ManifoldMappedDistribution::getBackground() const
{
  return background_;
}

/* Manifold accessor */
void ManifoldMappedDistribution::setManifold(const Manifold & manifold)
{
  setBackgroundManifoldAndChartCenter(background_, manifold, chartCenter_);
}

Manifold ManifoldMappedDistribution::getManifold() const
{
  return manifold_;
}

/* Chart center accessor */
void ManifoldMappedDistribution::setChartCenter(const Point & chartCenter)
{
  setBackgroundManifoldAndChartCenter(background_, manifold_, chartCenter);
}

Point ManifoldMappedDistribution::getChartCenter() const
{
  return chartCenter_;
}

/* Tangent basis at the chart center accessor */
Sample ManifoldMappedDistribution::getTangentBasis() const
{
  return tangentBasis_;
}

/* Get one realization of the distribution */
Point ManifoldMappedDistribution::getRealization() const
{
  const Point coordinates(background_.getRealization());
  const UnsignedInteger ambientDimension = manifold_.getAmbientDimension();
  const UnsignedInteger intrinsicDimension = manifold_.getIntrinsicDimension();
  Point tangentVector(ambientDimension);
  for (UnsignedInteger k = 0; k < intrinsicDimension; ++k)
  {
    const Point direction(tangentBasis_[k]);
    for (UnsignedInteger i = 0; i < ambientDimension; ++i)
      tangentVector[i] += coordinates[k] * direction[i];
  }
  return manifold_.expMap(chartCenter_, tangentVector);
}

/* Get the PDF of the distribution */
Scalar ManifoldMappedDistribution::computePDF(const Point & point) const
{
  const Scalar logPDF = computeLogPDF(point);
  if (logPDF == -SpecFunc::Infinity) return 0.0;
  return std::exp(logPDF);
}

/* Get the log-PDF of the distribution */
Scalar ManifoldMappedDistribution::computeLogPDF(const Point & point) const
{
  try
  {
    const Point projected(manifold_.projectToManifold(point));
    const Point tangentVector(manifold_.logMap(chartCenter_, projected));
    const UnsignedInteger intrinsicDimension = manifold_.getIntrinsicDimension();
    Point coordinates(intrinsicDimension);
    for (UnsignedInteger k = 0; k < intrinsicDimension; ++k)
    {
      const Point direction(tangentBasis_[k]);
      Scalar scalarProduct = 0.0;
      for (UnsignedInteger i = 0; i < tangentVector.getDimension(); ++i)
        scalarProduct += tangentVector[i] * direction[i];
      coordinates[k] = scalarProduct;
    }
    const Scalar density = manifold_.getExpMapVolumeDensity(coordinates);
    return background_.computeLogPDF(coordinates) - std::log(density);
  }
  catch (const Exception &)
  {
    // At the cut locus of the chart center the density is singular
    return -SpecFunc::Infinity;
  }
}

/* Parameters value and description accessor */
ManifoldMappedDistribution::PointWithDescriptionCollection ManifoldMappedDistribution::getParametersCollection() const
{
  return background_.getParametersCollection();
}

void ManifoldMappedDistribution::setParametersCollection(const PointCollection & parametersCollection)
{
  background_.setParametersCollection(parametersCollection);
  setBackgroundManifoldAndChartCenter(background_, manifold_, chartCenter_);
}

/* Parameters value accessors */
void ManifoldMappedDistribution::setParameter(const Point & parameter)
{
  background_.setParameter(parameter);
  setBackgroundManifoldAndChartCenter(background_, manifold_, chartCenter_);
}

Point ManifoldMappedDistribution::getParameter() const
{
  return background_.getParameter();
}

/* Parameters description accessor */
Description ManifoldMappedDistribution::getParameterDescription() const
{
  return background_.getParameterDescription();
}

/* Tell if the distribution is continuous */
Bool ManifoldMappedDistribution::isContinuous() const
{
  return background_.isContinuous();
}

/* Tell if the distribution is discrete */
Bool ManifoldMappedDistribution::isDiscrete() const
{
  return background_.isDiscrete();
}

/* Compute the numerical range of the distribution given the parameters values */
void ManifoldMappedDistribution::computeRange()
{
  const UnsignedInteger ambientDimension = manifold_.getAmbientDimension();
  const UnsignedInteger sampleSize = ResourceMap::GetAsUnsignedInteger("ManifoldMappedDistribution-SampleSize");
  const Sample sample(getSample(sampleSize));
  const Point lowerBound(sample.getMin());
  const Point upperBound(sample.getMax());
  Point marginedLowerBound(ambientDimension);
  Point marginedUpperBound(ambientDimension);
  for (UnsignedInteger i = 0; i < ambientDimension; ++i)
  {
    // Add a margin of 5% of each marginal span to avoid rejecting points
    // lying on the edge of the support
    const Scalar halfSpan = 0.025 * (upperBound[i] - lowerBound[i]);
    marginedLowerBound[i] = lowerBound[i] - halfSpan;
    marginedUpperBound[i] = upperBound[i] + halfSpan;
  }
  const Interval::BoolCollection finiteLowerBound(ambientDimension, true);
  const Interval::BoolCollection finiteUpperBound(ambientDimension, true);
  setRange(Interval(marginedLowerBound, marginedUpperBound, finiteLowerBound, finiteUpperBound));
}

/* Compute the mean of the distribution */
void ManifoldMappedDistribution::computeMean() const
{
  const UnsignedInteger sampleSize = ResourceMap::GetAsUnsignedInteger("ManifoldMappedDistribution-SampleSize");
  const Sample sample(getSample(sampleSize));
  mean_ = sample.computeMean();
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void ManifoldMappedDistribution::computeCovariance() const
{
  const UnsignedInteger sampleSize = ResourceMap::GetAsUnsignedInteger("ManifoldMappedDistribution-SampleSize");
  const Sample sample(getSample(sampleSize));
  covariance_ = sample.computeCovariance();
  isAlreadyComputedCovariance_ = true;
}

/* Method save() stores the object through the StorageManager */
void ManifoldMappedDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute("background_", background_);
  adv.saveAttribute("manifold_", manifold_);
  adv.saveAttribute("chartCenter_", chartCenter_);
}

/* Method load() reloads the object from the StorageManager */
void ManifoldMappedDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute("background_", background_);
  adv.loadAttribute("manifold_", manifold_);
  adv.loadAttribute("chartCenter_", chartCenter_);
  setBackgroundManifoldAndChartCenter(background_, manifold_, chartCenter_);
}

END_NAMESPACE_OPENTURNS
