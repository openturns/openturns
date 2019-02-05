//                                               -*- C++ -*-
/**
 *  @brief This class acts like a KernelMixture factory, implementing a
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/KernelSmoothing.hxx"
#include "openturns/Normal.hxx"
#include "openturns/Dirac.hxx"
#include "openturns/Mixture.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Brent.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/Function.hxx"
#include "openturns/HermiteFactory.hxx"
#include "openturns/UniVariatePolynomial.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KernelSmoothing
 *
 * The class describes the probabilistic concept of KernelSmoothing.
 */

CLASSNAMEINIT(KernelSmoothing)

static const Factory<KernelSmoothing> Factory_KernelSmoothing;

/* Default constructor */
KernelSmoothing::KernelSmoothing()
  : DistributionFactoryImplementation()
  , bandwidth_(Point(0))
  , kernel_(Normal())
  , bined_(true)
  , binNumber_(ResourceMap::GetAsUnsignedInteger( "KernelSmoothing-BinNumber" ))
  , boundingOption_(NONE)
  , lowerBound_(0.0)
  , automaticLowerBound_(true)
  , upperBound_(0.0)
  , automaticUpperBound_(true)
{
  setName("KernelSmoothing");
  if (binNumber_ < 2) throw InvalidArgumentException(HERE) << "Error: The default number of bins=" << binNumber_ << " is less than 2. Check the ResourceMap or the openturns.conf file.";
}

