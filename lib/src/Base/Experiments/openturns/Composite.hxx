//                                               -*- C++ -*-
/**
 *  @brief Composite experiment plane
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_COMPOSITE_HXX
#define OPENTURNS_COMPOSITE_HXX

#include "openturns/StratifiedExperiment.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class Composite
 *
 * The class describes the probabilistic concept of composite experiment plan
 */
class OT_API Composite
  : public StratifiedExperiment
{
  CLASSNAME
public:


  /** Default constructor */
  Composite();

  /** Constructor with parameters */
  Composite(const Point & center,
            const Point & levels);

  /** Constructor with parameters */
  Composite(const UnsignedInteger dimension,
            const Point & levels);

  /** Virtual constructor */
  virtual Composite * clone() const;

  /** Experiment plane generation */
  virtual Sample generate() const;

  /** String converter */
  virtual String __repr__() const;

}; /* class Composite */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPOSITE_HXX */
