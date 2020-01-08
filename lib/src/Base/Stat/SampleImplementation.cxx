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
#include <sstream>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include <cstdio>        // std::fopen, std::errno
#include <cstring>       // std::strerror

#include "openturns/OTconfig.hxx"
#include "openturns/SampleImplementation.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Path.hxx"
#include "openturns/TBB.hxx"
#include "kendall.h"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Lapack.hxx"
#include "openturns/Os.hxx"

#include <locale.h>
#ifdef OPENTURNS_HAVE_XLOCALE_H
#include <xlocale.h>
#endif
#if defined(OPENTURNS_HAVE_BISON) && defined(OPENTURNS_HAVE_FLEX)
#include "openturns/csv_parser_state.hxx"
#include "csv_parser.hh"
#include "csv_lexer.h"

int csvparse (OT::CSVParserState & theState, yyscan_t yyscanner, FILE * theFile, OT::SampleImplementation &impl, OT::UnsignedInteger & theDimension, const char * separator);
#endif

namespace
{

// wraps std::getline to handle EOLs across systems
OT::Bool GetLine(std::ifstream & file, OT::String & line)
{
  if (std::getline(file, line).fail())
    return false;
  // deal with DOS EOLs from UNIX
  if (line.size() && (line[line.size() - 1] == '\r'))
  {
    line.erase(line.size() - 1, 1);
  }
  return true;
}

}

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
  return !( lhs <= rhs );
}

bool operator <= (const NSI_point & lhs, const NSI_point & rhs)
{
  return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                      rhs.begin(), rhs.end(),
                                      std::less_equal<Scalar>());
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
  return !( lhs <= rhs );
}

bool operator <= (const NSI_const_point & lhs, const NSI_const_point & rhs)
{
  return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                      rhs.begin(), rhs.end(),
                                      std::less_equal<Scalar>());
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


/* Factory of SampleImplementation from CSV file */
SampleImplementation SampleImplementation::BuildFromCSVFile(const FileName & fileName,
    const String & csvSeparator)
{
  if (csvSeparator == " ") throw InvalidArgumentException(HERE) << "Error: the space separator is not compatible for CSV file.";



  SampleImplementation impl(0, 0);

#if defined(OPENTURNS_HAVE_BISON) && defined(OPENTURNS_HAVE_FLEX)

  FILE * theFile = std::fopen(fileName.c_str(), "r");
  if (!theFile)
  {
    // theFile can not be found. Errno is set
    throw FileNotFoundException(HERE) << "Can NOT open file '" << fileName
                                      << "'. Reason: " << std::strerror(errno);
  }

  impl.setName(fileName);
  yyscan_t scanner = 0;

  CSVParserState state;
  state.theFileName = fileName;

#ifdef OPENTURNS_HAVE_USELOCALE
  locale_t new_locale = newlocale (LC_NUMERIC_MASK, "C", NULL);
  locale_t old_locale = uselocale(new_locale);
#else
#ifdef WIN32
  _configthreadlocale(_ENABLE_PER_THREAD_LOCALE);
#endif
  const char * initialLocale = setlocale(LC_NUMERIC, NULL);
  setlocale(LC_NUMERIC, "C");
#endif

  csvlex_init(&scanner);
  csvparse(state, scanner, theFile, impl, impl.dimension_, csvSeparator.c_str());
  csvlex_destroy(scanner);

#ifdef OPENTURNS_HAVE_USELOCALE
  uselocale(old_locale);
  freelocale(new_locale);
#else
#ifdef WIN32
  _configthreadlocale(_DISABLE_PER_THREAD_LOCALE);
#endif
  setlocale(LC_NUMERIC, initialLocale);
#endif
  std::fclose(theFile);

  // Check the description
  if (impl.p_description_.isNull() || (impl.p_description_->getSize() != impl.getDimension()))
    impl.setDescription(Description::BuildDefault(impl.getDimension(), "data_"));
  if (impl.getDimension() == 0) LOGWARN(OSS() << "Warning: No data from the file has been stored.");

#else
  impl = SampleImplementation::BuildFromTextFile(fileName, csvSeparator);
#endif
  return impl;
}

/* Factory of SampleImplementation from TXT file
 * The file is supposed to be formated this way:
 * + an optional description of each column in the first row
 * + an arbitrary number of rows containing the same number of values
 * + the values can have an arbitrary number of spaces before or after them, with
 *   a potentially non-space separator between the values
 */
Bool SampleImplementation::ParseStringAsValues(const String & line,
    const char separator,
    Point & data)
{
  const Bool isBlankSeparator = separator == ' ';
  char * begin = const_cast<char *>(line.c_str());
  char * end = begin;
  data = Point(0);
  while (!(*end == '\0'))
  {
    const Scalar value = strtod(begin, &end);

    // Check if there was a problem
    if (begin == end)
    {
      data = Point(0);
      return false;
    }
    data.add(value);

    // eat blanks
    while(*end == ' ') ++end;

    if (!isBlankSeparator)
    {
      // Early exit to avoid going past the end of the line
      // when dealing with a non-blank separator
      if (*end == '\0') return true;
      if (*end != separator)
      {
        data = Point(0);
        return false;
      }
      // To skip the separator
      ++ end;
    }
    begin = end;
  }
  return true;
}


Bool SampleImplementation::ParseStringAsDescription(const String & line,
    const char separator,
    Description & description)
{
  // Here the speed is not critical at all

  description.clear();
  UnsignedInteger start = 0;
  UnsignedInteger len = 0;
  Bool escaped = false;

  for (UnsignedInteger i = 0; i < line.size(); ++ i)
  {
    if (line[i] == '\"')
    {
      if (!escaped)
        ++ start;
      escaped = !escaped;
    }
    else if ((line[i] == separator) && !escaped)
    {
      String field(line.substr(start, len));
      if (field.empty())
      {
        LOGINFO(OSS() << "empty component, description is ignored");
        return false;
      }
      description.add(field);
      start = i + 1;
      len = 0;
    }
    else
      ++ len;
  }
  if (len > 0)
  {
    String field(line.substr(start, len));
    if (field.empty())
    {
      LOGINFO(OSS() << "empty component, description is ignored");
      return false;
    }
    description.add(field);
  }
  return true;
}


