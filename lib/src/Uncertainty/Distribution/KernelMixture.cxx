//                                               -*- C++ -*-
/**
 *  @brief Class for a product-kernel multidimensional mixture.
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cmath>
#include "openturns/KernelMixture.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Brent.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(KernelMixture)

static const Factory<KernelMixture> Factory_KernelMixture;

/* Default constructor */
KernelMixture::KernelMixture()
  : ContinuousDistribution()
  , p_kernel_(Distribution().getImplementation())
  , bandwidth_(0)
  , bandwidthInverse_(0)
  , normalizationFactor_(0.0)
  , sample_(1, 1)
  , pdfApproximationCDF_()
  , cdfApproximation_()
  , pdfApproximationCCDF_()
  , ccdfApproximation_()
  , useApproximatePDFCDF_(false)
{
  setName("KernelMixture");
  setBandwidth(Point(1, 1.0));
}

/* Parameters constructor */
KernelMixture::KernelMixture(const Distribution & kernel,
                             const Point & bandwidth,
                             const Sample & sample)
  : ContinuousDistribution()
  , p_kernel_(kernel.getImplementation())
  , bandwidth_(0)
  , bandwidthInverse_(0)
  , normalizationFactor_(0.0)
  , sample_(sample)
  , pdfApproximationCDF_()
  , cdfApproximation_()
  , pdfApproximationCCDF_()
  , ccdfApproximation_()
  , useApproximatePDFCDF_(false)
{
  setName("KernelMixture");
  // We check if the given kernel is 1-D (product kernel)
  if (kernel.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: only 1D kernel is allowed for multidimensional product kernels";
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a KernelMixture based on an empty sample.";
  setDimension(sample.getDimension());
  // This call set also the range.
  setBandwidth(bandwidth);
  if ((getDimension() == 1) && (sample.getSize() >= ResourceMap::GetAsUnsignedInteger("KernelMixture-SmallSize")) && (sample.getSize() < ResourceMap::GetAsUnsignedInteger("KernelMixture-LargeSize")))
  {
    // Here we use the implementation provided by the DistributionImplementation class instead of the ContinuousDistribution class in order to use both the PDF and the CDF
    Collection<PiecewiseHermiteEvaluation> coll(DistributionImplementation::interpolatePDFCDF(ResourceMap::GetAsUnsignedInteger("KernelMixture-PDFCDFDiscretization")));
    pdfApproximationCDF_ = coll[0];
    cdfApproximation_ = coll[1];
    pdfApproximationCCDF_ = coll[2];
    ccdfApproximation_ = coll[3];
    useApproximatePDFCDF_ = true;
  }
  setParallel(p_kernel_->isParallel());
}

/* Comparison operator */
Bool KernelMixture::operator ==(const KernelMixture & other) const
{
  if (this == &other) return true;
  if (useApproximatePDFCDF_) return (bandwidth_ == other.bandwidth_) && (*p_kernel_ == *other.p_kernel_) && (pdfApproximationCDF_ == other.pdfApproximationCDF_) && (cdfApproximation_ == other.cdfApproximation_) && (pdfApproximationCCDF_ == other.pdfApproximationCCDF_) && (ccdfApproximation_ == other.ccdfApproximation_);
  return (bandwidth_ == other.bandwidth_) && (*p_kernel_ == *other.p_kernel_) && (sample_ == other.sample_);
}

Bool KernelMixture::equals(const DistributionImplementation & other) const
{
  const KernelMixture* p_other = dynamic_cast<const KernelMixture*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String KernelMixture::__repr__() const
{
  OSS oss;
  oss << "class=" << KernelMixture::GetClassName()
      << " name=" << getName()
      << " kernel=" << p_kernel_->__repr__()
      << " bandwidth=" << bandwidth_
      << " sample=" << sample_;
  return oss;
}

String KernelMixture::__str__(const String & offset) const
{
  OSS oss;
  oss << getClassName() << "(kernel = " << p_kernel_->__str__() << ", bandwidth = " << bandwidth_.__str__() << ", sample = " << Os::GetEndOfLine() << offset << sample_.__str__(offset);
  return oss;
}


/* Compute the numerical range of the distribution given the parameters values */
void KernelMixture::computeRange()
{
  const Interval kernelRange(p_kernel_->getRange());
  const UnsignedInteger dimension = getDimension();
  const Point lowerBound(sample_.getMin() + kernelRange.getLowerBound()[0] * bandwidth_);
  const Point upperBound(sample_.getMax() + kernelRange.getUpperBound()[0] * bandwidth_);
  const Interval::BoolCollection finiteLowerBound(dimension, kernelRange.getFiniteLowerBound()[0]);
  const Interval::BoolCollection finiteUpperBound(dimension, kernelRange.getFiniteUpperBound()[0]);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Kernel accessor */
void KernelMixture::setKernel(const Distribution & kernel)
{
  // We check if the kernel is 1D
  if (kernel.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the kernel must be 1D for product kernel mixture";
  p_kernel_ = kernel.getImplementation();
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computeRange();
}

Distribution KernelMixture::getKernel() const
{
  return *p_kernel_;
}

/* Sample accessor */
void KernelMixture::setInternalSample(const Sample & sample)
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a KernelMixture based on an empty sample.";
  if (sample.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given sample has dimension=" << sample.getDimension() << ", expected dimension=" << getDimension() << ".";
  sample_ = sample;
  computeRange();
  if ((getDimension() == 1) && (sample.getSize() >= ResourceMap::GetAsUnsignedInteger("KernelMixture-SmallSize")) && (sample.getSize() < ResourceMap::GetAsUnsignedInteger("KernelMixture-LargeSize")))
  {
    // Here we use the implementation provided by the DistributionImplementation class instead of the ContinuousDistribution class in order to use both the PDF and the CDF
    Collection<PiecewiseHermiteEvaluation> coll(DistributionImplementation::interpolatePDFCDF(ResourceMap::GetAsUnsignedInteger("KernelMixture-PDFCDFDiscretization")));
    pdfApproximationCDF_ = coll[0];
    cdfApproximation_ = coll[1];
    pdfApproximationCCDF_ = coll[2];
    ccdfApproximation_ = coll[3];
    useApproximatePDFCDF_ = true;
  }
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
}

Sample KernelMixture::getInternalSample() const
{
  return sample_;
}


/* Bandwidth accessor */
void KernelMixture::setBandwidth(const Point & bandwidth)
{
  const UnsignedInteger dimension = getDimension();
  normalizationFactor_ = sample_.getSize();
  if (bandwidth.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the dimensions of the bandwidth and the sample must be equal";
  bandwidthInverse_ = Point(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar hi = bandwidth[i];
    if (!(hi > 0.0)) throw InvalidArgumentException(HERE) << "Error: the bandwidth components must be > 0, here bandwidth=" << bandwidth;
    bandwidthInverse_[i] = 1.0 / hi;
    normalizationFactor_ *= hi;
  }
  bandwidth_ = bandwidth;
  normalizationFactor_ = 1.0 / normalizationFactor_;
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computeRange();
}

/* Distribution collection accessor */
Point KernelMixture::getBandwidth() const
{
  return bandwidth_;
}

/* Virtual constructor */
KernelMixture * KernelMixture::clone() const
{
  return new KernelMixture(*this);
}

/* Get one realization of the KernelMixture */
Point KernelMixture::getRealization() const
{
  // Select the atom uniformly amongst the possible points
  Point result(sample_[RandomGenerator::IntegerGenerate(sample_.getSize())]);
  // Then add a random noise according to the product kernel
  const UnsignedInteger dimension = getDimension();
  const Sample kernelSample(p_kernel_->getSample(dimension));
  for (UnsignedInteger i = 0; i < dimension; ++i) result[i] += bandwidth_[i] * kernelSample(i, 0);
  return result;
}

/* Get the DDF of the KernelMixture */
Point KernelMixture::computeDDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  Point ddfValue(dimension, 0.0);
  // Quick rejection test
  if (!getRange().numericallyContains(point)) return ddfValue;
  const UnsignedInteger size = sample_.getSize();
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    Point atom(dimension, 0.0);
    Point kernelPdfAtom(dimension, 0.0);
    Scalar pdfAtom = 1.0;
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      atom[j] = (point[j] - sample_(i, j)) * bandwidthInverse_[j];
      kernelPdfAtom[j] = p_kernel_->computePDF(Point(1, atom[j]));
      pdfAtom *= kernelPdfAtom[j];
    }
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      // Only aggregate the values associated with kernelPdfAtom>0
      if (kernelPdfAtom[j] > 0.0)
        ddfValue[j] += pdfAtom / kernelPdfAtom[j] * p_kernel_->computeDDF(Point(1, atom[j]))[0] * bandwidthInverse_[j];
    }
  } /* end for */
  return normalizationFactor_ * ddfValue;
}

/* Get the PDF of the KernelMixture */
Scalar KernelMixture::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  if (useApproximatePDFCDF_)
  {
    if (point[0] < getMean()[0]) return pdfApproximationCDF_(point)[0];
    else return pdfApproximationCCDF_(point)[0];
  }
  Scalar pdfValue = 0.0;
  const UnsignedInteger size = sample_.getSize();
  if (dimension == 1)
  {
    const Scalar x = point[0];
    const Scalar h = bandwidth_[0];
    for(UnsignedInteger i = 0; i < size; ++i) pdfValue += p_kernel_->computePDF((x - sample_(i, 0)) / h);
    return pdfValue / (h * size);
  }
  // Quick rejection test
  //if (!getRange().numericallyContains(point)) return pdfValue;
  //const Scalar pdfEpsilon = p_kernel_->getPDFEpsilon();
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    Scalar pdfAtom = p_kernel_->computePDF((point[0] - sample_(i, 0)) * bandwidthInverse_[0]);
    for (UnsignedInteger j = 1; j < dimension; ++j)
    {
      //if (pdfAtom < pdfEpsilon) break;
      pdfAtom *= p_kernel_->computePDF((point[j] - sample_(i, j)) * bandwidthInverse_[j]);
    }
    pdfValue += pdfAtom;
  } /* end for */
  return normalizationFactor_ * pdfValue;
}

