//                                               -*- C++ -*-
/**
 *  @brief The TruncatedDistribution distribution
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
#include <cstdlib>
#include <cmath>

#include "openturns/TruncatedDistribution.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/Exponential.hxx"
#include "openturns/Normal.hxx"
#include "openturns/TruncatedNormal.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/UserDefined.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef Collection<UnsignedInteger> BoolCollection;

CLASSNAMEINIT(TruncatedDistribution)

static const Factory<TruncatedDistribution> Factory_TruncatedDistribution;

/* Default constructor */
TruncatedDistribution::TruncatedDistribution()
  : DistributionImplementation()
  , distribution_(Uniform(0.0, 1.0))
  , bounds_(1)
  , thresholdRealization_(ResourceMap::GetAsScalar("TruncatedDistribution-DefaultThresholdRealization"))
  , pdfLowerBound_(1.0)
  , pdfUpperBound_(1.0)
  , cdfLowerBound_(0.0)
  , cdfUpperBound_(1.0)
  , normalizationFactor_(1.0)
{
  setName("TruncatedDistribution");
  setDimension(1);
  // Adjust the truncation interval and the distribution range
  computeRange();
}

/* Parameters constructor to use when the two bounds are finite */
TruncatedDistribution::TruncatedDistribution(const Distribution & distribution,
    const Scalar lowerBound,
    const Scalar upperBound,
    const Scalar thresholdRealization)
  : DistributionImplementation()
  , bounds_(Point(distribution.getDimension(), lowerBound), Point(distribution.getDimension(), upperBound))
{
  if (!SpecFunc::IsNormal(lowerBound)) throw InvalidArgumentException(HERE) << "The lower bound parameter must be a real value, here bound=" << lowerBound;
  if (!SpecFunc::IsNormal(upperBound)) throw InvalidArgumentException(HERE) << "The upper bound parameter must be a real value, here bound=" << upperBound;
  setName("TruncatedDistribution");
  // This call also set the range
  setDistribution(distribution);
  setThresholdRealization(thresholdRealization);
}

/* Parameters constructor to use when one of the bounds is not finite */
TruncatedDistribution::TruncatedDistribution(const Distribution & distribution,
    const Scalar bound,
    const BoundSide side,
    const Scalar thresholdRealization)
  : DistributionImplementation()
  , bounds_(distribution.getDimension())
{
  if (!SpecFunc::IsNormal(bound)) throw InvalidArgumentException(HERE) << "The bound parameter must be a real value, here bound=" << bound;
  setName("TruncatedDistribution");
  setThresholdRealization(thresholdRealization);
  switch (side)
  {
    case LOWER:
      bounds_.setLowerBound(Point(distribution.getDimension(), bound));
      bounds_.setUpperBound(distribution.getRange().getUpperBound());
      bounds_.setFiniteUpperBound(distribution.getRange().getFiniteUpperBound());
      break;
    case UPPER:
      bounds_.setLowerBound(distribution.getRange().getLowerBound());
      bounds_.setUpperBound(Point(distribution.getDimension(), bound));
      bounds_.setFiniteLowerBound(distribution.getRange().getFiniteLowerBound());
      break;
    default:
      throw InvalidArgumentException(HERE) << "Error: invalid side argument for bounds, must be LOWER or UPPER, here side=" << side;
  } /* end switch */
  setDistribution(distribution);
}


/* Parameters constructor to use when one of the bounds is not finite */
TruncatedDistribution::TruncatedDistribution(const Distribution & distribution,
    const Interval & truncationInterval,
    const Scalar thresholdRealization)
  : DistributionImplementation()
  , bounds_(truncationInterval)
{
  setName("TruncatedDistribution");
  // This call also set the range and compute the normalization factor
  // Don't use the bounds accessor to avoid computing the range and
  // the normalization factor twice
  setDistribution(distribution);
  setThresholdRealization(thresholdRealization);
}


