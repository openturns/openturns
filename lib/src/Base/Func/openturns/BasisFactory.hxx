//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all BasisFactory
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_BASISFACTORY_HXX
#define OPENTURNS_BASISFACTORY_HXX

#include "openturns/Basis.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API BasisFactory
  : public PersistentObject
{
  CLASSNAME
public:

  /** Default constructor */
  BasisFactory();

  /* String converter */
  OT::String __repr__() const;

  /* Here is the interface that all derived class must implement */

  /* Virtual constructor */
  virtual BasisFactory * clone() const;

  /** @copydoc BasisFactory::build() const */
  virtual Basis build() const;

};

END_NAMESPACE_OPENTURNS

#endif
