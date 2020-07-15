//                                               -*- C++ -*-
/**
 *  @brief CalibrationResult implements the result of an algorithm for solving a calibration problem
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
#ifndef OPENTURNS_CALIBRATIONRESULT_HXX
#define OPENTURNS_CALIBRATIONRESULT_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Function.hxx"
#include "openturns/GridLayout.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class CalibrationResult
 * CalibrationResult stores the calibration result
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
                    const Point & parameterMAP,
                    const Distribution & observationsError,
                    const Sample & inputObservations,
                    const Sample & outputObservations,
                    const Function & residualFunction);

  /** Virtual constructor */
  virtual CalibrationResult * clone() const;

  /** Parameter prior distribution accessors */
  void setParameterPrior(const Distribution & parameterPrior);
  Distribution getParameterPrior() const;

  /** Parameter posterior distribution accessor */
  void setParameterPosterior(const Distribution & parameterPosterior);
  Distribution getParameterPosterior() const;

  /** Observations error distribution accessor */
  void setObservationsError(const Distribution & observationsError);
  Distribution getObservationsError() const;

  /** Parameter maximum a posteriori accessors */
  void setParameterMAP(const Point & parameterMAP);
  Point getParameterMAP() const;

  /** Input observations accessors */
  void setInputObservations(const Sample & inputObservations);
  Sample getInputObservations() const;

  /** Output observations accessors */
  void setOutputObservations(const Sample & outputObservations);
  Sample getOutputObservations() const;

  /** Residual function accessors */
  void setResidualFunction(const Function & residualFunction);
  Function getResidualFunction() const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

  /** Output at prior/posterior accessor */
  void setOutputAtPriorAndPosteriorMean(const Sample & outputAtPriorMean, const Sample & outputAtPosteriorMean);
  Sample getOutputAtPriorMean() const;
  Sample getOutputAtPosteriorMean() const;

  /** Graphic analysis */
  GridLayout drawParameterDistributions() const;
  GridLayout drawResiduals() const;
  GridLayout drawObservationsVsInputs() const;
  GridLayout drawObservationsVsPredictions() const;

private:

  /* Parameter prior distribution */
  Distribution parameterPrior_;

  /* Parameter posterior distribution */
  Distribution parameterPosterior_;

  /* Parameter MAP */
  Point parameterMAP_;

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

}; // class CalibrationResult


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CALIBRATIONRESULT_HXX */

