//                                               -*- C++ -*-
/**
 * @brief Abstract top-level class for all evaluation implementations
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include <algorithm>
#include <iterator>

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/ComposedEvaluation.hxx"
#include "openturns/OTconfig.hxx"
#ifdef OPENTURNS_HAVE_MUPARSER
#include "openturns/SymbolicEvaluation.hxx"
#endif
#include "openturns/LinearEvaluation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Full.hxx"
#include "openturns/Contour.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Box.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef EvaluationImplementation::CacheType EvaluationImplementationCache;

static const Factory<EvaluationImplementationCache> Factory_EvaluationImplementationCache;

/* These methods are implemented here for the needs of Cache */
/* We should be careful because they may interfere with other definitions placed elsewhere */
static const Factory<PersistentCollection<UnsignedInteger> > Factory_PersistentCollection_UnsignedInteger;
#ifndef OPENTURNS_UNSIGNEDLONG_SAME_AS_UINT64
static const Factory<PersistentCollection<Unsigned64BitsInteger> > Factory_PersistentCollection_Unsigned64BitsInteger;
#endif

TEMPLATE_CLASSNAMEINIT(PersistentCollection<PersistentCollection<Scalar> >)
static const Factory<PersistentCollection<PersistentCollection<Scalar> > > Factory_PersistentCollection_PersistentCollection_Scalar;


CLASSNAMEINIT(EvaluationImplementation)

static const Factory<EvaluationImplementation> Factory_EvaluationImplementation;


/* Default constructor */
EvaluationImplementation::EvaluationImplementation()
  : PersistentObject()
  , callsNumber_(0)
  , p_cache_(new CacheType)
  , inputStrategy_(Full())
  , outputStrategy_(Full())
  , isHistoryEnabled_(false)
  , parameter_(0)
  , isParallel_(ResourceMap::GetAsBool("Evaluation-Parallel"))
  , inputDescription_(0)
  , outputDescription_(0)
{
  // We disable the cache by default
  p_cache_->disable();
}

/* Virtual constructor */
EvaluationImplementation * EvaluationImplementation::clone() const
{
  return new EvaluationImplementation(*this);
}


/* Comparison operator */
Bool EvaluationImplementation::operator ==(const EvaluationImplementation & other) const
{
  return true;
}

/* String converter */
String EvaluationImplementation::__repr__() const
{
  PointWithDescription parameters(parameter_);
  parameters.setDescription(parameterDescription_);

  OSS oss(true);
  oss << "class=" << EvaluationImplementation::GetClassName()
      << " name=" << getName()
      << " input description=" << inputDescription_
      << " output description=" << outputDescription_
      << " parameters=" <<  parameters;
  return oss;
}

/* String converter */
String EvaluationImplementation::__str__(const String & offset) const
{
  return OSS(false) << offset << "EvaluationImplementation";
}

