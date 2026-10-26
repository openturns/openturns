//                                               -*- C++ -*-
/**
 *  @brief CalibrationResult base class to store the result of a calibration algorithm
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_CALIBRATIONRESULT_HXX
#define OPENTURNS_CALIBRATIONRESULT_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CalibrationResult
 * Base class for calibration result
 */
class OT_API CalibrationResult
  : public PersistentObject
{
  CLASSNAME

public:

  /** Default constructor */
  CalibrationResult();

  /** Standard constructor */
  CalibrationResult(const Distribution & parameterPrior,
                    const Distribution & parameterPosterior,
                    const Point & calibratedParameter,
                    const Distribution & observationsError,
                    const Sample & inputObservations,
                    const Sample & outputObservations,
                    const Function & residualFunction,
                    const UnsignedInteger bootstrapSize);

  /** Virtual constructor */
  CalibrationResult * clone() const override;

  /** Bayesian method accessor */
  virtual Bool isBayesian() const;

  /** Observations error distribution accessor */
  Distribution getObservationsError() const;

  /** Input observations accessors */
  void setInputObservations(const Sample & inputObservations);
  Sample getInputObservations() const;

  /** Output observations accessors */
  void setOutputObservations(const Sample & outputObservations);
  Sample getOutputObservations() const;

  /** Residual function accessor */
  Function getResidualFunction() const;

  /** Bootstrap size accessor */
  UnsignedInteger getBootstrapSize();

  /** Output at prior/posterior accessors */
  void setOutputAtPriorAndPosteriorMean(const Sample & outputAtPriorMean, const Sample & outputAtPosteriorMean);
  Sample getOutputAtPriorMean() const;
  Sample getOutputAtPosteriorMean() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Graphic analysis */
  GridLayout drawParameterDistributions() const;
  GridLayout drawResiduals() const;
  GridLayout drawObservationsVsInputs() const;
  GridLayout drawObservationsVsPredictions() const;
  GridLayout drawResidualsNormalPlot() const;

protected:

  void setColors();

  /* Parameter prior distribution */
  Distribution parameterPrior_;

  /* Parameter posterior distribution */
  Distribution parameterPosterior_;

  /* Calibrated value of the parameter */
  Point calibratedParameter_;

  /* Observations error distribution */
  Distribution observationsError_;

  /* Input observations */
  Sample inputObservations_;

  /* Output observations */
  Sample outputObservations_;

  /* Residual function */
  Function residualFunction_;

  /* Output at prior/posterior */
  Sample outputAtPriorMean_;
  Sample outputAtPosteriorMean_;

  /* Bootstrap size */
  UnsignedInteger bootstrapSize_ = 0;

  /* Colors */
  String priorColor_;
  String posteriorColor_;
  String observationColor_;

}; // class CalibrationResult

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CALIBRATIONRESULT_HXX */
