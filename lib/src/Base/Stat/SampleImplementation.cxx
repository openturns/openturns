//                                               -*- C++ -*-
/**
 *  @brief The class SampleImplementation implements blank free samples
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
#include <sstream>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <regex>
#include <limits>

#include "openturns/OTconfig.hxx"
#include "openturns/SampleImplementation.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/Exception.hxx"
#include "openturns/TBBImplementation.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/StatisticsEngine.hxx"
#include "openturns/Lapack.hxx"
#include "openturns/CSVParser.hxx"

BEGIN_NAMESPACE_OPENTURNS


TEMPLATE_CLASSNAMEINIT(PersistentCollection<Point>)

static const Factory<PersistentCollection<Point> > Factory_PersistentCollection_Point;

NSI_point::NSI_point(SampleImplementation * p_nsi, const UnsignedInteger index)
  : p_nsi_(p_nsi), index_(index), dimension_(p_nsi->dimension_) {}

NSI_point::NSI_point(const NSI_point & other)
  : p_nsi_(other.p_nsi_), index_(other.index_), dimension_(other.dimension_) {}

NSI_point & NSI_point::operator = (const NSI_point & rhs)
{
  if ( (this != &rhs) && (getDimension() == rhs.getDimension()) )
    std::copy( rhs.begin(), rhs.end(), begin() );
  return *this;
}

// NSI_point & NSI_point::operator = (const NSI_const_point & rhs)
// {
//   if ( getDimension() == rhs.getDimension() )
//     std::copy( rhs.begin(), rhs.end(), begin() );
//   return *this;
// }

// NSI_point & NSI_point::operator = (const Point & rhs)
// {
//   if ( getDimension() == rhs.getDimension() )
//     std::copy( rhs.begin(), rhs.end(), begin() );
//   return *this;
// }

Scalar & NSI_point::operator [] (UnsignedInteger i)
{
  return p_nsi_->data_[index_ * dimension_ + i];
}

const Scalar & NSI_point::operator [] (const UnsignedInteger i) const
{
  return p_nsi_->data_[index_ * dimension_ + i];
}

Scalar & NSI_point::at (UnsignedInteger i)
{
  return p_nsi_->data_.at(index_ * dimension_ + i);
}

const Scalar & NSI_point::at (const UnsignedInteger i) const
{
  return p_nsi_->data_.at(index_ * dimension_ + i);
}

NSI_point & NSI_point::operator += (const NSI_point & other)
{
  if (getDimension() != other.getDimension())
    throw InvalidArgumentException(HERE)
        << "Points of different dimensions cannot be added (LHS dimension = "
        << getDimension()
        << "; RHS dimension = "
        << other.getDimension();

  for (UnsignedInteger i = 0; i < getDimension(); ++i) (*this)[i] += other[i];
  return *this;
}

NSI_point & NSI_point::operator -= (const NSI_point & other)
{
  if (getDimension() != other.getDimension())
    throw InvalidArgumentException(HERE)
        << "Points of different dimensions cannot be subtracted (LHS dimension = "
        << getDimension()
        << "; RHS dimension = "
        << other.getDimension();

  for (UnsignedInteger i = 0; i < getDimension(); ++i) (*this)[i] -= other[i];
  return *this;
}

NSI_point & NSI_point::operator += (const Point & other)
{
  if (getDimension() != other.getDimension())
    throw InvalidArgumentException(HERE)
        << "Points of different dimensions cannot be added (LHS dimension = "
        << getDimension()
        << "; RHS dimension = "
        << other.getDimension();

  for (UnsignedInteger i = 0; i < getDimension(); ++i) (*this)[i] += other[i];
  return *this;
}

NSI_point & NSI_point::operator -= (const Point & other)
{
  if (getDimension() != other.getDimension())
    throw InvalidArgumentException(HERE)
        << "Points of different dimensions cannot be subtracted (LHS dimension = "
        << getDimension()
        << "; RHS dimension = "
        << other.getDimension();

  for (UnsignedInteger i = 0; i < getDimension(); ++i) (*this)[i] -= other[i];
  return *this;
}

NSI_point & NSI_point::operator *= (const Scalar val)
{
  for(UnsignedInteger i = 0; i < getDimension(); ++i) (*this)[i] *= val;
  return *this;
}

NSI_point & NSI_point::operator /= (const Scalar val)
{
  for(UnsignedInteger i = 0; i < getDimension(); ++i) (*this)[i] /= val;
  return *this;
}


bool operator == (const NSI_point & lhs, const NSI_point & rhs)
{
  return (lhs.getDimension() == rhs.getDimension()) &&
         std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

bool operator != (const NSI_point & lhs, const NSI_point & rhs)
{
  return ! (lhs == rhs);
}

bool operator < (const NSI_point & lhs, const NSI_point & rhs)
{
  return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                      rhs.begin(), rhs.end(),
                                      std::less<Scalar>());
}

bool operator > (const NSI_point & lhs, const NSI_point & rhs)
{
  return rhs < lhs;
}

bool operator <= (const NSI_point & lhs, const NSI_point & rhs)
{
  return !(rhs < lhs);
}

bool operator >= (const NSI_point & lhs, const NSI_point & rhs)
{
  return !( lhs < rhs );
}


std::ostream & operator <<(std::ostream & os, const NSI_point & point)
{
  return os << Point( point );
}


OStream & operator << (OStream & OS, const NSI_point & point)
{
  OS.getStream() << Point( point ).__repr__();
  return OS;
}


NSI_const_point::NSI_const_point(const SampleImplementation * p_nsi, const UnsignedInteger index)
  : p_nsi_(p_nsi), index_(index), dimension_(p_nsi->dimension_) {}

NSI_const_point::NSI_const_point(const NSI_point & point)
  : p_nsi_(point.p_nsi_), index_(point.index_), dimension_(point.dimension_) {}

bool operator == (const NSI_const_point & lhs, const NSI_const_point & rhs)
{
  return (lhs.getDimension() == rhs.getDimension()) &&
         std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

const Scalar & NSI_const_point::operator [] (const UnsignedInteger i) const
{
  return p_nsi_->data_[index_ * dimension_ + i];
}

const Scalar & NSI_const_point::at (const UnsignedInteger i) const
{
  return p_nsi_->data_.at(index_ * dimension_ + i);
}

bool operator != (const NSI_const_point & lhs, const NSI_const_point & rhs)
{
  return ! (lhs == rhs);
}

bool operator < (const NSI_const_point & lhs, const NSI_const_point & rhs)
{
  return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                      rhs.begin(), rhs.end(),
                                      std::less<Scalar>());
}

bool operator > (const NSI_const_point & lhs, const NSI_const_point & rhs)
{
  return rhs < lhs;
}

bool operator <= (const NSI_const_point & lhs, const NSI_const_point & rhs)
{
  return !(rhs < lhs);
}

bool operator >= (const NSI_const_point & lhs, const NSI_const_point & rhs)
{
  return !( lhs < rhs );
}


std::ostream & operator <<(std::ostream & os, const NSI_const_point & point)
{
  return os << Point( point );
}


OStream & operator << (OStream & OS, const NSI_const_point & point)
{
  OS.getStream() << Point( point ).__repr__();
  return OS;
}

static const Factory<SampleImplementation> Factory_SampleImplementation;

CLASSNAMEINIT(SampleImplementation)

/* Default constructor is private */
SampleImplementation::SampleImplementation()
  : PersistentObject()
  , size_(0)
  , dimension_(0)
  , data_(size_ * dimension_, 0.0)
  , p_description_(NULL)
{
  // Nothing to do
}

