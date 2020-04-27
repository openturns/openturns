//                                               -*- C++ -*-
/**
 *  @brief The class SampleImplementation implements blank free samples
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
#ifndef OPENTURNS_SAMPLEIMPLEMENTATION_HXX
#define OPENTURNS_SAMPLEIMPLEMENTATION_HXX

#include <stdint.h> // for uint64_t

#include "openturns/Point.hxx"
#include "openturns/Description.hxx"
#include "openturns/Indices.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/CorrelationMatrix.hxx"
#include "openturns/Collection.hxx"
#include "openturns/ResourceMap.hxx"


BEGIN_NAMESPACE_OPENTURNS

#ifndef SWIG
class SampleImplementation;
class NSI_const_point;

/****************************************/


class OT_API NSI_point
{
  SampleImplementation * p_nsi_;
  UnsignedInteger index_;
  UnsignedInteger dimension_;

  friend class NSI_const_point;
  friend OT_API std::ostream & operator << (std::ostream & os, const NSI_point & point);

public:
  typedef       Scalar *       iterator;
  typedef const Scalar * const_iterator;

public:
  NSI_point(SampleImplementation * p_nsi,
            const UnsignedInteger index);
  NSI_point(const NSI_point & other);
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
  // NSI_point & operator = (const Point & rhs);

  Scalar & operator [] (const UnsignedInteger i);
  const Scalar & operator [] (const UnsignedInteger i) const;
  Scalar & at (const UnsignedInteger i);
  const Scalar & at (const UnsignedInteger i) const;

  inline Collection<Scalar> getCollection() const
  {
    if (dimension_ == 0) return Collection<Scalar>(0);
    return Collection<Scalar>( begin(), end() );
  }
  inline operator Point () const
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
  NSI_point & operator += (const Point & other);
  NSI_point & operator -= (const Point & other);
  NSI_point & operator *= (const Scalar val);
  NSI_point & operator /= (const Scalar val);
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
  const SampleImplementation * p_nsi_;
  UnsignedInteger index_;
  UnsignedInteger dimension_;

  friend OT_API std::ostream & operator << (std::ostream & os, const NSI_const_point & point);

public:
  typedef       Scalar *       iterator;
  typedef const Scalar * const_iterator;

public:
  NSI_const_point(const SampleImplementation * p_nsi, UnsignedInteger index);
  NSI_const_point(const NSI_point & point);

  const Scalar & operator [] (const UnsignedInteger i) const;
  const Scalar & at (const UnsignedInteger i) const;