/* Description Accessor */
void EvaluationImplementation::setDescription(const Description & description)
{
  if (description.getSize() != getInputDimension() + getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the description must have a size of input dimension + output dimension, here size=" << description.getSize() << ", input dimension=" << getInputDimension() << ", output dimension=" << getOutputDimension();
  inputDescription_ = Description(getInputDimension());
  std::copy(description.begin(), description.begin() + getInputDimension(), inputDescription_.begin());
  outputDescription_ = Description(getOutputDimension());
  std::copy(description.begin() + getInputDimension(), description.begin() + description.getSize(), outputDescription_.begin());
}


/* Description Accessor */
Description EvaluationImplementation::getDescription() const
{
  Description description(getInputDescription());
  Description outputDescription(getOutputDescription());
  for (UnsignedInteger i = 0; i < getOutputDimension(); ++i) description.add(outputDescription[i]);
  return description;
}

/* Input description Accessor */
void EvaluationImplementation::setInputDescription(const Description & inputDescription)
{
  if (inputDescription.getSize() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the input description must have a size=" << inputDescription.getSize() << " equal to the input dimension=" << getInputDimension();
  inputDescription_ = inputDescription;
}

Description EvaluationImplementation::getInputDescription() const
{
  if (inputDescription_.getSize() == 0) return Description::BuildDefault(getInputDimension(), "x");
  return inputDescription_;
}

/* Output description Accessor */
void EvaluationImplementation::setOutputDescription(const Description & outputDescription)
{
  if (outputDescription.getSize() != getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the output description must have a size=" << outputDescription.getSize() << " equal to the output dimension=" << getOutputDimension();
  outputDescription_ = outputDescription;
}

Description EvaluationImplementation::getOutputDescription() const
{
  if (outputDescription_.getSize() == 0) return Description::BuildDefault(getOutputDimension(), "y");
  return outputDescription_;
}

/* Test for actual implementation */
Bool EvaluationImplementation::isActualImplementation() const
{
  return true;
}

/* Here is the interface that all derived class must implement */

namespace {
Sample evaluateSequential(const EvaluationImplementation & evaluation, const Sample & inSample)
{
  const UnsignedInteger size = inSample.getSize();
  Sample outSample(size, evaluation.getOutputDimension());
  // Simple loop over the evaluation operator based on point
  // The calls number is updated by these calls
  for (UnsignedInteger i = 0; i < size; ++i) outSample[i] = evaluation.operator()(inSample[i]);
  return outSample;
}

/* Parallel evaluation */
struct EvaluationPolicy
{
  const Sample & input_;
  Sample & output_;
  const EvaluationImplementation & evaluation_;

  EvaluationPolicy( const Sample & input,
                    Sample & output,
                    const EvaluationImplementation & evaluation)
    : input_(input)
    , output_(output)
    , evaluation_(evaluation)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i) output_[i] = evaluation_.operator()(input_[i]);
  }

}; /* end struct EvaluationPolicy */

Sample evaluateParallel(const EvaluationImplementation & evaluation, const Sample & inSample)
{
  const UnsignedInteger size = inSample.getSize();
  Sample result(size, evaluation.getOutputDimension());
  const EvaluationPolicy policy( inSample, result, evaluation );
  TBB::ParallelFor( 0, size, policy );
  return result;
}


}

/* Operator () */
Sample EvaluationImplementation::operator() (const Sample & inSample) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inSample.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inSample.getDimension();

  Sample outSample((isParallel_ && !isCacheEnabled() && !isHistoryEnabled()) ? evaluateParallel(*this, inSample) : evaluateSequential(*this, inSample));
  outSample.setDescription(getOutputDescription());
  return outSample;
}

/* Operator () */
Field EvaluationImplementation::operator() (const Field & inField) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inField.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given time series has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inField.getDimension();
  return Field(inField.getMesh(), operator()(inField.getValues()));
}


/* Enable or disable the internal cache */
void EvaluationImplementation::enableCache() const
{
  p_cache_->enable();
}

void EvaluationImplementation::disableCache() const
{
  p_cache_->disable();
}

Bool EvaluationImplementation::isCacheEnabled() const
{
  return p_cache_->isEnabled();
}

UnsignedInteger EvaluationImplementation::getCacheHits() const
{
  return p_cache_->getHits();
}

void EvaluationImplementation::addCacheContent(const Sample& inSample, const Sample& outSample)
{
  p_cache_->enable();
  const UnsignedInteger size = inSample.getSize();
  for ( UnsignedInteger i = 0; i < size; ++ i )
  {
    p_cache_->add( inSample[i], outSample[i] );
  }
}

Sample EvaluationImplementation::getCacheInput() const
{
  Bool cacheEnabled = isCacheEnabled();
  enableCache();
  PersistentCollection<CacheKeyType> keyColl( p_cache_->getKeys() );
  if ( ! cacheEnabled )
    disableCache();
  Sample inSample(0, getInputDimension());
  for ( UnsignedInteger i = 0; i < keyColl.getSize(); ++ i ) inSample.add( keyColl[i] );
  return inSample;
}

Sample EvaluationImplementation::getCacheOutput() const
{
  Bool cacheEnabled = isCacheEnabled();
  enableCache();
  PersistentCollection<CacheValueType> valuesColl( p_cache_->getValues() );
  if ( ! cacheEnabled )
    disableCache();
  Sample outSample(0, getOutputDimension());
  for ( UnsignedInteger i = 0; i < valuesColl.getSize(); ++ i )
  {
    outSample.add( valuesColl[i] );
  }
  return outSample;
}

void EvaluationImplementation::clearCache() const
{
  p_cache_->clear();
}

/* Enable or disable the input/output history */
void EvaluationImplementation::enableHistory() const
{
  isHistoryEnabled_ = true;
}

