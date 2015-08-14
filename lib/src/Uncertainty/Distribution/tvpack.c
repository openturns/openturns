/* tvpack.f -- translated by f2c (version 20100827).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#include "f2c.h"

/* Common Block Declarations */

union {
    struct {
	doublereal h1, h2, h3, r23, rua, rub, ar, ruc;
	integer nuc;
    } _1;
    struct {
	doublereal h1, h2, h3, r23, rua, rub, ar, ruc;
	integer nu;
    } _2;
} tvtmbk_;

#define tvtmbk_1 (tvtmbk_._1)
#define tvtmbk_2 (tvtmbk_._2)

struct {
    doublereal err;
    integer im;
} ablk_;

#define ablk_1 ablk_

/* Table of constant values */

static doublereal c_b2 = 0.;
static doublereal c_b6 = 1.;


/* This file contains a test program and functions TVTL */
/* (trivariate normal */
/* and t), BVTL (bivariate t), BVND (bivariate normal), STUDNT */
/* (univariate */
/* t), PHID (univariate normal), plus some support functions. */
/* The file is self contained and should compile without errors on (77) */
/* standard Fortran compilers. The test program demonstrates the use of */
/* TVTL for computing trivariate distribution values 20 test problems */
/* with NU = 0 (normal case), 3, 6, 9, and 12. */

/* The software is based on work described in the paper */
/*  "Numerical Computation of Rectangular Bivariate and Trivariate Normal */
/*    and t Probabilities", by */

/*          Alan Genz */
/*          Department of Mathematics */
/*          Washington State University */
/*          Pullman, WA 99164-3113 */
/*          Email : alangenz@wsu.edu */

/* COMMENTED FOR OPENTURNS INCLUSION */
/*      PROGRAM TVTST */
/*      INTEGER I, J, NU, NT */
/*      PARAMETER ( NT = 20 ) */
/*      DOUBLE PRECISION TVTL, LIMIT(3,NT), SIGMA(3,NT), EPS, V */
/*      DATA ( LIMIT(I,1), I = 1, 3 ), ( SIGMA(I,1), I = 1, 3 ) */
/*     &     / .5D0, .5D0, .8D0, .1D0, .6D0, .8D0 / */
/*      DATA ( LIMIT(I,2), I = 1, 3 ), ( SIGMA(I,2), I = 1, 3 ) */
/*     &     / -2.5D0, .5D0, .8D0, .1D0, -.6D0, -.8D0 / */
/*      DATA ( LIMIT(I,3), I = 1, 3 ), ( SIGMA(I,3), I = 1, 3 ) */
/*     &     / 1.5D0, .5D0, .8D0, .1D0, .6D0, .8D0 / */
/*      DATA ( LIMIT(I,4), I = 1, 3 ), ( SIGMA(I,4), I = 1, 3 ) */
/*     &     / .5D0, .5D0, .8D0, .1D0, -.6D0, -.8D0 / */
/*      DATA ( LIMIT(I,5), I = 1, 3 ), ( SIGMA(I,5), I = 1, 3 ) */
/*     &     / .5D0, .5D0, .8D0, .1D0, -.5D0, .5D0 / */
/*      DATA ( LIMIT(I,6), I = 1, 3 ), ( SIGMA(I,6), I = 1, 3 ) */
/*     &     / -1.5D0, .5D0, .8D0, .1D0, -.5D0, .5D0 / */
/*      DATA ( LIMIT(I,7), I = 1, 3 ), ( SIGMA(I,7), I = 1, 3 ) */
/*     &     / 1.5D0, .5D0, .8D0, .1D0, .5D0, -.5D0 / */
/*      DATA ( LIMIT(I,8), I = 1, 3 ), ( SIGMA(I,8), I = 1, 3 ) */
/*     &     / -.5D0, 1D0, 1.2D0, -.4D0, .2D0, .7D0 / */
/*      DATA ( LIMIT(I,9), I = 1, 3 ), ( SIGMA(I,9), I = 1, 3 ) */
/*     &     / 1D0, 1D0, 2D0, .4D0, .8D0, .8D0 / */
/*      DATA ( LIMIT(I,10), I = 1, 3 ), ( SIGMA(I,10), I = 1, 3 ) */
/*     &     / 1D0, 2D0, 1D0, .4D0, .8D0, .8D0 / */
/*      DATA ( LIMIT(I,11), I = 1, 3 ), ( SIGMA(I,11), I = 1, 3 ) */
/*     &     / -2D0, -2D0, -2D0, .4D0, .8D0, .8D0 / */
/*      DATA ( LIMIT(I,12), I = 1, 3 ), ( SIGMA(I,12), I = 1, 3 ) */
/*     *     / 1D0, 2D0, 3D0, -.998D0, -0.248D0, 0.248D0 / */
/*      DATA ( LIMIT(I,13), I = 1, 3 ), ( SIGMA(I,13), I = 1, 3 ) */
/*     *     / -1D0, 2D0, 3D0, .25D0, 0.25D0, 0.25D0 / */
/*      DATA ( LIMIT(I,14), I = 1, 3 ), ( SIGMA(I,14), I = 1, 3 ) */
/*     *     /  1D0, 1D0, 3D0, .998D0, 0.2482D0, 0.2487D0 / */
/*      DATA ( LIMIT(I,15), I = 1, 3 ), ( SIGMA(I,15), I = 1, 3 ) */
/*     *     /  1D0, 1D0, 3D0, .998D0, 0.5D0, 0.5D0 / */
/*      DATA ( LIMIT(I,16), I = 1, 3 ), ( SIGMA(I,16), I = 1, 3 ) */
/*     *     /  1D0, 1D0, 3D0, .99D0, 0.99D0, 0.99D0 / */
/*      DATA ( LIMIT(I,17), I = 1, 3 ), ( SIGMA(I,17), I = 1, 3 ) */
/*     *     /  1D0, 2D0, 3D0, -1D0, -.99D0, .99D0 / */
/*      DATA ( LIMIT(I,18), I = 1, 3 ), ( SIGMA(I,18), I = 1, 3 ) */
/*     *     /  1D0, 2D0, 3D0, 1D0, -.99D0, -.99D0 / */
/*      DATA ( LIMIT(I,19), I = 1, 3 ), ( SIGMA(I,19), I = 1, 3 ) */
/*     *     /  1D0, -1D0, 1D0, .998D0, -0.2482D0, -0.2482D0 / */
/*      DATA ( LIMIT(I,NT), I = 1, 3 ), ( SIGMA(I,NT), I = 1, 3 ) */
/*     *     /  1D0, -1D0, 2D0, .99992D0, 0.64627D0, 0.63975D0 / */
/*      EPS = 1D-16 */
/*      PRINT '(''      Trivariate t Test with EPS ='', E10.1)', EPS */
/*      DO NU = 0, 12, 3 */
/*         PRINT '(''NU   B1   B2   B3    R21      R31      R32  TVT'')' */
/*         DO J = 1, NT */
/*            V = TVTL( NU, LIMIT(1,J), SIGMA(1,J), EPS ) */
/*            PRINT '(I2,3F5.1,3F9.5,F13.10)', NU, */
/*     &           ( LIMIT(I,J), I = 1, 3 ), ( SIGMA(I,J), I = 1, 3 ), V */
/*         END DO */
/*      END DO */
/*      END */

