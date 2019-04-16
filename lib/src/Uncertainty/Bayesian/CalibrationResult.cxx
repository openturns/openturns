//                                               -*- C++ -*-
/**
 *  @brief CalibrationResult stores the result of a CalibrationAlgorithmImplementation
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/CalibrationResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CalibrationResult)

static const Factory<CalibrationResult> Factory_CalibrationResult;

/* Default constructor */
CalibrationResult::CalibrationResult()
  : PersistentObject()
  , parameterPrior_()
  , parameterPosterior_()
  , observationsError_()
{
  // Nothing to do
}

/* Parameter constructor */
CalibrationResult::CalibrationResult(const Distribution & parameterPrior,
				     const Distribution & parameterPosterior,
				     const Point & parameterMAP,
				     const Distribution & observationsError)
  : PersistentObject()
  , parameterPrior_(parameterPrior)
  , parameterPosterior_(parameterPosterior)
  , parameterMAP_(parameterMAP)
  , observationsError_(observationsError)
{
  // Nothing to do
}


/* Virtual constructor */
CalibrationResult * CalibrationResult::clone() const
{
  return new CalibrationResult(*this);
}

/* Parameter prior accessors */
Distribution CalibrationResult::getParameterPrior() const
{
  return parameterPrior_;
}

void CalibrationResult::setParameterPrior(const Distribution & parameterPrior)
{
  parameterPrior_ = parameterPrior;
}

/* Parameter posterior accessors */
Distribution CalibrationResult::getParameterPosterior() const
{
  return parameterPosterior_;
}

void CalibrationResult::setParameterPosterior(const Distribution & parameterPosterior)
{
  parameterPosterior_ = parameterPosterior;
}

/* Parameter MAP accessors */
Point CalibrationResult::getParameterMAP() const
{
  return parameterMAP_;
}

void CalibrationResult::setParameterMAP(const Point & parameterMAP)
{
  parameterMAP_ = parameterMAP;
}

/* Observations error accessors */
Distribution CalibrationResult::getObservationsError() const
{
  return observationsError_;
}

void CalibrationResult::setObservationsError(const Distribution & observationsError)
{
  observationsError_ = observationsError;
}


/* String converter */
String CalibrationResult::__repr__() const
{
  OSS oss;
  oss << "class=" << CalibrationResult::GetClassName()
      << " parameter prior=" << parameterPrior_
      << " parameter posterior=" << parameterPosterior_
      << " parameter MAP=" << parameterMAP_
      << " observations error=" << observationsError_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void CalibrationResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "parameterPrior_", parameterPrior_ );
  adv.saveAttribute( "parameterPosterior_", parameterPosterior_ );
  adv.saveAttribute( "parameterMAP_", parameterMAP_ );
  adv.saveAttribute( "observationsError_", observationsError_ );
}

/* Method load() reloads the object from the StorageManager */
void CalibrationResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "parameterPrior_", parameterPrior_ );
  adv.loadAttribute( "parameterPosterior_", parameterPosterior_ );
  adv.loadAttribute( "parameterMAP_", parameterMAP_ );
  adv.loadAttribute( "observationsError_", observationsError_ );
}


END_NAMESPACE_OPENTURNS