/* Get the CDF of the KernelMixture */
Scalar KernelMixture::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  if (useApproximatePDFCDF_)
  {
    if (point[0] < getMean()[0]) return cdfApproximation_(point)[0];
    else return 1.0 - ccdfApproximation_(point)[0];
  }
  Scalar cdfValue = 0.0;
  const UnsignedInteger size = sample_.getSize();
  if (dimension == 1)
  {
    const Scalar x = point[0];
    const Scalar h = bandwidth_[0];
    for(UnsignedInteger i = 0; i < size; ++i)
      cdfValue += p_kernel_->computeCDF((x - sample_(i, 0)) / h);
    return cdfValue / size;
  }
  // Check against the range of the distribution
  Bool allTooLarge = true;
  Bool oneTooSmall = false;
  const Point lower(getRange().getLowerBound());
  const Point upper(getRange().getUpperBound());
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    allTooLarge = allTooLarge && (point[i] >= upper[i]);
    oneTooSmall = oneTooSmall || (point[i] <= lower[i]);
  }
  if (allTooLarge) return 1.0;
  if (oneTooSmall) return 0.0;
  const Scalar cdfEpsilon = p_kernel_->getCDFEpsilon();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Scalar cdfAtom = p_kernel_->computeCDF((point[0] - sample_(i, 0)) * bandwidthInverse_[0]);
    for (UnsignedInteger j = 1; j < dimension; ++j)
    {
      if (cdfAtom < cdfEpsilon) break;
      cdfAtom *= p_kernel_->computeCDF((point[j] - sample_(i, j)) * bandwidthInverse_[j]);
    }
    cdfValue += cdfAtom;
  } /* end for */
  return cdfValue / size;
}

