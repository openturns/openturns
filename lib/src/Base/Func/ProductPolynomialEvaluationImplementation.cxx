//                                               -*- C++ -*-
/**
 *  @brief This is a nD polynomial build as a product of n 1D polynomial
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
#include "ProductPolynomialEvaluationImplementation.hxx"
#include "OSS.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection<UniVariatePolynomial>);

static const Factory<PersistentCollection<UniVariatePolynomial> > RegisteredFactory_PC_UVP;

CLASSNAMEINIT(ProductPolynomialEvaluationImplementation);

static const Factory<ProductPolynomialEvaluationImplementation> RegisteredFactory_PPEI;


/* Default constructor */
ProductPolynomialEvaluationImplementation::ProductPolynomialEvaluationImplementation()
  : NumericalMathEvaluationImplementation()
  , polynomials_()
{
  // Nothing to do
}


/* Constructor */
ProductPolynomialEvaluationImplementation::ProductPolynomialEvaluationImplementation(const PolynomialCollection & coll)
  : NumericalMathEvaluationImplementation()
  , polynomials_(coll)
{
  // Set the descriptions
  setInputDescription(Description::BuildDefault(getInputDimension(), "x"));
  setOutputDescription(Description::BuildDefault(getOutputDimension(), "y"));
}


/* Virtual constructor */
ProductPolynomialEvaluationImplementation * ProductPolynomialEvaluationImplementation::clone() const
{
  return new ProductPolynomialEvaluationImplementation(*this);
}


/* String converter */
String ProductPolynomialEvaluationImplementation::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << ", polynomials=" << polynomials_;
}

String ProductPolynomialEvaluationImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset;
  const UnsignedInteger size(polynomials_.getSize());
  if (size == 0) return oss;
  const Description description(getInputDescription());
  if (size == 1) return (oss << polynomials_[0].__str__(description[0], ""));
  Bool allScalar(true);
  NumericalScalar scalarValue(1.0);
  Bool onlyOneNotScalar(false);
  UnsignedInteger indexNotScalar(0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger degree(polynomials_[i].getDegree());
    const Bool isScalar(degree == 0);
    // Only one non-scalar so far, and the current one is not scalar
    if (onlyOneNotScalar && !isScalar) onlyOneNotScalar = false;
    // Only scalars so far, and the current is not scalar
    if (allScalar && !isScalar)
    {
      onlyOneNotScalar = true;
      indexNotScalar = i;
    }
    if (isScalar) scalarValue *= polynomials_[i].getCoefficients()[0];
    allScalar = allScalar && isScalar;
  }
  // Scalar polynomial
  if (allScalar) oss << scalarValue;
  // Only one no unit polynomial in the product
  else if (onlyOneNotScalar) oss << (polynomials_[indexNotScalar] * scalarValue).__str__(description[indexNotScalar], "");
  // At least two non-scalar factors
  else
  {
    const String scalarValueString(OSS(false) << scalarValue);
    Bool first(scalarValueString == "1");
    // There is a non-unit factor
    if (!first) oss << scalarValue;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const UnsignedInteger degree(polynomials_[i].getDegree());
      // All the degree 0 factors have already been taken into account
      if (degree > 0)
      {
        if (!first) oss << " * ";
        oss << "(" << polynomials_[i].__str__(description[i], "") << ")";
        first = false;
      }
    } // Loop over the factors
  } // At least two non-scalar factors
  return oss;
}


/* Operator (): Evaluate a product of 1D polynomials for one sample */
NumericalPoint ProductPolynomialEvaluationImplementation::operator() (const NumericalPoint & inP) const
{
  const UnsignedInteger inDimension(inP.getDimension());
  if (inDimension != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a ProductPolynomialFunction with an argument of invalid dimension";
  NumericalScalar productEvaluation(1.0) ;
  for (UnsignedInteger i = 0; i < inDimension; ++i) productEvaluation *= polynomials_[i](inP[i]);
  const NumericalPoint result(1, productEvaluation);
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}

struct ProductPolynomialEvaluationComputeSamplePolicy
{
  const NumericalSample & input_;
  NumericalSample & output_;
  const ProductPolynomialEvaluationImplementation::PolynomialCollection & polynomials_;

  ProductPolynomialEvaluationComputeSamplePolicy(const NumericalSample & input,
      NumericalSample & output,
      const ProductPolynomialEvaluationImplementation::PolynomialCollection & polynomials)
    : input_(input)
    , output_(output)
    , polynomials_(polynomials)
  {
    // Nothing to do
  }

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      NumericalScalar value(polynomials_[0](input_[i][0]));
      for (UnsignedInteger j = 1; j < polynomials_.getSize(); ++j)
        value *= polynomials_[j](input_[i][j]);
      output_[i][0] = value;
    } // i
  } // operator ()
};  // struct ProductPolynomialEvaluationComputeSamplePolicy

/* Operator (): Evaluate a product of 1D polynomials for one sample */
NumericalSample ProductPolynomialEvaluationImplementation::operator() (const NumericalSample & inS) const
{
  const UnsignedInteger inDimension(inS.getDimension());
  if (inDimension != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a ProductPolynomialFunction with an argument of invalid dimension";
  const UnsignedInteger size(inS.getSize());
  NumericalSample result(size, getOutputDimension());
  const ProductPolynomialEvaluationComputeSamplePolicy policy( inS, result, polynomials_ );
  TBB::ParallelFor( 0, size, policy );
  result.setDescription(getOutputDescription());
  callsNumber_ += size;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inS);
    outputStrategy_.store(result);
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger ProductPolynomialEvaluationImplementation::getInputDimension() const
{
  return polynomials_.getSize();
}

/* Accessor for output point dimension */
UnsignedInteger ProductPolynomialEvaluationImplementation::getOutputDimension() const
{
  return 1;
}


/* Method save() stores the object through the StorageManager */
void ProductPolynomialEvaluationImplementation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute( "polynomials_", polynomials_ );
}

/* Method load() reloads the object from the StorageManager */
void ProductPolynomialEvaluationImplementation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute( "polynomials_", polynomials_ );
}


END_NAMESPACE_OPENTURNS
