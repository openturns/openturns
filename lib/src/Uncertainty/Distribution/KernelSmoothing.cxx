//                                               -*- C++ -*-
/**
 *  @brief This class acts like a KernelMixture factory, implementing a
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
#include "openturns/KernelSmoothing.hxx"
#include "openturns/Normal.hxx"
#include "openturns/Dirac.hxx"
#include "openturns/KernelMixture.hxx"
#include "openturns/Mixture.hxx"
#include "openturns/TruncatedDistribution.hxx"
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

CLASSNAMEINIT(KernelSmoothing);

static const Factory<KernelSmoothing> Factory_KernelSmoothing;

/* Default constructor */
KernelSmoothing::KernelSmoothing()
  : DistributionFactoryImplementation()
  , bandwidth_(Point(0))
  , kernel_(Normal())
  , bined_(true)
  , binNumber_(ResourceMap::GetAsUnsignedInteger( "KernelSmoothing-BinNumber" ))
  , boundaryCorrection_(false)
{
  setName("KernelSmoothing");
  if (binNumber_ < 2) throw InvalidArgumentException(HERE) << "Error: The default number of bins=" << binNumber_ << " is less than 2. Check the ResourceMap or the openturns.conf file.";
}

/* Default constructor */
KernelSmoothing::KernelSmoothing(const Distribution & kernel,
                                 const Bool bined,
                                 const UnsignedInteger binNumber,
                                 const Bool boundaryCorrection)
  : DistributionFactoryImplementation()
  , bandwidth_(Point(0))
  , kernel_(kernel)
  , bined_(bined)
  , binNumber_(binNumber)
  , boundaryCorrection_(boundaryCorrection)
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
  NumericalScalar factor = std::pow(size, -1.0 / (4.0 + dimension)) / kernel_.getStandardDeviation()[0];
  // Scott's Normal rule
  return factor * scale;
}