/* Parameters constructor to use when one of the bounds is not finite */
TruncatedDistribution::TruncatedDistribution(const Distribution & distribution)
  : DistributionImplementation()
  , bounds_(distribution.getRange())
{
  setName("TruncatedDistribution");
  // This call also set the range and compute the normalization factor
  // Don't use the bounds accessor to avoid computing the range and
  // the normalization factor twice
  setDistribution(distribution);
  setThresholdRealization(ResourceMap::GetAsScalar("TruncatedDistribution-DefaultThresholdRealization"));
}


/* Comparison operator */
Bool TruncatedDistribution::operator ==(const TruncatedDistribution & other) const
{
  if (this == &other) return true;
  return (bounds_ == other.bounds_) && (distribution_ == other.getDistribution());
}

Bool TruncatedDistribution::equals(const DistributionImplementation & other) const
{
  const TruncatedDistribution* p_other = dynamic_cast<const TruncatedDistribution*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String TruncatedDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << TruncatedDistribution::GetClassName()
      << " name=" << getName()
      << " distribution=" << distribution_
      << " bounds=" << bounds_
      << " thresholdRealization=" << thresholdRealization_;
  return oss;
}

String TruncatedDistribution::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(" << distribution_.__str__() << ", bounds = " << bounds_.__str__() << ")";
  return oss;
}

/* Virtual constructor */
TruncatedDistribution * TruncatedDistribution::clone() const
{
  return new TruncatedDistribution(*this);
}

/* Get the simplified version (or clone the distribution) */
Distribution TruncatedDistribution::getSimplifiedVersion() const
{
  // Simplification of the 1D case
  Distribution localDistribution(distribution_);
  String kind(localDistribution.getImplementation()->getClassName());
  // Delve into the antecedents until we get something which is not a truncated distribution
  while (kind == "TruncatedDistribution")
  {
    const TruncatedDistribution * truncatedDistribution = dynamic_cast<const TruncatedDistribution *>(localDistribution.getImplementation().get());
    localDistribution = truncatedDistribution->getDistribution();
    kind = localDistribution.getImplementation()->getClassName();
  }
  // If no truncation
  const Scalar w = getWeight();
  const Interval range(getRange());
  if (distribution_.getRange() == range)
  {
    localDistribution.setWeight(w);
    return localDistribution;
  }
  // If UserDefined
  if (kind == "UserDefined")
  {
    const Sample support(localDistribution.getSupport());
    const Point probabilities(localDistribution.getProbabilities());
    Sample reducedSupport(0, localDistribution.getDimension());
    Point reducedProbabilities(0);
    for (UnsignedInteger i = 0; i < support.getSize(); ++i)
    {
      const Point x(support[i]);
      if (range.contains(x))
      {
        reducedSupport.add(x);
        reducedProbabilities.add(probabilities[i]);
      }
    }
    UserDefined simplified(reducedSupport, reducedProbabilities);
    simplified.setWeight(getWeight());
    return simplified;
  }
  // At this point, no more simplification in the multivariate case
  if (getDimension() == 1)
  {
    // Now, the 1D simplifications
    const Scalar b = localDistribution.getRange().getUpperBound()[0];
    const Scalar alpha = getRange().getLowerBound()[0];
    const Scalar beta = getRange().getUpperBound()[0];
    // Actual simplifications
    if (kind == "Uniform")
    {
      Uniform simplified(alpha, beta);
      simplified.setWeight(getWeight());
      return simplified;
    }
    if (kind == "Normal")
    {
      const Normal * normal(dynamic_cast< const Normal * >(localDistribution.getImplementation().get()));
      const Scalar mu = normal->getMean()[0];
      const Scalar sigma = normal->getSigma()[0];
      TruncatedNormal simplified(mu, sigma, alpha, beta);
      simplified.setWeight(getWeight());
      return simplified;
    }
    if (kind == "TruncatedNormal")
    {
      const TruncatedNormal * truncatedNormal(dynamic_cast< const TruncatedNormal * >(localDistribution.getImplementation().get()));
      const Scalar mu = truncatedNormal->getMu();
      const Scalar sigma = truncatedNormal->getSigma();
      TruncatedNormal simplified(mu, sigma, alpha, beta);
      simplified.setWeight(getWeight());
      return simplified;
    }
    if ((kind == "Exponential") && (beta >= b))
    {
      const Exponential * exponential(dynamic_cast< const Exponential * >(localDistribution.getImplementation().get()));
      const Scalar lambda = exponential->getLambda();
      Exponential simplified(lambda, alpha);
      simplified.setWeight(getWeight());
      return simplified;
    }
  }
  // No simplification
  TruncatedDistribution simplifiedTruncated(localDistribution, getRange());
  simplifiedTruncated.setWeight(w);
  return simplifiedTruncated;
}

