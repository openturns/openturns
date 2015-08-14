//                                               -*- C++ -*-
/**
 *  @brief A math expression parser
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

#include "AnalyticalParser.hxx"
#include "SpecFunc.hxx"

using namespace mu;

BEGIN_NAMESPACE_OPENTURNS

AnalyticalParser::AnalyticalParser()
  : Parser()
{
  DefineFun(_T("cotan"), Cotan); // modified
  DefineFun(_T("acotan"), ACotan); // modified
  DefineFun(_T("asinh"), ASinh); // modified
  DefineFun(_T("acosh"), ACosh); // modified
  DefineFun(_T("atanh"), ATanh); // modified
  DefineFun(_T("log"), Ln); // modified: assigned to log10 by default
  DefineFun(_T("log2"), Log2); // modified
  DefineFun(_T("lngamma"), LnGamma); // added
  DefineFun(_T("gamma"), Gamma); // added
  DefineFun(_T("erf"), Erf); // added
  DefineFun(_T("erfc"), Erfc); // added
  DefineFun(_T("abs"), Abs); // modified
  DefineFun(_T("cbrt"), Cbrt); // added
  DefineFun(_T("besselJ0"), J0); // added
  DefineFun(_T("besselJ1"), J1); // added
  DefineFun(_T("besselY0"), Y0); // added
  DefineFun(_T("besselY1"), Y1); // added
  DefineFun(_T("rint"), Rint); // modified
  DefineFun(_T("floor"), Floor); // added
  DefineFun(_T("ceil"), Ceil); // added
  DefineFun(_T("trunc"), Trunc); // added
  DefineFun(_T("round"), Round); // added
}

value_type AnalyticalParser::Cotan(value_type v)
{
  return 1.0 / tan(v);
}

value_type AnalyticalParser::ACotan(value_type v)
{
  if (v < 0.0) return -M_PI_2 - atan(v);
  return M_PI_2 - atan(v);
}

value_type AnalyticalParser::ASinh(value_type v)
{
  return asinh(v);
}

value_type AnalyticalParser::ACosh(value_type v)
{
  return acosh(v);
}

value_type AnalyticalParser::ATanh(value_type v)
{
  return atanh(v);
}

value_type AnalyticalParser::Ln(value_type v)
{
  return log(v);
}

value_type AnalyticalParser::Log2(value_type v)
{
  return log2(v);
}

value_type AnalyticalParser::LnGamma(value_type v)
{
  return lgamma(v);
}

value_type AnalyticalParser::Gamma(value_type v)
{
  return tgamma(v);
}

value_type AnalyticalParser::Erf(value_type v)
{
  return erf(v);
}

value_type AnalyticalParser::Erfc(value_type v)
{
  return erfc(v);
}

value_type AnalyticalParser::Abs(value_type v)
{
  return fabs(v);
}

value_type AnalyticalParser::Cbrt(value_type v)
{
  return cbrt(v);
}

value_type AnalyticalParser::J0(value_type v)
{
  return j0(v);
}

value_type AnalyticalParser::J1(value_type v)
{
  return j1(v);
}

value_type AnalyticalParser::Y0(value_type v)
{
  return y0(v);
}

value_type AnalyticalParser::Y1(value_type v)
{
  return y1(v);
}

value_type AnalyticalParser::Rint(value_type v)
{
  return rint(v);
}

value_type AnalyticalParser::Floor(value_type v)
{
  return floor(v);
}

value_type AnalyticalParser::Ceil(value_type v)
{
  return ceil(v);
}

value_type AnalyticalParser::Trunc(value_type v)
{
  return trunc(v);
}

value_type AnalyticalParser::Round(value_type v)
{
  return round(v);
}

END_NAMESPACE_OPENTURNS
