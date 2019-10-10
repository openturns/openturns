//                                               -*- C++ -*-
/**
 *  @brief The evaluation part of functional linear combination of vectors
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/DualLinearCombinationEvaluation.hxx"
#include "openturns/LinearCombinationEvaluation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Description.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DualLinearCombinationEvaluation)

static const Factory<DualLinearCombinationEvaluation> Factory_DualLinearCombinationEvaluation;


/* Default constructor */
DualLinearCombinationEvaluation::DualLinearCombinationEvaluation()
  : EvaluationImplementation()
  , functionsCollection_(0)
  , coefficients_(0, 0)
{
  // Nothing to do
}


/* Parameters constructor */
DualLinearCombinationEvaluation::DualLinearCombinationEvaluation(const FunctionCollection & functionsCollection,
    const Sample & coefficients)
  : EvaluationImplementation()
  , functionsCollection_(0)
  , coefficients_(0, 0)
{
  setFunctionsCollectionAndCoefficients(functionsCollection, coefficients);
}


/* Virtual constructor */
DualLinearCombinationEvaluation * DualLinearCombinationEvaluation::clone() const
{
  return new DualLinearCombinationEvaluation(*this);
}

/* Description accessor */
void DualLinearCombinationEvaluation::setDescription(const Description & description)
{
  EvaluationImplementation::setDescription(description);
  const Description inputDescription(getInputDescription());
  const UnsignedInteger inputDimension = getInputDimension();
  const UnsignedInteger size = functionsCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Description atomDescription(functionsCollection_[i].getDescription());
    for (UnsignedInteger j = 0; j < inputDimension; ++j) atomDescription[j] = inputDescription[j];
    functionsCollection_[i].setDescription(atomDescription);
  }
}

/* Get the i-th marginal function */
Evaluation DualLinearCombinationEvaluation::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  // We use a LinearCombinationEvaluation instead of a DualLinearCombinationEvaluation as it is more efficient and more easy to read
  const UnsignedInteger size = coefficients_.getSize();
  Point marginalCoefficients(size);
  for (UnsignedInteger marginalIndex = 0; marginalIndex < size; ++marginalIndex) marginalCoefficients[marginalIndex] = coefficients_(marginalIndex, i);
  return new LinearCombinationEvaluation(functionsCollection_, marginalCoefficients);
}

/* Get the function corresponding to indices components */
Evaluation DualLinearCombinationEvaluation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "The indices of a marginal function must be in the range [0, dim-1] and must be different";
  // Special case for 1D marginal
  if (indices.getSize() == 1) return getMarginal(indices[0]);
  return new DualLinearCombinationEvaluation(functionsCollection_, coefficients_.getMarginal(indices));
}

/* String converter */
String DualLinearCombinationEvaluation::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " functions=" << functionsCollection_
         << " coefficients=" << coefficients_.__repr__();
}

String DualLinearCombinationEvaluation::__str__(const String & ) const
{
  OSS oss(false);
  const UnsignedInteger size = functionsCollection_.getSize();
  const UnsignedInteger outputDimension = getOutputDimension();
  Bool first = true;
  static const String valid("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_()[]{}^*/");
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (outputDimension == 1)
    {
      const Scalar value = coefficients_(i, 0);
      if (value != 0.0)
      {
        if (first) oss << value;
        else if (value > 0.0) oss << " + " << value;
        else oss << " - " << -value;
        first = false;
        const String expr(functionsCollection_[i].getEvaluation().getImplementation()->__str__());
        // Print the function factor only if it is different from 1
        if (expr != "1")
        {
          oss << " * ";

          const Bool complexString = expr.find_first_not_of(valid) != String::npos;
          if (complexString) oss << "(";
          oss << expr;
          if (complexString) oss << ")";
        }
      }
    } // output dimension == 1
    else
    {
      if (first) oss << coefficients_[i];
      else oss << " + " << coefficients_[i];
      first = false;
      const String expr(functionsCollection_[i].getEvaluation().getImplementation()->__str__());
      if (expr != "1")
      {
        oss << " * ";

        const Bool complexString = expr.find_first_not_of(valid) != String::npos;
        if (complexString) oss << "(";
        oss << expr;
        if (complexString) oss << ")";
      }
    }
  }
  return oss;
}

// Helper for the parallel version of the point-based evaluation operator
struct DualLinearCombinationEvaluationPointFunctor
{
  const Point & input_;
  const DualLinearCombinationEvaluation & evaluation_;
  Point accumulator_;