Bool SampleImplementation::ParseComment(const String & line, const String & markers)
{
  Bool result = false;
  if (line.size() > 0)
  {
    const char firstChar = line[0];
    for (UnsignedInteger i = 0; i < markers.size(); ++ i)
    {
      if (firstChar == markers[i])
      {
        result = true;
        break;
      }
    }
  }
  return result;
}


SampleImplementation SampleImplementation::BuildFromTextFile(const FileName & fileName,
    const String & separator,
    const UnsignedInteger skippedLines)
{
  if (separator.size() != 1) throw InvalidArgumentException(HERE) << "Expected a separator with one character, got separator=" << separator;
  const char theSeparator = separator[0];

  const String commentMarkers(ResourceMap::GetAsString("Sample-CommentMarkers"));
  SampleImplementation impl(0, 0);

  std::ifstream theFile(fileName.c_str());

  if (!theFile.is_open())
  {
    // theFile can not be found. Errno is set
    throw FileNotFoundException(HERE) << "Can NOT open file '" << fileName
                                      << "'. Reason: " << std::strerror(errno);
  }

  // Manage the locale such that the decimal point IS a point ('.')
#ifdef OPENTURNS_HAVE_USELOCALE
  locale_t new_locale = newlocale (LC_NUMERIC_MASK, "C", NULL);
  locale_t old_locale = uselocale(new_locale);
#else
#ifdef WIN32
  _configthreadlocale(_ENABLE_PER_THREAD_LOCALE);
#endif
  const char * initialLocale = setlocale(LC_NUMERIC, NULL);
  setlocale(LC_NUMERIC, "C");
#endif

  String line;
  UnsignedInteger index = 1;

  // skip N lines
  for ( ; index <= skippedLines; ++ index)
    GetLine(theFile, line);

  // Go to the first line containing data
  Point data;
  Description description(0);
  while (!impl.dimension_ && GetLine(theFile, line))
  {
    if (line.empty())
    {
      LOGWARN(OSS() << "The line number " << index << " is empty");
      LOGDEBUG(OSS() << "line=" << line);
    }
    else if (ParseComment(line, commentMarkers))
    {
      LOGWARN(OSS() << "The line number " << index << " contains a comment");
      LOGDEBUG(OSS() << "line=" << line);
    }
    else if (ParseStringAsValues(line, theSeparator, data))
    {
      // The dimension is given by the first row of values
      impl.dimension_ = data.getDimension();
      impl.data_.add(data);
      ++ impl.size_;
      LOGDEBUG(OSS() << "The line number " << index << " contains data");
    }
    else if (ParseStringAsDescription(line, theSeparator, description))
    {
      // The dimension is given by the description
      impl.dimension_ = description.getSize();
      impl.setDescription(description);
      LOGDEBUG(OSS() << "The line number " << index << " contains a description");
    }
    ++ index;
  }

  // Now, read all the other rows. If they don't contain a point
  // or if the number of values is not equal to the dimension the
  // row is ignored
  while (GetLine(theFile, line))
  {
    if (ParseComment(line, commentMarkers))
    {
      LOGWARN(OSS() << "The line number " << index << " contains a comment");
      LOGDEBUG(OSS() << "line=" << line);
    }
    else
    {
      if (ParseStringAsValues(line, theSeparator, data))
      {
        if (data.getDimension() == impl.dimension_)
        {
          impl.data_.add(data);
          ++impl.size_;
        }
        else
        {
          LOGWARN(OSS() << "The line number " << index << " has a dimension=" << data.getDimension() << ", expected dimension=" << impl.dimension_);
          LOGDEBUG(OSS() << "line=" << line);
        }
      }
      else
      {
        LOGWARN(OSS() << "The line number " << index << " does not contain a point");
        LOGDEBUG(OSS() << "line=" << line);
      }
    }
    ++ index;
  } // while (std::getLine(theFile, line))
  // Check the description
  if (impl.p_description_.isNull() || (impl.p_description_->getSize() != impl.getDimension()))
  {
    impl.setDescription(Description::BuildDefault(impl.getDimension(), "data_"));
  }
  if (impl.getDimension() == 0) LOGWARN(OSS() << "Warning: No data from the file has been stored.");
#ifdef OPENTURNS_HAVE_USELOCALE
  uselocale(old_locale);
  freelocale(new_locale);
#else
#ifdef WIN32
  _configthreadlocale(_DISABLE_PER_THREAD_LOCALE);
#endif
  setlocale(LC_NUMERIC, initialLocale);
#endif
  impl.setName(fileName);
  return impl;
}

/* Store a sample in a temporary text file, one realization by line. Returns the file name. */
String SampleImplementation::storeToTemporaryFile() const
{
  const String dataFileName(Path::BuildTemporaryFileName("RData.txt.XXXXXX"));
  std::ofstream dataFile(dataFileName.c_str());
  dataFile << std::setprecision(16);
  // Fill-in the data file
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    Scalar value = data_[index];
    ++index;
    Bool isNaN = value != value;
    if (isNaN) dataFile << '\"' << value << '\"';
    else dataFile << value;
    for (UnsignedInteger j = 1; j < dimension_; ++j)
    {
      value = data_[index];
      ++index;
      isNaN = value != value;
      if (isNaN) dataFile << ' ' << '\"' << value << '\"';
      else dataFile << ' ' << value;
    }
    dataFile << "\n";
  }
  dataFile.close();
  return dataFileName;
}

/* Export a sample as a matrix, one row by realization, in a format suitable to exchange with R */
String SampleImplementation::streamToRFormat() const
{
  OSS oss;
  oss.setPrecision(16);
  oss << "matrix(c(";
  String separator("");
  for (UnsignedInteger j = 0; j < dimension_; ++j)
  {
    UnsignedInteger index = j;
    for (UnsignedInteger i = 0; i < size_; ++i, separator = ",")
    {
      const Scalar value = data_[index];
      index += dimension_;
      const Bool isNaN = value != value;
      oss << separator << (isNaN ? "\"" : "") << value << (isNaN ? "\"" : "");
    }
  }
  oss << "), nrow=" << size_ << ", ncol=" << dimension_ << ")";
  return oss;
}

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


