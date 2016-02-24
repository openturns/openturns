//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all copula
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
#ifndef OPENTURNS_COPULA_HXX
#define OPENTURNS_COPULA_HXX

#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class Copula
 *
 * The class describes the probabilistic concept of distribution.
 * Instances of this class can produce samples following the
 * distribution, can compute PDF or CDF, etc.
 * They are the actual key component of RandomVectors.
 */
class OT_API Copula
  : public Distribution
{
  CLASSNAME;
public:

  /* Default constructor */
  Copula();

  /* Constructor from distribution */
  Copula(const Distribution & distribution);

  /* Constructor from implementation */
  Copula(const DistributionImplementation & implementation);

  /* Constructor from implementation */
  Copula(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  Copula(DistributionImplementation * p_implementation);
#endif

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

}; /* class Copula */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COPULA_HXX */
