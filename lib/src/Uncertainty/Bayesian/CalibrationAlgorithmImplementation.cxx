//                                               -*- C++ -*-
/**
 *  @brief Default CalibrationAlgorithmImplementation
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/CalibrationAlgorithmImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(CalibrationAlgorithmImplementation)

static const Factory<CalibrationAlgorithmImplementation> Factory_CalibrationAlgorithmImplementation;

/* Default constructor */
CalibrationAlgorithmImplementation::CalibrationAlgorithmImplementation()
  : PersistentObject()
{
  // Nothing to do
}

/* Parameter constructor */
CalibrationAlgorithmImplementation::CalibrationAlgorithmImplementation(const Function & model,
    const Sample & inputObservations,
    const Sample & outputObservations,
    const Distribution & parameterPrior)
  : PersistentObject()
  , model_(model)
  , inputObservations_(inputObservations)
  , outputObservations_(outputObservations)
  , parameterPrior_(parameterPrior)
  , result_()
{
  // Allow for a dim-0 sample but it should have the right size internally
  if (!inputObservations_.getDimension())
    inputObservations_ = Sample(outputObservations.getSize(), 0);
}

/* Performs the actual computation. Must be overloaded by the actual calibration algorithm */
void CalibrationAlgorithmImplementation::run()
{
  throw NotYetImplementedException(HERE) << "In CalibrationAlgorithmImplementation::run()";
}

/* String converter */
String CalibrationAlgorithmImplementation::__repr__() const
{
  return OSS() << "class=" << CalibrationAlgorithmImplementation::GetClassName()
         << " output observations=" << outputObservations_
         << " parameter prior=" << parameterPrior_
         << " result=" << result_;
}

/* Model accessor */
Function CalibrationAlgorithmImplementation::getModel() const
{
  return model_;
}

/* Input observations accessor */
Sample CalibrationAlgorithmImplementation::getInputObservations() const
{
  return inputObservations_;
}

/* Output observations accessor */
Sample CalibrationAlgorithmImplementation::getOutputObservations() const
{
  return outputObservations_;
}

/* Parameter prior accessor */
Distribution CalibrationAlgorithmImplementation::getParameterPrior() const
{
  return parameterPrior_;
}

/* Result accessor */
void CalibrationAlgorithmImplementation::setResult(const CalibrationResult & result)
{
  result_ = result;
}

CalibrationResult CalibrationAlgorithmImplementation::getResult() const
{
  return result_;
}

/* Virtual copy constructor */
CalibrationAlgorithmImplementation* CalibrationAlgorithmImplementation::clone() const
{
  return new CalibrationAlgorithmImplementation(*this);
}

/* Method save() stores the object through the StorageManager */
void CalibrationAlgorithmImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("model_", model_);
  adv.saveAttribute("inputObservations_", inputObservations_);
  adv.saveAttribute("outputObservations_", outputObservations_);
  adv.saveAttribute("parameterPrior_", parameterPrior_);
  adv.saveAttribute("result_", result_);
}

/* Method load() reloads the object from the StorageManager */
void CalibrationAlgorithmImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("model_", model_);
  adv.loadAttribute("inputObservations_", inputObservations_);
  adv.loadAttribute("outputObservations_", outputObservations_);
  adv.loadAttribute("parameterPrior_", parameterPrior_);
  adv.loadAttribute("result_", result_);
}


END_NAMESPACE_OPENTURNS
