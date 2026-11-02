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
#ifndef OPENTURNS_MEMOIZEPOINTOFIELDFUNCTION_HXX
#define OPENTURNS_MEMOIZEPOINTOFIELDFUNCTION_HXX

#include "openturns/PointToFieldFunctionImplementation.hxx"
#include "openturns/PointToFieldFunction.hxx"
#include "openturns/Cache.hxx"
#include "openturns/Field.hxx"
#include "openturns/ProcessSample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MemoizePointToFieldFunction
 *
 * This class caches the evaluations of a point to field function, so that
 * repeated evaluations at the same input points reuse the stored fields
 * instead of calling the wrapped function again. See issue #2802.
 */
class OT_API MemoizePointToFieldFunction
  : public PointToFieldFunctionImplementation
{
  CLASSNAME
public:

  typedef PersistentCollection<Scalar>                        CacheKeyType;
  typedef PersistentCollection<Scalar>                        CacheValueType;
  typedef Cache<CacheKeyType, CacheValueType>                 CacheType;

  /** Default constructor */
  MemoizePointToFieldFunction();

#ifdef SWIG
  /** Dummy copy ctor to fix overload precedence */
  MemoizePointToFieldFunction(const MemoizePointToFieldFunction & other);
#endif

  /** Parameter constructor */
  explicit MemoizePointToFieldFunction(const PointToFieldFunction & function);

  /** Virtual constructor */
  MemoizePointToFieldFunction * clone() const override;

  /** Operator () */
  Sample operator() (const Point & inP) const override;
  ProcessSample operator() (const Sample & inS) const override;

  /** Enable or disable the cache */
  void enableCache() const;
  void disableCache() const;

  /** Test the cache mechanism activity */
  Bool isCacheEnabled() const;

  /** Get the number of cache hits */
  UnsignedInteger getCacheHits() const;

  /** Empty the cache */
  void clearCache() const;

  /** Accessor to the cached input points */
  Sample getInputCache() const;

  /** Accessor to the cached output fields */
  ProcessSample getOutputCache() const;

  /** Accessor to the wrapped function */
  PointToFieldFunction getFunction() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  /** The wrapped function */
  PointToFieldFunction function_;

  /** The cache, mapping input points to flattened field values */
  mutable CacheType cache_;

}; /* class MemoizePointToFieldFunction */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MEMOIZEPOINTOFIELDFUNCTION_HXX */
