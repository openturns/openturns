//                                               -*- C++ -*-
/**
 *  @brief Implementation for sensitivity algorithms
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
#ifndef OPENTURNS_JANSENSENSITIVITYALGORITHM_HXX
#define OPENTURNS_JANSENSENSITIVITYALGORITHM_HXX

#include "openturns/Point.hxx"
#include "openturns/SobolIndicesAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class JansenSensitivityAlgorithm
 *
 */

class OT_API JansenSensitivityAlgorithm
  : public SobolIndicesAlgorithmImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  JansenSensitivityAlgorithm();

  /** Constructor with parameters */
  JansenSensitivityAlgorithm(const Sample & inputDesign,
                             const Sample & outputDesign,
                             const UnsignedInteger size);

  /** Constructor with distribution / model parameters */
  JansenSensitivityAlgorithm(const Distribution & distribution,
                             const UnsignedInteger size,
                             const Function & model,
                             const Bool computeSecondOrder = false);

  /** Constructor with experiment / model parameters */
  JansenSensitivityAlgorithm(const WeightedExperiment & experiment,
                             const Function & model,
                             const Bool computeSecondOrder = false);

  /** Virtual constructor */
  JansenSensitivityAlgorithm * clone() const override;

  /** String converter */
  String __repr__() const override;

protected:

  /** Internal method that compute Vi/VTi using a huge sample */
  Sample computeIndices(const Sample & sample, Sample & VTi) const override;

  /** void method that computes asymptotic distribution */
  void computeAsymptoticDistribution() const override;

}; /* class JansenSensitivityAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_JANSENSENSITIVITYALGORITHM_HXX */
