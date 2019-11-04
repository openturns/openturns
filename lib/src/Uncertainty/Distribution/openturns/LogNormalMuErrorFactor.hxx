//                                               -*- C++ -*-
/**
 *  @brief LogNormal distribution with mu and sigma as parameters
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_LOGNORMALMUERRORFACTOR_HXX
#define OPENTURNS_LOGNORMALMUERRORFACTOR_HXX

#include "openturns/DistributionParametersImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LogNormalMuErrorFactor
 *
 */
class OT_API LogNormalMuErrorFactor
  : public DistributionParametersImplementation
{

  CLASSNAME
public:

  /** Default constructor */
  LogNormalMuErrorFactor();

  /** Constructor with parameters */
  LogNormalMuErrorFactor(const Scalar muLog, const Scalar ef, const Scalar gamma = 0.);

  /** Virtual constructor */
  virtual LogNormalMuErrorFactor * clone() const;

  /** Comparison operator */
  Bool operator ==(const LogNormalMuErrorFactor & other) const;

  /** Build a distribution based on a set of native parameters */
  Distribution getDistribution() const;

  /** Compute jacobian / native parameters */
  Matrix gradient() const;

  /** Conversion operator */
  Point operator () (const Point & inP) const;

  Point inverse(const Point & inP) const;

  /** Parameters value and description accessor */
  virtual void setValues(const Point & values);
  virtual Point getValues() const;
  virtual Description getDescription() const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:
  /** The main parameter set of the distribution */
  Scalar muLog_;
  Scalar ef_;
  Scalar gamma_;
}; /* class LogNormalMuErrorFactor */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LOGNORMALMUERRORFACTOR_HXX */