/* Get the complementary CDF of the distribution */
Scalar KernelMixture::computeComplementaryCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  if (useApproximatePDFCDF_)
  {
    if (point[0] < getMean()[0]) return 1.0 - cdfApproximation_(point)[0];
    else return ccdfApproximation_(point)[0];
  }
  // More accurate computation for 1D case...
  if (dimension == 1) return computeSurvivalFunction(point);
  // ... than in the general case
  return DistributionImplementation::computeComplementaryCDF(point);
}

/* Get the survival function of the KernelMixture */
Scalar KernelMixture::computeSurvivalFunction(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  if (useApproximatePDFCDF_)
  {
    if (point[0] < getMean()[0]) return 1.0 - cdfApproximation_(point)[0];
    else return ccdfApproximation_(point)[0];
  }
  // Check against the range of the distribution
  Bool oneTooLarge = true;
  Bool allTooSmall = false;
  const Point lower(getRange().getLowerBound());
  const Point upper(getRange().getUpperBound());
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    oneTooLarge = oneTooLarge && (point[i] >= upper[i]);
    allTooSmall = allTooSmall || (point[i] <= lower[i]);
  }
  if (oneTooLarge) return 0.0;
  if (allTooSmall) return 1.0;
  const Scalar cdfEpsilon = p_kernel_->getCDFEpsilon();
  Scalar survivalValue = 0.0;
  const UnsignedInteger size = sample_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Scalar cdfAtom = p_kernel_->computeSurvivalFunction((point[0] - sample_(i, 0)) * bandwidthInverse_[0]);
    for (UnsignedInteger j = 1; j < dimension; ++j)
    {
      if (cdfAtom < cdfEpsilon) break;
      cdfAtom *= p_kernel_->computeSurvivalFunction((point[j] - sample_(i, j)) * bandwidthInverse_[j]);
    }
    survivalValue += cdfAtom;
  } /* end for */
  return survivalValue / size;
}

