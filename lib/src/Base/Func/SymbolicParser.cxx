//                                               -*- C++ -*-
/**
 *  @brief A math expression parser
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/SymbolicParser.hxx"
#include "openturns/SpecFunc.hxx"

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
  static mu::value_type Not(mu::value_type v);
  static mu::value_type Xor(mu::value_type v1, mu::value_type v2);
};


SymbolicParser::SymbolicParser()
  : Object()
{
  // Nothing to do
}


void SymbolicParser::setVariablesFormulas(const Description & inputVariablesNames,
    const Description & formulas)
{
  inputVariablesNames_ = inputVariablesNames;
  formulas_ = formulas;
}


void SymbolicParser::initialize(const UnsignedInteger size) const
{
  const UnsignedInteger inputDimension = inputVariablesNames_.getSize();
  const UnsignedInteger outputDimension = formulas_.getSize();
  if (parsers_.getSize() == outputDimension && inputStack_.getDimension() >= size && inputStack_.getSize() > 0) return;
  inputStack_ = Sample(inputDimension, size);
  parsers_ = Collection<Pointer<MuParser> >(outputDimension);
  try
  {
    // For each parser of a formula, do
    for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++ outputIndex)
    {
      parsers_[outputIndex] = new MuParser;
      for (UnsignedInteger inputIndex = 0; inputIndex < inputDimension; ++ inputIndex)
      {
        // DefineVar defines all the values given to variables
        parsers_[outputIndex].get()->DefineVar(inputVariablesNames_[inputIndex].c_str(), &inputStack_(inputIndex, 0));
      }
      parsers_[outputIndex].get()->SetExpr(formulas_[outputIndex].c_str());
    }
  }
  catch (mu::Parser::exception_type & ex)
  {
    throw InvalidArgumentException(HERE) << "Error constructing an analytical function, message=" << ex.GetMsg() << " formula=" << ex.GetExpr() << " token=" << ex.GetToken() << " position=" << ex.GetPos();
  }
}


Point SymbolicParser::operator() (const Point & inP) const
{
  const UnsignedInteger inputDimension = inputVariablesNames_.getSize();
  const UnsignedInteger outputDimension = formulas_.getSize();
  if (inP.getDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "Error: invalid input dimension (" << inP.getDimension() << ") expected " << inputDimension;
  initialize(1);
  std::copy(inP.begin(), inP.end(), &inputStack_(0, 0));
  Point result(outputDimension);
  try
  {
    for (UnsignedInteger outputIndex = 0; outputIndex < result.getDimension(); ++ outputIndex)
    {
      const Scalar value = parsers_[outputIndex].get()->Eval();
      // By default muParser is not compiled with MUP_MATH_EXCEPTIONS enabled and does not throw on domain/division errors
      if (!SpecFunc::IsNormal(value))
        throw InternalException(HERE) << "Cannot evaluate " << formulas_[outputIndex] << " at " << inputVariablesNames_.__str__() << "=" << inP.__str__();
      result[outputIndex] = value;
    }
  }
  catch (mu::Parser::exception_type & ex)
  {
    throw InternalException(HERE) << ex.GetMsg();
  }
  return result;
}


Sample SymbolicParser::operator() (const Sample & inSample) const
{
  const UnsignedInteger inputDimension = inputVariablesNames_.getSize();
  const UnsignedInteger outputDimension = formulas_.getSize();
  if (inSample.getDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "Error: invalid input dimension (" << inSample.getDimension() << ") expected " << inputDimension;
  const UnsignedInteger size = inSample.getSize();
  initialize(size);
  for(UnsignedInteger i = 0; i < inputDimension; ++ i)
  {
    const Point inP(inSample.getMarginal(i).asPoint());
    std::copy(inP.begin(), inP.end(), &inputStack_(i, 0));
  }
  SampleImplementation transposed(outputDimension, size);
  Sample result(size, outputDimension);
  SampleImplementation & resultImpl(*result.getImplementation());
  try
  {
    for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++ outputIndex)
    {
      parsers_[outputIndex].get()->Eval(&transposed(outputIndex, 0), size);
    }
  }
  catch (mu::Parser::exception_type & ex)
  {
    throw InternalException(HERE) << ex.GetMsg();
  }
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++ outputIndex)
    {
      resultImpl(i, outputIndex) = transposed(outputIndex, i);
    }
  }
  return result;
}


Description SymbolicParser::getFormulas() const
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
  DefineInfixOprt(_T("!"), Not); // added
  DefineOprt(_T("^^"), Xor, 0, mu::oaLEFT, true); // added
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
  return std::abs(v);
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

value_type MuParser::Not(value_type v)
{
  return v == 0;
}

value_type MuParser::Xor(value_type v1, value_type v2)
{
  return (v1 == static_cast<value_type>(0)) ^ (v2 == static_cast<value_type>(0));
}

END_NAMESPACE_OPENTURNS
