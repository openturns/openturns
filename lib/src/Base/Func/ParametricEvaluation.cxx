//                                               -*- C++ -*-
/**
 *  @brief ParametricEvaluation
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ParametricEvaluation.hxx"
#include "openturns/SymbolicEvaluation.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/OSS.hxx"
#include <cmath>
#include <locale>
#include "openturns/CenteredFiniteDifferenceGradient.hxx"
#include "openturns/NonCenteredFiniteDifferenceGradient.hxx"
#include "openturns/FiniteDifferenceStep.hxx"
#include "openturns/MarginalGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/PointWithDescription.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ParametricEvaluation)

static const Factory<ParametricEvaluation> Factory_ParametricEvaluation;


/* Default constructor */
ParametricEvaluation::ParametricEvaluation()
  : EvaluationImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
ParametricEvaluation::ParametricEvaluation(const Function & function,
    const Indices & set,
    const Point & referencePoint,
    const Bool parametersSet)
  : EvaluationImplementation()
  , function_(function)
{
  const UnsignedInteger inputDimension = function.getInputDimension();
  const UnsignedInteger setDimension = set.getSize();
  // Check if the given parameters positions are compatible with the input dimension of the function
  if (!(inputDimension >= setDimension)) throw InvalidArgumentException(HERE) << "Error: the size of the " << (parametersSet ? "parameters" : "input") << " positions=" << setDimension << " is greater than the input dimension=" << inputDimension << " of the function.";
  // Check if the given indices are valid
  if (!set.check(inputDimension)) throw InvalidArgumentException(HERE) << "Error: the given set of positions contain either duplicate positions or positions greater than the input dimension of the function.";
  // Deduce the input position from the input dimension of the function and the parameters positions
  // Warning! the parameters positions can be in any order
  Indices fullIndices(inputDimension);
  Indices otherSet(0);
  fullIndices.fill();
  // Flag by inputDimension all the positions already used by the parameters
  for (UnsignedInteger i = 0; i < setDimension; ++i) fullIndices[set[i]] = inputDimension;
  // Then gather all the available indices
  for (UnsignedInteger i = 0; i < inputDimension; ++i)
    if (fullIndices[i] != inputDimension) otherSet.add(i);
  if (parametersSet)
  {
    parametersPositions_ = set;
    inputPositions_ = otherSet;
  }
  else
  {
    parametersPositions_ = otherSet;
    inputPositions_ = set;
  }
  const UnsignedInteger parametersSize = parametersPositions_.getSize();
  // Check if the given reference point has a dimension compatible with the function
  if (referencePoint.getDimension() != parametersSize) throw InvalidArgumentException(HERE) << "Error: the given reference point dimension=" << referencePoint.getDimension() << " does not match the parameters size=" << parametersSize;
  // Set the relevant part of the reference point in the parameters
  parameter_ = referencePoint;
  parameterDescription_ = function.getInputDescription().select(parametersPositions_);
  updateReducedFunction();
}

/* Virtual constructor method */
ParametricEvaluation * ParametricEvaluation::clone() const
{
  return new ParametricEvaluation(*this);
}

/* Comparison operator */
Bool ParametricEvaluation::operator ==(const ParametricEvaluation & other) const
{
  if (this == &other) return true;
  return hasEqualBase(other) && (function_ == other.function_) && (parametersPositions_ == other.parametersPositions_) && (inputPositions_ == other.inputPositions_);
}

Bool ParametricEvaluation::equals(const EvaluationImplementation & other) const
{
  const ParametricEvaluation * p_other = dynamic_cast<const ParametricEvaluation *>(&other);
  return p_other && (*this == *p_other);
}

/* Substitute the frozen parameters into symbolic formulas once and
   recompile, so repeated evaluations skip the splicing, see #2902 */
