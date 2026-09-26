//                                               -*- C++ -*-
/**
 *  @file  HODLRMatrixFactory.hxx
 *  @brief Factory for HODLR compressed matrices
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
 */
#ifndef OPENTURNS_HODLRMATRIXFACTORY_HXX
#define OPENTURNS_HODLRMATRIXFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/HODLRMatrixParameters.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

class HODLRMatrix;

class OT_API HODLRMatrixFactory
  : public PersistentObject
{
  CLASSNAME

public:
  HODLRMatrixFactory();
  HODLRMatrixFactory * clone() const override;

  /**
   * Build an empty HODLRMatrix of the correct size.
   * The caller must then call assemble() + factorize() on the result.
   */
  HODLRMatrix build(const Sample & sample, UnsignedInteger outputDimension,
                    Bool symmetric, const HODLRMatrixParameters & parameters = HODLRMatrixParameters());

  String __repr__() const override;

  void save(Advocate & adv) const override;
  void load(Advocate & adv) override;
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HODLRMATRIXFACTORY_HXX */