/* Compute the numerical range of the distribution given the parameters values */
void TruncatedDistribution::computeRange()
{
  const Interval distributionRange(distribution_.getRange());
  if (distributionRange == bounds_)
  {
    setRange(distributionRange);
    normalizationFactor_ = 1.0;
  }
  else
  {
    const Interval range(distributionRange.intersect(bounds_));
    setRange(range);
    const Scalar probability = distribution_.computeProbability(range);
    if (!(probability > 0.0)) throw InvalidArgumentException(HERE) << "Error: the truncation interval does not contain a non-empty part of the support of the distribution";
    normalizationFactor_ = 1.0 / probability;
  }
  epsilonRange_ = getRange() + Interval(Point(dimension_, -quantileEpsilon_), Point(dimension_, quantileEpsilon_));
}

/* Get one realization of the distribution */
Point TruncatedDistribution::getRealization() const
{
  // Use CDF inversion only if P([a, b]) < tau
  if ((getDimension() == 1) && (thresholdRealization_ * normalizationFactor_ > 1.0))
    return computeQuantile(RandomGenerator::Generate());
  // Here we use simple rejection of the underlying distribution against the bounds
  for (;;)
  {
    const Point realization(distribution_.getRealization());
    if (bounds_.contains(realization)) return realization;
  }
}

/* Get the DDF of the distribution: DDF_trunc = 1[a, b] * DDF / P([a, b]) */
Point TruncatedDistribution::computeDDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  if (!getRange().contains(point)) return Point(getDimension(), 0.0);
  return normalizationFactor_ * distribution_.computeDDF(point);
}


/* Get the PDF of the distribution: PDF_trunc = 1[a, b] * PDF / P([a, b]) */
Scalar TruncatedDistribution::computePDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  if (getDimension() == 1)
  {
    const Scalar x = point[0];
    if ((x < getRange().getLowerBound()[0] - quantileEpsilon_) || (x > getRange().getUpperBound()[0] + quantileEpsilon_)) return 0.0;
  }
  else if (!epsilonRange_.contains(point)) return 0.0;

  return normalizationFactor_ * distribution_.computePDF(point);
}


/* Get the CDF of the distribution: CDF_trunc = 1[a, b] * (CDF - CDF(a)) / P([a, b]) + 1]b, inf] */
Scalar TruncatedDistribution::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  if (dimension == 1)
  {
    const Scalar x = point[0];
    if (x <= getRange().getLowerBound()[0]) return 0.0;
    if (x >= getRange().getUpperBound()[0]) return 1.0;

    // If tail=true, don't call distribution_.computeCDF with tail=true in the next line!
    return normalizationFactor_ * (distribution_.computeCDF(point) - cdfLowerBound_);
  }

  // distribution_ should optimize computeProbability
  return normalizationFactor_ * distribution_.computeProbability(Interval(range_.getLowerBound(), point));
}

Scalar TruncatedDistribution::computeSurvivalFunction(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  if (dimension == 1)
  {
    const Scalar xI = point[0];
    if (xI <= getRange().getLowerBound()[0]) return 1.0;
    if (xI >= getRange().getUpperBound()[0]) return 0.0;

    // If tail=true, don't call distribution_.computeCDF with tail=true in the next line!
    return normalizationFactor_ * (cdfUpperBound_ - distribution_.computeCDF(point));
  }

  // distribution_ should optimize computeProbability
  return normalizationFactor_ * distribution_.computeProbability(Interval(point, range_.getUpperBound()));
}