void ParametricEvaluation::updateReducedFunction()
{
  useReduced_ = false;
  // Nothing is frozen: the underlying function is already compiled
  if (parametersPositions_.getSize() == 0) return;
  const SymbolicEvaluation * p_symbolic = dynamic_cast<const SymbolicEvaluation *>(function_.getEvaluation().getImplementation().get());
  if (!p_symbolic) return;
  const Description inputVariables(p_symbolic->getInputVariablesNames());
  const Description outputVariables(p_symbolic->getOutputVariablesNames());
  const Description formulas(p_symbolic->getFormulas());
  // Free variables keep the inputPositions_ order used by operator()
  Description freeVariables(0);
  for (UnsignedInteger j = 0; j < inputPositions_.getSize(); ++j)
    freeVariables.add(inputVariables[inputPositions_[j]]);
  // Nothing to compile when every input is frozen
  if (freeVariables.getSize() == 0) return;
  const UnsignedInteger parametersDimension = parametersPositions_.getSize();
  // A non-finite frozen value cannot be substituted into the formula: the
  // parser rejects nan and inf as undefined symbols. Keep the generic path,
  // which passes such values as ordinary input coordinates.
  for (UnsignedInteger i = 0; i < parametersDimension; ++i)
    if (!std::isfinite(parameter_[i])) return;
  Description reducedFormulas(formulas.getSize());
  for (UnsignedInteger f = 0; f < formulas.getSize(); ++f)
  {
    const String & formula = formulas[f];
    String reduced;
    String::size_type cursor = 0;
    while (cursor < formula.size())
    {
      if (std::isalnum(formula[cursor], std::locale::classic()) || (formula[cursor] == '_'))
      {
        String::size_type end = cursor;
        while ((end < formula.size()) && (std::isalnum(formula[end], std::locale::classic()) || (formula[end] == '_'))) ++end;
        const String token(formula.substr(cursor, end - cursor));
        Bool frozen = false;
        for (UnsignedInteger i = 0; i < parametersDimension; ++i)
          if (token == inputVariables[parametersPositions_[i]])
          {
            // Keep the full double precision and protect the value with
            // parentheses, as in a^2 where a = -2 must read (-2)^2
            OSS value;
            value.setPrecision(17);
            value << parameter_[i];
            reduced += "(" + String(value) + ")";
            frozen = true;
            break;
          }
        if (!frozen) reduced += token;
        cursor = end;
      }
      else
      {
        reduced += formula[cursor];
        ++cursor;
      }
    }
    reducedFormulas[f] = reduced;
  }
  // A single formula may define one or several outputs by their symbolic
  // names, as in `result := ...`: compile it with the output variables so
  // that the assignment resolves the declared output name. Plain expressions
  // are mapped positionally to the default output names, which the
  // output-name constructor would silently evaluate as zero.
  const Bool hasAssignments = (reducedFormulas.getSize() == 1)
                              && (reducedFormulas[0].find(":=") != String::npos);
  if (hasAssignments)
    reducedFunction_ = SymbolicFunction(freeVariables, outputVariables, reducedFormulas[0]);
  else
    reducedFunction_ = SymbolicFunction(freeVariables, reducedFormulas);
  reducedFunction_.setOutputDescription(function_.getOutputDescription());
  useReduced_ = true;
}

