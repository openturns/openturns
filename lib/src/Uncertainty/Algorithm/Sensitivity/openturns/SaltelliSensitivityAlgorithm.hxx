//                                               -*- C++ -*-
/**
 *  @brief Implementation for sensitivity algorithms
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SALTELLISENSITIVITYALGORITHM_HXX
#define OPENTURNS_SALTELLISENSITIVITYALGORITHM_HXX

#include "openturns/Point.hxx"
#include "openturns/SobolIndicesAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class SaltelliSensitivityAlgorithm
 *
 */

class OT_API SaltelliSensitivityAlgorithm
  : public SobolIndicesAlgorithmImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  SaltelliSensitivityAlgorithm();

  /** Constructor with parameters */
  SaltelliSensitivityAlgorithm(const Sample & inputDesign,
                               const Sample & outputDesign,
                               const UnsignedInteger size);

  /** Constructor with distribution / model parameters */
  SaltelliSensitivityAlgorithm(const Distribution & distribution,
                               const UnsignedInteger size,
                               const Function & model,
                               const Bool computeSecondOrder = false);

  /** Constructor with experiment / model parameters */
  SaltelliSensitivityAlgorithm(const WeightedExperiment & experiment,
                               const Function & model,
                               const Bool computeSecondOrder = false);
  /** Virtual constructor */
  SaltelliSensitivityAlgorithm * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Internal method that compute Vi/VTi using a huge sample */
  Sample computeIndices(const Sample & sample, Sample & VTi) const override;

  /** void method that computes asymptotic distribution */
  void computeAsymptoticDistribution() const override;

}; /* class SaltelliSensitivityAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SALTELLISENSITIVITYALGORITHM_HXX */