/* **************************************** */
/* END COMMENTED FOR OPENTURNS INCLUSION * */
/* **************************************** */
doublereal tvtl_(integer *nu, doublereal *h__, doublereal *r__, doublereal *
	epsi)
{
    /* System generated locals */
    doublereal ret_val, d__1, d__2;

    /* Builtin functions */
    double asin(doublereal), d_sign(doublereal *, doublereal *);

    /* Local variables */
    static doublereal r12, r13, pt, eps, tvt;
    extern doublereal phid_(doublereal *), bvtl_(integer *, doublereal *, 
	    doublereal *, doublereal *), adonet_(D_fp, doublereal *, 
	    doublereal *, doublereal *);
    extern /* Subroutine */ doublereal tvtmfn_(doublereal *);


/*     A function for computing trivariate normal and t-probabilities. */
/*     This function uses algorithms developed from the ideas */
/*     described in the papers: */
/*       R.L. Plackett, Biometrika 41(1954), pp. 351-360. */
/*       Z. Drezner, Math. Comp. 62(1994), pp. 289-294. */
/*     with adaptive integration from (0,0,1) to (0,0,r23) to R. */

/*      Calculate the probability that X(I) < H(I), for I = 1,2,3 */
/*    NU   INTEGER degrees of freedom; use NU = 0 for normal cases. */
/*    H    REAL array of uppoer limits for probability distribution */
/*    R    REAL array of three correlation coefficients, R should */
/*         contain the lower left portion of the correlation matrix r. */
/*         R should contains the values r21, r31, r23 in that order. */
/*   EPSI  REAL required absolute accuracy; maximum accuracy for most */
/*          computations is approximately 1D-14 */

/*    The software is based on work described in the paper */
/*     "Numerical Computation of Rectangular Bivariate and Trivariate */
/*      Normal and t Probabilities", by the code author: */

/*       Alan Genz */
/*       Department of Mathematics */
/*       Washington State University */
/*       Pullman, WA 99164-3113 */
/*       Email : alangenz@wsu.edu */

/* , HS(3), ZROS(3) */
    /* Parameter adjustments */
    --r__;
    --h__;

    /* Function Body */
    eps = max(1e-14,*epsi);
    pt = asin(1.);
    tvtmbk_1.nuc = *nu;
    tvtmbk_1.h1 = h__[1];
    tvtmbk_1.h2 = h__[2];
    tvtmbk_1.h3 = h__[3];
    r12 = r__[1];
    r13 = r__[2];
    tvtmbk_1.r23 = r__[3];

/*     Sort R's and check for special cases */

    if (abs(r12) > abs(r13)) {
	tvtmbk_1.h2 = tvtmbk_1.h3;
	tvtmbk_1.h3 = h__[2];
	r12 = r13;
	r13 = r__[1];
    }
    if (abs(r13) > abs(tvtmbk_1.r23)) {
	tvtmbk_1.h1 = tvtmbk_1.h2;
	tvtmbk_1.h2 = h__[1];
	tvtmbk_1.r23 = r13;
	r13 = r__[3];
    }
    tvt = 0.;
    if (abs(tvtmbk_1.h1) + abs(tvtmbk_1.h2) + abs(tvtmbk_1.h3) < eps) {
	tvt = ((asin(r12) + asin(r13) + asin(tvtmbk_1.r23)) / pt + 1) / 8;
    } else if (*nu < 1 && abs(r12) + abs(r13) < eps) {
	tvt = phid_(&tvtmbk_1.h1) * bvtl_(nu, &tvtmbk_1.h2, &tvtmbk_1.h3, &
		tvtmbk_1.r23);
    } else if (*nu < 1 && abs(r13) + abs(tvtmbk_1.r23) < eps) {
	tvt = phid_(&tvtmbk_1.h3) * bvtl_(nu, &tvtmbk_1.h1, &tvtmbk_1.h2, &
		r12);
    } else if (*nu < 1 && abs(r12) + abs(tvtmbk_1.r23) < eps) {
	tvt = phid_(&tvtmbk_1.h2) * bvtl_(nu, &tvtmbk_1.h1, &tvtmbk_1.h3, &
		r13);
    } else if (1 - tvtmbk_1.r23 < eps) {
	d__1 = min(tvtmbk_1.h2,tvtmbk_1.h3);
	tvt = bvtl_(nu, &tvtmbk_1.h1, &d__1, &r12);
    } else if (tvtmbk_1.r23 + 1 < eps) {
	if (tvtmbk_1.h2 > -tvtmbk_1.h3) {
	    d__1 = -tvtmbk_1.h3;
	    tvt = bvtl_(nu, &tvtmbk_1.h1, &tvtmbk_1.h2, &r12) - bvtl_(nu, &
		    tvtmbk_1.h1, &d__1, &r12);
	}
    } else {

/*        Compute singular TVT value */

	if (*nu < 1) {
	    tvt = bvtl_(nu, &tvtmbk_1.h2, &tvtmbk_1.h3, &tvtmbk_1.r23) * 
		    phid_(&tvtmbk_1.h1);
	} else if (tvtmbk_1.r23 >= 0.) {
	    d__1 = min(tvtmbk_1.h2,tvtmbk_1.h3);
	    tvt = bvtl_(nu, &tvtmbk_1.h1, &d__1, &c_b2);
	} else if (tvtmbk_1.h2 > -tvtmbk_1.h3) {
	    d__1 = -tvtmbk_1.h3;
	    tvt = bvtl_(nu, &tvtmbk_1.h1, &tvtmbk_1.h2, &c_b2) - bvtl_(nu, &
		    tvtmbk_1.h1, &d__1, &c_b2);
	}

/*        Use numerical integration to compute probability */


	tvtmbk_1.rua = asin(r12);
	tvtmbk_1.rub = asin(r13);
	tvtmbk_1.ar = asin(tvtmbk_1.r23);
	tvtmbk_1.ruc = d_sign(&pt, &tvtmbk_1.ar) - tvtmbk_1.ar;
	tvt += adonet_((D_fp)tvtmfn_, &c_b2, &c_b6, &eps) / (pt * 4);
    }
/* Computing MAX */
    d__1 = 0., d__2 = min(tvt,1.);
    ret_val = max(d__1,d__2);
    return ret_val;
} /* tvtl_ */