/* Standard constructor */
SampleImplementation::SampleImplementation(const UnsignedInteger size,
    const UnsignedInteger dim)
  : PersistentObject()
  , size_(size)
  , dimension_(dim)
  , data_(size_ * dimension_, 0.0)
  , p_description_(NULL)
{
  // Nothing to do
}

/* Constructor from a Point */
SampleImplementation::SampleImplementation(const UnsignedInteger size,
    const Point & point)
  : PersistentObject()
  , size_(size)
  , dimension_(point.getDimension())
  , data_(size_ * dimension_, 0.0)
  , p_description_(NULL)
{
  for (UnsignedInteger i = 0; i < size_; ++i)
    for (UnsignedInteger j = 0; j < dimension_; ++j)
      data_[i * dimension_ + j] = point[j];
}


/* Constructor from a collection of Point */
SampleImplementation::SampleImplementation(const Collection<Point> & coll)
  : PersistentObject()
  , size_(coll.getSize())
  , dimension_((coll.getSize() > 0) ? coll[0].getDimension() : 0)
  , data_(size_ * dimension_, 0.0)
  , p_description_(NULL)
{
  for (UnsignedInteger i = 0; i < size_; ++i)
    for (UnsignedInteger j = 0; j < dimension_; ++j)
      data_[i * dimension_ + j] = coll[i][j];
}

/* Constructor from a collection of Indices */
SampleImplementation::SampleImplementation(const Collection<Indices> & coll)
  : PersistentObject()
  , size_(coll.getSize())
  , dimension_((coll.getSize() > 0) ? coll[0].getSize() : 0)
  , data_(size_ * dimension_, 0.0)
  , p_description_(NULL)
{
  for (UnsignedInteger i = 0; i < size_; ++i)
    for (UnsignedInteger j = 0; j < dimension_; ++j)
      data_[i * dimension_ + j] = coll[i][j];
}

/* Partial copy constructor */
SampleImplementation::SampleImplementation(const SampleImplementation & other, const_iterator first, const_iterator last)
  : PersistentObject()
  , size_(last - first)
  , dimension_(other.getDimension())
  , data_(size_ * dimension_, 0.0)
  , p_description_(other.p_description_)
{
  std::copy( first, last, begin() );
}


/* Copy constructor */
SampleImplementation::SampleImplementation(const SampleImplementation & other)
  : PersistentObject(other)
  , size_(other.size_)
  , dimension_(other.dimension_)
  , data_(other.data_)
  , p_description_(other.p_description_)
{
}


/* Assignment operator */
SampleImplementation & SampleImplementation::operator = (const SampleImplementation & other)
{
  if (this != &other)
  {
    PersistentObject::operator = (other);
    size_ = other.size_;
    dimension_ = other.dimension_;
    data_ = other.data_;
    p_description_ = other.p_description_;
  }
  return *this;
}


/* Virtual constructor */
SampleImplementation * SampleImplementation::clone() const
{
  return new SampleImplementation(*this);
}

const Scalar * SampleImplementation::data() const
{
  return data_.data();
}

UnsignedInteger SampleImplementation::elementSize() const
{
  return sizeof(Scalar);
}

void SampleImplementation::swap_points(const UnsignedInteger a, const UnsignedInteger b)
{
  std::swap_ranges( &data_[ a * dimension_ ], &data_[ (a + 1) * dimension_ ], &data_[ b * dimension_ ] );
}

void SampleImplementation::swap_range_points(const UnsignedInteger fa, const UnsignedInteger ta, const UnsignedInteger fb)
{
  for (UnsignedInteger i = 0; i < ta - fa; ++i) swap_points( fa + i, fb + i );
}



/* Description Accessor */
void SampleImplementation::setDescription(const Description & description)
{
  if (description.getSize() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given description does not match the sample dimension.";
  p_description_ = Pointer<Description>(new Description(description));
}


/* Description Accessor */
Description SampleImplementation::getDescription() const
{
  if (p_description_.isNull()) return Description::BuildDefault(dimension_, "v");
  return *p_description_;
}

/* Comparison function */
Bool operator ==(const SampleImplementation & lhs,
                 const SampleImplementation & rhs)
{
  if (&lhs == &rhs) return true;
  return (lhs.size_ == rhs.size_) && (lhs.dimension_ == rhs.dimension_) &&
         std::equal( lhs.begin(), lhs.end(), rhs.begin() );
}

void SampleImplementation::erase(iterator first,
                                 iterator last)
{
  data_.erase((first - begin()) * dimension_, (last - begin()) * dimension_);
  size_ -= last - first;
}

void SampleImplementation::erase(const UnsignedInteger first,
                                 const UnsignedInteger last)
{
  data_.erase(first * dimension_, last * dimension_);
  size_ -= last - first;
}

void SampleImplementation::clear()
{
  data_.clear();
  size_ = 0;
}


/* Raw internal format accessor */
Point SampleImplementation::getData() const
{
  Point result(size_ * dimension_);
  std::copy(data_.data(), data_.data() + size_ * dimension_, result.begin());
  return result;
}

void SampleImplementation::setData(const Collection<Scalar> & data)
{
  if (data.getSize() != dimension_ * size_) throw InvalidArgumentException(HERE) << "Error: the given raw data are not compatible with the dimension and size of the sample.";
  std::copy(data.begin(), data.end(), data_.data());
}

/* Whether the list contains the value val */
Bool SampleImplementation::contains(const Point & val) const
{
  for (UnsignedInteger i = 0; i < size_; ++i) if ( (*this)[i] == val ) return true;
  return false;
}

/* String converter */
String SampleImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << SampleImplementation::GetClassName()
      << " name=" << getName()
      << " size=" << size_
      << " dimension=" << dimension_;

  const UnsignedInteger printEllipsisThreshold = ResourceMap::GetAsUnsignedInteger("Sample-PrintEllipsisThreshold");
  const UnsignedInteger printEllipsisSize = ResourceMap::GetAsUnsignedInteger("Sample-PrintEllipsisSize");
  const Bool ellipsis = (data_.getSize() > printEllipsisThreshold);

  const Bool printDescription = !p_description_.isNull() && (p_description_->getSize() == dimension_) && !p_description_->isBlank();

  if (printDescription)
  {
    const char * sep = "";

    oss << " description=[";
    for (UnsignedInteger j = 0; j < dimension_; ++ j, sep = ",")
    {
      if (ellipsis && (dimension_ > 2 * printEllipsisSize))
      {
        if (j == printEllipsisSize)
        {
          oss << sep << "...";
        }
        if ((j >= printEllipsisSize) && (j < dimension_ - printEllipsisSize))
        {
          continue;
        }
      }
      oss << sep << (*p_description_)[j];
    }
    oss << "]";
  }

  oss << " data=[";
  const char * sep = "";

  for (UnsignedInteger i = 0; i < size_; ++ i, sep = ",")
  {
    if (ellipsis && (size_ > 2 * printEllipsisSize))
    {
      if (i == printEllipsisSize)
      {
        oss << sep << "...";
      }
      if ((i >= printEllipsisSize) && (i < size_ - printEllipsisSize))
      {
        continue;
      }
    }
    oss << sep << "[";
    const char * sep2 = "";
    for (UnsignedInteger j = 0; j < dimension_; ++ j, sep2 = ",")
    {
      if (ellipsis && (dimension_ > 2 * printEllipsisSize))
      {
        if (j == printEllipsisSize)
        {
          oss << sep2 << "...";
        }
        if ((j >= printEllipsisSize) && (j < dimension_ - printEllipsisSize))
        {
          continue;
        }
      }
      oss << sep2 << data_[i * dimension_ + j];
    }
    oss << "]";
  }
  oss << "]";
  return oss;
}