/* Parameter constructor */
KernelSmoothing::KernelSmoothing(const Distribution & kernel,
                                 const Bool bined,
                                 const UnsignedInteger binNumber,
                                 const Bool boundaryCorrection)
  : DistributionFactoryImplementation()
  , bandwidth_(Point(0))
  , kernel_(kernel)
  , bined_(bined)
  , binNumber_(binNumber)
  , boundingOption_(boundaryCorrection ? BOTH : NONE)
  , lowerBound_(-SpecFunc::MaxScalar)
  , automaticLowerBound_(true)
  , upperBound_(SpecFunc::MaxScalar)
  , automaticUpperBound_(true)
{
  setName("KernelSmoothing");
  // Only 1D kernel allowed here
  if (kernel.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: only 1D kernel allowed for product kernel smoothing";
  if (bined && (binNumber < 2)) throw InvalidArgumentException(HERE) << "Error: The number of bins=" << binNumber << " is less than 2.";
}

/* Virtual constructor */
KernelSmoothing * KernelSmoothing::clone() const
{
  return new KernelSmoothing(*this);
}

/* Compute the bandwidth according to Silverman's rule */
Point KernelSmoothing::computeSilvermanBandwidth(const Sample & sample) const
{
  UnsignedInteger dimension = sample.getDimension();
  UnsignedInteger size = sample.getSize();
  // Compute the first scale estimator based on inter-quartile
  const Point scaleQuartile((sample.computeQuantilePerComponent(0.75) - sample.computeQuantilePerComponent(0.25)) / (2.0 * DistFunc::qNormal(0.75)));
  Point scaleStd(0);
  Point scale(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (scaleQuartile[i] > 0.0) scale[i] = scaleQuartile[i];
    else
    {
      if (scaleStd.getSize() == 0) scaleStd = sample.computeStandardDeviationPerComponent();
      scale[i] = scaleStd[i];
    }
  }
  // Silverman's Normal rule
  Scalar factor = std::pow(size, -1.0 / (4.0 + dimension)) / kernel_.getStandardDeviation()[0];
  // Scott's Normal rule
  return factor * scale;
}

struct PluginConstraint
{
  /** Constructor from a sample and a derivative factor estimate */
  PluginConstraint(const Sample & sample,
                   const Scalar K,
                   const UnsignedInteger order):
    sample_(sample),
    N_(sample.getSize()),
    K_(K),
    order_(order),
    hermitePolynomial_(HermiteFactory().build(order))
  {
    // Normalize the polynomial
    hermitePolynomial_ = hermitePolynomial_ * (1.0 / hermitePolynomial_.getCoefficients()[order]);
  };

  /** Compute the derivative estimate based on the given bandwidth */
  Scalar computePhi(const Scalar h) const
  {
    // Quick return for odd order
    if (order_ % 2 == 1) return 0.0;
    Scalar phi = N_ * hermitePolynomial_(0.0);
    const Scalar cutOffPlugin = ResourceMap::GetAsScalar( "KernelSmoothing-CutOffPlugin" );
    for (UnsignedInteger i = 1; i < N_; ++i)
    {
      for (UnsignedInteger j = 0; j < i; ++j)
      {
        const Scalar dx = sample_(i, 0) - sample_(j, 0);
        const Scalar x = dx / h;
        // Clipping: if x is large enough, the exponential factor is 0.0
        if (std::abs(x) < cutOffPlugin) phi += 2.0 * hermitePolynomial_(x) * std::exp(-0.5 * x * x);
      }
    }
    const Scalar res = phi / ((N_ * (N_ - 1.0)) * std::pow(h, order_ + 1.0) * std::sqrt(2.0 * M_PI));
    return res;
  }

  /** Compute the constraint for the plugin bandwidth */
  Point computeBandwidthConstraint(const Point & x) const
  {
    const Scalar h = x[0];
    const Scalar gammaH = K_ * std::pow(h, 5.0 / 7.0);
    const Scalar phiGammaH = computePhi(gammaH);
    const Scalar res = h - std::pow(2.0 * std::sqrt(M_PI) * phiGammaH * N_, -1.0 / 5.0);
    return Point(1, res);
  }

  const Sample & sample_;
  UnsignedInteger N_;
  Scalar K_;
  UnsignedInteger order_;
  UniVariatePolynomial hermitePolynomial_;
};

/* Compute the bandwidth according to the plugin rule
   See Vikas Chandrakant Raykar, Ramani Duraiswami, "Very Fast optimal bandwidth selection for univariate kernel density estimation" CS-TR-4774
   We implement only the basic estimator, not the fast version of it.
*/
Point KernelSmoothing::computePluginBandwidth(const Sample & sample) const
{
  const UnsignedInteger dimension = sample.getDimension();
  if (dimension != 1) throw InvalidArgumentException(HERE) << "Error: plugin bandwidth is available only for 1D sample";
  const UnsignedInteger size = sample.getSize();
  // Approximate the derivatives by smoothing under the Normal assumption
  const Scalar sd = sample.computeStandardDeviationPerComponent()[0];
  const Scalar phi6Normal = -15.0 / (16.0 * std::sqrt(M_PI)) * std::pow(sd, -7.0);
  const Scalar phi8Normal = 105.0 / (32.0 * std::sqrt(M_PI)) * std::pow(sd, -9.0);
  const Scalar g1 = std::pow(-6.0 / (std::sqrt(2.0 * M_PI) * phi6Normal * size), 1.0 / 7.0);
  const Scalar g2 = std::pow(30.0 / (std::sqrt(2.0 * M_PI) * phi8Normal * size), 1.0 / 9.0);
  const Scalar phi4 = PluginConstraint(sample, 1.0, 4).computePhi(g1);
  const Scalar phi6 = PluginConstraint(sample, 1.0, 6).computePhi(g2);
  const Scalar K = std::pow(-6.0 * std::sqrt(2.0) * phi4 / phi6, 1.0 / 7.0);
  PluginConstraint constraint(sample, K, 4);
  const Function f(bindMethod<PluginConstraint, Point, Point>(constraint, &PluginConstraint::computeBandwidthConstraint, 1, 1));
  // Find a bracketing interval
  Scalar a = g1;
  Scalar b = g2;
  Scalar fA = f(Point(1, a))[0];
  Scalar fB = f(Point(1, b))[0];
  // While f has the same sign at the two bounds, update the interval
  while ((fA * fB > 0.0))
  {
    a = 0.5 * a;
    fA = f(Point(1, a))[0];
    if (fA * fB <= 0.0) break;
    b = 2.0 * b;
    fB = f(Point(1, b))[0];
  }
  // Solve loosely the constraint equation
  Brent solver(ResourceMap::GetAsScalar( "KernelSmoothing-AbsolutePrecision" ), ResourceMap::GetAsScalar( "KernelSmoothing-RelativePrecision" ), ResourceMap::GetAsScalar( "KernelSmoothing-ResidualPrecision" ), ResourceMap::GetAsUnsignedInteger( "KernelSmoothing-MaximumIteration" ));
  return Point(1, solver.solve(f, 0.0, a, b) / kernel_.getStandardDeviation()[0]);
}

/* Compute the bandwidth according to a mixed rule:
 * simply use the plugin rule for small sample, and
 * estimate the ratio between the plugin rule and
 * the Silverman rule on a small sample, then
 * scale the Silverman bandwidth computed on the full
 * sample with this ratio
 */
Point KernelSmoothing::computeMixedBandwidth(const Sample & sample) const
{
  const UnsignedInteger dimension = sample.getDimension();
  if (dimension != 1) throw InvalidArgumentException(HERE) << "Error: mixed bandwidth is available only for 1D sample";
  const UnsignedInteger size = sample.getSize();
  // Small sample, just return the plugin bandwidth
  if (size <= ResourceMap::GetAsUnsignedInteger( "KernelSmoothing-SmallSize" )) return computePluginBandwidth(sample);
  Sample smallSample(ResourceMap::GetAsUnsignedInteger( "KernelSmoothing-SmallSize" ), 1);
  for (UnsignedInteger i = 0; i < ResourceMap::GetAsUnsignedInteger( "KernelSmoothing-SmallSize" ); ++i) smallSample(i, 0) = sample(i, 0);
  const Scalar h1 = computePluginBandwidth(smallSample)[0];
  const Scalar h2 = computeSilvermanBandwidth(smallSample)[0];
  return computeSilvermanBandwidth(sample) * (h1 / h2);
}

/* Build a Normal kernel mixture based on the given sample. If no bandwith has already been set, Silverman's rule is used */
Distribution KernelSmoothing::build(const Sample & sample) const
{
  // For 1D sample, use the rule that give the best tradeoff between speed and precision
  if (sample.getDimension() == 1) return build(sample, computeMixedBandwidth(sample));
  // For nD sample, use the only available rule
  return build(sample, computeSilvermanBandwidth(sample));
}

/* Build a Normal kernel mixture based on the given sample and bandwidth
 * For multi-dimentional data, no binning and no boundary correction
 * If boundary correction: mirroring on the sides where needed, followed by truncation
 * If binning: condensation on a regular grid
 */
Distribution KernelSmoothing::build(const Sample & sample,
                                    const Point & bandwidth) const
{
  const UnsignedInteger dimension = sample.getDimension();
  if (bandwidth.getDimension() != dimension) throw InvalidDimensionException(HERE) << "Error: the given bandwidth must have the same dimension as the given sample, here bandwidth dimension=" << bandwidth.getDimension() << " and sample dimension=" << dimension;
  const Point xmin(sample.getMin());
  const Point xmax(sample.getMax());
  // Check the degenerate case of constant sample
  if (xmin == xmax)
  {
    setBandwidth(bandwidth);
    KernelSmoothing::Implementation result(new Dirac(xmin));
    result->setDescription(sample.getDescription());
    return result;
  }
  // Check if we have to perform boundary correction
  // In this case, call buildAsTruncatedDistribution(). It will take
  // care of the other sub-cases
  if (boundingOption_ != NONE) return buildAsTruncatedDistribution(sample, bandwidth);
  // Here we know that no boundary correction is needed
  // Check if we have to bin the data
  UnsignedInteger size = sample.getSize();
  const Bool mustBin = bined_ && (dimension * std::log(1.0 * binNumber_) < std::log(1.0 * size));
  if (bined_ != mustBin) LOGINFO("Will not bin the data because the bin number is greater than the sample size");
  // The usual case: no boundary correction, no binning
  if ((dimension > 2) || (!mustBin)) return buildAsKernelMixture(sample, bandwidth);
  // Only binning
  return buildAsMixture(sample, bandwidth);
}

KernelMixture KernelSmoothing::buildAsKernelMixture(const Sample & sample,
    const Point & bandwidth) const
{
  const UnsignedInteger dimension = sample.getDimension();
  if (bandwidth.getDimension() != dimension) throw InvalidDimensionException(HERE) << "Error: the given bandwidth must have the same dimension as the given sample, here bandwidth dimension=" << bandwidth.getDimension() << " and sample dimension=" << dimension;
  setBandwidth(bandwidth);
  KernelMixture result(kernel_, bandwidth, sample);
  result.setDescription(sample.getDescription());
  return result;
}

Mixture KernelSmoothing::buildAsMixture(const Sample & sample,
                                        const Point & bandwidth) const
{
  const UnsignedInteger dimension = sample.getDimension();
  if (bandwidth.getDimension() != dimension) throw InvalidDimensionException(HERE) << "Error: the given bandwidth must have the same dimension as the given sample, here bandwidth dimension=" << bandwidth.getDimension() << " and sample dimension=" << dimension;
  if (dimension > 2) throw InternalException(HERE) << "Error: binning is not available for dimension > 2, here dimension=" << dimension;
  setBandwidth(bandwidth);
  const UnsignedInteger size = sample.getSize();
  // 2D binning?
  if (dimension == 2)
  {
    const Point xMin(sample.getMin());
    const Point xMax(sample.getMax());
    Point weights((binNumber_ + 1) * (binNumber_ + 1));
    Point gridX(binNumber_ + 1);
    Point gridY(binNumber_ + 1);
    const Scalar deltaX = (xMax[0] - xMin[0]) / binNumber_;
    const Scalar deltaY = (xMax[1] - xMin[1]) / binNumber_;
    const Scalar factor = 1.0 + SpecFunc::Precision;
    for (UnsignedInteger i = 0; i <= binNumber_; ++i)
    {
      gridX[i] = xMin[0] + i * deltaX;
      gridY[i] = xMin[1] + i * deltaY;
    }
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Scalar x = sample(i, 0);
      const UnsignedInteger indexX = static_cast< UnsignedInteger > (trunc(factor * (x - xMin[0]) / deltaX));
      const Scalar y = sample(i, 1);
      const UnsignedInteger indexY = static_cast< UnsignedInteger > (trunc(factor * (y - xMin[1]) / deltaY));
      const Scalar wRight  = (x - gridX[indexX]) / deltaX;
      const Scalar wLeft   = 1.0 - wRight;
      const Scalar wTop    = (y - gridY[indexY]) / deltaY;
      const Scalar wBottom = 1.0 - wTop;
      if ((indexX > 0) && (indexX < binNumber_))
      {
        // Fully inside of a patch
        if ((indexY > 0) && (indexY < binNumber_))
        {
          weights[indexX     + indexY * (binNumber_ + 1)]       += wLeft  * wBottom;
          weights[indexX + 1 + indexY * (binNumber_ + 1)]       += wRight * wBottom;
          weights[indexX     + (indexY + 1) * (binNumber_ + 1)] += wLeft  * wTop;
          weights[indexX + 1 + (indexY + 1) * (binNumber_ + 1)] += wRight * wTop;
        } // Fully inside of a patch
        else
        {
          weights[indexX     + indexY * (binNumber_ + 1)] += wLeft;
          weights[indexX + 1 + indexY * (binNumber_ + 1)] += wRight;
        } // On an y boundary
      } // Fully inside of the columns
      else
      {
        // Fully inside of y
        if ((indexY > 0) && (indexY < binNumber_))
        {
          weights[indexX     + indexY * (binNumber_ + 1)]       += wBottom;
          weights[indexX     + (indexY + 1) * (binNumber_ + 1)] += wTop;
        } // Fully inside of a patch
        else
        {
          weights[indexX     + indexY * (binNumber_ + 1)] += 1.0;
        } // On a corner
      } // On an x-boundary
    }
    Collection< Distribution > atoms((binNumber_ + 1) * (binNumber_ + 1));
    for (UnsignedInteger j = 0; j <= binNumber_; ++j)
    {
      Point point(2);
      point[1] = gridY[j];
      for (UnsignedInteger i = 0; i <= binNumber_; ++i)
      {
        point[0] = gridX[i];
        atoms[i + j * (binNumber_ + 1)] = KernelMixture(kernel_, bandwidth, Sample(1, point));
      } // i
    } // j
    Mixture result(atoms, weights);
    result.setDescription(sample.getDescription());
    return result;
  } // 2D binning
  // 1D binning
  Scalar xMin = sample.getMin()[0];
  Scalar xMax = sample.getMax()[0];

  Point weights(binNumber_ + 1);
  Point grid(binNumber_ + 1);
  const Scalar delta = (xMax - xMin) / binNumber_;
  const Scalar factor = 1.0 + SpecFunc::Precision;
  for (UnsignedInteger i = 0; i <= binNumber_; ++i) grid[i] = xMin + i * delta;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar x = sample(i, 0);
    // x will be located between grid[index] and grid[index+1] if 0<index<binNumber
    // if index=0 then x=xMin and if index=binNumber then x=xMax
    // Here we increase a little bit the slice number to insure that the max value will have an index equal to binNumber
    const UnsignedInteger index = static_cast< UnsignedInteger > (trunc(factor * (x - xMin) / delta));
    // Split the point contribution between the two endpoints of the bin containing
    // the point using a linear split
    if ((index > 0) && (index < binNumber_))
    {
      weights[index]     += (grid[index + 1] - x) / delta;
      weights[index + 1] += (x - grid[index])     / delta;
    }
    // The full weight is given to the end points
    else weights[index] += 1.0;
  }
  Collection< Distribution > atoms(binNumber_ + 1);
  for (UnsignedInteger i = 0; i <= binNumber_; ++i)
  {
    atoms[i] = KernelMixture(kernel_, bandwidth, Sample(1, Point(1, grid[i])));
  }
  Mixture result(atoms, weights);
  result.setDescription(sample.getDescription());
  return result;
}