doublereal tvtmfn_(doublereal *x)
{
    /* System generated locals */
    doublereal ret_val, d__1;

    /* Local variables */
    static doublereal r__, r12, r13, rr, rr2, rr3;
    extern /* Subroutine */ int sincs_(doublereal *, doublereal *, doublereal 
	    *);
    extern doublereal pntgnd_(integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *);


/*     Computes Plackett formula integrands */

    ret_val = 0.;
    d__1 = tvtmbk_2.rua * *x;
    sincs_(&d__1, &r12, &rr2);
    d__1 = tvtmbk_2.rub * *x;
    sincs_(&d__1, &r13, &rr3);
    if (abs(tvtmbk_2.rua) > 0.) {
	ret_val += tvtmbk_2.rua * pntgnd_(&tvtmbk_2.nu, &tvtmbk_2.h1, &
		tvtmbk_2.h2, &tvtmbk_2.h3, &r13, &tvtmbk_2.r23, &r12, &rr2);
    }
    if (abs(tvtmbk_2.rub) > 0.) {
	ret_val += tvtmbk_2.rub * pntgnd_(&tvtmbk_2.nu, &tvtmbk_2.h1, &
		tvtmbk_2.h3, &tvtmbk_2.h2, &r12, &tvtmbk_2.r23, &r13, &rr3);
    }
    if (tvtmbk_2.nu > 0) {
	d__1 = tvtmbk_2.ar + tvtmbk_2.ruc * *x;
	sincs_(&d__1, &r__, &rr);
	ret_val -= tvtmbk_2.ruc * pntgnd_(&tvtmbk_2.nu, &tvtmbk_2.h2, &
		tvtmbk_2.h3, &tvtmbk_2.h1, &c_b2, &c_b2, &r__, &rr);
    }
    return ret_val;
} /* tvtmfn_ */


