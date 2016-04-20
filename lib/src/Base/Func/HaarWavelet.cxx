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
  , a_(0.0)
  , m_(0.5)
  , b_(1.0)
  , value_(1.0)
{
  // Nothing to do
  if (!isScaling)
    {
      const NumericalScalar denominator = 1 << j;
      value_ = std::sqrt(denominator);
      a_ = k / denominator;
      m_ = (k + 0.5) / denominator;
      b_ = (k + 1.0) / denominator;
    }
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
                   << " isScaling=" << isScaling_
                   << " a=" << a_
                   << " m=" << m_
                   << " b=" << b_;
}


String HaarWavelet::__str__(const String & offset) const
{
  OSS oss(false);
  if (isScaling_)
    oss << offset << "f:X -> {1.0 for 0.0<=X<1.0, 0.0 elsewhere}";
  else
    oss << offset << "f:X -> {" << value_ << " for " << a_ << "<=X<" << m_ << ", " << -value_ << " for " << m_ << "<=X<" << b_ << ", 0.0 elsewhere}";
  return oss;
}


/* HaarWavelet are evaluated as functors */
NumericalScalar HaarWavelet::operator() (const NumericalScalar x) const
{
  if (isScaling_) return ((x < 0.0 || x > 1.0) ? 0.0 : 1.0);
  if (x < a_) return 0.0;
  if (x < m_) return value_;
  if (x < b_) return -value_;
  return 0.0;
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
  *this = HaarWavelet(j_, k_, isScaling_);
}


END_NAMESPACE_OPENTURNS
