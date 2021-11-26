//                                               -*- C++ -*-
/**
 *  @brief The evaluation part of linear combination of polynomials
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/LinearCombinationEvaluation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/IdentityFunction.hxx"
#include "openturns/TBBImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

//TEMPLATE_CLASSNAMEINIT(PersistentCollection<Function>)

static const Factory<PersistentCollection<Function> > Factory_PersistentCollection_Function;


CLASSNAMEINIT(LinearCombinationEvaluation)

static const Factory<LinearCombinationEvaluation> Factory_LinearCombinationEvaluation;


/* Default constructor */
LinearCombinationEvaluation::LinearCombinationEvaluation()
  : EvaluationImplementation()
  , functionsCollection_(1, IdentityFunction(1))
  , coefficients_(1, 1.0)
  , isZero_(false)
{
  // Nothing to do
}


/* Parameters constructor */
LinearCombinationEvaluation::LinearCombinationEvaluation(const FunctionCollection & functionsCollection,
    const Point & coefficients)
  : EvaluationImplementation()
  , functionsCollection_(0)
  , coefficients_(0)
  , isZero_(false)
{
  setFunctionsCollectionAndCoefficients(functionsCollection, coefficients);
}


/* Virtual constructor */
LinearCombinationEvaluation * LinearCombinationEvaluation::clone() const
{
  return new LinearCombinationEvaluation(*this);
}


/* String converter */
String LinearCombinationEvaluation::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " functions=" << functionsCollection_
         << " coefficients=" << coefficients_;
}

String LinearCombinationEvaluation::__str__(const String & ) const
{
  OSS oss(false);
  const UnsignedInteger size = functionsCollection_.getSize();
  if (size > 1) oss << "(";
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Bool isNegative = coefficients_[i] < 0.0;
    const Scalar absCoefficient = std::abs(coefficients_[i]);
    if (i > 0) oss << (isNegative ? " - " : " + ");
    else if (isNegative) oss << "-";
    if (absCoefficient != 1.0) oss << absCoefficient << " * ";
    oss << "(" << functionsCollection_[i].getEvaluation().getImplementation()->__str__() << ")";
  }
  if (size > 1) oss << ")";
  if (size == 0) oss << "0.0";
  return oss;
}

// Helper for the parallel version of the point-based evaluation operator
struct LinearCombinationEvaluationPointFunctor
{
  const Point & input_;
  const LinearCombinationEvaluation & evaluation_;
  Point accumulator_;

  LinearCombinationEvaluationPointFunctor(const Point & input,
                                          const LinearCombinationEvaluation & evaluation)
    : input_(input)
    , evaluation_(evaluation)
    , accumulator_(Point(evaluation.getOutputDimension()))
  {}

  LinearCombinationEvaluationPointFunctor(const LinearCombinationEvaluationPointFunctor & other,
                                          TBBImplementation::Split)
    : input_(other.input_)
    , evaluation_(other.evaluation_)
    , accumulator_(Point(other.accumulator_.getDimension()))
  {}

  inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r )
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i) accumulator_ += evaluation_.coefficients_[i] * evaluation_.functionsCollection_[i](input_);
  } // operator()

  inline void join(const LinearCombinationEvaluationPointFunctor & other)
  {
    accumulator_ += other.accumulator_;
  }

}; // struct LinearCombinationEvaluationPointFunctor

/* Evaluation operator */
Point LinearCombinationEvaluation::operator () (const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  if (isZero_) return Point(getOutputDimension());
  const UnsignedInteger size = functionsCollection_.getSize();
  LinearCombinationEvaluationPointFunctor functor( inP, *this );
  TBBImplementation::ParallelReduce( 0, size, functor );
  const Point result(functor.accumulator_);
  callsNumber_.increment();
  return result;
}

Sample LinearCombinationEvaluation::operator () (const Sample & inS) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inS.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inS.getDimension();
  const UnsignedInteger sampleSize = inS.getSize();
  Sample result(sampleSize, getOutputDimension());
  result.setDescription(getOutputDescription());
  if (sampleSize == 0) return result;

  if (!isZero_)
  {
    const UnsignedInteger size = functionsCollection_.getSize();
    for (UnsignedInteger i = 0; i < size; ++i)
      // Exploit possible parallelism in the basis functions
      result += functionsCollection_[i](inS) * coefficients_[i];
    callsNumber_.fetchAndAdd(sampleSize);
  }
  return result;
}

/* Coefficients accessor */
Point LinearCombinationEvaluation::getCoefficients() const
{
  return coefficients_;
}

/* Functions accessor */
LinearCombinationEvaluation::FunctionCollection LinearCombinationEvaluation::getFunctionsCollection() const
{
  return functionsCollection_;
}