/* Evaluation operator */
Point ParametricEvaluation::operator() (const Point & point) const
{
  // Precompiled reduced function when the underlying is symbolic
  if (useReduced_)
  {
    Point value(reducedFunction_(point));
    callsNumber_.increment();
    return value;
  }
  const UnsignedInteger inputDimension = function_.getInputDimension();
  const UnsignedInteger pointDimension = point.getDimension();
  const UnsignedInteger parametersDimension = getParameterDimension();
  if (pointDimension + parametersDimension != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << inputDimension - parametersDimension << ", got dimension=" << pointDimension;
  Point x(inputDimension);
  for (UnsignedInteger i = 0; i < parametersDimension; ++i) x[parametersPositions_[i]] = parameter_[i];
  for (UnsignedInteger i = 0; i < pointDimension; ++i) x[inputPositions_[i]] = point[i];
  const Point value(function_(x));
  callsNumber_.increment();
  return value;
}

/* Operator () */
Sample ParametricEvaluation::operator() (const Sample & inSample) const
{
  // Precompiled reduced function when the underlying is symbolic
  if (useReduced_)
  {
    const Sample output(reducedFunction_(inSample));
    callsNumber_.fetchAndAdd(inSample.getSize());
    return output;
  }
  const UnsignedInteger size = inSample.getSize();
  const UnsignedInteger inputDimension = function_.getInputDimension();
  const UnsignedInteger sampleDimension = inSample.getDimension();
  const UnsignedInteger parametersDimension = getParameterDimension();
  if (sampleDimension + parametersDimension != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a sample of dimension=" << inputDimension - parametersDimension << ", got dimension=" << sampleDimension;
  Sample input(size, inputDimension);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    for (UnsignedInteger j = 0; j < parametersDimension; ++j) input(i, parametersPositions_[j]) = parameter_[j];
    for (UnsignedInteger j = 0; j < sampleDimension; ++j) input(i, inputPositions_[j]) = inSample(i, j);
  }
  const Sample output(function_(input));
  callsNumber_.fetchAndAdd(size);
  return output;
}

/* Gradient according to the marginal parameters */
Matrix ParametricEvaluation::parameterGradient(const Point & inP) const
{
  const UnsignedInteger parametersDimension = getParameterDimension();
  const UnsignedInteger inputDimension = function_.getInputDimension();
  const UnsignedInteger pointDimension = inP.getDimension();
  if (pointDimension + parametersDimension != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << inputDimension - parametersDimension << ", got dimension=" << pointDimension;
  // Unwrap MarginalGradient if present
  const GradientImplementation * p_gradient_impl = function_.getGradient().getImplementation().get();
  const MarginalGradient * p_marginal = dynamic_cast<const MarginalGradient *>(p_gradient_impl);
  if (p_marginal) p_gradient_impl = p_marginal->getGradient().getImplementation().get();
  // Special case if the gradient of the underlying function is based on finite differences
  {
    const CenteredFiniteDifferenceGradient * p_gradient = dynamic_cast<const CenteredFiniteDifferenceGradient *>(p_gradient_impl);
    if (p_gradient)
    {
      // Retrieve the full gradient parameters
      FiniteDifferenceStep step(p_gradient->getFiniteDifferenceStep());
      const Point fullEpsilon(step.getEpsilon());
      // Build the step restricted to the parameter set of the function
      step.setEpsilon(fullEpsilon.select(parametersPositions_));

      ParametricEvaluation evaluation(function_, inputPositions_, inP);
      evaluation.inputPositions_ = parametersPositions_; // keep parameters order
      evaluation.useReduced_ = false; // positions changed after the reduced function was built
      const Matrix reducedGradient(CenteredFiniteDifferenceGradient(step, evaluation).gradient(parameter_));
      return reducedGradient;
    }
  }
  // Second try: NonCenteredFiniteDifferenceGradient
  {
    const NonCenteredFiniteDifferenceGradient * p_gradient = dynamic_cast<const NonCenteredFiniteDifferenceGradient *>(p_gradient_impl);
    if (p_gradient)
    {
      // Retrieve the full gradient parameters
      FiniteDifferenceStep step(p_gradient->getFiniteDifferenceStep());
      const Point fullEpsilon(step.getEpsilon());
      // Build the step restricted to the parameter set of the function
      step.setEpsilon(fullEpsilon.select(parametersPositions_));

      ParametricEvaluation evaluation(function_, inputPositions_, inP);
      evaluation.inputPositions_ = parametersPositions_; // keep parameters order
      evaluation.useReduced_ = false; // positions changed after the reduced function was built
      const NonCenteredFiniteDifferenceGradient reducedGradient(step, evaluation);
      return reducedGradient.gradient(parameter_);
    }
  }
  Point x(inputDimension);
  for (UnsignedInteger i = 0; i < parametersDimension; ++i) x[parametersPositions_[i]] = parameter_[i];
  for (UnsignedInteger i = 0; i < pointDimension; ++i) x[inputPositions_[i]] = inP[i];
  const UnsignedInteger outputDimension = getOutputDimension();
  const Matrix fullGradient(function_.gradient(x));
  // The gradient wrt x corresponds to the parameterPositions rows of the full gradient
  Matrix result(parametersDimension, outputDimension);
  for (UnsignedInteger i = 0; i < parametersDimension; ++i)
  {
    const UnsignedInteger i0 = parametersPositions_[i];
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
      result(i, j) = fullGradient(i0, j);
  }
  return result;
}

/* Parameters accessor */
void ParametricEvaluation::setParameter(const Point & parameters)
{
  const UnsignedInteger parametersDimension = parameters.getDimension();
  if (parametersDimension != parametersPositions_.getSize())
    throw InvalidArgumentException(HERE) << "ParametricEvaluation expected a parameter of dimension " << parametersPositions_.getSize()
                                         << ", got dimension " << parametersDimension;
  parameter_ = parameters;
  // Do not recompile the substituted formulas here: setParameter() is called in
  // hot loops where the parameter changes at every call, e.g. by the
  // Metropolis-Hastings likelihood. The generic path already splits the
  // current parameter values into the input point of the compiled function.
  useReduced_ = false;
  reducedFunction_ = Function();
}

/* Parameters positions accessor */
Indices ParametricEvaluation::getParametersPositions() const
{
  return parametersPositions_;
}

/* Input positions accessor */
Indices ParametricEvaluation::getInputPositions() const
{
  return inputPositions_;
}

/* Function accessor */
Function ParametricEvaluation::getFunction() const
{
  return function_;
}

/* Dimension accessor */
UnsignedInteger ParametricEvaluation::getInputDimension() const
{
  return inputPositions_.getSize();
}

UnsignedInteger ParametricEvaluation::getParameterDimension() const
{
  return parametersPositions_.getSize();
}

UnsignedInteger ParametricEvaluation::getOutputDimension() const
{
  return function_.getOutputDimension();
}

/* Linearity accessors */
Bool ParametricEvaluation::isLinear() const
{
  return function_.isLinear();
}

Bool ParametricEvaluation::isLinearlyDependent(const UnsignedInteger index) const
{
  if (index >= getInputDimension())
    throw InvalidDimensionException(HERE) << "index (" << index << ") exceeds function input dimension (" << getInputDimension() << ")";
  return function_.isLinearlyDependent(inputPositions_[index]);
}

Bool ParametricEvaluation::isParallel() const
{
  return function_.getImplementation()->isParallel();
}

/* String converter */
String ParametricEvaluation::__repr__() const
{
  PointWithDescription parameters(parameter_);
  parameters.setDescription(parameterDescription_);

  OSS oss;
  oss << "class=" << ParametricEvaluation::GetClassName()
      << " function=" << function_
      << " parameters positions=" << parametersPositions_
      << " parameters=" << parameters
      << " input positions=" << inputPositions_;
  return oss;
}

String ParametricEvaluation::__str__(const String & ) const
{
  PointWithDescription parameters(parameter_);
  parameters.setDescription(parameterDescription_);

  OSS oss(false);
  oss << ParametricEvaluation::GetClassName()
      << "(" << function_
      << ", parameters positions=" << parametersPositions_
      << ", parameters=" << parameters
      << ", input positions=" << inputPositions_ << ")";
  return oss;
}

/* Input description accessor, i.e. the names of the input parameters */
void ParametricEvaluation::setInputDescription(const Description & inputDescription)
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inputDescription.getSize() != inputDimension)
    throw InvalidArgumentException(HERE) << "Input description size must match the input dimension (" << inputDimension << ")";
  Description fullInputDescription(function_.getInputDescription());
  for (UnsignedInteger i = 0; i < inputDimension; ++ i)
    fullInputDescription[inputPositions_[i]] = inputDescription[i];
  function_.setInputDescription(fullInputDescription);
  if (useReduced_) reducedFunction_.setInputDescription(inputDescription);
}

