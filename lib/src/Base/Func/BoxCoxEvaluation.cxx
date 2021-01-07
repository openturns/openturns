//                                               -*- C++ -*-
/**
 *  @brief Class for a Box cox implementation
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

#include "openturns/BoxCoxEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/TBB.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BoxCoxEvaluation)

static const Factory<BoxCoxEvaluation> Factory_BoxCoxEvaluation;

/* Default constructor */
BoxCoxEvaluation::BoxCoxEvaluation()
  : EvaluationImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
BoxCoxEvaluation::BoxCoxEvaluation(const Point & lambda)
  : EvaluationImplementation()
  , lambda_(lambda)
  , shift_(lambda.getDimension())
{
  setInputDescription(Description::BuildDefault(lambda_.getSize(), "x"));
  setOutputDescription(Description::BuildDefault(lambda_.getSize(), "y"));
}

BoxCoxEvaluation::BoxCoxEvaluation(const Point & lambda,
                                   const Point & shift)
  : EvaluationImplementation()
  , lambda_(lambda)
  , shift_(shift)
{
  if (lambda.getDimension() != shift.getDimension()) throw InvalidArgumentException(HERE) << "Error: the given exponent vector has a dimension=" << lambda.getDimension() << " different from the shift dimension=" << shift.getDimension();
  setInputDescription(Description::BuildDefault(lambda_.getSize(), "x"));
  setOutputDescription(Description::BuildDefault(lambda_.getSize(), "y"));
}

/* Clone constructor */
BoxCoxEvaluation * BoxCoxEvaluation::clone() const
{
  return new BoxCoxEvaluation(*this);
}

/* Comparison operator */
Bool BoxCoxEvaluation::operator ==(const BoxCoxEvaluation & other) const
{
  if (this == &other) return true;
  return (lambda_ == other.lambda_) && (shift_ == other.shift_);
}

/* String converter */
String BoxCoxEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << BoxCoxEvaluation::GetClassName()
      << " name=" << getName()
      << " dimension=" << getInputDimension()
      << " lambda=" << lambda_.__repr__()
      << " shift=" << shift_.__repr__();
  return oss;
}

/* String converter __str__ */
String BoxCoxEvaluation::__str__(const String & ) const
{
  OSS oss (false);
  oss << "BoxCox(lambda=" << lambda_
      << ", shift=" << shift_
      << ")";
  return oss;
}

/* Accessor for the lambda */
Point BoxCoxEvaluation::getLambda() const
{
  return lambda_;
}

/* Accessor for the shift */
Point BoxCoxEvaluation::getShift() const
{
  return shift_;
}

struct BoxCoxEvaluationComputeSamplePolicy
{
  const Sample & input_;
  Sample & output_;
  const BoxCoxEvaluation & evaluation_;

  BoxCoxEvaluationComputeSamplePolicy(const Sample & input,
                                      Sample & output,
                                      const BoxCoxEvaluation & evaluation)
    : input_(input)
    , output_(output)
    , evaluation_(evaluation)
  {
    // Nothing to do
  }

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      for (UnsignedInteger j = 0; j < evaluation_.getInputDimension(); ++j)
      {
        const Scalar lambda_j = evaluation_.getLambda()[j];
        const Scalar logX = log(input_(i, j) + evaluation_.getShift()[j]);
        if (std::abs(lambda_j * logX) < 1e-8) output_(i, j) = logX * (1.0 + 0.5 * lambda_j * logX);
        else output_(i, j) = expm1(lambda_j * logX) / lambda_j;
      } // j
    } // i
  } // operator ()
};  // struct BoxCoxEvaluationComputeSamplePolicy

/* Operator () */
Sample BoxCoxEvaluation::operator() (const Sample & inS) const
{
  const UnsignedInteger inDimension = inS.getDimension();
  if (inDimension != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << getInputDimension() << ", got " << inS.getDimension();
  const UnsignedInteger size = inS.getSize();
  Sample result(size, inDimension);
  const BoxCoxEvaluationComputeSamplePolicy policy( inS, result, *this );
  TBB::ParallelFor( 0, size, policy );
  result.setDescription(getOutputDescription());
  callsNumber_.fetchAndAdd(size);
  result.setDescription(getOutputDescription());
  return result;
}

/* Operator () */
Point BoxCoxEvaluation::operator() (const Point & inP) const
{
  const UnsignedInteger dimension = getInputDimension();
  if (inP.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << dimension << ", got " << inP.getDimension();
  Point result(lambda_.getDimension());

  // There is no check of positive variables
  // This last one must be done by user or, as the evaluation is used in a stochastic context, in the BoxCoxTransform class
  for (UnsignedInteger index = 0; index < dimension; ++index)
  {
    const Scalar x = inP[index] + shift_[index];
    if (x <= 0.0)
      throw InvalidArgumentException(HERE) << "Can not apply the Box Cox evaluation function to a negative shifted value x=" << x;

    // Applying the Box-Cox function
    const Scalar lambda_i = lambda_[index];
    const Scalar logX = log(x);
    if (std::abs(lambda_i * logX) < 1e-8) result[index] = logX * (1.0 + 0.5 * lambda_i * logX);
    else result[index] = expm1(lambda_i * logX) / lambda_i;
  }
  callsNumber_.increment();
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger BoxCoxEvaluation::getInputDimension() const
{
  return lambda_.getDimension();
}

/* Accessor for output point dimension */
UnsignedInteger BoxCoxEvaluation::getOutputDimension() const
{
  return lambda_.getDimension();
}

/* Method save() stores the object through the StorageManager */
void BoxCoxEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "lambda_", lambda_ );
  adv.saveAttribute( "shift_", shift_ );
}

/* Method load() reloads the object from the StorageManager */
void BoxCoxEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "lambda_", lambda_ );
  adv.loadAttribute( "shift_", shift_ );
}

END_NAMESPACE_OPENTURNS