/* Subroutine */ int sincs_(doublereal *x, doublereal *sx, doublereal *cs)
{
    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
    double d_sign(doublereal *, doublereal *), sin(doublereal);

    /* Local variables */
    static doublereal ee;


/*     Computes SIN(X), COS(X)^2, with series approx. for |X| near PI/2 */

/* Computing 2nd power */
    d__1 = 1.57079632679489661923132169163975 - abs(*x);
    ee = d__1 * d__1;
    if (ee < 5e-5) {
	d__1 = 1 - ee * (1 - ee / 12) / 2;
	*sx = d_sign(&d__1, x);
	*cs = ee * (1 - ee * (1 - ee * 2 / 15) / 3);
    } else {
	*sx = sin(*x);
	*cs = 1 - *sx * *sx;
    }
    return 0;
} /* sincs_ */


doublereal pntgnd_(integer *nu, doublereal *ba, doublereal *bb, doublereal *
	bc, doublereal *ra, doublereal *rb, doublereal *r__, doublereal *rr)
{
    /* System generated locals */
    doublereal ret_val, d__1;

    /* Builtin functions */
    double sqrt(doublereal), exp(doublereal), pow_di(doublereal *, integer *);

    /* Local variables */
    static doublereal bt, dt, ft;
    extern doublereal phid_(doublereal *), studnt_(integer *, doublereal *);


/*     Computes Plackett formula integrand */

    ret_val = 0.;
/* Computing 2nd power */
    d__1 = *ra - *rb;
    dt = *rr * (*rr - d__1 * d__1 - *ra * 2 * *rb * (1 - *r__));
    if (dt > 0.) {
	bt = (*bc * *rr + *ba * (*r__ * *rb - *ra) + *bb * (*r__ * *ra - *rb))
		 / sqrt(dt);
/* Computing 2nd power */
	d__1 = *ba - *r__ * *bb;
	ft = d__1 * d__1 / *rr + *bb * *bb;
	if (*nu < 1) {
	    if (bt > -10. && ft < 100.) {
		ret_val = exp(-ft / 2);
		if (bt < 10.) {
		    ret_val *= phid_(&bt);
		}
	    }
	} else {
	    ft = sqrt(ft / *nu + 1);
	    d__1 = bt / ft;
	    ret_val = studnt_(nu, &d__1) / pow_di(&ft, nu);
	}
    }
    return ret_val;
} /* pntgnd_ */


doublereal adonet_(D_fp f, doublereal *a, doublereal *b, doublereal *tol)
{
    /* System generated locals */
    integer i__1;
    doublereal ret_val, d__1;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer i__;
    static doublereal ai[100], bi[100], ei[100], fi[100];
    static integer ip;
    static doublereal fin;
    extern doublereal krnrdt_(doublereal *, doublereal *, D_fp, doublereal *);


/*     One Dimensional Globally Adaptive Integration Function */

    ai[0] = *a;
    bi[0] = *b;
    ablk_1.err = 1.;
    ip = 1;
    ablk_1.im = 1;
    while(ablk_1.err * 4 > *tol && ablk_1.im < 100) {
	++ablk_1.im;
	bi[ablk_1.im - 1] = bi[ip - 1];
	ai[ablk_1.im - 1] = (ai[ip - 1] + bi[ip - 1]) / 2;
	bi[ip - 1] = ai[ablk_1.im - 1];
	fi[ip - 1] = krnrdt_(&ai[ip - 1], &bi[ip - 1], (D_fp)f, &ei[ip - 1]);
	fi[ablk_1.im - 1] = krnrdt_(&ai[ablk_1.im - 1], &bi[ablk_1.im - 1], (
		D_fp)f, &ei[ablk_1.im - 1]);
	ablk_1.err = 0.;
	fin = 0.;
	i__1 = ablk_1.im;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (ei[i__ - 1] > ei[ip - 1]) {
		ip = i__;
	    }
	    fin += fi[i__ - 1];
/* Computing 2nd power */
	    d__1 = ei[i__ - 1];
	    ablk_1.err += d__1 * d__1;
	}
	ablk_1.err = sqrt(ablk_1.err);
    }
    ret_val = fin;
    return ret_val;
} /* adonet_ */