void LinearCombinationEvaluation::setFunctionsCollectionAndCoefficients(const FunctionCollection & functionsCollection,
    const Point & coefficients)
{
  const UnsignedInteger size = functionsCollection.getSize();
  // Check for empty functions collection
  if (!(size > 0)) throw InvalidArgumentException(HERE) << "Error: cannot build a linear combination from an empty collection of functions.";
  // Check for incompatible number of functions and coefficients
  if (size != coefficients.getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot build a linear combination with a different number of functions and coefficients.";
  // Check for coherent input and output dimensions of the functions
  UnsignedInteger inputDimension = functionsCollection[0].getInputDimension();
  UnsignedInteger outputDimension = functionsCollection[0].getOutputDimension();
  for (UnsignedInteger i = 1; i < size; ++i)
  {
    if (functionsCollection[i].getInputDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given functions have incompatible input dimension.";
    if (functionsCollection[i].getOutputDimension() != outputDimension) throw InvalidArgumentException(HERE) << "Error: the given functions have incompatible output dimension.";
  }
  // Keep only the non zero coefficients
  isZero_ = false;
  coefficients_ = Point(0);
  functionsCollection_ = FunctionCollection(0);
  const Scalar epsilon = ResourceMap::GetAsScalar("LinearCombinationEvaluation-SmallCoefficient");
  for (UnsignedInteger i = 0; i < size; ++i)
    if (std::abs(coefficients[i]) > epsilon)
    {
      coefficients_.add(coefficients[i]);
      functionsCollection_.add(functionsCollection[i]);
    }
    else LOGWARN(OSS() << "removed the contributor " << i << "=" << functionsCollection[i] << " from the linear combination as its coefficient=" << coefficients[i] << " is too small.");
  if (functionsCollection_.getSize() == 0)
  {
    LOGINFO("Error: no significant contributors in the linear combination.");
    // Must keep at least one function and one coefficient for dimension accessors and consistency
    coefficients_.add(coefficients[0]);
    functionsCollection_.add(functionsCollection[0]);
    isZero_ = true;
  }
  setDescription(functionsCollection[0].getDescription());
}


/* Input dimension accessor */
UnsignedInteger LinearCombinationEvaluation::getInputDimension() const
{
  return functionsCollection_[0].getInputDimension();
}

/* Output dimension accessor */
UnsignedInteger LinearCombinationEvaluation::getOutputDimension() const
{
  return functionsCollection_[0].getOutputDimension();
}


/* Gradient according to the marginal parameters */
Matrix LinearCombinationEvaluation::parameterGradient(const Point & inP) const
{
  Matrix result(getParameter().getDimension(), getOutputDimension());
  const UnsignedInteger size = functionsCollection_.getSize();
  // Get the parameters gradients for each atom and stack them into the result
  UnsignedInteger rowIndex = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // Extract the atom gradient
    const Matrix atomParametersGradient(functionsCollection_[i].parameterGradient(inP));
    const UnsignedInteger rowDimension = atomParametersGradient.getNbRows();
    const UnsignedInteger columnDimension = atomParametersGradient.getNbColumns();
    // Scale the atom gradient and copy it into the result
    const Scalar coefficient = coefficients_[i];
    for (UnsignedInteger j = 0; j < rowDimension; ++j)
    {
      for (UnsignedInteger k = 0; k < columnDimension; ++k)
      {
        result(rowIndex, k) = coefficient * atomParametersGradient(j, k);
      }
      ++rowIndex;
    }
  }
  return result;
}


/* Parameters value accessor */
Point LinearCombinationEvaluation::getParameter() const
{
  Point parameter(0);
  const UnsignedInteger size = functionsCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    parameter.add(functionsCollection_[i].getParameter());
  }
  return parameter;
}


void LinearCombinationEvaluation::setParameter(const Point & parameter)
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
Description LinearCombinationEvaluation::getParameterDescription() const
{
  Description description;
  const UnsignedInteger size = functionsCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    description.add(functionsCollection_[i].getParameterDescription());
  }
  return description;
}


/* Get the i-th marginal function */
Evaluation LinearCombinationEvaluation::getMarginal(const UnsignedInteger i) const
{
  if (!(i < getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1], here index=" << i << " and outputDimension=" << getOutputDimension();
  const UnsignedInteger size = functionsCollection_.getSize();
  FunctionCollection marginalFunctions(size);
  for (UnsignedInteger j = 0; j < size; ++j) marginalFunctions[j] = functionsCollection_[j].getMarginal(i);
  return new LinearCombinationEvaluation(marginalFunctions, coefficients_);
}

/* Get the function corresponding to indices components */
Evaluation LinearCombinationEvaluation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "The indices of a marginal function must be in the range [0, dim-1] and must be different";
  const UnsignedInteger size = functionsCollection_.getSize();
  FunctionCollection marginalFunctions(size);
  for (UnsignedInteger i = 0; i < size; ++i) marginalFunctions[i] = functionsCollection_[i].getMarginal(indices);
  return new LinearCombinationEvaluation(marginalFunctions, coefficients_);
}

/* Linearity accessors */
Bool LinearCombinationEvaluation::isLinear() const
{
  for (UnsignedInteger i = 0; i < functionsCollection_.getSize(); ++i)
    if (!functionsCollection_[i].isLinear())
      return false;

  return true;
}

Bool LinearCombinationEvaluation::isLinearlyDependent(const UnsignedInteger index) const
{
  if (!(index <= getOutputDimension()))
    throw InvalidDimensionException(HERE) << "index (" << index << ") exceeds function output dimension (" << getOutputDimension() << ")";

  for (UnsignedInteger i = 0; i < functionsCollection_.getSize(); ++i)
    if (!functionsCollection_[i].isLinearlyDependent(index))
      return false;

  return true;
}

/* Is it safe to call in parallel? */
Bool LinearCombinationEvaluation::isParallel() const
{
  for (UnsignedInteger i = 0; i < functionsCollection_.getSize(); ++i)
    if (!functionsCollection_[i].getImplementation()->isParallel())
      return false;

  return true;
}


/* Method save() stores the object through the StorageManager */
void LinearCombinationEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "functionsCollection_", functionsCollection_ );
  adv.saveAttribute( "coefficients_", coefficients_ );
}


/* Method load() reloads the object from the StorageManager */
void LinearCombinationEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  FunctionPersistentCollection functionsCollection;
  Point coefficients;
  adv.loadAttribute( "functionsCollection_", functionsCollection );
  adv.loadAttribute( "coefficients_", coefficients );
  // For additional checks
  setFunctionsCollectionAndCoefficients(functionsCollection, coefficients);
}



END_NAMESPACE_OPENTURNS
