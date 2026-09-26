//                                               -*- C++ -*-
/**
 *  @file  HODLRMatrixFactory.cxx
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
#include "openturns/HODLRMatrixFactory.hxx"
#include "openturns/HODLRMatrix.hxx"
#include "openturns/HODLRMatrixImplementation.hxx"
#include "openturns/KDTree.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Log.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HODLRMatrixFactory)

static const Factory<HODLRMatrixFactory> Factory_HODLRMatrixFactory;

HODLRMatrixFactory::HODLRMatrixFactory()
  : PersistentObject()
{
}

HODLRMatrixFactory* HODLRMatrixFactory::clone() const
{
  return new HODLRMatrixFactory(*this);
}

HODLRMatrix
HODLRMatrixFactory::build(const Sample& sample, UnsignedInteger outputDimension,
                           Bool symmetric, const HODLRMatrixParameters& parameters)
{
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger n = outputDimension * size;
  LOGDEBUG(OSS() << "HODLRMatrixFactory::build n=" << n << " outputDim=" << outputDimension);

  // Create the implementation with correct size
  // Phase 1: build empty structure
  // Phase 2: caller does assemble(CovarianceAssemblyFunction, 'L')
  // Phase 3: caller does factorize()
  HODLRMatrixImplementation* impl = new HODLRMatrixImplementation();
  impl->n_ = n;
  impl->parameters_ = parameters;
  impl->symmetric_ = symmetric;

  // Reorder the vertices along a space-filling curve so that the recursive
  // split of the HODLR tree separates spatially close points at the leaves
  if (parameters.getUseSpatialOrdering())
  {
    const Indices order = KDTree(sample).getOrdering();
    Indices permutation(n);
    for (UnsignedInteger k = 0; k < size; ++k)
      for (UnsignedInteger d = 0; d < outputDimension; ++d)
        permutation[k * outputDimension + d] = order[k] * outputDimension + d;
    impl->setPermutation(permutation);
  }

  return HODLRMatrix(impl);
}

String HODLRMatrixFactory::__repr__() const
{
  return OSS() << "class=" << getClassName();
}

void HODLRMatrixFactory::save(Advocate& adv) const
{
  PersistentObject::save(adv);
}

void HODLRMatrixFactory::load(Advocate& adv)
{
  PersistentObject::load(adv);
}

END_NAMESPACE_OPENTURNS
