//                                               -*- C++ -*-
/**
 *  @brief Class for a product-kernel multidimensional mixture.
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
#ifndef OPENTURNS_KERNELMIXTURE_HXX
#define OPENTURNS_KERNELMIXTURE_HXX

#include "openturns/Distribution.hxx"
#include "openturns/ContinuousDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KernelMixture
 *
 * The class describes the probabilistic concept of KernelMixture.
 */
class OT_API KernelMixture
  : public ContinuousDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  KernelMixture();

  /** Parameters constructor */
  KernelMixture(const Distribution & kernel,
                const Point & bandwidth,
                const Sample & sample);


  /** Comparison operator */
  using ContinuousDistribution::operator ==;
  Bool operator ==(const KernelMixture & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;


  /** Kernel accessor */
  void setKernel(const Distribution & kernel);
  Distribution getKernel() const;

  /** Bandwidth accessor */
  void setBandwidth(const Point & bandwidth);
  Point getBandwidth() const;

  /** Sample accessor */
  void setInternalSample(const Sample & sample);
  Sample getInternalSample() const;


  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  KernelMixture * clone() const override;

  /** Get one realization of the KernelMixture */
  Point getRealization() const override;

  /** Get the DDF of the KernelMixture */
  using ContinuousDistribution::computeDDF;
  Point computeDDF(const Point & point) const override;

  /** Get the PDF of the KernelMixture */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the KernelMixture */
  using ContinuousDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const override;
  using ContinuousDistribution::computeComplementaryCDF;
  Scalar computeComplementaryCDF(const Point & point) const override;

  /** Compute the survival function */
  using ContinuousDistribution::computeSurvivalFunction;
  Scalar computeSurvivalFunction(const Point & point) const override;

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const override;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const override;

  /** Get the PDF gradient of the distribution */
  using ContinuousDistribution::computePDFGradient;
  Point computePDFGradient(const Point & point) const override;

  /** Get the CDF gradient of the distribution */
  using ContinuousDistribution::computeCDFGradient;
  Point computeCDFGradient(const Point & point) const override;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using ContinuousDistribution::computeConditionalPDF;
  Scalar computeConditionalPDF(const Scalar x, const Point & y) const override;
  Point computeSequentialConditionalPDF(const Point & x) const override;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using ContinuousDistribution::computeConditionalCDF;
  Scalar computeConditionalCDF(const Scalar x, const Point & y) const override;
  Point computeSequentialConditionalCDF(const Point & x) const override;

  /** Get the i-th marginal distribution */
  Distribution getMarginal(const UnsignedInteger i) const override;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Distribution getMarginal(const Indices & indices) const override;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const override;

  /** Get the skewness of the distribution */
  Point getSkewness() const override;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const override;

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const override;

  /** Check if the distribution is constinuous */
  Bool isContinuous() const override;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const override;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:

  /** Get the quantile of the distribution, i.e the value Xp such that P(X <= Xp) = prob */
  Scalar computeScalarQuantile(const Scalar prob, const Bool tail = false) const override;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** Get the mean of a kernelMixture */
  void computeMean() const override;

  /** Get the covariance of a kernelMixture */
  void computeCovariance() const override;

  /** Compute the normalization factor */
  void computeNormalizationFactor();

  /** The kernel of the kernelMixture */
  Pointer<DistributionImplementation> p_kernel_;

  /** The bandwidth of the KernelMixture */
  Point bandwidth_;

  /** The componentwise bandwidth inverse of the KernelMixture */
  Point bandwidthInverse_;

  /** The (n\product_{k=1}^{dim}h_k)^{-1} normalization factor */
  Scalar normalizationFactor_;

  /** The sample of the kernelMixture */
  Sample sample_;

  /** PDF approximation associated to the CDF approximation */
  PiecewiseHermiteEvaluation pdfApproximationCDF_;

  /** CDF approximation */
  PiecewiseHermiteEvaluation cdfApproximation_;

  /** PDF approximation associated to the CCDF approximation */
  PiecewiseHermiteEvaluation pdfApproximationCCDF_;

  /** Complementary CDF approximation */
  PiecewiseHermiteEvaluation ccdfApproximation_;

  /** Do I have an approximation for the CDF? */
  Bool useApproximatePDFCDF_;

}; /* class KernelMixture */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KERNELMIXTURE_HXX */
