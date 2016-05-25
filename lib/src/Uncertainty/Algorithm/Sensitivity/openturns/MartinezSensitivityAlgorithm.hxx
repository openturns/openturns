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
#ifndef OPENTURNS_MARTINEZSENSITIVITYALGORITHM_HXX
#define OPENTURNS_MARTINEZSENSITIVITYALGORITHM_HXX

#include "openturns/NumericalPoint.hxx"
#include "openturns/SobolIndicesAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class MartinezSensitivityAlgorithm
 *
 */

class OT_API MartinezSensitivityAlgorithm
  : public SobolIndicesAlgorithmImplementation
{
  CLASSNAME;
public:

  /** Default constructor */
  MartinezSensitivityAlgorithm();

  /** Constructor with parameters */
  MartinezSensitivityAlgorithm(const NumericalSample & inputDesign,
                               const NumericalSample & outputDesign,
                               const UnsignedInteger size);

  /** Constructor with distribution / model parameters */
  MartinezSensitivityAlgorithm(const Distribution & distribution,
                               const UnsignedInteger size,
                               const NumericalMathFunction & model,
                               const Bool computeSecondOrder = true);

  /** Constructor with experiment / model parameters */
  MartinezSensitivityAlgorithm(const WeightedExperiment & experiment,
                               const NumericalMathFunction & model,
                               const Bool computeSecondOrder = true);

  /** Virtual constructor */
  virtual MartinezSensitivityAlgorithm * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Interval for the first order indices accessor */
  virtual Interval getFirstOrderIndicesInterval() const;

  /** Interval for the total order indices accessor */
  virtual Interval getTotalOrderIndicesInterval() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:

  /** Internal method that compute Vi/VTi using a huge sample */
  NumericalSample computeIndices(const NumericalSample & sample,
                                 NumericalSample & VTi) const;

  // Compute the fisher transform
  void computeAsymptoticInterval() const;

private:
  mutable Bool useAsymptoticInterval_;

}; /* class MartinezSensitivityAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MARTINEZSENSITIVITYALGORITHM_HXX */