/* Get the PDFGradient of the distribution */
Point TruncatedDistribution::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  if (!bounds_.contains(point)) return Point(getParameterDimension(), 0.0);
  if (getDimension() > 1) return DistributionImplementation::computePDFGradient(point);
  const Point pdfGradientX(distribution_.computePDFGradient(point));
  const Point cdfGradientLowerBound(bounds_.getFiniteLowerBound()[0] ? distribution_.computeCDFGradient(bounds_.getLowerBound()) : Point(distribution_.getParametersCollection()[0].getDimension()));
  const Point cdfGradientUpperBound(bounds_.getFiniteUpperBound()[0] ? distribution_.computeCDFGradient(bounds_.getUpperBound()) : Point(distribution_.getParametersCollection()[0].getDimension()));
  const Scalar pdfPoint = distribution_.computePDF(point);
  Point pdfGradient(normalizationFactor_ * pdfGradientX - pdfPoint * normalizationFactor_ * normalizationFactor_ * (cdfGradientUpperBound - cdfGradientLowerBound));
  // If the lower bound is finite, add a component to the gradient
  if (bounds_.getFiniteLowerBound()[0])
  {
    pdfGradient.add(pdfLowerBound_ * pdfPoint * normalizationFactor_ * normalizationFactor_);
  }
  // If the upper bound is finite, add a component to the gradient
  if (bounds_.getFiniteUpperBound()[0])
  {
    pdfGradient.add(-pdfUpperBound_ * pdfPoint * normalizationFactor_ * normalizationFactor_);
  }
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
Point TruncatedDistribution::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  if (!bounds_.contains(point)) return Point(getParameterDimension(), 0.0);
  if (getDimension() > 1) return DistributionImplementation::computeCDFGradient(point);
  const Point cdfGradientX(distribution_.computeCDFGradient(point));
  const Point cdfGradientLowerBound(bounds_.getFiniteLowerBound()[0] ? distribution_.computeCDFGradient(bounds_.getLowerBound()) : Point(distribution_.getParametersCollection()[0].getDimension()));
  const Point cdfGradientUpperBound(bounds_.getFiniteUpperBound()[0] ? distribution_.computeCDFGradient(bounds_.getUpperBound()) : Point(distribution_.getParametersCollection()[0].getDimension()));
  const Scalar cdfPoint = distribution_.computeCDF(point);
  Point cdfGradient(normalizationFactor_ * (cdfGradientX - cdfGradientLowerBound) - (cdfPoint - cdfLowerBound_) * normalizationFactor_ * normalizationFactor_ * (cdfGradientUpperBound - cdfGradientLowerBound));
  // If the lower bound is finite, add a component to the gradient
  if (bounds_.getFiniteLowerBound()[0])
  {
    cdfGradient.add(pdfLowerBound_ * normalizationFactor_ * ((cdfPoint - cdfLowerBound_) * normalizationFactor_ - 1.0));
  }
  // If the upper bound is finite, add a component to the gradient
  if (bounds_.getFiniteUpperBound()[0])
  {
    cdfGradient.add(-pdfUpperBound_ * normalizationFactor_ * (cdfPoint - cdfLowerBound_) * normalizationFactor_);
  }
  return cdfGradient;
}

/* Get the quantile of the distribution */
Scalar TruncatedDistribution::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "Error: the method computeScalarQuantile is only defined for 1D distributions";

  if (tail) return distribution_.computeQuantile(cdfUpperBound_ - prob * (cdfUpperBound_ - cdfLowerBound_))[0];
  return distribution_.computeQuantile(cdfLowerBound_ + prob * (cdfUpperBound_ - cdfLowerBound_))[0];
}

/* Parameters value accessor */
Point TruncatedDistribution::getParameter() const
{
  Point point(distribution_.getParameter());
  // First the lower bound
  for (UnsignedInteger k = 0; k < getDimension(); ++ k)
    if (bounds_.getFiniteLowerBound()[k])
      point.add(bounds_.getLowerBound()[k]);
  // Second the upper bound
  for (UnsignedInteger k = 0; k < getDimension(); ++ k)
    if (bounds_.getFiniteUpperBound()[k])
      point.add(bounds_.getUpperBound()[k]);
  return point;
}

