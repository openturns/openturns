//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between numerical
 *        math functions implementations
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/MemoizeEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MarginalEvaluation.hxx"
#include "openturns/BatchFailedException.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef MemoizeEvaluation::CacheType MemoizeEvaluationCache;

static const Factory<MemoizeEvaluationCache> Factory_MemoizeEvaluationCache;

/* These methods are implemented here for the needs of Cache */
/* We should be careful because they may interfere with other definitions placed elsewhere */
static const Factory<PersistentCollection<UnsignedInteger> > Factory_PersistentCollection_UnsignedInteger;
#ifndef OPENTURNS_UNSIGNEDLONG_SAME_AS_UINT64
static const Factory<PersistentCollection<Unsigned64BitsInteger> > Factory_PersistentCollection_Unsigned64BitsInteger;
#endif

TEMPLATE_CLASSNAMEINIT(PersistentCollection<PersistentCollection<Scalar> >)
static const Factory<PersistentCollection<PersistentCollection<Scalar> > > Factory_PersistentCollection_PersistentCollection_Scalar;


CLASSNAMEINIT(MemoizeEvaluation)

static const Factory<MemoizeEvaluation> Factory_MemoizeEvaluation;

/* Default constructor */
MemoizeEvaluation::MemoizeEvaluation()
  : EvaluationProxy()
  , inputStrategy_(Full())
  , outputStrategy_(Full())
  , p_cache_(new CacheType)
{
  // We disable the cache by default
  p_cache_->disable();
}

/* Parameter constructor */
MemoizeEvaluation::MemoizeEvaluation(const Evaluation & evaluation, const HistoryStrategy & historyStrategy)
  : EvaluationProxy(evaluation)
  , inputStrategy_(historyStrategy)
  , outputStrategy_(historyStrategy)
  , p_cache_(new CacheType)
{
  setEvaluation(evaluation);
}

/* Virtual constructor */
MemoizeEvaluation * MemoizeEvaluation::clone() const
{
  return new MemoizeEvaluation(*this);
}

/** Function implementation accessors */
void MemoizeEvaluation::setEvaluation(const Evaluation & evaluation)
{
  inputStrategy_.setDimension(evaluation_.getInputDimension());
  outputStrategy_.setDimension(evaluation_.getOutputDimension());
  // If argument is a MemoizeEvaluation, copy history and cache
  MemoizeEvaluation * p_MemoizeEvaluation = dynamic_cast<MemoizeEvaluation*>(evaluation.getImplementation().get());
  if (p_MemoizeEvaluation)
  {
    const Sample inSample(p_MemoizeEvaluation->getInputHistory());
    const Sample outSample(p_MemoizeEvaluation->getOutputHistory());
    inputStrategy_.store(inSample);
    outputStrategy_.store(outSample);
    isHistoryEnabled_ = p_MemoizeEvaluation->isHistoryEnabled_;
    p_cache_ = p_MemoizeEvaluation->p_cache_;
    // To avoid nested MemoizeEvaluation
    evaluation_ = p_MemoizeEvaluation->evaluation_;
  }
  else
    evaluation_ = evaluation;
}

/* Operator () */
Point MemoizeEvaluation::operator() (const Point & inPoint) const
{
  Point outPoint;
  if (p_cache_->isEnabled())
  {
    // If cache is enabled
    CacheKeyType inKey = inPoint.getCollection();
    if (p_cache_->hasKey(inKey))
    {
      outPoint = Point::ImplementationType(p_cache_->find(inKey));
    }
    else
    {
      outPoint = evaluation_.operator()(inPoint);
      callsNumber_.increment();
      CacheValueType outValue(outPoint.getCollection());
      p_cache_->add(inKey, outValue);
    }
  }
  else
  {
    outPoint = evaluation_.operator()(inPoint);
    callsNumber_.increment();
  }

  if (isHistoryEnabled_)
  {
    inputStrategy_.getImplementation()->store(inPoint);
    outputStrategy_.getImplementation()->store(outPoint);
  }
  return outPoint;
}

