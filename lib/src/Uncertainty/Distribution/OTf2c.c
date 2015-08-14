
#include "math.h"
#include "OTf2c.h"

double pow_dd(doublereal *ap, doublereal *bp)
{
  return (pow(*ap, *bp));
}

double pow_di(doublereal *ap, integer *bp)
{
  double pow = 1.;
  double x = *ap;
  integer n = *bp;
  unsigned long u;

  pow = 1.;
  x = *ap;
  n = *bp;

  if(n != 0)
  {
    if(n < 0)
    {
      n = -n;
      x = 1./x;
    }
    for(u = n; ; )
    {
      if(u & 01)
        pow *= x;
      if(u >>= 1)
        x *= x;
      else
        break;
    }
  }
  return(pow);
}

double d_sign(doublereal *a, doublereal *b)
{
  double x = (*a >= 0 ? *a : - *a);
  return( *b >= 0 ? x : -x);
}

double d_mod(doublereal *x, doublereal *y)
{
#ifdef IEEE_drem
        double xa, ya, z;
        if ((ya = *y) < 0.)
                ya = -ya;
        z = drem(xa = *x, ya);
        if (xa > 0) {
                if (z < 0)
                        z += ya;
                }
        else if (z > 0)
                z -= ya;
        return z;
#else
        double quotient;
        if( (quotient = *x / *y) >= 0)
                quotient = floor(quotient);
        else
                quotient = -floor(-quotient);
        return(*x - (*y) * quotient );
#endif
}