  DualLinearCombinationEvaluationPointFunctor(const Point & input,
      const DualLinearCombinationEvaluation & evaluation)
    : input_(input)
    , evaluation_(evaluation)
    , accumulator_(Point(evaluation.getOutputDimension()))
  {}

  DualLinearCombinationEvaluationPointFunctor(const DualLinearCombinationEvaluationPointFunctor & other,
      TBB::Split)
    : input_(other.input_)
    , evaluation_(other.evaluation_)
    , accumulator_(Point(other.accumulator_.getDimension()))
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r )
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i) accumulator_ += evaluation_.coefficients_[i] * evaluation_.functionsCollection_[i](input_)[0];
  } // operator()

  inline void join(const DualLinearCombinationEvaluationPointFunctor & other)
  {
    accumulator_ += other.accumulator_;
  }

}; // struct DualLinearCombinationEvaluationPointFunctor

/* Evaluation operator */
Point DualLinearCombinationEvaluation::operator () (const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  const UnsignedInteger size = functionsCollection_.getSize();
  DualLinearCombinationEvaluationPointFunctor functor( inP, *this );
  TBB::ParallelReduce( 0, size, functor );
  const Point result(functor.accumulator_);
  callsNumber_.increment();
  return result;
}

Sample DualLinearCombinationEvaluation::operator () (const Sample & inS) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inS.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inS.getDimension();
  const UnsignedInteger sampleSize = inS.getSize();
  Sample result(sampleSize, getOutputDimension());
  result.setDescription(getOutputDescription());
  if (sampleSize == 0) return result;
  const UnsignedInteger size = functionsCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // Exploit possible parallelism in the basis functions
    const Sample basisSample(functionsCollection_[i](inS));
    // Should be parallelized
    for (UnsignedInteger j = 0; j < sampleSize; ++j) result[j] += coefficients_[i] * basisSample(j, 0);
  }
  callsNumber_.fetchAndAdd(sampleSize);
  return result;
}

/* Coefficients accessor */
Sample DualLinearCombinationEvaluation::getCoefficients() const
{
  return coefficients_;
}

/* Functions accessor */
DualLinearCombinationEvaluation::FunctionCollection DualLinearCombinationEvaluation::getFunctionsCollection() const
{
  return functionsCollection_;
}