/* Get the probability content of an interval */
Scalar KernelMixture::computeProbability(const Interval & interval) const
{
  const UnsignedInteger dimension = getDimension();
  if (interval.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given interval must have dimension=" << dimension << ", here dimension=" << interval.getDimension();
  const Interval reducedInterval(interval.intersect(getRange()));
  if (reducedInterval == getRange()) return 1.0;
  if (reducedInterval.isEmpty()) return 0.0;
  const Point lowerBound(reducedInterval.getLowerBound());
  const Point upperBound(reducedInterval.getUpperBound());
  if (useApproximatePDFCDF_)
  {
    const Scalar mean = getMean()[0];
    if (lowerBound[0] > mean) return ccdfApproximation_(lowerBound)[0] - ccdfApproximation_(upperBound)[0];
    else return cdfApproximation_(upperBound)[0] - cdfApproximation_(lowerBound)[0];
  }
  Scalar probability = 0.0;
  const UnsignedInteger size = sample_.getSize();
  if (dimension == 1)
  {
    const Scalar hInverse = bandwidthInverse_[0];
    for(UnsignedInteger i = 0; i < size; ++i)
      probability += p_kernel_->computeProbability(Interval((lowerBound[0] - sample_(i, 0)) * hInverse, (upperBound[0] - sample_(i, 0)) * hInverse));
    return probability / size;
  }
  const Scalar probabilityEpsilon = p_kernel_->getCDFEpsilon();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Scalar probabilityAtom = p_kernel_->computeProbability(Interval((lowerBound[0] - sample_(i, 0)) * bandwidthInverse_[0], (upperBound[0] - sample_(i, 0)) * bandwidthInverse_[0]));
    for (UnsignedInteger j = 1; j < dimension; ++j)
    {
      if (probabilityAtom < probabilityEpsilon) break;
      probabilityAtom *= p_kernel_->computeProbability(Interval((lowerBound[j] - sample_(i, j)) * bandwidthInverse_[j], (upperBound[j] - sample_(i, j)) * bandwidthInverse_[j]));
    }
    probability += probabilityAtom;
  } /* end for */
  return probability / size;
}

/* Compute the quantile function of the distribution */
Scalar KernelMixture::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "Error: the method computeScalarQuantile is only defined for 1D distributions";
  if (!useApproximatePDFCDF_) return DistributionImplementation::computeScalarQuantile(prob, tail);
  const Scalar a = getRange().getLowerBound()[0];
  const Scalar b = getRange().getUpperBound()[0];
  if (prob <= 0.0) return (tail ? b : a);
  if (prob >= 1.0) return (tail ? a : b);
  const UnsignedInteger n = cdfApproximation_.getLocations().getSize();
  if (tail)
  {
    // Here we have to solve ComplementaryCDF(x) = prob which is mathematically
    // equivalent to CDF(x) = 1 - prob, but numerically different with an
    // accuracy that depends on prob.
    // The cut-off is around the mean value
    if (prob <= ccdfApproximation_.getValues()(0, 0)) return Brent(quantileEpsilon_, cdfEpsilon_, cdfEpsilon_, quantileIterations_).solve(ccdfApproximation_, prob, ccdfApproximation_.getLocations()[0], ccdfApproximation_.getLocations()[n - 1], ccdfApproximation_.getValues()(0, 0), ccdfApproximation_.getValues()(n - 1, 0));
    return Brent(quantileEpsilon_, cdfEpsilon_, cdfEpsilon_, quantileIterations_).solve(cdfApproximation_, 1.0 - prob, cdfApproximation_.getLocations()[0], cdfApproximation_.getLocations()[n - 1], cdfApproximation_.getValues()(0, 0), cdfApproximation_.getValues()(n - 1, 0));
  }
  // Here we have to solve CDF(x) = prob which is mathematically
  // equivalent to ComplementaryCDF(x) = 1 - prob, but numerically
  // different with an accuracy that depends on prob.
  // The cut-off is around the mean value
  if (prob <= cdfApproximation_.getValues()(n - 1, 0)) return Brent(quantileEpsilon_, cdfEpsilon_, cdfEpsilon_, quantileIterations_).solve(cdfApproximation_, prob, cdfApproximation_.getLocations()[0], cdfApproximation_.getLocations()[n - 1], cdfApproximation_.getValues()(0, 0), cdfApproximation_.getValues()(n - 1, 0));
  return Brent(quantileEpsilon_, cdfEpsilon_, cdfEpsilon_, quantileIterations_).solve(ccdfApproximation_, 1.0 - prob, ccdfApproximation_.getLocations()[0], ccdfApproximation_.getLocations()[n - 1], ccdfApproximation_.getValues()(0, 0), ccdfApproximation_.getValues()(n - 1, 0));
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex KernelMixture::computeCharacteristicFunction(const Scalar x) const
{
  if (x == 0.0) return 1.0;
  Complex cfValue(0.0);
  const UnsignedInteger size = sample_.getSize();
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    cfValue += p_kernel_->computeCharacteristicFunction(x * bandwidth_[0]) * std::exp(Complex(0.0, sample_(i, 0) * x));
  } /* end for */
  return cfValue * (1.0 / size);
}