/* Operator () */
Sample MemoizeEvaluation::operator() (const Sample & inSample) const
{
  const UnsignedInteger size = inSample.getSize();
  const UnsignedInteger inDim = inSample.getDimension();
  const UnsignedInteger outDim = getOutputDimension();

  Sample outSample;
  if (p_cache_->isEnabled())
  {
    outSample = Sample(size, outDim);
    std::map<Point, UnsignedInteger> uniqueValues;
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      CacheKeyType inKey(inSample[i].getCollection());
      if (p_cache_->hasKey(inKey))
      {
        outSample[i] = Point::ImplementationType(p_cache_->find(inKey));
      }
      else
      {
        uniqueValues[inSample[i]] = i;
      }
    }
    Sample toDo(0, inDim);
    Indices toDoIndices;
    for (auto const & entry : uniqueValues)
    {
      // store unique values
      toDo.add(entry.first);
      toDoIndices.add(entry.second);
    }
    UnsignedInteger toDoSize = toDo.getSize();
    CacheType tempCache(toDoSize);
    tempCache.enable();

    if (toDoSize > 0)
    {
      try
      {
        const Sample result(evaluation_.operator()(toDo));
        callsNumber_.fetchAndAdd(toDoSize);
        for (UnsignedInteger i = 0; i < toDoSize; ++ i)
          tempCache.add(toDo[i], result[i]);
      }
      catch (const BatchFailedException & exc)
      {
        // rethrow another BatchFailedException, but with updated indices
        // as the original evaluation is called on a subset of the input sample
        callsNumber_.fetchAndAdd(toDoSize);
        const Indices localOkIndices(exc.getSucceededIndices());
        const Indices localFailedIndices(exc.getFailedIndices());
        const Sample localOkSample(exc.getOutputSample());
        const Description localErrorDescription(exc.getErrorDescription());
        Indices failedIndices;
        Description errorDescription;
        Sample okSample(0, outDim);
        okSample.setDescription(evaluation_.getOutputDescription());
        for (UnsignedInteger i = 0; i < localOkIndices.getSize(); ++ i)
          tempCache.add(toDo[localOkIndices[i]], localOkSample[i]);
        std::map<Point, String> failCache;
        for (UnsignedInteger i = 0; i < localFailedIndices.getSize(); ++ i)
          failCache[toDo[localFailedIndices[i]]] = localErrorDescription[i];
        for (UnsignedInteger i = 0; i < size; ++ i)
        {
          const CacheKeyType inKey(inSample[i].getCollection());
          if (p_cache_->hasKey(inKey))
            okSample.add(outSample[i]);// already retrieved
          else if (tempCache.hasKey(inKey))
            okSample.add(Point::ImplementationType(tempCache.find(inKey)));
          else
          {
            failedIndices.add(i);
            errorDescription.add(failCache[inSample[i]]);
          }
        }
        p_cache_->merge(tempCache);
        const Indices okIndices(failedIndices.complement(size));
        throw BatchFailedException(HERE, failedIndices, errorDescription, okIndices, okSample) << exc.what();
      }
    }
    // Fill remaining output values
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      const CacheKeyType inKey(inSample[i].getCollection());
      if (tempCache.hasKey(inKey))
        outSample[i] = Point::ImplementationType(tempCache.find(inKey));
    }
    p_cache_->merge(tempCache);
  }
  else
  {
    // Cache not enabled
    outSample = evaluation_.operator()(inSample);
    callsNumber_.fetchAndAdd(size);
  }

  outSample.setDescription(evaluation_.getOutputDescription());
  if (isHistoryEnabled_)
  {
    inputStrategy_.getImplementation()->store(inSample);
    outputStrategy_.getImplementation()->store(outSample);
  }
  return outSample;
}

/* Get the evaluation corresponding to indices components */
Evaluation MemoizeEvaluation::getMarginal(const Indices & indices) const
{
  // don't rely on the proxy here, we want a marginal on the memoized original evaluation
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal evaluation must be in the range [0, outputDimension-1] and must be different";
  Indices full(getOutputDimension());
  full.fill();
  if (indices == full) return clone();
  return new MarginalEvaluation(clone(), indices);
}

/* Enable or disable the internal cache */
void MemoizeEvaluation::enableCache() const
{
  p_cache_->enable();
}

void MemoizeEvaluation::disableCache() const
{
  p_cache_->disable();
}

Bool MemoizeEvaluation::isCacheEnabled() const
{
  return p_cache_->isEnabled();
}

UnsignedInteger MemoizeEvaluation::getCacheHits() const
{
  return p_cache_->getHits();
}

void MemoizeEvaluation::addCacheContent(const Sample& inSample, const Sample& outSample)
{
  p_cache_->enable();
  const UnsignedInteger size = inSample.getSize();
  const UnsignedInteger cacheSize = p_cache_->getMaxSize();
  const UnsignedInteger start = size <= cacheSize ? 0 : size - cacheSize;
  for (UnsignedInteger i = start; i < size; ++ i)
  {
    p_cache_->add(inSample[i], outSample[i]);
  }
}

