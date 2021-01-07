//                                               -*- C++ -*-
/**
 *  @brief Box experiment plane
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
  Box * clone() const override;

  /** Experiment plane generation :
   *  The box [0, 1]^n is uniformly sampled in each dimension
   */
  Sample generate() const override;

  /** String converter */
  String __repr__() const override;

  /** Specific levels accessor */
  void setLevels(const Point & levels) override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  Interval bounds_;

}; /* class Box */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BOX_HXX */
