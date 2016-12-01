//                                               -*- C++ -*-
/**
 * @brief Abstract top-level class for all evaluation implementations
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
#include <algorithm>
#include <iterator>

#include "openturns/NumericalMathEvaluationImplementation.hxx"
#include "openturns/ComposedNumericalMathEvaluationImplementation.hxx"
#include "openturns/OTconfig.hxx"
#ifdef OPENTURNS_HAVE_MUPARSER
#include "openturns/AnalyticalNumericalMathEvaluationImplementation.hxx"
#endif
#include "openturns/LinearNumericalMathEvaluationImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Full.hxx"
#include "openturns/Contour.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Box.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef NumericalMathEvaluationImplementation::CacheType NumericalMathEvaluationImplementationCache;

static const Factory<NumericalMathEvaluationImplementationCache> Factory_NumericalMathEvaluationImplementationCache;

/* These methods are implemented here for the needs of Cache */
/* We should be careful because they may interfere with other definitions placed elsewhere */
static const Factory<PersistentCollection<UnsignedInteger> > Factory_PersistentCollection_UnsignedInteger;
#ifndef OPENTURNS_UNSIGNEDLONG_SAME_AS_UINT64
static const Factory<PersistentCollection<Unsigned64BitsInteger> > Factory_PersistentCollection_Unsigned64BitsInteger;
#endif

TEMPLATE_CLASSNAMEINIT(PersistentCollection<PersistentCollection<NumericalScalar> >);
static const Factory<PersistentCollection<PersistentCollection<NumericalScalar> > > Factory_PersistentCollection_PersistentCollection_NumericalScalar;


CLASSNAMEINIT(NumericalMathEvaluationImplementation);

static const Factory<NumericalMathEvaluationImplementation> Factory_NumericalMathEvaluationImplementation;


/* Default constructor */
NumericalMathEvaluationImplementation::NumericalMathEvaluationImplementation()
  : PersistentObject()
  , callsNumber_(0)
  , p_cache_(new CacheType)
  , inputStrategy_(Full())
  , outputStrategy_(Full())
  , isHistoryEnabled_(false)
  , parameter_(0)
  , inputDescription_(0)
  , outputDescription_(0)
{
  // We disable the cache by default
  p_cache_->disable();
}

/* Virtual constructor */
NumericalMathEvaluationImplementation * NumericalMathEvaluationImplementation::clone() const
{
  return new NumericalMathEvaluationImplementation(*this);
}


/* Comparison operator */
Bool NumericalMathEvaluationImplementation::operator ==(const NumericalMathEvaluationImplementation & other) const
{
  return true;
}

/* String converter */
String NumericalMathEvaluationImplementation::__repr__() const
{
  NumericalPointWithDescription parameters(parameter_);
  parameters.setDescription(parameterDescription_);

  OSS oss(true);
  oss << "class=" << NumericalMathEvaluationImplementation::GetClassName()
      << " name=" << getName()
      << " input description=" << inputDescription_
      << " output description=" << outputDescription_
      << " parameters=" <<  parameters;
  return oss;
}

/* String converter */
String NumericalMathEvaluationImplementation::__str__(const String & offset) const
{
  return OSS(false) << offset << "NumericalMathEvaluationImplementation";
}

