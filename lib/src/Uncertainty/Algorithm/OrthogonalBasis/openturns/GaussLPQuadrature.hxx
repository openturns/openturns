//                                               -*- C++ -*-
/**
 *  @brief Gauss-LP quadrature rule builder
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

#ifndef OPENTURNS_GAUSSLPQUADRATURE_HXX
#define OPENTURNS_GAUSSLPQUADRATURE_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Function.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API GaussLPQuadrature
  : public PersistentObject
{
  CLASSNAME
public:

  typedef Collection<Function>           FunctionCollection;
  typedef PersistentCollection<Function> FunctionPersistentCollection;

  GaussLPQuadrature();

  GaussLPQuadrature(const FunctionCollection & orthonormalFunctions,
                    const Distribution & measure);

  GaussLPQuadrature * clone() const override;

  Sample build(const UnsignedInteger n,
               Point & weightsOut) const;

  void setOrthonormalFunctions(const FunctionCollection & functions);
  FunctionCollection getOrthonormalFunctions() const;

  void setMeasure(const Distribution & measure);
  Distribution getMeasure() const;

  String __repr__() const override;

  void save(Advocate & adv) const override;
  void load(Advocate & adv) override;

private:

  FunctionPersistentCollection orthonormalFunctions_;
  Distribution measure_;

};

END_NAMESPACE_OPENTURNS

#endif
