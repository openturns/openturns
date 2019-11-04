//                                               -*- C++ -*-
/**
 *  @brief Axial experiment plane
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
#ifndef OPENTURNS_AXIAL_HXX
#define OPENTURNS_AXIAL_HXX

#include "openturns/StratifiedExperiment.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class Axial
 *
 * The class describes the probabilistic concept of axial experiment plan
 */
class OT_API Axial
  : public StratifiedExperiment
{
  CLASSNAME
public:


  /** Default constructor */
  Axial();

  /** Constructor with parameters */
  Axial(const Point & center,
        const Point & levels);

  /** Constructor with parameters */
  Axial(const UnsignedInteger dimension,
        const Point & levels);

  /** Virtual constructor */
  virtual Axial * clone() const;

  /** Experiment plane generation */
  virtual Sample generate() const;

  /** String converter */
  virtual String __repr__() const;

}; /* class Axial */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_AXIAL_HXX */
