//                                               -*- C++ -*-
/**
 *  @brief Efficient implementation of the computation of the incomplete
 *         regularized gamma function and related functions
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_INCGAM_HXX
#define OPENTURNS_INCGAM_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

namespace GammaFunctions
{
void incgam(const Scalar a,
            const Scalar x,
            Scalar & p,
            Scalar & q,
            SignedInteger & ierr);

void invincgam(const Scalar a,
               const Scalar p,
               const Scalar q,
               Scalar & xr,
               SignedInteger & ierr);

// computes (exp(x)-1)/x;
Scalar exmin1(const Scalar x);

// computes (exp(x)-1-x)/(0.5*x*x)
Scalar exmin1minx(const Scalar x);

// computes ln(1+x)-x
Scalar lnec(const Scalar x);

// partition the domain of the incomplete gamma function
Scalar alpha(const Scalar x);

// dompart is approx. of  x^a * exp(-x) / gamma(a+1) ;
Scalar dompart(const Scalar a,
               const Scalar x,
               const Bool qt);

// a[0]/2+a[1]T1(x)+...a[n]Tn(x); series of Chebychev polynomials
Scalar chepolsum(const Scalar x,
                 const Point & a);

// function g in 1/gamma(x+1) = 1+x*(x-1)*g(x), -1 <= x <= 1
Scalar auxgam(const Scalar x);

// gamstar(x) = exp(stirling(x)), x > 0; or
// gamma(x)/(exp(-x+(x-0.5)*ln(x))/sqrt(2pi)
Scalar gamstar(const Scalar x);

Scalar errorfunction(const Scalar x,
                     const Bool erfcc,
                     const Bool expo);

Scalar fractio(const Scalar x,
               const Point & r,
               const Point & s);

Scalar pqasymp(const Scalar a,
               const Scalar x,
               const Scalar dp,
               const Bool p);

Scalar saeta(const Scalar a,
             const Scalar eta);

Scalar qfraction(const Scalar a,
                 const Scalar x,
                 const Scalar dp);

Scalar qtaylor(const Scalar a,
               const Scalar x,
               const Scalar dp);

Scalar ptaylor(const Scalar a,
               const Scalar x,
               const Scalar dp);

Scalar eps1(const Scalar eta);

Scalar eps2(const Scalar eta);

Scalar eps3(const Scalar eta);

Scalar lambdaeta(const Scalar eta);

Scalar invq(const Scalar x);

Scalar inverfc(const Scalar x);

Scalar ratfun(const Scalar x,
              const Point & ak,
              const Point & bk);

} // GammaFunctions

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INCGAM_HXX */
