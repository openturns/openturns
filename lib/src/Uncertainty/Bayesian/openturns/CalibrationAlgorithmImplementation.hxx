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
#ifndef OPENTURNS_CALIBRATIONALGORITHMIMPLEMENTATION_HXX
#define OPENTURNS_CALIBRATIONALGORITHMIMPLEMENTATION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/CalibrationResult.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CalibrationAlgorithmImplementation
 *
 * @brief The class implements the concept of CalibrationAlgorithm.
 *
 * This class is abstract so it can not be instantiated. It must be derived.
 * @see CalibrationAlgorithm
 */
class OT_API CalibrationAlgorithmImplementation
  : public PersistentObject
{
  CLASSNAME
public:

  /** Default constructor */
  CalibrationAlgorithmImplementation();

  /** Parameter constructor */
  CalibrationAlgorithmImplementation(const Function & model,
                                     const Sample & inputObservations,
                                     const Sample & outputObservations,
                                     const Distribution & parameterPrior);

  /** String converter */
  String __repr__() const override;

  /** Model accessor */
  Function getModel() const;

  /** Input observations accessor */
  Sample getInputObservations() const;

  /** Output observations accessor */
  Sample getOutputObservations() const;

  /** Prior parameter distribution accessor */
  Distribution getParameterPrior() const;

  /** Performs the actual computation. Must be overloaded by the actual calibration algorithm */
  virtual void run();

  /** Calibration result accessor */
  CalibrationResult getResult() const;
  void setResult(const CalibrationResult & result);

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  CalibrationAlgorithmImplementation * clone() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /* Model to calibrate */
  Function model_;

  /* The input observations */
  Sample inputObservations_;

  /* The output observations */
  Sample outputObservations_;

  /* The parameter prior distribution */
  Distribution parameterPrior_;

  /* The calibration result */
  CalibrationResult result_;

}; /* class CalibrationAlgorithmImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CALIBRATIONALGORITHMIMPLEMENTATION_HXX */