  inline Collection<Scalar> getCollection() const
  {
    if (dimension_ == 0) return Collection<Scalar>(0);
    return Collection<Scalar>( begin(), end() );
  }
  inline operator Point () const
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
Point operator * (const NSI_point & point,
                  const Scalar val)
{
  Point res(point.getDimension(), 0.);
  for(UnsignedInteger i = 0; i < point.getDimension(); ++i) res[i] = point[i] * val;
  return res;
}

inline
Point operator * (const Scalar val,
                  const NSI_point & point)
{
  return point * val;
}

inline
Point operator * (const NSI_const_point & point,
                  const Scalar val)
{
  Point res(point.getDimension(), 0.);
  for(UnsignedInteger i = 0; i < point.getDimension(); ++i) res[i] = point[i] * val;
  return res;
}

inline
Point operator * (const Scalar val,
                  const NSI_const_point & point)
{
  return point * val;
}




/****************************************/


class OT_API NSI_iterator
{
  SampleImplementation * p_nsi_;
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
  NSI_iterator(SampleImplementation & nsi, const UnsignedInteger index) : p_nsi_(&nsi), current_(index) {}

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
  const SampleImplementation * p_nsi_;
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
  NSI_const_iterator(const SampleImplementation & nsi, const UnsignedInteger index) : p_nsi_(&nsi), current_(index) {}
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
 * @class SampleImplementation
 */

class OT_API SampleImplementation
  : public PersistentObject
{
  CLASSNAME
  friend class NSI_point;
  friend class NSI_const_point;
  friend class NSI_iterator;
  friend class NSI_const_iterator;
  friend class Factory<SampleImplementation>;
  friend class BuildMethodMap;
  friend class ExportMethodMap;
#ifndef SWIG
  friend OT_API Bool operator ==(const SampleImplementation & lhs, const SampleImplementation & rhs);
#endif

public:

  /* Some typedefs for easy reading */
  typedef NSI_iterator               iterator;
  typedef NSI_const_iterator         const_iterator;
  typedef Collection<Scalar>::iterator       data_iterator;
  typedef Collection<Scalar>::const_iterator data_const_iterator;

  typedef Collection<UnsignedInteger>   UnsignedIntegerCollection;

  enum ExternalFileFormat { CSV };

public:

  /** Factory of SampleImplementation from CSV file */
  static SampleImplementation BuildFromCSVFile(const FileName & fileName,
      const String & csvSeparator = ResourceMap::GetAsString( "csv-file-separator" ));

  /** Factory of SampleImplementation from Text file */
  static SampleImplementation BuildFromTextFile(const FileName & fileName,
      const String & separator = " ",
      const UnsignedInteger skippedLines = 0);

  /** Store a sample in a temporary text file, one realization by line. Returns the file name. */
  String storeToTemporaryFile() const;

  /** Export a sample as a matrix, one row by realization, in a format suitable to exchange with R */
  String streamToRFormat() const;

protected:

  /**
   * Default constructor is protected
   */
  SampleImplementation();

public:

  /** Standard constructor */
  SampleImplementation(const UnsignedInteger size,
                       const UnsignedInteger dim);

  /** Constructor from a Point */
  SampleImplementation(const UnsignedInteger size,
                       const Point & point);

#ifndef SWIG
  /** Constructor from a collection of Point */
  SampleImplementation(const Collection<Point> & coll);

  /** Constructor from a collection of Indices */
  SampleImplementation(const Collection<Indices> & coll);

  /** Partial copy constructor */
  SampleImplementation(const SampleImplementation & other,
                       const_iterator first,
                       const_iterator last);

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

  inline data_iterator data_begin()
  {
    return data_.begin();
  }
  inline data_iterator data_end()
  {
    return data_.end();
  }
  inline data_const_iterator data_begin() const
  {
    return data_.begin();
  }
  inline data_const_iterator data_end() const
  {
    return data_.end();
  }

  void erase(const UnsignedInteger first, const UnsignedInteger last);
  void erase(iterator first, iterator last);
  void clear();

  /** Raw internal format accessor */
  Point getData() const;
  void setData(const Collection<Scalar> & data);

  inline NSI_point operator [] (const UnsignedInteger index)
  {
    return NSI_point(this, index);
  }
  inline NSI_const_point operator [] (const UnsignedInteger index) const
  {
    return NSI_const_point(this, index);
  }

  inline Scalar & operator () (const UnsignedInteger i,
                               const UnsignedInteger j)
  {
    return data_[j + i * dimension_];
  }
  inline const Scalar & operator () (const UnsignedInteger i,
                                     const UnsignedInteger j) const
  {
    return data_[j + i * dimension_];
  }

  void swap_points(const UnsignedInteger a, const UnsignedInteger b);
  void swap_range_points(const UnsignedInteger fa, const UnsignedInteger ta, const UnsignedInteger fb);
#endif
  // These functions are only intended to be used by SWIG, DO NOT use them for your own purpose !
  // INTENTIONALY NOT DOCUMENTED
  const Scalar * __baseaddress__ () const
  {
    return (data_.getSize() > 0) ? &data_[0] : 0;
  }
  UnsignedInteger __elementsize__ () const
  {
    return sizeof(Scalar);
  }

  /** Virtual constructor */
  virtual SampleImplementation * clone() const;

  /** Description Accessor */
  void setDescription(const Description & description);
  Description getDescription() const;

  /**
   * String converter
   * This method shows human readable information on the
   * internal state of an SampleImplementation. It is used when streaming
   * the SampleImplementation or for user information.
   */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  inline Bool __eq__(const SampleImplementation & rhs) const
  {
    return (*this == rhs);
  }

  /** Whether the list contains the value val */
  Bool contains(const Point & val) const;

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
  SampleImplementation & add(const Point & point);

  /** Appends another sample to the collection */
  SampleImplementation & add(const SampleImplementation & sample);

  /** Stack a sample to the current one */
  SampleImplementation & stack(const SampleImplementation & sample);

  /**
   * Gives the mean of the sample, based on the formula
   * mean = sum of the elements in the sample / size of the sample
   */
  virtual Point computeMean() const;

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
  virtual Point computeStandardDeviationPerComponent() const;

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
  Point computeRange() const;

  /**
   * Gives the median of the sample (by component)
   */
  Point computeMedian() const;

  /**
   * Gives the variance of the sample (by component)
   */
  virtual Point computeVariance() const;

  /**
   * Gives the skewness of the sample (by component)
   */
  virtual Point computeSkewness() const;

  /**
   * Gives the kurtosis of the sample (by component)
   */
  virtual Point computeKurtosis() const;

  /**
   * Gives the centered moment of order k of the sample (by component)
   */
  Point computeCenteredMoment(const UnsignedInteger k) const;

  /**
   * Gives the raw moment of order k of the sample (by component)
   */
  Point computeRawMoment(const UnsignedInteger k) const;

  /**
   * Gives the quantile per component of the sample
   */
  virtual Point computeQuantilePerComponent(const Scalar prob) const;
  virtual Pointer<SampleImplementation> computeQuantilePerComponent(const Point & prob) const;

  /**
   * Gives the N-dimension quantile of the sample
   */
  Point computeQuantile(const Scalar prob) const;
  Pointer<SampleImplementation> computeQuantile(const Point & prob) const;

  /**
   * Get the empirical CDF of the sample
   */
  Scalar computeEmpiricalCDF(const Point & point,
                             const Bool tail = false) const;

  /** Maximum accessor */
  virtual Point getMax() const;

  /** Minimum accessor */
  virtual Point getMin() const;

  /** Ranked sample */
  Pointer<SampleImplementation> rank() const;

  /** Ranked component */
  Pointer<SampleImplementation> rank(const UnsignedInteger index) const;

  /** Sorted sample */
  Pointer<SampleImplementation> sort() const;
  void sortInPlace();

  /** Sorted component */
  Pointer<SampleImplementation> sort(const UnsignedInteger index) const;

  /** Sorted component */
  Pointer<SampleImplementation> sortAccordingToAComponent(const UnsignedInteger index) const;
  void sortAccordingToAComponentInPlace(const UnsignedInteger index);

  /* Sorted and duplicated points removed */
  Pointer<SampleImplementation> sortUnique() const;
  void sortUniqueInPlace();

  /** Get the i-th marginal sample */
  Pointer<SampleImplementation> getMarginal(const UnsignedInteger index) const;

  /** Get the marginal sample by indices */
  Pointer<SampleImplementation> getMarginal(const Indices & indices) const;

  /** Get the marginal sample by identifiers */
  Pointer<SampleImplementation> getMarginal(const Description & description) const;

  /** Select points in the sample */
  Pointer<SampleImplementation> select(const UnsignedIntegerCollection & indices) const;

  /**
   * Translate realizations in-place
   */
  SampleImplementation & operator += (const Scalar translation);
  SampleImplementation & operator += (const Point & translation);
  SampleImplementation & operator += (const SampleImplementation & translation);
  SampleImplementation & operator -= (const Scalar translation);
  SampleImplementation & operator -= (const Point & translation);
  SampleImplementation & operator -= (const SampleImplementation & translation);

  /** Translate realizations */
  SampleImplementation operator + (const Scalar translation) const;
  SampleImplementation operator + (const Point & translation) const;
  SampleImplementation operator + (const SampleImplementation & translation) const;
  SampleImplementation operator - (const Scalar translation) const;
  SampleImplementation operator - (const Point & translation) const;
  SampleImplementation operator - (const SampleImplementation & translation) const;

  SampleImplementation operator * (const Scalar scaling) const;
  SampleImplementation operator * (const Point & scaling) const;
  SampleImplementation operator / (const Scalar scaling) const;
  SampleImplementation operator / (const Point & scaling) const;

  /**
   * Scale realizations componentwise in-place
   */

  SampleImplementation & operator *= (const Scalar scaling);
  SampleImplementation & operator *= (const Point & scaling);
  SampleImplementation & operator /= (const Scalar scaling);
  SampleImplementation & operator /= (const Point & scaling);

  /** Save to CSV file */
  void exportToCSVFile(const FileName & filename,
                       const String & csvSeparator = ResourceMap::GetAsString( "csv-file-separator" )) const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);


private:
  // Parse a string into a vector of scalars and tell if there was an error
  static Bool ParseStringAsValues(const String & line,
                                  const char theSeparator,
                                  Point & data);

  // Parse a string into a vector of strings and tell if there was an error
  static Bool ParseStringAsDescription (const String & line,
                                        const char theSeparator,
                                        Description & data);

  // Parse a comment line
  static Bool ParseComment(const String & line, const String & markers);

  void translate(const Point & translation);
  void scale(const Point & scaling);

  /** The size of the sample */
  UnsignedInteger size_;

  /** The dimension of the sample */
  UnsignedInteger dimension_;

  /** The collection of unwrapped points */
  PersistentCollection<Scalar> data_;

  /** The description of all components */
  Pointer<Description> p_description_;

}; /* class SampleImplementation */


/* Comparison function */
OT_API Bool operator ==(const SampleImplementation & lhs, const SampleImplementation & rhs);
OT_API SampleImplementation operator *(const Scalar, const SampleImplementation & rhs);

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SAMPLEIMPLEMENTATION_HXX */
