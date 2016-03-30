//                                               -*- C++ -*-
/**
 *  @brief Subset sampling result
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
#ifndef OPENTURNS_SUBSETSAMPLINGRESULT_HXX
#define OPENTURNS_SUBSETSAMPLINGRESULT_HXX

#include "openturns/SimulationResultImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API SubsetSamplingResult
: public SimulationResultImplementation
{
  CLASSNAME;
public:

  /** Constructor with parameters */
  SubsetSamplingResult();

  /** Constructor with parameters */
  SubsetSamplingResult(const Event & event,
                       const NumericalScalar probabilityEstimate,
                       const NumericalScalar varianceEstimate,
                       const UnsignedInteger outerSampling,
                       const UnsignedInteger blockSize,
                       const NumericalScalar coefficientOfVariation = 0.0);

  /** Virtual constructor */
  virtual SubsetSamplingResult * clone() const;

  /** Coefficient of variation estimate accessor */
  virtual NumericalScalar getCoefficientOfVariation() const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:
  NumericalScalar coefficientOfVariation_;

private:

} ; /* class SubsetSamplingResult */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SUBSETSAMPLINGRESULT_HXX */
