//                                               -*- C++ -*-
/**
 *  @brief The class NumericalSampleImplementation implements blank free samples
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
#ifndef OPENTURNS_NUMERICALSAMPLEIMPLEMENTATION_HXX
#define OPENTURNS_NUMERICALSAMPLEIMPLEMENTATION_HXX

#include <stdint.h> // for uint64_t

#include "NumericalPoint.hxx"
#include "Description.hxx"
#include "Indices.hxx"
#include "PersistentCollection.hxx"
#include "SquareMatrix.hxx"
#include "CovarianceMatrix.hxx"
#include "CorrelationMatrix.hxx"
#include "SquareMatrix.hxx"
#include "Collection.hxx"
#include "ResourceMap.hxx"


BEGIN_NAMESPACE_OPENTURNS

#ifndef SWIG
class NumericalSampleImplementation;
class NSI_const_point;

/****************************************/


class OT_API NSI_point
{
  NumericalSampleImplementation * p_nsi_;
  UnsignedInteger index_;
  UnsignedInteger dimension_;

  friend class NSI_const_point;
  friend OT_API std::ostream & operator << (std::ostream & os, const NSI_point & point);

public:
  typedef       NumericalScalar *       iterator;
  typedef const NumericalScalar * const_iterator;

public:
  NSI_point(NumericalSampleImplementation * p_nsi,
            const UnsignedInteger index);

  NSI_point & operator = (const NSI_point & rhs);

  template <typename POINT>
  inline
  NSI_point & operator = (POINT rhs)
  {
    if ( getDimension() == rhs.getDimension() )
      std::copy( rhs.begin(), rhs.end(), begin() );
    return *this;
  }


  // NSI_point & operator = (const NSI_const_point & rhs);
  // NSI_point & operator = (const NumericalPoint & rhs);

  NumericalScalar & operator [] (const UnsignedInteger i);
  const NumericalScalar & operator [] (const UnsignedInteger i) const;
  NumericalScalar & at (const UnsignedInteger i);
  const NumericalScalar & at (const UnsignedInteger i) const;

  inline Collection<NumericalScalar> getCollection() const
  {
    if (dimension_ == 0) return Collection<NumericalScalar>(0);
    return Collection<NumericalScalar>( begin(), end() );
  }
  inline operator NumericalPoint () const
  {
    return getCollection();
  }

  inline UnsignedInteger getDimension() const
  {
    return dimension_;
  }

  inline iterator begin()
  {
    return &operator[](0);
  }
  inline iterator end()
  {
    return &operator[](0) + dimension_;
  }
  inline const_iterator begin() const
  {
    return &operator[](0);
  }
  inline const_iterator end() const
  {
    return &operator[](0) + dimension_;
  }

  NSI_point & operator += (const NSI_point & other);
  NSI_point & operator -= (const NSI_point & other);
  NSI_point & operator += (const NumericalPoint & other);
  NSI_point & operator -= (const NumericalPoint & other);
  NSI_point & operator *= (const NumericalScalar val);
  NSI_point & operator /= (const NumericalScalar val);
};

OT_API bool operator == (const NSI_point & lhs, const NSI_point & rhs);
OT_API bool operator != (const NSI_point & lhs, const NSI_point & rhs);
OT_API bool operator <  (const NSI_point & lhs, const NSI_point & rhs);
OT_API bool operator >  (const NSI_point & lhs, const NSI_point & rhs);
OT_API bool operator <= (const NSI_point & lhs, const NSI_point & rhs);
OT_API bool operator >= (const NSI_point & lhs, const NSI_point & rhs);

OT_API std::ostream & operator <<(std::ostream & os, const NSI_point & point);
OT_API OStream & operator << (OStream & OS, const NSI_point & point);


class OT_API NSI_const_point
{
  const NumericalSampleImplementation * p_nsi_;
  UnsignedInteger index_;
  UnsignedInteger dimension_;