String SampleImplementation::__str__(const String & offset) const
{
  // First, print the description if it is not empty.
  // If you use the getDescription() method you get a default value
  // for the description that is not stored in the sample, producing a spurious output
  const Bool printDescription = !p_description_.isNull() && (p_description_->getSize() == dimension_) && !p_description_->isBlank();

  const UnsignedInteger printEllipsisThreshold = ResourceMap::GetAsUnsignedInteger("Sample-PrintEllipsisThreshold");
  const UnsignedInteger printEllipsisSize = ResourceMap::GetAsUnsignedInteger("Sample-PrintEllipsisSize");
  const Bool ellipsis = (size_ >= printEllipsisThreshold);

  size_t twidth = 0; // column title max width
  size_t lwidth = 0; // LHS number max width
  size_t rwidth = 0; // RHS number max width
  size_t iwidth = 0; // index max width

  if (printDescription)
  {
    for( UnsignedInteger j = 0; j < dimension_; ++j )
    {
      if (ellipsis && (dimension_ > 2 * printEllipsisSize))
      {
        if ((j >= printEllipsisSize) && (j < dimension_ - printEllipsisSize))
        {
          continue;
        }
      }
      twidth = std::max( twidth, (*p_description_)[j].size() );
    }
  }

  for( UnsignedInteger i = 0; i < size_; ++i )
  {
    if (ellipsis && (size_ > 2 * printEllipsisSize))
    {
      if ((i >= printEllipsisSize) && (i < size_ - printEllipsisSize))
      {
        continue;
      }
    }
    for (UnsignedInteger j = 0; j < dimension_; ++ j)
    {
      if (ellipsis && (dimension_ > 2 * printEllipsisSize))
      {
        if ((j >= printEllipsisSize) && (j < dimension_ - printEllipsisSize))
        {
          continue;
        }
      }
      String st = OSS() << data_[i * dimension_ + j];
      size_t dotpos = st.find( '.' );
      lwidth = std::max( lwidth, (dotpos != String::npos) ? dotpos             : st.size() );
      rwidth = std::max( rwidth, (dotpos != String::npos) ? st.size() - dotpos : 0         );
    }
  }

  if (twidth > lwidth + rwidth)
    rwidth = twidth - lwidth;
  else
    twidth = lwidth + rwidth;

  {
    // Computing the size of the last index (max width of the indexes)
    String sti = OSS() << size_ - 1;
    iwidth = sti.size();
  }

  OSS oss(false);
  // Print the column title
  if (printDescription)
  {
    oss << String( iwidth, ' ' ) << "   [ ";
    const char * sep = "";
    for( UnsignedInteger j = 0; j < dimension_; ++j, sep = " " )
    {
      if (ellipsis && (dimension_ > 2 * printEllipsisSize))
      {
        if (j == printEllipsisSize)
        {
          oss << sep << "...";
        }
        if ((j >= printEllipsisSize) && (j < dimension_ - printEllipsisSize))
        {
          continue;
        }
      }
      oss << sep << (*p_description_)[j] << String( twidth - (*p_description_)[j].size(), ' ' );
    }
    oss << " ]" << "\n" << offset;
  }

  const char * newline = "";
  for( UnsignedInteger i = 0; i < size_; ++i, newline = "\n" )
  {
    if (ellipsis && (size_ > 2 * printEllipsisSize))
    {
      if (i == printEllipsisSize)
      {
        oss << "\n" << offset <<  "...";
      }
      if ((i >= printEllipsisSize) && (i < size_ - printEllipsisSize))
      {
        continue;
      }
    }
    String sti = OSS() << i;
    oss << newline << offset << String( iwidth - sti.size(), ' ' ) << sti << " : [ ";
    const char * sep = "";
    for( UnsignedInteger j = 0; j < dimension_; ++j, sep = " " )
    {
      if (ellipsis && (dimension_ > 2 * printEllipsisSize))
      {
        if (j == printEllipsisSize)
        {
          oss << sep << "...";
        }
        if ((j >= printEllipsisSize) && (j < dimension_ - printEllipsisSize))
        {
          continue;
        }
      }
      String st = OSS() << data_[i * dimension_ + j];
      size_t dotpos = st.find( '.' );
      oss << sep << String( lwidth - ((dotpos != String::npos) ? dotpos : st.size()), ' ' )
          << st
          << String( rwidth - ((dotpos != String::npos) ? st.size() - dotpos : 0), ' ' );
    }
    oss << " ]";
  }
  return oss;
}


/* Appends an element to the collection */
SampleImplementation & SampleImplementation::add(const Point & point)
{
  if (point.getDimension() != dimension_)
    throw InvalidArgumentException(HERE) << "Point has invalid dimension ("
                                         << point.getDimension()
                                         << ") expected : "
                                         << getDimension();
  const UnsignedInteger oldSize = size_;
  const UnsignedInteger newCount = oldSize + 1;
  if (dimension_ != 0 && newCount > std::numeric_limits<UnsignedInteger>::max() / dimension_)
    throw InternalException(HERE) << "Error: cannot add point, overflow detected";
  data_.resize(newCount * dimension_);
  ++ size_;
  std::copy(point.begin(), point.begin() + dimension_, data_.data() + oldSize * dimension_);
  return *this;
}


/* Appends another sample to the collection */
SampleImplementation & SampleImplementation::add(const SampleImplementation & other)
{
  if (other.getDimension() != dimension_)
    throw InvalidArgumentException(HERE) << "Sample has invalid dimension (" << other.getDimension()
                                         << ") expected : " << getDimension();
  // save original size in case other=this
  const UnsignedInteger otherSize = other.getSize();
  size_ += otherSize;
  data_.resize(size_ * dimension_);
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4146)
#endif
  std::copy_backward(other.begin(), other.begin() + otherSize, end());
#ifdef _MSC_VER
#pragma warning(pop)
#endif
  return *this;
}

