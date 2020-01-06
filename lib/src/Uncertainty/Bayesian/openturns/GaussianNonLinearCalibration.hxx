//                                               -*- C++ -*-
/**
 *  @brief GaussianNonLinearCalibration algorithm
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
#ifndef OPENTURNS_GAUSSIANNONLINEARCALIBRATION_HXX
#define OPENTURNS_GAUSSIANNONLINEARCALIBRATION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/CalibrationAlgorithmImplementation.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Function.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/CenteredFiniteDifferenceGradient.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GaussianNonLinearCalibration
 *
 * @brief The class implements the Gaussian non-linear calibration
 *
 */
class OT_API GaussianNonLinearCalibration
  : public CalibrationAlgorithmImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  GaussianNonLinearCalibration();

  /** Parameter constructor */
  GaussianNonLinearCalibration(const Function & model,
                               const Sample & inputObservations,
                               const Sample & outputObservations,
                               const Point & candidate,
                               const CovarianceMatrix & parameterCovariance,
                               const CovarianceMatrix & errorCovariance);

  /** String converter */
  virtual String __repr__() const;

  /** Performs the actual computation. Must be overloaded by the actual calibration algorithm */
  virtual void run();
  Point run(const Sample & inputObservations,
            const Sample & outputObservations,
            const Point & candidate,
            const TriangularMatrix & parameterInverseCholesky,
            const TriangularMatrix & errorInverseCholesky);

  /** Algorithm accessor */
  OptimizationAlgorithm getOptimizationAlgorithm() const;
  void setOptimizationAlgorithm(const OptimizationAlgorithm & algorithm);

  /** Candidate accessor */
  Point getCandidate() const;

  /** Parameter covariance accessor */
  CovarianceMatrix getParameterCovariance() const;

  /** Error covariance accessor */
  CovarianceMatrix getErrorCovariance() const;

  /** Flag for the full error covariance accessor */
  Bool getGlobalErrorCovariance() const;

  /** Bootstrap size accessor */
  UnsignedInteger getBootstrapSize() const;
  void setBootstrapSize(const UnsignedInteger bootstrapSize);

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  virtual GaussianNonLinearCalibration * clone() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  /* Model to calibrate */
  Function model_;

  /* The input observations */
  Sample inputObservations_;

  /* The optimization algorithm */
  OptimizationAlgorithm algorithm_;

  /* Number of bootstrap replica */
  UnsignedInteger bootstrapSize_;

  /* The error covariance */
  CovarianceMatrix errorCovariance_;

  /* Flag to tell if the error covariance is for the whole observations */
  Bool globalErrorCovariance_;

}; /* class GaussianNonLinearCalibration */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GAUSSIANNONLINEARCALIBRATION_HXX */