/* Get the PDF gradient of the distribution */
Point KernelMixture::computePDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In KernelMixture::computePDFGradient(const Point & point) const";
}

/* Get the CDF gradient of the distribution */
Point KernelMixture::computeCDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In KernelMixture::computeCDFGradient(const Point & point) const";
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1)
*/
Scalar KernelMixture::computeConditionalPDF(const Scalar x,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return getMarginal(conditioningDimension).computePDF(x);
  // Build the conditional mixture weights
  const UnsignedInteger size = sample_.getSize();
  Scalar jointPDF = 0.0;
  Scalar marginalPDF = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Scalar marginalAtomPDF = 1.0;
    for (UnsignedInteger j = 0; j < conditioningDimension; ++j)
      marginalAtomPDF *= p_kernel_->computePDF((y[j] - sample_(i, j)) / bandwidth_[j]);
    marginalPDF += marginalAtomPDF;
    jointPDF += marginalAtomPDF * p_kernel_->computePDF((x - sample_(i, conditioningDimension)) / bandwidth_[conditioningDimension]);
  }
  if (marginalPDF <= 0.0) return 0.0;
  // No need to normalize by 1/h as it simplifies
  return jointPDF / marginalPDF;
}

Point KernelMixture::computeSequentialConditionalPDF(const Point & x) const
{
  Point result(dimension_);
  const UnsignedInteger size = sample_.getSize();
  Point atomsValues(size);
  Scalar pdfConditioning = 0.0;
  Scalar currentX = x[0];
  Scalar currentH = bandwidth_[0];
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    atomsValues[i] = p_kernel_->computePDF((currentX - sample_(i, 0)) / currentH);
    pdfConditioning += atomsValues[i];
  }
  result[0] = pdfConditioning / (size * currentH);
  for (UnsignedInteger conditioningDimension = 1; conditioningDimension < dimension_; ++conditioningDimension)
  {
    // Return the result as soon as a conditional pdf is zero
    if (pdfConditioning == 0) return result;
    currentX = x[conditioningDimension];
    currentH = bandwidth_[conditioningDimension];
    Scalar pdfConditioned = 0.0;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      atomsValues[i] *= p_kernel_->computePDF((currentX - sample_(i, conditioningDimension)) / currentH);
      pdfConditioned += atomsValues[i];
    }
    result[conditioningDimension] = pdfConditioned / pdfConditioning;
    pdfConditioning = pdfConditioned;
  } // conditioningDimension
  return result;
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar KernelMixture::computeConditionalCDF(const Scalar x,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return getMarginal(conditioningDimension).computeCDF(x);
  // Build the conditional mixture weights
  const UnsignedInteger size = sample_.getSize();
  Scalar jointCDF = 0.0;
  Scalar marginalPDF = 0.0;
  Scalar h = bandwidth_[conditioningDimension];
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Scalar marginalAtomPDF = p_kernel_->computePDF((y[0] - sample_(i, 0)) / bandwidth_[0]);
    for (UnsignedInteger j = 1; j < conditioningDimension; ++j)
      marginalAtomPDF *= p_kernel_->computePDF((y[j] - sample_(i, j)) / bandwidth_[j]);
    marginalPDF += marginalAtomPDF;
    jointCDF += marginalAtomPDF * p_kernel_->computeCDF((x - sample_(i, conditioningDimension)) / h);
  }
  if (marginalPDF <= 0.0) return 0.0;
  // No need to normalize by 1/h as it simplifies
  return std::min(1.0, jointCDF / marginalPDF);
}

