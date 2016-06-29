//                                               -*- C++ -*-
/**
 *  @brief Implementation of SimulationResult
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
#ifndef OPENTURNS_QUASIMONTECARLORESULT_HXX
#define OPENTURNS_QUASIMONTECARLORESULT_HXX

#include "openturns/SimulationResultImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class QuasiMonteCarloResult
 * Implementation of SimulationResult
 */
class OT_API QuasiMonteCarloResult
  : public SimulationResultImplementation
{

  CLASSNAME;
public:


  /** Default constructor */
  QuasiMonteCarloResult();

  /** Standard constructor */
  QuasiMonteCarloResult(const Event & event,
                        const NumericalScalar probabilityEstimate,
                        const NumericalScalar varianceEstimate,
                        const UnsignedInteger outerSampling,
                        const UnsignedInteger blockSize);

  /** Virtual constructor */
  virtual QuasiMonteCarloResult * clone() const;

  /** Coefficient of variation estimate accessor */
  virtual NumericalScalar getCoefficientOfVariation() const;

  /** Standard deviation estimate accessor */
  virtual NumericalScalar getStandardDeviation() const;

  /** Confidence length */
  virtual NumericalScalar getConfidenceLength(const NumericalScalar level = ResourceMap::GetAsNumericalScalar("SimulationResult-DefaultConfidenceLevel")) const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

}; // class QuasiMonteCarloResult

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_QUASIMONTECARLORESULT_HXX */