struct PluginConstraint
{
  /** Constructor from a sample and a derivative factor estimate */
  PluginConstraint(const Sample & sample,
                   const NumericalScalar K,
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
  NumericalScalar computePhi(const NumericalScalar h) const
  {
    // Quick return for odd order
    if (order_ % 2 == 1) return 0.0;
    NumericalScalar phi = N_ * hermitePolynomial_(0.0);
    const NumericalScalar cutOffPlugin = ResourceMap::GetAsNumericalScalar( "KernelSmoothing-CutOffPlugin" );
    for (UnsignedInteger i = 1; i < N_; ++i)
    {
      for (UnsignedInteger j = 0; j < i; ++j)
      {
        const NumericalScalar dx = sample_[i][0] - sample_[j][0];
        const NumericalScalar x = dx / h;
        // Clipping: if x is large enough, the exponential factor is 0.0
        if (std::abs(x) < cutOffPlugin) phi += 2.0 * hermitePolynomial_(x) * std::exp(-0.5 * x * x);
      }
    }
    const NumericalScalar res = phi / ((N_ * (N_ - 1.0)) * std::pow(h, order_ + 1.0) * std::sqrt(2.0 * M_PI));
    return res;
  }

  /** Compute the constraint for the plugin bandwidth */
  Point computeBandwidthConstraint(const Point & x) const
  {
    const NumericalScalar h = x[0];
    const NumericalScalar gammaH = K_ * std::pow(h, 5.0 / 7.0);
    const NumericalScalar phiGammaH = computePhi(gammaH);
    const NumericalScalar res = h - std::pow(2.0 * std::sqrt(M_PI) * phiGammaH * N_, -1.0 / 5.0);
    return Point(1, res);
  }

  const Sample & sample_;
  UnsignedInteger N_;
  NumericalScalar K_;
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
  const NumericalScalar sd = sample.computeStandardDeviationPerComponent()[0];
  const NumericalScalar phi6Normal = -15.0 / (16.0 * std::sqrt(M_PI)) * std::pow(sd, -7.0);
  const NumericalScalar phi8Normal = 105.0 / (32.0 * std::sqrt(M_PI)) * std::pow(sd, -9.0);
  const NumericalScalar g1 = std::pow(-6.0 / (std::sqrt(2.0 * M_PI) * phi6Normal * size), 1.0 / 7.0);
  const NumericalScalar g2 = std::pow(30.0 / (std::sqrt(2.0 * M_PI) * phi8Normal * size), 1.0 / 9.0);
  const NumericalScalar phi4 = PluginConstraint(sample, 1.0, 4).computePhi(g1);
  const NumericalScalar phi6 = PluginConstraint(sample, 1.0, 6).computePhi(g2);
  const NumericalScalar K = std::pow(-6.0 * std::sqrt(2.0) * phi4 / phi6, 1.0 / 7.0);
  PluginConstraint constraint(sample, K, 4);
  const Function f(bindMethod<PluginConstraint, Point, Point>(constraint, &PluginConstraint::computeBandwidthConstraint, 1, 1));
  // Find a bracketing interval
  NumericalScalar a = g1;
  NumericalScalar b = g2;
  NumericalScalar fA = f(Point(1, a))[0];
  NumericalScalar fB = f(Point(1, b))[0];
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
  Brent solver(ResourceMap::GetAsNumericalScalar( "KernelSmoothing-AbsolutePrecision" ), ResourceMap::GetAsNumericalScalar( "KernelSmoothing-RelativePrecision" ), ResourceMap::GetAsNumericalScalar( "KernelSmoothing-ResidualPrecision" ), ResourceMap::GetAsUnsignedInteger( "KernelSmoothing-MaximumIteration" ));
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
  for (UnsignedInteger i = 0; i < ResourceMap::GetAsUnsignedInteger( "KernelSmoothing-SmallSize" ); ++i) smallSample[i][0] = sample[i][0];
  const NumericalScalar h1 = computePluginBandwidth(smallSample)[0];
  const NumericalScalar h2 = computeSilvermanBandwidth(smallSample)[0];
  return computeSilvermanBandwidth(sample) * (h1 / h2);
}

/* Build a Normal kernel mixture based on the given sample. If no bandwith has already been set, Silverman's rule is used */
KernelSmoothing::Implementation KernelSmoothing::build(const Sample & sample) const
{
  // For 1D sample, use the rule that give the best tradeoff between speed and precision
  if (sample.getDimension() == 1) return build(sample, computeMixedBandwidth(sample));
  // For nD sample, use the only available rule
  return build(sample, computeSilvermanBandwidth(sample));
}

/* Build a Normal kernel mixture based on the given sample and bandwidth
 * For multi-dimentional data, no binning and no boundary correction
 * If boundary correction: mirroring on the two sides, followed by truncation
 * If binning: condensation on a regular grid
 */
KernelSmoothing::Implementation KernelSmoothing::build(const Sample & sample,
    const Point & bandwidth) const
{
  const UnsignedInteger dimension = sample.getDimension();
  if (bandwidth.getDimension() != dimension) throw InvalidDimensionException(HERE) << "Error: the given bandwidth must have the same dimension as the given sample, here bandwidth dimension=" << bandwidth.getDimension() << " and sample dimension=" << dimension;
  setBandwidth(bandwidth);
  UnsignedInteger size = sample.getSize();
  // The usual case: no boundary correction, no binning
  const Bool mustBin = bined_ && (dimension * std::log(1.0 * binNumber_) < std::log(1.0 * size));
  if (bined_ != mustBin) LOGINFO("Will not bin the data because the bin number is greater than the sample size");
  if ((dimension > 2) || ((!mustBin) && (!boundaryCorrection_)))
  {
    KernelMixture result(kernel_, bandwidth, sample);
    result.setDescription(sample.getDescription());
    return result.clone();
  }
  const Point xmin(sample.getMin());
  const Point xmax(sample.getMax());
  if (xmin == xmax)
  {
    Dirac result(xmin);
    result.setDescription(sample.getDescription());
    return result.clone();
  }
  // 2D binning?
  if ((dimension == 2) && mustBin)
  {
    Point reducedData(binNumber_ * binNumber_);
    Point x(binNumber_);
    Point y(binNumber_);
    const NumericalScalar deltaX = (xmax[0] - xmin[0]) / (binNumber_ - 1);
    const NumericalScalar deltaY = (xmax[1] - xmin[1]) / (binNumber_ - 1);
    const NumericalScalar hX = 0.5 * deltaX;
    const NumericalScalar hY = 0.5 * deltaY;
    for (UnsignedInteger i = 0; i < binNumber_; ++i)
    {
      x[i] = xmin[0] + i * deltaX;
      y[i] = xmin[1] + i * deltaY;
    }
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      UnsignedInteger indexX = 0;
      NumericalScalar sliceX = (sample[i][0] - (xmin[0] - hX)) / deltaX;
      if (sliceX >= 0.0) indexX = static_cast< UnsignedInteger > (trunc(sliceX));
      if (indexX >= binNumber_) indexX = binNumber_ - 1;
      UnsignedInteger indexY = 0;
      NumericalScalar sliceY = (sample[i][1] - (xmin[1] - hY)) / deltaY;
      if (sliceY >= 0.0) indexY = static_cast< UnsignedInteger > (trunc(sliceY));
      if (indexY >= binNumber_) indexY = binNumber_ - 1;
      ++reducedData[indexX + indexY * binNumber_];
    }
    Collection< Distribution > atoms(binNumber_ * binNumber_);
    for (UnsignedInteger i = 0; i < binNumber_; ++i)
    {
      Point point(2);
      point[0] = x[i];
      for (UnsignedInteger j = 0; j < binNumber_; ++j)
      {
        point[1] = y[j];
        KernelMixture atom(kernel_, bandwidth, Sample(1, point));
        atoms[i + j * binNumber_] = atom;
      }
    }
    Mixture result(atoms, reducedData);
    result.setDescription(sample.getDescription());
    return result.clone();
  } // 2D binning

