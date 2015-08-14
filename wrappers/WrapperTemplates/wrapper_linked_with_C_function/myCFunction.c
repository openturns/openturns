/*                                               -*- C -*- */
/**
 *  @brief The myCFunction declaration file
 */

#include "myCFunction.h"

/* myCFuntion returns a non-null value when it fails */
int myCFunction( const double * X, unsigned long N, 
		 double * Y, unsigned long P )
{
  int i, j;

  for( j=0; j<P; ++j) {
    double s = (double) j;
    for( i=0; i<N; ++i) s += X[ i ];
    Y[ j ] = s;
  }

  return 0;
}