/* Virtual constructor */
SampleImplementation * SampleImplementation::clone() const
{
  return new SampleImplementation(*this);
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
  PersistentCollection<Scalar>::iterator f = data_.begin() + (first - begin()) * dimension_;
  PersistentCollection<Scalar>::iterator l = data_.begin() + (last - begin()) * dimension_;
  data_.erase( f, l );
  size_ -= last - first;
}

void SampleImplementation::erase(const UnsignedInteger first,
                                 const UnsignedInteger last)
{
  PersistentCollection<Scalar>::iterator f = data_.begin() + first * dimension_;
  PersistentCollection<Scalar>::iterator l = data_.begin() + last * dimension_;
  data_.erase( f, l );
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
  return data_;
}

void SampleImplementation::setData(const Collection<Scalar> & data)
{
  if (data.getSize() != dimension_ * size_) throw InvalidArgumentException(HERE) << "Error: the given raw data are not compatible with the dimension and size of the sample.";
  data_ = data;
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
    oss << " ]" << Os::GetEndOfLine() << offset;
  }

  const char * newline = "";
  for( UnsignedInteger i = 0; i < size_; ++i, newline = Os::GetEndOfLine() )
  {
    if (ellipsis && (size_ > 2 * printEllipsisSize))
    {
      if (i == printEllipsisSize)
      {
        oss << Os::GetEndOfLine() << offset <<  "...";
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
  ++ size_;
  data_.resize(size_ * dimension_);
  std::copy(point.begin(), point.begin() + dimension_, data_.begin() + oldSize * dimension_);
  return *this;
}


/* Appends another sample to the collection */
SampleImplementation & SampleImplementation::add(const SampleImplementation & sample)
{
  if (sample.getDimension() != dimension_)
    throw InvalidArgumentException(HERE) << "Sample has invalid dimension ("
                                         << sample.getDimension()
                                         << ") expected : "
                                         << getDimension();
  //const UnsignedInteger oldSize = size_;
  size_ += sample.getSize();
  data_.resize(size_ * dimension_);
//   memmove( &data_[oldSize * dimension_], &(sample.data_[0]), sample.getSize() * dimension_ * sizeof(Scalar) );
  std::copy_backward(sample.begin(), sample.end(), end());
  return *this;
}

/* Stack the given sample to the current one */
SampleImplementation & SampleImplementation::stack(const SampleImplementation & sample)
{
  if (sample.getSize() != size_) throw InvalidArgumentException(HERE) << "Error: the given sample has size=" << sample.getSize() << ", expected size=" << size_;
  const UnsignedInteger otherDimension = sample.getDimension();
  SampleImplementation result(size_, dimension_ + otherDimension);
  // First, the values
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension_; ++j) result(i, j) = operator()(i, j);
    for (UnsignedInteger j = 0; j < otherDimension; ++j) result(i, dimension_ + j) = sample(i, j);
  }
  // Second, the description
  if (!p_description_.isNull() || !sample.p_description_.isNull())
  {
    Description description(getDescription());
    const Description otherDescription(sample.getDescription());
    for (UnsignedInteger i = 0; i < otherDimension; ++i) description.add(otherDescription[i]);
    result.setDescription(description);
  }
  *this = result;
  return *this;
}

template <typename OP>
struct ReductionFunctor
{
  const SampleImplementation & nsi_;
  const OP & op_;
  typename OP::value_type accumulator_;

  ReductionFunctor(const SampleImplementation & nsi, const OP & op = OP())
    : nsi_(nsi), op_(op), accumulator_(OP::GetInvariant(nsi_)) {}

  ReductionFunctor(const ReductionFunctor & other, TBB::Split)
    : nsi_(other.nsi_), op_(other.op_), accumulator_(OP::GetInvariant(nsi_)) {}

  void operator() (const TBB::BlockedRange<UnsignedInteger> & r)
  {
    NSI_const_iterator it = nsi_.begin() + r.begin();
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i, ++it) op_.inplace_op( accumulator_, *it );
  }

  void join(const ReductionFunctor & other)
  {
    op_.inplace_op( accumulator_, other.accumulator_ );
  }

}; /* end struct ReductionFunctor */


template <typename OP>
class ParallelFunctor
{
  SampleImplementation & nsi_;
  const OP & op_;
public:
  ParallelFunctor(SampleImplementation & nsi, const OP & op) : nsi_(nsi), op_(op) {}

  void operator() (const TBB::BlockedRange<UnsignedInteger> & r) const
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
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the mean of an empty sample.";
  Point accumulated(dimension_);

  data_const_iterator it(data_begin());
  const data_const_iterator guard(data_end());
  while (it != guard)
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i, ++it)
    {
      accumulated[i] += *it;
    }
  }

  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    accumulated[i] *= (1.0 / size_);
  }
  return accumulated;
}

/*
 * Gives the covariance matrix of the sample, normalization by 1 / (size - 1) if size > 1
 */
CovarianceMatrix SampleImplementation::computeCovariance() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the covariance of an empty sample.";
  // Special case for a sample of size 1
  if (size_ == 1) return CovarianceMatrix(dimension_, Point(dimension_ * dimension_));

  const Point mean(computeMean());
  const UnsignedInteger squaredDim(dimension_ * dimension_);
  Point accumulated(squaredDim);

  data_const_iterator it(data_begin());
  const data_const_iterator guard(data_end());
  while (it != guard)
  {
    UnsignedInteger baseIndex = 0;
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      const Scalar deltaI = *(it + i) - mean[i];
      for (UnsignedInteger j = i; j < dimension_; ++j)
      {
        const Scalar deltaJ = *(it + j) - mean[j];
        accumulated[baseIndex + j] += deltaI * deltaJ;
      }
      baseIndex += dimension_;
    }
    it += dimension_;
  }

  for (UnsignedInteger i = 0; i < squaredDim; ++i)
  {
    accumulated[i] /= (size_ - 1);
  }
  CovarianceMatrix result(dimension_, accumulated);
  return result;
}

/*
 * Gives the standard deviation of the sample, i.e. the square-root of the covariance matrix.
 */
