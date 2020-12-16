//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between numerical
 *        math functions implementations
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
  , isHistoryEnabled_(true)
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
  , isHistoryEnabled_(true)
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
    inputStrategy_.store(inPoint);
    outputStrategy_.store(outPoint);
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
  Sample toDo(0, inDim);
  if (p_cache_->isEnabled())
  {
    outSample = Sample(size, outDim);
    std::set<Point> uniqueValues;
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      CacheKeyType inKey(inSample[i].getCollection());
      if (p_cache_->hasKey(inKey))
      {
        outSample[i] = Point::ImplementationType(p_cache_->find(inKey));
      }
      else
      {
        uniqueValues.insert(inSample[i]);
      }
    }
    for(std::set<Point>::const_iterator it = uniqueValues.begin(); it != uniqueValues.end(); ++ it)
    {
      // store unique values
      toDo.add(*it);
    }
    UnsignedInteger toDoSize = toDo.getSize();
    CacheType tempCache(toDoSize);
    tempCache.enable();

    if (toDoSize > 0)
    {
      const Sample result(evaluation_.operator()(toDo));
      callsNumber_.fetchAndAdd(toDoSize);
      for (UnsignedInteger i = 0; i < toDoSize; ++ i)
        tempCache.add(toDo[i], result[i]);
    }
    // Fill all the output values
    for(UnsignedInteger i = 0; i < size; ++ i)
    {
      CacheKeyType inKey(inSample[i].getCollection());
      if (tempCache.hasKey(inKey))
      {
        outSample[i] = Point::ImplementationType(tempCache.find(inKey));
      }
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
    inputStrategy_.store(inSample);
    outputStrategy_.store(outSample);
  }
  return outSample;
}

/* Get the evaluation corresponding to indices components */
Evaluation MemoizeEvaluation::getMarginal(const Indices & indices) const
{
  MemoizeEvaluation* marginal = new MemoizeEvaluation(evaluation_.getMarginal(indices), inputStrategy_);
  if (isCacheEnabled())
    marginal->addCacheContent(getCacheInput(), getCacheOutput().getMarginal(indices));
  return marginal;
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
  Bool cacheEnabled = isCacheEnabled();
  enableCache();
  PersistentCollection<CacheKeyType> keyColl(p_cache_->getKeys());
  if (!cacheEnabled)
    disableCache();
  Sample inSample(0, getInputDimension());
  for (UnsignedInteger i = 0; i < keyColl.getSize(); ++ i) inSample.add(keyColl[i]);
  return inSample;
}

Sample MemoizeEvaluation::getCacheOutput() const
{
  Bool cacheEnabled = isCacheEnabled();
  enableCache();
  PersistentCollection<CacheValueType> valuesColl(p_cache_->getValues());
  if (! cacheEnabled)
    disableCache();
  Sample outSample(0, getOutputDimension());
  for (UnsignedInteger i = 0; i < valuesColl.getSize(); ++ i)
  {
    outSample.add(valuesColl[i]);
  }
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
  return OSS(true) << evaluation_.getImplementation()->__repr__();
}

String MemoizeEvaluation::__str__(const String & offset) const
{
  return OSS(false) << evaluation_.getImplementation()->__str__(offset);
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
  adv.saveAttribute("cache_", *p_cache_);
}

/* Method load() reloads the object from the StorageManager */
void MemoizeEvaluation::load(Advocate & adv)
{
  EvaluationProxy::load(adv);
  adv.loadAttribute("inputStrategy_", inputStrategy_);
  adv.loadAttribute("outputStrategy_", outputStrategy_);
  adv.loadAttribute("isHistoryEnabled_", isHistoryEnabled_);
  adv.loadAttribute("cache_", *p_cache_);
}

END_NAMESPACE_OPENTURNS
