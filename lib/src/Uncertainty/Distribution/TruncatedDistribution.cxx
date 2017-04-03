//                                               -*- C++ -*-
/**
 *  @brief The TruncatedDistribution distribution
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
#include <cstdlib>
#include <cmath>

#include "openturns/TruncatedDistribution.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TruncatedDistribution);

static const Factory<TruncatedDistribution> Factory_TruncatedDistribution;

/* Default constructor */
TruncatedDistribution::TruncatedDistribution()
  : DistributionImplementation()
  , distribution_(Uniform(0.0, 1.0))
  , lowerBound_(0.0)
  , finiteLowerBound_(true)
  , upperBound_(1.0)
  , finiteUpperBound_(true)
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
    const NumericalScalar lowerBound,
    const NumericalScalar upperBound,
    const NumericalScalar thresholdRealization)
  : DistributionImplementation()
{
  setName("TruncatedDistribution");
  setDistribution(distribution);
  setThresholdRealization(thresholdRealization);
  setDimension(1);
  if (upperBound <= lowerBound) throw InvalidArgumentException(HERE) << "Error: the upper bound must be greater than the lower bound, here lowerBound=" << lowerBound << " and upperBound=" << upperBound;
  cdfLowerBound_ = distribution.computeCDF(Point(1, lowerBound));
  cdfUpperBound_ = distribution.computeCDF(Point(1, upperBound));
  if (cdfLowerBound_ == cdfUpperBound_) throw InvalidArgumentException(HERE) << "Error: the interval [lowerBound, upperBound] must contain a non-empty part of the support of the distribution, here CDf(lowerBound)=" << cdfLowerBound_ << " and CDF(upperBound)=" << cdfUpperBound_;
  normalizationFactor_ = 1.0 / (cdfUpperBound_ - cdfLowerBound_);
  pdfLowerBound_ = distribution.computePDF(Point(1, lowerBound));
  pdfUpperBound_ = distribution.computePDF(Point(1, upperBound));
  lowerBound_ = lowerBound;
  upperBound_ = upperBound;
  finiteLowerBound_ = true;
  finiteUpperBound_ = true;
  computeRange();
}

