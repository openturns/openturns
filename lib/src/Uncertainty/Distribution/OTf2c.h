
#ifndef OPENTURNS_F2C_H
#define OPENTURNS_F2C_H

#include "f2c.h"

#ifdef __cplusplus
extern "C" {
#endif

double pow_dd(doublereal *ap, doublereal *bp);

double pow_di(doublereal *ap, integer *bp);

double d_sign(doublereal *a, doublereal *b);

double d_mod(doublereal *x, doublereal *y);

#ifdef __cplusplus
}
#endif

#endif
