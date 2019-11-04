//                                               -*- C++ -*-
/**
 *  @brief Factorial experiment plane
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
#ifndef OPENTURNS_FACTORIAL_HXX
#define OPENTURNS_FACTORIAL_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/StratifiedExperiment.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class Factorial
 *
 * The class describes the probabilistic concept of factorial experiment plan
 */
class OT_API Factorial
  : public StratifiedExperiment
{
  CLASSNAME
public:


  /** Default constructor */
  Factorial();

  /** Constructor with parameters */
  Factorial(const Point & center,
            const Point & levels);

  /** Constructor with parameters */
  Factorial(const UnsignedInteger dimension,
            const Point & levels);

  /** Virtual constructor */
  virtual Factorial * clone() const;

  /** Experiment plane generation */
  virtual Sample generate() const;

  /** String converter */
  virtual String __repr__() const;

}; /* class Factorial */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FACTORIAL_HXX */
