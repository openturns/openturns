//                                               -*- C++ -*-
/**
 *  @brief Implementation for sobol indices algorithms
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_JANSENSOBOLINDICES_HXX
#define OPENTURNS_JANSENSOBOLINDICES_HXX

#include "openturns/Point.hxx"
#include "openturns/Interval.hxx"
#include "openturns/IterativeVariance.hxx"
#include "openturns/SobolIndicesImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class JansenSobolIndices
 *
 */

class OT_API JansenSobolIndices
  : public SobolIndicesImplementation
{
  CLASSNAME

public:

  /** Default constructor */
  JansenSobolIndices();

  /** Parameter constructor */
  JansenSobolIndices(const UnsignedInteger modelInputDimension,
                       const UnsignedInteger modelOutputDimension);

  /** Virtual constructor */
  virtual JansenSobolIndices * clone() const;

  /** Method that compute first and total order indices using a collection of sample */
  void computeIndices(const Sample & inputSample);

  /** Method that increment first and total order indices using a sample */
  void incrementIndices(const Sample & inputSample);

  /** Interval for the first order indices accessor */
  Interval getFirstOrderIndicesInterval() const;

  /** Interval for the total order indices accessor */
  Interval getTotalOrderIndicesInterval() const;

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

private:

  IterativeVariance varianceA_;

  Sample squaredSumyBMinusyE_;

  Sample squaredSumyAMinusyE_;

}; /* class JansenSobolIndices */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_JANSENSOBOLINDICES_HXX */