/* Description Accessor */
void NumericalMathEvaluationImplementation::setDescription(const Description & description)
{
  if (description.getSize() != getInputDimension() + getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the description must have a size of input dimension + output dimension, here size=" << description.getSize() << ", input dimension=" << getInputDimension() << ", output dimension=" << getOutputDimension();
  inputDescription_ = Description(getInputDimension());
  std::copy(description.begin(), description.begin() + getInputDimension(), inputDescription_.begin());
  outputDescription_ = Description(getOutputDimension());
  std::copy(description.begin() + getInputDimension(), description.begin() + description.getSize(), outputDescription_.begin());
}


/* Description Accessor */
Description NumericalMathEvaluationImplementation::getDescription() const
{
  Description description(getInputDescription());
  Description outputDescription(getOutputDescription());
  for (UnsignedInteger i = 0; i < getOutputDimension(); ++i) description.add(outputDescription[i]);
  return description;
}

/* Input description Accessor */
void NumericalMathEvaluationImplementation::setInputDescription(const Description & inputDescription)
{
  if (inputDescription.getSize() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the input description must have a size=" << inputDescription.getSize() << " equal to the input dimension=" << getInputDimension();
  inputDescription_ = inputDescription;
}

Description NumericalMathEvaluationImplementation::getInputDescription() const
{
  if (inputDescription_.getSize() == 0) return Description::BuildDefault(getInputDimension(), "x");
  return inputDescription_;
}

/* Output description Accessor */
void NumericalMathEvaluationImplementation::setOutputDescription(const Description & outputDescription)
{
  if (outputDescription.getSize() != getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the output description must have a size=" << outputDescription.getSize() << " equal to the output dimension=" << getOutputDimension();
  outputDescription_ = outputDescription;
}

Description NumericalMathEvaluationImplementation::getOutputDescription() const
{
  if (outputDescription_.getSize() == 0) return Description::BuildDefault(getOutputDimension(), "y");
  return outputDescription_;
}

/* Test for actual implementation */
Bool NumericalMathEvaluationImplementation::isActualImplementation() const
{
  return true;
}

/* Here is the interface that all derived class must implement */

/* Operator () */
NumericalSample NumericalMathEvaluationImplementation::operator() (const NumericalSample & inSample) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inSample.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inSample.getDimension();

  const UnsignedInteger size = inSample.getSize();
  NumericalSample outSample(size, getOutputDimension());
  // Simple loop over the evaluation operator based on point
  // The calls number is updated by these calls
  for (UnsignedInteger i = 0; i < size; ++i) outSample[i] = operator()(inSample[i]);
  outSample.setDescription(getOutputDescription());
  return outSample;
}


/* Operator () */
Field NumericalMathEvaluationImplementation::operator() (const Field & inField) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inField.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given time series has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inField.getDimension();
  return Field(inField.getMesh(), operator()(inField.getValues()));
}


/* Enable or disable the internal cache */
void NumericalMathEvaluationImplementation::enableCache() const
{
  p_cache_->enable();
}

void NumericalMathEvaluationImplementation::disableCache() const
{
  p_cache_->disable();
}

Bool NumericalMathEvaluationImplementation::isCacheEnabled() const
{
  return p_cache_->isEnabled();
}

UnsignedInteger NumericalMathEvaluationImplementation::getCacheHits() const
{
  return p_cache_->getHits();
}

void NumericalMathEvaluationImplementation::addCacheContent(const NumericalSample& inSample, const NumericalSample& outSample)
{
  p_cache_->enable();
  const UnsignedInteger size = inSample.getSize();
  for ( UnsignedInteger i = 0; i < size; ++ i )
  {
    p_cache_->add( inSample[i], outSample[i] );
  }
}

NumericalSample NumericalMathEvaluationImplementation::getCacheInput() const
{
  Bool cacheEnabled = isCacheEnabled();
  enableCache();
  PersistentCollection<CacheKeyType> keyColl( p_cache_->getKeys() );
  if ( ! cacheEnabled )
    disableCache();
  NumericalSample inSample(0, getInputDimension());
  for ( UnsignedInteger i = 0; i < keyColl.getSize(); ++ i ) inSample.add( keyColl[i] );
  return inSample;
}

NumericalSample NumericalMathEvaluationImplementation::getCacheOutput() const
{
  Bool cacheEnabled = isCacheEnabled();
  enableCache();
  PersistentCollection<CacheValueType> valuesColl( p_cache_->getValues() );
  if ( ! cacheEnabled )
    disableCache();
  NumericalSample outSample(0, getOutputDimension());
  for ( UnsignedInteger i = 0; i < valuesColl.getSize(); ++ i )
  {
    outSample.add( valuesColl[i] );
  }
  return outSample;
}

void NumericalMathEvaluationImplementation::clearCache() const
{
  p_cache_->clear();
}

/* Enable or disable the input/output history */
void NumericalMathEvaluationImplementation::enableHistory() const
{
  isHistoryEnabled_ = true;
}

void NumericalMathEvaluationImplementation::disableHistory() const
{
  isHistoryEnabled_ = false;
}

Bool NumericalMathEvaluationImplementation::isHistoryEnabled() const
{
  return isHistoryEnabled_;
}

void NumericalMathEvaluationImplementation::clearHistory() const
{
  inputStrategy_ = Full();
  outputStrategy_ = Full();
}

HistoryStrategy NumericalMathEvaluationImplementation::getHistoryInput() const
{
  return inputStrategy_;
}

HistoryStrategy NumericalMathEvaluationImplementation::getHistoryOutput() const
{
  return outputStrategy_;
}

/* Input point / parameter history accessor */
NumericalSample NumericalMathEvaluationImplementation::getInputPointHistory() const
{
  if (getParameterDimension() == 0) return inputStrategy_.getSample();
  throw NotYetImplementedException(HERE) << "in NumericalMathEvaluationImplementation::getInputPointHistory";
}

NumericalSample NumericalMathEvaluationImplementation::getInputParameterHistory() const
{
  throw NotYetImplementedException(HERE) << "in NumericalMathEvaluationImplementation::getInputParameterHistory";
}


/* Gradient according to the marginal parameters */
Matrix NumericalMathEvaluationImplementation::parameterGradient(const NumericalPoint & inP) const
{
  NumericalPoint parameter(getParameter());
  const UnsignedInteger parameterDimension = parameter.getDimension();
  const UnsignedInteger outputDimension = getOutputDimension();

  const NumericalScalar epsilon = ResourceMap::GetAsNumericalScalar("NumericalMathEvaluation-ParameterEpsilon");

  NumericalSample inS(parameterDimension + 1, parameter);
  for (UnsignedInteger i = 0; i < parameterDimension; ++ i)
  {
    inS[1 + i][i] += epsilon;
  }
  // operator()(x, theta) is non-const as it sets the parameter
  Pointer<NumericalMathEvaluationImplementation> p_evaluation(clone());
  NumericalSample outS(p_evaluation->operator()(inP, inS));

  Matrix grad(parameterDimension, outputDimension);
  for (UnsignedInteger i = 0; i < parameterDimension; ++ i)
  {
    for (UnsignedInteger j = 0; j < outputDimension; ++ j)
    {
      grad(i, j) = (outS[1 + i][j] - outS[0][j]) / epsilon;
    }
  }
  return grad;
}

/* Parameters value accessor */
NumericalPoint NumericalMathEvaluationImplementation::getParameter() const
{
  return parameter_;
}

void NumericalMathEvaluationImplementation::setParameter(const NumericalPoint & parameter)
{
  parameter_ = parameter;
}

void NumericalMathEvaluationImplementation::setParameterDescription(const Description & description)
{
  parameterDescription_ = description;
}

/* Parameters description accessor */
Description NumericalMathEvaluationImplementation::getParameterDescription() const
{
  return parameterDescription_;
}


/* Operator () */
NumericalPoint NumericalMathEvaluationImplementation::operator() (const NumericalPoint & inP) const
{
  throw NotYetImplementedException(HERE) << "In NumericalMathEvaluationImplementation::operator() (const NumericalPoint & inP) const";
}

NumericalPoint NumericalMathEvaluationImplementation::operator() (const NumericalPoint & inP,
    const NumericalPoint & parameter)
{
  setParameter(parameter);
  return (*this)(inP);
}

NumericalSample NumericalMathEvaluationImplementation::operator() (const NumericalPoint & inP,
    const NumericalSample & parameters)
{
  const UnsignedInteger size = parameters.getSize();
  NumericalSample outS(size, getOutputDimension());
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    setParameter(parameters[i]);
    outS[i] = operator()(inP);
  }
  return outS;
}

