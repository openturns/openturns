//                                               -*- C++ -*-
/**
 *  @brief This class is enables to estimate trend
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
#ifndef OPENTURNS_TRENDFACTORY_HXX
#define OPENTURNS_TRENDFACTORY_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Field.hxx"
#include "openturns/TrendTransform.hxx"
#include "openturns/FittingAlgorithm.hxx"
#include "openturns/BasisSequenceFactory.hxx"
#include "openturns/CorrectedLeaveOneOut.hxx"
#include "openturns/LARS.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Basis.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class TrendFactory
 */

class OT_API TrendFactory
  : public PersistentObject
{

  CLASSNAME

public:

  /** Standard constructor  */
  explicit TrendFactory(const BasisSequenceFactory & basisSequenceFactory = LARS(),
                        const FittingAlgorithm & fittingAlgorithm = CorrectedLeaveOneOut());

  /** BasisSequenceFactory accessors */
  BasisSequenceFactory getBasisSequenceFactory() const;
  void setBasisSequenceFactory(const BasisSequenceFactory & basisSequenceFactory);

  /** FittingAlgorithm accessors */
  FittingAlgorithm getFittingAlgorithm() const;
  void setFittingAlgorithm(const FittingAlgorithm & fittingAlgorithm);

  /** Virtual copy constructor */
  TrendFactory * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Build the factory from data  */
  TrendTransform build(const Field & field,
                       const Basis & basis) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


private:

  /** BasisSequenceFactory argument - Default is LAR */
  BasisSequenceFactory basisSequenceFactory_;

  /** FittingAlgorithm used in evaluation */
  FittingAlgorithm fittingAlgorithm_;

} ; /* class TrendFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TRENDFACTORY_HXX */
