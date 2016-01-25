//                                               -*- C++ -*-
/**
 *  @brief A math expression parser
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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

#include "muParser.h"

using namespace mu;

BEGIN_NAMESPACE_OPENTURNS

class MuParser : public mu::Parser
{

public:
  /** Default constructor */
  MuParser();

protected:
  static mu::value_type Cotan(mu::value_type v);
  static mu::value_type ACotan(mu::value_type v);
  static mu::value_type ASinh(mu::value_type v);
  static mu::value_type ACosh(mu::value_type v);
  static mu::value_type ATanh(mu::value_type v);
  static mu::value_type Ln(mu::value_type v);
  static mu::value_type Log2(mu::value_type v);
  static mu::value_type LnGamma(mu::value_type v);
  static mu::value_type Gamma(mu::value_type v);
  static mu::value_type Erf(mu::value_type v);
  static mu::value_type Erfc(mu::value_type v);
  static mu::value_type Abs(mu::value_type v);
  static mu::value_type Cbrt(mu::value_type v);
  static mu::value_type J0(mu::value_type v);
  static mu::value_type J1(mu::value_type v);
  static mu::value_type Y0(mu::value_type v);
  static mu::value_type Y1(mu::value_type v);
  static mu::value_type Rint(mu::value_type v);
  static mu::value_type Floor(mu::value_type v);
  static mu::value_type Ceil(mu::value_type v);
  static mu::value_type Trunc(mu::value_type v);
  static mu::value_type Round(mu::value_type v);
};


AnalyticalParser::AnalyticalParser()
: Object()
{
}


void AnalyticalParser::setVariablesFormulas(const Description & inputVariablesNames,
                                    const Description & formulas)
{
  inputVariablesNames_ = inputVariablesNames;
  formulas_ = formulas;
}


void AnalyticalParser::initialize() const
{
  const UnsignedInteger inputDimension = inputVariablesNames_.getSize();
  const UnsignedInteger outputDimension = formulas_.getSize();
  if (parsers_.getSize() == outputDimension) return;
  parsers_ = Collection<Pointer<MuParser> >(outputDimension);
  inputStack_ = NumericalPoint(inputDimension);
  try
  {
    // For each parser of a formula, do
    for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++ outputIndex)
    {
      parsers_[outputIndex] = new MuParser;
      for (UnsignedInteger inputIndex = 0; inputIndex < inputDimension; ++ inputIndex)
      {
        // DefineVar defines all the values given to variables
        parsers_[outputIndex].get()->DefineVar(inputVariablesNames_[inputIndex].c_str(), &inputStack_[inputIndex]);
      }
      parsers_[outputIndex].get()->SetExpr(formulas_[outputIndex].c_str());
    }
  }
  catch (mu::Parser::exception_type & ex)
  {
    throw InvalidArgumentException(HERE) << "Error constructing an analytical function, message=" << ex.GetMsg() << " formula=" << ex.GetExpr() << " token=" << ex.GetToken() << " position=" << ex.GetPos();
  }
}


NumericalPoint AnalyticalParser::operator() (const NumericalPoint & inP) const
{
  const UnsignedInteger inputDimension = inputVariablesNames_.getSize();
  const UnsignedInteger outputDimension = formulas_.getSize();
  if (inP.getDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "Error: invalid input dimension (" << inP.getDimension() << ") expected " << inputDimension;
  initialize();
  std::copy(inP.begin(), inP.end(), inputStack_.begin());
  NumericalPoint result(outputDimension);
  try
  {
    for (UnsignedInteger outputIndex = 0; outputIndex < result.getDimension(); ++ outputIndex)
    {
      result[outputIndex] = parsers_[outputIndex].get()->Eval();
    }
  }
  catch (mu::Parser::exception_type & ex)
  {
    throw InternalException(HERE) << ex.GetMsg();
  }
  return result;
}


Description AnalyticalParser::getFormulas() const
{
  return formulas_;
}


MuParser::MuParser()
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

value_type MuParser::Cotan(value_type v)
{
  return 1.0 / tan(v);
}

value_type MuParser::ACotan(value_type v)
{
  if (v < 0.0) return -M_PI_2 - atan(v);
  return M_PI_2 - atan(v);
}

value_type MuParser::ASinh(value_type v)
{
  return asinh(v);
}

value_type MuParser::ACosh(value_type v)
{
  return acosh(v);
}

value_type MuParser::ATanh(value_type v)
{
  return atanh(v);
}

value_type MuParser::Ln(value_type v)
{
  return log(v);
}

value_type MuParser::Log2(value_type v)
{
  return log2(v);
}

value_type MuParser::LnGamma(value_type v)
{
  return lgamma(v);
}

value_type MuParser::Gamma(value_type v)
{
  return tgamma(v);
}

value_type MuParser::Erf(value_type v)
{
  return erf(v);
}

value_type MuParser::Erfc(value_type v)
{
  return erfc(v);
}

value_type MuParser::Abs(value_type v)
{
  return fabs(v);
}

value_type MuParser::Cbrt(value_type v)
{
  return cbrt(v);
}

value_type MuParser::J0(value_type v)
{
  return j0(v);
}

value_type MuParser::J1(value_type v)
{
  return j1(v);
}

value_type MuParser::Y0(value_type v)
{
  return y0(v);
}

value_type MuParser::Y1(value_type v)
{
  return y1(v);
}

value_type MuParser::Rint(value_type v)
{
  return rint(v);
}

value_type MuParser::Floor(value_type v)
{
  return floor(v);
}

value_type MuParser::Ceil(value_type v)
{
  return ceil(v);
}

value_type MuParser::Trunc(value_type v)
{
  return trunc(v);
}

value_type MuParser::Round(value_type v)
{
  return round(v);
}

END_NAMESPACE_OPENTURNS
