//                                               -*- C++ -*-
/**
 *  @brief A math expression parser
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/SymbolicParserMuParser.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/TBBImplementation.hxx"

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

CLASSNAMEINIT(SymbolicParserMuParser);

static const Factory<SymbolicParserMuParser> Factory_SymbolicParserMuParser;


SymbolicParserMuParser::SymbolicParserMuParser()
  : SymbolicParserImplementation()
  , smallSize_(ResourceMap::GetAsUnsignedInteger("SymbolicParserMuParser-SmallSize"))
{
  // Nothing to do
}

/* Virtual constructor */
SymbolicParserMuParser * SymbolicParserMuParser::clone() const
{
  return new SymbolicParserMuParser(*this);
}

Collection<Pointer<MuParser > > SymbolicParserMuParser::allocateExpressions(Point & stack) const
{
  const UnsignedInteger inputDimension = inputVariablesNames_.getSize();
  const UnsignedInteger numberOfParsers = formulas_.getSize();
  stack = Point(inputDimension);
  Collection<Pointer<MuParser> > parsers(numberOfParsers);
  try
  {
    // For each parser of a formula, do
    for (UnsignedInteger outputIndex = 0; outputIndex < numberOfParsers; ++ outputIndex)
    {
      parsers[outputIndex] = new MuParser;
      for (UnsignedInteger inputIndex = 0; inputIndex < inputDimension; ++ inputIndex)
      {
        // DefineVar defines all the values given to variables
        parsers[outputIndex].get()->DefineVar(inputVariablesNames_[inputIndex].c_str(), &stack[inputIndex]);
      }
      parsers[outputIndex].get()->SetExpr(formulas_[outputIndex].c_str());
    }
  }
  catch (const mu::Parser::exception_type & ex)
  {
    throw InvalidArgumentException(HERE) << "Error constructing an analytical function, message=" << ex.GetMsg() << " formula=" << ex.GetExpr() << " token=" << ex.GetToken() << " position=" << ex.GetPos();
  }
  return parsers;
}

void SymbolicParserMuParser::initialize() const
{
  const UnsignedInteger numberOfParsers = formulas_.getSize();
  if (expressions_.getSize() == numberOfParsers) return;
  expressions_ = allocateExpressions(stack_);
}

Point SymbolicParserMuParser::operator() (const Point & inP) const
{
  const UnsignedInteger inputDimension = inputVariablesNames_.getSize();
  const UnsignedInteger outputDimension = formulas_.getSize();
  if (inP.getDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "Error: invalid input dimension (" << inP.getDimension() << ") expected " << inputDimension;
  if (outputDimension == 0) return Point();
  initialize();
  std::copy(inP.begin(), inP.end(), stack_.begin());
  Point result(outputDimension);
  try
  {
    for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++ outputIndex)
    {
      const Scalar value = expressions_[outputIndex]->Eval();
      // By default muParser is not compiled with MUP_MATH_EXCEPTIONS enabled and does not throw on domain/division errors
      if (checkOutput_ && !SpecFunc::IsNormal(value))
        throw InternalException(HERE) << "Cannot evaluate " << formulas_[outputIndex] << " at " << inputVariablesNames_.__str__() << "=" << inP.__str__();
      result[outputIndex] = value;
    } // outputIndex
  }
  catch (const mu::Parser::exception_type & ex)
  {
    throw InternalException(HERE) << ex.GetMsg();
  }
  return result;
}



struct SymbolicParserMuParserPolicy
{
  const SampleImplementation & input_;
  SampleImplementation & output_;
  const SymbolicParserMuParser & evaluation_;

  SymbolicParserMuParserPolicy(const Sample & input,
                               Sample & output,
                               const SymbolicParserMuParser & evaluation)
    : input_(*input.getImplementation())
    , output_(*output.getImplementation())
    , evaluation_(evaluation)
  {}

  inline void operator()(const TBBImplementation::BlockedRange<UnsignedInteger> & r) const
  {
    const UnsignedInteger threadIndex = TBBImplementation::GetThreadIndex();
    if (!evaluation_.threadStack_[threadIndex].getDimension())
      evaluation_.threadExpressions_[threadIndex] = evaluation_.allocateExpressions(evaluation_.threadStack_[threadIndex]);

    const UnsignedInteger inputDimension = evaluation_.inputVariablesNames_.getSize();
    const UnsignedInteger outputDimension = evaluation_.formulas_.getSize();

    for (UnsignedInteger i = r.begin(); i != r.end(); ++ i)
    {
      std::copy(&input_(i, 0), &input_(i, 0) + inputDimension, evaluation_.threadStack_[threadIndex].begin());

      try
      {
        for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++ outputIndex)
        {
          const Scalar value = evaluation_.threadExpressions_[threadIndex][outputIndex]->Eval();
          // By default muParser is not compiled with MUP_MATH_EXCEPTIONS enabled and does not throw on domain/division errors
          if (evaluation_.checkOutput_ && !SpecFunc::IsNormal(value))
            throw InternalException(HERE) << "Cannot evaluate " << evaluation_.formulas_[outputIndex] << " at " << evaluation_.inputVariablesNames_.__str__() << "=" << Point(input_[i]).__str__();
          output_(i, outputIndex) = value;
        }
      }
      catch (const mu::Parser::exception_type & ex)
      {
        throw InternalException(HERE) << ex.GetMsg();
      }
    }
  }

}; /* end struct SymbolicParserMuParserPolicy */


Sample SymbolicParserMuParser::operator() (const Sample & inS) const
{
  const UnsignedInteger inputDimension = inputVariablesNames_.getSize();
  const UnsignedInteger outputDimension(formulas_.getSize());
  if (inS.getDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "Error: invalid input dimension (" << inS.getDimension() << ") expected " << inputDimension;
  if (outputDimension == 0) return Sample(inS.getSize(), 0);
  initialize();
  const UnsignedInteger size = inS.getSize();
  Sample result(size, outputDimension);
  if (size < smallSize_)
  {
    // account for the penalty on small samples
    for (UnsignedInteger i = 0; i < size; ++ i)
      result[i] = operator()(inS[i]);
  }
  else
  {
    if (threadExpressions_.getSize() != TBBImplementation::GetThreadsNumber())
    {
      threadExpressions_.resize(TBBImplementation::GetThreadsNumber());
      threadStack_.resize(TBBImplementation::GetThreadsNumber());
    }
    const SymbolicParserMuParserPolicy policy(inS, result, *this);
    TBBImplementation::ParallelFor(0, size, policy);
  }
  return result;
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
  DefineConst(_T("e_"), 2.71828182845904523536028747135266249775724709369996);
  DefineConst(_T("pi_"), 3.14159265358979323846264338327950288419716939937510);
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
