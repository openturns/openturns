//                                               -*- C++ -*-
/**
 *  @brief FORM implements the First Order Reliability Method
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/FORM.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(FORM)

static const Factory<FORM> Factory_FORM;

/* Default constructor for the save/load mechanism */
FORM::FORM()
  : Analytical(),
    formResult_()
{
  // Nothing to do
}

/*
 * @brief  Standard constructor: the class is defined by an optimisation algorithm, a failure event and a physical starting point
 */
FORM::FORM(const OptimizationAlgorithm & nearestPointAlgorithm,
           const RandomVector & event,
           const Point & physicalStartingPoint):
  Analytical(nearestPointAlgorithm, event, physicalStartingPoint)
{
  // Nothing to do
}

/* Virtual constructor */
FORM * FORM::clone() const
{
  return new FORM(*this);
}

/* Result accessor */
FORMResult FORM::getResult() const
{
  return formResult_;
}

/* Result accessor */
void FORM::setResult(const FORMResult & formResult)
{
  formResult_ = formResult;
}


/* String converter */
String FORM::__repr__() const
{
  OSS oss;
  oss << "class=" << FORM::GetClassName()
      << " " << Analytical::__repr__()
      << " result=" << formResult_;
  return oss;
}

/* Function that computes the design point by re-using the Analytical::run() and creates a FORMResult */
void FORM::run()
{
  Analytical::run();
  formResult_ = FORMResult(Analytical::getAnalyticalResult().getStandardSpaceDesignPoint(), Analytical::getAnalyticalResult().getLimitStateVariable(), Analytical::getAnalyticalResult().getIsStandardPointOriginInFailureSpace());
  formResult_.setOptimizationResult(Analytical::getAnalyticalResult().getOptimizationResult());
}

/* Method save() stores the object through the StorageManager */
void FORM::save(Advocate & adv) const
{

  Analytical::save(adv);
  adv.saveAttribute("formResult_", formResult_);
}

/* Method load() reloads the object from the StorageManager */
void FORM::load(Advocate & adv)
{
  Analytical::load(adv);
  adv.loadAttribute("formResult_", formResult_);
}

END_NAMESPACE_OPENTURNS
