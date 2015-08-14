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
#include "OTconfig.hxx" // Only needed for test wrappers
#include "Wrapper.h"


/*
 *  This is the declaration of function named 'compute_deviation' into the wrapper.
 */


BEGIN_C_DECLS

WRAPPER_BEGIN

/*
******************************************************************************************
*                                                                                        *
*                             compute_deviation function                                 *
*                                                                                        *
******************************************************************************************
*/


/**
 * Execution function
 * This function is called by the platform to do the real work of the wrapper. It may be
 * called concurrently, so be aware of not using shared or global data not protected by
 * a critical section.
 * This function has a mathematical meaning. It operates on one vector (aka point) and
 * returns another vector.
 */
enum WrapperErrorCode func_exec_compute_deviation(void * p_state,
    const struct point * inPoint,
    struct point * outPoint,
    const struct WrapperExchangedData * p_exchangedData,
    void * p_error)
{
  const double & E = inPoint->data_[0];
  const double & F = inPoint->data_[1];
  const double & L = inPoint->data_[2];
  const double & I = inPoint->data_[3];

  double & d = outPoint->data_[0];

  if ((E == 0.0) || (I == 0.0))
  {
    setError( p_error, "Neither E nor I should be zero. Got E=%g and I=%g", E, I );
    return WRAPPER_EXECUTION_ERROR;
  }

  /* The real computation is here */
  d = -( F * L * L * L ) / ( 3. * E * I );

  return WRAPPER_OK;
}

WRAPPER_END

END_C_DECLS