void DualLinearCombinationEvaluation::setFunctionsCollectionAndCoefficients(const FunctionCollection & functionsCollection,
    const Sample & coefficients)
{
  const UnsignedInteger size = functionsCollection.getSize();
  // Check for empty functions collection
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a linear combination from an empty collection of functions.";
  // Check for incompatible number of functions and coefficients
  if (size != coefficients.getSize()) throw InvalidArgumentException(HERE) << "Error: cannot build a linear combination with a different number of functions and coefficients.";
  // Check for coherent input and output dimensions of the functions
  UnsignedInteger inputDimension = functionsCollection[0].getInputDimension();
  for (UnsignedInteger i = 1; i < size; ++i)
  {
    if (functionsCollection[i].getInputDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given functions have incompatible input dimension.";
    if (functionsCollection[i].getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given functions must have a one dimensional output.";
  }
  // Keep only the non zero coefficients
  coefficients_ = Sample(0, coefficients.getDimension());
  functionsCollection_ = FunctionCollection(0);
  // First pass, extract the maximum absolute coefficient
  Scalar maximumAbsoluteCoefficient = 0.0;
  Point absoluteCoefficients(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // Must cast the NSI_const_point into a Point to use the normInf() method.
    absoluteCoefficients[i] = Point(coefficients[i]).normInf();
    if (absoluteCoefficients[i] > maximumAbsoluteCoefficient) maximumAbsoluteCoefficient = absoluteCoefficients[i];
  }
  if (maximumAbsoluteCoefficient == 0.0) throw InvalidArgumentException(HERE) << "Error: all the coefficients are zero.";
  // Second pass, remove the small coefficients
  const Scalar epsilon = maximumAbsoluteCoefficient * ResourceMap::GetAsScalar("DualLinearCombinationEvaluation-SmallCoefficient");
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (absoluteCoefficients[i] > epsilon)
    {
      Point currentCoefficient(coefficients[i]);
      for (UnsignedInteger j = 0; j < currentCoefficient.getDimension(); ++j)
        if (std::abs(currentCoefficient[j]) <= epsilon)
        {
          currentCoefficient[j] = 0.0;
          LOGWARN(OSS() << "set the component " << j << " of contributor " << i << "=" << currentCoefficient[j] << " to zero as it is too small");
        }
      coefficients_.add(currentCoefficient);
      functionsCollection_.add(functionsCollection[i]);
    }
    else LOGWARN(OSS() << "removed the contributor " << i << "=" << functionsCollection[i] << " from the linear combination as its coefficient is too small.");
  }
  Description description(0);
  Description inputDescription(functionsCollection[0].getInputDescription());
  for (UnsignedInteger i = 0; i < inputDescription.getSize(); ++i)
  {
    if (inputDescription[i].size() == 0) description.add(OSS() << "x" << i);
    else description.add(inputDescription[i]);
  }
  Description outputDescription(coefficients.getDescription());
  for (UnsignedInteger i = 0; i < outputDescription.getSize(); ++i)
  {
    if (outputDescription[i].size() == 0) description.add(OSS() << "y" << i);
    else description.add(outputDescription[i]);
  }
  setDescription(description);
}


/* Input dimension accessor */
UnsignedInteger DualLinearCombinationEvaluation::getInputDimension() const
{
  if (functionsCollection_.getSize() == 0) return 0;
  return functionsCollection_[0].getInputDimension();
}

/* Output dimension accessor */
UnsignedInteger DualLinearCombinationEvaluation::getOutputDimension() const
{
  return coefficients_.getDimension();
}


/* Gradient according to the marginal parameters */
Matrix DualLinearCombinationEvaluation::parameterGradient(const Point & ) const
{
  Matrix result(getParameter().getDimension(), getOutputDimension());
  // const UnsignedInteger size(functionsCollection_.getSize());
  // // Get the parameters gradients for each atom and stack them into the result
  // UnsignedInteger rowIndex(0);
  // for (UnsignedInteger i = 0; i < size; ++i)
  //   {
  //     // Extract the atom gradient
  //     const Matrix atomParametersGradient(functionsCollection_[i].parameterGradient(inP));
  //     const UnsignedInteger rowDimension(atomParametersGradient.getNbRows());
  //     const UnsignedInteger columnDimension(atomParametersGradient.getNbColumns());
  //     // Scale the atom gradient and copy it into the result
  //     const Scalar coefficient(coefficients_[i]);
  //     for (UnsignedInteger j = 0; j < rowDimension; ++j)
  //       {
  //         for (UnsignedInteger k = 0; k < columnDimension; ++k)
  //           {
  //             result(rowIndex, k) = coefficient * atomParametersGradient(j, k);
  //           }
  //         ++rowIndex;
  //       }
  //   }
  return result;
}

/* Parameters value accessor */
Point DualLinearCombinationEvaluation::getParameter() const
{
  Point parameter(0);
  const UnsignedInteger size = functionsCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    parameter.add(functionsCollection_[i].getParameter());
  }
  return parameter;
}

void DualLinearCombinationEvaluation::setParameter(const Point & parameter)
{
  const UnsignedInteger size = functionsCollection_.getSize();
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    Point marginalParameter(functionsCollection_[i].getParameter());
    const UnsignedInteger marginalDimension = marginalParameter.getDimension();
    for (UnsignedInteger j = 0; j < marginalDimension; ++ j)
    {
      marginalParameter[j] = parameter[index];
      ++ index;
    }
    functionsCollection_[i].setParameter(marginalParameter);
  }
}

/* Parameters description accessor */
Description DualLinearCombinationEvaluation::getParameterDescription() const
{
  Description description;
  const UnsignedInteger size = functionsCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    description.add(functionsCollection_[i].getParameterDescription());
  }
  return description;
}

/* Linearity accessors */
Bool DualLinearCombinationEvaluation::isLinear() const
{ 
  for (UnsignedInteger i=0; i<functionsCollection_.getSize(); ++i)
    if (!functionsCollection_[i].isLinear())
      return false;
    
  return true;
}

Bool DualLinearCombinationEvaluation::isLinearlyDependent(const UnsignedInteger index) const
{
  for (UnsignedInteger i=0; i<functionsCollection_.getSize(); ++i)
    if (!functionsCollection_[i].isLinearlyDependent(index))
      return false;
    
  return true;
}

/* Method save() stores the object through the StorageManager */
void DualLinearCombinationEvaluation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "functionsCollection_", functionsCollection_ );
  adv.saveAttribute( "coefficients_", coefficients_ );
}


/* Method load() reloads the object from the StorageManager */
void DualLinearCombinationEvaluation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "functionsCollection_", functionsCollection_ );
  adv.loadAttribute( "coefficients_", coefficients_ );
}



END_NAMESPACE_OPENTURNS
