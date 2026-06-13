//                                               -*- C++ -*-
/**
 *  @brief The VonMisesFisher distribution
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
#ifndef OPENTURNS_VONMISESFISHER_HXX
#define OPENTURNS_VONMISESFISHER_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class VonMisesFisher
 *
 * The VonMisesFisher distribution.
 */
class OT_API VonMisesFisher
  : public DistributionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  VonMisesFisher();

  /** Parameters constructor */
  VonMisesFisher(const Point & mu,
                 const Scalar kappa);

  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const VonMisesFisher & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  VonMisesFisher * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get a sample of the distribution */
  Sample getSample(const UnsignedInteger size) const override;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;
  using DistributionImplementation::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  /** Get the CDF of the distribution */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const override;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const override;

  /** Get the skewness of the distribution */
  Point getSkewness() const override;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /* Interface specific to VonMisesFisher */


  /** Mu accessor */
  void setMu(const Point & mu);
  Point getMu() const;

  /** Kappa accessor */
  void setKappa(const Scalar kappa);
  Scalar getKappa() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


protected:


private:

  /** Compute the mean of the distribution */
  void computeMean() const override;

  /** Compute the covariance of the distribution */
  void computeCovariance() const override;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** Update the normalization factor on a log scale */
  void update();

  /** Simultaneous mu/kappa accessor */
  void setMuKappa(const Point & mu, const Scalar kappa);

  /** Intermediate sampling algorithm */
  Point getCosSample(const UnsignedInteger size) const;
  
  /** The main parameter set of the distribution */
  Point mu_;
  Scalar kappa_;

  /** The log-normalization factor */
  Scalar logNormalizationFactor_;

  /** The constants used for the sampling algorithm.
      The names are taken from
      Carlos Pinzón, Kangsoo Jung. Fast Python sampler for the von Mises Fisher distribution. 2023. ⟨hal-04004568v2⟩ */
  Scalar b_;
  Scalar x0_;
  Scalar c_;
}; /* class VonMisesFisher */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_VONMISESFISHER_HXX */
