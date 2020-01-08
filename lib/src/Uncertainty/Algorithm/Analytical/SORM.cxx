//                                               -*- C++ -*-
/**
 *  @brief SORM implements the Second Order Reliability Method
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

#include "openturns/SORM.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(SORM)

static const Factory<SORM> Factory_SORM;

/* Default constructor for the save/load mechanism */
SORM::SORM()
  : Analytical(),
    sormResult_()
{
  // Nothing to do
}

/*
 * @brief  Standard constructor: the class is defined by an optimisation algorithm, a failure event and a physical starting point
 */
SORM::SORM(const OptimizationAlgorithm & nearestPointAlgorithm,
           const RandomVector & event,
           const Point & physicalStartingPoint):
  Analytical(nearestPointAlgorithm, event, physicalStartingPoint),
  sormResult_()
{
  // Nothing to do
}

/* Virtual constructor */
SORM * SORM::clone() const
{
  return new SORM(*this);
}

/* Result accessor */
SORMResult SORM::getResult() const
{
  return sormResult_;
}

/* Result accessor */
void SORM::setResult(const SORMResult & sormResult)
{
  sormResult_ = sormResult;
}

/* String converter */
String SORM::__repr__() const
{
  OSS oss;
  oss << "class=" << SORM::GetClassName()
      << " " << Analytical::__repr__()
      << " result=" << sormResult_;
  return oss;
}

/* Function that computes the design point by re-using the Analytical::run() and creates a SORMResult */
void SORM::run()
{
  Analytical::run();
  sormResult_ = SORMResult(Analytical::getAnalyticalResult().getStandardSpaceDesignPoint(), Analytical::getAnalyticalResult().getLimitStateVariable(), Analytical::getAnalyticalResult().getIsStandardPointOriginInFailureSpace());
  sormResult_.setOptimizationResult(Analytical::getAnalyticalResult().getOptimizationResult());
}


/* Method save() stores the object through the StorageManager */
void SORM::save(Advocate & adv) const
{

  Analytical::save(adv);
  adv.saveAttribute("sormResult_", sormResult_);
}

/* Method load() reloads the object from the StorageManager */
void SORM::load(Advocate & adv)
{
  Analytical::load(adv);
  adv.loadAttribute("sormResult_", sormResult_);
}

END_NAMESPACE_OPENTURNS

