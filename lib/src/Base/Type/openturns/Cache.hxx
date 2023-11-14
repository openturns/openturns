//                                               -*- C++ -*-
/**
 *  @brief Cache holds the already computed points to speed up calculations
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
#ifndef OPENTURNS_CACHE_HXX
#define OPENTURNS_CACHE_HXX

#include <map>
#include <list>
#include <sstream>
#include "openturns/PersistentObject.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Point.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Log.hxx"
#include "openturns/OStream.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class Cache
 *
 * Cache holds the already computed points to speed up calculations
 * Like any cache system, this objet saves time by keeping some costly data
 * and provides access to them through a key.
 * Here the key is the input Point of a Function and the
 * value is the output Point that was computed.
 * When used with ComputedFunctionImplementation objects the Cache
 * may greatly speed up the computations of Analytical algorithms (like FORM
 * or SORM). Its advantage is more doubtful with other FunctionImplementations
 * or Simulation algorithms.
 *
 * The Cache may be set with a maximum size to avoid too much memory consumption especially
 * when running Simulation algorithms (Monte-Carlo or LHS). This maximum size is
 * an upper bound for the number of data kept by the Cache. When this upper bound is
 * reached, the next data insertion into the Cache flushes the least recently used
 * data before the insertion.
 */

template <typename KeyType, typename ValueType>
class Cache
  : public PersistentObject
{
  typedef typename std::list<KeyType> list_key_type;
  typedef typename list_key_type::iterator         list_key_iterator;
  typedef typename list_key_type::const_iterator   list_key_const_iterator;
  typedef typename list_key_type::reverse_iterator list_key_reverse_iterator;
  typedef typename std::map<KeyType, std::pair<ValueType, list_key_iterator> > map_type;

public:
  static  OT::String GetClassName()
  {
    return OT::String("Cache<") + OT::String(KeyType::GetClassName()) + OT::String(", ") + OT::String(ValueType::GetClassName()) + OT::String(">");
  }
  virtual OT::String getClassName() const
  {
    return Cache<KeyType, ValueType>::GetClassName();
  }

protected:

  /** True if cache is enabled */
  mutable Bool enabled_;

  /** Upper bound for the cache size */
  const UnsignedInteger maxSize_;

  /** Number of hits */
  mutable UnsignedInteger hits_;

  /** The list of keys */
  mutable list_key_type keys_;

  /** The map of elements */
  mutable map_type points_;

public:

  /** Default constructor */
  inline
  Cache() : PersistentObject(),
    enabled_(true),
    maxSize_(ResourceMap::GetAsUnsignedInteger("Cache-MaxSize")),
    hits_(0),
    keys_(),
    points_()
  {
    // Nothing to do
  }

  /** Constructor with upper bound size */
  inline
  Cache(const UnsignedInteger maxSize) : PersistentObject(),
    enabled_(true),
    maxSize_(maxSize),
    hits_(0),
    keys_(),
    points_()
  {
    // Nothing to do
  }

  Cache(const Cache & other) : PersistentObject(other),
    enabled_(other.enabled_),
    maxSize_(other.maxSize_),
    hits_(other.hits_),
    points_()
  {
    for(list_key_reverse_iterator it = other.keys_.rbegin(); it != other.keys_.rend(); ++it)
    {
      const KeyType & key = *it;
      keys_.push_front(key);
      points_.insert(std::make_pair(key, std::make_pair(other.points_[key].first, keys_.begin())));
    }
  }

  /** Virtual constructor */
  inline virtual Cache * clone() const
  {
    return new Cache(*this);
  }

  /** String converter */
  inline
  virtual String __repr__() const
  {
    OSS oss;
    oss << "class=" << Cache::GetClassName()
        << " enabled=" << enabled_
        << " name=" << getName()
        << " maxSize=" << maxSize_
        << " size=" << getSize()
        << " hits=" << getHits()
        << " points={" ;
    Bool first = true;
    for(list_key_const_iterator it = keys_.begin(); it != keys_.end(); ++it)
    {
      if (!first) oss << ", ";
      const KeyType & key = *it;
      oss << std::make_pair(key, points_[key].first);
      first = false;
    }
    oss << "}" ;

    return oss;
  }


#ifndef SWIG
  /** Assignment operator */
  inline
  Cache & operator = (const Cache & other)
  {
    if (this != &other)
    {
      clear();
      PersistentObject::operator=(other);
      const_cast<UnsignedInteger&>(maxSize_) = other.maxSize_;
      hits_                                  = other.hits_;
      enabled_                               = other.enabled_;
      for(list_key_reverse_iterator it = other.keys_.rbegin(); it != other.keys_.rend(); ++it)
      {
        const KeyType & key = *it;
        keys_.push_front(key);
        points_.insert(std::make_pair(key, std::make_pair(other.points_[key].first, keys_.begin())));
      }
    }
    return *this;
  }

#endif

  /** Merge the contents of two caches */
  inline
  Cache & merge (const Cache & other)
  {
    if (enabled_)
    {
      for(list_key_reverse_iterator it = other.keys_.rbegin(); it != other.keys_.rend(); ++it)
      {
        const KeyType & key = *it;
        const ValueType & value = other.points_.find(key)->second.first;
        add(key, value);
      }
    }
    return *this;
  }

  /** Returns the number of successful hits in the cache */
  inline UnsignedInteger getHits() const
  {
    return hits_;
  }

  /** Query the cache for the key presence */
  inline
  Bool hasKey(const KeyType & key) const
  {
    if (!enabled_) return false;
    Bool found = (points_.find(key) != points_.end());
    return found;
  }

  /** Retrieve the value from the cache with the key */
  inline
  const ValueType find(const KeyType & key) const
  {
    if (!enabled_) return ValueType();

    typename map_type::iterator it = points_.find(key);
    Bool found = (it != points_.end());
    if (!found) return ValueType();

    list_key_iterator pos = it->second.second;
    if (pos != keys_.begin())
    {
      // Move key at the beginning
      keys_.erase(pos);
      keys_.push_front(key);
      it->second.second = keys_.begin();
    }
    LOGDEBUG(OSS() << "Cache hit !");
    ++hits_;
    return it->second.first;
  }

  /** Add a pair (key,value) to the cache. This may wipe out some older pair if maxSize is reached */
  inline
  void add(const KeyType & key,
           const ValueType & value)
  {
    // Do not allow the key to be added twice
    if (!enabled_ || (points_.find(key) != points_.end())) return;

    if (points_.size() == maxSize_)
    {
      // Remove the last element
      list_key_iterator last = keys_.end();
      --last;
      points_.erase(*last);
      keys_.erase(last);
    }

    // Insert the element at the beginning
    keys_.push_front(key);

    points_[key] = std::make_pair(value, keys_.begin());
  }


  /** Method save() stores the object through the StorageManager */
  inline
  void save(Advocate & adv) const
  {
    const UnsignedInteger size = points_.size();
    PersistentCollection< KeyType >   keyColl(size);
    PersistentCollection< ValueType > valueColl(size);
    UnsignedInteger index = 0;
    // Save entries in reverse order, so that they get loaded in the right order
    for(list_key_reverse_iterator it = keys_.rbegin(); it != keys_.rend(); ++it, ++index)
    {
      const KeyType & key = *it;
      keyColl[index] = key;
      valueColl[index] = points_[key].first;
    }
    PersistentObject::save(adv);
    adv.saveAttribute( "enabled_", enabled_ );
    adv.saveAttribute( "hits_", hits_ );
    adv.saveAttribute( "keyColl", keyColl );
    adv.saveAttribute( "valueColl", valueColl );
  }

  /** Method load() reloads the object from the StorageManager */
  inline
  void load(Advocate & adv)
  {
    clear();
    PersistentObject::load(adv);
    adv.loadAttribute( "enabled_", enabled_ );
    adv.loadAttribute( "hits_", hits_ );
    PersistentCollection< KeyType >      keyColl;
    PersistentCollection< ValueType >    valueColl;
    adv.loadAttribute( "keyColl", keyColl );
    adv.loadAttribute( "valueColl", valueColl );
    const UnsignedInteger size = keyColl.getSize();
    for (UnsignedInteger i = 0; i < size; ++i)
      add(keyColl[i], valueColl[i]);
  }


  /** Accessors */

  /** @brief return the size
   */
  inline UnsignedInteger getSize() const
  {
    return points_.size();
  }

  /** @brief return the maximum size
   */
  inline UnsignedInteger getMaxSize() const
  {
    return maxSize_;
  }

  /** @brief return the keys
   */
  inline PersistentCollection<KeyType> getKeys() const
  {
    PersistentCollection<KeyType> keysColl;
    if (enabled_)
    {
      for (typename map_type::iterator it = points_.begin(); it != points_.end(); ++ it)
      {
        keysColl.add(it->first);
      }
    }
    return keysColl;
  }

  /** @brief return the values
   */
  inline PersistentCollection<ValueType> getValues() const
  {
    PersistentCollection<ValueType> valuesColl;
    if (enabled_)
    {
      for (typename map_type::iterator it = points_.begin(); it != points_.end(); ++ it)
      {
        valuesColl.add(it->second.first);
      }
    }
    return valuesColl;
  }

  /** Enable or disable the cache */
  inline void enable()  const
  {
    enabled_ = true;
  }
  inline void disable() const
  {
    enabled_ = false;
  }
  inline Bool isEnabled() const
  {
    return enabled_;
  }

  /** Empty the cache */
  inline void clear()
  {
    points_.clear();
    keys_.clear();
    hits_ = 0;
  }

}; /* class Cache */




END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CACHE_HXX */