TruncatedDistribution KernelSmoothing::buildAsTruncatedDistribution(const Sample & sample,
    const Point & bandwidth) const
{
  const UnsignedInteger dimension = sample.getDimension();
  if (bandwidth.getDimension() != dimension) throw InvalidDimensionException(HERE) << "Error: the given bandwidth must have the same dimension as the given sample, here bandwidth dimension=" << bandwidth.getDimension() << " and sample dimension=" << dimension;
  if (dimension > 1) throw InternalException(HERE) << "Error: cannot make boundary correction on samples with dimension>1, here dimension=" << dimension;
  setBandwidth(bandwidth);
  Scalar xMin = 0.0;
  Scalar xMax = 0.0;
  if ((boundingOption_ == LOWER) || (boundingOption_ == BOTH))
    {
      xMin = sample.getMin()[0];
      if (!automaticLowerBound_)
	{
	  // Check the sample against the user-defined bounds
	  if (!(lowerBound_ <= xMin)) throw InvalidArgumentException(HERE) << "Error: expected a sample with a minimum value at least equal to lowerBound=" << lowerBound_ << ", got xMin=" << xMin;
	  xMin = lowerBound_;
	} // !automaticLowerBound
    } // Boundary correction on the lower bound
  if ((boundingOption_ == UPPER) || (boundingOption_ == BOTH))
    {
      xMax = sample.getMax()[0];
      if (!automaticUpperBound_)
	{
	  // Check the sample against the user-defined bounds
	  if (!(upperBound_ >= xMax)) throw InvalidArgumentException(HERE) << "Error: expected a sample with a maximum value at most equal to upperBound=" << upperBound_ << ", got xMax=" << xMax;
	  xMax = upperBound_;
	} // !automaticUpperBound
    } // Boundary correction on the upper bound
  // Here we are in the 1D case with boundary boundary correction
  Point newSampleData(sample.asPoint());
  if (xMin == xMax) throw InvalidArgumentException(HERE) << "Error: cannot make boundary correction on constant samples.";
  const Scalar h = bandwidth[0];
  // Reflect and add points close to the boundaries of the sample. No need to
  // reflect the whole sample as suggested in the literature, only the point
  // in the support of the kernels located at each boundary
  const Scalar lower = kernel_.getRange().getLowerBound()[0];
  const Scalar upper = kernel_.getRange().getUpperBound()[0];
  UnsignedInteger size = sample.getSize();
  const Bool doLower = ((boundingOption_ == LOWER)  || (boundingOption_ == BOTH)) && (xMin > -0.5 * SpecFunc::MaxScalar + h * lower);
  const Bool doUpper = ((boundingOption_ == UPPER) || (boundingOption_ == BOTH)) && (xMax <  0.5 * SpecFunc::MaxScalar + h * upper);
  for (UnsignedInteger i = 0; i < size; i++)
  {
    const Scalar x = newSampleData[i];
    // lower < 0
    if (doLower && (x <= xMin - h * lower)) newSampleData.add(2.0 * xMin - x);
    // upper > 0
    else if (doUpper && (x >= xMax - h * upper)) newSampleData.add(2.0 * xMax - x);
  }
  // Now, work on the extended sample
  SampleImplementation newSample(newSampleData.getSize(), 1);
  newSample.setData(newSampleData);
  size = newSample.getSize();
  const Bool mustBin = bined_ && (dimension * std::log(1.0 * binNumber_) < std::log(1.0 * size));
  if (bined_ != mustBin) LOGINFO("Will not bin the data because the bin number is greater than the sample size");
  Distribution baseDistribution;
  if (mustBin) baseDistribution = buildAsMixture(newSample, bandwidth);
  else baseDistribution = buildAsKernelMixture(newSample, bandwidth);
  if (boundingOption_ == LOWER) return TruncatedDistribution(baseDistribution, xMin, TruncatedDistribution::LOWER);
  if (boundingOption_ == UPPER) return TruncatedDistribution(baseDistribution, xMax, TruncatedDistribution::UPPER);
  return TruncatedDistribution(baseDistribution, xMin, xMax);
}