  friend OT_API std::ostream & operator << (std::ostream & os, const NSI_const_point & point);

public:
  typedef       NumericalScalar *       iterator;
  typedef const NumericalScalar * const_iterator;

public:
  NSI_const_point(const NumericalSampleImplementation * p_nsi, UnsignedInteger index);
  NSI_const_point(const NSI_point & point);

  const NumericalScalar & operator [] (const UnsignedInteger i) const;
  const NumericalScalar & at (const UnsignedInteger i) const;

  inline Collection<NumericalScalar> getCollection() const
  {
    if (dimension_ == 0) return Collection<NumericalScalar>(0);
    return Collection<NumericalScalar>( begin(), end() );
  }
  inline operator NumericalPoint () const
  {
    return getCollection();
  }

  inline UnsignedInteger getDimension() const
  {
    return dimension_;
  }

  inline const_iterator begin() const
  {
    return &operator[](0);
  }
  inline const_iterator end() const
  {
    return &operator[](0) + dimension_;
  }
};

OT_API bool operator == (const NSI_const_point & lhs, const NSI_const_point & rhs);
OT_API bool operator != (const NSI_const_point & lhs, const NSI_const_point & rhs);
OT_API bool operator <  (const NSI_const_point & lhs, const NSI_const_point & rhs);
OT_API bool operator >  (const NSI_const_point & lhs, const NSI_const_point & rhs);
OT_API bool operator <= (const NSI_const_point & lhs, const NSI_const_point & rhs);
OT_API bool operator >= (const NSI_const_point & lhs, const NSI_const_point & rhs);

OT_API std::ostream & operator <<(std::ostream & os, const NSI_const_point & point);
OT_API OStream & operator << (OStream & OS, const NSI_const_point & point);


inline
NumericalPoint operator * (const NSI_point & point,
                           const NumericalScalar val)
{
  NumericalPoint res(point.getDimension(), 0.);
  for(UnsignedInteger i = 0; i < point.getDimension(); ++i) res[i] = point[i] * val;
  return res;
}

inline
NumericalPoint operator * (const NumericalScalar val,
                           const NSI_point & point)
{
  return point * val;
}

inline
NumericalPoint operator * (const NSI_const_point & point,
                           const NumericalScalar val)
{
  NumericalPoint res(point.getDimension(), 0.);
  for(UnsignedInteger i = 0; i < point.getDimension(); ++i) res[i] = point[i] * val;
  return res;
}

inline
NumericalPoint operator * (const NumericalScalar val,
                           const NSI_const_point & point)
{
  return point * val;
}




/****************************************/


class OT_API NSI_iterator
{
  NumericalSampleImplementation * p_nsi_;
  UnsignedInteger current_;

public:
  typedef size_t difference_type;
  typedef std::random_access_iterator_tag iterator_category;
  typedef NSI_point value_type;
  typedef NSI_point * pointer;
  typedef NSI_point   reference;

  friend class NSI_const_iterator;
  friend bool operator == (const NSI_iterator & lhs, const NSI_iterator & rhs);
  friend bool operator <  (const NSI_iterator & lhs, const NSI_iterator & rhs);
  friend bool operator >  (const NSI_iterator & lhs, const NSI_iterator & rhs);
  friend difference_type operator - (const NSI_iterator & lhs, const NSI_iterator & rhs);

public:
  NSI_iterator() : p_nsi_(0), current_(0) {}
  NSI_iterator(NumericalSampleImplementation & nsi, const UnsignedInteger index) : p_nsi_(&nsi), current_(index) {}

  inline NSI_iterator & operator ++ ()
  {
    ++current_;
    return *this;
  }
  inline NSI_iterator & operator -- ()
  {
    --current_;
    return *this;
  }
  inline NSI_iterator   operator ++ (int)
  {
    NSI_iterator old(*this);
    ++current_;
    return old;
  }
  inline NSI_iterator   operator -- (int)
  {
    NSI_iterator old(*this);
    --current_;
    return old;
  }

