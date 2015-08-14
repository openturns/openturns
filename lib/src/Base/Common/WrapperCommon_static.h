//                                               -*- C -*-
/**
 * @brief This file provides definitions for exported wrapper functions
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

#ifndef OPENTURNS_WRAPPERCOMMON_STATIC_H
#define OPENTURNS_WRAPPERCOMMON_STATIC_H

#include <WrapperInterface.h>
#include <WrapperCommon_macros.h>
#include <OTdebug.h>

#ifdef __cplusplus
extern "C" {
#endif

#undef BIND_ACTION
#define BIND_ACTION(rtype, name, args) OT_API rtype (* name) args;
BIND_METHODS;

#ifdef __cplusplus
}
#endif

#endif /* OPENTURNS_WRAPPERCOMMON_STATIC_H */
