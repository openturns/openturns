//                                               -*- C++ -*-
/**
 *  @brief GaussianLinearCalibration algorithm
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
#ifndef OPENTURNS_GAUSSIANLINEARCALIBRATION_HXX
#define OPENTURNS_GAUSSIANLINEARCALIBRATION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/CalibrationAlgorithmImplementation.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Function.hxx"
#include "openturns/LeastSquaresMethod.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/CovarianceMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GaussianLinearCalibration
 *
 * @brief The class implements the best linear unbiased estimator (GaussianLinearCalibration) calibration
 *
 */
class OT_API GaussianLinearCalibration
  : public CalibrationAlgorithmImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  GaussianLinearCalibration();

  /** Parameter constructor */
  GaussianLinearCalibration(const Function & model,
                            const Sample & inputObservations,
                            const Sample & outputObservations,
                            const Point & parameterMean,
                            const CovarianceMatrix & parameterCovariance,
                            const CovarianceMatrix & errorCovariance,
                            const String & methodName = ResourceMap::GetAsString("GaussianLinearCalibration-Method"));

  /** Parameter constructor */
  GaussianLinearCalibration(const Sample & modelObservations,
                            const Matrix & gradientObservations,
                            const Sample & outputObservations,
                            const Point & parameterMean,
                            const CovarianceMatrix & parameterCovariance,
                            const CovarianceMatrix & errorCovariance,
                            const String & methodName = ResourceMap::GetAsString("GaussianLinearCalibration-Method"));

  /** String converter */
  String __repr__() const override;

  /** Performs the actual computation. Must be overloaded by the actual calibration algorithm */
  void run() override;

  /** Model observations accessor */
  Sample getModelObservations() const;

  /** Model gradient wrt the parameter accessor */
  Matrix getGradientObservations() const;

  /** ParameterMean accessor */
  Point getParameterMean() const;

  /** Parameter covariance accessor */
  CovarianceMatrix getParameterCovariance() const;

  /** Error covariance accessor */
  CovarianceMatrix getErrorCovariance() const;

  /** Flag for the full error covariance accessor */
  Bool getGlobalErrorCovariance() const;

  /** Least squares method name accessor */
  String getMethodName() const;

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  GaussianLinearCalibration * clone() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  /* The model observations */
  Sample modelObservations_;

  /* The model gradient wrt the parameter */
  Matrix gradientObservations_;

  /* The error covariance */
  CovarianceMatrix errorCovariance_;

  /* Flag to tell if the error covariance is for the whole observations */
  Bool globalErrorCovariance_;

  /* The least squares method name */
  String methodName_;

}; /* class GaussianLinearCalibration */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GAUSSIANLINEARCALIBRATION_HXX */
