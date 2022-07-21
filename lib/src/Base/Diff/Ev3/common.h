//                                               -*- C++ -*-
/**
 *  @brief common stuff
 *
 *  Copyright (C) 2008-2010 Leo Liberti
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

#ifndef __EV3COMMONH__
#define __EV3COMMONH__

#define NOVARIABLE -1
#define LARGE 1E10
#define NOTVARNAME "_var_not_found_"

namespace Ev3
{
typedef int Int;

// various operator types
enum OperatorType
{
  SUM, DIFFERENCE, PRODUCT, FRACTION, POWER,
  PLUS, MINUS, SIN, COS, TAN, ASIN, ACOS,
  ATAN, SINH, COSH, TANH, ASINH, ACOSH,
  ATANH, LOG2, LOG10, LOG, LN, LNGAMMA,
  GAMMA, EXP, ERF, ERFC, SQRT, CBRT,
  BESSELJ0, BESSELJ1, BESSELY0, BESSELY1,
  SIGN, RINT, ABS, COT, COTH,
  VAR, CONST,
  ERROR
};

// utility functions
extern double Ev3NearZero();
extern double Ev3Infinity();

} /* namespace Ev3 */

#ifdef _MSC_VER
template <typename T>
T log2(T arg)
{
  return log(arg) / log(2.0);
}
#endif

#endif /* __EV3COMMONH__ */
