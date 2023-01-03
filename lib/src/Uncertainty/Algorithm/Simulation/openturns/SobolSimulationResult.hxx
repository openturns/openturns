//                                               -*- C++ -*-
/**
 *  @brief Result of a Sobol indices simulation
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SOBOLSIMULATIONRESULT_HXX
#define OPENTURNS_SOBOLSIMULATIONRESULT_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/SimulationResult.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SobolSimulationResult
 */
class OT_API SobolSimulationResult
  : public SimulationResult
{

  CLASSNAME
public:


  /** Default constructor */
  SobolSimulationResult();

  /** Standard constructor */
  SobolSimulationResult(const Distribution & firstOrderIndicesDistribution,
                        const Distribution & totalOrderIndicesDistribution,
                        const UnsignedInteger outerSampling,
                        const UnsignedInteger blockSize);

  /** Virtual constructor */
  SobolSimulationResult * clone() const override;

  /** Probability estimate accessor */
  Point getFirstOrderIndicesEstimate() const;
  Point getTotalOrderIndicesEstimate() const;

  /** Distribution of the expectation */
  void setFirstOrderIndicesDistribution(const Distribution & firstOrderIndicesDistribution);
  void setTotalOrderIndicesDistribution(const Distribution & totalOrderIndicesDistribution);
  Distribution getFirstOrderIndicesDistribution() const;
  Distribution getTotalOrderIndicesDistribution() const;

  /** String converter */
  String __repr__() const override;

  /** Draw indices */
  Graph draw(const Scalar confidenceLevel = 0.95) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /* The event we are interested in */
  Distribution firstOrderIndicesDistribution_;
  Distribution totalOrderIndicesDistribution_;

}; // class SobolSimulationResult

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SOBOLSIMULATIONRESULT_HXX */
