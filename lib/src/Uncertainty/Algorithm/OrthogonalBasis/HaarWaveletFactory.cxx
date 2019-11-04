//                                               -*- C++ -*-
/**
 *  @brief Haar wavelet function factory
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
#include "openturns/HaarWaveletFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/HaarWavelet.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(HaarWaveletFactory)

static const Factory<HaarWaveletFactory> Factory_HaarWaveletFactory;


/* Default constructor */
HaarWaveletFactory::HaarWaveletFactory()
  : OrthogonalUniVariateFunctionFactory(Uniform(0.0, 1.0))
{
  initializeCache();
}

/* Virtual constructor */
HaarWaveletFactory * HaarWaveletFactory::clone() const
{
  return new HaarWaveletFactory(*this);
}


/* String converter */
String HaarWaveletFactory::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " measure=" << measure_;
}

/* The method to get the function of any order */
UniVariateFunction HaarWaveletFactory::build(const UnsignedInteger order) const
{
  if (order == 0) return HaarWavelet(0, 0, true);
  const UnsignedInteger j = SpecFunc::Log2(order);
  const UnsignedInteger k = order - (1 << j);
  return HaarWavelet(j, k, false);
}


/* Method save() stores the object through the StorageManager */
void HaarWaveletFactory::save(Advocate & adv) const
{
  OrthogonalUniVariateFunctionFactory::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void HaarWaveletFactory::load(Advocate & adv)
{
  OrthogonalUniVariateFunctionFactory::load(adv);
}

END_NAMESPACE_OPENTURNS
