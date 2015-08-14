/*                                               -*- C -*- */
/**
 *  @brief The wrapper adapts the interface of OpenTURNS and of the wrapped code
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

#include "Wrapper.h"

/* WARNING : Please read the following lines
 *
 * In this program, we make the assumption that the end user wishes to
 * call a function (aka NumericalMathFunction) named "wcode" (wcode stands
 * for wrapped code).
 * In order to individualize the wrapper to the user's needs, we encourage
 * you (as the developer of this wrapper) to rename any occurence of "wcode"
 * (in either case) to the real name of the function.
 * It will also avoid any confusion with other "wcode"s written by other entities
 * or developers.
 */

/* If you plan to link this wrapper against a FORTRAN library,
 * remember to change the name WCODE for your actual FORTRAN subroutine name.
 * Otherwise you can ignore these lines.
 *
 * Remember that FORTRAN passes its arguments by reference, not by value as C and C++
 * usually do. So you need to pass the pointer to the arguments rather than its value.
 * This is true for single values (integers, reals, etc.) but not for arrays that are
 * already pointers in the C/C++ environment. Those ones can directly be passed as
 * "values" though they are pointers indeed.
 * Be careful that C and C++ arrays start from 0 and FORTRAN from 1!
 * Be also very careful with the size of the value your plan to pass. Integers in C/C++
 * are not INTEGER*8 in many cases. Float or doubles do not necessarily match REAL*4
 * or REAL*8 in FORTRAN.
 *
 * FORTRAN gives no clue for preventing const values from being altered. So you need to
 * protect them by copying them before calling the FORTRAN subroutine if this import
 * to you.
 *
 * Summary: there are only exceptions to the rule and you need to
 * know exactly what you are doing! You may be desappointed at first, but it will keep
 * you away from segmentation violation and other similar fantasies. ;-)
 */

/* If you want to customize this wrapper to your needs, you have to do the following :
 *  - change the current wrapper name 'wcode' to any name you chooze in the #define WRAPPERNAME macro.
 *  - adapt the signatures of the calls. You have to write the function in another file and link
 *        the wrapper with the corresponding object file. But, due to some technical
 *        aspect of C/FORTRAN linking, you have to declare your FORTRAN function to C in
 *        a special manner and use the F77_FUNC macro below. Don't worry, it's easy !
 *        First, change the name 'wcode' everywhere in the macro to the name of your FORTRAN
 *        subroutine. Second, declare the arguments passed to the subroutine in a prototype
 *        declaration. Remember that all FORTRAN arguments are passed as reference (aka pointers)
 *        conversely to C. Long and double in C match INTEGER*4 and REAL*8 in FORTRAN
 *        respectively. Avoid using strings.
 *  - call your C function in the execute function of your wrapper.
 */

/* The name of the wrapper's functions is defined in WRAPPERNAME macro */
#define WRAPPERNAME wcode

#include "FortranCInterface.h"
#define F77_FUNC(name,NAME) FortranCInterface_GLOBAL(name,NAME)
#define F77_FUNC_(name,NAME) FortranCInterface_GLOBAL_(name,NAME)

/* Adapt the following lines to your needs */
#define CALCUL_F77 F77_FUNC(calcul,CALCUL)
#ifdef __cplusplus
extern "C"  /* prevent C++ name mangling */
#endif
void CALCUL_F77(double *, long *, double *, long *, long *);


BEGIN_C_DECLS
WRAPPER_BEGIN