/* Stack the given sample to the current one */
SampleImplementation & SampleImplementation::stack(const SampleImplementation & sample)
{
  if (sample.getSize() != size_) throw InvalidArgumentException(HERE) << "Error: the given sample has size=" << sample.getSize() << ", expected size=" << size_;
  const UnsignedInteger otherDimension = sample.getDimension();
  const UnsignedInteger totalDimension = dimension_ + otherDimension;
  SampleImplementation result(size_, totalDimension);
  // First, the values
  for (UnsignedInteger i = 0; i < size_; ++ i)
  {
    std::copy(data_begin() + i * dimension_, data_begin() + (i + 1) * dimension_,
              result.data_begin() + i * totalDimension);
    std::copy(sample.data_begin() + i * otherDimension, sample.data_begin() + (i + 1) * otherDimension,
              result.data_begin() + i * totalDimension + dimension_);
  }
  // Second, the description
  if (!p_description_.isNull() || !sample.p_description_.isNull())
  {
    Description description(getDescription());
    const Description otherDescription(sample.getDescription());
    for (UnsignedInteger i = 0; i < otherDimension; ++ i)
      description.add(otherDescription[i]);
    result.setDescription(description);
  }
  *this = result;
  return *this;
}


template <typename OP>
class ParallelFunctor
{
  SampleImplementation & nsi_;
  const OP & op_;
public:
  ParallelFunctor(SampleImplementation & nsi, const OP & op) : nsi_(nsi), op_(op) {}

  void operator() (const TBBImplementation::BlockedRange<UnsignedInteger> & r) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i) op_.inplace_op( nsi_[i] );
  }

}; /* end class ParallelFunctor */



/*
 * Gives the mean of the sample, based on the formula
 * mean = sum of the elements in the sample / size of the sample
 */
Point SampleImplementation::computeMean() const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot compute the mean of an empty sample.";
  const DataContainer result(StatisticsEngine::ComputeMean(toDataContainer()));
  Point mean(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i) mean[i] = result[i];
  return mean;
}

/*
 * Gives the covariance matrix of the sample, normalization by 1 / (size - 1) if size > 1
 */
CovarianceMatrix SampleImplementation::computeCovariance() const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot compute the covariance of an empty sample.";
  // Special case for a sample of size 1
  if (size_ == 1) return CovarianceMatrix(dimension_, Point(dimension_ * dimension_));

  const DataContainer result(StatisticsEngine::ComputeCovariance(toDataContainer()));
  const UnsignedInteger squaredDim(dimension_ * dimension_);
  Point accumulated(squaredDim);
  for (UnsignedInteger i = 0; i < squaredDim; ++i) accumulated[i] = result[i];
  CovarianceMatrix matrix(dimension_, accumulated);
  return matrix;
}

/*
 * Gives the Euclidean distance matrix of the sample
 */
SymmetricMatrix SampleImplementation::computePairwiseDistance() const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot compute the pairwise distances of an empty sample.";
  SymmetricMatrix distances(size_, size_);
  for (UnsignedInteger i = 0; i < size_; ++i)
    for (UnsignedInteger j = i + 1; j < size_; ++j)
    {
      Scalar distance = 0.0;
      for (UnsignedInteger k = 0; k < dimension_; ++k)
      {
        const Scalar delta = operator()(i, k) - operator()(j, k);
        // hypot avoids the overflow/underflow of delta * delta
        distance = std::hypot(distance, delta);
      }
      distances(i, j) = distance;
    }
  return distances;
}

/*
 * Gives the standard deviation of each component of the sample.
 */
Point SampleImplementation::computeStandardDeviation() const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot compute the standard deviation per component of an empty sample.";
  Point sd(computeVariance());
  for (UnsignedInteger i = 0; i < dimension_; ++i) sd[i] = sqrt(sd[i]);
  return sd;
}


/*
 * Gives the variance of the sample (by component)
 */
Point SampleImplementation::computeVariance() const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot compute the variance per component of an empty sample.";

  // Special case for a sample of size 1
  if (size_ == 1) return Point(dimension_, 0.0);

  const DataContainer result(StatisticsEngine::ComputeVariance(toDataContainer()));
  Point variance(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i) variance[i] = result[i];
  return variance;

}


/*
 * Gives the Pearson correlation matrix of the sample
 */
CorrelationMatrix SampleImplementation::computeLinearCorrelation() const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot compute the Pearson correlation of an empty sample.";
  CorrelationMatrix correlation(dimension_);
  if (dimension_ == 1) return correlation;

  const DataContainer result(StatisticsEngine::ComputePearsonCorrelation(toDataContainer()));
  for (UnsignedInteger i = 0; i < dimension_; ++i)
    for (UnsignedInteger j = 0; j < dimension_; ++j) correlation(i, j) = result(i, j);

  return correlation;
}

struct Pair
{
  Scalar value_;
  UnsignedInteger index_;
  Pair() : value_(0.0), index_() {}
  Pair(Scalar value, UnsignedInteger index) : value_(value), index_(index) {}
  Bool operator < (const Pair & other) const
  {
    return value_ < other.value_;
  }
};

struct Comparison
{
  // Main sorting key
  UnsignedInteger first_;
  // Secondary sorting key
  UnsignedInteger second_;
  // Pointer to the data
  const SampleImplementation & nsi_;
  // Sorting permutation
  Indices permutation_;

  Comparison(UnsignedInteger first,
             const SampleImplementation & nsi)
    : first_(first), second_(first), nsi_(nsi), permutation_(nsi_.getSize())
  {
    permutation_.fill();
  }

  Comparison(UnsignedInteger first, UnsignedInteger second,
             const SampleImplementation & nsi)
    : first_(first), second_(second), nsi_(nsi), permutation_(nsi_.getSize())
  {
    permutation_.fill();
  }

  Bool operator() (const UnsignedInteger i, const UnsignedInteger j) const
  {
    const Scalar xI = nsi_( permutation_[i], first_  );
    const Scalar xJ = nsi_( permutation_[j], first_  );
    const Scalar yI = nsi_( permutation_[i], second_ );
    const Scalar yJ = nsi_( permutation_[j], second_ );
    return ( (xI < xJ) || ((xI == xJ) && (yI < yJ)) );
  }
}; // struct Comparison




/* Ranked sample */
Pointer<SampleImplementation> SampleImplementation::rank() const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot rank an empty sample.";
  Pointer<SampleImplementation> rankedSample = new SampleImplementation(size_, dimension_);

  // Sort and rank all the marginal samples
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    Collection<Pair> sortedMarginalSamples(size_);
    for (UnsignedInteger j = 0; j < size_; ++j)
    {
      sortedMarginalSamples[j].value_ = operator()(j, i);
      sortedMarginalSamples[j].index_ = j;
    }
    // sort
    TBBImplementation::ParallelSort(sortedMarginalSamples.begin(), sortedMarginalSamples.end());
    // rank
    Scalar lastValue = sortedMarginalSamples[0].value_;
    UnsignedInteger lastIndex = 0;
    Scalar currentValue = 0.0;
    for (UnsignedInteger j = 1; j < size_; ++j)
    {
      currentValue = sortedMarginalSamples[j].value_;
      if (currentValue > lastValue)
      {
        const Scalar rankValue = 0.5 * (lastIndex + j - 1);
        for (UnsignedInteger k = lastIndex; k < j; ++k) rankedSample->operator()(sortedMarginalSamples[k].index_, i) = rankValue;
        lastIndex = j;
        lastValue = currentValue;
      }
    }
    // If we end with a constant range
    if (currentValue == lastValue)
    {
      const Scalar rankValue = 0.5 * (lastIndex + size_ - 1);
      for (UnsignedInteger k = lastIndex; k < size_; ++k) rankedSample->operator()(sortedMarginalSamples[k].index_, i) = rankValue;
    }
  }
  if (!p_description_.isNull()) rankedSample->setDescription(getDescription());
  return rankedSample;
}