void EvaluationImplementation::disableHistory() const
{
  isHistoryEnabled_ = false;
}

Bool EvaluationImplementation::isHistoryEnabled() const
{
  return isHistoryEnabled_;
}

void EvaluationImplementation::clearHistory() const
{
  inputStrategy_ = Full();
  outputStrategy_ = Full();
}

HistoryStrategy EvaluationImplementation::getHistoryInput() const
{
  return inputStrategy_;
}

HistoryStrategy EvaluationImplementation::getHistoryOutput() const
{
  return outputStrategy_;
}

/* Input point / parameter history accessor */
Sample EvaluationImplementation::getInputPointHistory() const
{
  if (getParameterDimension() == 0) return inputStrategy_.getSample();
  throw NotYetImplementedException(HERE) << "in EvaluationImplementation::getInputPointHistory";
}

Sample EvaluationImplementation::getInputParameterHistory() const
{
  throw NotYetImplementedException(HERE) << "in EvaluationImplementation::getInputParameterHistory";
}


/* Gradient according to the marginal parameters */
Matrix EvaluationImplementation::parameterGradient(const Point & inP) const
{
  const Point parameter(getParameter());
  const UnsignedInteger parameterDimension = parameter.getDimension();
  const UnsignedInteger outputDimension = getOutputDimension();

  const Scalar epsilon = ResourceMap::GetAsScalar("NumericalMathEvaluation-ParameterEpsilon");

  // Const method, we need a copy in order to call setParameter()
  Pointer<EvaluationImplementation> p_evaluation(clone());
  const Point outCentered(operator()(inP));

  Matrix grad(parameterDimension, outputDimension);
  for (UnsignedInteger i = 0; i < parameterDimension; ++ i)
  {
    Point inPerturbed(parameter);
    inPerturbed[i] += epsilon;
    p_evaluation->setParameter(inPerturbed);
    const Point outPerturbed(p_evaluation->operator()(inP));
    // Since we use a copy, we have to update callsNumber_
    ++callsNumber_;
    if (isHistoryEnabled_) {
      inputStrategy_.store(inPerturbed);
      outputStrategy_.store(outPerturbed);
    }
    for (UnsignedInteger j = 0; j < outputDimension; ++ j)
    {
      grad(i, j) = (outPerturbed[j] - outCentered[j]) / epsilon;
    }
  }
  return grad;
}

/* Parameters value accessor */
Point EvaluationImplementation::getParameter() const
{
  return parameter_;
}

void EvaluationImplementation::setParameter(const Point & parameter)
{
  parameter_ = parameter;
}

void EvaluationImplementation::setParameterDescription(const Description & description)
{
  parameterDescription_ = description;
}

/* Parameters description accessor */
Description EvaluationImplementation::getParameterDescription() const
{
  return parameterDescription_;
}


/* Operator () */
Point EvaluationImplementation::operator() (const Point & inP) const
{
  throw NotYetImplementedException(HERE) << "In EvaluationImplementation::operator() (const Point & inP) const";
}

/* Accessor for input point dimension */
UnsignedInteger EvaluationImplementation::getInputDimension() const
{
  throw NotYetImplementedException(HERE) << "In EvaluationImplementation::getInputDimension() const";
}

/* Accessor for output point dimension */
UnsignedInteger EvaluationImplementation::getOutputDimension() const
{
  throw NotYetImplementedException(HERE) << "In EvaluationImplementation::getOutputDimension() const";
}

/* Accessor for input point dimension */
UnsignedInteger EvaluationImplementation::getParameterDimension() const
{
  return getParameter().getDimension();
}

/* Get the i-th marginal function */
EvaluationImplementation::Implementation EvaluationImplementation::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
EvaluationImplementation::Implementation EvaluationImplementation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal function must be in the range [0, outputDimension-1] and must be different";
  // We build an analytical function that extract the needed component
  // If X1,...,XN are the descriptions of the input of this function, it is a function from R^n to R^p
  // with formula Yk = Xindices[k] for k=1,...,p
  // Build non-ambigous names for the inputs. We cannot simply use the output description, as it must be valid muParser identifiers
  const UnsignedInteger inputDimension = getOutputDimension();
  const UnsignedInteger outputDimension = indices.getSize();
