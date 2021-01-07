//                                               -*- C++ -*-
/**
 *  @brief Efficient implementation of the exponential integral function
 *         for complex arguments
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
#include <cmath>

#include "openturns/ExponentialIntegralFunctions.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

/* Algorithms to compute the Exponential Integral function as described in:
   Vincent Pegoraro and Philipp Slusallek, On the Evaluation of the Complex-Valued Exponential Integral, Journal of Graphics, GPU, and Game Tools, 15(3), 183-198, 2011.
*/
namespace ExponentialIntegralFunctions
{

Bool hasConverged(const Complex & current,
                  const Complex & previous)
{
  return (std::abs(std::real(current) - std::real(previous)) <= SpecFunc::Precision * std::abs(std::real(current))) && (std::abs(std::imag(current) - std::imag(previous)) <= SpecFunc::Precision * std::abs(std::imag(current)));
}

inline Scalar sign(const Scalar x)
{
  return (x > 0.0) - (x < 0.0);
}

Complex Ei(const Complex & z)
{
  const Scalar absZ = std::abs(z);
  if (absZ >= SpecFunc::LogMaxScalar) return std::exp(z) / z + Complex(0.0, sign(std::imag(z)) * M_PI);
  if (absZ > 2.0 - 1.035 * std::log(SpecFunc::Precision)) return EiAsymptoticSeries(z);
  if ((absZ > 1.0) && ((std::real(z) < 0.0) || (std::abs(std::imag(z)) > 1.0))) return EiContinuedFractionForward(z);
  if (absZ > 0.0) return EiPowerSeries(z);
  return SpecFunc::LogMinScalar;
}

Scalar Ei(const Scalar z)
{
  return std::real(Ei(Complex(z, 0.0)));
}

Complex EiPowerSeries(const Complex & z)
{
  Complex ei(SpecFunc::EulerConstant + std::log(std::abs(z)), std::abs(std::arg(z)) * sign(std::imag(z)));
  Complex tmp(1.0);
  for (UnsignedInteger k = 1; k < SpecFunc::MaximumIteration; ++k)
  {
    tmp *= z / static_cast<Scalar>(k);
    const Complex old(ei);
    ei += tmp / static_cast<Scalar>(k);
    if (hasConverged(ei, old)) break;
  }
  return ei;
}

Scalar EiPowerSeries(const Scalar z)
{
  return real(EiPowerSeries(Complex(z, 0.0)));
}

Complex EiAsymptoticSeries(const Complex & z)
{
  Complex ei(0.0, sign(std::imag(z) * M_PI));
  Complex tmp(std::exp(z) / z);
  for (UnsignedInteger k = 1; k <= std::floor(std::abs(z)) + 1; ++k)
  {
    const Complex old(ei);
    ei += tmp;
    if (hasConverged(ei, old)) break;
    tmp *= static_cast<Scalar>(k) / z;
  }
  return ei;
}

Scalar EiAsymptoticSeries(const Scalar z)
{
  return std::real(EiAsymptoticSeries(Complex(z, 0.0)));
}

Complex EiContinuedFractionBackward(const Complex & z)
{
  Complex ei(0.0);
  for (UnsignedInteger k = SpecFunc::MaximumIteration; k >= 1; --k)
    ei = - static_cast<Scalar>(k) / (2.0 + (1.0 - z + ei) / static_cast<Scalar>(k));
  return -std::exp(z) / (1.0 - z + ei) + Complex(0.0, sign(std::imag(z)));
}

Scalar EiContinuedFractionBackward(const Scalar z)
{
  return std::real(EiContinuedFractionBackward(Complex(z, 0.0)));
}

Complex EiContinuedFractionForward(const Complex & z)
{
  Complex ei(0.0, sign(std::imag(z)) * M_PI);
  Complex c(0.0);
  Complex d(1.0 / (1.0 - z));
  if (ei != 0.0)
  {
    c = 1.0 / (1.0 - z - std::exp(z) / ei);
    ei *= d / c;
  }
  else
  {
    ei = -d * std::exp(z);
  }
  for (UnsignedInteger k = 1; k <= SpecFunc::MaximumIteration; ++k)
  {
    const Scalar l = 2 * k + 1;
    const Scalar k2 = k * k;
    c = 1.0 / (l - z - k2 * c);
    d = 1.0 / (l - z - k2 * d);
    const Complex old(ei);
    ei *= d / c;
    if (hasConverged(ei, old)) break;
  }
  return ei;
}

Scalar EiContinuedFractionForward(const Scalar z)
{
  return std::real(EiContinuedFractionBackward(Complex(z, 0.0)));
}


} // ExponentialIntegralFunctions

END_NAMESPACE_OPENTURNS
