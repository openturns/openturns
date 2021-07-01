/*
 *  @brief Include windows.h cleanly
 */
#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
# define NOMINMAX
#endif
#include <windows.h>

// windows.h which defines a HUGE number of macros...

// wrappers
#undef GetClassName
#undef ERROR
#undef IN
#undef OUT

// ev3
#undef DIFFERENCE 
#undef CONST 
#undef interface 
#undef NONE 

#endif