/* Accessor for input point dimension */
UnsignedInteger NumericalMathEvaluationImplementation::getInputDimension() const
{
  throw NotYetImplementedException(HERE) << "In NumericalMathEvaluationImplementation::getInputDimension() const";
}

/* Accessor for output point dimension */
UnsignedInteger NumericalMathEvaluationImplementation::getOutputDimension() const
{
  throw NotYetImplementedException(HERE) << "In NumericalMathEvaluationImplementation::getOutputDimension() const";
}

/* Accessor for input point dimension */
UnsignedInteger NumericalMathEvaluationImplementation::getParameterDimension() const
{
  return getParameter().getDimension();
}

/* Get the i-th marginal function */
NumericalMathEvaluationImplementation::Implementation NumericalMathEvaluationImplementation::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
NumericalMathEvaluationImplementation::Implementation NumericalMathEvaluationImplementation::getMarginal(const Indices & indices) const
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
  const AnalyticalNumericalMathEvaluationImplementation left(input, output, formulas);
#else
  NumericalPoint center(inputDimension);
  Matrix linear(inputDimension, outputDimension);
  for ( UnsignedInteger index = 0; index < outputDimension; ++ index )
    linear(indices[index], index) = 1.0;
  NumericalPoint constant(outputDimension);
  const LinearNumericalMathEvaluationImplementation left(center, constant, linear);
