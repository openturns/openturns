//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all LinearBasisFactory
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
#ifndef OPENTURNS_LINEARBASISFACTORY_HXX
#define OPENTURNS_LINEARBASISFACTORY_HXX

#include "BasisFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API LinearBasisFactory
  : public BasisFactory
{
  CLASSNAME;
public:

  /** Default constructor */
  LinearBasisFactory (const UnsignedInteger inputDimension = 1);

  /* String converter */
  OT::String __repr__() const;

  /* Here is the interface that all derived class must implement */

  /* Virtual constructor */
  virtual LinearBasisFactory * clone() const;

  /** @copydoc BasisFactory::build() const */
  virtual Basis build() const;

  /* Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /* Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  UnsignedInteger inputDimension_;
};

END_NAMESPACE_OPENTURNS

#endif
