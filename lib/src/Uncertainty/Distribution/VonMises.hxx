//                                               -*- C++ -*-
/**
 *  @brief The VonMises distribution
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
#ifndef OPENTURNS_VONMISES_HXX
#define OPENTURNS_VONMISES_HXX

#include "ContinuousDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class VonMises
 *
 * The VonMises distribution.
 */
class OT_API VonMises
  : public ContinuousDistribution
{
  CLASSNAME;
public:

  /** Default constructor */
  VonMises();

  /** Parameters constructor */
  explicit VonMises(const NumericalScalar mu,
                    const NumericalScalar kappa);


  /** Comparison operator */
  Bool operator ==(const VonMises & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;


  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual VonMises * clone() const;

  /** Get one realization of the distribution */
  NumericalPoint getRealization() const;

  /** Get the DDF of the distribution */
  using ContinuousDistribution::computeDDF;
  NumericalPoint computeDDF(const NumericalPoint & point) const;

  /** Get the PDF of the distribution */
  using ContinuousDistribution::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;
  using ContinuousDistribution::computeLogPDF;
  NumericalScalar computeLogPDF(const NumericalPoint & point) const;

  /** Parameters value and description accessor */
  NumericalPointWithDescriptionCollection getParametersCollection() const;
  using ContinuousDistribution::setParametersCollection;
  void setParametersCollection(const NumericalPointCollection & parametersCollection);

  /* Interface specific to VonMises */

  /** Mu accessor */
  void setMu(const NumericalScalar mu);

  NumericalScalar getMu() const;

  /** Kappa accessor */
  void setKappa(const NumericalScalar kappa);

  NumericalScalar getKappa() const;

  /** Circular moments accessor */
  NumericalScalar getCircularMean() const;

  NumericalScalar getCircularVariance() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


protected:


private:

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** Update the derivative attributes */
  void update();

  /** The main parameter set of the distribution */
  NumericalScalar mu_;
  NumericalScalar kappa_;
  NumericalScalar normalizationFactor_;
  NumericalScalar ratioOfUniformsBound_;

}; /* class VonMises */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_VONMISES_HXX */