/* Ranked component */
Pointer<SampleImplementation> SampleImplementation::rank(const UnsignedInteger index) const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot rank an empty sample.";
  if (!(index < dimension_)) throw OutOfBoundException(HERE) << "The requested index is too large, index=" << index << ", dimension=" << dimension_;
  return getMarginal(index)->rank();
}

/* Sorted sample, component by component */
Pointer<SampleImplementation> SampleImplementation::sort() const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot sort an empty sample.";

  Pointer<SampleImplementation> sortedSample = SampleImplementation::FromDataContainer(StatisticsEngine::SortLexicographic(toDataContainer()));
  if (!p_description_.isNull()) sortedSample->setDescription(getDescription());
  return sortedSample;
}

void SampleImplementation::sortInPlace()
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot sort an empty sample.";

  // Sort a copy, then write back in place to preserve view sharing
  const DataContainer sorted(StatisticsEngine::SortLexicographic(toDataContainer()));
  std::copy(sorted.data(), sorted.data() + size_ * dimension_, data_.data());
}

/* Sorted sample, one component */
Pointer<SampleImplementation> SampleImplementation::sort(const UnsignedInteger index) const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot sort an empty sample.";

  if (!(index < getDimension())) throw OutOfBoundException(HERE) << "The requested index is too large, index=" << index << ", dimension=" << getDimension();

  return getMarginal(index)->sort();
}


/* Sorted according a component */
Pointer<SampleImplementation> SampleImplementation::sortAccordingToAComponent(const UnsignedInteger index) const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot sort an empty sample.";
  if (!(index < getDimension())) throw OutOfBoundException(HERE) << "The requested index is too large, index=" << index << ", dimension=" << getDimension();

  Pointer<SampleImplementation> sortedSample = SampleImplementation::FromDataContainer(StatisticsEngine::Sort(toDataContainer(), index));
  if (!p_description_.isNull()) sortedSample->setDescription(getDescription());
  return sortedSample;
}

/* Sorted according a component */
void SampleImplementation::sortAccordingToAComponentInPlace(const UnsignedInteger index)
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot sort an empty sample.";
  if (!(index < getDimension())) throw OutOfBoundException(HERE) << "The requested index is too large, index=" << index << ", dimension=" << getDimension();

  // Sort a copy, then write back in place to preserve view sharing
  const DataContainer sorted(StatisticsEngine::Sort(toDataContainer(), index));
  std::copy(sorted.data(), sorted.data() + size_ * dimension_, data_.data());
}

/* Sort and remove duplicated points */
Pointer<SampleImplementation> SampleImplementation::sortUnique() const
{
  Pointer<SampleImplementation> sampleSorted(sort());
  Pointer<SampleImplementation> sampleUnique = new SampleImplementation(size_, dimension_);
  sampleUnique->operator[](0) = sampleSorted->operator[](0);
  UnsignedInteger last = 0;
  for (UnsignedInteger i = 1; i < size_; ++i)
  {
    if (sampleSorted->operator[](i) != sampleUnique->operator[](last))
    {
      ++last;
      sampleUnique->operator[](last) = sampleSorted->operator[](i);
    }
  }
  if (last + 1 < size_) sampleUnique->erase(last + 1, size_);
  if (!p_description_.isNull()) sampleUnique->setDescription(getDescription());
  return sampleUnique;
}

void SampleImplementation::sortUniqueInPlace()
{
  sortInPlace();
  UnsignedInteger last = 0;
  for (UnsignedInteger i = 1; i < size_; ++i)
  {
    if ((*this)[i] != (*this)[last])
    {
      ++last;
      (*this)[last] = (*this)[i];
    }
  }
  if (last + 1 < size_) erase(last + 1, size_);
}

Indices SampleImplementation::argsort(Bool isIncreasing) const
{
  Collection< std::pair<Point, UnsignedInteger> > pointsPairs(size_);
  if (isIncreasing)
  {
    for (UnsignedInteger i = 0; i < size_; ++i)
    {
      pointsPairs[i] = std::pair<Point, UnsignedInteger>((*this)[i], i);
    }
  }
  else
  {
    for (UnsignedInteger i = 0; i < size_; ++i)
    {
      const Point pointI(static_cast<Point>((*this)[i]));
      pointsPairs[i] = std::pair<Point, UnsignedInteger>(- pointI, i);
    }
  }
  std::sort(pointsPairs.begin(), pointsPairs.end());

  Indices sortedIndices(size_);
  for (UnsignedInteger i = 0; i < size_; ++i)
    sortedIndices[i] = pointsPairs[i].second;

  return sortedIndices;
}

/*
 * Gives the Spearman correlation matrix of the sample
 */
CorrelationMatrix SampleImplementation::computeSpearmanCorrelation() const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot compute the Spearman correlation of an empty sample.";

  return rank()->computeLinearCorrelation();
}


/*
 * Gives the Kendall tau matrix of the sample, including ties correction
 */
CorrelationMatrix SampleImplementation::computeKendallTau() const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot compute the Kendall tau of an empty sample.";
  if (dimension_ < 2) return CorrelationMatrix(dimension_);

  const DataContainer result(StatisticsEngine::ComputeKendallTau(toDataContainer()));
  CorrelationMatrix tau(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
    for (UnsignedInteger j = 0; j < dimension_; ++j) tau(i, j) = result(i, j);
  return tau;
}

/*
 * Gives the range of the sample (by component)
 */
Point SampleImplementation::computeRange() const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot compute the range per component of an empty sample.";
  return getMax() - getMin();
}

/*
 * Gives the median of the sample (by component)
 */
Point SampleImplementation::computeMedian() const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot compute the median per component of an empty sample.";
  return computeQuantilePerComponent(0.5);
}

/*
 * Gives the skewness of the sample (by component)
 */
Point SampleImplementation::computeSkewness(Bool adjusted) const
{
  if (!(size_ >= 2)) throw InternalException(HERE) << "Error: cannot compute the skewness per component of a sample of size less than 2.";

  if (size_ == 2) return Point(dimension_, 0.0);

  // Preserve the historical guard: the skewness is not defined on a constant component
  const Point variance(computeVariance());
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    if (!(variance[i] < 0.0 || variance[i] > 0.0)) throw NotDefinedException(HERE) << "Error: the sample has component " << i << " constant. The skewness is not defined.";
  }
  const DataContainer result(StatisticsEngine::ComputeSkewness(toDataContainer(), adjusted));
  Point skewness(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i) skewness[i] = result[i];
  return skewness;
}

/*
 * Gives the kurtosis of the sample (by component)
 */
Point SampleImplementation::computeKurtosis(Bool adjusted) const
{
  if (!(size_ >= 3)) throw InternalException(HERE) << "Error: cannot compute the kurtosis per component of a sample of size less than 3.";

  if (size_ == 3) return Point(dimension_, 0.0);

  // Preserve the historical guard: the kurtosis is not defined on a constant component
  const Point variance(computeVariance());
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    if (!(variance[i] < 0.0 || variance[i] > 0.0)) throw NotDefinedException(HERE) << "Error: the sample has component " << i << " constant. The kurtosis is not defined.";
  }
  const DataContainer result(StatisticsEngine::ComputeKurtosis(toDataContainer(), adjusted));
  Point kurtosis(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i) kurtosis[i] = result[i];
  return kurtosis;
}