/* Bandwidth accessor */
void KernelSmoothing::setBandwidth(const Point & bandwidth) const
{
  // Check the given bandwidth
  for (UnsignedInteger i = 0; i < bandwidth.getDimension(); i++) if (!(bandwidth[i] > 0.0)) throw InvalidArgumentException(HERE) << "Error: the bandwidth must be > 0, here bandwith=" << bandwidth;
  bandwidth_ = bandwidth;
}

Point KernelSmoothing::getBandwidth() const
{
  return bandwidth_;
}

/* Kernel accessor */
Distribution KernelSmoothing::getKernel() const
{
  return kernel_;
}

/* Boundary management */
void KernelSmoothing::setBoundaryCorrection(const Bool boundaryCorrection)
{
  boundingOption_ = (boundaryCorrection ? BOTH : NONE);
}

/* Boundary correction accessor */
void KernelSmoothing::setBoundingOption(const BoundingOption boundingOption)
{
  boundingOption_ = boundingOption;
}

/* Boundary accessor */
void KernelSmoothing::setLowerBound(const Scalar lowerBound)
{
  lowerBound_ = lowerBound;
  automaticLowerBound_ = false;
}

void KernelSmoothing::setUpperBound(const Scalar upperBound)
{
  upperBound_ = upperBound;
  automaticUpperBound_ = false;
}