TriangularMatrix SampleImplementation::computeStandardDeviation() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the standard deviation of an empty sample.";
  return computeCovariance().computeCholesky();
}


/*
 * Gives the variance of the sample (by component)
 */
Point SampleImplementation::computeVariance() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the variance per component of an empty sample.";

  // Special case for a sample of size 1
  if (size_ == 1) return Point(dimension_, 0.0);

  const Point mean( computeMean() );
  Point accumulated(dimension_);

  data_const_iterator it(data_begin());
  const data_const_iterator guard(data_end());
  while (it != guard)
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i, ++it)
    {
      const Scalar val = *it - mean[i];
      accumulated[i] += val * val;
    }
  }

  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    accumulated[i] /= (size_ - 1);
  }
  return accumulated;

}

/*
 * Gives the standard deviation of each component of the sample
 */
Point SampleImplementation::computeStandardDeviationPerComponent() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the standard deviation per component of an empty sample.";
  Point sd(computeVariance());
  for (UnsignedInteger i = 0; i < dimension_; ++i) sd[i] = sqrt(sd[i]);
  return sd;
}



/*
 * Gives the Pearson correlation matrix of the sample
 */
CorrelationMatrix SampleImplementation::computePearsonCorrelation() const
{
  return computeLinearCorrelation();
}

CorrelationMatrix SampleImplementation::computeLinearCorrelation() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the Pearson correlation of an empty sample.";
  CorrelationMatrix correlation(dimension_);
  if (dimension_ == 1) return correlation;

  const CovarianceMatrix covariance(computeCovariance());
  Point sd(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    sd[i] = sqrt( covariance(i, i) );
    if (sd[i] != 0.0)
      for (UnsignedInteger j = 0; j < i; ++j)
        if (sd[j] != 0.0) correlation(i, j) = covariance(i, j) / (sd[i] * sd[j]);
  }

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
  // True if sample has ties
  mutable Bool hasTies_;

  Comparison(UnsignedInteger first,
             const SampleImplementation & nsi)
    : first_(first), second_(first), nsi_(nsi), permutation_(nsi_.getSize()), hasTies_(false)
  {
    permutation_.fill();
  }

  Comparison(UnsignedInteger first, UnsignedInteger second,
             const SampleImplementation & nsi)
    : first_(first), second_(second), nsi_(nsi), permutation_(nsi_.getSize()), hasTies_(false)
  {
    permutation_.fill();
  }

  Bool operator() (const UnsignedInteger i, const UnsignedInteger j) const
  {
    const Scalar xI = nsi_( permutation_[i], first_  );
    const Scalar xJ = nsi_( permutation_[j], first_  );
    const Scalar yI = nsi_( permutation_[i], second_ );
    const Scalar yJ = nsi_( permutation_[j], second_ );
    hasTies_ |= (xI == xJ);
    return ( (xI < xJ) || ((xI == xJ) && (yI < yJ)) );
  }
}; // struct Comparison




/* Ranked sample */
SampleImplementation SampleImplementation::rank() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot rank an empty sample.";
  SampleImplementation rankedSample(size_, dimension_);

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
    TBB::ParallelSort(sortedMarginalSamples.begin(), sortedMarginalSamples.end());
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
        for (UnsignedInteger k = lastIndex; k < j; ++k) rankedSample(sortedMarginalSamples[k].index_, i) = rankValue;
        lastIndex = j;
        lastValue = currentValue;
      }
    }
    // If we end with a constant range
    if (currentValue == lastValue)
    {
      const Scalar rankValue = 0.5 * (lastIndex + size_ - 1);
      for (UnsignedInteger k = lastIndex; k < size_; ++k) rankedSample(sortedMarginalSamples[k].index_, i) = rankValue;
    }
  }
  if (!p_description_.isNull()) rankedSample.setDescription(getDescription());
  return rankedSample;
}

/* Ranked component */
SampleImplementation SampleImplementation::rank(const UnsignedInteger index) const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot rank an empty sample.";
  if (index >= dimension_) throw OutOfBoundException(HERE) << "The requested index is too large, index=" << index << ", dimension=" << dimension_;
  return getMarginal(index).rank();
}

struct NSI_Sortable
{
  const SampleImplementation * p_nsi_;
  UnsignedInteger index_;
  NSI_Sortable(const SampleImplementation * nsi, UnsignedInteger index) : p_nsi_(nsi), index_(index) {}
  NSI_Sortable() : p_nsi_(NULL), index_(0) {}
  Bool operator < (const NSI_Sortable & other) const
  {
    return p_nsi_->operator[](index_) < other.p_nsi_->operator[](other.index_);
  }
  operator NSI_const_point() const
  {
    return NSI_const_point(p_nsi_, index_);
  }
  inline UnsignedInteger getDimension() const
  {
    return p_nsi_->getDimension();
  }

  inline NSI_const_point::const_iterator begin()
  {
    return &NSI_const_point(p_nsi_, index_).operator[](0);
  }
  inline NSI_const_point::const_iterator end()
  {
    return &NSI_const_point(p_nsi_, index_).operator[](0) + getDimension();
  }

};

/* Sorted sample, component by component */
SampleImplementation SampleImplementation::sort() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot sort an empty sample.";

  SampleImplementation sortedSample(size_, dimension_);
  // Special case for 1D sample
  if (dimension_ == 1)
  {
    Point sortedData(data_);
    TBB::ParallelSort(sortedData.begin(), sortedData.end());
    sortedSample.setData(sortedData);
    return sortedSample;
  }
  // The nD samples
  Collection<NSI_Sortable> sortables(size_);
  for (UnsignedInteger i = 0; i < size_; ++i) sortables[i] = NSI_Sortable(this, i);
  TBB::ParallelSort(sortables.begin(), sortables.end());
  for (UnsignedInteger i = 0; i < size_; ++i) sortedSample[i] = sortables[i];
  if (!p_description_.isNull()) sortedSample.setDescription(getDescription());
  return sortedSample;
}

