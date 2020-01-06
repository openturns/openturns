//                                               -*- C++ -*-
/**
 *  @brief Box experiment plane
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
#ifndef OPENTURNS_BOX_HXX
#define OPENTURNS_BOX_HXX

#include "openturns/StratifiedExperiment.hxx"
#include "openturns/Interval.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class Box
 *
 * The class describes the probabilistic concept of box experiment plan
 */
class OT_API Box
  : public StratifiedExperiment
{
  CLASSNAME
public:


  /** Default constructor */
  Box();

  /** Constructor with parameters */
  explicit Box(const Point & levels);

  explicit Box(const Indices & levels);

  Box(const Indices & levels,
      const Interval & bounds);

  /** Virtual constructor */
  virtual Box * clone() const;

  /** Experiment plane generation :
   *  The box [0, 1]^n is uniformly sampled in each dimension
   */
  virtual Sample generate() const;

  /** String converter */
  virtual String __repr__() const;

  /** Specific levels accessor */
  void setLevels(const Point & levels);

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:
  Interval bounds_;

}; /* class Box */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BOX_HXX */