  inline NSI_iterator & operator += (difference_type n)
  {
    current_ += n;
    return *this;
  }
  inline NSI_iterator & operator -= (difference_type n)
  {
    current_ -= n;
    return *this;
  }
  inline NSI_iterator   operator +  (difference_type n) const
  {
    NSI_iterator old(*this);
    old.current_ += n;
    return old;
  }
  inline NSI_iterator   operator -  (difference_type n) const
  {
    NSI_iterator old(*this);
    old.current_ -= n;
    return old;
  }

  inline NSI_point operator *  () const
  {
    return NSI_point(p_nsi_, current_);
  }
};

inline
bool operator == (const NSI_iterator & lhs, const NSI_iterator & rhs)
{
  return (lhs.p_nsi_ == rhs.p_nsi_) && (lhs.current_ == rhs.current_);
}

inline
bool operator != (const NSI_iterator & lhs, const NSI_iterator & rhs)
{
  return ! (lhs == rhs);
}

inline
bool operator < (const NSI_iterator & lhs, const NSI_iterator & rhs)
{
  return (lhs.p_nsi_ == rhs.p_nsi_) && (lhs.current_ < rhs.current_);
}

inline
bool operator > (const NSI_iterator & lhs, const NSI_iterator & rhs)
{
  return (lhs.p_nsi_ == rhs.p_nsi_) && (lhs.current_ > rhs.current_);
}

inline
NSI_iterator::difference_type operator - (const NSI_iterator & lhs, const NSI_iterator & rhs)
{
  return (lhs.current_ - rhs.current_);
}




class OT_API NSI_const_iterator
{
  const NumericalSampleImplementation * p_nsi_;
  UnsignedInteger current_;

public:
  typedef size_t difference_type;
  typedef std::random_access_iterator_tag iterator_category;
  typedef NSI_const_point value_type;
  typedef NSI_const_point * pointer;
  typedef NSI_const_point   reference;

  friend bool operator == (const NSI_const_iterator & lhs, const NSI_const_iterator & rhs);
  friend bool operator <  (const NSI_const_iterator & lhs, const NSI_const_iterator & rhs);
  friend bool operator >  (const NSI_const_iterator & lhs, const NSI_const_iterator & rhs);
  friend difference_type operator - (const NSI_const_iterator & lhs, const NSI_const_iterator & rhs);

public:
  NSI_const_iterator() : p_nsi_(0), current_(0) {}
  NSI_const_iterator(const NumericalSampleImplementation & nsi, const UnsignedInteger index) : p_nsi_(&nsi), current_(index) {}
  NSI_const_iterator(const NSI_iterator & iter) : p_nsi_(iter.p_nsi_), current_(iter.current_) {}

  inline NSI_const_iterator & operator ++ ()
  {
    ++current_;
    return *this;
  }
  inline NSI_const_iterator & operator -- ()
  {
    --current_;
    return *this;
  }
  inline NSI_const_iterator   operator ++ (int)
  {
    NSI_const_iterator old(*this);
    ++current_;
    return old;
  }
  inline NSI_const_iterator   operator -- (int)
  {
    NSI_const_iterator old(*this);
    --current_;
    return old;
  }

  inline NSI_const_iterator & operator += (difference_type n)
  {
    current_ += n;
    return *this;
  }
  inline NSI_const_iterator & operator -= (difference_type n)
  {
    current_ -= n;
    return *this;
  }
  inline NSI_const_iterator   operator +  (difference_type n) const
  {
    NSI_const_iterator old(*this);
    old.current_ += n;
    return old;
  }
  inline NSI_const_iterator   operator -  (difference_type n) const
  {
    NSI_const_iterator old(*this);
    old.current_ -= n;
    return old;
  }

  inline NSI_const_point operator *  () const
  {
    return NSI_const_point(p_nsi_, current_);
  }
};

inline
bool operator == (const NSI_const_iterator & lhs, const NSI_const_iterator & rhs)
{
  return (lhs.p_nsi_ == rhs.p_nsi_) && (lhs.current_ == rhs.current_);
}