#endif
  ComposedNumericalMathEvaluationImplementation marginal(left.clone(), clone());
  if (isHistoryEnabled())
  {
    marginal.enableHistory();
  }
  return marginal.clone();
}

/* Get the number of calls to operator() */
UnsignedInteger NumericalMathEvaluationImplementation::getCallsNumber() const
{
  return callsNumber_;
}


/* Draw the given 1D marginal output as a function of the given 1D marginal input around the given central point */
Graph NumericalMathEvaluationImplementation::draw(const UnsignedInteger inputMarginal,
    const UnsignedInteger outputMarginal,
    const NumericalPoint & centralPoint,
    const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber,
    const GraphImplementation::LogScale scale) const
{
  if (getInputDimension() < 1) throw InvalidArgumentException(HERE) << "Error: cannot use this version of the draw() method with a function of input dimension less than 1";
  if (inputMarginal >= getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the given input marginal index=" << inputMarginal << " must be less than the input dimension=" << getInputDimension();
  if (outputMarginal >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the given output marginal index=" << outputMarginal << " must be less than the output dimension=" << getOutputDimension();
  if ((scale != GraphImplementation::NONE) && (scale != GraphImplementation::LOGX)) throw InvalidArgumentException(HERE) << "Error: expected scale=" << GraphImplementation::NONE << " or scale=" << GraphImplementation::LOGX << ", got scale=" << scale;
  if ((scale == GraphImplementation::LOGX) && ((xMin <= 0.0) || (xMax <= 0.0))) throw InvalidArgumentException(HERE) << "Error: cannot use logarithmic scale on an interval containing nonpositive values.";
  NumericalSample inputData(pointNumber, centralPoint);
  if (scale == GraphImplementation::NONE)
  {
    const NumericalScalar dx = (xMax - xMin) / (pointNumber - 1.0);
    for (UnsignedInteger i = 0; i < pointNumber; ++i)
      inputData[i][inputMarginal] = xMin + i * dx;
  }
  else
  {
    const NumericalScalar a = std::log(xMin);
    const NumericalScalar b = std::log(xMax);
    const NumericalScalar dLogX = (b - a) / (pointNumber - 1.0);
    for (UnsignedInteger i = 0; i < pointNumber; ++i)
      inputData[i][inputMarginal] = std::exp(a + i * dLogX);
  }
  // Evaluate the function over all its input in one call in order to benefit from potential parallelism
  const NumericalSample outputData((*this)(inputData));
  const Description inputDescription(getInputDescription());
  const Description outputDescription(getOutputDescription());
  const String xName(inputDescription[inputMarginal]);
  const String yName(outputDescription[outputMarginal]);
  String title(OSS() << yName << " as a function of " << xName);
  if (centralPoint.getDimension() > 1) title = String(OSS(false) << title << " around " << centralPoint);
  Graph graph(title, xName, yName, true, "", 1.0, scale);
  graph.add(Curve(inputData.getMarginal(inputMarginal), outputData.getMarginal(outputMarginal)));
  // Add a slight vertical margin
  GraphImplementation::BoundingBox bb(graph.getBoundingBox());
  const NumericalScalar height = bb[3] - bb[2];
  bb[2] -= 0.05 * height;
  bb[3] += 0.05 * height;
  graph.setBoundingBox(bb);
  return graph;
}

/* Draw the given 1D marginal output as a function of the given 2D marginal input around the given central point */
Graph NumericalMathEvaluationImplementation::draw(const UnsignedInteger firstInputMarginal,
    const UnsignedInteger secondInputMarginal,
    const UnsignedInteger outputMarginal,
    const NumericalPoint & centralPoint,
    const NumericalPoint & xMin,
    const NumericalPoint & xMax,
    const Indices & pointNumber,
    const GraphImplementation::LogScale scale) const
{
  if (getInputDimension() < 2) throw InvalidArgumentException(HERE) << "Error: cannot use this version of the draw() method with a function of input dimension less than 2";
  if ((xMin.getDimension() != 2) || (xMax.getDimension() != 2) || (pointNumber.getSize() != 2)) throw InvalidArgumentException(HERE) << "Error: xMin, xMax and PointNumber must be bidimensional";
  if ((pointNumber[0] <= 2) || (pointNumber[1] <= 2)) throw InvalidArgumentException(HERE) << "Error: the discretization must have at least 2 points per component";
  if ((scale != GraphImplementation::NONE) && (scale != GraphImplementation::LOGX) && (scale != GraphImplementation::LOGY) && (scale != GraphImplementation::LOGXY)) throw InvalidArgumentException(HERE) << "Error: expected scale=" << GraphImplementation::NONE << " or scale=" << GraphImplementation::LOGX << " or scale=" << GraphImplementation::LOGY << " or scale=" << GraphImplementation::LOGXY << ", got scale=" << scale;
  if (((scale == GraphImplementation::LOGX) || (scale == GraphImplementation::LOGXY)) && ((xMin[0] <= 0.0) || (xMax[0] <= 0.0))) throw InvalidArgumentException(HERE) << "Error: cannot use logarithmic scale on an interval containing nonpositive values for the first argument.";
  if (((scale == GraphImplementation::LOGY) || (scale == GraphImplementation::LOGXY)) && ((xMin[1] <= 0.0) || (xMax[1] <= 0.0))) throw InvalidArgumentException(HERE) << "Error: cannot use logarithmic scale on an interval containing nonpositive values for the second argument.";
  NumericalPoint discretization(2);
  NumericalPoint scaling(2);
  NumericalPoint origin(2);
  const NumericalScalar nX = pointNumber[0] - 2;
  discretization[0] = nX;
  // Discretization of the first component
  NumericalSample x(Box(NumericalPoint(1, nX)).generate());
  {
    NumericalScalar a = xMin[0];
    NumericalScalar b = xMax[0];
    if ((scale == GraphImplementation::LOGX) || (scale == GraphImplementation::LOGXY))
    {
      a = std::log(a);
      b = std::log(b);
    }
    origin[0] = a;
    scaling[0] = b - a;
  }
  x *= NumericalPoint(1, scaling[0]);
  x += NumericalPoint(1, origin[0]);
  // Recover the original scale if the discretization has been done in the logarithmic scale
  if ((scale == GraphImplementation::LOGY) || (scale == GraphImplementation::LOGXY))
    for (UnsignedInteger i = 0; i < x.getDimension(); ++i) x[i][0] = std::exp(x[i][0]);
  const NumericalScalar nY = pointNumber[1] - 2;
  discretization[1] = nY;
  // Discretization of the second component
  NumericalSample y(Box(NumericalPoint(1, nY)).generate());
  {
    NumericalScalar a = xMin[1];
    NumericalScalar b = xMax[1];
    if ((scale == GraphImplementation::LOGY) || (scale == GraphImplementation::LOGXY))
    {
      a = std::log(a);
      b = std::log(b);
    }
    origin[1] = a;
    scaling[1] = b - a;
  }
  y *= NumericalPoint(1, scaling[1]);
  y += NumericalPoint(1, origin[1]);
  // Recover the original scale if the discretization has been done in the logarithmic scale
  if ((scale == GraphImplementation::LOGY) || (scale == GraphImplementation::LOGXY))
    for (UnsignedInteger i = 0; i < y.getDimension(); ++i) y[i][0] = std::exp(y[i][0]);
  // Discretization of the XY plane
  NumericalSample inputSample((nX + 2) * (nY + 2), centralPoint);
  // Prepare the input sample
  UnsignedInteger index = 0;
  for (UnsignedInteger j = 0; j < nY + 2; ++j)
  {
    const NumericalScalar yJ = (scale == GraphImplementation::LOGY) || (scale == GraphImplementation::LOGXY) ? exp(y[j][0]) : y[j][0];
    for (UnsignedInteger i = 0; i < nX + 2; ++i)
    {
      const NumericalScalar xI = (scale == GraphImplementation::LOGX) || (scale == GraphImplementation::LOGXY) ? exp(x[i][0]) : x[i][0];
      inputSample[index][firstInputMarginal]  = xI;
      inputSample[index][secondInputMarginal]  = yJ;
      ++index;
    } // i
  } // j
  // Compute the output sample, using possible parallelism
  const NumericalSample z((*this)(inputSample).getMarginal(outputMarginal));
  const String xName(getInputDescription()[firstInputMarginal]);
  const String yName(getInputDescription()[secondInputMarginal]);
  String title(OSS() << getOutputDescription()[outputMarginal] << " as a function of (" << xName << "," << yName << ")");
  if (centralPoint.getDimension() > 2) title = String(OSS(false) << title << " around " << centralPoint);
  Graph graph(title, xName, yName, true, "topright", 1.0, scale);
  Contour isoValues(Contour(x, y, z, NumericalPoint(0), Description(0), true, title));
  isoValues.buildDefaultLevels();
  isoValues.buildDefaultLabels();
  NumericalPoint levels(isoValues.getLevels());
  Description labels(isoValues.getLabels());
  for (UnsignedInteger i = 0; i < levels.getDimension(); ++i)
  {
    Contour current(isoValues);
    current.setLevels(NumericalPoint(1, levels[i]));
    current.setLabels(Description(1, labels[i]));
    current.setDrawLabels(false);
    current.setLegend(labels[i]);
    current.setColor(Contour::ConvertFromHSV((360.0 * i / levels.getDimension()), 1.0, 1.0));
    graph.add(current);
  }
  return graph;
}

/* Draw the output of the function with respect to its input when the input and output dimensions are 1 */
Graph NumericalMathEvaluationImplementation::draw(const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber,
    const GraphImplementation::LogScale scale) const
{
  if (getInputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot draw a function with input dimension=" << getInputDimension() << " different from 1 using this method. See the other draw() methods.";
  if (getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot draw a function with output dimension=" << getInputDimension() << " different from 1 using this method. See the other draw() methods.";
  return draw(0, 0, NumericalPoint(1), xMin, xMax, pointNumber, scale);
}

/* Draw the output of the function with respect to its input when the input dimension is 2 and the output dimension is 1 */
Graph NumericalMathEvaluationImplementation::draw(const NumericalPoint & xMin,
    const NumericalPoint & xMax,
    const Indices & pointNumber,
    const GraphImplementation::LogScale scale) const
{
  if (getInputDimension() == 1) return draw(xMin[0], xMax[0], pointNumber[0], scale);
  if ((getInputDimension() == 0) || (getInputDimension() > 2)) throw InvalidArgumentException(HERE) << "Error: cannot draw a function with input dimension=" << getInputDimension() << " different from 1 or 2 using this method. See the other draw() methods.";
  if (getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot draw a function with output dimension=" << getInputDimension() << " different from 1 using this method. See the other draw() methods.";
  return draw(0, 1, 0, NumericalPoint(2), xMin, xMax, pointNumber, scale);
}


/* Method save() stores the object through the StorageManager */
void NumericalMathEvaluationImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "callsNumber_", callsNumber_ );
  adv.saveAttribute( "cache_", *p_cache_ );
  adv.saveAttribute( "inputDescription_", inputDescription_ );
  adv.saveAttribute( "outputDescription_", outputDescription_ );
  adv.saveAttribute( "parameter_", parameter_ );
  adv.saveAttribute( "parameterDescription_", parameterDescription_ );
}

/* Method load() reloads the object from the StorageManager */
void NumericalMathEvaluationImplementation::load(Advocate & adv)
{
  TypedInterfaceObject<CacheType> cache;
  PersistentObject::load(adv);
  adv.loadAttribute( "callsNumber_", callsNumber_ );
  adv.loadAttribute( "cache_", cache );
  p_cache_ = cache.getImplementation();
  adv.loadAttribute( "inputDescription_", inputDescription_ );
  adv.loadAttribute( "outputDescription_", outputDescription_ );
  adv.loadAttribute( "parameter_", parameter_ );
  adv.loadAttribute( "parameterDescription_", parameterDescription_ );
}

END_NAMESPACE_OPENTURNS