void SampleImplementation::sortInPlace()
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot sort an empty sample.";

  // Special case for 1D sample
  if (dimension_ == 1)
  {
    TBB::ParallelSort(data_.begin(), data_.end());
    return;
  }
  // The nD samples
  Collection<NSI_Sortable> sortables(size_);
  SampleImplementation work(*this);
  for (UnsignedInteger i = 0; i < size_; ++i) sortables[i] = NSI_Sortable(&work, i);
  TBB::ParallelSort(sortables.begin(), sortables.end());
  for (UnsignedInteger i = 0; i < size_; ++i) (*this)[i] = sortables[i];
}

/* Sorted sample, one component */
SampleImplementation SampleImplementation::sort(const UnsignedInteger index) const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot sort an empty sample.";

  if (index >= getDimension()) throw OutOfBoundException(HERE) << "The requested index is too large, index=" << index << ", dimension=" << getDimension();

  return getMarginal(index).sort();
}


struct Sortable
{
  Point values_;
  UnsignedInteger index_;
  Sortable() : values_(1, 0.0), index_(0) {}
  Sortable(const Point & values, const UnsignedInteger index) : values_(values), index_(index) {}
  Bool operator < (const Sortable & other) const
  {
    return values_[index_] < other.values_[other.index_];
  }
};

/* Sorted according a component */
SampleImplementation SampleImplementation::sortAccordingToAComponent(const UnsignedInteger index) const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot sort an empty sample.";
  if (index >= getDimension()) throw OutOfBoundException(HERE) << "The requested index is too large, index=" << index << ", dimension=" << getDimension();

  Collection<Sortable> sortables(size_);
  for (UnsignedInteger i = 0; i < size_; ++i) sortables[i] = Sortable((*this)[i], index);
  TBB::ParallelSort(sortables.begin(), sortables.end());
  SampleImplementation sortedSample(size_, dimension_);
  UnsignedInteger shift = 0;
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    std::copy(sortables[i].values_.begin(), sortables[i].values_.end(), sortedSample.data_.begin() + shift);
    shift += dimension_;
  }
  if (!p_description_.isNull()) sortedSample.setDescription(getDescription());
  return sortedSample;
}

/* Sorted according a component */
void SampleImplementation::sortAccordingToAComponentInPlace(const UnsignedInteger index)
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot sort an empty sample.";
  if (index >= getDimension()) throw OutOfBoundException(HERE) << "The requested index is too large, index=" << index << ", dimension=" << getDimension();

  Collection<Sortable> sortables(size_);
  SampleImplementation work(*this);
  for (UnsignedInteger i = 0; i < size_; ++i) sortables[i] = Sortable(work[i], index);
  TBB::ParallelSort(sortables.begin(), sortables.end());
  UnsignedInteger shift = 0;
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    std::copy(sortables[i].values_.begin(), sortables[i].values_.end(), data_.begin() + shift);
    shift += dimension_;
  }
}

/* Sort and remove duplicated points */
SampleImplementation SampleImplementation::sortUnique() const
{
  SampleImplementation sampleSorted(sort());
  SampleImplementation sampleUnique(size_, dimension_);
  sampleUnique[0] = sampleSorted[0];
  UnsignedInteger last = 0;
  for (UnsignedInteger i = 1; i < size_; ++i)
  {
    if (sampleSorted[i] != sampleUnique[last])
    {
      ++last;
      sampleUnique[last] = sampleSorted[i];
    }
  }
  if (last + 1 < size_) sampleUnique.erase(last + 1, size_);
  if (!p_description_.isNull()) sampleUnique.setDescription(getDescription());
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

/*
 * Gives the Spearman correlation matrix of the sample
 */
CorrelationMatrix SampleImplementation::computeSpearmanCorrelation() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the Spearman correlation of an empty sample.";

  return rank().computePearsonCorrelation();
}


/*
 * Gives the Kendall tau matrix of the sample, including ties correction
 */

struct ComputeKendallPolicy
{
  const SampleImplementation & input_;
  Point & output_;
  const Indices & indicesX_;
  const Indices & indicesY_;
  const Bool smallCase_;

  ComputeKendallPolicy( const SampleImplementation & input,
                        Point & output,
                        const Indices & indicesX,
                        const Indices & indicesY,
                        const Bool smallCase)
    : input_(input)
    , output_(output)
    , indicesX_(indicesX)
    , indicesY_(indicesY)
    , smallCase_(smallCase)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    const UnsignedInteger size = input_.getSize();
    Point x(size);
    Point y(size);
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const UnsignedInteger indX = indicesX_[i];
      const UnsignedInteger indY = indicesY_[i];
      for (UnsignedInteger k = 0; k < size; ++k)
      {
        x[k] = input_(k, indX);
        y[k] = input_(k, indY);
      }
      if (smallCase_) output_[i] = kendallSmallN(&x[0], &y[0], size);
      else output_[i] = kendallNlogN(&x[0], &y[0], size);
    }
  }

}; /* end struct ComputeKendallPolicy */

CorrelationMatrix SampleImplementation::computeKendallTau() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the Kendall tau of an empty sample.";

  // Use external efficient C implementation of the O(Nlog(N)) or O(N^2) Kendall tau computation depending on the sample size
  const Bool smallCase = size_ < ResourceMap::GetAsUnsignedInteger("Sample-SmallKendallTau");
  const UnsignedInteger caseNumber = (dimension_ * (dimension_ - 1)) / 2;
  Indices indX(caseNumber);
  Indices indY(caseNumber);
  // The resulting matrix
  CorrelationMatrix tau(dimension_);
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < dimension_ - 1; ++i)
  {
    for (UnsignedInteger j = i + 1; j < dimension_; ++j)
    {
      LOGINFO(OSS() << "in SampleImplementation::computeKendallTau(), computing tau[" << i << ", " << j << "]");
      indX[index] = i;
      indY[index] = j;
      ++index;
    } // end for j
  } // end for i
  // Now the computation
  Point result(caseNumber);
  const ComputeKendallPolicy policy( *this, result, indX, indY, smallCase );
  TBB::ParallelFor( 0, caseNumber, policy );
  index = 0;
  for (UnsignedInteger i = 0; i < dimension_ - 1; ++i)
  {
    for (UnsignedInteger j = i + 1; j < dimension_; ++j)
    {
      tau(i, j) = result[index];
      ++index;
    } // end for j
  } // end for i

  return tau;
}

