//                                               -*- C++ -*-
/**
 *  @brief This is a nD polynomial build as a product of n 1D polynomial
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ProductPolynomialEvaluation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/TBBImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection<UniVariatePolynomial>)

static const Factory<PersistentCollection<UniVariatePolynomial> > Factory_PersistentCollection_UniVariatePolynomial;

CLASSNAMEINIT(ProductPolynomialEvaluation)

static const Factory<ProductPolynomialEvaluation> Factory_ProductPolynomialEvaluation;


/* Default constructor */
ProductPolynomialEvaluation::ProductPolynomialEvaluation()
  : EvaluationImplementation()
  , polynomials_()
{
  // Nothing to do
}


/* Constructor */
ProductPolynomialEvaluation::ProductPolynomialEvaluation(const PolynomialCollection & coll)
  : EvaluationImplementation()
  , polynomials_(coll)
{
  // Set the descriptions
  setInputDescription(Description::BuildDefault(getInputDimension(), "x"));
  setOutputDescription(Description::BuildDefault(getOutputDimension(), "y"));
}


/* Virtual constructor */
ProductPolynomialEvaluation * ProductPolynomialEvaluation::clone() const
{
  return new ProductPolynomialEvaluation(*this);
}


/* String converter */
String ProductPolynomialEvaluation::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << ", polynomials=" << polynomials_;
}

String ProductPolynomialEvaluation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset;
  const UnsignedInteger size = polynomials_.getSize();
  if (size == 0) return oss;
  const Description description(getInputDescription());
  if (size == 1) return (oss << polynomials_[0].__str__(description[0], ""));
  Bool allScalar = true;
  Scalar scalarValue = 1.0;
  Bool onlyOneNotScalar = false;
  UnsignedInteger indexNotScalar = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger degree = polynomials_[i].getDegree();
    const Bool isScalar = degree == 0;
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
    Bool first = scalarValueString == "1";
    // There is a non-unit factor
    if (!first) oss << scalarValue;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const UnsignedInteger degree = polynomials_[i].getDegree();
      // All the degree 0 factors have already been taken into account
      if (degree > 0)
      {
        if (!first) oss << " * ";
        // We count the number of non zeros coefficients
        const Point marginalCoefficeints = polynomials_[i].getCoefficients();
        UnsignedInteger numberOfNonZeros = 0;
        for (UnsignedInteger j = 0; j < marginalCoefficeints.getDimension(); ++j)
          if (marginalCoefficeints[j] != 0.0)
            numberOfNonZeros += 1;
        // We need parentheses if there are two non zero coefficients or more
        const Bool isNeedForParentheses = (numberOfNonZeros > 1);
        if (isNeedForParentheses)
          oss << "(" << polynomials_[i].__str__(description[i], "") << ")";
        else
          oss << polynomials_[i].__str__(description[i], "");
        first = false;
      }
    } // Loop over the factors
  } // At least two non-scalar factors
  return oss;
}

String ProductPolynomialEvaluation::_repr_html_() const
{
  OSS oss(false);
  const UnsignedInteger size = polynomials_.getSize();
  if (size == 0) return oss;
  const Description description(getInputDescription());
  if (size == 1) return (oss << polynomials_[0]._repr_html_(description[0]));
  Bool allScalar = true;
  Scalar scalarValue = 1.0;
  Bool onlyOneNotScalar = false;
  UnsignedInteger indexNotScalar = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger degree = polynomials_[i].getDegree();
    const Bool isScalar = degree == 0;
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
  else if (onlyOneNotScalar) oss << (polynomials_[indexNotScalar] * scalarValue)._repr_html_(description[indexNotScalar]);
  // At least two non-scalar factors
  else
  {
    const String scalarValueString(OSS(false) << scalarValue);
    Bool first = scalarValueString == "1";
    // There is a non-unit factor
    if (!first) oss << scalarValue;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const UnsignedInteger degree = polynomials_[i].getDegree();
      // All the degree 0 factors have already been taken into account
      if (degree > 0)
      {
        if (!first) oss << " <span>&#215;</span> ";        
        // We count the number of non zeros coefficients
        const Point marginalCoefficeints = polynomials_[i].getCoefficients();
        UnsignedInteger numberOfNonZeros = 0;
        for (UnsignedInteger j = 0; j < marginalCoefficeints.getDimension(); ++j)
          if (marginalCoefficeints[j] != 0.0)
            numberOfNonZeros += 1;
        // We need parentheses if there are two non zero coefficients or more
        const Bool isNeedForParentheses = (numberOfNonZeros > 1);
        if (isNeedForParentheses)
          oss << "(" << polynomials_[i]._repr_html_(description[i]) << ")";
        else
          oss << polynomials_[i]._repr_html_(description[i]);
        first = false;
      }
    } // Loop over the factors
  } // At least two non-scalar factors
  return oss;
}

/* Operator (): Evaluate a product of 1D polynomials for one sample */
Point ProductPolynomialEvaluation::operator() (const Point & inP) const
{
  const UnsignedInteger inDimension = inP.getDimension();
  if (inDimension != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a ProductPolynomialFunction with an argument of invalid dimension";
  Scalar productEvaluation(1.0) ;
  for (UnsignedInteger i = 0; i < inDimension; ++i) productEvaluation *= polynomials_[i](inP[i]);
  const Point result(1, productEvaluation);
  return result;
}

struct ProductPolynomialEvaluationComputeSamplePolicy
{
  const Sample & input_;
  Sample & output_;
  const ProductPolynomialEvaluation::PolynomialCollection & polynomials_;

  ProductPolynomialEvaluationComputeSamplePolicy(const Sample & input,
      Sample & output,
      const ProductPolynomialEvaluation::PolynomialCollection & polynomials)
    : input_(input)
    , output_(output)
    , polynomials_(polynomials)
  {
    // Nothing to do
  }

  inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      Scalar value = polynomials_[0](input_(i, 0));
      for (UnsignedInteger j = 1; j < polynomials_.getSize(); ++j)
        value *= polynomials_[j](input_(i, j));
      output_(i, 0) = value;
    } // i
  } // operator ()
};  // struct ProductPolynomialEvaluationComputeSamplePolicy

/* Operator (): Evaluate a product of 1D polynomials for one sample */
Sample ProductPolynomialEvaluation::operator() (const Sample & inS) const
{
  const UnsignedInteger inDimension = inS.getDimension();
  if (inDimension != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a ProductPolynomialFunction with an argument of invalid dimension";
  const UnsignedInteger size = inS.getSize();
  Sample result(size, getOutputDimension());
  const ProductPolynomialEvaluationComputeSamplePolicy policy( inS, result, polynomials_ );
  TBBImplementation::ParallelFor( 0, size, policy );
  result.setDescription(getOutputDescription());
  callsNumber_.fetchAndAdd(size);
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger ProductPolynomialEvaluation::getInputDimension() const
{
  return polynomials_.getSize();
}

/* Accessor for output point dimension */
UnsignedInteger ProductPolynomialEvaluation::getOutputDimension() const
{
  return 1;
}


/* Method save() stores the object through the StorageManager */
void ProductPolynomialEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "polynomials_", polynomials_ );
}

/* Method load() reloads the object from the StorageManager */
void ProductPolynomialEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "polynomials_", polynomials_ );
}


END_NAMESPACE_OPENTURNS
