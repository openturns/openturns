//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all BasisFactory
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */
#ifndef OPENTURNS_BASISFACTORY_HXX
#define OPENTURNS_BASISFACTORY_HXX

#include "Basis.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API BasisFactory
  : public PersistentObject
{
  CLASSNAME;
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