/* Parameters constructor to use when one of the bounds is not finite */
TruncatedDistribution::TruncatedDistribution(const Distribution & distribution,
    const NumericalScalar bound,
    const BoundSide side,
    const NumericalScalar thresholdRealization)
  : DistributionImplementation()
{
  setName("TruncatedDistribution");
  if (distribution.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can truncate only distribution with dimension=1, here dimension=" << distribution.getDimension();
  distribution_ = distribution;
  setThresholdRealization(thresholdRealization);
  setDimension(1);
  setDescription(distribution.getDescription());
  switch (side)
  {
    case LOWER:
      lowerBound_ = bound;
      finiteLowerBound_ = true;
      cdfLowerBound_ = distribution.computeCDF(Point(1, bound));
      if (cdfLowerBound_ == 1.0) throw InvalidArgumentException(HERE) << "Error: the interval [bound, +inf[ must contain a non-empty part of the support of the distribution, here CDF(bound)=" << cdfLowerBound_;
      pdfLowerBound_ = distribution.computePDF(Point(1, bound));
      upperBound_ = distribution.getRange().getUpperBound()[0];
      cdfUpperBound_ = 1.0;
      pdfUpperBound_ = 0.0;
      finiteUpperBound_ = distribution.getRange().getFiniteUpperBound()[0] == 1;
      break;
    case UPPER:
      upperBound_ = bound;
      finiteUpperBound_ = true;
      cdfUpperBound_ = distribution.computeCDF(Point(1, bound));
      if (cdfUpperBound_ == 0.0) throw InvalidArgumentException(HERE) << "Error: the interval ]-inf, bound] must contain a non-empty part of the support of the distribution, here CDF(bound)=" << cdfUpperBound_;
      pdfUpperBound_ = distribution.computePDF(Point(1, bound));
      lowerBound_ = distribution.getRange().getLowerBound()[0];
      cdfLowerBound_ = 0.0;
      pdfLowerBound_ = 0.0;
      finiteLowerBound_ = distribution.getRange().getFiniteLowerBound()[0] == 1;
      break;
    default:
      throw InvalidArgumentException(HERE) << "Error: invalid side argument for bounds, must be LOWER or UPPER, here side=" << side;
  } /* end switch */
  normalizationFactor_ = 1.0 / (cdfUpperBound_ - cdfLowerBound_);
  // Adjust the truncation interval and the distribution range
  computeRange();
  lowerBound_ = getRange().getLowerBound()[0];
  upperBound_ = getRange().getUpperBound()[0];
  finiteLowerBound_ = getRange().getFiniteLowerBound()[0] == 1;
  finiteUpperBound_ = getRange().getFiniteUpperBound()[0] == 1;
}


/* Parameters constructor to use when one of the bounds is not finite */
TruncatedDistribution::TruncatedDistribution(const Distribution & distribution,
    const Interval & truncationInterval,
    const NumericalScalar thresholdRealization)
  : DistributionImplementation()
{
  setName("TruncatedDistribution");
  if (distribution.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can truncate only distribution with dimension=1, here dimension=" << distribution.getDimension();
  distribution_ = distribution;
  setThresholdRealization(thresholdRealization);
  setDimension(1);
  setDescription(distribution.getDescription());
  lowerBound_ = truncationInterval.getLowerBound()[0];
  upperBound_ = truncationInterval.getUpperBound()[0];
  finiteLowerBound_ = truncationInterval.getFiniteLowerBound()[0] == 1;
  finiteUpperBound_ = truncationInterval.getFiniteUpperBound()[0] == 1;
  cdfLowerBound_ = distribution.computeCDF(Point(1, lowerBound_));
  cdfUpperBound_ = distribution.computeCDF(Point(1, upperBound_));
  if (cdfLowerBound_ >= cdfUpperBound_) throw InvalidArgumentException(HERE) << "Error: the truncation interval does not contain a non-empty part of the support of the distribution";
  pdfLowerBound_ = distribution.computePDF(Point(1, lowerBound_));
  pdfUpperBound_ = distribution.computePDF(Point(1, upperBound_));
  normalizationFactor_ = 1.0 / (cdfUpperBound_ - cdfLowerBound_);
  // Adjust the truncation interval and the distribution range
  computeRange();
  lowerBound_ = getRange().getLowerBound()[0];
  upperBound_ = getRange().getUpperBound()[0];
  finiteLowerBound_ = getRange().getFiniteLowerBound()[0] == 1;
  finiteUpperBound_ = getRange().getFiniteUpperBound()[0] == 1;
}


/* Comparison operator */
Bool TruncatedDistribution::operator ==(const TruncatedDistribution & other) const
{
  if (this == &other) return true;
  return (lowerBound_ == other.getLowerBound()) && (upperBound_ == other.getUpperBound()) && (distribution_ == other.getDistribution());
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
      << " lowerBound=" << lowerBound_
      << " finiteLowerBound=" << finiteLowerBound_
      << " upperBound=" << upperBound_
      << " finiteUpperBound=" << finiteUpperBound_
      << " thresholdRealization=" << thresholdRealization_;
  return oss;
}

String TruncatedDistribution::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(" << distribution_.__str__() << ", a = ";
  if (finiteLowerBound_) oss << lowerBound_;
  else oss << "-inf";
  oss << ", b = ";
  if (finiteUpperBound_) oss << upperBound_;
  else oss << "+inf";
  oss << ")";
  return oss;
}

/* Virtual constructor */
TruncatedDistribution * TruncatedDistribution::clone() const
{
  return new TruncatedDistribution(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void TruncatedDistribution::computeRange()
{
  const Point lowerBound(1, lowerBound_);
  const Point upperBound(1, upperBound_);
  const Interval::BoolCollection finiteLowerBound(1, finiteLowerBound_);
  const Interval::BoolCollection finiteUpperBound(1, finiteUpperBound_);
  setRange(distribution_.getRange().intersect(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound)));
}

/* Get one realization of the distribution */
Point TruncatedDistribution::getRealization() const
{
  // Do we use CDF inversion?
  if (isContinuous() && (cdfUpperBound_ - cdfLowerBound_) < thresholdRealization_) return computeQuantile(RandomGenerator::Generate());
  // Here we use simple rejection of the underlying distribution against the bounds
  for (;;)
  {
    Point realization(distribution_.getRealization());
    if ((!finiteLowerBound_ || realization[0] >= lowerBound_) && (!finiteUpperBound_ || realization[0] <= upperBound_)) return realization;
  }
}

/* Get the DDF of the distribution: DDF_trunc = 1[a, b] * DDF / (CDF(b) - CDF(a)) */
Point TruncatedDistribution::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  if ((x <= lowerBound_) || (x > upperBound_)) return Point(1, 0.0);
  return normalizationFactor_ * distribution_.computeDDF(point);
}


/* Get the PDF of the distribution: PDF_trunc = 1[a, b] * PDF / (CDF(b) - CDF(a)) */
NumericalScalar TruncatedDistribution::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  if ((x < lowerBound_ - quantileEpsilon_) || (x > upperBound_ + quantileEpsilon_)) return 0.0;
  return normalizationFactor_ * distribution_.computePDF(point);
}


/* Get the CDF of the distribution: CDF_trunc = 1[a, b] * (CDF - CDF(a)) / (CDF(b) - CDF(a)) + 1]b, inf] */
NumericalScalar TruncatedDistribution::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  if (x <= lowerBound_) return 0.0;
  if (x >= upperBound_) return 1.0;
  // If tail=true, don't call distribution_.computeCDF with tail=true in the next line!
  return normalizationFactor_ * (distribution_.computeCDF(point) - cdfLowerBound_);
}

