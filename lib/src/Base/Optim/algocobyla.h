/* cobyla : contrained optimization by linear approximation */

/*
 * Copyright (c) 1992, Michael J. D. Powell (M.J.D.Powell@damtp.cam.ac.uk)
 * Copyright (c) 2004, Jean-Sebastien Roy (js@jeannot.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * This software is a C version of COBYLA2, a contrained optimization by linear
 * approximation package developed by Michael J. D. Powell in Fortran.
 *
 * The original source code can be found at :
 * http://plato.la.asu.edu/topics/problems/nlores.html
 */

/* $Jeannot: cobyla.h,v 1.10 2004/04/18 09:51:37 js Exp $ */

#ifndef _COBYLA_
#define _COBYLA_

#include "openturns/OTdebug.h"

BEGIN_C_DECLS

/*
 * Verbosity level
 */
typedef enum {
  COBYLA_MSG_NONE = 0, /* No messages */
  COBYLA_MSG_EXIT = 1, /* Exit reasons */
  COBYLA_MSG_ITER = 2, /* Rho and Sigma changes */
  COBYLA_MSG_INFO = 3, /* Informational messages */
} cobyla_message;

/*
 * Possible return values for cobyla
 */
typedef enum
  {
    COBYLA_MINRC     = -2, /* Constant to add to get the rc_string */
    COBYLA_EINVAL    = -2, /* N<0 or M<0 */
    COBYLA_ENOMEM    = -1, /* Memory allocation failed */
    COBYLA_NORMAL    =  0, /* Normal return from cobyla */
    COBYLA_MAXFUN    =  1, /* Maximum number of function evaluations reach */
    COBYLA_ROUNDING  =  2, /* Rounding errors are becoming damaging */
    COBYLA_USERABORT =  3  /* User requested end of minimization */
  } cobyla_rc;

/*
 * Return code strings
 * use cobyla_rc_string[rc - COBYLA_MINRC] to get the message associated with
 * return code rc.
 */
extern const char *cobyla_rc_string[6];

/*
 * A function as required by cobyla
 * state is a void pointer provided to the function at each call
 *
 * n     : the number of variables
 * m     : the number of constraints
 * x     : on input, then vector of variables (should not be modified)
 * f     : on output, the value of the function
 * con   : on output, the value of the constraints (vector of size m)
 * state : on input, the value of the state variable as provided to cobyla
 *
 * COBYLA will try to make all the values of the constraints positive.
 * So if you want to input a constraint j such as x[i] <= MAX, set:
 *   con[j] = MAX - x[i]
 * The function must returns 0 if no error occurs or 1 to immediately end the
 * minimization.
 *
 */
typedef int cobyla_function(int n, int m, double *x, double *f, double *con,
                            void *state);

/*
 * cobyla : minimize a function subject to constraints
 *
 * n         : number of variables (>=0)
 * m         : number of constraints (>=0)
 * x         : on input, initial estimate ; on output, the solution
 * rhobeg    : a reasonable initial change to the variables
 * rhoend    : the required accuracy for the variables
 * message   : see the cobyla_message enum
 * maxfun    : on input, the maximum number of function evaluations
 *             on output, the number of function evaluations done
 * calcfc    : the function to minimize (see cobyla_function)
 * state     : used by function (see cobyla_function)
 *
 * The cobyla function returns a code defined in the cobyla_rc enum.
 *
 */
extern int ot_cobyla(int n, int m, double *x, double rhobeg, double rhoend,
                  int message, int *maxfun, cobyla_function *calcfc, void *state);

END_C_DECLS

#endif /* _COBYLA_ */
