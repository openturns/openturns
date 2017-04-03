//                                               -*- C++ -*-
/**
 *  @brief This class acts like a (possibly truncated) KernelMixture factory, implementing density estimation
 *         using the kernel smoothing method. It uses Silverman's rule for product kernel.
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
#ifndef OPENTURNS_KERNELSMOOTHING_HXX
#define OPENTURNS_KERNELSMOOTHING_HXX

#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KernelSmoothing
 *
 * The class describes the probabilistic concept of KernelSmoothing.
 */
class OT_API KernelSmoothing
  : public DistributionFactoryImplementation
{
  CLASSNAME;
public:

  /** Default constructor */
  KernelSmoothing();

  /** Default constructor */
  explicit KernelSmoothing(const Distribution & kernel,
                           const Bool bined = true,
                           const UnsignedInteger binNumber = ResourceMap::GetAsUnsignedInteger("KernelSmoothing-BinNumber"),
                           const Bool boundaryCorrection = false);

  /** Virtual constructor */
  virtual KernelSmoothing * clone() const;

  /** Build a Normal kernel mixture based on the given sample. If no bandwith has already been set, Silverman's rule is used */
  using DistributionFactoryImplementation::build;
  virtual Implementation build(const Sample & sample) const;

  /** Build a (possibly truncated) kernel mixture based on the given sample and bandwidth */
  virtual Implementation build(const Sample & sample,
                               const Point & bandwidth) const;

  /** Bandwidth accessor */
  Point getBandwidth() const;

  /** Kernel accessor */
  Distribution getKernel() const;

  void setBoundaryCorrection(const Bool boundaryCorrection);

  /** Compute the bandwidth according to Silverman's rule */
  Point computeSilvermanBandwidth(const Sample & sample) const;

  /** Compute the bandwidth according to the plugin rule. Warning!
   * it can take a lot of time for large samples, as the cost is
   * quadratic with the sample size
   */
  Point computePluginBandwidth(const Sample & sample) const;

  /** Compute the bandwidth according to a mixed rule:
   * simply use the plugin rule for small sample, and
   * estimate the ratio between the plugin rule and
   * the Silverman rule on a small sample, then
   * scale the Silverman bandwidth computed on the full
   * sample with this ratio
   */
  Point computeMixedBandwidth(const Sample & sample) const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  void setBandwidth(const Point & bandwidth) const;

  // Bandwith of the smoothing
  mutable Point bandwidth_;

  // 1D kernel for kernel product
  Distribution kernel_;

  // Flag to tell if we compute a bined version of the estimator
  Bool bined_;

  // Number of bins
  UnsignedInteger binNumber_;

  Bool boundaryCorrection_;

}; /* class KernelSmoothing */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KERNELSMOOTHING_HXX */
