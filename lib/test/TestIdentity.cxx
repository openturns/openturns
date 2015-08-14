//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include <iostream>
#include <cstdlib>
#include "OTconfig.hxx" // Only needed for test wrappers
#include "WrapperInterface.h"
#include "WrapperCommon.h"
#include "WrapperMacros.h"


namespace WrapperInternals
{
struct internalState
{
  long numberOfCalls;
};

void internalStateInitialization(struct internalState * p_internalState)
{
  if (p_internalState) p_internalState->numberOfCalls = 0;
}

void internalStateIncrement(struct internalState * p_internalState)
{
  if (p_internalState) p_internalState->numberOfCalls++;
}

long internalStateGetNumberOfCalls(struct internalState * p_internalState)
{
  return (p_internalState ? p_internalState->numberOfCalls : -1);
}
} /* namespace WrapperInternals */

#define WRAPPERNAME TestIdentity

BEGIN_C_DECLS

WRAPPER_BEGIN

static struct WrapperInformation info_TestIdentity = {/* inSize_  = */ 4,
         /* outSize_ = */ 4
};


/* Function */
FUNC_CREATESTATE( WRAPPERNAME ,
{
  *p_p_state = malloc(sizeof(struct WrapperInternals::internalState));
  internalStateInitialization(static_cast<struct WrapperInternals::internalState *>(*p_p_state));
} )

FUNC_DELETESTATE( WRAPPERNAME , { free(p_state); } )

FUNC_INFO( WRAPPERNAME , { *p_info = info_TestIdentity; } )

FUNC_INIT( WRAPPERNAME , {} )

FUNC_EXEC( WRAPPERNAME ,
{
  internalStateIncrement(static_cast<struct WrapperInternals::internalState *>(p_state));
  double x1(inPoint->data_[0]);
  double x2(inPoint->data_[1]);
  double x3(inPoint->data_[2]);
  double x4(inPoint->data_[3]);

  outPoint->data_[0] = x1;
  outPoint->data_[1] = x2;
  outPoint->data_[2] = x3;
  outPoint->data_[3] = x4;
} )

FUNC_FINALIZE( WRAPPERNAME , {} )


/* Gradient, which is void at this time */
GRAD_CREATESTATE( WRAPPERNAME ,
{
  *p_p_state = malloc(sizeof(struct WrapperInternals::internalState));
  internalStateInitialization(static_cast<struct WrapperInternals::internalState *>(*p_p_state));
} )

GRAD_DELETESTATE( WRAPPERNAME , { free(p_state); } )

GRAD_INFO( WRAPPERNAME , { *p_info = info_TestIdentity; } )

GRAD_INIT( WRAPPERNAME , {} )

GRAD_EXEC( WRAPPERNAME ,
{
  internalStateIncrement(static_cast<struct WrapperInternals::internalState *>(p_state));
  outMatrix->data_[ 0] = 1.0;
  outMatrix->data_[ 1] = 0.0;
  outMatrix->data_[ 2] = 0.0;
  outMatrix->data_[ 3] = 0.0;
  outMatrix->data_[ 4] = 0.0;
  outMatrix->data_[ 5] = 1.0;
  outMatrix->data_[ 6] = 0.0;
  outMatrix->data_[ 7] = 0.0;
  outMatrix->data_[ 8] = 0.0;
  outMatrix->data_[ 9] = 0.0;
  outMatrix->data_[10] = 1.0;
  outMatrix->data_[11] = 0.0;
  outMatrix->data_[12] = 0.0;
  outMatrix->data_[13] = 0.0;
  outMatrix->data_[14] = 0.0;
  outMatrix->data_[15] = 1.0;
} )

GRAD_FINALIZE( WRAPPERNAME , {} )

/* Hessian, which is void at this time */
HESS_CREATESTATE( WRAPPERNAME ,
{
  *p_p_state = malloc(sizeof(struct WrapperInternals::internalState));
  internalStateInitialization(static_cast<struct WrapperInternals::internalState *>(*p_p_state));
} )

HESS_DELETESTATE( WRAPPERNAME , { free(p_state); } )

HESS_INFO( WRAPPERNAME , { *p_info = info_TestIdentity; } )

HESS_INIT( WRAPPERNAME , {} )

HESS_EXEC( WRAPPERNAME ,
{
  internalStateIncrement(static_cast<struct WrapperInternals::internalState *>(p_state));

  for (int i = 0; i < 64; i++)
    outTensor->data_[i] = 0.0;
} )

HESS_FINALIZE( WRAPPERNAME , {} )

WRAPPER_END

END_C_DECLS