Point KernelMixture::computeSequentialConditionalCDF(const Point & x) const
{
  /* pdf(x_n|x_0,...,x_{n-1})=pdf(x_1,...,x_n)/pdf(x_1,...,x_{n-1})
     cdf(x_n|x_0,...,x_{n-1})=\int_{-\infty}^{x_n}pdf(x_1,...,t)/pdf(x_1,...,x_{n-1})dt
     \int_{-\infty}^{x_n}pdf(x_1,...,t)dt=1/N\sum_{i=1}^N\int_{-\infty}^{x_n}[\prod_{j=1}^{n-1} k((x_j-X^i_j)/h_j)/h_j]k((t-X^i_n)/h_n)/h_n dt
     =1/N\sum_{i=1}^N P_{n-1}^i K((x_n-X^i_n)/h_n)dt
     and
     pdf(x_1,...,x_{n-1})=1/N\sum_{i=1}^N\prod_{j=1}^{n-1}k((x_j-X^i_j)/h_j)/h_j=1/N\sum_{i=1}^N P_{n-1}^i
  */
  Point result(dimension_);
  const UnsignedInteger size = sample_.getSize();
  Point atomsValues(size);
  Scalar currentX = x[0];
  Scalar currentH = bandwidth_[0];
  Scalar pdfConditioning = 0.0;
  Scalar pdfConditioned = 0.0;
  Scalar cdfConditioned = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar kI = p_kernel_->computePDF((currentX - sample_(i, 0)) / currentH) / currentH;
    cdfConditioned += p_kernel_->computeCDF((currentX - sample_(i, 0)) / currentH);
    atomsValues[i] = kI;
    pdfConditioning += kI;
  }
  result[0] = cdfConditioned / size;
  for (UnsignedInteger conditioningDimension = 1; conditioningDimension < dimension_; ++conditioningDimension)
  {
    // Return the result as soon as a conditional pdf is zero
    if (pdfConditioning == 0) return result;
    currentX = x[conditioningDimension];
    currentH = bandwidth_[conditioningDimension];
    pdfConditioned = 0.0;
    cdfConditioned = 0.0;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      cdfConditioned += atomsValues[i] * p_kernel_->computeCDF((currentX - sample_(i, conditioningDimension)) / currentH);
      atomsValues[i] *= p_kernel_->computePDF((currentX - sample_(i, conditioningDimension)) / currentH) / currentH;
      pdfConditioned += atomsValues[i];
    }
    result[conditioningDimension] = cdfConditioned / pdfConditioning;
    pdfConditioning = pdfConditioned;
  } // conditioningDimension
  return result;
}

/* Get the i-th marginal distribution */
Distribution KernelMixture::getMarginal(const UnsignedInteger i) const
{
  const UnsignedInteger dimension = getDimension();
  if (i >= dimension) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // General case
  KernelMixture::Implementation marginal(new KernelMixture(*p_kernel_, Point(1, bandwidth_[i]), sample_.getMarginal(i)));
  marginal->setDescription(Description(1, getDescription()[i]));
  return marginal;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution KernelMixture::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // General case
  KernelMixture::Implementation marginal(new KernelMixture(*p_kernel_, bandwidth_.select(indices), sample_.getMarginal(indices)));
  marginal->setDescription(getDescription().select(indices));
  return marginal;
}

/* Compute the mean of the KernelMixture
 * PDF(x) = C\sum_{i=1}^N\prod_{j=1}^n K((X^i_j-x_j)/h_j),
 * where:
 * C = \frac{1}{N\prod_{k=1}^n h_k}
 * mu_j = \int_{-\infty}^{\infty} x_j\frac{1}{Nh_j}\sum_{i=1}^N K((x_j - X^i_j) / h_j) dx_j
 = \int_{-\infty}^{\infty} \frac{1}{Nh_j}\sum_{i=1}^N(h_jt + X^i_j)K(t) h_j dt
 = \frac{1}{Nh_j}\sum_{i=1}^NX^i_j + \int_{-\infty}^{\infty} \frac{1}{N}\sum_{i=1}^Nh_jtK(t) dt
 = mu_sample_j + h_j mu_K
*/
void KernelMixture::computeMean() const
{
  // We know that the kernel is 1D, so its mean value is actually a scalar
  const Scalar meanKernel = p_kernel_->getMean()[0];
  mean_ = sample_.computeMean();
  isAlreadyComputedMean_ = true;
  // Special case for symmetric kernel
  if (meanKernel == 0.0) return;
  // General case
  mean_ += meanKernel * bandwidth_;
}

/* Compute the covariance of the KernelMixture
   Covariance(KernelMixture) = (1-1/N) Covariance(sample) + Covariance(kernel) * diag(bandwidth[i]^2)
*/
void KernelMixture::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  // We know that the kernel is 1D, so its standard deviation is actually a scalar
  const Scalar sigmaKernel = p_kernel_->getStandardDeviation()[0];
  // Covariance(sample) term, with the proper scaling
  covariance_ = CovarianceMatrix(dimension, Collection<Scalar>(sample_.computeCovariance().getImplementation()->operator*(1.0 - 1.0 / sample_.getSize())));
  // Add the diagonal kernel covariance contribution
  for (UnsignedInteger i = 0; i < dimension; ++i)
    covariance_(i, i) += std::pow(bandwidth_[i] * sigmaKernel, 2);
  isAlreadyComputedCovariance_ = true;
}