/*
 * Gives the central moment of order k of the sample (by component)
 */
Point SampleImplementation::computeCentralMoment(const UnsignedInteger k) const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot compute the central moments per component of an empty sample.";

  // Special case: order 0, return (1,...,1)
  if (k == 0) return Point(dimension_, 1.0);
  // Special case: order 1, return (0,...,0)
  if (k == 1) return Point(dimension_, 0.0);
  // Special case: order 2, return biased variance estimator
  if (k == 2) return computeVariance() * (size_ - 1.0) / size_;
  // General case
  const DataContainer result(StatisticsEngine::ComputeCentralMoment(toDataContainer(), k));
  Point centeredMoment(dimension_);
  for (UnsignedInteger j = 0; j < dimension_; ++j) centeredMoment[j] = result[j];
  return centeredMoment;
}

/*
 * Gives the raw moment of order k of the sample (by component)
 */
Point SampleImplementation::computeRawMoment(const UnsignedInteger k) const
{
  if (size_ == 0) throw InvalidArgumentException(HERE) << "Cannot compute centered moments on an empty sample";

  // Special case: order 0, return (1,...,1)
  if (k == 0) return Point(dimension_, 1.0);

  const DataContainer result(StatisticsEngine::ComputeRawMoment(toDataContainer(), k));
  Point moment(dimension_);
  for (UnsignedInteger j = 0; j < dimension_; ++j) moment[j] = result[j];
  return moment;
}

Point SampleImplementation::computeSuperQuantile(const Scalar prob) const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot compute the super-quantile of an empty sample.";
  if (!(prob >= 0.0) || !(prob <= 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a super-quantile for a probability level outside of [0, 1]";
  // Mean of the points above the quantile, component by component
  const Point threshold(computeQuantilePerComponent(prob));
  Point result(dimension_, 0.0);
  Point counts(dimension_, 0.0);
  for (UnsignedInteger i = 0; i < size_; ++i)
    for (UnsignedInteger j = 0; j < dimension_; ++j)
      if (operator()(i, j) >= threshold[j])
      {
        result[j] += operator()(i, j);
        counts[j] += 1.0;
      }
  for (UnsignedInteger j = 0; j < dimension_; ++j)
  {
    if (!(counts[j] > 0.0)) throw InternalException(HERE) << "Error: cannot compute the super-quantile in component " << j << ", no point is above the quantile.";
    result[j] /= counts[j];
  }
  return result;
}

/*
 * Gives the quantile per component of the sample
 */
Point SampleImplementation::computeQuantilePerComponent(const Scalar prob,
    const Bool tail,
    const Bool type7) const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot compute the quantile per component of an empty sample.";
  if (!(prob >= 0.0) || !(prob <= 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a quantile for a probability level outside of [0, 1]";

  // Tail flag orders 1-p instead of p, which stays accurate when p is close to 1
  const Scalar effectiveProb = tail ? 1.0 - prob : prob;
  const DataContainer result(StatisticsEngine::ComputeQuantile(toDataContainer(), effectiveProb, type7));
  Point quantile(dimension_);
  for (UnsignedInteger j = 0; j < dimension_; ++j) quantile[j] = result[j];
  return quantile;
}

/*
 * Gives the quantile per component of the sample
 */
Pointer<SampleImplementation> SampleImplementation::computeQuantilePerComponent(const Point & prob,
    const Bool tail,
    const Bool type7) const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot compute the quantile per component of an empty sample.";
  const UnsignedInteger probSize = prob.getSize();
  if (!(probSize > 0)) throw InternalException(HERE) << "Error: cannot compute the quantile per component with an empty argument.";

  // Tail flag orders 1-p instead of p
  Point effectiveProb(probSize);
  for (UnsignedInteger i = 0; i < probSize; ++i) effectiveProb[i] = tail ? 1.0 - prob[i] : prob[i];

  // Check that prob is inside bounds
  for (UnsignedInteger p = 0; p < probSize; ++p)
    if (!(effectiveProb[p] >= 0.0) || !(effectiveProb[p] <= 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a quantile for a probability level outside of [0, 1]";

  DataContainer levels(1, probSize, 0.0, DataContainer::ROW_MAJOR);
  for (UnsignedInteger p = 0; p < probSize; ++p) levels[p] = effectiveProb[p];
  const DataContainer result(StatisticsEngine::ComputeQuantile(toDataContainer(), levels, type7));

  Pointer<SampleImplementation> quantile = new SampleImplementation(probSize, dimension_);
  for (UnsignedInteger j = 0; j < dimension_; ++j)
    for (UnsignedInteger p = 0; p < probSize; ++p)
      quantile->operator()(p, j) = result(p, j);
  quantile->setDescription(Description::BuildDefault(dimension_, "q"));
  return quantile;
}

/*
 * Gives the N-dimension quantile of the sample
 */
Point SampleImplementation::computeQuantile(const Scalar prob,
    const Bool tail,
    const Bool type7) const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot compute the quantile of an empty sample.";

  if (getDimension() == 1) return computeQuantilePerComponent(prob, tail, type7);
  throw NotYetImplementedException(HERE) << "In SampleImplementation::computeQuantile(const Scalar prob) const";
}

Pointer<SampleImplementation> SampleImplementation::computeQuantile(const Point & prob,
    const Bool tail,
    const Bool type7) const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Error: cannot compute the quantile of an empty sample.";

  if (getDimension() == 1) return computeQuantilePerComponent(prob, tail, type7);
  throw NotYetImplementedException(HERE) << "In SampleImplementation::computeQuantile(const Point & prob) const";
}

/*
 * Get the empirical CDF of the sample
 */
Scalar SampleImplementation::computeEmpiricalCDF(const Point & x,
    const Bool survival) const
{
  if (!(size_ > 0)) throw InvalidArgumentException(HERE) << "Cannot compute the empirical CDF of an empty sample.";
  if (getDimension() != x.getDimension()) throw InvalidArgumentException(HERE) << "Point has incorrect dimension. Got "
        << x.getDimension() << ". Expected " << getDimension();

  DataContainer location(1, dimension_, 0.0, DataContainer::ROW_MAJOR);
  for (UnsignedInteger j = 0; j < dimension_; ++j) location[j] = x[j];
  return StatisticsEngine::ComputeEmpiricalCDF(toDataContainer(), location, survival);
}


/* Maximum accessor */
Point SampleImplementation::getMax() const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Impossible to get the maximum of an empty Sample";

  const DataContainer result(StatisticsEngine::ComputeMax(toDataContainer()));
  Point maxPoint(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i) maxPoint[i] = result[i];
  return maxPoint;
}

/* Minimum accessor */
Point SampleImplementation::getMin() const
{
  if (!(size_ > 0)) throw InternalException(HERE) << "Impossible to get the minimum of an empty Sample";

  const DataContainer result(StatisticsEngine::ComputeMin(toDataContainer()));
  Point minPoint(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i) minPoint[i] = result[i];
  return minPoint;
}


