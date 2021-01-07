//                                               -*- C++ -*-
/**
 * @brief This file provides definitions for tty control sequences
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "openturns/TTY.hxx"

BEGIN_NAMESPACE_OPENTURNS

#ifndef WIN32
static volatile Bool Colored( true  ); // default is colored output
#else
static volatile Bool Colored( false ); // default is standard output
#endif

static const char * ColorMap[] =
{
  /* ISO 6429 color sequences */
  "\033[0m", /* DEFAULT */
  "\033[1m", /* BOLD */
  "\033[4m", /* UNDERLINE */
  "\033[5m", /* BLINK */

  /* Foreground */
  "\033[30m", /* BLACKFG */
  "\033[31m", /* REDFG */
  "\033[32m", /* GREENFG */
  "\033[33m", /* YELLOWFG */
  "\033[34m", /* BLUEFG */
  "\033[35m", /* PURPLEFG */
  "\033[36m", /* CYANFG */
  "\033[37m", /* WHITEFG */

  /* Background */
  "\033[30m", /* BLACKBG */
  "\033[31m", /* REDBG */
  "\033[32m", /* GREENBG */
  "\033[33m", /* YELLOWBG */
  "\033[34m", /* BLUEBG */
  "\033[35m", /* PURPLEBG */
  "\033[36m", /* CYANBG */
  "\033[37m", /* WHITEBG */
};

/* Return a control sequence corresponding to the current color */
const char * TTY::GetColor( Color c )
{
  if ( !Colored || (c < 0) || (c >= LASTCOLOR) ) return "";
  return ColorMap[ c ];
}

/* Define the colorization strategy. Flag = true if colored output */
/* Q: Do we need to perform MT-safe programming ? Not sure. Ivan   */
void TTY::ShowColors( Bool flag )
{
  Colored = flag;
}

/* Query the colorization strategy. Return true if colored output */
Bool TTY::ColoredOutput()
{
  return Colored;
}
END_NAMESPACE_OPENTURNS
