//                                               -*- C++ -*-
/**
 *  @brief LogNormal distribution with mu and sigma over mu as parameters
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
#ifndef OPENTURNS_LOGNORMALMUSIGMAOVERMU_HXX
#define OPENTURNS_LOGNORMALMUSIGMAOVERMU_HXX

#include "openturns/DistributionParametersImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LogNormalMuSigmaOverMu
 *
 */
class OT_API LogNormalMuSigmaOverMu
  : public DistributionParametersImplementation
{

  CLASSNAME;
public:

  /** Default constructor */
  LogNormalMuSigmaOverMu();

  /** Constructor with parameters */
  LogNormalMuSigmaOverMu(const NumericalScalar mu, const NumericalScalar sigmaOverMu, const NumericalScalar gamma = 0.);

  /** Virtual constructor */
  virtual LogNormalMuSigmaOverMu * clone() const;

  /** Comparison operator */
  Bool operator ==(const LogNormalMuSigmaOverMu & other) const;

  /** Build a distribution based on a set of native parameters */
  Distribution getDistribution() const;

  /** Compute jacobian / native parameters */
  Matrix gradient() const;

  /** Conversion operator */
  NumericalPoint operator () (const NumericalPoint & inP) const;

  NumericalPoint inverse(const NumericalPoint & inP) const;

  /** Parameters value and description accessor */
  virtual void setValues(const NumericalPoint & values);
  virtual NumericalPoint getValues() const;
  virtual Description getDescription() const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

private:
  /** The main parameter set of the distribution */
  NumericalScalar mu_;
  NumericalScalar sigmaOverMu_;
  NumericalScalar gamma_;
}; /* class LogNormalMuSigmaOverMu */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LOGNORMALMUSIGMAOVERMU_HXX */