/*
 * Gives the range of the sample (by component)
 */
Point SampleImplementation::computeRange() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the range per component of an empty sample.";
  return getMax() - getMin();
}

/*
 * Gives the median of the sample (by component)
 */
Point SampleImplementation::computeMedian() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the median per component of an empty sample.";
  return computeQuantilePerComponent(0.5);
}

/*
 * Gives the skewness of the sample (by component)
 */
Point SampleImplementation::computeSkewness() const
{
  if (size_ < 2) throw InternalException(HERE) << "Error: cannot compute the skewness per component of a sample of size less than 2.";

  if (size_ == 2) return Point(dimension_, 0.0);

  const Point mean(computeMean());
  Point centeredMoments(2 * dimension_);
  for(UnsignedInteger i = 0; i < size_; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension_; ++j)
    {
      const Scalar val = operator()(i, j) - mean[j];
      const Scalar val2 = val * val;
      centeredMoments[j] += val2;
      centeredMoments[j + dimension_] += val2 * val;
    }
  }
  Point skewness(dimension_);
  const Scalar factor = size_ * sqrt(size_ - 1.0) / (size_ - 2);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    if (centeredMoments[i] == 0.0) throw NotDefinedException(HERE) << "Error: the sample has component " << i << " constant. The skewness is not defined.";
    skewness[i] = factor * centeredMoments[i + dimension_] / pow(centeredMoments[i], 1.5);
  }
  return skewness;
}

/*
 * Gives the kurtosis of the sample (by component)
 */
Point SampleImplementation::computeKurtosis() const
{
  if (size_ < 3) throw InternalException(HERE) << "Error: cannot compute the kurtosis per component of a sample of size less than 3.";

  if (size_ == 3) return Point(dimension_, 0.0);

  const Point mean(computeMean());
  Point centeredMoments(2 * dimension_);
  for(UnsignedInteger i = 0; i < size_; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension_; ++j)
    {
      const Scalar val = operator()(i, j) - mean[j];
      const Scalar val2 = val * val;
      centeredMoments[j] += val2;
      centeredMoments[j + dimension_] += val2 * val2;
    }
  }
  Point kurtosis(dimension_);
  const Scalar factor1 = (size_ + 1.0) * size_ * (size_ - 1.0) / ((size_ - 2.0) * (size_ - 3.0));
  const Scalar factor2 = -3.0 * (3.0 * size_ - 5.0) / ((size_ - 2.0) * (size_ - 3.0));
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    if (centeredMoments[i] == 0.0) throw NotDefinedException(HERE) << "Error: the sample has component " << i << " constant. The kurtosis is not defined.";
    kurtosis[i] = factor1 * centeredMoments[i + dimension_] / (centeredMoments[i] * centeredMoments[i]) + factor2;
  }
  return kurtosis;
}

/*
 * Gives the centered moment of order k of the sample (by component)
 */
Point SampleImplementation::computeCenteredMoment(const UnsignedInteger k) const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the centered moments per component of an empty sample.";

  // Special case: order 0, return (1,...,1)
  if (k == 0) return Point(dimension_, 1.0);
  // Special case: order 1, return (0,...,0)
  if (k == 1) return Point(dimension_, 0.0);
  // Special case: order 2, return biased variance estimator
  if (k == 2) return computeVariance() * (size_ - 1.0) / size_;
  // General case
  const Point mean(computeMean());

  Point centeredMoment(dimension_);
  const int intK = static_cast<int>(k);
  for(UnsignedInteger i = 0; i < size_; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension_; ++j)
    {
      const Scalar val = operator()(i, j) - mean[j];
      centeredMoment[j] += pow(val, intK);
    }
  }
  return centeredMoment / size_;
}

/*
 * Gives the raw moment of order k of the sample (by component)
 */
Point SampleImplementation::computeRawMoment(const UnsignedInteger k) const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the centered moments per component of an empty sample.";

  if (size_ == 0) throw InvalidArgumentException(HERE) << "Cannot compute centered moments on an empty sample";

  // Special case: order 0, return (size,...,size)
  if (k == 0) return Point(dimension_, 1.0);

  Point moment(dimension_);
  const int intK = static_cast<int>(k);
  for(UnsignedInteger i = 0; i < size_; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension_; ++j)
    {
      moment[j] += pow(operator()(i, j), intK);
    }
  }
  return moment / size_;
}

/*
 * Gives the quantile per component of the sample
 */
