//                                               -*- C++ -*-
/**
 *  @brief Efficient implementation of the computation of the incomplete
 *         regularized gamma function and related functions
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
#ifndef OPENTURNS_INCGAM_HXX
#define OPENTURNS_INCGAM_HXX

#include "OTprivate.hxx"
#include "NumericalPoint.hxx"

BEGIN_NAMESPACE_OPENTURNS

namespace GammaFunctions
{
void incgam(const NumericalScalar a,
            const NumericalScalar x,
            NumericalScalar & p,
            NumericalScalar & q,
            SignedInteger & ierr);

void invincgam(const NumericalScalar a,
               const NumericalScalar p,
               const NumericalScalar q,
               NumericalScalar & xr,
               SignedInteger & ierr);

// computes (exp(x)-1)/x;
NumericalScalar exmin1(const NumericalScalar x);

// computes (exp(x)-1-x)/(0.5*x*x)
NumericalScalar exmin1minx(const NumericalScalar x);

// computes ln(1+x)-x
NumericalScalar lnec(const NumericalScalar x);

// partition the domain of the incomplete gamma function
NumericalScalar alpha(const NumericalScalar x);

// dompart is approx. of  x^a * exp(-x) / gamma(a+1) ;
NumericalScalar dompart(const NumericalScalar a,
                        const NumericalScalar x,
                        const Bool qt);

// a[0]/2+a[1]T1(x)+...a[n]Tn(x); series of Chebychev polynomials
NumericalScalar chepolsum(const NumericalScalar x,
                          const NumericalPoint & a);

// function g in 1/gamma(x+1) = 1+x*(x-1)*g(x), -1 <= x <= 1
NumericalScalar auxgam(const NumericalScalar x);

// gamstar(x) = exp(stirling(x)), x > 0; or
// gamma(x)/(exp(-x+(x-0.5)*ln(x))/sqrt(2pi)
NumericalScalar gamstar(const NumericalScalar x);

NumericalScalar errorfunction(const NumericalScalar x,
                              const Bool erfcc,
                              const Bool expo);

NumericalScalar fractio(const NumericalScalar x,
                        const NumericalPoint & r,
                        const NumericalPoint & s);

NumericalScalar pqasymp(const NumericalScalar a,
                        const NumericalScalar x,
                        const NumericalScalar dp,
                        const Bool p);

NumericalScalar saeta(const NumericalScalar a,
                      const NumericalScalar eta);

NumericalScalar qfraction(const NumericalScalar a,
                          const NumericalScalar x,
                          const NumericalScalar dp);

NumericalScalar qtaylor(const NumericalScalar a,
                        const NumericalScalar x,
                        const NumericalScalar dp);

NumericalScalar ptaylor(const NumericalScalar a,
                        const NumericalScalar x,
                        const NumericalScalar dp);

NumericalScalar eps1(const NumericalScalar eta);

NumericalScalar eps2(const NumericalScalar eta);

NumericalScalar eps3(const NumericalScalar eta);

NumericalScalar lambdaeta(const NumericalScalar eta);

NumericalScalar invq(const NumericalScalar x);

NumericalScalar inverfc(const NumericalScalar x);

NumericalScalar ratfun(const NumericalScalar x,
                       const NumericalPoint & ak,
                       const NumericalPoint & bk);

} // GammaFunctions

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INCGAM_HXX */