#ifdef OPENTURNS_HAVE_MUPARSER
  Description input(inputDimension);
  for (UnsignedInteger index = 0; index < inputDimension; ++index)
    input[index] = OSS() << "x" << index;
  // Extract the components
  Description output(outputDimension);
  Description formulas(outputDimension);
  Description currentOutputDescription(getOutputDescription());
  for (UnsignedInteger index = 0; index < outputDimension; ++index)
  {
    output[index] = currentOutputDescription[indices[index]];
    formulas[index] = input[indices[index]];
  }
  const SymbolicEvaluation left(input, output, formulas);
#else
  Point center(inputDimension);
  Matrix linear(inputDimension, outputDimension);
  for ( UnsignedInteger index = 0; index < outputDimension; ++ index )
    linear(indices[index], index) = 1.0;
  Point constant(outputDimension);
  const LinearEvaluation left(center, constant, linear);
#endif
  ComposedEvaluation marginal(left.clone(), clone());
  if (isHistoryEnabled())
  {
    marginal.enableHistory();
  }
  return marginal.clone();
}

/* Get the number of calls to operator() */
UnsignedInteger EvaluationImplementation::getCallsNumber() const
{
  return callsNumber_;
}

/** Parallelization flag accessor */
void EvaluationImplementation::setParallel(const Bool flag)
{
  isParallel_ = flag;
}

Bool EvaluationImplementation::isParallel() const
{
  return isParallel_;
}


