//                                               -*- C++ -*-
/**
 *  @brief Memoize the evaluations of a point to field function
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
#include "openturns/MemoizePointToFieldFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/OSS.hxx"
#include <set>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MemoizePointToFieldFunction)

static const Factory<MemoizePointToFieldFunction> Factory_MemoizePointToFieldFunction;

/* Default constructor */
MemoizePointToFieldFunction::MemoizePointToFieldFunction()
  : PointToFieldFunctionImplementation()
  , function_()
  , cache_()
{
  // Nothing to do
}

/* Parameter constructor */
MemoizePointToFieldFunction::MemoizePointToFieldFunction(const PointToFieldFunction & function)
  : PointToFieldFunctionImplementation(function.getInputDimension(), function.getOutputMesh(), function.getOutputDimension())
  , function_(function)
  , cache_()
{
  setInputDescription(function.getInputDescription());
  setOutputDescription(function.getOutputDescription());
}

/* Virtual constructor */
MemoizePointToFieldFunction * MemoizePointToFieldFunction::clone() const
{
  return new MemoizePointToFieldFunction(*this);
}

/* Rebuild field values from a flattened cache entry */
static Sample UnflattenValues(const MemoizePointToFieldFunction::CacheValueType & flatValues,
                              const UnsignedInteger verticesNumber,
                              const UnsignedInteger outputDimension)
{
  Sample values(verticesNumber, outputDimension);
  for (UnsignedInteger v = 0; v < verticesNumber; ++v)
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
      values(v, j) = flatValues[v * outputDimension + j];
  return values;
}

/* Operator () */
Sample MemoizePointToFieldFunction::operator() (const Point & inP) const
{
  const UnsignedInteger verticesNumber = getOutputMesh().getVerticesNumber();
  const UnsignedInteger outputDimension = getOutputDimension();
  if (cache_.isEnabled())
  {
    const CacheKeyType inKey(inP.getCollection());
    if (cache_.hasKey(inKey))
      return UnflattenValues(cache_.find(inKey), verticesNumber, outputDimension);
    const Sample values(function_(inP));
    callsNumber_.increment();
    cache_.add(inKey, CacheValueType(values.getImplementation()->getData().getCollection()));
    return values;
  }
  const Sample values(function_(inP));
  callsNumber_.increment();
  return values;
}

/* Operator () */
ProcessSample MemoizePointToFieldFunction::operator() (const Sample & inS) const
{
  const UnsignedInteger size = inS.getSize();
  const UnsignedInteger verticesNumber = getOutputMesh().getVerticesNumber();
  const UnsignedInteger outputDimension = getOutputDimension();
  ProcessSample outSample;
  if (cache_.isEnabled())
  {
    // Evaluate only the inputs missing from the cache, once each
    std::set<Point> uniqueValues;
    Indices toDoIndices;
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      const CacheKeyType inKey(inS[i].getCollection());
      if (!cache_.hasKey(inKey) && (uniqueValues.find(inS[i]) == uniqueValues.end()))
      {
        toDoIndices.add(i);
        uniqueValues.insert(inS[i]);
      }
    }
    const Sample toDo(inS.select(toDoIndices));
    const UnsignedInteger toDoSize = toDo.getSize();
    // Keep the new entries in a temporary cache until the output sample is
    // built, so that an eviction from the main cache cannot lose them
    CacheType tempCache(toDoSize);
    tempCache.enable();
    if (toDoSize > 0)
    {
      const ProcessSample computed(function_(toDo));
      callsNumber_.fetchAndAdd(toDoSize);
      for (UnsignedInteger i = 0; i < toDoSize; ++ i)
      {
        const Sample values(computed[i]);
        tempCache.add(CacheKeyType(toDo[i].getCollection()), CacheValueType(values.getImplementation()->getData().getCollection()));
      }
    }
    const Mesh mesh(getOutputMesh());
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      const CacheKeyType inKey(inS[i].getCollection());
      const CacheValueType flatValues(cache_.hasKey(inKey) ? cache_.find(inKey) : tempCache.find(inKey));
      outSample.add(Field(mesh, UnflattenValues(flatValues, verticesNumber, outputDimension)));
    }
    cache_.merge(tempCache);
    return outSample;
  }
  const ProcessSample computed(function_(inS));
  callsNumber_.fetchAndAdd(size);
  return computed;
}

/* Enable or disable the cache */
void MemoizePointToFieldFunction::enableCache() const
{
  cache_.enable();
}

void MemoizePointToFieldFunction::disableCache() const
{
  cache_.disable();
}

/* Test the cache mechanism activity */
Bool MemoizePointToFieldFunction::isCacheEnabled() const
{
  return cache_.isEnabled();
}

/* Get the number of cache hits */
UnsignedInteger MemoizePointToFieldFunction::getCacheHits() const
{
  return cache_.getHits();
}

/* Empty the cache */
void MemoizePointToFieldFunction::clearCache() const
{
  cache_.clear();
}

/* Accessor to the cached input points */
Sample MemoizePointToFieldFunction::getInputCache() const
{
  const Bool cacheEnabled = isCacheEnabled();
  enableCache();
  const PersistentCollection<CacheKeyType> keyColl(cache_.getKeys());
  if (!cacheEnabled)
    disableCache();
  Sample inSample(0, getInputDimension());
  for (UnsignedInteger i = 0; i < keyColl.getSize(); ++ i)
    inSample.add(keyColl[i]);
  return inSample;
}

/* Accessor to the cached output fields */
ProcessSample MemoizePointToFieldFunction::getOutputCache() const
{
  const Bool cacheEnabled = isCacheEnabled();
  enableCache();
  const PersistentCollection<CacheValueType> valuesColl(cache_.getValues());
  if (!cacheEnabled)
    disableCache();
  const Mesh mesh(getOutputMesh());
  const UnsignedInteger verticesNumber = mesh.getVerticesNumber();
  const UnsignedInteger outputDimension = getOutputDimension();
  ProcessSample outSample;
  for (UnsignedInteger i = 0; i < valuesColl.getSize(); ++ i)
    outSample.add(Field(mesh, UnflattenValues(valuesColl[i], verticesNumber, outputDimension)));
  return outSample;
}

/* Accessor to the wrapped function */
PointToFieldFunction MemoizePointToFieldFunction::getFunction() const
{
  return function_;
}

/* String converter */
String MemoizePointToFieldFunction::__repr__() const
{
  OSS oss;
  oss << "class=" << MemoizePointToFieldFunction::GetClassName()
      << " function=" << function_.__str__();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void MemoizePointToFieldFunction::save(Advocate & adv) const
{
  PointToFieldFunctionImplementation::save(adv);
  adv.saveAttribute("function_", function_);
  // Enable the cache temporarily so that its content is saved even when
  // the cache is disabled
  const Bool cacheEnabled = isCacheEnabled();
  enableCache();
  adv.saveAttribute("cacheEnabled", cacheEnabled);
  adv.saveAttribute("cacheInput", getInputCache());
  // Stack the cached field values into a single sample
  const PersistentCollection<CacheValueType> valuesColl(cache_.getValues());
  const UnsignedInteger verticesNumber = getOutputMesh().getVerticesNumber();
  const UnsignedInteger outputDimension = getOutputDimension();
  Sample stackedValues(0, outputDimension);
  for (UnsignedInteger i = 0; i < valuesColl.getSize(); ++ i)
  {
    const CacheValueType & flatValues = valuesColl[i];
    for (UnsignedInteger v = 0; v < verticesNumber; ++ v)
    {
      Point row(outputDimension);
      for (UnsignedInteger j = 0; j < outputDimension; ++ j)
        row[j] = flatValues[v * outputDimension + j];
      stackedValues.add(row);
    }
  }
  adv.saveAttribute("cacheValues", stackedValues);
  if (!cacheEnabled)
    disableCache();
}

/* Method load() reloads the object from the StorageManager */
void MemoizePointToFieldFunction::load(Advocate & adv)
{
  PointToFieldFunctionImplementation::load(adv);
  adv.loadAttribute("function_", function_);
  if (adv.hasAttribute("cacheEnabled"))
  {
    Bool cacheEnabled = true;
    adv.loadAttribute("cacheEnabled", cacheEnabled);
    Sample cacheInput;
    Sample stackedValues;
    adv.loadAttribute("cacheInput", cacheInput);
    adv.loadAttribute("cacheValues", stackedValues);
    const UnsignedInteger verticesNumber = getOutputMesh().getVerticesNumber();
    const UnsignedInteger outputDimension = getOutputDimension();
    enableCache();
    for (UnsignedInteger i = 0; i < cacheInput.getSize(); ++ i)
    {
      CacheValueType flatValues(verticesNumber * outputDimension);
      for (UnsignedInteger v = 0; v < verticesNumber; ++ v)
        for (UnsignedInteger j = 0; j < outputDimension; ++ j)
          flatValues[v * outputDimension + j] = stackedValues(i * verticesNumber + v, j);
      cache_.add(CacheKeyType(cacheInput[i].getCollection()), flatValues);
    }
    if (!cacheEnabled)
      disableCache();
  }
}

END_NAMESPACE_OPENTURNS