/* Get the standard deviation of the distribution. We don't use the square root of the covariance since it involves a O(dim^2) computation where only a O(dim) computation is required.
   std = [var_sample + h^2 var_K]^(1/2)
*/
Point KernelMixture::getStandardDeviation() const
{
  const UnsignedInteger dimension = getDimension();
  // We know that the kernel is 1D, so its standard deviation is actually a scalar
  const Scalar sigmaKernel = p_kernel_->getStandardDeviation()[0];
  Point result(sample_.computeCenteredMoment(2));
  for (UnsignedInteger i = 0; i < dimension; ++i)
    result[i] = std::sqrt(result[i] + std::pow(bandwidth_[i] * sigmaKernel, 2));
  return result;
}

/* Get the skewness of the distribution:
   skew = [skew_sample * std_sample^3 + h^3 * skew_K * std_K^3] / std^3
*/
Point KernelMixture::getSkewness() const
{
  const UnsignedInteger dimension = getDimension();
  // We know that the kernel is 1D, so its standard deviation is actually a scalar
  const Scalar sigmaKernel = p_kernel_->getStandardDeviation()[0];
  // We know that the kernel is 1D, so its skewness is actually a scalar
  const Scalar skewnessKernel = p_kernel_->getSkewness()[0];
  // Standard deviation of the KernelMixture
  const Point sigma(getStandardDeviation());
  Point result(sample_.computeCenteredMoment(3));
  for (UnsignedInteger i = 0; i < dimension; ++i)
    result[i] = (result[i] + std::pow(bandwidth_[i] * sigmaKernel, 3) * skewnessKernel) / std::pow(sigma[i], 3);
  return result;
}

/* Get the kurtosis of the distribution:
   kurt = [kurt_sample * std_sample^4 + h^4 * kurt_K * std_K^4 + 6 * h^2 * var_sample * var_K] / std^4
*/
Point KernelMixture::getKurtosis() const
{
  const UnsignedInteger dimension = getDimension();
  // We know that the kernel is 1D, so its standard deviation is actually a scalar
  const Scalar sigmaKernel = p_kernel_->getStandardDeviation()[0];
  // We know that the kernel is 1D, so its skewness is actually a scalar
  const Scalar kurtosisKernel = p_kernel_->getKurtosis()[0];
  // Standard deviation of the sample
  const Point varSample(sample_.computeCenteredMoment(2));
  // Standard deviation of the KernelMixture
  const Point sigma(getStandardDeviation());
  Point result(sample_.computeCenteredMoment(4));
  for (UnsignedInteger i = 0; i < dimension; ++i)
    result[i] = (result[i] + std::pow(bandwidth_[i] * sigmaKernel, 4) * kurtosisKernel + 6.0 * varSample[i] * std::pow(bandwidth_[i] * sigmaKernel, 2)) / std::pow(sigma[i], 4);
  return result;
}

/* Parameters value and description accessor */
KernelMixture::PointWithDescriptionCollection KernelMixture::getParametersCollection() const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger size = sample_.getSize();
  PointWithDescriptionCollection parameters(dimension + (dimension > 1 ? 1 : 0));
  // The marginal parameters : the sample and the bandwidth
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    PointWithDescription marginalParameters(size + 1);
    Description description(marginalParameters.getDimension());
    for (UnsignedInteger j = 0; j < size; ++j)
    {
      marginalParameters[j] = sample_(j, i);
      if (dimension > 1) description[j] = (OSS() << "x_" << j << "^" << i);
      else description[j] = (OSS() << "x_" << j);
    }
    marginalParameters[size] = bandwidth_[i];
    if (dimension > 1) description[size] = (OSS() << "h_" << i);
    else description[size] = "h";
    marginalParameters.setDescription(description);
    parameters[i] = marginalParameters;
  }
  // The dependence parameters is the union of all the parameters as they all contribute to the copula, presented in a different way
  if (dimension > 1)
  {
    PointWithDescription dependence(dimension * (size + 1));
    Description description(dependence.getDimension());
    UnsignedInteger index = 0;
    for (UnsignedInteger i = 0; i < size; ++i)
      for (UnsignedInteger j = 0; j < dimension; ++j)
      {
        dependence[index] = sample_(i, j);
        description[index] = (OSS() << "x_" << i << "^" << j);
        ++index;
      }
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      dependence[index] = bandwidth_[i];
      description[index] = (OSS() << "h_" << i);
      ++index;
    }
    dependence.setDescription(description);
    parameters[dimension] = dependence;
  }
  return parameters;
} // getParametersCollection


