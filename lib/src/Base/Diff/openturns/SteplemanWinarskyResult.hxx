//                                               -*- C++ -*-
/**
 *  @brief SteplemanWinarskyResult stores the result of SteplemanWinarsky
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_STEPLEMANWINARSKYRESULT_HXX
#define OPENTURNS_STEPLEMANWINARSKYRESULT_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class SteplemanWinarskyResult
 * @brief Stores the optimal step computed by SteplemanWinarsky
 */
class OT_API SteplemanWinarskyResult
  : public PersistentObject
{
  CLASSNAME
public:

  /** Default constructor */
  SteplemanWinarskyResult();

  /** Parameters constructor */
  SteplemanWinarskyResult(const Sample & optimalStepCollection,
                          const Function & functionWithOptimalStep);

  /** Virtual constructor */
  SteplemanWinarskyResult * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Comparison operator */
  using PersistentObject::operator ==;
  Bool operator ==(const SteplemanWinarskyResult & other) const;

  /** Optimal step accessor */
  void setOptimalStepCollection(const Sample & optimalStepCollection);
  Sample getOptimalStepCollection() const;
  Point getOptimalStep(const UnsignedInteger marginalIndex = 0) const;

  /** Function with optimal finite-difference gradient accessor */
  Function getFunctionWithOptimalStep() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

 private:
  Sample optimalStepCollection_;
  Function functionWithOptimalStep_;

}; /* class SteplemanWinarskyResult */


END_NAMESPACE_OPENTURNS


#endif