Point SampleImplementation::computeQuantilePerComponent(const Scalar prob) const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the quantile per component of an empty sample.";
  if (!(prob >= 0.0) || !(prob <= 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a quantile for a probability level outside of [0, 1]";

  // Special case for extremum cases
  const Scalar scalarIndex = prob * size_ - 0.5;
  if (scalarIndex >= size_ - 1) return getMax();
  // Special case for prob <= 0.0
  if (scalarIndex <= 0.0) return getMin();

  const UnsignedInteger index = static_cast<UnsignedInteger>( floor( scalarIndex) );
  const Scalar beta = scalarIndex - index;
  const Scalar alpha = 1.0 - beta;
  Point quantile(dimension_);
  Point component(size_);
  for (UnsignedInteger j = 0; j < dimension_; ++j)
  {
    for (UnsignedInteger i = 0; i < size_; ++i)
      component[i] = operator()(i, j);

    // Find index-th and (index+1)-th elements.
    // We call nth_element twice, and select the right order so that second
    // call operates on the smallest subset.
    if (2 * index > size_)
    {
      std::nth_element(component.begin(), component.begin() + index, component.end());
      std::nth_element(component.begin() + index, component.begin() + index + 1, component.end());
    }
    else
    {
      std::nth_element(component.begin(), component.begin() + index + 1, component.end());
      std::nth_element(component.begin(), component.begin() + index, component.begin() + index + 1);
    }
    // Interpolation between the two adjacent empirical quantiles
    quantile[j] = alpha * component[index] + beta * component[index + 1];
  } // end for

  return quantile;
}

/*
 * Gives the quantile per component of the sample
 */
SampleImplementation SampleImplementation::computeQuantilePerComponent(const Point & prob) const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the quantile per component of an empty sample.";
  const UnsignedInteger probSize = prob.getSize();
  if (probSize == 0) throw InternalException(HERE) << "Error: cannot compute the quantile per component with an empty argument.";

  // Check that prob is inside bounds
  for (UnsignedInteger p = 0; p < probSize; ++p)
    if (!(prob[p] >= 0.0) || !(prob[p] <= 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a quantile for a probability level outside of [0, 1]";

  // Sort prob in ascending order
  Collection< std::pair<Scalar, UnsignedInteger> > probPairs(probSize);
  for (UnsignedInteger i = 0; i < probSize; ++i)
    probPairs[i] = std::pair<Scalar, UnsignedInteger>(prob[i], i);
  std::sort(probPairs.begin(), probPairs.end());
  Bool sorted = true;
  Indices indices(probSize);
  for (UnsignedInteger i = 0; i < probSize; ++i)
  {
    if (probPairs[i].second != i) sorted = false;
    indices[probPairs[i].second] = i;
  }

  // Compute the list of pivots and coefficients
  Indices pivots(probSize);
  Point betas(probSize);
  for (UnsignedInteger p = 0; p < probSize; ++p)
  {
    const Scalar scalarIndex = probPairs[p].first * size_ - 0.5;
    UnsignedInteger index = static_cast<UnsignedInteger>( floor( scalarIndex) );
    Scalar beta = scalarIndex - index;
    // Special case for extremum cases
    if (scalarIndex >= size_ - 1)
    {
      beta = 0.0;
      index = size_ - 1;
    }
    else if (scalarIndex <= 0.0)
    {
      beta = 0.0;
      // Ensure that index does not overflow
      index = 0;
    }
    pivots[p] = index;
    betas[p] = beta;
  }

  SampleImplementation quantile(probSize, dimension_);
  quantile.setDescription(Description::BuildDefault(dimension_, "q"));
  Point component(size_);
  for (UnsignedInteger j = 0; j < dimension_; ++j)
  {
    for (UnsignedInteger i = 0; i < size_; ++i)
      component[i] = operator()(i, j);

    UnsignedInteger lastIndex = 0;
    for (UnsignedInteger p = 0; p < probSize; ++p)
    {
      // Find index-th and (index+1)-th elements.
      // We call nth_element twice, and select the right order so that second
      // call operates on the smallest subset.
      const UnsignedInteger index = pivots[p];
      const Scalar beta = betas[p];
      const Scalar alpha = 1.0 - beta;
      if (beta == 0.0)
      {
        // We use a special case here to avoid using an indefinite value if index is the last element
        std::nth_element(component.begin() + lastIndex, component.begin() + index, component.end());
        quantile(p, j) = component[index];
      }
      else if (lastIndex == index && p > 0)
      {
        // Same index, but alpha and beta may have changed
        quantile(p, j) = alpha * component[index] + beta * component[index + 1];
      }
      else if (2 * index > size_ + lastIndex)
      {
        std::nth_element(component.begin() + lastIndex, component.begin() + index, component.end());
        std::nth_element(component.begin() + index, component.begin() + index + 1, component.end());
        // Interpolation between the two adjacent empirical quantiles
        quantile(p, j) = alpha * component[index] + beta * component[index + 1];
      }
      else
      {
        std::nth_element(component.begin() + lastIndex, component.begin() + index + 1, component.end());
        std::nth_element(component.begin() + lastIndex, component.begin() + index, component.begin() + index + 1);
        // Interpolation between the two adjacent empirical quantiles
        quantile(p, j) = alpha * component[index] + beta * component[index + 1];
      }
      lastIndex = index;
    }
  }

  return sorted ? quantile : quantile.select(indices);
}

/*
 * Gives the N-dimension quantile of the sample
 */
Point SampleImplementation::computeQuantile(const Scalar prob) const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the quantile of an empty sample.";

  if (getDimension() == 1) return computeQuantilePerComponent(prob);
  throw NotYetImplementedException(HERE) << "In SampleImplementation::computeQuantile(const Scalar prob) const";
}

SampleImplementation SampleImplementation::computeQuantile(const Point & prob) const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the quantile of an empty sample.";

  if (getDimension() == 1) return computeQuantilePerComponent(prob);
  throw NotYetImplementedException(HERE) << "In SampleImplementation::computeQuantile(const Point & prob) const";
}

struct CDFPolicy
{
  typedef Scalar value_type;

  const Point & point_;
  const Bool tail_;
  const UnsignedInteger dimension_;

  CDFPolicy( const SampleImplementation & nsi,
             const Point & point,
             const Bool tail )
    : point_(point), tail_(tail), dimension_(nsi.getDimension()) {}

  static inline value_type GetInvariant(const SampleImplementation &)
  {
    return value_type(0);
  }

  inline value_type & inplace_op( value_type & a,
                                  const value_type & other ) const
  {
    return a += other;
  }

  template <typename T>
  inline value_type & inplace_op( value_type & a,
                                  const T & pt ) const
  {
    UnsignedInteger j = 0;
    while ( (j < dimension_) && (tail_ ^ (pt[j] <= point_[j])) ) ++j;
    if (j == dimension_) ++a;
    return a;
  }

}; /* end struct CDFPolicy */

/*
 * Get the empirical CDF of the sample
 */
Scalar SampleImplementation::computeEmpiricalCDF(const Point & point,
    const Bool tail) const
{
  if (size_ == 0) throw InvalidArgumentException(HERE) << "Cannot compute the empirical CDF of an empty sample.";
  if (getDimension() != point.getDimension()) throw InvalidArgumentException(HERE) << "Point has incorrect dimension. Got "
        << point.getDimension() << ". Expected " << getDimension();

  const CDFPolicy policy( *this, point, tail );
  ReductionFunctor<CDFPolicy> functor( *this, policy );
  TBB::ParallelReduce( 0, size_, functor );
  return static_cast < Scalar > (functor.accumulator_) / size_;
}


/* Maximum accessor */
Point SampleImplementation::getMax() const
{
  if (size_ == 0) throw InternalException(HERE) << "Impossible to get the maximum of an empty Sample";

  Point maxPoint(dimension_, - SpecFunc::MaxScalar);
  for(UnsignedInteger i = 0; i < size_; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension_; ++j)
    {
      const Scalar val = operator()(i, j);
      if (val > maxPoint[j])
        maxPoint[j] = val;
    }
  }
  return maxPoint;
}