NumericalScalar TruncatedDistribution::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  if (x <= lowerBound_) return 1.0;
  if (x > upperBound_) return 0.0;
  // If tail=true, don't call distribution_.computeCDF with tail=true in the next line!
  return normalizationFactor_ * (cdfUpperBound_ - distribution_.computeCDF(point));
}

/* Get the PDFGradient of the distribution */
Point TruncatedDistribution::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  if ((x <= lowerBound_) || (x > upperBound_)) return Point(distribution_.getParametersCollection()[0].getDimension() + finiteLowerBound_ + finiteUpperBound_);
  const Point pdfGradientX(distribution_.computePDFGradient(point));
  const Point cdfGradientLowerBound(finiteLowerBound_ ? distribution_.computeCDFGradient(Point(1, lowerBound_)) : Point(distribution_.getParametersCollection()[0].getDimension()));
  const Point cdfGradientUpperBound(finiteUpperBound_ ? distribution_.computeCDFGradient(Point(1, upperBound_)) : Point(distribution_.getParametersCollection()[0].getDimension()));
  const NumericalScalar pdfPoint = distribution_.computePDF(point);
  Point pdfGradient(normalizationFactor_ * pdfGradientX - pdfPoint * normalizationFactor_ * normalizationFactor_ * (cdfGradientUpperBound - cdfGradientLowerBound));
  // If the lower bound is finite, add a component to the gradient
  if (finiteLowerBound_)
  {
    pdfGradient.add(pdfLowerBound_ * pdfPoint * normalizationFactor_ * normalizationFactor_);
  }
  // If the upper bound is finite, add a component to the gradient
  if (finiteUpperBound_)
  {
    pdfGradient.add(-pdfUpperBound_ * pdfPoint * normalizationFactor_ * normalizationFactor_);
  }
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
Point TruncatedDistribution::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  if ((x <= lowerBound_) || (x > upperBound_)) return Point(distribution_.getParametersCollection()[0].getDimension() + finiteLowerBound_ + finiteUpperBound_);
  const Point cdfGradientX(distribution_.computeCDFGradient(point));
  const Point cdfGradientLowerBound(finiteLowerBound_ ? distribution_.computeCDFGradient(Point(1, lowerBound_)) : Point(distribution_.getParametersCollection()[0].getDimension()));
  const Point cdfGradientUpperBound(finiteUpperBound_ ? distribution_.computeCDFGradient(Point(1, upperBound_)) : Point(distribution_.getParametersCollection()[0].getDimension()));
  const NumericalScalar cdfPoint = distribution_.computeCDF(point);
  Point cdfGradient(normalizationFactor_ * (cdfGradientX - cdfGradientLowerBound) - (cdfPoint - cdfLowerBound_) * normalizationFactor_ * normalizationFactor_ * (cdfGradientUpperBound - cdfGradientLowerBound));
  // If the lower bound is finite, add a component to the gradient
  if (finiteLowerBound_)
  {
    cdfGradient.add(pdfLowerBound_ * normalizationFactor_ * ((cdfPoint - cdfLowerBound_) * normalizationFactor_ - 1.0));
  }
  // If the upper bound is finite, add a component to the gradient
  if (finiteUpperBound_)
  {
    cdfGradient.add(-pdfUpperBound_ * normalizationFactor_ * (cdfPoint - cdfLowerBound_) * normalizationFactor_);
  }
  return cdfGradient;
}

/* Get the quantile of the distribution */
NumericalScalar TruncatedDistribution::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  if (tail) return distribution_.computeQuantile(cdfUpperBound_ - prob * (cdfUpperBound_ - cdfLowerBound_))[0];
  return distribution_.computeQuantile(cdfLowerBound_ + prob * (cdfUpperBound_ - cdfLowerBound_))[0];
}

