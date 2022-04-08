//                                               -*- C++ -*-
/**
 *  @brief The VonMises distribution
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_VONMISES_HXX
#define OPENTURNS_VONMISES_HXX

#include "openturns/ContinuousDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class VonMises
 *
 * The VonMises distribution.
 */
class OT_API VonMises
  : public ContinuousDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  VonMises();

  /** Parameters constructor */
  explicit VonMises(const Scalar mu,
                    const Scalar kappa);


  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const VonMises & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;


  /* Interface inherited from Distribution */

  /** Virtual constructor */
  VonMises * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the DDF of the distribution */
  using ContinuousDistribution::computeDDF;
  Point computeDDF(const Point & point) const override;

  /** Get the PDF of the distribution */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Point & point) const override;
  using ContinuousDistribution::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const override;

  /* Interface specific to VonMises */

  /** Mu accessor */
  void setMu(const Scalar mu);

  Scalar getMu() const;

  /** Kappa accessor */
  void setKappa(const Scalar kappa);

  Scalar getKappa() const;

  /** Circular moments accessor */
  Scalar getCircularMean() const;

  Scalar getCircularVariance() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


protected:


private:

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** Update the derivative attributes */
  void update();

  /** The main parameter set of the distribution */
  Scalar mu_;
  Scalar kappa_;
  Scalar normalizationFactor_;
  Scalar ratioOfUniformsBound_;

}; /* class VonMises */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_VONMISES_HXX */