Sample MemoizeEvaluation::getCacheInput() const
{
  const Bool cacheEnabled = isCacheEnabled();
  enableCache();
  const PersistentCollection<CacheKeyType> keyColl(p_cache_->getKeys());
  if (!cacheEnabled)
    disableCache();
  Sample inSample(0, getInputDimension());
  for (UnsignedInteger i = 0; i < keyColl.getSize(); ++ i)
    inSample.add(keyColl[i]);
  return inSample;
}

Sample MemoizeEvaluation::getCacheOutput() const
{
  const Bool cacheEnabled = isCacheEnabled();
  enableCache();
  const PersistentCollection<CacheValueType> valuesColl(p_cache_->getValues());
  if (!cacheEnabled)
    disableCache();
  Sample outSample(0, getOutputDimension());
  for (UnsignedInteger i = 0; i < valuesColl.getSize(); ++ i)
    outSample.add(valuesColl[i]);
  return outSample;
}

void MemoizeEvaluation::clearCache() const
{
  p_cache_->clear();
}

/* Enable or disable the input/output history */
void MemoizeEvaluation::enableHistory() const
{
  isHistoryEnabled_ = true;
}

void MemoizeEvaluation::disableHistory() const
{
  isHistoryEnabled_ = false;
}

/* Test the history mechanism activity */
Bool MemoizeEvaluation::isHistoryEnabled() const
{
  return isHistoryEnabled_;
}

/* Clear history of the input and output values */
void MemoizeEvaluation::clearHistory() const
{
  inputStrategy_.clear();
  outputStrategy_.clear();
}

/* Retrieve the history of the input values */
Sample MemoizeEvaluation::getInputHistory() const
{
  return inputStrategy_.getSample();
}

/* Retrieve the history of the output values */
Sample MemoizeEvaluation::getOutputHistory() const
{
  return outputStrategy_.getSample();
}

Evaluation MemoizeEvaluation::getEvaluation() const
{
  return evaluation_;
}

/* Comparison operator */
Bool MemoizeEvaluation::operator ==(const MemoizeEvaluation & other) const
{
  return evaluation_ == other.evaluation_;
}

/* String converter */
String MemoizeEvaluation::__repr__() const
{
  return OSS(true) << "MemoizeEvaluation(" << evaluation_.getImplementation()->__repr__() << ")";
}

String MemoizeEvaluation::__str__(const String & offset) const
{
  return OSS(false) << "MemoizeEvaluation(" << evaluation_.getImplementation()->__str__(offset) << ")";
}

/* Is it safe to call in parallel? */
Bool MemoizeEvaluation::isParallel() const
{
  return false;
}

/* Method save() stores the object through the StorageManager */
void MemoizeEvaluation::save(Advocate & adv) const
{
  EvaluationProxy::save(adv);
  adv.saveAttribute("inputStrategy_", inputStrategy_);
  adv.saveAttribute("outputStrategy_", outputStrategy_);
  adv.saveAttribute("isHistoryEnabled_", isHistoryEnabled_);
  const Bool cacheEnabled = isCacheEnabled();
  adv.saveAttribute("cacheEnabled", cacheEnabled);
  const Sample cacheInput(getCacheInput());
  const Sample cacheOutput(getCacheOutput());
  adv.saveAttribute("cacheInput", cacheInput);
  adv.saveAttribute("cacheOutput", cacheOutput);
}

/* Method load() reloads the object from the StorageManager */
void MemoizeEvaluation::load(Advocate & adv)
{
  EvaluationProxy::load(adv);
  adv.loadAttribute("inputStrategy_", inputStrategy_);
  adv.loadAttribute("outputStrategy_", outputStrategy_);
  adv.loadAttribute("isHistoryEnabled_", isHistoryEnabled_);
  if (adv.hasAttribute("cacheEnabled"))
  {
    Bool cacheEnabled = true;
    adv.loadAttribute("cacheEnabled", cacheEnabled);
    Sample cacheInput;
    Sample cacheOutput;
    adv.loadAttribute("cacheInput", cacheInput);
    adv.loadAttribute("cacheOutput", cacheOutput);
    addCacheContent(cacheInput, cacheOutput); // enables the cache
    if (!cacheEnabled)
      disableCache();
  }
  else // old approach
    adv.loadAttribute("cache_", *p_cache_);
}

END_NAMESPACE_OPENTURNS