/* Parameters value accessor */
Point TruncatedDistribution::getParameter() const
{
  Point point(distribution_.getParameter());
  point.add(finiteLowerBound_ ? 1.0 : 0.0);
  point.add(lowerBound_);
  point.add(finiteUpperBound_ ? 1.0 : 0.0);
  point.add(upperBound_);
  return point;
}

void TruncatedDistribution::setParameter(const Point & parameter)
{
  const UnsignedInteger parametersSize = distribution_.getParameterDimension();
  if (parameter.getSize() != parametersSize + 4) throw InvalidArgumentException(HERE) << "Error: expected " << parametersSize + 4 << " values, got " << parameter.getSize();
  Point newParameters(parametersSize);
  std::copy(parameter.begin(), parameter.begin() + parametersSize, newParameters.begin());
  Distribution newDistribution(distribution_);
  newDistribution.setParameter(newParameters);
  const Bool finiteLowerBound = parameter[parametersSize] == 1.0;
  const NumericalScalar lowerBound = parameter[parametersSize + 1];
  const Bool finiteUpperBound = parameter[parametersSize + 2] == 1.0;
  const NumericalScalar upperBound = parameter[parametersSize + 3];
  const NumericalScalar w = getWeight();
  if (finiteLowerBound && finiteUpperBound) *this = TruncatedDistribution(newDistribution, lowerBound, upperBound);
  else if (finiteLowerBound) *this = TruncatedDistribution(newDistribution, lowerBound, LOWER);
  else if (finiteUpperBound) *this = TruncatedDistribution(newDistribution, upperBound, UPPER);
  else throw InvalidArgumentException(HERE) << "Error: no bound";
  setWeight(w);
}

/* Parameters description accessor */
Description TruncatedDistribution::getParameterDescription() const
{
  Description description(distribution_.getParameterDescription());
  description.add("finiteLowerBound");
  description.add("lowerBound");
  description.add("finiteUpperBound");
  description.add("upperBound");
  return description;
}