doublereal krnrdt_(doublereal *a, doublereal *b, D_fp f, doublereal *err)
{
    /* Initialized data */

    static doublereal wg[7] = { .2729250867779007,.05566856711617449,
	    .1255803694649048,.1862902109277352,.2331937645919914,
	    .2628045445102478 };
    static doublereal xgk[12] = { 0.,.9963696138895427,.978228658146057,
	    .9416771085780681,.8870625997680953,.8160574566562211,
	    .7301520055740492,.6305995201619651,.5190961292068118,
	    .3979441409523776,.269543155952345,.1361130007993617 };
    static doublereal wgk[12] = { .1365777947111183,.00976544104596129,
	    .02715655468210443,.04582937856442671,.06309742475037484,
	    .07866457193222764,.09295309859690074,.1058720744813894,
	    .1167395024610472,.1251587991003195,.1312806842298057,
	    .1351935727998845 };

    /* System generated locals */
    doublereal ret_val, d__1, d__2;

    /* Local variables */
    static integer j;
    static doublereal t, fc, cen, wid, resg, resk;


/*     Kronrod Rule */


/*        The abscissae and weights are given for the interval (-1,1); */
/*        only positive abscissae and corresponding weights are given. */

/*        XGK    - abscissae of the 2N+1-point Kronrod rule: */
/*                 XGK(2), XGK(4), ...  N-point Gauss rule abscissae; */
/*                 XGK(1), XGK(3), ...  optimally added abscissae. */
/*        WGK    - weights of the 2N+1-point Kronrod rule. */
/*        WG     - weights of the N-point Gauss rule. */




/*           Major variables */

/*           CEN  - mid point of the interval */
/*           WID  - half-length of the interval */
/*           RESG - result of the N-point Gauss formula */
/*           RESK - result of the 2N+1-point Kronrod formula */

/*           Compute the 2N+1-point Kronrod approximation to */
/*            the integral, and estimate the absolute error. */

    wid = (*b - *a) / 2;
    cen = (*b + *a) / 2;
    fc = (*f)(&cen);
    resg = fc * wg[0];
    resk = fc * wgk[0];
    for (j = 1; j <= 11; ++j) {
	t = wid * xgk[j];
	d__1 = cen - t;
	d__2 = cen + t;
	fc = (*f)(&d__1) + (*f)(&d__2);
	resk += wgk[j] * fc;
	if (j % 2 == 0) {
	    resg += wg[j / 2] * fc;
	}
    }
    ret_val = wid * resk;
    *err = (d__1 = wid * (resk - resg), abs(d__1));
    return ret_val;
} /* krnrdt_ */


doublereal studnt_(integer *nu, doublereal *t)
{
    /* System generated locals */
    doublereal ret_val, d__1, d__2;

    /* Builtin functions */
    double acos(doublereal), atan(doublereal), sqrt(doublereal);

    /* Local variables */
    static integer j;
    static doublereal pi, rn, ts, tt;
    extern doublereal phid_(doublereal *);
    static doublereal snthe, polyn, cssthe;


/*     Student t Distribution Function */

/*                       T */
/*         STUDNT = C   I  ( 1 + y*y/NU )**( -(NU+1)/2 ) dy */
/*                   NU -INF */

    pi = acos(-1.);
    if (*nu < 1) {
	ret_val = phid_(t);
    } else if (*nu == 1) {
	ret_val = (atan(*t) * 2 / pi + 1) / 2;
    } else if (*nu == 2) {
	ret_val = (*t / sqrt(*t * *t + 2) + 1) / 2;
    } else {
	tt = *t * *t;
	cssthe = 1 / (tt / *nu + 1);
	polyn = 1.;
	for (j = *nu - 2; j >= 2; j += -2) {
	    polyn = (j - 1) * cssthe * polyn / j + 1;
	}
	if (*nu % 2 == 1) {
	    rn = (doublereal) (*nu);
	    ts = *t / sqrt(rn);
	    ret_val = ((atan(ts) + ts * cssthe * polyn) * 2 / pi + 1) / 2;
	} else {
	    snthe = *t / sqrt(*nu + tt);
	    ret_val = (snthe * polyn + 1) / 2;
	}
/* Computing MAX */
	d__1 = 0., d__2 = min(ret_val,1.);
	ret_val = max(d__1,d__2);
    }
    return ret_val;
} /* studnt_ */


doublereal bvtl_(integer *nu, doublereal *dh, doublereal *dk, doublereal *r__)
{
    /* System generated locals */
    integer i__1;
    doublereal ret_val, d__1, d__2, d__3;

    /* Builtin functions */
    double acos(doublereal), sqrt(doublereal), d_sign(doublereal *, 
	    doublereal *), atan2(doublereal, doublereal);

    /* Local variables */
    static integer j;
    static doublereal pi;
    static integer hs, ks;
    static doublereal hkn, hpk, hrk, krh, bvt, tpi, ors, snu;
    extern doublereal bvnd_(doublereal *, doublereal *, doublereal *);
    static doublereal gmph, gmpk, hkrn, qhrk, xnkh, xnhk, btnckh, btnchk, 
	    btpdkh, btpdhk;
    extern doublereal studnt_(integer *, doublereal *);


/*     A function for computing bivariate t probabilities. */

/*       Alan Genz */
/*       Department of Mathematics */
/*       Washington State University */
/*       Pullman, WA 99164-3113 */
/*       Email : alangenz@wsu.edu */

/*    This function is based on the method described by */
/*        Dunnett, C.W. and M. Sobel, (1954), */
/*        A bivariate generalization of Student's t-distribution */
/*        with tables for certain special cases, */
/*        Biometrika 41, pp. 153-169. */

/* BVTL - calculate the probability that X < DH and Y < DK. */

/* parameters */

/*   NU number of degrees of freedom */
/*   DH 1st lower integration limit */
/*   DK 2nd lower integration limit */
/*   R   correlation coefficient */

    if (*nu < 1) {
	d__1 = -(*dh);
	d__2 = -(*dk);
	ret_val = bvnd_(&d__1, &d__2, r__);
    } else if (1 - *r__ <= 1e-15) {
	d__1 = min(*dh,*dk);
	ret_val = studnt_(nu, &d__1);
    } else if (*r__ + 1 <= 1e-15) {
	if (*dh > -(*dk)) {
	    d__1 = -(*dk);
	    ret_val = studnt_(nu, dh) - studnt_(nu, &d__1);
	} else {
	    ret_val = 0.;
	}
    } else {
	pi = acos(-1.);
	tpi = pi * 2;
	snu = (doublereal) (*nu);
	snu = sqrt(snu);
	ors = 1 - *r__ * *r__;
	hrk = *dh - *r__ * *dk;
	krh = *dk - *r__ * *dh;
	if (abs(hrk) + ors > 0.) {
/* Computing 2nd power */
	    d__1 = hrk;
/* Computing 2nd power */
	    d__2 = hrk;
/* Computing 2nd power */
	    d__3 = *dk;
	    xnhk = d__1 * d__1 / (d__2 * d__2 + ors * (*nu + d__3 * d__3));
/* Computing 2nd power */
	    d__1 = krh;
/* Computing 2nd power */
	    d__2 = krh;
/* Computing 2nd power */
	    d__3 = *dh;
	    xnkh = d__1 * d__1 / (d__2 * d__2 + ors * (*nu + d__3 * d__3));
	} else {
	    xnhk = 0.;
	    xnkh = 0.;
	}
	d__1 = *dh - *r__ * *dk;
	hs = (integer) d_sign(&c_b6, &d__1);
	d__1 = *dk - *r__ * *dh;
	ks = (integer) d_sign(&c_b6, &d__1);
	if (*nu % 2 == 0) {
	    bvt = atan2(sqrt(ors), -(*r__)) / tpi;
/* Computing 2nd power */
	    d__1 = *dh;
	    gmph = *dh / sqrt((*nu + d__1 * d__1) * 16);
/* Computing 2nd power */
	    d__1 = *dk;
	    gmpk = *dk / sqrt((*nu + d__1 * d__1) * 16);
	    btnckh = atan2(sqrt(xnkh), sqrt(1 - xnkh)) * 2 / pi;
	    btpdkh = sqrt(xnkh * (1 - xnkh)) * 2 / pi;
	    btnchk = atan2(sqrt(xnhk), sqrt(1 - xnhk)) * 2 / pi;
	    btpdhk = sqrt(xnhk * (1 - xnhk)) * 2 / pi;
	    i__1 = *nu / 2;
	    for (j = 1; j <= i__1; ++j) {
		bvt += gmph * (ks * btnckh + 1);
		bvt += gmpk * (hs * btnchk + 1);
		btnckh += btpdkh;
		btpdkh = (j << 1) * btpdkh * (1 - xnkh) / ((j << 1) + 1);
		btnchk += btpdhk;
		btpdhk = (j << 1) * btpdhk * (1 - xnhk) / ((j << 1) + 1);
/* Computing 2nd power */
		d__1 = *dh;
		gmph = gmph * ((j << 1) - 1) / ((j << 1) * (d__1 * d__1 / *nu 
			+ 1));
/* Computing 2nd power */
		d__1 = *dk;
		gmpk = gmpk * ((j << 1) - 1) / ((j << 1) * (d__1 * d__1 / *nu 
			+ 1));
	    }
	} else {
/* Computing 2nd power */
	    d__1 = *dh;
/* Computing 2nd power */
	    d__2 = *dk;
	    qhrk = sqrt(d__1 * d__1 + d__2 * d__2 - *r__ * 2 * *dh * *dk + *
		    nu * ors);
	    hkrn = *dh * *dk + *r__ * *nu;
	    hkn = *dh * *dk - *nu;
	    hpk = *dh + *dk;
	    bvt = atan2(-snu * (hkn * qhrk + hpk * hkrn), hkn * hkrn - *nu * 
		    hpk * qhrk) / tpi;
	    if (bvt < -1e-15) {
		bvt += 1;
	    }
/* Computing 2nd power */
	    d__1 = *dh;
	    gmph = *dh / (tpi * snu * (d__1 * d__1 / *nu + 1));
/* Computing 2nd power */
	    d__1 = *dk;
	    gmpk = *dk / (tpi * snu * (d__1 * d__1 / *nu + 1));
	    btnckh = sqrt(xnkh);
	    btpdkh = btnckh;
	    btnchk = sqrt(xnhk);
	    btpdhk = btnchk;
	    i__1 = (*nu - 1) / 2;
	    for (j = 1; j <= i__1; ++j) {
		bvt += gmph * (ks * btnckh + 1);
		bvt += gmpk * (hs * btnchk + 1);
		btpdkh = ((j << 1) - 1) * btpdkh * (1 - xnkh) / (j << 1);
		btnckh += btpdkh;
		btpdhk = ((j << 1) - 1) * btpdhk * (1 - xnhk) / (j << 1);
		btnchk += btpdhk;
/* Computing 2nd power */
		d__1 = *dh;
		gmph = (j << 1) * gmph / (((j << 1) + 1) * (d__1 * d__1 / *nu 
			+ 1));
/* Computing 2nd power */
		d__1 = *dk;
		gmpk = (j << 1) * gmpk / (((j << 1) + 1) * (d__1 * d__1 / *nu 
			+ 1));
	    }
	}
	ret_val = bvt;
    }
/*     END BVTL */
    return ret_val;
} /* bvtl_ */


doublereal phid_(doublereal *z__)
{
    /* Initialized data */

    static doublereal a[44] = { .610143081923200417926465815756,
	    -.434841272712577471828182820888,.176351193643605501125840298123,
	    -.060710795609249414860051215825,.017712068995694114486147141191,
	    -.004321119385567293818599864968,8.54216676887098678819832055e-4,
	    -1.2715509060916274262889394e-4,1.1248167243671189468847072e-5,
	    3.13063885421820972630152e-7,-2.70988068537762022009086e-7,
	    3.0737622701407688440959e-8,2.515620384817622937314e-9,
	    -1.02892992132031912759e-9,2.9944052119949939363e-11,
	    2.605178968726693629e-11,-2.634839924171969386e-12,
	    -6.43404509890636443e-13,1.12457401801663447e-13,
	    1.7281533389986098e-14,-4.264101694942375e-15,
	    -5.45371977880191e-16,1.58697607761671e-16,2.0899837844334e-17,
	    -5.900526869409e-18,-9.41893387554e-19,2.1497735647e-19,
	    4.6660985008e-20,-7.243011862e-21,-2.387966824e-21,1.91177535e-22,
	    1.20482568e-22,-6.72377e-25,-5.747997e-24,-4.28493e-25,
	    2.44856e-25,4.3793e-26,-8.151e-27,-3.089e-27,9.3e-29,1.74e-28,
	    1.6e-29,-8e-30,-2e-30 };

    /* System generated locals */
    doublereal ret_val;

    /* Builtin functions */
    double exp(doublereal);

    /* Local variables */
    static doublereal b;
    static integer i__;
    static doublereal p, t, bm, bp, xa;


/*     Normal distribution probabilities accurate to 1d-15. */
/*     Reference: J.L. Schonfelder, Math Comp 32(1978), pp 1232-1240. */


    xa = abs(*z__) / 1.414213562373095048801688724209;
    if (xa > 100.) {
	p = 0.;
    } else {
	t = (xa * 8 - 30) / (xa * 4 + 15);
	bm = 0.;
	b = 0.;
	for (i__ = 24; i__ >= 0; --i__) {
	    bp = b;
	    b = bm;
	    bm = t * b - bp + a[i__];
	}
	p = exp(-xa * xa) * (bm - bp) / 4;
    }
    if (*z__ > 0.) {
	p = 1 - p;
    }
    ret_val = p;
    return ret_val;
} /* phid_ */


