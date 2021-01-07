//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Hamming.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Hamming
 */

CLASSNAMEINIT(Hamming)

static const Factory<Hamming> Factory_Hamming;

/* Constructor with parameters */
Hamming::Hamming()
  : FilteringWindowsImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
Hamming * Hamming::clone() const
{
  return new Hamming(*this);
}

/* String converter */
String Hamming::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Hamming::GetClassName();
  return oss;
}

/* String converter */
String Hamming::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << Hamming::GetClassName();
  return oss;
}

/* Evaluation of Hamming filter */
Scalar Hamming::operator()(const Scalar t) const
{
  if ((t < 0.0) || (t > 1.0))
    return 0.0;

  // Classical hamming is hamming(t) = 25/46 - 21/46 * cos(2 * \pi * t)
  // It allows to cancel the largest side-lob of the filter
  // Here, it is normalized such that int_0^1 (alpha*hamming(t))^2 dt = 1
  // It leads to alpha = sqrt(4232/1691) = 1.581980134910090405941101
  // 25/46 = 0.5434782608695652173913043
  // 21/46 = 0.4565217391304347826086957
  return 1.581980134910090405941101 * (0.5434782608695652173913043 - 0.4565217391304347826086957 * cos(2.0 * M_PI * t));
}

/* Method save() stores the object through the StorageManager */
void Hamming::save(Advocate & adv) const
{
  FilteringWindowsImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void Hamming::load(Advocate & adv)
{
  FilteringWindowsImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