/*
 * Translate realizations in-place
 */
void SampleImplementation::translate(const Point & translation)
{
  if (dimension_ != translation.getDimension())
    throw InvalidDimensionException(HERE) << "Translation point has incorrect dimension. Got " << translation.getDimension()
                                          << ". Expected " << dimension_;

  if (size_ == 0) return;

  for(UnsignedInteger i = 0; i < size_; ++i)
    for (UnsignedInteger j = 0; j < dimension_; ++j)
      operator()(i, j) += translation[j];
}

SampleImplementation & SampleImplementation::operator += (const Scalar translation)
{
  translate(Point(dimension_, translation));
  return *this;
}

SampleImplementation & SampleImplementation::operator += (const Point & translation)
{
  translate(translation);
  return *this;
}

SampleImplementation & SampleImplementation::operator += (const SampleImplementation & translation)
{
  if (translation.getDimension() != dimension_) throw InvalidDimensionException(HERE) << "Error: the dimension of the given translation=" << translation.getDimension() << " does not match the dimension of the sample=" << dimension_;
  if (translation.getSize() != size_) throw InvalidDimensionException(HERE) << "Error: the size of the given translation=" << translation.getSize() << " does not match the size of the sample=" << size_;
  int size = size_ * dimension_;
  double alpha = 1.0;
  int one = 1;
  daxpy_(&size, &alpha, const_cast<double*>(translation.data()), &one, const_cast<double*>(data()), &one);
  return *this;
}

SampleImplementation & SampleImplementation::operator -= (const Scalar translation)
{
  return operator +=(-translation);
}

SampleImplementation & SampleImplementation::operator -= (const Point & translation)
{
  return operator +=(translation * (-1.0));
}

SampleImplementation & SampleImplementation::operator -= (const SampleImplementation & translation)
{
  if (translation.getDimension() != dimension_) throw InvalidDimensionException(HERE) << "Error: the dimension of the given translation=" << translation.getDimension() << " does not match the dimension of the sample=" << dimension_;
  if (translation.getSize() != size_) throw InvalidDimensionException(HERE) << "Error: the size of the given translation=" << translation.getSize() << " does not match the size of the sample=" << size_;
  int size = size_ * dimension_;
  double alpha = -1.0;
  int one = 1;
  daxpy_(&size, &alpha, const_cast<double*>(translation.data()), &one, const_cast<double*>(data()), &one);
  return *this;
}

SampleImplementation SampleImplementation::operator + (const Scalar translation) const
{
  return operator+(Point(dimension_, translation));
}

SampleImplementation SampleImplementation::operator + (const Point & translation) const
{
  SampleImplementation sample(*this);
  sample += translation;
  sample.setName("");
  return sample;
}

SampleImplementation SampleImplementation::operator + (const SampleImplementation & translation) const
{
  SampleImplementation sample(*this);
  sample += translation;
  sample.setName("");
  return sample;
}

SampleImplementation SampleImplementation::operator - (const Scalar translation) const
{
  return operator-(Point(dimension_, translation));
}

SampleImplementation SampleImplementation::operator - (const Point & translation) const
{
  SampleImplementation sample(*this);
  sample -= translation;
  sample.setName("");
  return sample;
}

SampleImplementation SampleImplementation::operator - (const SampleImplementation & translation) const
{
  SampleImplementation sample(*this);
  sample -= translation;
  sample.setName("");
  return sample;
}

/*
 * Scale realizations componentwise in-place
 */
void SampleImplementation::scale(const Point & scaling)
{
  if (dimension_ != scaling.getDimension())
    throw InvalidArgumentException(HERE) << "Scaling point has incorrect dimension. Got " << scaling.getDimension()
                                         << ". Expected " << dimension_;

  if (size_ == 0) return;

  for(UnsignedInteger i = 0; i < size_; ++i)
    for (UnsignedInteger j = 0; j < dimension_; ++j)
      operator()(i, j) *= scaling[j];
}

SampleImplementation & SampleImplementation::operator *= (const Scalar scaling)
{
  scale(Point(dimension_, scaling));
  return *this;
}

SampleImplementation & SampleImplementation::operator *= (const Point & scaling)
{
  scale(scaling);
  return *this;
}

SampleImplementation & SampleImplementation::operator /= (const Scalar scaling)
{
  return operator/=(Point(dimension_, scaling));
}

SampleImplementation & SampleImplementation::operator /= (const Point & scaling)
{
  Point inverseScaling(getDimension());
  for (UnsignedInteger i = 0; i < getDimension(); ++ i)
  {
    if (!(scaling[i] < 0.0 || scaling[i] > 0.0)) throw InvalidArgumentException(HERE) << "Error: the scaling must have nonzero components, here scaling=" << scaling;
    inverseScaling[i] = 1.0 / scaling[i];
  }
  scale(inverseScaling);
  return *this;
}

SampleImplementation SampleImplementation::operator * (const Scalar scaling) const
{
  return operator*(Point(dimension_, scaling));
}

SampleImplementation SampleImplementation::operator * (const Point & scaling) const
{
  SampleImplementation nsi(*this);
  nsi *= scaling;
  nsi.setName("");
  return nsi;
}

SampleImplementation SampleImplementation::operator / (const Scalar scaling) const
{
  return operator/(Point(dimension_, scaling));
}

SampleImplementation SampleImplementation::operator / (const Point & scaling) const
{
  SampleImplementation nsi(*this);
  nsi /= scaling;
  nsi.setName("");
  return nsi;
}

/* Get the i-th marginal sample */
Pointer<SampleImplementation> SampleImplementation::getMarginal(const UnsignedInteger index) const
{
  if (!(index < dimension_)) throw InvalidArgumentException(HERE) << "The index of a marginal sample must be in the range [0, dim-1]";

  // Special case for dimension 1
  if (dimension_ == 1) return clone();

  // General case
  Pointer<SampleImplementation> marginalSample = new SampleImplementation(size_, 1);

  // If the sample has a description, extract the marginal description
  if (!p_description_.isNull())
    marginalSample->setDescription(Description(1, getDescription()[index]));
  for (UnsignedInteger i = 0; i < size_; ++i)
    marginalSample->operator()(i, 0) = operator()(i, index);

  return marginalSample;
}

/* Get the marginal by indices */
Pointer<SampleImplementation> SampleImplementation::getMarginal(const Indices & indices) const
{
  if (!indices.check(dimension_)) throw InvalidArgumentException(HERE) << "The indices of a marginal sample must be in the range [0, dim-1] and must be different";

  // Special case for dimension 0
  if (!indices.getSize()) return new SampleImplementation(size_, 0);

  // Special case for dimension 1
  if (dimension_ == 1) return clone();

  // General case
  const UnsignedInteger outputDimension = indices.getSize();
  Pointer<SampleImplementation> marginalSample = new SampleImplementation(size_, outputDimension);

  // If the sample has a description, extract the marginal description
  if (!p_description_.isNull())
    marginalSample->setDescription(getDescription().select(indices));

  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
    {
      // We access directly to the component of the Point for performance reason
      marginalSample->operator()(i, j) = operator()(i, indices[j]);
    }
  }

  return marginalSample;
}

