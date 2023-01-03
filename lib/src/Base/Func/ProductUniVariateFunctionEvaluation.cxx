//                                               -*- C++ -*-
/**
 *  @brief This is a nD function build as a product of n 1D function
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
#include "openturns/ProductUniVariateFunctionEvaluation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/TBBImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection<UniVariateFunction>)

static const Factory<PersistentCollection<UniVariateFunction> > Factory_PersistentCollection_UniVariateFunction;

CLASSNAMEINIT(ProductUniVariateFunctionEvaluation)

static const Factory<ProductUniVariateFunctionEvaluation> Factory_ProductUniVariateFunctionEvaluation;


/* Default constructor */
ProductUniVariateFunctionEvaluation::ProductUniVariateFunctionEvaluation()
  : EvaluationImplementation()
  , functions_()
{
  // Nothing to do
}


/* Constructor */
ProductUniVariateFunctionEvaluation::ProductUniVariateFunctionEvaluation(const UniVariateFunctionCollection & coll)
  : EvaluationImplementation()
  , functions_(coll)
{
  // Set the descriptions
  setInputDescription(Description::BuildDefault(getInputDimension(), "x"));
  setOutputDescription(Description::BuildDefault(getOutputDimension(), "y"));
}


/* Virtual constructor */
ProductUniVariateFunctionEvaluation * ProductUniVariateFunctionEvaluation::clone() const
{
  return new ProductUniVariateFunctionEvaluation(*this);
}


/* String converter */
String ProductUniVariateFunctionEvaluation::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " functions=" << functions_;
}

String ProductUniVariateFunctionEvaluation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << __repr__();
  return oss;
}


/* Operator (): Evaluate a product of 1D polynomials for one sample */
Point ProductUniVariateFunctionEvaluation::operator() (const Point & inP) const
{
  const UnsignedInteger inDimension = inP.getDimension();
  if (inDimension != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a ProductPolynomialFunction with an argument of invalid dimension";
  Scalar productEvaluation(1.0) ;
  for (UnsignedInteger i = 0; i < inDimension; ++ i) productEvaluation *= functions_[i](inP[i]);
  const Point result(1, productEvaluation);
  return result;
}

struct ProductUniVariateFunctionEvaluationComputeSamplePolicy
{
  const Sample & input_;
  Sample & output_;
  const ProductUniVariateFunctionEvaluation::UniVariateFunctionCollection & functions_;

  ProductUniVariateFunctionEvaluationComputeSamplePolicy(const Sample & input,
      Sample & output,
      const ProductUniVariateFunctionEvaluation::UniVariateFunctionCollection & functions)
    : input_(input)
    , output_(output)
    , functions_(functions)
  {
    // Nothing to do
  }

  inline void operator()(const TBBImplementation::BlockedRange<UnsignedInteger> & r) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++ i)
    {
      Scalar value = functions_[0](input_(i, 0));
      for (UnsignedInteger j = 1; j < functions_.getSize(); ++j)
        value *= functions_[j](input_(i, j));
      output_(i, 0) = value;
    } // i
  } // operator ()
};  // struct ProductUniVariateFunctionEvaluationComputeSamplePolicy

/* Operator (): Evaluate a product of 1D polynomials for one sample */
Sample ProductUniVariateFunctionEvaluation::operator() (const Sample & inS) const
{
  const UnsignedInteger inDimension = inS.getDimension();
  if (inDimension != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a ProductPolynomialFunction with an argument of invalid dimension";
  const UnsignedInteger size = inS.getSize();
  Sample result(size, getOutputDimension());
  const ProductUniVariateFunctionEvaluationComputeSamplePolicy policy( inS, result, functions_ );
  TBBImplementation::ParallelFor( 0, size, policy );
  result.setDescription(getOutputDescription());
  callsNumber_.fetchAndAdd(size);
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger ProductUniVariateFunctionEvaluation::getInputDimension() const
{
  return functions_.getSize();
}

/* Accessor for output point dimension */
UnsignedInteger ProductUniVariateFunctionEvaluation::getOutputDimension() const
{
  return 1;
}


/* Method save() stores the object through the StorageManager */
void ProductUniVariateFunctionEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute("functions_", functions_);
}

/* Method load() reloads the object from the StorageManager */
void ProductUniVariateFunctionEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute("functions_", functions_);
}


END_NAMESPACE_OPENTURNS
