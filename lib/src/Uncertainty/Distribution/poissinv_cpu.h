/*

This software was written by Mike Giles, copyright University of Oxford, 
and is provided under the terms of the GNU GPLv3 license:
http://www.gnu.org/licenses/gpl.html

Commercial users who would like to use the software under a more
permissive license, such as BSD, should contact the author:
mike.giles@maths.ox.ac.uk


The code below requires the functions:

erfc    defined in the GCC math library
erfcinv defined in the next header file 

Used in OpenTURNS with the written permission of the Author, see COPYING.poissinv.

*/

#ifndef POISSINV_H
#define POISSINV_H

#ifdef __cplusplus
extern "C" {
#endif

/* Replace the initial implementation of erfinv by some OpenTURNS equivalent */
// #include <erfinv.h>
#include "openturns/DistFunc.hxx"


//
// This double precision function computes the inverse
// of the Poisson CDF using the vector algorithm
//
// u   = CDF value in range (0,1)
// lam = Poisson rate
//
// For lam < 1e15,  max |error| no more than 1
//  ave |error| < 1e-16*max(4,lam) for lam < 1e9
//              < 1e-6             for lam < 1e15
//
// For lam > 1e15, the errors will be about 1 ulp.
//

inline double poissinv_vector(double U, double Lam) {
  int    i;
  double X=0.0, Xi, T, Del, Rm, R, R2, S, S2, Eta, B0, B1;

// large lam

  if (Lam > 4.0) {

    S   = OT::DistFunc::qNormal(U)/sqrt(Lam);
    //    S   = - sqrt(2.0)*erfcinv(2.0*U)/sqrt(Lam);

    Del = 2.0e-6;

// use polynomial approximations in central region
 
    if ( (S>-0.6833501) && (S<1.777993) ) {;
                                             
//  polynomial approximation to f^{-1}(s) - 1
                                             
      Rm =  2.82298751e-07;                 
      Rm = -2.58136133e-06 + Rm*S;          
      Rm =  1.02118025e-05 + Rm*S;          
      Rm = -2.37996199e-05 + Rm*S;          
      Rm =  4.05347462e-05 + Rm*S;          
      Rm = -6.63730967e-05 + Rm*S;          
      Rm =  0.000124762566 + Rm*S;          
      Rm = -0.000256970731 + Rm*S;          
      Rm =  0.000558953132 + Rm*S;          
      Rm =  -0.00133129194 + Rm*S;          
      Rm =   0.00370367937 + Rm*S;          
      Rm =   -0.0138888706 + Rm*S;          
      Rm =     0.166666667 + Rm*S;          
      S +=                S*(Rm*S);
      Rm = S;
                                                
//  polynomial approximation to correction c0(r)
                                                
      T  =   1.86386867e-05;                   
      T  =  -0.000207319499 + T*Rm;            
      T  =     0.0009689451 + T*Rm;            
      T  =   -0.00247340054 + T*Rm;            
      T  =    0.00379952985 + T*Rm;            
      T  =   -0.00386717047 + T*Rm;            
      T  =    0.00346960934 + T*Rm;            
      T  =   -0.00414125511 + T*Rm;            
      T  =    0.00586752093 + T*Rm;            
      T  =   -0.00838583787 + T*Rm;            
      T  =     0.0132793933 + T*Rm;            
      T  =     -0.027775536 + T*Rm;            
      T  =      0.333333333 + T*Rm;            
                                   
//  O(1/lam) correction             
                                    
      X  =   -0.00014585224;       
      X  =    0.00146121529 + X*Rm;
      X  =   -0.00610328845 + X*Rm;
      X  =     0.0138117964 + X*Rm;
      X  =    -0.0186988746 + X*Rm;
      X  =     0.0168155118 + X*Rm;
      X  =     -0.013394797 + X*Rm;
      X  =     0.0135698573 + X*Rm;
      X  =    -0.0155377333 + X*Rm;
      X  =     0.0174065334 + X*Rm;
      X  =    -0.0198011178 + X*Rm;
      X  =  X / Lam;

//    sum from smallest to largest to minimise rounding error
//    rounding down final sum is important

      S = Lam + (((X+Del)+T)+Lam*S);
    }

// otherwise use Newton iteration

    else if (S > -sqrt(2.0)) {

      R = 1.0 + S;
      if (R<0.1) R = 0.1;

      do {
        T  = log(R);
        R2 = R;
        S2 = sqrt(2.0*(1.0 - R + R*T));
        if (R<1.0) S2 = -S2;
        R = R2 - (S2-S)*S2/T;
        if (R<0.1*R2) R = 0.1*R2;
      } while (fabs(R-R2)>1e-5);

      T   = log(R);
      S   = Lam*R + log(sqrt(2.0*R*(1.0-R+R*T))/fabs(R-1.0)) / T;
      S   = S - (8.2/405.0)/(S+0.025*Lam);
      Del = 0.01/S;
      S   = S + Del;
    }

// if x>10, round down to nearest integer, and check accuracy

    X = floor(S);

    if (S>10.0 && S<X+2.0*Del) {

// correction procedure based on Temme approximation (double precision)

      if (X>0.5*Lam && X<2.0*Lam) {

        Xi = 1.0 / X;
        Eta = X / Lam;
        Eta = sqrt(2.0*(1.0-Eta+Eta*log(Eta))/Eta);
        if (X>Lam) Eta = -Eta;

        B1 =  8.0995211567045583e-16;              S = B1;      
        B0 = -1.9752288294349411e-15;              S = B0 + S*Eta;
        B1 = -5.1391118342426808e-16 + 25.0*B1*Xi; S = B1 + S*Eta;
        B0 =  2.8534893807047458e-14 + 24.0*B0*Xi; S = B0 + S*Eta;
        B1 = -1.3923887224181616e-13 + 23.0*B1*Xi; S = B1 + S*Eta;
        B0 =  3.3717632624009806e-13 + 22.0*B0*Xi; S = B0 + S*Eta;
        B1 =  1.1004392031956284e-13 + 21.0*B1*Xi; S = B1 + S*Eta;
        B0 = -5.0276692801141763e-12 + 20.0*B0*Xi; S = B0 + S*Eta;
        B1 =  2.4361948020667402e-11 + 19.0*B1*Xi; S = B1 + S*Eta;
        B0 = -5.8307721325504166e-11 + 18.0*B0*Xi; S = B0 + S*Eta;
        B1 = -2.5514193994946487e-11 + 17.0*B1*Xi; S = B1 + S*Eta;
        B0 =  9.1476995822367933e-10 + 16.0*B0*Xi; S = B0 + S*Eta;
        B1 = -4.3820360184533521e-09 + 15.0*B1*Xi; S = B1 + S*Eta;
        B0 =  1.0261809784240299e-08 + 14.0*B0*Xi; S = B0 + S*Eta;
        B1 =  6.7078535434015332e-09 + 13.0*B1*Xi; S = B1 + S*Eta;
        B0 = -1.7665952736826086e-07 + 12.0*B0*Xi; S = B0 + S*Eta;
        B1 =  8.2967113409530833e-07 + 11.0*B1*Xi; S = B1 + S*Eta;
        B0 = -1.8540622107151585e-06 + 10.0*B0*Xi; S = B0 + S*Eta;
        B1 = -2.1854485106799979e-06 +  9.0*B1*Xi; S = B1 + S*Eta;
        B0 =  3.9192631785224383e-05 +  8.0*B0*Xi; S = B0 + S*Eta;
        B1 = -0.00017875514403292177 +  7.0*B1*Xi; S = B1 + S*Eta;
        B0 =  0.00035273368606701921 +  6.0*B0*Xi; S = B0 + S*Eta;
        B1 =   0.0011574074074074078 +  5.0*B1*Xi; S = B1 + S*Eta;
        B0 =   -0.014814814814814815 +  4.0*B0*Xi; S = B0 + S*Eta;
        B1 =    0.083333333333333329 +  3.0*B1*Xi; S = B1 + S*Eta;
        B0 =    -0.33333333333333331 +  2.0*B0*Xi; S = B0 + S*Eta;
        S  = S / (1.0 + B1*Xi);

        S = S*exp(-0.5*X*Eta*Eta)/sqrt(2.0*3.141592653589793*X);
        if (X<Lam) {
          S += 0.5*erfc(Eta*sqrt(0.5*X));
          if (S > U) X -= 1.0;
        }
        else {
          S -= 0.5*erfc(-Eta*sqrt(0.5*X));
          if (S > U-1.0) X -= 1.0;
        }
      }

// sum downwards or upwards

      else {
        Xi = 1.0 / X;
        S = - (691.0/360360.0);
        S =   (1.0/1188.0) + S*Xi*Xi;
        S = - (1.0/1680.0) + S*Xi*Xi;
        S =   (1.0/1260.0) + S*Xi*Xi;
        S = - (1.0/360.0)  + S*Xi*Xi;
        S =   (1.0/12.0)   + S*Xi*Xi;
        S =                  S*Xi;
        S = (X - Lam) - X*log(X/Lam) - S;

        if (X<Lam) {
          T  = exp(-0.5*S);
          S  = 1.0 - T*(U*T) * sqrt(2.0*3.141592653589793*Xi) * Lam;
          T  = 1.0;
          Xi = X;
          for (i=1; i<50; i++) {
            Xi -= 1.0;
            T  *= Xi/Lam;
            S  += T;
          }
          if (S > 0.0) X -= 1.0;
        }

        else {
          T  = exp(-0.5*S);
          S  = 1.0 - T*((1.0-U)*T) * sqrt(2.0*3.141592653589793*X);
          Xi = X;
          for (i=0; i<50; i++) {
            Xi += 1.0;
            S   = S*Xi/Lam + 1.0;
          }
          if (S < 0.0) X -= 1.0;
        }
      }
    }
  }

// bottom-up summation

  if (X<10.0) {
    X   = 0.0;
    T   = exp(0.5*Lam);
    Del = 0.0;
    if (U>0.5) Del = T*(1e-13*T);
    S   = 1.0 - T*(U*T) + Del;

    while (S<0.0) {
      X  += 1.0;
      T   = X/Lam;
      Del = T*Del;
      S   = T*S + 1.0;
    }

// top-down summation if needed

    if (S < 2.0*Del) {
      Del = 1e13*Del;
      T   = 1e15*Del;

      while (Del<T) {
        X   += 1.0;
        Del *= X/Lam;
      }

      S = (1.0-U)*Del;
      T = 1.0;
      while (S>0.0) {
        T *= X/Lam;
        S -= T;
        X -= 1.0;
      }
    }
  }

  return X;
}

//
// This double precision function computes the inverse
// of the Poisson CDF using the scalar algorithm
//
// u   = CDF value in range (0,1)
// lam = Poisson rate
//
// For lam < 1e15,  max |error| no more than 1
//  ave |error| < 1e-16*max(4,lam) for lam < 1e9
//              < 1e-6             for lam < 1e15
//
// For lam > 1e15, the errors will be about 1 ulp.
//

inline double poissinv_scalar(double U, double Lam) {
  int    i;
  double X=0.0, Xi, W, T, Del, R, R2, S, S2, Eta, B0, B1;

// large lam

  if (Lam > 4.0) {

    W = OT::DistFunc::qNormal(U);
    //    W = - sqrt(2.0)*erfcinv(2.0*U);

// use polynomial approximations in central region
 
    if ( fabs(W)<3.0 ) {;
      double Lam_root = sqrt(Lam);

      S = Lam_root*W + (1.0/3.0 + (1.0/6.0)*W*W)*(1.0 - W/(12.0*Lam_root));

      Del = (1.0 /160.0);
      Del = (1.0 / 80.0) + Del*(W*W);
      Del = (1.0 / 40.0) + Del*(W*W);
      Del = Del / Lam;

      S = Lam + (S + Del);
    }

// otherwise use Newton iteration

    else {
      S = W / sqrt(Lam);
      R = 1.0 + S;
      if (R<0.1) R = 0.1;

      do {
        T  = log(R);
        R2 = R;
        S2 = sqrt(2.0*((1.0-R) + R*T));
        if (R<1.0) S2 = -S2;
        R = R2 - (S2-S)*S2/T;
        if (R<0.1*R2) R = 0.1*R2;
      } while (fabs(R-R2)>1e-8);

      T   = log(R);
      S   = Lam*R + log(sqrt(2.0*R*((1.0-R)+R*T))/fabs(R-1.0)) / T;
      S   = S - 0.0218/(S+0.065*Lam);
      Del = 0.01/S;
      S   = S + Del;
    }

// if x>10, round down to nearest integer, and check accuracy

    X = floor(S);

    if (S>10.0 && S<X+2.0*Del) {

// correction procedure based on Temme approximation (double precision)

      if (X>0.5*Lam && X<2.0*Lam) {

        Xi = 1.0 / X;
        Eta = X / Lam;
        Eta = sqrt(2.0*(1.0-Eta+Eta*log(Eta))/Eta);
        if (X>Lam) Eta = -Eta;

        B1 =  8.0995211567045583e-16;              S = B1;      
        B0 = -1.9752288294349411e-15;              S = B0 + S*Eta;
        B1 = -5.1391118342426808e-16 + 25.0*B1*Xi; S = B1 + S*Eta;
        B0 =  2.8534893807047458e-14 + 24.0*B0*Xi; S = B0 + S*Eta;
        B1 = -1.3923887224181616e-13 + 23.0*B1*Xi; S = B1 + S*Eta;
        B0 =  3.3717632624009806e-13 + 22.0*B0*Xi; S = B0 + S*Eta;
        B1 =  1.1004392031956284e-13 + 21.0*B1*Xi; S = B1 + S*Eta;
        B0 = -5.0276692801141763e-12 + 20.0*B0*Xi; S = B0 + S*Eta;
        B1 =  2.4361948020667402e-11 + 19.0*B1*Xi; S = B1 + S*Eta;
        B0 = -5.8307721325504166e-11 + 18.0*B0*Xi; S = B0 + S*Eta;
        B1 = -2.5514193994946487e-11 + 17.0*B1*Xi; S = B1 + S*Eta;
        B0 =  9.1476995822367933e-10 + 16.0*B0*Xi; S = B0 + S*Eta;
        B1 = -4.3820360184533521e-09 + 15.0*B1*Xi; S = B1 + S*Eta;
        B0 =  1.0261809784240299e-08 + 14.0*B0*Xi; S = B0 + S*Eta;
        B1 =  6.7078535434015332e-09 + 13.0*B1*Xi; S = B1 + S*Eta;
        B0 = -1.7665952736826086e-07 + 12.0*B0*Xi; S = B0 + S*Eta;
        B1 =  8.2967113409530833e-07 + 11.0*B1*Xi; S = B1 + S*Eta;
        B0 = -1.8540622107151585e-06 + 10.0*B0*Xi; S = B0 + S*Eta;
        B1 = -2.1854485106799979e-06 +  9.0*B1*Xi; S = B1 + S*Eta;
        B0 =  3.9192631785224383e-05 +  8.0*B0*Xi; S = B0 + S*Eta;
        B1 = -0.00017875514403292177 +  7.0*B1*Xi; S = B1 + S*Eta;
        B0 =  0.00035273368606701921 +  6.0*B0*Xi; S = B0 + S*Eta;
        B1 =   0.0011574074074074078 +  5.0*B1*Xi; S = B1 + S*Eta;
        B0 =   -0.014814814814814815 +  4.0*B0*Xi; S = B0 + S*Eta;
        B1 =    0.083333333333333329 +  3.0*B1*Xi; S = B1 + S*Eta;
        B0 =    -0.33333333333333331 +  2.0*B0*Xi; S = B0 + S*Eta;
        S  = S / (1.0 + B1*Xi);

        S = S*exp(-0.5*X*Eta*Eta)/sqrt(2.0*3.141592653589793*X);
        if (X<Lam) {
          S += 0.5*erfc(Eta*sqrt(0.5*X));
          if (S > U) X -= 1.0;
        }
        else {
          S -= 0.5*erfc(-Eta*sqrt(0.5*X));
          if (S > U-1.0) X -= 1.0;
        }
      }

// sum downwards or upwards

      else {
        Xi = 1.0 / X;
        S = - (691.0/360360.0);
        S =   (1.0/1188.0) + S*Xi*Xi;
        S = - (1.0/1680.0) + S*Xi*Xi;
        S =   (1.0/1260.0) + S*Xi*Xi;
        S = - (1.0/360.0)  + S*Xi*Xi;
        S =   (1.0/12.0)   + S*Xi*Xi;
        S =                  S*Xi;
        S = (X - Lam) - X*log(X/Lam) - S;

        if (X<Lam) {
          T  = exp(-0.5*S);
          S  = 1.0 - T*(U*T) * sqrt(2.0*3.141592653589793*Xi) * Lam;
          T  = 1.0;
          Xi = X;
          for (i=1; i<50; i++) {
            Xi -= 1.0;
            T  *= Xi/Lam;
            S  += T;
          }
          if (S > 0.0) X -= 1.0;
        }

        else {
          T  = exp(-0.5*S);
          S  = 1.0 - T*((1.0-U)*T) * sqrt(2.0*3.141592653589793*X);
          Xi = X;
          for (i=0; i<50; i++) {
            Xi += 1.0;
            S   = S*Xi/Lam + 1.0;
          }
          if (S < 0.0) X -= 1.0;
        }
      }
    }
  }

// bottom-up summation

  if (X<10.0) {
    X   = 0.0;
    T   = exp(0.5*Lam);
    Del = 0.0;
    if (U>0.5) Del = T*(1e-13*T);
    S   = 1.0 - T*(U*T) + Del;

    while (S<0.0) {
      X  += 1.0;
      T   = X/Lam;
      Del = T*Del;
      S   = T*S + 1.0;
    }

// top-down summation if needed

    if (S < 2.0*Del) {
      Del = 1e13*Del;
      T   = 1e15*Del;

      while (Del<T) {
        X   += 1.0;
        Del *= X/Lam;
      }

      S = (1.0-U)*Del;
      T = 1.0;
      while (S>0.0) {
        T *= X/Lam;
        S -= T;
        X -= 1.0;
      }
    }
  }

  return X;
}

#ifdef __cplusplus
}
#endif

#endif
