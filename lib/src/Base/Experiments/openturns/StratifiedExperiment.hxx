//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an stratifiedExperiment plane
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
#ifndef OPENTURNS_STRATIFIEDEXPERIMENT_HXX
#define OPENTURNS_STRATIFIEDEXPERIMENT_HXX

#include "openturns/ExperimentImplementation.hxx"
#include "openturns/NumericalPoint.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class StratifiedExperiment
 *
 * The class describes the probabilistic concept of stratifiedExperiment plan
 */
class OT_API StratifiedExperiment
  : public ExperimentImplementation
{
  CLASSNAME;
public:


  /** Default constructor */
  StratifiedExperiment();

  /** Constructor with parameters */
  StratifiedExperiment(const NumericalPoint & center,
                       const NumericalPoint & levels);

  /** String converter */
  virtual String __repr__() const;

  /** Center accessor */
  virtual void setCenter(const NumericalPoint & center);

  /** Center accessor */
  virtual NumericalPoint getCenter() const;

  /** Levels accessor */
  virtual void setLevels(const NumericalPoint & levels);

  /** Levels accessor */
  virtual NumericalPoint getLevels() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);
protected:

  NumericalPoint center_;
  NumericalPoint levels_;

}; /* class StratifiedExperiment */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_STRATIFIEDEXPERIMENT_HXX */