void TruncatedDistribution::setParameter(const Point & parameter)
{
  const UnsignedInteger parametersSize = distribution_.getParameterDimension();
  const UnsignedInteger dimension = getDimension();
  UnsignedInteger finiteBoundCount = 0;
  for (UnsignedInteger k = 0; k < dimension; ++ k)
  {
    if (bounds_.getFiniteLowerBound()[k])
      ++ finiteBoundCount;
    if (bounds_.getFiniteUpperBound()[k])
      ++ finiteBoundCount;
  }

  if (parameter.getSize() != parametersSize + finiteBoundCount) throw InvalidArgumentException(HERE) << "Error: expected " << parametersSize + finiteBoundCount << " values, got " << parameter.getSize();
  Point newParameters(parametersSize);
  std::copy(parameter.begin(), parameter.begin() + parametersSize, newParameters.begin());
  Distribution newDistribution(distribution_);
  newDistribution.setParameter(newParameters);

  UnsignedInteger index = 0;

  Point lowerBound(bounds_.getLowerBound());
  Point upperBound(bounds_.getUpperBound());
  // First the lower bound
  for (UnsignedInteger k = 0; k < dimension; ++ k)
    if (bounds_.getFiniteLowerBound()[k])
    {
      lowerBound[k] = parameter[parametersSize + index];
      ++ index;
    }
  // Second the upper bound
  for (UnsignedInteger k = 0; k < dimension; ++ k)
    if (bounds_.getFiniteUpperBound()[k])
    {
      upperBound[k] = parameter[parametersSize + index];
      ++ index;
    }
  Interval bounds(lowerBound, upperBound, bounds_.getFiniteLowerBound(), bounds_.getFiniteUpperBound());
  const Scalar w = getWeight();
  *this = TruncatedDistribution(newDistribution, bounds);
  setWeight(w);
}

/* Parameters description accessor */
Description TruncatedDistribution::getParameterDescription() const
{
  Description description(distribution_.getParameterDescription());
  for (UnsignedInteger k = 0; k < getDimension(); ++ k)
  {
    if (bounds_.getFiniteLowerBound()[k])
      description.add((getDimension() > 1) ? String(OSS() << "lowerBound_" << k) : "lowerBound");
  }
  for (UnsignedInteger k = 0; k < getDimension(); ++ k)
  {
    if (bounds_.getFiniteUpperBound()[k])
      description.add((getDimension() > 1) ? String(OSS() << "upperBound_" << k) : "upperBound");
  }
  return description;
}

