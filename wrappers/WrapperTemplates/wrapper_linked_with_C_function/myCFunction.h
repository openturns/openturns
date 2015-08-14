/*                                               -*- C -*- */
/**
 *  @brief The header file for myCFunction
 */

#ifndef MYCFUNCTION_H
#define MYCFUNCTION_H

#ifdef __cplusplus
extern "C" {
#endif

/* myCFuntion returns a non-null value when it fails */
extern int myCFunction( const double *, unsigned long, 
                        double *, unsigned long );

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* MYCFUNCTION_H */

