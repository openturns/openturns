//                                               -*- C++ -*-
/**
 *  @brief The UserDefinedPair class.
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
#ifndef OPENTURNS_USERDEFINEDPAIR_HXX
#define OPENTURNS_USERDEFINEDPAIR_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/PersistentObject.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class UserDefinedPair
 *
 * The UserDefinedPair class.
 */
class OT_API UserDefinedPair
  : public PersistentObject
{
  CLASSNAME;
public:

  UserDefinedPair();
  UserDefinedPair(const NumericalPoint & x,
                  const NumericalScalar p);

  virtual UserDefinedPair * clone() const;

  Bool operator == (const UserDefinedPair & other) const;

  void setX(const NumericalPoint & x);

  void setP(const NumericalScalar p);

  NumericalPoint getX() const;

  NumericalScalar getP() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

  virtual String __repr__() const;

private:

  /** Position of the modality */
  NumericalPoint x_;
  /** Probability of the modality */
  NumericalScalar p_;

}; /* end class UserDefinedPair */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_USERDEFINEDPAIR_HXX */
