//                                               -*- C++ -*-
/**
 *  @brief Quadratic canonical basis factory
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
#ifndef OPENTURNS_QUADRATICBASISFACTORY_HXX
#define OPENTURNS_QUADRATICBASISFACTORY_HXX

#include "openturns/BasisFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API QuadraticBasisFactory
  : public BasisFactory
{
  CLASSNAME
public:

  /** Default constructor */
  explicit QuadraticBasisFactory (const UnsignedInteger inputDimension = 1);

  /* String converter */
  OT::String __repr__() const;

  /* Here is the interface that all derived class must implement */

  /* Virtual constructor */
  virtual QuadraticBasisFactory * clone() const;

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