Description ParametricEvaluation::getInputDescription() const
{
  return function_.getInputDescription().select(inputPositions_);
}

/* Output description accessor, i.e. the names of the output parameters */
void ParametricEvaluation::setOutputDescription(const Description & outputDescription)
{
  function_.setOutputDescription(outputDescription);
  if (useReduced_) reducedFunction_.setOutputDescription(outputDescription);
}

Description ParametricEvaluation::getOutputDescription() const
{
  return function_.getOutputDescription();
}

void ParametricEvaluation::setStopCallback(StopCallback callBack, void * state)
{
  function_.setStopCallback(callBack, state);
}

void ParametricEvaluation::setDetachCallback(DetachCallback callBack, void * state)
{
  function_.setDetachCallback(callBack, state);
}

/* Method save() stores the object through the StorageManager */
void ParametricEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save( adv );
  adv.saveAttribute( "function_", function_ );
  adv.saveAttribute( "parametersPositions_", parametersPositions_ );
  adv.saveAttribute( "inputPositions_", inputPositions_ );
}

/* Method load() reloads the object from the StorageManager */
void ParametricEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load( adv );
  adv.loadAttribute( "function_", function_ );
  adv.loadAttribute( "parametersPositions_", parametersPositions_ );
  adv.loadAttribute( "inputPositions_", inputPositions_ );
  // parameter_ is restored by EvaluationImplementation::load()
  updateReducedFunction();
}


END_NAMESPACE_OPENTURNS
