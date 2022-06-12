//                                               -*- C++ -*-
/**
 *  @brief The class IdFactory builds unique Ids for all Objects
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
#ifndef OPENTURNS_IDFACTORY_HXX
#define OPENTURNS_IDFACTORY_HXX

#include <iostream>              // for std::ostream
#include "openturns/OTprivate.hxx"
#include "openturns/AtomicInt.hxx"
#include "openturns/OStream.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class IdFactory
 *
 * @brief %IdFactory builds unique Ids for all PersistentObjects
 * @see PersistentObject
 * @see StorageManager
 *
 * The IdFactory ensures that each PersistentObject has an unique Id so they can be
 * distinguished from each other.
 * The IdFactory implements a 'singleton' strategy.
 */

class OT_API IdFactory
{
public:

  /** Id accessor
   *
   * The way to get an Id for a new PersistentObject.
   */
  static Id BuildId();

private:
  /** Disable default constructors */
  IdFactory();
  IdFactory(const IdFactory& other);
  IdFactory & operator=(const IdFactory& other);
}; /* class IdFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_IDFACTORY_HXX */
