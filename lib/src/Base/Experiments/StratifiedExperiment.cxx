//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of a stratified experiment plane
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
#include "openturns/StratifiedExperiment.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(StratifiedExperiment);

static const Factory<StratifiedExperiment> Factory_StratifiedExperiment;

/* Default constructor */
StratifiedExperiment::StratifiedExperiment()
  : ExperimentImplementation()
  , center_(NumericalPoint(0))
  , levels_(NumericalPoint(0))
{
  // Nothing to do
}

/* Constructor with parameters */
StratifiedExperiment::StratifiedExperiment(const NumericalPoint & center,
    const NumericalPoint & levels)
  : ExperimentImplementation()
  , center_(center)
  , levels_(levels)
{
  // Nothing to do
}

/* String converter */
String StratifiedExperiment::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName ()
      << " center=" << center_
      << " levels=" << levels_;
  return oss;
}

/* Center accessor */
void StratifiedExperiment::setCenter(const NumericalPoint & center)
{
  center_ = center;
}

/* Center accessor */
NumericalPoint StratifiedExperiment::getCenter() const
{
  return center_;
}

/* Levels accessor */
void StratifiedExperiment::setLevels(const NumericalPoint & levels)
{
  levels_ = levels;
}

/* Levels accessor */
NumericalPoint StratifiedExperiment::getLevels() const
{
  return levels_;
}

/* Method save() stores the object through the StorageManager */
void StratifiedExperiment::save(Advocate & adv) const
{
  ExperimentImplementation::save(adv);
  adv.saveAttribute("center_", center_);
  adv.saveAttribute("levels_", levels_);
}

/* Method load() reloads the object from the StorageManager */
void StratifiedExperiment::load(Advocate & adv)
{
  ExperimentImplementation::load(adv);
  adv.loadAttribute("center_", center_);
  adv.loadAttribute("levels_", levels_);
}

END_NAMESPACE_OPENTURNS