/* Automatic boundary accessor */
void KernelSmoothing::setAutomaticLowerBound(const Bool automaticLowerBound)
{
  automaticLowerBound_ = automaticLowerBound;
}

void KernelSmoothing::setAutomaticUpperBound(const Bool automaticUpperBound)
{
  automaticUpperBound_ = automaticUpperBound;
}


/* Method save() stores the object through the StorageManager */
void KernelSmoothing::save(Advocate & adv) const
{
  DistributionFactoryImplementation::save(adv);
  adv.saveAttribute("bandwidth_", bandwidth_);
  adv.saveAttribute("kernel_", kernel_);
  adv.saveAttribute("bined_", bined_);
  adv.saveAttribute("binNumber_", binNumber_);
  adv.saveAttribute("boundingOption_", static_cast<UnsignedInteger>(boundingOption_));
  adv.saveAttribute("lowerBound_", lowerBound_);
  adv.saveAttribute("automaticLowerBound_", automaticLowerBound_);
  adv.saveAttribute("upperBound_", upperBound_);
  adv.saveAttribute("automaticUpperBound_", automaticUpperBound_);
}

/* Method load() reloads the object from the StorageManager */
void KernelSmoothing::load(Advocate & adv)
{
  DistributionFactoryImplementation::load(adv);
  adv.loadAttribute("bandwidth_", bandwidth_);
  adv.loadAttribute("kernel_", kernel_);
  adv.loadAttribute("bined_", bined_);
  adv.loadAttribute("binNumber_", binNumber_);
  UnsignedInteger boundingOption = 0;
  adv.loadAttribute("boundingOption_", boundingOption);
  boundingOption_ = static_cast<BoundingOption>(boundingOption);
  adv.loadAttribute("lowerBound_", lowerBound_);
  adv.loadAttribute("automaticLowerBound_", automaticLowerBound_);
  adv.loadAttribute("upperBound_", upperBound_);
  adv.loadAttribute("automaticUpperBound_", automaticUpperBound_);
}

END_NAMESPACE_OPENTURNS