/* Get the marginal by identifiers */
Pointer<SampleImplementation> SampleImplementation::getMarginal(const Description & description) const
{
  Indices indices;
  for (UnsignedInteger i = 0; i < description.getSize(); ++ i)
  {
    const UnsignedInteger index = getDescription().find(description[i]);
    if (!(index < getDimension()))
      throw InvalidArgumentException(HERE) << "Marginal " << description[i] << " not found";
    indices.add(index);
  }
  return getMarginal(indices);
}

/* Select points as a sample */
Pointer<SampleImplementation> SampleImplementation::select(const UnsignedIntegerCollection & indices) const
{
  const UnsignedInteger size = indices.getSize();

  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const UnsignedInteger index = indices[i];
    if (!(index < size_)) throw InvalidArgumentException(HERE) << "selection expected indices less than " << size_ << ", here indices[" << i << "]=" << index;
  }

  Pointer<SampleImplementation> result = new SampleImplementation(size, dimension_);
  UnsignedInteger offset = 0;
  UnsignedInteger i1 = 0;
  while (i1 < size)
  {
    // count consecutive indices
    UnsignedInteger i2 = 0;
    for (i2 = i1 + 1; i2 < size; ++ i2)
    {
      if (indices[i2] != indices[i2 - 1] + 1)
        break;
    }
    const UnsignedInteger count = i2 - i1;
    const UnsignedInteger start = indices[i1];
    std::copy(data_.data() + start * dimension_, data_.data() + (start + count) * dimension_, result->data_.data() + offset * dimension_);
    offset += count;
    i1 = i2;
  }
  result->setDescription(getDescription());
  return result;
}

/* Save to CSV file */
void SampleImplementation::exportToCSVFile(const FileName & fileName,
    const String & separator,
    const String & decimalSeparator,
    const UnsignedInteger precision,
    const String & format) const
{
  if (decimalSeparator.size() != 1)
    throw InvalidArgumentException(HERE) << "Decimal separator must be a string of size 1, got " << decimalSeparator.size();
  if (separator == decimalSeparator)
    throw InvalidArgumentException(HERE) << "Column and decimal separators cannot be identical";
#if defined(__cplusplus) && (__cplusplus >= 202002L)
  const std::u8string u8FileName(reinterpret_cast<const char8_t*>(fileName.data()),
                                 reinterpret_cast<const char8_t*>(fileName.data() + fileName.size()));
  std::ofstream csvFile(std::filesystem::path{u8FileName});
#else
  std::ofstream csvFile(std::filesystem::u8path(fileName));
#endif
  if (csvFile.fail())
    throw FileOpenException(HERE) << "Could not open file " << fileName;

  csvFile.imbue(std::locale(std::locale::classic(), new CSVParser::CSVParserFormat(decimalSeparator[0])));

  // Export the description
  if (!p_description_.isNull())
  {
    const Description description(getDescription());
    String separatorI;
    for (UnsignedInteger i = 0; i < dimension_; ++ i, separatorI = separator)
    {
      String label(description[i]);
      Bool isBlank = true;
      for (UnsignedInteger j = 0; isBlank && j < label.size(); ++ j)
        isBlank = (label[j] == ' ') || (label[j] == '\t');
      if (isBlank)
        label = "NoDescription";
      csvFile << separatorI << "\"" << label << "\"";
    }
    csvFile << "\n";
  }

  csvFile.precision(precision);
  if (format == "scientific")
    csvFile << std::scientific;
  else if (format == "fixed")
    csvFile << std::fixed;
  else if (format != "defaultfloat")
    throw InvalidArgumentException(HERE) << "Invalid format: " << format << " must be scientific/fixed/defaultfloat";

  // Write the data
  UnsignedInteger index = 0;
  for(UnsignedInteger i = 0; i < size_; ++i)
  {
    csvFile << data_[index];
    ++index;
    for(UnsignedInteger j = 1; j < dimension_; ++j)
    {
      csvFile << separator << data_[index];
      ++index;
    } // j
    csvFile << "\n";
  } // i
  // Close the file
  csvFile.close();
}


/* Convert to DataContainer (row-major) */
DataContainer SampleImplementation::toDataContainer() const
{
  // Always row-major, even empty: engine kernels check the layout first
  DataContainer dc(size_, dimension_, 0.0, DataContainer::ROW_MAJOR);
  std::copy(data_begin(), data_end(), dc.data());
  return dc;
}

/* Construct a SampleImplementation from a DataContainer (row-major) */
Pointer<SampleImplementation> SampleImplementation::FromDataContainer(const DataContainer & dc)
{
  if (dc.isEmpty()) return new SampleImplementation(0, 0);
  const UnsignedInteger nbRows = dc.getSize();
  const UnsignedInteger nbCols = dc.getDimension();
  Pointer<SampleImplementation> p_sam = new SampleImplementation(nbRows, nbCols);
  std::copy(dc.data(), dc.data() + nbRows * nbCols, p_sam->data_begin());
  return p_sam;
}

/* Shallow constructor: share a view's storage, copy owned data */
Pointer<SampleImplementation> SampleImplementation::FromDataContainerView(const DataContainer & dc)
{
  if (dc.isEmpty()) return new SampleImplementation(0, 0);
  const UnsignedInteger nbRows = dc.getSize();
  const UnsignedInteger nbCols = dc.getDimension();
  Pointer<SampleImplementation> p_sam = new SampleImplementation(nbRows, nbCols);
  p_sam->data_ = dc; // DataContainer shares views, deep-copies owning buffers
  return p_sam;
}


/* Method save() stores the object through the StorageManager */
void SampleImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "size_", size_);
  adv.saveAttribute( "dimension_", dimension_);
  // Store the flat data inline (self-contained node)
  const UnsignedInteger totalSize = size_ * dimension_;
  adv.saveAttribute( "size", totalSize );
  AdvocateIterator<Scalar> adv_it(adv);
  const Scalar * pData = data_.data();
  for (UnsignedInteger i = 0; i < totalSize; ++i, ++adv_it)
  {
    *adv_it = pData[i];
  }
  if (!p_description_.isNull())
    adv.saveAttribute( "description_", *p_description_ );
}


/* Method load() reloads the object from the StorageManager */
void SampleImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "size_", size_);
  adv.loadAttribute( "dimension_", dimension_);
  const UnsignedInteger totalSize = size_ * dimension_;
  data_.resize(totalSize);
  if (adv.hasAttribute("data_"))
  {
    // Legacy format: the data was stored as a nested PersistentCollection<Scalar>
    PersistentCollection<Scalar> legacyData;
    adv.loadAttribute( "data_", legacyData );
    std::copy(legacyData.begin(), legacyData.begin() + totalSize, data_.data());
  }
  else
  {
    Scalar * pData = data_.data();
    AdvocateIterator<Scalar> adv_it(adv);
    for (UnsignedInteger i = 0; i < totalSize; ++i, ++adv_it)
    {
      pData[i] = adv_it();
    }
  }
  Description description;
  adv.loadAttribute( "description_", description );
  if (description.getSize() != 0) setDescription(description);
}


/* Product by a scalar */
SampleImplementation operator *(const Scalar scalar,
                                const SampleImplementation & rhs)
{
  return rhs.operator * (scalar);
}

END_NAMESPACE_OPENTURNS
