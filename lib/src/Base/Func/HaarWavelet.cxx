//                                               -*- C++ -*-
/**
 *  @brief Haar wavelet function implementation
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
#include "openturns/HaarWavelet.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HaarWavelet);

static const Factory<HaarWavelet> RegisteredFactory;


/* Parameter constructor */
HaarWavelet::HaarWavelet(const UnsignedInteger j,
                                         const UnsignedInteger k,
					 const Bool isScaling)
  : UniVariateFunctionImplementation()
  , j_(j)
  , k_(k)
  , isScaling_(isScaling)
{
  // Nothing to do
}


/* Virtual constructor */
HaarWavelet * HaarWavelet::clone() const
{
  return new HaarWavelet(*this);
}


/* String converter */
String HaarWavelet::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
                   << " j=" << j_
                   << " k=" << k_
                   << " isScaling=" << isScaling_;
}


String HaarWavelet::__str__(const String & offset) const
{
  OSS oss(false);
  if (isScaling_)
    oss << offset << "f:X -> {1.0 for -0.5<=X<=0.5, 0.0 elsewhere}";
  else
    {
      const NumericalScalar a1 = 1.0 * k_ / (1 << j_) - 0.5;
      const NumericalScalar b1 = a1 + 1.0 / (1 << (j_ + 1));
      const NumericalScalar am1 = b1;
      const NumericalScalar bm1 = 1.0 * (k_ + 1) / (1 << j_) - 0.5;
      oss << offset << "f:X -> {1.0 for " << a1 << "<=X<" << b1 << ", -1.0 for " << am1 << "<X<=" << bm1 << ", 0.0 elsewhere}";
    }
  return oss;
}


/* HaarWavelet are evaluated as functors */
NumericalScalar HaarWavelet::operator() (const NumericalScalar x) const
{
  if (isScaling_) return (std::abs(x) > 0.5 ? 0.0 : 1.0);
  const NumericalScalar a1 = 1.0 * k_ / (1 << j_) - 0.5;
  const NumericalScalar b1 = a1 + 1.0 / (1 << (j_ + 1));
  const NumericalScalar am1 = b1;
  const NumericalScalar bm1 = 1.0 * (k_ + 1) / (1 << j_) - 0.5;
  NumericalScalar value = 0.;
  if ((a1 <= x) && (x < b1)) value = 1.0;
  else if ((am1 < x) && (x <= bm1)) value = -1.0;
  return value;
}

/* HaarWavelet gradient */
NumericalScalar HaarWavelet::gradient(const NumericalScalar x) const
{
  return 0.0;
}

/* HaarWavelet hessian */
NumericalScalar HaarWavelet::hessian(const NumericalScalar x) const
{
  return 0.0;
}

/* Method save() stores the object through the StorageManager */
void HaarWavelet::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("j_", j_);
  adv.saveAttribute("k_", k_);
  adv.saveAttribute("isScaling_", isScaling_);
}

/* Method load() reloads the object from the StorageManager */
void HaarWavelet::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("j_", j_);
  adv.loadAttribute("k_", k_);
  adv.loadAttribute("isScaling_", isScaling_);
}


END_NAMESPACE_OPENTURNS