  // Here we are in the 1D case, with at least binning or boundary boundary correction
  Sample newSample(sample);
  NumericalScalar xminNew = xmin[0];
  NumericalScalar xmaxNew = xmax[0];
  // If boundary correction,
  if (boundaryCorrection_)
  {
    NumericalScalar h = bandwidth[0];
    // Reflect and add points close to the boundaries to the sample
    for (UnsignedInteger i = 0; i < size; i++)
    {
      const Point realization(sample[i]);
      if (realization[0] <= xmin[0] + h) newSample.add(2.0 * xmin - realization);
      if (realization[0] >= xmax[0] - h) newSample.add(2.0 * xmax - realization);
    }
  }
  // Now, work on the extended sample
  if (!mustBin)
  {
    TruncatedDistribution result(KernelMixture(kernel_, bandwidth, newSample), xmin[0], xmax[0]);
    result.setDescription(sample.getDescription());
    return result.clone();
  }
  if (boundaryCorrection_)
  {
    xminNew = newSample.getMin()[0];
    xmaxNew = newSample.getMax()[0];
  }
  size = newSample.getSize();
  // Here, we have to bin the data
  Point reducedData(binNumber_);
  Point x(binNumber_);
  const NumericalScalar delta = (xmaxNew - xminNew) / (binNumber_ - 1);
  const NumericalScalar h = 0.5 * delta;
  for (UnsignedInteger i = 0; i < binNumber_; ++i) x[i] = xminNew + i * delta;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    UnsignedInteger index = 0;
    NumericalScalar slice = (newSample[i][0] - (xminNew - h)) / delta;
    if (slice >= 0.0) index = static_cast< UnsignedInteger > (trunc(slice));
    if (index >= binNumber_) index = binNumber_ - 1;
    ++reducedData[index];
  }
  Collection< Distribution > atoms(binNumber_);
  for (UnsignedInteger i = 0; i < binNumber_; ++i)
  {
    KernelMixture atom(kernel_, bandwidth, Sample(1, Point(1, x[i])));
    atoms[i] = atom;
  }
  if (boundaryCorrection_)
  {
    TruncatedDistribution result(Mixture(atoms, reducedData), xmin[0], xmax[0]);
    result.setDescription(sample.getDescription());
    return result.clone();
  }
  Mixture result(atoms, reducedData);
  result.setDescription(sample.getDescription());
  return result.clone();
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

Distribution KernelSmoothing::getKernel() const
{
  return kernel_;
}

void KernelSmoothing::setBoundaryCorrection(const Bool boundaryCorrection)
{
  boundaryCorrection_ = boundaryCorrection;
}

/* Method save() stores the object through the StorageManager */
void KernelSmoothing::save(Advocate & adv) const
{
  DistributionFactoryImplementation::save(adv);
  adv.saveAttribute("bandwidth_", bandwidth_);
  adv.saveAttribute("kernel_", kernel_);
  adv.saveAttribute("bined_", bined_);
  adv.saveAttribute("binNumber_", binNumber_);
  adv.saveAttribute("boundaryCorrection_", boundaryCorrection_);
}

/* Method load() reloads the object from the StorageManager */
void KernelSmoothing::load(Advocate & adv)
{
  DistributionFactoryImplementation::load(adv);
  adv.loadAttribute("bandwidth_", bandwidth_);
  adv.loadAttribute("kernel_", kernel_);
  adv.loadAttribute("bined_", bined_);
  adv.loadAttribute("binNumber_", binNumber_);
  adv.loadAttribute("boundaryCorrection_", boundaryCorrection_);
}

END_NAMESPACE_OPENTURNS