/* Check if the distribution is elliptical */
Bool TruncatedDistribution::isElliptical() const
{
  return distribution_.isElliptical() && finiteLowerBound_ && finiteUpperBound_ && (std::abs(distribution_.getRange().getLowerBound()[0] - lowerBound_ + distribution_.getRange().getUpperBound()[0] - upperBound_) < ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon"));
}

/* distribution accessor */
void TruncatedDistribution::setDistribution(const Distribution & distribution)
{
  if (distribution.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can truncate only distribution with dimension=1, here dimension=" << distribution.getDimension();
  distribution_ = distribution;
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

/* Realiation threshold accessor */
void TruncatedDistribution::setThresholdRealization(const NumericalScalar thresholdRealization)
{
  if ((thresholdRealization < 0.0) || (thresholdRealization > 1.0)) throw InvalidArgumentException(HERE) << "Realization threshold must be in [0, 1], here thresholdRealization=" << thresholdRealization;
  thresholdRealization_ = thresholdRealization;
}

NumericalScalar TruncatedDistribution::getThresholdRealization() const
{
  return thresholdRealization_;
}

/* Lower bound accessor */
void TruncatedDistribution::setLowerBound(const NumericalScalar lowerBound)
{
  if ((finiteUpperBound_) && (lowerBound > upperBound_)) throw InvalidArgumentException(HERE) << "Error: the lower bound must be strictly less than the upper bound, here lower bound=" << lowerBound << " and upper bound=" << upperBound_;
  cdfLowerBound_ = distribution_.computeCDF(Point(1, lowerBound));
  if (cdfLowerBound_ >= cdfUpperBound_) throw InvalidArgumentException(HERE) << "Error: the truncation interval does not contain a non-empty part of the support of the distribution";
  pdfLowerBound_ = distribution_.computePDF(Point(1, lowerBound));
  lowerBound_ = lowerBound;
  finiteLowerBound_ = true;
  normalizationFactor_ = 1.0 / (cdfUpperBound_ - cdfLowerBound_);
  computeRange();
}

NumericalScalar TruncatedDistribution::getLowerBound() const
{
  return lowerBound_;
}


/* Upper bound accessor */
void TruncatedDistribution::setUpperBound(const NumericalScalar upperBound)
{
  if ((finiteLowerBound_) && (upperBound < lowerBound_)) throw InvalidArgumentException(HERE) << "Error: the upper bound must be strictly greater than the lower bound, here upper bound=" << upperBound << " and lower bound=" << lowerBound_;
  cdfUpperBound_ = distribution_.computeCDF(Point(1, upperBound));
  if (cdfUpperBound_ <= cdfLowerBound_) throw InvalidArgumentException(HERE) << "Error: the truncation interval does not contain a non-empty part of the support of the distribution";
  pdfUpperBound_ = distribution_.computePDF(Point(1, upperBound));
  upperBound_ = upperBound;
  finiteUpperBound_ = true;
  normalizationFactor_ = 1.0 / (cdfUpperBound_ - cdfLowerBound_);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  isAlreadyCreatedGeneratingFunction_ = false;
  computeRange();
}

NumericalScalar TruncatedDistribution::getUpperBound() const
{
  return upperBound_;
}

/* Lower bound finite flag accessor */
void TruncatedDistribution::setFiniteLowerBound(const Bool finiteLowerBound)
{
  // A stange case: the new flag tells that the bound is finite, but no finite previous value has been given
  if (finiteLowerBound && !finiteLowerBound_) throw InvalidArgumentException(HERE) << "Error: cannot set a finite flag on a non finite previous value";
  // If we switched from a finite value to an infinite one, update everything
  if (!finiteLowerBound)
  {
    lowerBound_ = -SpecFunc::MaxNumericalScalar;
    pdfLowerBound_ = 0.0;
    cdfLowerBound_ = 0.0;
    finiteLowerBound_ = false;
    normalizationFactor_ = 1.0 / (cdfUpperBound_ - cdfLowerBound_);
  }
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  isAlreadyCreatedGeneratingFunction_ = false;
  computeRange();
}

Bool TruncatedDistribution::getFiniteLowerBound() const
{
  return finiteLowerBound_;
}

/* Upper bound finite flag accessor */
void TruncatedDistribution::setFiniteUpperBound(const Bool finiteUpperBound)
{
  // A strange case: the new flag tells that the bound is finite, but no finite previous value has been given
  if (finiteUpperBound && !finiteUpperBound_) throw InvalidArgumentException(HERE) << "Error: cannot set a finite flag on a non finite previous value";
  // If we switched from a finite value to an infinite one, update everything
  if (!finiteUpperBound)
  {
    upperBound_ = SpecFunc::MaxNumericalScalar;
    pdfUpperBound_ = 0.0;
    cdfUpperBound_ = 1.0;
    finiteUpperBound_ = false;
    normalizationFactor_ = 1.0 / (cdfUpperBound_ - cdfLowerBound_);
  }
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  isAlreadyCreatedGeneratingFunction_ = false;
  computeRange();
}

Bool TruncatedDistribution::getFiniteUpperBound() const
{
  return finiteUpperBound_;
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
  Point singularities(0);
  Point nontruncatedSingularities(distribution_.getSingularities());
  const UnsignedInteger size = nontruncatedSingularities.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalScalar x = nontruncatedSingularities[i];
    if (x >= upperBound_) return singularities;
    if (x > lowerBound_) singularities.add(x);
  }
  return singularities;
}

/* Method save() stores the object through the StorageManager */
void TruncatedDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "distribution_", distribution_ );
  adv.saveAttribute( "lowerBound_", lowerBound_ );
  adv.saveAttribute( "finiteLowerBound_", finiteLowerBound_ );
  adv.saveAttribute( "upperBound_", upperBound_ );
  adv.saveAttribute( "finiteUpperBound_", finiteUpperBound_ );
  adv.saveAttribute( "thresholdRealization_", thresholdRealization_ );
  adv.saveAttribute( "pdfLowerBound_", pdfLowerBound_ );
  adv.saveAttribute( "cdfLowerBound_", cdfLowerBound_ );
  adv.saveAttribute( "pdfUpperBound_", pdfUpperBound_ );
  adv.saveAttribute( "cdfUpperBound_", cdfUpperBound_ );
  adv.saveAttribute( "normalizationFactor_", normalizationFactor_ );
}

/* Method load() reloads the object from the StorageManager */
void TruncatedDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "distribution_", distribution_ );
  adv.loadAttribute( "lowerBound_", lowerBound_ );
  adv.loadAttribute( "finiteLowerBound_", finiteLowerBound_ );
  adv.loadAttribute( "upperBound_", upperBound_ );
  adv.loadAttribute( "finiteUpperBound_", finiteUpperBound_ );
  adv.loadAttribute( "thresholdRealization_", thresholdRealization_ );
  adv.loadAttribute( "pdfLowerBound_", pdfLowerBound_ );
  adv.loadAttribute( "cdfLowerBound_", cdfLowerBound_ );
  adv.loadAttribute( "pdfUpperBound_", pdfUpperBound_ );
  adv.loadAttribute( "cdfUpperBound_", cdfUpperBound_ );
  adv.loadAttribute( "normalizationFactor_", normalizationFactor_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS

