//                                               -*- C -*-
/**
 * @brief This file provides the signatures for wrapper function
 *
 * Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#ifndef OPENTURNS_WRAPPERINTERFACE_V0_H
#define OPENTURNS_WRAPPERINTERFACE_V0_H

#include "WrapperInterface.h"

BEGIN_C_DECLS

/**
 * This is the wrapper C API.
 * Every wrapper that intends to plug into the OpenTURNS platform should
 * provide those functions. They may do nothing if the wrapper does not
 * need it.
 *
 * Anyway, always remember that THE WRAPPER IS TO BE CALLED CONCURRENTLY
 * by the platform, so be careful with data sharing and global variable.
 * If not protected with critical sections, it is almost sure that something
 * will break one day or another.
 *
 * Because there is no internal synchronisation mechanism in C, it is UP TO YOU
 * to do the work.
 *
 * So keep cool. Do simple things. And if things come to be tough, have a break !
 * If at last, you always stands with global or static variables, shared data or
 * things leaving outside of your functions, then you have to enter the wonderful
 * but terrible world or parallelism. Do you have a talisman ?
 */


/**
 * StateCreationFunctionPointer : a function that creates a reserved memory space
 * StateDeletionFunctionPointer : a function that frees the reserved memory space
 * GetWrapperInformationFunctionPointer : a function that gives information about
 *   the wrapper internal function
 * InitializationFunctionPointer : a function that prepare the wrapper before the
 *   first call of the wrapper internal function
 * ExecutionFunctionPointer : a function that calls the internal wrapper function
 *   which is the purpose of the wrapper
 * GradientFunctionPointer : a function that calls the internal wrapper gradient
 *   which is the purpose of the wrapper
 * HessianFunctionPointer : a function that calls the internal wrapper hessian
 *   which is the purpose of the wrapper
 * FinalizationFunctionPointer : a function that clean everything up after the last
 *   call of the wrapper internal function
 */

typedef enum WrapperErrorCode (*StateCreationFunctionPointer_V0)(void **, const struct WrapperExchangedData *);
typedef enum WrapperErrorCode (*StateDeletionFunctionPointer_V0)(void *);
typedef enum WrapperErrorCode (*GetWrapperInformationFunctionPointer_V0)(void *, struct WrapperInformation *);
typedef enum WrapperErrorCode (*InitializationFunctionPointer_V0)(void *);
typedef enum WrapperErrorCode (*ExecutionFunctionPointer_V0)(void *, const struct point *, struct point *);
typedef enum WrapperErrorCode (*ExecutionSampleFunctionPointer_V0)(void *, const struct sample *, struct sample *);
typedef enum WrapperErrorCode (*GradientFunctionPointer_V0)(void *, const struct point *, struct matrix *);
typedef enum WrapperErrorCode (*HessianFunctionPointer_V0)(void *, const struct point *, struct tensor *);
typedef enum WrapperErrorCode (*FinalizationFunctionPointer_V0)(void *);

END_C_DECLS

#endif /* OPENTURNS_WRAPPERINTERFACE_V0_H */
