//                                               -*- C++ -*-
/**
 *  @brief The class that implements kernelMixtures
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
#ifndef OPENTURNS_KERNELMIXTURE_HXX
#define OPENTURNS_KERNELMIXTURE_HXX

#include "Distribution.hxx"
#include "ContinuousDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KernelMixture
 *
 * The class describes the probabilistic concept of KernelMixture.
 */
class OT_API KernelMixture
  : public ContinuousDistribution
{
  CLASSNAME;
public:

  /** Default constructor */
  KernelMixture();

  /** Parameters constructor */
  KernelMixture(const Distribution & kernel,
                const NumericalPoint & bandwidth,
                const NumericalSample & sample);


  /** Comparison operator */
  Bool operator ==(const KernelMixture & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;


  /** Kernel accessor */
  void setKernel(const Distribution & kernel);
  Distribution getKernel() const;

  /** Bandwidth accessor */
  void setBandwidth(const NumericalPoint & bandwidth);
  NumericalPoint getBandwidth() const;

  /** Sample accessor */
  void setInternalSample(const NumericalSample & sample);
  NumericalSample getInternalSample() const;


  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  virtual KernelMixture * clone() const;

  /** Get one realization of the KernelMixture */
  NumericalPoint getRealization() const;

  /** Get the DDF of the KernelMixture */
  using ContinuousDistribution::computeDDF;
  NumericalPoint computeDDF(const NumericalPoint & point) const;

  /** Get the PDF of the KernelMixture */
  using ContinuousDistribution::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;

  /** Get the CDF of the KernelMixture */
  using ContinuousDistribution::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;
  using ContinuousDistribution::computeComplementaryCDF;
  NumericalScalar computeComplementaryCDF(const NumericalPoint & point) const;

  /** Compute the survival function */
  using ContinuousDistribution::computeSurvivalFunction;
  NumericalScalar computeSurvivalFunction(const NumericalPoint & point) const;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  NumericalComplex computeCharacteristicFunction(const NumericalScalar x) const;

  /** Get the PDF gradient of the distribution */
  using ContinuousDistribution::computePDFGradient;
  NumericalPoint computePDFGradient(const NumericalPoint & point) const;

  /** Get the CDF gradient of the distribution */
  using ContinuousDistribution::computeCDFGradient;
  NumericalPoint computeCDFGradient(const NumericalPoint & point) const;

  /** Get the i-th marginal distribution */
  Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Implementation getMarginal(const Indices & indices) const;

  /** Get the standard deviation of the distribution */
  NumericalPoint getStandardDeviation() const;

  /** Get the skewness of the distribution */
  NumericalPoint getSkewness() const;

  /** Get the kurtosis of the distribution */
  NumericalPoint getKurtosis() const;

  /** Parameters value and description accessor */
  NumericalPointWithDescriptionCollection getParametersCollection() const;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const;

  /** Check if the distribution is constinuous */
  Bool isContinuous() const;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:


private:

  /** Get the quantile of the distribution, i.e the value Xp such that P(X <= Xp) = prob */
  NumericalScalar computeScalarQuantile(const NumericalScalar prob,
                                        const Bool tail = false) const;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  friend class Factory<KernelMixture>;

  /** Get the mean of a kernelMixture */
  void computeMean() const;

  /** Get the covariance of a kernelMixture */
  void computeCovariance() const;

  /** Compute the normalization factor */
  void computeNormalizationFactor();

  /** The kernel of the kernelMixture */
  Distribution kernel_;

  /** The bandwidth of the KernelMixture */
  NumericalPoint bandwidth_;

  /** The componentwise bandwidth inverse of the KernelMixture */
  NumericalPoint bandwidthInverse_;

  /** The (n\product_{k=1}^{dim}h_k)^{-1} normalization factor */
  NumericalScalar normalizationFactor_;

  /** The sample of the kernelMixture */
  NumericalSample sample_;

  /** PDF approximation associated to the CDF approximation */
  PiecewiseHermiteEvaluationImplementation pdfApproximationCDF_;

  /** CDF approximation */
  PiecewiseHermiteEvaluationImplementation cdfApproximation_;

  /** PDF approximation associated to the CCDF approximation */
  PiecewiseHermiteEvaluationImplementation pdfApproximationCCDF_;

  /** Complementary CDF approximation */
  PiecewiseHermiteEvaluationImplementation ccdfApproximation_;

  /** Do I have an approximation for the CDF? */
  Bool useApproximatePDFCDF_;

}; /* class KernelMixture */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KERNELMIXTURE_HXX */
