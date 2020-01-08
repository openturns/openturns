//                                               -*- C++ -*-
/**
 *  @brief Multiple FORM result
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_MULTIFORMRESULT_HXX
#define OPENTURNS_MULTIFORMRESULT_HXX

#include "FORMResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
  * @class MultiFORMResult
  * Multiple FORM result
  */
class OT_API MultiFORMResult
  : public PersistentObject
{
  CLASSNAME;
public:
  typedef Collection<FORMResult> FORMResultCollection;
  typedef PersistentCollection<FORMResult> FORMResultPersistentCollection;

  /** Default constructor */
  MultiFORMResult();

  /** Collection constructor */
  explicit MultiFORMResult(const FORMResultCollection & collection);

  /** Virtual constructor */
  virtual MultiFORMResult * clone() const;

  /** String converter */
  String __repr__() const;

  /** EventProbability accessor */
  void setEventProbability(const Scalar eventProbability);
  Scalar getEventProbability() const;

  /** Generalized reliability index */
  Scalar getGeneralisedReliabilityIndex() const;

  /** FORM result accessor */
  FORMResultCollection getFORMResultCollection() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:
  Scalar eventProbability_;
  FORMResultPersistentCollection formResultCollection_;

}; // class MultiFORMResult


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MULTIFORMRESULT_HXX */