inline
bool operator != (const NSI_const_iterator & lhs, const NSI_const_iterator & rhs)
{
  return ! (lhs == rhs);
}

inline
bool operator < (const NSI_const_iterator & lhs, const NSI_const_iterator & rhs)
{
  return (lhs.p_nsi_ == rhs.p_nsi_) && (lhs.current_ < rhs.current_);
}

inline
bool operator > (const NSI_const_iterator & lhs, const NSI_const_iterator & rhs)
{
  return (lhs.p_nsi_ == rhs.p_nsi_) && (lhs.current_ > rhs.current_);
}

inline
NSI_const_iterator::difference_type operator - (const NSI_const_iterator & lhs, const NSI_const_iterator & rhs)
{
  return (lhs.current_ - rhs.current_);
}

#endif


/****************************************/



/**
 * @class NumericalSampleImplementation
 */

class OT_API NumericalSampleImplementation
  : public PersistentObject
{
  CLASSNAME;
  friend class NSI_point;
  friend class NSI_const_point;
  friend class NSI_iterator;
  friend class NSI_const_iterator;
  friend class Factory<NumericalSampleImplementation>;
  friend class BuildMethodMap;
  friend class ExportMethodMap;
#ifndef SWIG
  friend OT_API Bool operator ==(const NumericalSampleImplementation & lhs, const NumericalSampleImplementation & rhs);
#endif

public:

  /* Some typedefs for easy reading */
  typedef NSI_iterator               iterator;
  typedef NSI_const_iterator         const_iterator;

  typedef Collection<UnsignedInteger>   UnsignedIntegerCollection;

  enum ExternalFileFormat { CSV };

public:

  /** Factory of NumericalSampleImplementation from CSV file */
  static NumericalSampleImplementation BuildFromCSVFile(const FileName & fileName,
      const String & csvSeparator = ResourceMap::Get( "csv-file-separator" ));

  /** Factory of NumericalSampleImplementation from Text file */
  static NumericalSampleImplementation BuildFromTextFile(const FileName & fileName,
      const String & separator = " ");

  /** Store a sample in a temporary text file, one realization by line. Returns the file name. */
  String storeToTemporaryFile() const;

  /** Export a sample as a matrix, one row by realization, in a format suitable to exchange with R */
  String streamToRFormat() const;

protected:

  /**
   * Default constructor is protected
   */
  NumericalSampleImplementation();

public:

  /** Standard constructor */
  NumericalSampleImplementation(const UnsignedInteger size,
                                const UnsignedInteger dim);

  /** Constructor from a NumericalPoint */
  NumericalSampleImplementation(const UnsignedInteger size,
                                const NumericalPoint & point);

#ifndef SWIG
  /** Constructor from a collection of NumericalPoint */
  NumericalSampleImplementation(const Collection<NumericalPoint> & coll);

  /** Constructor from a collection of Indices */
  NumericalSampleImplementation(const Collection<Indices> & coll);

  /** Partial copy constructor */
  NumericalSampleImplementation(const NumericalSampleImplementation & other,
                                iterator first,
                                iterator last);

  inline iterator begin()
  {
    return iterator(*this, 0);
  }
  inline iterator end()
  {
    return iterator(*this, size_);
  }
  inline const_iterator begin() const
  {
    return const_iterator(*this, 0);
  }
  inline const_iterator end() const
  {
    return const_iterator(*this, size_);
  }

  void erase(const UnsignedInteger first, const UnsignedInteger last);
  void erase(iterator first, iterator last);
  void clear();

  /** Raw internal format accessor */
  NumericalPoint getData() const;
  void setData(const Collection<NumericalScalar> & data);

  inline NSI_point operator [] (const UnsignedInteger index)
  {
    return NSI_point(this, index);
  }
  inline NSI_const_point operator [] (const UnsignedInteger index) const
  {
    return NSI_const_point(this, index);
  }

  void swap_points(const UnsignedInteger a, const UnsignedInteger b);
  void swap_range_points(const UnsignedInteger fa, const UnsignedInteger ta, const UnsignedInteger fb);
#endif
  // These functions are only intended to be used by SWIG, DO NOT use them for your own purpose !
  // INTENTIONALY NOT DOCUMENTED
  const NumericalScalar * __baseaddress__ () const
  {
    return &data_[0];
  }
  UnsignedInteger __elementsize__ () const
  {
    return sizeof( NumericalScalar );
  }

  /** Virtual constructor */
  virtual NumericalSampleImplementation * clone() const;

  /** Description Accessor */
  void setDescription(const Description & description);
  Description getDescription() const;

  /**
   * String converter
   * This method shows human readable information on the
   * internal state of an NumericalSampleImplementation. It is used when streaming
   * the NumericalSampleImplementation or for user information.
   */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  inline Bool __eq__(const NumericalSampleImplementation & rhs) const
  {
    return (*this == rhs);
  }

  /* Method contains() is for Python */
  Bool contains(const NumericalPoint & val) const;

  /** Size accessor */
  inline UnsignedInteger getSize() const
  {
    return size_;
  }

  /** Dimension accessor */
  inline UnsignedInteger getDimension() const
  {
    return dimension_;
  }

  /** Appends an element to the collection */
  NumericalSampleImplementation & add(const NumericalPoint & point);

  /** Appends another sample to the collection */
  NumericalSampleImplementation & add(const NumericalSampleImplementation & sample);

  /** Stack a sample to the current one */
  NumericalSampleImplementation & stack(const NumericalSampleImplementation & sample);

  /**
   * Gives the mean of the sample, based on the formula
   * mean = sum of the elements in the sample / size of the sample
   */
  virtual NumericalPoint computeMean() const;

  /**
   * Gives the covariance matrix of the sample, normalization by 1 / (size - 1) if size > 1
   */
  virtual CovarianceMatrix computeCovariance() const;

  /**
   * Gives the standard deviation of the sample, i.e. the square-root of the covariance matrix.
   */
  TriangularMatrix computeStandardDeviation() const;

  /**
   * Gives the standard deviation of each component of the sample
   */
  virtual NumericalPoint computeStandardDeviationPerComponent() const;

  /**
   * Gives the Pearson correlation matrix of the sample
   */
  virtual CorrelationMatrix computeLinearCorrelation() const;
  virtual CorrelationMatrix computePearsonCorrelation() const;

  /**
   * Gives the Spearman correlation matrix of the sample
   */
  virtual CorrelationMatrix computeSpearmanCorrelation() const;

  /**
   * Gives the Kendall correlation matrix of the sample
   */
  virtual CorrelationMatrix computeKendallTau() const;

  /**
   * Gives the range of the sample (by component)
   */
  NumericalPoint computeRange() const;

  /**
   * Gives the median of the sample (by component)
   */
  NumericalPoint computeMedian() const;

  /**
   * Gives the variance of the sample (by component)
   */
  virtual NumericalPoint computeVariance() const;

  /**
   * Gives the skewness of the sample (by component)
   */
  virtual NumericalPoint computeSkewness() const;

  /**
   * Gives the kurtosis of the sample (by component)
   */
  virtual NumericalPoint computeKurtosis() const;

  /**
   * Gives the centered moment of order k of the sample (by component)
   */
  NumericalPoint computeCenteredMoment(const UnsignedInteger k) const;

  /**
   * Gives the raw moment of order k of the sample (by component)
   */
  NumericalPoint computeRawMoment(const UnsignedInteger k) const;

  /**
   * Gives the quantile per component of the sample
   */
  virtual NumericalPoint computeQuantilePerComponent(const NumericalScalar prob) const;

  /**
   * Gives the N-dimension quantile of the sample
   */
  NumericalPoint computeQuantile(const NumericalScalar prob) const;

  /**
   * Get the empirical CDF of the sample
   */
  NumericalScalar computeEmpiricalCDF(const NumericalPoint & point,
                                      const Bool tail = false) const;

  /** Maximum accessor */
  virtual NumericalPoint getMax() const;

  /** Minimum accessor */
  virtual NumericalPoint getMin() const;

  /** Ranked sample */
  NumericalSampleImplementation rank() const;

  /** Ranked component */
  NumericalSampleImplementation rank(const UnsignedInteger index) const;

  /** Sorted sample */
  NumericalSampleImplementation sort() const;

  /** Sorted component */
  NumericalSampleImplementation sort(const UnsignedInteger index) const;

  /** Sorted component */
  NumericalSampleImplementation sortAccordingToAComponent(const UnsignedInteger index) const;

  /** Get the i-th marginal sample */
  NumericalSampleImplementation getMarginal(const UnsignedInteger index) const;

  /** Get the marginal sample corresponding to indices dimensions */
  NumericalSampleImplementation getMarginal(const Indices & indices) const;

  /**
   * Translate realizations in-place
   */
  NumericalSampleImplementation & operator += (const NumericalScalar translation);
  NumericalSampleImplementation & operator += (const NumericalPoint & translation);
  NumericalSampleImplementation & operator += (const NumericalSampleImplementation & translation);
  NumericalSampleImplementation & operator -= (const NumericalScalar translation);
  NumericalSampleImplementation & operator -= (const NumericalPoint & translation);
  NumericalSampleImplementation & operator -= (const NumericalSampleImplementation & translation);

  /** Translate realizations */
  NumericalSampleImplementation operator + (const NumericalScalar translation) const;
  NumericalSampleImplementation operator + (const NumericalPoint & translation) const;
  NumericalSampleImplementation operator + (const NumericalSampleImplementation & translation) const;
  NumericalSampleImplementation operator - (const NumericalScalar translation) const;
  NumericalSampleImplementation operator - (const NumericalPoint & translation) const;
  NumericalSampleImplementation operator - (const NumericalSampleImplementation & translation) const;

  NumericalSampleImplementation operator * (const NumericalScalar scaling) const;
  NumericalSampleImplementation operator * (const NumericalPoint & scaling) const;
  NumericalSampleImplementation operator * (const SquareMatrix & scaling) const;
  NumericalSampleImplementation operator / (const NumericalScalar scaling) const;
  NumericalSampleImplementation operator / (const NumericalPoint & scaling) const;
  NumericalSampleImplementation operator / (const SquareMatrix & scaling) const;

  /**
   * Scale realizations componentwise in-place
   */

  NumericalSampleImplementation & operator *= (const NumericalScalar scaling);
  NumericalSampleImplementation & operator *= (const NumericalPoint & scaling);
  NumericalSampleImplementation & operator *= (const SquareMatrix & scaling);
  NumericalSampleImplementation & operator /= (const NumericalScalar scaling);
  NumericalSampleImplementation & operator /= (const NumericalPoint & scaling);
  NumericalSampleImplementation & operator /= (const SquareMatrix & scaling);

  /** Save to CSV file */
  void exportToCSVFile(const FileName & filename,
                       const String & csvSeparator = ResourceMap::Get( "csv-file-separator" )) const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);


private:
  void translate(const NumericalPoint & translation);
  void scale(const NumericalPoint & scaling);
  void scale(const SquareMatrix & scaling);

  /** The size of the sample */
  UnsignedInteger size_;

  /** The dimension of the sample */
  UnsignedInteger dimension_;

  /** The collection of unwrapped points */
  PersistentCollection<NumericalScalar> data_;

  /** The description of all components */
  Pointer<Description> p_description_;

}; /* class NumericalSampleImplementation */


/* Comparison function */
OT_API Bool operator ==(const NumericalSampleImplementation & lhs, const NumericalSampleImplementation & rhs);


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NUMERICALSAMPLEIMPLEMENTATION_HXX */
