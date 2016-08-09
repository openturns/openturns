//                                               -*- C++ -*-
/**
 *  @brief Beta distribution with mu and sigma as parameters
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_BETAMUSIGMA_HXX
#define OPENTURNS_BETAMUSIGMA_HXX

#include "openturns/DistributionParametersImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BetaMuSigma
 *
 */
class OT_API BetaMuSigma
  : public DistributionParametersImplementation
{

  CLASSNAME;
public:

  /** Default constructor */
  BetaMuSigma();

  /** Constructor with parameters */
  BetaMuSigma(const NumericalScalar mu, const NumericalScalar sigma, const NumericalScalar a = 0., const NumericalScalar b = 1.);

  /** Virtual constructor */
  virtual BetaMuSigma * clone() const;

  /** Comparison operator */
  Bool operator ==(const BetaMuSigma & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** Build a distribution based on a set of native parameters */
  Distribution getDistribution() const;

  /** Compute jacobian / native parameters */
  Matrix gradient() const;

  /** Conversion operator */
  NumericalPoint operator () (const NumericalPoint & inP) const;

  NumericalPoint inverse(const NumericalPoint & inP) const;

  /** Parameters value and description accessor */
  virtual NumericalPoint getValues() const;
  virtual Description getDescription() const;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

private:
  /** The main parameter set of the distribution */
  NumericalScalar mu_;
  NumericalScalar sigma_;
  NumericalScalar a_;
  NumericalScalar b_;
}; /* class BetaMuSigma */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BETAMUSIGMA_HXX */