Point KernelMixture::getParameter() const
{
  const UnsignedInteger size = sample_.getSize();
  Point parameter;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    parameter.add(sample_[i]);
  }
  parameter.add(bandwidth_);
  return parameter;
}

Description KernelMixture::getParameterDescription() const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger size = sample_.getSize();
  Description description;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    if (dimension > 1)
      for (UnsignedInteger j = 0; j < dimension; ++ j)
        description.add(OSS() << "x_" << i << "^" << j);
    else
      description.add(OSS() << "x_" << i);
  }

  if (dimension > 1)
    for (UnsignedInteger j = 0; j < dimension; ++ j)
      description.add(OSS() << "h_" << j);
  else
    description.add("h");
  return description;
}


void KernelMixture::setParameter(const Point & parameter)
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger size = sample_.getSize();
  if (parameter.getDimension() != (dimension * (size + 1))) throw InvalidArgumentException(HERE) << "Expected " << (dimension * (size + 1)) << " parameters";
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      sample_(i, j) = parameter[index];
      ++ index;
    }
  }
  for (UnsignedInteger j = 0; j < dimension; ++ j)
  {
    bandwidth_[j] = parameter[index];
    ++ index;
  }
  // Use the constructor in order to recompute the PDF/CDF approximation if needed
  const Scalar w = getWeight();
  *this = KernelMixture(*p_kernel_, bandwidth_, sample_);
  setWeight(w);
}


/* Check if the distribution is elliptical */
Bool KernelMixture::isElliptical() const
{
  // No chance to have something symmetrical if sample size > 2
  if (sample_.getSize() > 2) return false;
  // In dimension 1, elliptical == symmetric
  if (getDimension() == 1) return p_kernel_->isElliptical();
  // In dimension > 1, only samples with 1 point and Normal kernels lead to an elliptical distribution
  return (sample_.getSize() == 1) && (p_kernel_->getClassName() == "Normal");
}

/* Check if the distribution is continuos */
Bool KernelMixture::isContinuous() const
{
  return p_kernel_->isContinuous();
}

/* Tell if the distribution has elliptical copula */
Bool KernelMixture::hasEllipticalCopula() const
{
  // In 1D, all the distributions have an elliptical copula
  if (getDimension() == 1) return true;
  return false;
}

/* Tell if the distribution has independent copula */
Bool KernelMixture::hasIndependentCopula() const
{
  // In 1D, all the distributions have an independent copula
  return (getDimension() == 1);
}

/* Method save() stores the object through the StorageManager */
void KernelMixture::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "kernel_", Distribution(p_kernel_) );
  adv.saveAttribute( "bandwidth_", bandwidth_ );
  adv.saveAttribute( "bandwidthInverse_", bandwidthInverse_ );
  adv.saveAttribute( "normalizationFactor_", normalizationFactor_ );
  adv.saveAttribute( "sample_", sample_ );
  adv.saveAttribute( "pdfApproximationCDF_", pdfApproximationCDF_ );
  adv.saveAttribute( "cdfApproximation_", cdfApproximation_ );
  adv.saveAttribute( "pdfApproximationCCDF_", pdfApproximationCCDF_ );
  adv.saveAttribute( "ccdfApproximation_", ccdfApproximation_ );
  adv.saveAttribute( "useApproximatePDFCDF_", useApproximatePDFCDF_ );
}

/* Method load() reloads the object from the StorageManager */
void KernelMixture::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  Distribution kernel;
  adv.loadAttribute( "kernel_", kernel );
  p_kernel_ = kernel.getImplementation();
  adv.loadAttribute( "bandwidth_", bandwidth_ );
  adv.loadAttribute( "bandwidthInverse_", bandwidthInverse_ );
  adv.loadAttribute( "normalizationFactor_", normalizationFactor_ );
  adv.loadAttribute( "sample_", sample_ );
  adv.loadAttribute( "pdfApproximationCDF_", pdfApproximationCDF_ );
  adv.loadAttribute( "cdfApproximation_", cdfApproximation_ );
  adv.loadAttribute( "pdfApproximationCCDF_", pdfApproximationCCDF_ );
  adv.loadAttribute( "ccdfApproximation_", ccdfApproximation_ );
  adv.loadAttribute( "useApproximatePDFCDF_", useApproximatePDFCDF_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
