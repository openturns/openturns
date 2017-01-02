//                                               -*- C++ -*-
/**
 * @brief Class for the inverse Box cox function
 *
 * Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#include "openturns/InverseBoxCoxEvaluationImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(InverseBoxCoxEvaluationImplementation);

static const Factory<InverseBoxCoxEvaluationImplementation> Factory_InverseBoxCoxEvaluationImplementation;

/* Default constructor */
InverseBoxCoxEvaluationImplementation::InverseBoxCoxEvaluationImplementation()
  : NumericalMathEvaluationImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
InverseBoxCoxEvaluationImplementation::InverseBoxCoxEvaluationImplementation(const NumericalPoint & lambda)
  : NumericalMathEvaluationImplementation()
  , lambda_(lambda)
  , shift_(lambda.getDimension())
{
  setInputDescription(Description::BuildDefault(lambda_.getSize(), "x"));
  setOutputDescription(Description::BuildDefault(lambda_.getSize(), "y"));
}

InverseBoxCoxEvaluationImplementation::InverseBoxCoxEvaluationImplementation(const NumericalPoint & lambda,
    const NumericalPoint & shift)
  : NumericalMathEvaluationImplementation()
  , lambda_(lambda)
  , shift_(shift)
{
  if (lambda.getDimension() != shift.getDimension()) throw InvalidArgumentException(HERE) << "Error: the given exponent vector has a dimension=" << lambda.getDimension() << " different from the shift dimension=" << shift.getDimension();
  setInputDescription(Description::BuildDefault(lambda_.getSize(), "x"));
  setOutputDescription(Description::BuildDefault(lambda_.getSize(), "y"));
}

/* Clone constructor */
InverseBoxCoxEvaluationImplementation * InverseBoxCoxEvaluationImplementation::clone() const
{
  return new InverseBoxCoxEvaluationImplementation(*this);
}

/* Comparison operator */
Bool InverseBoxCoxEvaluationImplementation::operator ==(const InverseBoxCoxEvaluationImplementation & other) const
{
  if (this == &other) return true;
  return (lambda_ == other.lambda_) && (shift_ == other.shift_);
}

/* String converter */
String InverseBoxCoxEvaluationImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << InverseBoxCoxEvaluationImplementation::GetClassName()
      << " name=" << getName()
      << " dimension=" << getInputDimension()
      << " lambda=" << lambda_
      << " shift=" << shift_;
  return oss;
}

/* String converter __str__ */
String InverseBoxCoxEvaluationImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "InverseBoxCox(lambda=" << lambda_
      << ", shift=" << shift_
      << ")";
  return oss;
}

/* Accessor for the lambda */
NumericalPoint InverseBoxCoxEvaluationImplementation::getLambda() const
{
  return lambda_;
}

/* Accessor for the shift */
NumericalPoint InverseBoxCoxEvaluationImplementation::getShift() const
{
  return shift_;
}

struct InverseBoxCoxEvaluationComputeSamplePolicy
{
  const NumericalSample & input_;
  NumericalSample & output_;
  const InverseBoxCoxEvaluationImplementation & evaluation_;

  InverseBoxCoxEvaluationComputeSamplePolicy(const NumericalSample & input,
      NumericalSample & output,
      const InverseBoxCoxEvaluationImplementation & evaluation)
    : input_(input)
    , output_(output)
    , evaluation_(evaluation)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      for (UnsignedInteger j = 0; j < evaluation_.getInputDimension(); ++j)
      {
        const NumericalScalar lambda_j = evaluation_.getLambda()[j];
        const NumericalScalar x = input_[i][j] - evaluation_.getShift()[j];
        if (std::abs(lambda_j * x * x) < 1e-8) output_[i][j] = exp(x) * (1.0 - 0.5 * lambda_j * x * x);
        else output_[i][j] = pow(lambda_j * x + 1.0, 1.0 / lambda_j);
      } // j
    } // i
  } // operator ()
}; /* end struct InverseBoxCoxEvaluationComputeSamplePolicy */

/* Operator () */
NumericalSample InverseBoxCoxEvaluationImplementation::operator() (const NumericalSample & inS) const
{
  if (inS.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << getInputDimension() << ", got " << inS.getDimension();
  const UnsignedInteger size = inS.getSize();
  NumericalSample result(size, getInputDimension());
  const InverseBoxCoxEvaluationComputeSamplePolicy policy( inS, result, *this );
  TBB::ParallelFor( 0, size, policy );
  callsNumber_ += size;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inS);
    outputStrategy_.store(result);
  }
  result.setDescription(getOutputDescription());
  return result;
}


/* Operator () */
NumericalPoint InverseBoxCoxEvaluationImplementation::operator() (const NumericalPoint & inP) const
{
  const UnsignedInteger dimension = getInputDimension();
  if (inP.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << dimension << ", got " << inP.getDimension();
  NumericalPoint result(dimension);

  // There is no check of positive variables
  // This last one must be done by user or, as the evaluation is used in a stochastic context, in the BoxCoxTransform class
  for (UnsignedInteger index = 0; index < dimension; ++index)
  {
    const NumericalScalar x = inP[index] - shift_[index];
    const NumericalScalar lambda_i = lambda_[index];
    if (std::abs(lambda_i * x * x) < 1e-8) result[index] = exp(x) * (1.0 - 0.5 * lambda_i * x * x);
    else
    {
      const NumericalScalar evaluation = lambda_i * x + 1.0;
      if (evaluation <= 0) throw InvalidArgumentException(HERE) << "Can not apply the inverse Box Cox function " ;
      result[index] = pow(evaluation, 1.0 / lambda_i);
    }
  }
  ++callsNumber_;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger InverseBoxCoxEvaluationImplementation::getInputDimension() const
{
  return lambda_.getDimension();
}

/* Accessor for output point dimension */
UnsignedInteger InverseBoxCoxEvaluationImplementation::getOutputDimension() const
{
  return lambda_.getDimension();
}

/* Method save() stores the object through the StorageManager */
void InverseBoxCoxEvaluationImplementation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute( "lambda_", lambda_ );
  adv.saveAttribute( "shift_", shift_ );
}

/* Method load() reloads the object from the StorageManager */
void InverseBoxCoxEvaluationImplementation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute( "lambda_", lambda_ );
  adv.loadAttribute( "shift_", shift_ );
}

END_NAMESPACE_OPENTURNS
