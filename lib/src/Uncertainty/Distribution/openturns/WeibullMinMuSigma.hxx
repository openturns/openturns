//                                               -*- C++ -*-
/**
 *  @brief WeibullMin distribution with mu and sigma as parameters
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_WEIBULLMINMUSIGMA_HXX
#define OPENTURNS_WEIBULLMINMUSIGMA_HXX

#include "openturns/DistributionParametersImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class WeibullMinMuSigma
 *
 */
class OT_API WeibullMinMuSigma
  : public DistributionParametersImplementation
{

  CLASSNAME
public:

  /** Default constructor */
  WeibullMinMuSigma();

  /** Constructor with parameters */
  WeibullMinMuSigma(const Scalar mu, const Scalar sigma, const Scalar gamma = 0.);

  /** Virtual constructor */
  WeibullMinMuSigma * clone() const override;

  /** Comparison operator */
  using DistributionParametersImplementation::operator ==;
  Bool operator ==(const WeibullMinMuSigma & other) const;

  /** Build a distribution based on a set of native parameters */
  Distribution getDistribution() const override;

  /** Compute jacobian / native parameters */
  Matrix gradient() const override;

  /** Conversion operator */
  Point operator () (const Point & inP) const override;

  Point inverse(const Point & inP) const override;

  /** Parameters value and description accessor */
  void setValues(const Point & values) override;
  Point getValues() const override;
  Description getDescription() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  /** The main parameter set of the distribution */
  Scalar mu_;
  Scalar sigma_;
  Scalar gamma_;
}; /* class WeibullMinMuSigma */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_WEIBULLMINMUSIGMA_HXX */
