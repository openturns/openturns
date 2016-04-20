//                                               -*- C++ -*-
/**
 *  @brief This is a nD function build as a product of n 1D function
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
#include "openturns/ProductUniVariateFunctionEvaluationImplementation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection<UniVariateFunction>);

static const Factory<PersistentCollection<UniVariateFunction> > RegisteredFactory_PC_UVF;

CLASSNAMEINIT(ProductUniVariateFunctionEvaluationImplementation);

static const Factory<ProductUniVariateFunctionEvaluationImplementation> RegisteredFactory_PUVFEI;


/* Default constructor */
ProductUniVariateFunctionEvaluationImplementation::ProductUniVariateFunctionEvaluationImplementation()
  : NumericalMathEvaluationImplementation()
  , functions_()
{
  // Nothing to do
}


/* Constructor */
ProductUniVariateFunctionEvaluationImplementation::ProductUniVariateFunctionEvaluationImplementation(const UniVariateFunctionCollection & coll)
  : NumericalMathEvaluationImplementation()
  , functions_(coll)
{
  // Set the descriptions
  setInputDescription(Description::BuildDefault(getInputDimension(), "x"));
  setOutputDescription(Description::BuildDefault(getOutputDimension(), "y"));
}


/* Virtual constructor */
ProductUniVariateFunctionEvaluationImplementation * ProductUniVariateFunctionEvaluationImplementation::clone() const
{
  return new ProductUniVariateFunctionEvaluationImplementation(*this);
}


/* String converter */
String ProductUniVariateFunctionEvaluationImplementation::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " functions=" << functions_;
}

String ProductUniVariateFunctionEvaluationImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << __repr__();
  return oss;
}


/* Operator (): Evaluate a product of 1D polynomials for one sample */
NumericalPoint ProductUniVariateFunctionEvaluationImplementation::operator() (const NumericalPoint & inP) const
{
  const UnsignedInteger inDimension = inP.getDimension();
  if (inDimension != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a ProductPolynomialFunction with an argument of invalid dimension";
  NumericalScalar productEvaluation(1.0) ;
  for (UnsignedInteger i = 0; i < inDimension; ++ i) productEvaluation *= functions_[i](inP[i]);
  const NumericalPoint result(1, productEvaluation);
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}

struct ProductUniVariateFunctionEvaluationComputeSamplePolicy
{
  const NumericalSample & input_;
  NumericalSample & output_;
  const ProductUniVariateFunctionEvaluationImplementation::UniVariateFunctionCollection & functions_;

  ProductUniVariateFunctionEvaluationComputeSamplePolicy(const NumericalSample & input,
      NumericalSample & output,
      const ProductUniVariateFunctionEvaluationImplementation::UniVariateFunctionCollection & functions)
    : input_(input)
    , output_(output)
    , functions_(functions)
  {
    // Nothing to do
  }

  inline void operator()(const TBB::BlockedRange<UnsignedInteger> & r) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++ i)
    {
      NumericalScalar value = functions_[0](input_[i][0]);
      for (UnsignedInteger j = 1; j < functions_.getSize(); ++j)
        value *= functions_[j](input_[i][j]);
      output_[i][0] = value;
    } // i
  } // operator ()
};  // struct ProductUniVariateFunctionEvaluationComputeSamplePolicy

/* Operator (): Evaluate a product of 1D polynomials for one sample */
NumericalSample ProductUniVariateFunctionEvaluationImplementation::operator() (const NumericalSample & inS) const
{
  const UnsignedInteger inDimension = inS.getDimension();
  if (inDimension != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a ProductPolynomialFunction with an argument of invalid dimension";
  const UnsignedInteger size = inS.getSize();
  NumericalSample result(size, getOutputDimension());
  const ProductUniVariateFunctionEvaluationComputeSamplePolicy policy( inS, result, functions_ );
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
UnsignedInteger ProductUniVariateFunctionEvaluationImplementation::getInputDimension() const
{
  return functions_.getSize();
}

/* Accessor for output point dimension */
UnsignedInteger ProductUniVariateFunctionEvaluationImplementation::getOutputDimension() const
{
  return 1;
}


/* Method save() stores the object through the StorageManager */
void ProductUniVariateFunctionEvaluationImplementation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute("functions_", functions_);
}

/* Method load() reloads the object from the StorageManager */
void ProductUniVariateFunctionEvaluationImplementation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute("functions_", functions_);
}


END_NAMESPACE_OPENTURNS