doublereal bvnd_(doublereal *dh, doublereal *dk, doublereal *r__)
{
    /* Initialized data */

    static struct {
	doublereal e_1[3];
	doublereal fill_2[7];
	doublereal e_3[6];
	doublereal fill_4[4];
	doublereal e_5[10];
	} equiv_92 = { .1713244923791705, .3607615730481384, 
		.4679139345726904, {0}, .04717533638651177, .1069393259953183,
		 .1600783285433464, .2031674267230659, .2334925365383547, 
		.2491470458134029, {0}, .01761400713915212, 
		.04060142980038694, .06267204833410906, .08327674157670475, 
		.1019301198172404, .1181945319615184, .1316886384491766, 
		.1420961093183821, .1491729864726037, .1527533871307259 };

#define w ((doublereal *)&equiv_92)

    static struct {
	doublereal e_1[3];
	doublereal fill_2[7];
	doublereal e_3[6];
	doublereal fill_4[4];
	doublereal e_5[10];
	} equiv_93 = { -.9324695142031522, -.6612093864662647, 
		-.238619186083197, {0}, -.9815606342467191, -.904117256370475,
		 -.769902674194305, -.5873179542866171, -.3678314989981802, 
		-.1252334085114692, {0}, -.9931285991850949, 
		-.9639719272779138, -.9122344282513259, -.8391169718222188, 
		-.7463319064601508, -.636053680726515, -.5108670019508271, 
		-.3737060887154196, -.2277858511416451, -.07652652113349733 };

#define x ((doublereal *)&equiv_93)


    /* System generated locals */
    integer i__1;
    doublereal ret_val, d__1, d__2;

    /* Builtin functions */
    double asin(doublereal), sin(doublereal), exp(doublereal), sqrt(
	    doublereal);

    /* Local variables */
    static doublereal a, b, c__, d__, h__;
    static integer i__;
    static doublereal k;
    static integer lg;
    static doublereal as;
    static integer ng;
    static doublereal bs, hk, hs;
    static integer is;
    static doublereal sn, rs, xs, bvn, asr;
    extern doublereal phid_(doublereal *);


/*     A function for computing bivariate normal probabilities. */

/*       Alan Genz */
/*       Department of Mathematics */
/*       Washington State University */
/*       Pullman, WA 99164-3113 */
/*       Email : alangenz@wsu.edu */

/*    This function is based on the method described by */
/*        Drezner, Z and G.O. Wesolowsky, (1989), */
/*        On the computation of the bivariate normal integral, */
/*        Journal of Statist. Comput. Simul. 35, pp. 101-107, */
/*    with major modifications for double precision, */
/*    and for |R| close to 1. */

/* BVND calculates the probability that X > DH and Y > DK. */
/*      Note: Prob( X < DH, Y < DK ) = BVND( -DH, -DK, R ). */

/* Parameters */

/*   DH  DOUBLE PRECISION, integration limit */
/*   DK  DOUBLE PRECISION, integration limit */
/*   R   DOUBLE PRECISION, correlation coefficient */

/*     Gauss Legendre Points and Weights, N =  6 */
/*     Gauss Legendre Points and Weights, N = 12 */
/*     Gauss Legendre Points and Weights, N = 20 */
    if (abs(*r__) < .3f) {
	ng = 1;
	lg = 3;
    } else if (abs(*r__) < .75f) {
	ng = 2;
	lg = 6;
    } else {
	ng = 3;
	lg = 10;
    }
    h__ = *dh;
    k = *dk;
    hk = h__ * k;
    bvn = 0.;
    if (abs(*r__) < .925f) {
	if (abs(*r__) > 0.) {
	    hs = (h__ * h__ + k * k) / 2;
	    asr = asin(*r__);
	    i__1 = lg;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		for (is = -1; is <= 1; is += 2) {
		    sn = sin(asr * (is * x[i__ + ng * 10 - 11] + 1) / 2);
		    bvn += w[i__ + ng * 10 - 11] * exp((sn * hk - hs) / (1 - 
			    sn * sn));
		}
	    }
	    bvn = bvn * asr / 12.566370614359172;
	}
	d__1 = -h__;
	d__2 = -k;
	bvn += phid_(&d__1) * phid_(&d__2);
    } else {
	if (*r__ < 0.) {
	    k = -k;
	    hk = -hk;
	}
	if (abs(*r__) < 1.) {
	    as = (1 - *r__) * (*r__ + 1);
	    a = sqrt(as);
/* Computing 2nd power */
	    d__1 = h__ - k;
	    bs = d__1 * d__1;
	    c__ = (4 - hk) / 8;
	    d__ = (12 - hk) / 16;
	    asr = -(bs / as + hk) / 2;
	    if (asr > -100.) {
		bvn = a * exp(asr) * (1 - c__ * (bs - as) * (1 - d__ * bs / 5)
			 / 3 + c__ * d__ * as * as / 5);
	    }
	    if (-hk < 100.) {
		b = sqrt(bs);
		d__1 = -b / a;
		bvn -= exp(-hk / 2) * sqrt(6.283185307179586) * phid_(&d__1) *
			 b * (1 - c__ * bs * (1 - d__ * bs / 5) / 3);
	    }
	    a /= 2;
	    i__1 = lg;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		for (is = -1; is <= 1; is += 2) {
/* Computing 2nd power */
		    d__1 = a * (is * x[i__ + ng * 10 - 11] + 1);
		    xs = d__1 * d__1;
		    rs = sqrt(1 - xs);
		    asr = -(bs / xs + hk) / 2;
		    if (asr > -100.) {
			bvn += a * w[i__ + ng * 10 - 11] * exp(asr) * (exp(
				-hk * (1 - rs) / ((rs + 1) * 2)) / rs - (c__ *
				 xs * (d__ * xs + 1) + 1));
		    }
		}
	    }
	    bvn = -bvn / 6.283185307179586;
	}
	if (*r__ > 0.) {
	    d__1 = -max(h__,k);
	    bvn += phid_(&d__1);
	} else {
	    bvn = -bvn;
	    if (k > h__) {
		bvn = bvn + phid_(&k) - phid_(&h__);
	    }
	}
    }
    ret_val = bvn;
    return ret_val;
} /* bvnd_ */

#undef x
#undef w