/*
 *  This is the declaration of function named 'wcode' into the wrapper.
 */
  

  /*
*********************************************************************************
*                                                                               *
*                             wcode function                                    *
*                                                                               *
*********************************************************************************
*/

  /* The wrapper information informs the NumericalMathFunction object that loads the wrapper of the
   * signatures of the wrapper functions. In particular, it hold the size of the input
   * NumericalPoint (inSize_) and of the output NumericalPoint (outSize_).
   * Those information are also used by the gradient and hessian functions to set the correct size
   * of the returned matrix and tensor.
   */
  
  /* The getInfo function is optional. Except if you alter the description of the wrapper, you'd better
   * use the standard one automatically provided by the platform. Uncomment the following definition if
   * you want to provide yours instead. */
  /* FUNC_INFO( WRAPPERNAME , {} ) */
    
  /* The state creation/deletion functions allow the wrapper to create or delete a memory location
   * that it will manage itself. It can save in this location any information it needs. The OpenTURNS
   * platform only ensures that the wrapper will receive the state (= the memory location) it works
   * with. If many wrappers are working simultaneously or if the same wrapper is called concurrently,
   * this mechanism will avoid any collision or confusion.
   * The consequence is that NO STATIC DATA should be used in the wrapper OR THE WRAPPER WILL BREAKE
   * one day. You may think that you can't do without static data, but in general this is the case
   * of a poor design. But if you persist to use static data, do your work correctly and make use
   * of mutex (for instance) to protect your data against concurrent access. But don't complain about
   * difficulties or poor computational performance! 
   */
    
    
  /* The createState function is optional. If you need to manage an internal state, uncomment the following
   * definitions and adapt the source code to your needs. By default Open TURNS provides default ones. */
  /* FUNC_CREATESTATE( WRAPPERNAME , {
     CHECK_WRAPPER_MODE( WRAPPER_STATICLINK );
     CHECK_WRAPPER_IN(   WRAPPER_ARGUMENTS  );
     CHECK_WRAPPER_OUT(  WRAPPER_ARGUMENTS  );
     
     COPY_EXCHANGED_DATA_TO( p_p_state );
     
     PRINT( "My message is here" );
     } ) */
  
  /* The deleteState function is optional. See FUNC_CREATESTATE for explanation. */
  /* FUNC_DELETESTATE( WRAPPERNAME , {
     DELETE_EXCHANGED_DATA_FROM( p_state );
     } ) */
    





  /* Any function declared into the wrapper may declare three actual functions prefixed with
   * 'init_', 'exec_' and 'finalize_' followed by the name of the function, here 'wcode'.
   *
   * The 'init_' function is only called once when the NumericalMathFunction object is created.
   * It allows the wrapper to set some internal state, read some external file, prepare the function
   * to run, etc.
   *
   * The 'exec_' function is intended to execute what the wrapper is done for: compute an mathematical
   * function or anything else. It takes the internal state pointer as its first argument, the input
   * NumericalPoint pointer as the second and the output NumericalPoint pointer as the third.
   *
   * The 'finalize_' function is only called once when the NumericalMathFunction object is destroyed.
   * It allows the wrapper to flush anything before unloading.
   *
   * Only the 'exec_' function is mandatory because the other ones are automatically provided by the platform.
   */
    
    
  /**
   * Initialization function
   * This function is called once just before the wrapper first called to initialize
   * it, ie create a temparary subdirectory (remember that the wrapper may be called
   * concurrently), store exchanged data in some internal repository, do some
   * pre-computational operation, etc. Uncomment the following definition if you want to
   * do some pre-computation work.
   */
  /* FUNC_INIT( WRAPPERNAME , {} ) */
      
      
      

  /**
   * Execution function
   * This function is called by the platform to do the real work of the wrapper. It may be
   * called concurrently, so be aware of not using shared or global data not protected by
   * a critical section.
   * This function has a mathematical meaning. It operates on one vector (aka point) and
   * returns another vector.
   *
   * This definition is MANDATORY.
   */
  FUNC_EXEC( WRAPPERNAME, {
      /* Adapt this section to your needs */

      long rc = 0;
      CALCUL_F77( INPOINT_ARRAY, & INPOINT_SIZE, OUTPOINT_ARRAY, & OUTPOINT_SIZE, & rc );
      if (rc) {
	SETERROR( "Error in CALCUL. Function has returned code %d.", rc );
	return WRAPPER_EXECUTION_ERROR;
      }
    } )
  
  /**
   * Finalization function
   * This function is called once just before the wrapper is unloaded. It is the place to flush
   * any output file or free any allocated memory. When this function returns, the wrapper is supposed
   * to have all its work done, so it is not possible to get anymore information from it after that.
   * Uncomment the following definition if you need to do some post-computation work. See FUNC_INIT. */
  /* FUNC_FINALIZE( WRAPPERNAME , {} ) */


WRAPPER_END
END_C_DECLS
