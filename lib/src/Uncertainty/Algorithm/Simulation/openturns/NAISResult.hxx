//                                               -*- C++ -*-
/**
 *  @brief class for NAISResult
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
#ifndef OPENTURNS_NAISRESULT_HXX
#define OPENTURNS_NAISRESULT_HXX

#include "openturns/ProbabilitySimulationResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NAISResult
 *
 * NAISResult is some NAISResult type to illustrate how to add some classes in Open TURNS
 */
class OT_API NAISResult
  : public ProbabilitySimulationResult
{

  CLASSNAME
public:

  /** Default constructor */
  NAISResult();

  /** Get Samples */
  Sample getAuxiliarySample() const;

  /** Set NAISResult Samples */
  void setAuxiliarySample(const Sample & sample);

  /** Get Auxiliary Density */
  Distribution getAuxiliaryDistribution() const;

  /** Set Auxiliary Density */
  void setAuxiliaryDistribution(const Distribution & auxiliaryDistribution);
  
  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  // The fixed Auxiliary distribution that will be returned at each call
  Distribution auxiliaryDistribution_;

  // The fixed sample that will be returned at each call
  Sample sample_;


}; /* class NAISResult */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NAISRESULT_HXX */
