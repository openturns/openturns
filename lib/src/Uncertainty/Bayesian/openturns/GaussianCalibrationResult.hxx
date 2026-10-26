//                                               -*- C++ -*-
/**
 *  @brief GaussianCalibrationResult stores the result of a Gaussian calibration algorithm
 *
 *  Copyright 2005-2026 Airbus-EDF-ONERA-Phimeca
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
#ifndef OPENTURNS_GAUSSIANCALIBRATIONRESULT_HXX
#define OPENTURNS_GAUSSIANCALIBRATIONRESULT_HXX

#include "openturns/CalibrationResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GaussianCalibrationResult
 * GaussianCalibrationResult stores the result of a Gaussian calibration
 */
class OT_API GaussianCalibrationResult
  : public CalibrationResult
{
  CLASSNAME

public:

  /** Default constructor */
  GaussianCalibrationResult();

  /** Parameter constructor */
  GaussianCalibrationResult(const Distribution & parameterPrior,
                    const Distribution & parameterPosterior,
                    const Point & calibratedParameter,
                    const Distribution & observationsError,
                    const Sample & inputObservations,
                    const Sample & outputObservations,
                    const Function & residualFunction,
                    const UnsignedInteger bootstrapSize);

  /** Virtual constructor */
  GaussianCalibrationResult * clone() const override;

  /** Bayesian method accessor */
  Bool isBayesian() const override;

  /** Parameter prior distribution accessor */
  Distribution getParameterPrior() const;

  /** Parameter posterior distribution accessor */
  Distribution getParameterPosterior() const;

  /** Bootstrap MAP distribution accessor */
  Distribution getMAPDistribution() const;

  /** Parameter maximum a posteriori accessor */
  Point getParameterMAP() const;

  /** String converter */
  String __repr__() const override;

}; // class GaussianCalibrationResult

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GAUSSIANCALIBRATIONRESULT_HXX */
