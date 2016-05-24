//                                               -*- C++ -*-
/**
 *  @brief Implementation for sensitivity algorithms
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_JANSENSENSITIVITYALGORITHM_HXX
#define OPENTURNS_JANSENSENSITIVITYALGORITHM_HXX

#include "openturns/NumericalPoint.hxx"
#include "openturns/SobolIndicesAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class JansenSensitivityAlgorithm
 *
 */

class OT_API JansenSensitivityAlgorithm
  : public SobolIndicesAlgorithmImplementation
{
  CLASSNAME;
public:

  /** Default constructor */
  JansenSensitivityAlgorithm();

  /** Constructor with parameters */
  JansenSensitivityAlgorithm(const NumericalSample & inputDesign,
                             const NumericalSample & outputDesign,
                             const UnsignedInteger size);

  /** Constructor with distribution / model parameters */
  JansenSensitivityAlgorithm(const Distribution & distribution,
                             const UnsignedInteger size,
                             const NumericalMathFunction & model,
                             const Bool computeSecondOrder = true);

  /** Constructor with experiment / model parameters */
  JansenSensitivityAlgorithm(const WeightedExperiment & experiment,
                             const NumericalMathFunction & model,
                             const Bool computeSecondOrder = true);

  /** Virtual constructor */
  virtual JansenSensitivityAlgorithm * clone() const;

  /** String converter */
  virtual String __repr__() const;

protected:

  /** Internal method that compute Vi/VTi using a huge sample */
  NumericalSample computeIndices(const NumericalSample & sample,
                                 NumericalSample & VTi) const;
}; /* class JansenSensitivityAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_JANSENSENSITIVITYALGORITHM_HXX */
