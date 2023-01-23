//                                               -*- C++ -*-
/**
 *  @brief Efficient implementation of the computation of the incomplete
 *         regularized gamma function and related functions
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/GammaFunctions.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/SpecFunc.hxx"
#ifdef OPENTURNS_HAVE_BOOST
#include <boost/math/special_functions/gamma.hpp>
#else
#include "openturns/incgam.hxx"
#endif

BEGIN_NAMESPACE_OPENTURNS

namespace GammaFunctions
{

Scalar IncompleteGamma(const Scalar a,
                       const Scalar x,
                       const Bool tail)
{
  if (!(a > 0.0)) throw InvalidArgumentException(HERE) << "Error: a must be positive, here a=" << a;
  return RegularizedIncompleteGamma(a, x, tail) * SpecFunc::Gamma(a);
}

Scalar IncompleteGammaInverse(const Scalar a,
                              const Scalar x,
                              const Bool tail)
{
  if (!(a > 0.0)) throw InvalidArgumentException(HERE) << "Error: a must be positive, here a=" << a;
  return RegularizedIncompleteGammaInverse(a, x / SpecFunc::Gamma(a), tail);
}

Scalar RegularizedIncompleteGamma(const Scalar a,
                                  const Scalar x,
                                  const Bool tail)
{
  if (!(a > 0.0)) throw InvalidArgumentException(HERE) << "Error: a must be positive, here a=" << a;
  if (x <= 0.0) return (tail ? 1.0 : 0.0);
#ifdef OPENTURNS_HAVE_BOOST
  return (tail ? boost::math::gamma_q(a, x) : boost::math::gamma_p(a, x));
#else
  Scalar p = -1.0;
  Scalar q = -1.0;
  SignedInteger ierr;
  incgam(a, x, p, q, ierr);
  if (ierr == 1) LOGWARN(OSS() << "underflow or overflow in RegularizedIncompleteGamma for a=" << a << ", x=" << x);
  return (tail ? q : p);
#endif
}

Scalar RegularizedIncompleteGammaInverse(const Scalar a,
    const Scalar x,
    const Bool tail)
{
  if (!(a > 0.0)) throw InvalidArgumentException(HERE) << "Error: a must be positive, here a=" << a;
#ifdef OPENTURNS_HAVE_BOOST
  try
  {
    return (tail ? boost::math::gamma_q_inv(a, x) : boost::math::gamma_p_inv(a, x));
  }
  catch (const boost::math::evaluation_error & ex)
  {
    throw InvalidArgumentException(HERE) << ex.what();
  }
#else
  const Scalar xC = std::max(x, 1e-80);
  const Scalar y = 0.5 + (0.5 - xC);
  Scalar xr = -1.0;
  SignedInteger ierr = 0;
  invincgam(a, (tail ? y : xC), (tail ? xC : y), xr, ierr);
  if (ierr == -1) LOGWARN(OSS() << "cannot compute the RegularizedIncompleteGammaInverse function to full precision for a=" << a << ", x=" << x << ", tail=" << tail << " because of an overflow.");
  if (ierr == -2) LOGWARN(OSS() << "up to 15 Newton iterations have been made to compute the RegularizedIncompleteGammaInverse function for a=" << a << ", x=" << x << ", tail=" << tail << ". The accuracy may be reduced.");
  return xr;
#endif
}

} // GammaFunctions

END_NAMESPACE_OPENTURNS