/* Check if the distribution is elliptical */
Bool TruncatedDistribution::isElliptical() const
{
  if (getDimension() == 1)
  {
    return distribution_.isElliptical() && bounds_.getFiniteLowerBound()[0] && bounds_.getFiniteUpperBound()[0]
           && (std::abs(distribution_.getRange().getLowerBound()[0] - getRange().getLowerBound()[0] + distribution_.getRange().getUpperBound()[0] - getRange().getUpperBound()[0]) < ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon"));
  }
  return (normalizationFactor_ == 1.0) && distribution_.isElliptical();
}

/* distribution accessor */
void TruncatedDistribution::setDistribution(const Distribution & distribution)
{
  if (distribution.getDimension() != bounds_.getDimension())
    throw InvalidArgumentException(HERE) << "The distribution dimension (" << distribution.getDimension()
                                         << ") must match the bounds dimension (" << bounds_.getDimension() << ")";
  distribution_ = distribution;
  setDimension(distribution.getDimension());
  setDescription(distribution.getDescription());
  // Precompute some useful quantities for dimension=1
  if (getDimension() == 1)
  {
    pdfLowerBound_ = distribution.computePDF(bounds_.getLowerBound());
    pdfUpperBound_ = distribution.computePDF(bounds_.getUpperBound());
    cdfLowerBound_ = distribution.computeCDF(bounds_.getLowerBound());
    cdfUpperBound_ = distribution.computeCDF(bounds_.getUpperBound());
  }
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  isAlreadyCreatedGeneratingFunction_ = false;
  isParallel_ = distribution.getImplementation()->isParallel();
  computeRange();
}


Distribution TruncatedDistribution::getDistribution() const
{
  return distribution_;
}


Distribution TruncatedDistribution::getMarginal(const UnsignedInteger i) const
{
  return getMarginal(Indices(1, i));
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution TruncatedDistribution::getMarginal(const Indices & indices) const
{
  Interval marginalBounds(bounds_.getMarginal(indices));
  return new TruncatedDistribution(distribution_.getMarginal(indices), marginalBounds);
}

/* Realization threshold accessor */
void TruncatedDistribution::setThresholdRealization(const Scalar thresholdRealization)
{
  if ((thresholdRealization < 0.0) || (thresholdRealization > 1.0)) throw InvalidArgumentException(HERE) << "Realization threshold must be in [0, 1], here thresholdRealization=" << thresholdRealization;
  thresholdRealization_ = thresholdRealization;
}

Scalar TruncatedDistribution::getThresholdRealization() const
{
  return thresholdRealization_;
}

void TruncatedDistribution::setBounds(const Interval & bounds)
{
  if (distribution_.getDimension() != bounds.getDimension()) throw InvalidArgumentException(HERE) << "The truncation interval dimension must match the distribution dimension.";
  if (bounds_ != bounds)
  {
    bounds_ = bounds;
    // Precompute some useful quantities for dimension=1
    if (getDimension() == 1)
    {
      pdfLowerBound_ = distribution_.computePDF(bounds.getLowerBound());
      pdfUpperBound_ = distribution_.computePDF(bounds.getUpperBound());
      cdfLowerBound_ = distribution_.computeCDF(bounds.getLowerBound());
      cdfUpperBound_ = distribution_.computeCDF(bounds.getUpperBound());
    }
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    isAlreadyCreatedGeneratingFunction_ = false;
    computeRange();
  }
}

Interval TruncatedDistribution::getBounds() const
{
  return bounds_;
}

/* Tell if the distribution is continuous */
Bool TruncatedDistribution::isContinuous() const
{
  return distribution_.isContinuous();
}

/* Tell if the distribution is integer valued */
Bool TruncatedDistribution::isDiscrete() const
{
  return distribution_.isDiscrete();
}

/* Tell if the distribution is integer valued */
Bool TruncatedDistribution::isIntegral() const
{
  return distribution_.isIntegral();
}

/* Get the support of a distribution that intersect a given interval */
Sample TruncatedDistribution::getSupport(const Interval & interval) const
{
  return distribution_.getSupport(getRange().intersect(interval));
}

/* Get the PDF singularities inside of the range - 1D only */
Point TruncatedDistribution::getSingularities() const
{
  if (getDimension() > 1) throw NotYetImplementedException(HERE) << "TruncatedDistribution::getSingularities only defined for univariate distributions.";
  Point singularities(0);
  Point nontruncatedSingularities(distribution_.getSingularities());
  const UnsignedInteger size = nontruncatedSingularities.getSize();
  const Scalar a = bounds_.getLowerBound()[0];
  const Scalar b = bounds_.getUpperBound()[0];
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar x = nontruncatedSingularities[i];
    if (x >= b) return singularities;
    if (x > a) singularities.add(x);
  }
  return singularities;
}

/* Method save() stores the object through the StorageManager */
void TruncatedDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "distribution_", distribution_ );
  adv.saveAttribute( "bounds_", bounds_ );
  adv.saveAttribute( "thresholdRealization_", thresholdRealization_ );
  adv.saveAttribute( "pdfLowerBound_", pdfLowerBound_ );
  adv.saveAttribute( "cdfLowerBound_", cdfLowerBound_ );
  adv.saveAttribute( "pdfUpperBound_", pdfUpperBound_ );
  adv.saveAttribute( "cdfUpperBound_", cdfUpperBound_ );
}

/* Method load() reloads the object from the StorageManager */
void TruncatedDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "distribution_", distribution_ );
  adv.loadAttribute( "bounds_", bounds_ );
  adv.loadAttribute( "thresholdRealization_", thresholdRealization_ );
  adv.loadAttribute( "pdfLowerBound_", pdfLowerBound_ );
  adv.loadAttribute( "cdfLowerBound_", cdfLowerBound_ );
  adv.loadAttribute( "pdfUpperBound_", pdfUpperBound_ );
  adv.loadAttribute( "cdfUpperBound_", cdfUpperBound_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS

