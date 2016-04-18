//                                               -*- C++ -*-
/**
 *  @brief Cache holds the already computed points to speed up calculations
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
#ifndef OPENTURNS_CACHE_HXX
#define OPENTURNS_CACHE_HXX

#include <map>
#include <sstream>
#include "openturns/PersistentObject.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/Collection.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Log.hxx"
#include "openturns/OStream.hxx"

BEGIN_NAMESPACE_OPENTURNS


template <typename K_, typename V_, typename U_>
inline static
std::ostream & operator << ( std::ostream & os, const std::pair< K_, std::pair< V_, U_ > > & val)
{
  os << val.first << "->" << val.second.first << "/" << val.second.second;
  return os;
}

template <typename K_, typename V_, typename U_>
inline static
OStream & operator << ( OStream & OS, const std::pair< K_, std::pair< V_, U_ > > & val)
{
  OS << val.first << "->" << val.second.first << "/" << val.second.second;
  return OS;
}


/**
 * @class Cache
 *
 * Cache holds the already computed points to speed up calculations
 * Like any cache system, this objet saves time by keeping some costly data
 * and provides access to them through a key.
 * Here the key is the input NumericalPoint of a NumericalMathFunction and the
 * value is the output NumericalPoint that was computed.
 * When used with ComputedNumericalMathFunctionImplementation objects the Cache
 * may greatly speed up the computations of Analytical algorithms (like FORM
 * or SORM). Its advantage is more doubtful with other NumericalMathFunctionImplementations
 * or Simulation algorithms.
 *
 * The Cache may be set with a maximum size to avoid too much memory consumption especially
 * when running Simulation algorithms (Monte-Carlo or LHS). This maximum size is
 * an upper bound for the number of data kept by the Cache. When this upper bound is
 * reached, the next data insertion into the Cache flushes the least recently used
 * data before the insertion.
 */

template <typename K_, typename V_>
class Cache
  : public PersistentObject
{
public:
  static  OT::String GetClassName()
  {
    return OT::String("Cache<") + OT::String(K_::GetClassName()) + OT::String(", ") + OT::String(V_::GetClassName()) + OT::String(">");
  }
  virtual OT::String getClassName() const
  {
    return Cache<K_, V_>::GetClassName();
  }

public:

  typedef K_                              KeyType;
  typedef std::pair< V_ , UnsignedInteger >  ValueType;
  typedef std::pair< KeyType, ValueType > PairType;
#ifndef SWIG
  struct ConvertMapToCollections
  {
    typedef typename std::map< KeyType, ValueType >::iterator::value_type value_type;
    typedef typename std::map< KeyType, ValueType >::iterator::difference_type difference_type;
    typedef typename std::map< KeyType, ValueType >::iterator::pointer pointer;
    typedef typename std::map< KeyType, ValueType >::iterator::reference reference;
    typedef std::output_iterator_tag                 iterator_category;

    PersistentCollection< KeyType >      & keyColl_;
    PersistentCollection< KeyType >      & valueColl_;
    PersistentCollection< UnsignedInteger > & ageColl_;
    UnsignedInteger i_;

    ConvertMapToCollections(PersistentCollection< KeyType > & keyColl,
                            PersistentCollection< KeyType > & valueColl,
                            PersistentCollection< UnsignedInteger > & ageColl)
      : keyColl_(keyColl), valueColl_(valueColl), ageColl_(ageColl), i_(0) {}

    ConvertMapToCollections &
    operator = (const typename std::map< KeyType, ValueType >::value_type & val)
    {
      keyColl_  [i_] = val.first;
      valueColl_[i_] = val.second.first;
      ageColl_  [i_] = val.second.second;
      ++i_;
      return *this;
    }

    ConvertMapToCollections &
    operator = (const ConvertMapToCollections & other)
    {
      keyColl_   = other.keyColl_;
      valueColl_ = other.valueColl_;
      ageColl_   = other.ageColl_;
      i_         = other.i_;
      return *this;
    }

    ConvertMapToCollections & operator *  ()
    {
      return *this;
    }
    ConvertMapToCollections & operator ++ ()
    {
      return *this;
    }
    ConvertMapToCollections & operator ++ (int)
    {
      return *this;
    }
  };
  // #else
  //      typedef ConvertMapToCollections;
#endif
protected:

  /** True if cache is enabled */
  mutable Bool enabled_;

  /** Upper bound for the cache size */
  const UnsignedInteger maxSize_;

  /** Number of hits */
  mutable UnsignedInteger hits_;

  /** The map of elements */
  mutable std::map< KeyType, ValueType > points_;

public:

  /** Default constructor */
  inline
  Cache() : PersistentObject(),
    enabled_(true),
    maxSize_(ResourceMap::GetAsUnsignedInteger("cache-max-size")),
    hits_(0),
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
    points_()
  {
    // Nothing to do
  }

#ifdef SWIG
  Cache(const Cache & other ) : PersistentObject(other),
    enabled_(other.enabled_),
    maxSize_(other.maxSize_),
    hits_(other.hits_),
    points_(other.hits_)
  {
    //Nothing to do
  }

#endif

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
        << " enabled=" << this->enabled_
        << " name=" << getName()
        << " maxSize=" << this->maxSize_
        << " size=" << getSize()
        << " hits=" << getHits()
        << " points={" ;
    copy( this->points_.begin(), this->points_.end(), OSS_iterator<PairType>( oss, ", " ) );
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
      PersistentObject::operator=(other);
      const_cast<UnsignedInteger&>(this->maxSize_)        = other.maxSize_;
      this->points_                                    = other.points_;
      this->enabled_                                   = other.enabled_;
      this->hits_                                      = other.hits_;
    }

    return *this;
  }

#endif

  /** Merge the contents of two caches */
  inline
  Cache & merge (const Cache & other)
  {
    if (isEnabled())
    {
      for_each( other.points_.begin(), other.points_.end(), addFunctor( this ) );
    }
    return *this;
  }

  /** Returns the number of successful hits in the cache */
  inline UnsignedInteger getHits() const
  {
    return this->hits_;
  }

  /** Query the cache for the key presence */
  inline
  Bool hasKey(const K_ & key) const
  {
    if (! isEnabled() ) return false;
    Bool found = ( this->points_.find( key ) != this->points_.end() );
    return found;
  }

  /** Retrieve the value from the cache with the key */
  inline
  const V_ find(const K_ & key) const
  {
    if (isEnabled())
    {
      typename std::map< KeyType, ValueType >::iterator it = this->points_.find( key );
      Bool found = ( it != this->points_.end() );
      if (found)
      {
        ++(*it).second.second; // increment age
        ++hits_;
        LOGINFO(OSS() << "Cache hit !");
        return V_( (*it).second.first );

      }
      else
        return V_();
    }
    else
      return V_();
  }

  /** Add a pair (key,value) to the cache. This may wipe out some older pair if maxSize is reached */
  inline
  void add(const K_ & key,
           const V_ & value)
  {
    if (isEnabled()) insert( key, ValueType( value, 0 ) );
  }


  /** Method save() stores the object through the StorageManager */
  inline
  void save(Advocate & adv) const
  {
    const UnsignedInteger size = this->points_.size();
    PersistentCollection< KeyType >      keyColl(size);
    PersistentCollection< KeyType >      valueColl(size);
    PersistentCollection< UnsignedInteger > ageColl(size);
    std::copy(this->points_.begin(),
              this->points_.end(),
              ConvertMapToCollections(keyColl, valueColl, ageColl));

    PersistentObject::save(adv);
    adv.saveAttribute( "size", size );
    adv.saveAttribute( "keyColl", keyColl );
    adv.saveAttribute( "valueColl", valueColl );
    adv.saveAttribute( "ageColl", ageColl );
  }

  /** Method load() reloads the object from the StorageManager */
  inline
  void load(Advocate & adv)
  {
    PersistentObject::load(adv);
    UnsignedInteger size;
    adv.loadAttribute( "size", size );

    PersistentCollection< KeyType >      keyColl(size);
    PersistentCollection< KeyType >      valueColl(size);
    PersistentCollection< UnsignedInteger > ageColl(size);
    adv.loadAttribute( "keyColl", keyColl );
    adv.loadAttribute( "valueColl", valueColl );
    adv.loadAttribute( "ageColl", ageColl );

    clear();
    for( UnsignedInteger i = 0; i < size; ++i) this->points_[ keyColl[i] ] = ValueType( valueColl[i], ageColl[i] );
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
    return this->maxSize_;
  }

  /** @brief return the keys
   */
  inline PersistentCollection<KeyType> getKeys() const
  {
    PersistentCollection<KeyType> keyColl;
    if ( isEnabled() )
    {
      for( typename std::map< KeyType, ValueType >::iterator it = points_.begin(); it != points_.end(); ++ it )
      {
        keyColl.add( it->first );
      }
    }
    return keyColl;
  }

  /** @brief return the values
   */
  inline PersistentCollection<V_> getValues() const
  {
    PersistentCollection<V_> valuesColl;
    if ( isEnabled() )
    {
      for( typename std::map< KeyType, ValueType >::iterator it = points_.begin(); it != points_.end(); ++ it )
      {
        valuesColl.add( it->second.first );
      }
    }
    return valuesColl;
  }

  /** Enable or disable the cache */
  inline void enable()  const
  {
    this->enabled_ = true;
  }
  inline void disable() const
  {
    this->enabled_ = false;
  }
  inline Bool isEnabled() const
  {
    return this->enabled_;
  }

  /** Empty the cache */
  inline void clear()
  {
    points_.clear();
    hits_ = 0;
  }

private:

  /* Used in sort algorithm to find the Least Recently Used item.
   * This structure implements the BinaryPredicate concept of the STL.
   */
  struct OrderAccordingToAges
  {
    inline
    bool operator() (const PairType & a,
                     const PairType & b)
    {
      return a.second.second < b.second.second ;
    }
  };

  /* Used to insert elements into the cache */
  struct addFunctor : public std::unary_function< Cache, void >
  {
    Cache * p_cache_;
    inline addFunctor( Cache * p_cache ) : p_cache_(p_cache) {}
    inline void operator() ( const typename std::map< typename Cache::KeyType, typename Cache::ValueType >::value_type & val )
    {
      p_cache_->insert( val.first, val.second );
    }
  };


  /* Insert a (key,value) pair in the cache */
  inline
  void insert( const KeyType & key, const ValueType & value )
  {
    if (this->points_.size() == maxSize_)
    {
      typename std::map< KeyType, ValueType >::iterator it = min_element( this->points_.begin(), this->points_.end(), OrderAccordingToAges() );
      if (it != this->points_.end() ) this->points_.erase( it );
    }
    this->points_[key] = value;
  }

}; /* class Cache */




END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CACHE_HXX */
