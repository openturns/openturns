//                                               -*- C++ -*-
/**
 *  @brief LogNormal distribution with mu and sigma as parameters
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
#ifndef OPENTURNS_LOGNORMALMUSIGMA_HXX
#define OPENTURNS_LOGNORMALMUSIGMA_HXX

#include "DistributionParametersImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LogNormalMuSigma
 *
 */
class OT_API LogNormalMuSigma
  : public DistributionParametersImplementation
{

  CLASSNAME;
public:

  /** Default constructor */
  LogNormalMuSigma();

  /** Constructor with parameters */
  LogNormalMuSigma(const NumericalScalar mu, const NumericalScalar sigma, const NumericalScalar gamma = 0.);

  /** Virtual constructor */
  virtual LogNormalMuSigma * clone() const;

  /** Comparison operator */
  Bool operator ==(const LogNormalMuSigma & other) const;

  /** Build a distribution based on a set of native parameters */
  Distribution getDistribution() const;

  /** Compute jacobian / native parameters */
  Matrix gradient() const;

  /** Conversion operator */
  NumericalPoint operator () (const NumericalPoint & inP) const;

  /** Parameters value and description accessor */
  NumericalPointWithDescriptionCollection getParametersCollection() const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

private:
  /** The main parameter set of the distribution */
  NumericalScalar mu_;
  NumericalScalar sigma_;
  NumericalScalar gamma_;
}; /* class LogNormalMuSigma */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LOGNORMALMUSIGMA_HXX */