/* Draw the given 1D marginal output as a function of the given 1D marginal input around the given central point */
Graph EvaluationImplementation::draw(const UnsignedInteger inputMarginal,
                                     const UnsignedInteger outputMarginal,
                                     const Point & centralPoint,
                                     const Scalar xMin,
                                     const Scalar xMax,
                                     const UnsignedInteger pointNumber,
                                     const GraphImplementation::LogScale scale) const
{
  if (getInputDimension() < 1) throw InvalidArgumentException(HERE) << "Error: cannot use this version of the draw() method with a function of input dimension less than 1";
  if (inputMarginal >= getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the given input marginal index=" << inputMarginal << " must be less than the input dimension=" << getInputDimension();
  if (outputMarginal >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the given output marginal index=" << outputMarginal << " must be less than the output dimension=" << getOutputDimension();
  if ((scale != GraphImplementation::NONE) && (scale != GraphImplementation::LOGX)) throw InvalidArgumentException(HERE) << "Error: expected scale=" << GraphImplementation::NONE << " or scale=" << GraphImplementation::LOGX << ", got scale=" << scale;
  if ((scale == GraphImplementation::LOGX) && ((xMin <= 0.0) || (xMax <= 0.0))) throw InvalidArgumentException(HERE) << "Error: cannot use logarithmic scale on an interval containing nonpositive values.";
  Sample inputData(pointNumber, centralPoint);
  if (scale == GraphImplementation::NONE)
  {
    const Scalar dx = (xMax - xMin) / (pointNumber - 1.0);
    for (UnsignedInteger i = 0; i < pointNumber; ++i)
      inputData[i][inputMarginal] = xMin + i * dx;
  }
  else
  {
    const Scalar a = std::log(xMin);
    const Scalar b = std::log(xMax);
    const Scalar dLogX = (b - a) / (pointNumber - 1.0);
    for (UnsignedInteger i = 0; i < pointNumber; ++i)
      inputData[i][inputMarginal] = std::exp(a + i * dLogX);
  }
  // Evaluate the function over all its input in one call in order to benefit from potential parallelism
  const Sample outputData((*this)(inputData));
  const Description inputDescription(getInputDescription());
  const Description outputDescription(getOutputDescription());
  const String xName(inputDescription[inputMarginal]);
  const String yName(outputDescription[outputMarginal]);
  String title(OSS() << yName << " as a function of " << xName);
  if (centralPoint.getDimension() > 1) title = String(OSS(false) << title << " around " << centralPoint);
  Graph graph(title, xName, yName, true, "", 1.0, scale);
  graph.add(Curve(inputData.getMarginal(inputMarginal), outputData.getMarginal(outputMarginal)));
  return graph;
}

/* Draw the given 1D marginal output as a function of the given 2D marginal input around the given central point */
Graph EvaluationImplementation::draw(const UnsignedInteger firstInputMarginal,
                                     const UnsignedInteger secondInputMarginal,
                                     const UnsignedInteger outputMarginal,
                                     const Point & centralPoint,
                                     const Point & xMin,
                                     const Point & xMax,
                                     const Indices & pointNumber,
                                     const GraphImplementation::LogScale scale) const
{
  if (getInputDimension() < 2) throw InvalidArgumentException(HERE) << "Error: cannot use this version of the draw() method with a function of input dimension less than 2";
  if ((xMin.getDimension() != 2) || (xMax.getDimension() != 2) || (pointNumber.getSize() != 2)) throw InvalidArgumentException(HERE) << "Error: xMin, xMax and PointNumber must be bidimensional";
  if ((pointNumber[0] <= 2) || (pointNumber[1] <= 2)) throw InvalidArgumentException(HERE) << "Error: the discretization must have at least 2 points per component";
  if ((scale != GraphImplementation::NONE) && (scale != GraphImplementation::LOGX) && (scale != GraphImplementation::LOGY) && (scale != GraphImplementation::LOGXY)) throw InvalidArgumentException(HERE) << "Error: expected scale=" << GraphImplementation::NONE << " or scale=" << GraphImplementation::LOGX << " or scale=" << GraphImplementation::LOGY << " or scale=" << GraphImplementation::LOGXY << ", got scale=" << scale;
  if (((scale == GraphImplementation::LOGX) || (scale == GraphImplementation::LOGXY)) && ((xMin[0] <= 0.0) || (xMax[0] <= 0.0))) throw InvalidArgumentException(HERE) << "Error: cannot use logarithmic scale on an interval containing nonpositive values for the first argument.";
  if (((scale == GraphImplementation::LOGY) || (scale == GraphImplementation::LOGXY)) && ((xMin[1] <= 0.0) || (xMax[1] <= 0.0))) throw InvalidArgumentException(HERE) << "Error: cannot use logarithmic scale on an interval containing nonpositive values for the second argument.";
  Point discretization(2);
  Point scaling(2);
  Point origin(2);
  const Scalar nX = pointNumber[0] - 2;
  discretization[0] = nX;
  // Discretization of the first component
  Sample x(Box(Point(1, nX)).generate());
  {
    Scalar a = xMin[0];
    Scalar b = xMax[0];
    if ((scale == GraphImplementation::LOGX) || (scale == GraphImplementation::LOGXY))
    {
      a = std::log(a);
      b = std::log(b);
    }
    origin[0] = a;
    scaling[0] = b - a;
  }
  x *= Point(1, scaling[0]);
  x += Point(1, origin[0]);
  // Recover the original scale if the discretization has been done in the logarithmic scale
  if ((scale == GraphImplementation::LOGY) || (scale == GraphImplementation::LOGXY))
    for (UnsignedInteger i = 0; i < x.getDimension(); ++i) x[i][0] = std::exp(x[i][0]);
  const Scalar nY = pointNumber[1] - 2;
  discretization[1] = nY;
  // Discretization of the second component
  Sample y(Box(Point(1, nY)).generate());
  {
    Scalar a = xMin[1];
    Scalar b = xMax[1];
    if ((scale == GraphImplementation::LOGY) || (scale == GraphImplementation::LOGXY))
    {
      a = std::log(a);
      b = std::log(b);
    }
    origin[1] = a;
    scaling[1] = b - a;
  }
  y *= Point(1, scaling[1]);
  y += Point(1, origin[1]);
  // Recover the original scale if the discretization has been done in the logarithmic scale
  if ((scale == GraphImplementation::LOGY) || (scale == GraphImplementation::LOGXY))
    for (UnsignedInteger i = 0; i < y.getDimension(); ++i) y[i][0] = std::exp(y[i][0]);
  // Discretization of the XY plane
  Sample inputSample((nX + 2) * (nY + 2), centralPoint);
  // Prepare the input sample
  UnsignedInteger index = 0;
  for (UnsignedInteger j = 0; j < nY + 2; ++j)
  {
    const Scalar yJ = (scale == GraphImplementation::LOGY) || (scale == GraphImplementation::LOGXY) ? exp(y[j][0]) : y[j][0];
    for (UnsignedInteger i = 0; i < nX + 2; ++i)
    {
      const Scalar xI = (scale == GraphImplementation::LOGX) || (scale == GraphImplementation::LOGXY) ? exp(x[i][0]) : x[i][0];
      inputSample[index][firstInputMarginal]  = xI;
      inputSample[index][secondInputMarginal]  = yJ;
      ++index;
    } // i
  } // j
  // Compute the output sample, using possible parallelism
  const Sample z((*this)(inputSample).getMarginal(outputMarginal));
  const String xName(getInputDescription()[firstInputMarginal]);
  const String yName(getInputDescription()[secondInputMarginal]);
  String title(OSS() << getOutputDescription()[outputMarginal] << " as a function of (" << xName << "," << yName << ")");
  if (centralPoint.getDimension() > 2) title = String(OSS(false) << title << " around " << centralPoint);
  Graph graph(title, xName, yName, true, "topright", 1.0, scale);
  Contour isoValues(Contour(x, y, z, Point(0), Description(0), true, title));
  isoValues.buildDefaultLevels();
  isoValues.buildDefaultLabels();
  const Point levels(isoValues.getLevels());
  const Description labels(isoValues.getLabels());
  for (UnsignedInteger i = 0; i < levels.getDimension(); ++i)
  {
    Contour current(isoValues);
    current.setLevels(Point(1, levels[i]));
    current.setLabels(Description(1, labels[i]));
    current.setDrawLabels(false);
    current.setLegend(labels[i]);
    current.setColor(Contour::ConvertFromHSV((360.0 * i / levels.getDimension()), 1.0, 1.0));
    graph.add(current);
  }
  return graph;
}

/* Draw the output of the function with respect to its input when the input and output dimensions are 1 */
Graph EvaluationImplementation::draw(const Scalar xMin,
                                     const Scalar xMax,
                                     const UnsignedInteger pointNumber,
                                     const GraphImplementation::LogScale scale) const
{
  if (getInputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot draw a function with input dimension=" << getInputDimension() << " different from 1 using this method. See the other draw() methods.";
  if (getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot draw a function with output dimension=" << getOutputDimension() << " different from 1 using this method. See the other draw() methods.";
  return draw(0, 0, Point(1), xMin, xMax, pointNumber, scale);
}

/* Draw the output of the function with respect to its input when the input dimension is 2 and the output dimension is 1 */
Graph EvaluationImplementation::draw(const Point & xMin,
                                     const Point & xMax,
                                     const Indices & pointNumber,
                                     const GraphImplementation::LogScale scale) const
{
  if (getInputDimension() == 1) return draw(xMin[0], xMax[0], pointNumber[0], scale);
  if ((getInputDimension() == 0) || (getInputDimension() > 2)) throw InvalidArgumentException(HERE) << "Error: cannot draw a function with input dimension=" << getInputDimension() << " different from 1 or 2 using this method. See the other draw() methods.";
  if (getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot draw a function with output dimension=" << getOutputDimension() << " different from 1 using this method. See the other draw() methods.";
  return draw(0, 1, 0, Point(2), xMin, xMax, pointNumber, scale);
}


/* Method save() stores the object through the StorageManager */
void EvaluationImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "callsNumber_", callsNumber_ );
  adv.saveAttribute( "cache_", *p_cache_ );
  adv.saveAttribute( "isParallel_", isParallel_ );
  adv.saveAttribute( "inputDescription_", inputDescription_ );
  adv.saveAttribute( "outputDescription_", outputDescription_ );
  adv.saveAttribute( "parameter_", parameter_ );
  adv.saveAttribute( "parameterDescription_", parameterDescription_ );
}

/* Method load() reloads the object from the StorageManager */
void EvaluationImplementation::load(Advocate & adv)
{
  TypedInterfaceObject<CacheType> cache;
  PersistentObject::load(adv);
  adv.loadAttribute( "callsNumber_", callsNumber_ );
  adv.loadAttribute( "cache_", cache );
  p_cache_ = cache.getImplementation();
  adv.loadAttribute( "isParallel_", isParallel_ );
  adv.loadAttribute( "inputDescription_", inputDescription_ );
  adv.loadAttribute( "outputDescription_", outputDescription_ );
  adv.loadAttribute( "parameter_", parameter_ );
  adv.loadAttribute( "parameterDescription_", parameterDescription_ );
}

END_NAMESPACE_OPENTURNS