/* Minimum accessor */
Point SampleImplementation::getMin() const
{
  if (size_ == 0) throw InternalException(HERE) << "Impossible to get the minimum of an empty Sample";

  Point minPoint(dimension_, SpecFunc::MaxScalar);
  for(UnsignedInteger i = 0; i < size_; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension_; ++j)
    {
      const Scalar val = operator()(i, j);
      if (val < minPoint[j])
        minPoint[j] = val;
    }
  }
  return minPoint;
}


/*
 * Translate realizations in-place
 */
void SampleImplementation::translate(const Point & translation)
{
  if (dimension_ != translation.getDimension())
    throw InvalidArgumentException(HERE) << "Translation point has incorrect dimension. Got " << translation.getDimension()
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
  if (translation.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the dimension of the given translation=" << translation.getDimension() << " does not match the dimension of the sample=" << dimension_;
  if (translation.getSize() != size_) throw InvalidArgumentException(HERE) << "Error: the size of the given translation=" << translation.getSize() << " does not match the size of the sample=" << size_;
  int size(size_ * dimension_);
  double alpha(1.0);
  int one(1);
  daxpy_(&size, &alpha, const_cast<double*>(&((translation)(0, 0))), &one, &((*this)(0, 0)), &one);
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
  if (translation.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the dimension of the given translation=" << translation.getDimension() << " does not match the dimension of the sample=" << dimension_;
  if (translation.getSize() != size_) throw InvalidArgumentException(HERE) << "Error: the size of the given translation=" << translation.getSize() << " does not match the size of the sample=" << size_;
  for(UnsignedInteger i = 0; i < size_; ++i)
    for (UnsignedInteger j = 0; j < dimension_; ++j)
      operator()(i, j) -= translation(i, j);
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
    if (scaling[i] == 0.0) throw InvalidArgumentException(HERE) << "Error: the scaling must have nonzero components, here scaling=" << scaling;
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
SampleImplementation SampleImplementation::getMarginal(const UnsignedInteger index) const
{
  if (index >= dimension_) throw InvalidArgumentException(HERE) << "The index of a marginal sample must be in the range [0, dim-1]";

  // Special case for dimension 1
  if (dimension_ == 1) return *this;

  // General case
  SampleImplementation marginalSample(size_, 1);

  // If the sample has a description, extract the marginal description
  if (!p_description_.isNull())
    marginalSample.setDescription(Description(1, getDescription()[index]));
  for (UnsignedInteger i = 0; i < size_; ++i)
    marginalSample(i, 0) = operator()(i, index);

  return marginalSample;
}

/* Get the marginal sample corresponding to indices dimensions */
SampleImplementation SampleImplementation::getMarginal(const Indices & indices) const
{
  if (!indices.check(dimension_)) throw InvalidArgumentException(HERE) << "The indices of a marginal sample must be in the range [0, dim-1] and must be different";

  // Special case for dimension 1
  if (dimension_ == 1) return *this;

  // General case
  const UnsignedInteger outputDimension = indices.getSize();
  SampleImplementation marginalSample(size_, outputDimension);

  // If the sample has a description, extract the marginal description
  if (!p_description_.isNull())
  {
    const Description description(getDescription());
    Description marginalDescription(outputDimension);
    for (UnsignedInteger i = 0; i < outputDimension; ++ i)
      marginalDescription[i] = description[indices[i]];
    marginalSample.setDescription(marginalDescription);
  }

  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
    {
      // We access directly to the component of the Point for performance reason
      marginalSample(i, j) = operator()(i, indices[j]);
    }
  }

  return marginalSample;
}

/* Select points as a sample */
SampleImplementation SampleImplementation::select(const UnsignedIntegerCollection & indices) const
{
  const UnsignedInteger size = indices.getSize();
  SampleImplementation result(size, dimension_);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger index = indices[i];
    if (index >= size_) throw InvalidArgumentException(HERE) << "Error: expected indices less than " << size_ << ", here indices[" << i << "]=" << index;
    std::copy(data_.begin() + index * dimension_, data_.begin() + (index + 1) * dimension_, result.data_.begin() + i * dimension_);
  }
  result.setDescription(getDescription());
  return result;
}

/* Save to CSV file */
void SampleImplementation::exportToCSVFile(const FileName & filename,
    const String & csvSeparator) const
{

  std::ofstream csvFile(filename.c_str());
  if (csvFile.fail())
    throw FileOpenException(HERE) << "Could not open file " << filename;
  csvFile.imbue(std::locale("C"));
  csvFile.precision(16);
  csvFile << std::scientific;
  // Export the description
  if (!p_description_.isNull())
  {
    const Description description(getDescription());
    String separator;
    for (UnsignedInteger i = 0; i < dimension_; ++i, separator = csvSeparator)
    {
      String label(description[i]);
      Bool isBlank = true;
      for (UnsignedInteger j = 0; isBlank && j < label.size(); ++j) isBlank = (label[j] == ' ') || (label[j] == '\t');
      if (isBlank) csvFile << separator << "\"NoDescription\"";
      else csvFile << separator << "\"" << description[i] << "\"";
    }
    csvFile << "\n";
  }
  // Write the data
  UnsignedInteger index = 0;
  for(UnsignedInteger i = 0; i < size_; ++i)
  {
    csvFile << data_[index];
    ++index;
    for(UnsignedInteger j = 1; j < dimension_; ++j)
    {
      csvFile << csvSeparator << data_[index];
      ++index;
    } // j
    csvFile << "\n";
  } // i
  // Close the file
  csvFile.close();
}


/* Method save() stores the object through the StorageManager */
void SampleImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "size_", size_);
  adv.saveAttribute( "dimension_", dimension_);
  adv.saveAttribute( "data_", data_);
  if (!p_description_.isNull())
    adv.saveAttribute( "description_", *p_description_ );
}


/* Method load() reloads the object from the StorageManager */
void SampleImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "size_", size_);
  adv.loadAttribute( "dimension_", dimension_);
  adv.loadAttribute( "data_", data_);
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
