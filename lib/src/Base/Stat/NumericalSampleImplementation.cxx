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
#include <sstream>
#include <limits>        // std::numeric_limits
#include <map>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include <cstdio>        // std::fopen, std::errno
#include <cstring>       // std::strerror

#include "openturns/OTconfig.hxx"
#include "openturns/NumericalSampleImplementation.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Path.hxx"
#include "openturns/TBB.hxx"
#include "kendall.h"
#include "openturns/IdentityMatrix.hxx"

#if defined(OPENTURNS_HAVE_BISON) && defined(OPENTURNS_HAVE_FLEX)
#include "openturns/csv_parser_state.hxx"
#include "csv_parser.hh"
#include "csv_lexer.h"

int csvparse (OT::CSVParserState & theState, yyscan_t yyscanner, FILE * theFile, OT::NumericalSampleImplementation &impl, OT::UnsignedInteger & theDimension, const char * separator);
#endif

BEGIN_NAMESPACE_OPENTURNS


TEMPLATE_CLASSNAMEINIT(PersistentCollection<NumericalPoint>);

static const Factory<PersistentCollection<NumericalPoint> > RegisteredFactory_PC_NP;

NSI_point::NSI_point(NumericalSampleImplementation * p_nsi, const UnsignedInteger index)
  : p_nsi_(p_nsi), index_(index), dimension_(p_nsi->dimension_) {}

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

// NSI_point & NSI_point::operator = (const NumericalPoint & rhs)
// {
//   if ( getDimension() == rhs.getDimension() )
//     std::copy( rhs.begin(), rhs.end(), begin() );
//   return *this;
// }

NumericalScalar & NSI_point::operator [] (UnsignedInteger i)
{
  return p_nsi_->data_[index_ * dimension_ + i];
}

const NumericalScalar & NSI_point::operator [] (const UnsignedInteger i) const
{
  return p_nsi_->data_[index_ * dimension_ + i];
}

NumericalScalar & NSI_point::at (UnsignedInteger i)
{
  return p_nsi_->data_.at(index_ * dimension_ + i);
}

const NumericalScalar & NSI_point::at (const UnsignedInteger i) const
{
  return p_nsi_->data_.at(index_ * dimension_ + i);
}

NSI_point & NSI_point::operator += (const NSI_point & other)
{
  if (getDimension() != other.getDimension())
    throw InvalidArgumentException(HERE)
        << "NumericalPoints of different dimensions cannot be added (LHS dimension = "
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
        << "NumericalPoints of different dimensions cannot be substracted (LHS dimension = "
        << getDimension()
        << "; RHS dimension = "
        << other.getDimension();

  for (UnsignedInteger i = 0; i < getDimension(); ++i) (*this)[i] -= other[i];
  return *this;
}

NSI_point & NSI_point::operator += (const NumericalPoint & other)
{
  if (getDimension() != other.getDimension())
    throw InvalidArgumentException(HERE)
        << "NumericalPoints of different dimensions cannot be added (LHS dimension = "
        << getDimension()
        << "; RHS dimension = "
        << other.getDimension();

  for (UnsignedInteger i = 0; i < getDimension(); ++i) (*this)[i] += other[i];
  return *this;
}

NSI_point & NSI_point::operator -= (const NumericalPoint & other)
{
  if (getDimension() != other.getDimension())
    throw InvalidArgumentException(HERE)
        << "NumericalPoints of different dimensions cannot be substracted (LHS dimension = "
        << getDimension()
        << "; RHS dimension = "
        << other.getDimension();

  for (UnsignedInteger i = 0; i < getDimension(); ++i) (*this)[i] -= other[i];
  return *this;
}

NSI_point & NSI_point::operator *= (const NumericalScalar val)
{
  for(UnsignedInteger i = 0; i < getDimension(); ++i) (*this)[i] *= val;
  return *this;
}

NSI_point & NSI_point::operator /= (const NumericalScalar val)
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
                                      std::less<NumericalScalar>());
}

bool operator > (const NSI_point & lhs, const NSI_point & rhs)
{
  return !( lhs <= rhs );
}

bool operator <= (const NSI_point & lhs, const NSI_point & rhs)
{
  return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                      rhs.begin(), rhs.end(),
                                      std::less_equal<NumericalScalar>());
}

bool operator >= (const NSI_point & lhs, const NSI_point & rhs)
{
  return !( lhs < rhs );
}


std::ostream & operator <<(std::ostream & os, const NSI_point & point)
{
  return os << NumericalPoint( point );
}


OStream & operator << (OStream & OS, const NSI_point & point)
{
  OS.getStream() << NumericalPoint( point ).__repr__();
  return OS;
}


NSI_const_point::NSI_const_point(const NumericalSampleImplementation * p_nsi, const UnsignedInteger index)
  : p_nsi_(p_nsi), index_(index), dimension_(p_nsi->dimension_) {}

NSI_const_point::NSI_const_point(const NSI_point & point)
  : p_nsi_(point.p_nsi_), index_(point.index_), dimension_(point.dimension_) {}

bool operator == (const NSI_const_point & lhs, const NSI_const_point & rhs)
{
  return (lhs.getDimension() == rhs.getDimension()) &&
         std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

const NumericalScalar & NSI_const_point::operator [] (const UnsignedInteger i) const
{
  return p_nsi_->data_[index_ * dimension_ + i];
}

const NumericalScalar & NSI_const_point::at (const UnsignedInteger i) const
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
                                      std::less<NumericalScalar>());
}

bool operator > (const NSI_const_point & lhs, const NSI_const_point & rhs)
{
  return !( lhs <= rhs );
}

bool operator <= (const NSI_const_point & lhs, const NSI_const_point & rhs)
{
  return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                      rhs.begin(), rhs.end(),
                                      std::less_equal<NumericalScalar>());
}

bool operator >= (const NSI_const_point & lhs, const NSI_const_point & rhs)
{
  return !( lhs < rhs );
}


std::ostream & operator <<(std::ostream & os, const NSI_const_point & point)
{
  return os << NumericalPoint( point );
}


OStream & operator << (OStream & OS, const NSI_const_point & point)
{
  OS.getStream() << NumericalPoint( point ).__repr__();
  return OS;
}

static const Factory<NumericalSampleImplementation> RegisteredFactory_NSI;

CLASSNAMEINIT(NumericalSampleImplementation);


/* Factory of NumericalSampleImplementation from CSV file */
NumericalSampleImplementation NumericalSampleImplementation::BuildFromCSVFile(const FileName & fileName,
    const String & csvSeparator)
{
  if (csvSeparator == " ") throw InvalidArgumentException(HERE) << "Error: the space separator is not compatible for CSV file.";

  FILE * theFile = std::fopen(fileName.c_str(), "r");
  if (!theFile)
  {
    // theFile can not be found. Errno is set
    throw FileNotFoundException(HERE) << "Can NOT open file '" << fileName
                                      << "'. Reason: " << std::strerror(errno);
  }

  NumericalSampleImplementation impl(0, 0);

#if defined(OPENTURNS_HAVE_BISON) && defined(OPENTURNS_HAVE_FLEX)

  impl.setName(fileName);
  yyscan_t scanner = 0;

  CSVParserState state;
  state.theFileName = fileName;

  csvlex_init(&scanner);
  csvparse(state, scanner, theFile, impl, impl.dimension_, csvSeparator.c_str());
  csvlex_destroy(scanner);

  std::fclose(theFile);

  // Check the description
  if (impl.p_description_.isNull() || (impl.p_description_->getSize() != impl.getDimension()))
  {
    const UnsignedInteger dimension = impl.getDimension();
    Description defaultDescription(dimension);
    for (UnsignedInteger i = 0; i < dimension; ++i)
      defaultDescription[i] = String(OSS() << "data_" << i);
    impl.setDescription(defaultDescription);
  }

  if (impl.getDimension() == 0) LOGWARN(OSS() << "Warning: No data from the file has been stored.");

#else
  impl = NumericalSampleImplementation::BuildFromTextFile(fileName, csvSeparator);
#endif
  return impl;
}

/* Factory of NumericalSampleImplementation from TXT file */
NumericalSampleImplementation NumericalSampleImplementation::BuildFromTextFile(const FileName & fileName,
    const String & separator)
{
  NumericalSampleImplementation impl(0, 0);

  std::ifstream theFile(fileName.c_str());

  if (!theFile.is_open())
  {
    // theFile can not be found. Errno is set
    throw FileNotFoundException(HERE) << "Can NOT open file '" << fileName
                                      << "'. Reason: " << std::strerror(errno);
  }

  String line;
  Bool isDescription = false;
  Description description;
  NumericalScalar f;
  UnsignedInteger numLine = 1;

  // While there are lines to read
  while (std::getline(theFile, line))
  {
    if (line.size() != 0)
    {
      NumericalPoint dataRow(0);
      // Change the separator if it is not a space
      if (separator != " ")
      {
        std::replace(line.begin(), line.end(), separator.c_str()[0], ' ');
      }
      // Store every fields of the current line in a vector
      std::stringstream strstr(line);
      std::vector<std::string> words;
      std::copy(std::istream_iterator<std::string>(strstr),
                std::istream_iterator<std::string>(), back_inserter(words));

      // Check and store the fields in a NumericalPoint
      for(UnsignedInteger i = 0; i < words.size(); i++)
      {
        std::istringstream iss(words[i]);
        if ( iss >> ( f ) && iss.eof() )
        {
          if (numLine == 1 && isDescription)
          {
            LOGWARN(OSS() << "Warning: the given description is not valid. A default description will be used. ");
            isDescription = false;
            break;
          }
          else
          {
            iss >> f;
            dataRow.add(f);
          }
          if (i == 0) ++numLine;
        }
        else
        {
          if (numLine == 1)
          {
            description.add(words[i]);
            if (!isDescription && i == 0) isDescription = true;
            if (i == words.size() - 1) ++numLine;
          }
          else
          {
            if (i != 0) --numLine;
            break;
          }
        }
      }
      // Check and store the data in a NumericalSampleImplementation
      if (dataRow.getDimension() != 0 && dataRow.getDimension() == words.size())
      {
        if ((numLine == 2 && !isDescription) || (numLine == 3 && isDescription))
        {
          impl = NumericalSampleImplementation(0, words.size());
        }
        if (impl.getDimension() == dataRow.getDimension())
        {
          impl.add(dataRow);
        }
        else
        {
          LOGWARN(OSS() << "Warning: the given line ( " << line << " ) is not compatible with the previous row. It will be ignored.");
        }

      }
      else if (dataRow.getDimension() != 0 && dataRow.getDimension() != words.size())
      {
        LOGWARN(OSS() << "Warning: the given line ( " << line << " ) is not valid. It will be ignored.");
      }
    }
  }

  theFile.close();

  // Check the description
  if (isDescription && description.getSize() == impl.getDimension())
  {
    impl.setDescription(description);
  }
  if (impl.p_description_.isNull() || (impl.p_description_->getSize() != impl.getDimension()))
  {
    const UnsignedInteger dimension = impl.getDimension();
    Description defaultDescription(dimension);
    for (UnsignedInteger i = 0; i < dimension; ++i)
      defaultDescription[i] = String(OSS() << "data_" << i);
    impl.setDescription(defaultDescription);
  }

  if (impl.getDimension() == 0) LOGWARN(OSS() << "Warning: No data from the file has been stored.");

  impl.setName(fileName);

  return impl;
}

/* Store a sample in a temporary text file, one realization by line. Returns the file name. */
String NumericalSampleImplementation::storeToTemporaryFile() const
{
  const String dataFileName(Path::BuildTemporaryFileName("RData.txt.XXXXXX"));
  std::ofstream dataFile(dataFileName.c_str());
  // Fill-in the data file
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    String separator = "";
    for (UnsignedInteger j = 0; j < dimension_; ++j, separator = " ")
    {
      const NumericalScalar value = operator[](i)[j];
      const Bool isNaN = value != value;
      dataFile << separator << std::setprecision(16) << (isNaN ? "\"" : "") << value << (isNaN ? "\"" : "");
    }
    dataFile << std::endl;
  }
  dataFile.close();
  return dataFileName;
}

/* Export a sample as a matrix, one row by realization, in a format suitable to exchange with R */
String NumericalSampleImplementation::streamToRFormat() const
{
  OSS oss;
  oss.setPrecision(16);
  oss << "matrix(c(";
  String separator("");
  for (UnsignedInteger j = 0; j < dimension_; ++j)
    for (UnsignedInteger i = 0; i < size_; ++i, separator = ",")
    {
      const NumericalScalar value = operator[](i)[j];
      const Bool isNaN = value != value;
      oss << separator << (isNaN ? "\"" : "") << value << (isNaN ? "\"" : "");
    }
  oss << "), nrow=" << size_ << ", ncol=" << dimension_ << ")";
  return oss;
}

/* Default constructor is private */
NumericalSampleImplementation::NumericalSampleImplementation()
  : PersistentObject()
  , size_(0)
  , dimension_(0)
  , data_(size_ * dimension_, 0.0)
  , p_description_(NULL)
{
  // Nothing to do
}

/* Standard constructor */
NumericalSampleImplementation::NumericalSampleImplementation(const UnsignedInteger size,
    const UnsignedInteger dim)
  : PersistentObject()
  , size_(size)
  , dimension_(dim)
  , data_(size_ * dimension_, 0.0)
  , p_description_(NULL)
{
  // Nothing to do
}

/* Constructor from a NumericalPoint */
NumericalSampleImplementation::NumericalSampleImplementation(const UnsignedInteger size,
    const NumericalPoint & point)
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


/* Constructor from a collection of NumericalPoint */
NumericalSampleImplementation::NumericalSampleImplementation(const Collection<NumericalPoint> & coll)
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
NumericalSampleImplementation::NumericalSampleImplementation(const Collection<Indices> & coll)
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
NumericalSampleImplementation::NumericalSampleImplementation(const NumericalSampleImplementation & other, iterator first, iterator last)
  : PersistentObject()
  , size_(last - first)
  , dimension_(other.getDimension())
  , data_(size_ * dimension_, 0.0)
  , p_description_(other.p_description_)
{
  std::copy( first, last, begin() );
}


/* Virtual constructor */
NumericalSampleImplementation * NumericalSampleImplementation::clone() const
{
  return new NumericalSampleImplementation(*this);
}



void NumericalSampleImplementation::swap_points(const UnsignedInteger a, const UnsignedInteger b)
{
  std::swap_ranges( &data_[ a * dimension_ ], &data_[ (a + 1) * dimension_ ], &data_[ b * dimension_ ] );
}

void NumericalSampleImplementation::swap_range_points(const UnsignedInteger fa, const UnsignedInteger ta, const UnsignedInteger fb)
{
  for (UnsignedInteger i = 0; i < ta - fa; ++i) swap_points( fa + i, fb + i );
}



/* Description Accessor */
void NumericalSampleImplementation::setDescription(const Description & description)
{
  if (description.getSize() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given description does not match the sample dimension.";
  p_description_ = Pointer<Description>(new Description(description));
}


/* Description Accessor */
Description NumericalSampleImplementation::getDescription() const
{
  if (p_description_.isNull()) return Description(dimension_);
  return *p_description_;
}

/* Comparison function */
Bool operator ==(const NumericalSampleImplementation & lhs,
                 const NumericalSampleImplementation & rhs)
{
  if (&lhs == &rhs) return true;
  return (lhs.size_ == rhs.size_) && (lhs.dimension_ == rhs.dimension_) &&
         std::equal( lhs.begin(), lhs.end(), rhs.begin() );
}

void NumericalSampleImplementation::erase(iterator first,
    iterator last)
{
  PersistentCollection<NumericalScalar>::iterator f = data_.begin() + (first - begin()) * dimension_;
  PersistentCollection<NumericalScalar>::iterator l = data_.begin() + (last - begin()) * dimension_;
  data_.erase( f, l );
  size_ -= last - first;
}

void NumericalSampleImplementation::erase(const UnsignedInteger first,
    const UnsignedInteger last)
{
  PersistentCollection<NumericalScalar>::iterator f = data_.begin() + first * dimension_;
  PersistentCollection<NumericalScalar>::iterator l = data_.begin() + last * dimension_;
  data_.erase( f, l );
  size_ -= last - first;
}

void NumericalSampleImplementation::clear()
{
  data_.clear();
  size_ = 0;
}


/* Raw internal format accessor */
NumericalPoint NumericalSampleImplementation::getData() const
{
  return data_;
}

void NumericalSampleImplementation::setData(const Collection<NumericalScalar> & data)
{
  if (data.getSize() != dimension_ * size_) throw InvalidArgumentException(HERE) << "Error: the given raw data are not compatible with the dimension and size of the sample.";
  data_ = data;
}

/* Whether the list contains the value val */
Bool NumericalSampleImplementation::contains(const NumericalPoint & val) const
{
  for (UnsignedInteger i = 0; i < size_; ++i) if ( (*this)[i] == val ) return true;
  return false;
}

/* String converter */
String NumericalSampleImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << NumericalSampleImplementation::GetClassName()
      << " name=" << getName()
      << " size=" << size_
      << " dimension=" << dimension_;

  const UnsignedInteger printEllipsisThreshold = ResourceMap::GetAsUnsignedInteger("NumericalSampleImplementation-PrintEllipsisThreshold");
  const UnsignedInteger printEllipsisSize = ResourceMap::GetAsUnsignedInteger("NumericalSampleImplementation-PrintEllipsisSize");
  const Bool ellipsis = (data_.getSize() > printEllipsisThreshold);

  const Bool printDescription = !p_description_.isNull() && (p_description_->getSize() == dimension_) && !p_description_->isBlank();

  if (printDescription) {
    const char * sep = "";

    oss << " description=[";
    for (UnsignedInteger j = 0; j < dimension_; ++ j, sep = ",") {
      if (ellipsis && (dimension_ > 2 * printEllipsisSize)) {
        if (j == printEllipsisSize) {
          oss << sep << "...";
        }
        if ((j >= printEllipsisSize) && (j < dimension_ - printEllipsisSize)) {
          continue;
        }
      }
      oss << sep << (*p_description_)[j];
    }
    oss << "]";
  }

  oss << " data=[";
  const char * sep = "";

  for (UnsignedInteger i = 0; i < size_; ++ i, sep = ",") {
    if (ellipsis && (size_ > 2 * printEllipsisSize)) {
      if (i == printEllipsisSize) {
        oss << sep << "...";
      }
      if ((i >= printEllipsisSize) && (i < size_ - printEllipsisSize)) {
        continue;
      }
    }
    oss << sep << "[";
    const char * sep2 = "";
    for (UnsignedInteger j = 0; j < dimension_; ++ j, sep2 = ",")
    {
      if (ellipsis && (dimension_ > 2 * printEllipsisSize)) {
        if (j == printEllipsisSize) {
          oss << sep2 << "...";
        }
        if ((j >= printEllipsisSize) && (j < dimension_ - printEllipsisSize)) {
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

String NumericalSampleImplementation::__str__(const String & offset) const
{
  // First, print the description if it is not empty.
  // If you use the getDescription() method you get a default value
  // for the description that is not stored in the sample, producing a spurious output
  const Bool printDescription = !p_description_.isNull() && (p_description_->getSize() == dimension_) && !p_description_->isBlank();

  const UnsignedInteger printEllipsisThreshold = ResourceMap::GetAsUnsignedInteger("NumericalSampleImplementation-PrintEllipsisThreshold");
  const UnsignedInteger printEllipsisSize = ResourceMap::GetAsUnsignedInteger("NumericalSampleImplementation-PrintEllipsisSize");
  const Bool ellipsis = (data_.getSize() > printEllipsisThreshold);

  size_t twidth = 0; // column title max width
  size_t lwidth = 0; // LHS number max width
  size_t rwidth = 0; // RHS number max width
  size_t iwidth = 0; // index max width

  if (printDescription)
  {
    for( UnsignedInteger j = 0; j < dimension_; ++j )
    {
      if (ellipsis && (dimension_ > 2 * printEllipsisSize)) {
        if ((j >= printEllipsisSize) && (j < dimension_ - printEllipsisSize)) {
          continue;
        }
      }
      twidth = std::max( twidth, (*p_description_)[j].size() );
    }
  }

  for( UnsignedInteger i = 0; i < size_; ++i )
  {
    if (ellipsis && (size_ > 2 * printEllipsisSize)) {
      if ((i >= printEllipsisSize) && (i < size_ - printEllipsisSize)) {
        continue;
      }
    }
    for (UnsignedInteger j = 0; j < dimension_; ++ j)
    {
      if (ellipsis && (dimension_ > 2 * printEllipsisSize)) {
        if ((j >= printEllipsisSize) && (j < dimension_ - printEllipsisSize)) {
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
    oss << offset << String( iwidth , ' ' ) << "   [ ";
    const char * sep = "";
    for( UnsignedInteger j = 0; j < dimension_; ++j, sep = " " )
    {
      if (ellipsis && (dimension_ > 2 * printEllipsisSize)) {
        if (j == printEllipsisSize) {
          oss << sep << "...";
        }
        if ((j >= printEllipsisSize) && (j < dimension_ - printEllipsisSize)) {
          continue;
        }
      }
      oss << sep << (*p_description_)[j] << String( twidth - (*p_description_)[j].size(), ' ' );
    }
    oss << " ]\n";
  }

  const char * newline = "";
  for( UnsignedInteger i = 0; i < size_; ++i, newline = "\n" )
  {
    if (ellipsis && (size_ > 2 * printEllipsisSize)) {
      if (i == printEllipsisSize) {
        oss << "\n...";
      }
      if ((i >= printEllipsisSize) && (i < size_ - printEllipsisSize)) {
        continue;
      }
    }
    String sti = OSS() << i;
    oss << newline << offset << String( iwidth - sti.size(), ' ' ) << sti << " : [ ";
    const char * sep = "";
    for( UnsignedInteger j = 0; j < dimension_; ++j, sep = " " )
    {
      if (ellipsis && (dimension_ > 2 * printEllipsisSize)) {
        if (j == printEllipsisSize) {
          oss << sep << "...";
        }
        if ((j >= printEllipsisSize) && (j < dimension_ - printEllipsisSize)) {
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
NumericalSampleImplementation & NumericalSampleImplementation::add(const NumericalPoint & point)
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
NumericalSampleImplementation & NumericalSampleImplementation::add(const NumericalSampleImplementation & sample)
{
  if (sample.getDimension() != dimension_)
    throw InvalidArgumentException(HERE) << "Sample has invalid dimension ("
                                         << sample.getDimension()
                                         << ") expected : "
                                         << getDimension();
  //const UnsignedInteger oldSize = size_;
  size_ += sample.getSize();
  data_.resize(size_ * dimension_);
//   memmove( &data_[oldSize * dimension_], &(sample.data_[0]), sample.getSize() * dimension_ * sizeof(NumericalScalar) );
  std::copy_backward(sample.begin(), sample.end(), end());
  return *this;
}

/* Stack the given sample to the current one */
NumericalSampleImplementation & NumericalSampleImplementation::stack(const NumericalSampleImplementation & sample)
{
  if (sample.getSize() != size_) throw InvalidArgumentException(HERE) << "Error: the given sample has size=" << sample.getSize() << ", expected size=" << size_;
  const UnsignedInteger otherDimension = sample.getDimension();
  NumericalSampleImplementation result(size_, dimension_ + otherDimension);
  // First, the values
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension_; ++j) result[i][j] = (*this)[i][j];
    for (UnsignedInteger j = 0; j < otherDimension; ++j) result[i][dimension_ + j] = sample[i][j];
  }
  // Second, the description
  if (!p_description_.isNull() || !sample.p_description_.isNull()) {
    Description description(getDescription());
    const Description otherDescription(sample.getDescription());
    for (UnsignedInteger i = 0; i < otherDimension; ++i) description.add(otherDescription[i]);
    result.setDescription(description);
  }
  *this = result;
  return *this;
}

struct AddPolicy
{
  typedef NumericalPoint value_type;

  static inline value_type GetInvariant(const NumericalSampleImplementation & nsi)
  {
    return value_type(nsi.getDimension(), 0.0);
  }


  template <typename T>
  static inline value_type & inplace_op( value_type & a, const T & pt )
  {
    const UnsignedInteger dim = a.getDimension();
    for (UnsignedInteger i = 0; i < dim; ++i) a[i] += pt[i];
    return a;
  }
}; /* end struct AddPolicy */

template <typename OP>
struct ReductionFunctor
{
  const NumericalSampleImplementation & nsi_;
  const OP & op_;
  typename OP::value_type accumulator_;

  ReductionFunctor(const NumericalSampleImplementation & nsi, const OP & op = OP())
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
  NumericalSampleImplementation & nsi_;
  const OP & op_;
public:
  ParallelFunctor(NumericalSampleImplementation & nsi, const OP & op) : nsi_(nsi), op_(op) {}

  void operator() (const TBB::BlockedRange<UnsignedInteger> & r) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i) op_.inplace_op( nsi_[i] );
  }

}; /* end class ParallelFunctor */



/*
 * Gives the mean of the sample, based on the formula
 * mean = sum of the elements in the sample / size of the sample
 */
NumericalPoint NumericalSampleImplementation::computeMean() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the mean of an empty sample.";
  ReductionFunctor<AddPolicy> functor( *this );
  TBB::ParallelReduce( 0, size_, functor );
  return functor.accumulator_ * (1.0 / size_);
}

struct CovariancePolicy
{
  typedef NumericalPoint value_type;

  const value_type & mean_;
  const UnsignedInteger dimension_;

  CovariancePolicy( const value_type & mean)
    : mean_(mean), dimension_(mean_.getDimension()) {}

  static inline value_type GetInvariant(const NumericalSampleImplementation & nsi)
  {
    return value_type(nsi.getDimension() * nsi.getDimension(), 0.0);
  }

  inline value_type & inplace_op( value_type & var, NSI_const_point point ) const
  {
    UnsignedInteger baseIndex = 0;
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      const NumericalScalar deltaI = point[i] - mean_[i];
      for (UnsignedInteger j = i; j < dimension_; ++j)
      {
        const NumericalScalar deltaJ = point[j] - mean_[j];
        var[baseIndex + j] += deltaI * deltaJ;
      }
      baseIndex += dimension_;
    }
    return var;
  }

  static inline value_type & inplace_op( value_type & var, const value_type & point )
  {
    return var += point;
  }

}; /* end struct CovariancePolicy */

/*
 * Gives the covariance matrix of the sample, normalization by 1 / (size - 1) if size > 1
 */
CovarianceMatrix NumericalSampleImplementation::computeCovariance() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the covariance of an empty sample.";
  // Special case for a sample of size 1
  if (size_ == 1) return CovarianceMatrix(dimension_, NumericalPoint(dimension_ * dimension_));

  const NumericalPoint mean(computeMean());

  const CovariancePolicy policy ( mean );
  ReductionFunctor<CovariancePolicy> functor( *this, policy );
  TBB::ParallelReduce( 0, size_, functor );
  CovarianceMatrix result(dimension_, functor.accumulator_ / (size_ - 1));
  return result;
}

/*
 * Gives the standard deviation of the sample, i.e. the square-root of the covariance matrix.
 */
TriangularMatrix NumericalSampleImplementation::computeStandardDeviation() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the standard deviation of an empty sample.";
  return computeCovariance().computeCholesky();
}


struct VariancePerComponentPolicy
{
  typedef NumericalPoint value_type;

  const value_type & mean_;
  const UnsignedInteger dimension_;

  VariancePerComponentPolicy( const value_type & mean)
    : mean_(mean), dimension_(mean_.getDimension()) {}

  static inline value_type GetInvariant(const NumericalSampleImplementation & nsi)
  {
    return value_type(nsi.getDimension(), 0.0);
  }

  inline value_type & inplace_op( value_type & var, NSI_const_point point ) const
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      const NumericalScalar val = point[i] - mean_[i];
      var[i] += val * val;
    }
    return var;
  }

  static inline value_type & inplace_op( value_type & var, const value_type & point )
  {
    return var += point;
  }

}; /* end struct VariancePerComponentPolicy */

/*
 * Gives the variance of the sample (by component)
 */
NumericalPoint NumericalSampleImplementation::computeVariance() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the variance per component of an empty sample.";

  // Special case for a sample of size 1
  if (size_ == 1) return NumericalPoint(dimension_, 0.0);
  const NumericalPoint mean( computeMean() );

  const VariancePerComponentPolicy policy ( mean );
  ReductionFunctor<VariancePerComponentPolicy> functor( *this, policy );
  TBB::ParallelReduce( 0, size_, functor );
  return functor.accumulator_ / (size_ - 1);
}

/*
 * Gives the standard deviation of each component of the sample
 */
NumericalPoint NumericalSampleImplementation::computeStandardDeviationPerComponent() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the standard deviation per component of an empty sample.";
  NumericalPoint sd(computeVariance());
  for (UnsignedInteger i = 0; i < dimension_; ++i) sd[i] = sqrt(sd[i]);
  return sd;
}



/*
 * Gives the Pearson correlation matrix of the sample
 */
CorrelationMatrix NumericalSampleImplementation::computePearsonCorrelation() const
{
  return computeLinearCorrelation();
}

CorrelationMatrix NumericalSampleImplementation::computeLinearCorrelation() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the Pearson correlation of an empty sample.";
  CorrelationMatrix correlation(dimension_);
  if (dimension_ == 1) return correlation;

  const CovarianceMatrix covariance(computeCovariance());
  NumericalPoint sd(dimension_);
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
  NumericalScalar value_;
  UnsignedInteger index_;
  Pair() : value_(0.0), index_() {}
  Pair(NumericalScalar value, UnsignedInteger index) : value_(value), index_(index) {}
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
  const NumericalSampleImplementation & nsi_;
  // Sorting permutation
  Indices permutation_;
  // True if sample has ties
  mutable Bool hasTies_;

  Comparison(UnsignedInteger first,
             const NumericalSampleImplementation & nsi)
    : first_(first), second_(first), nsi_(nsi), permutation_(nsi_.getSize()), hasTies_(false)
  {
    permutation_.fill();
  }

  Comparison(UnsignedInteger first, UnsignedInteger second,
             const NumericalSampleImplementation & nsi)
    : first_(first), second_(second), nsi_(nsi), permutation_(nsi_.getSize()), hasTies_(false)
  {
    permutation_.fill();
  }

  Bool operator() (const UnsignedInteger i, const UnsignedInteger j) const
  {
    const NumericalScalar xI = nsi_[ permutation_[i] ][ first_  ];
    const NumericalScalar xJ = nsi_[ permutation_[j] ][ first_  ];
    const NumericalScalar yI = nsi_[ permutation_[i] ][ second_ ];
    const NumericalScalar yJ = nsi_[ permutation_[j] ][ second_ ];
    hasTies_ |= (xI == xJ);
    return ( (xI < xJ) || ((xI == xJ) && (yI < yJ)) );
  }
}; // struct Comparison




/* Ranked sample */
NumericalSampleImplementation NumericalSampleImplementation::rank() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot rank an empty sample.";
  NumericalSampleImplementation rankedSample(size_, dimension_);

  // Sort and rank all the marginal samples
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    Collection<Pair> sortedMarginalSamples(size_);
    for (UnsignedInteger j = 0; j < size_; ++j)
    {
      sortedMarginalSamples[j].value_ = (*this)[j][i];
      sortedMarginalSamples[j].index_ = j;
    }
    // sort
    TBB::ParallelSort(sortedMarginalSamples.begin(), sortedMarginalSamples.end());
    // rank
    NumericalScalar lastValue = sortedMarginalSamples[0].value_;
    UnsignedInteger lastIndex = 0;
    NumericalScalar currentValue = 0.0;
    for (UnsignedInteger j = 1; j < size_; ++j)
    {
      currentValue = sortedMarginalSamples[j].value_;
      if (currentValue > lastValue)
      {
        const NumericalScalar rankValue = 0.5 * (lastIndex + j - 1);
        for (UnsignedInteger k = lastIndex; k < j; ++k) rankedSample[ sortedMarginalSamples[k].index_ ][i] = rankValue;
        lastIndex = j;
        lastValue = currentValue;
      }
    }
    // If we end with a constant range
    if (currentValue == lastValue)
    {
      const NumericalScalar rankValue = 0.5 * (lastIndex + size_ - 1);
      for (UnsignedInteger k = lastIndex; k < size_; ++k) rankedSample[ sortedMarginalSamples[k].index_ ][i] = rankValue;
    }
  }
  if (!p_description_.isNull()) rankedSample.setDescription(getDescription());
  return rankedSample;
}

/* Ranked component */
NumericalSampleImplementation NumericalSampleImplementation::rank(const UnsignedInteger index) const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot rank an empty sample.";
  if (index >= dimension_) throw OutOfBoundException(HERE) << "The requested index is too large, index=" << index << ", dimension=" << dimension_;
  return getMarginal(index).rank();
}

/* Sorted sample, component by component */
NumericalSampleImplementation NumericalSampleImplementation::sort() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot sort an empty sample.";

  NumericalSampleImplementation sortedSample(size_, dimension_);
  // Special case for 1D sample
  if (dimension_ == 1)
  {
    NumericalPoint sortedData(data_);
    TBB::ParallelSort(sortedData.begin(), sortedData.end());
    sortedSample.setData(sortedData);
    return sortedSample;
  }
  // The nD samples
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    NumericalPoint data(size_);
    for (UnsignedInteger j = 0; j < size_; ++j) data[j] = (*this)[j][i];
    TBB::ParallelSort(data.begin(), data.end());
    // copy
    for (UnsignedInteger j = 0; j < size_; ++j) sortedSample[j][i] = data[j];
  } // loop over dimension
  if (!p_description_.isNull()) sortedSample.setDescription(getDescription());
  return sortedSample;
}

/* Sorted sample, one component */
NumericalSampleImplementation NumericalSampleImplementation::sort(const UnsignedInteger index) const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot sort an empty sample.";

  if (index >= getDimension()) throw OutOfBoundException(HERE) << "The requested index is too large, index=" << index << ", dimension=" << getDimension();

  return getMarginal(index).sort();
}


struct Sortable
{
  NumericalPoint values_;
  UnsignedInteger index_;
  Sortable() : values_(1, 0.0), index_(0) {}
  Sortable(const NumericalPoint & values, const UnsignedInteger index) : values_(values), index_(index) {}
  Bool operator < (const Sortable & other) const
  {
    return values_[index_] < other.values_[other.index_];
  }
};

/* Sorted according a component */
NumericalSampleImplementation NumericalSampleImplementation::sortAccordingToAComponent(const UnsignedInteger index) const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot sort an empty sample.";

  Collection<Sortable> sortables(size_);
  for (UnsignedInteger i = 0; i < size_; ++i) sortables[i] = Sortable((*this)[i], index);
  TBB::ParallelSort(sortables.begin(), sortables.end());
  NumericalSampleImplementation sortedSample(size_, dimension_);
  for (UnsignedInteger i = 0; i < size_; ++i) sortedSample[i] = NumericalPoint(sortables[i].values_);
  if (!p_description_.isNull()) sortedSample.setDescription(getDescription());
  return sortedSample;
}

/*
 * Gives the Spearman correlation matrix of the sample
 */
CorrelationMatrix NumericalSampleImplementation::computeSpearmanCorrelation() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the Spearman correlation of an empty sample.";

  return rank().computePearsonCorrelation();
}


/*
 * Gives the Kendall tau matrix of the sample, including ties correction
 */

struct ComputeKendallPolicy
{
  const NumericalSampleImplementation & input_;
  NumericalPoint & output_;
  const Indices & indicesX_;
  const Indices & indicesY_;
  const Bool smallCase_;

  ComputeKendallPolicy( const NumericalSampleImplementation & input,
                        NumericalPoint & output,
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
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const UnsignedInteger size = input_.getSize();
      const UnsignedInteger indX = indicesX_[i];
      const UnsignedInteger indY = indicesY_[i];
      NumericalPoint x(size);
      NumericalPoint y(size);
      for (UnsignedInteger k = 0; k < size; ++k)
      {
        x[k] = input_[k][indX];
        y[k] = input_[k][indY];
      }
      if (smallCase_) output_[i] = kendallSmallN(&x[0], &y[0], size);
      else output_[i] = kendallNlogN(&x[0], &y[0], size);
    }
  }

}; /* end struct ComputeKendallPolicy */

CorrelationMatrix NumericalSampleImplementation::computeKendallTau() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the Kendall tau of an empty sample.";

  // Use external efficient C implementation of the O(Nlog(N)) or O(N^2) Kendall tau computation depending on the sample size
  const Bool smallCase = size_ < ResourceMap::GetAsUnsignedInteger("NumericalSampleImplementation-SmallKendallTau");
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
      LOGINFO(OSS() << "in NumericalSampleImplementation::computeKendallTau(), computing tau[" << i << ", " << j << "]");
      indX[index] = i;
      indY[index] = j;
      ++index;
    } // end for j
  } // end for i
  // Now the computation
  NumericalPoint result(caseNumber);
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
NumericalPoint NumericalSampleImplementation::computeRange() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the range per component of an empty sample.";
  return getMax() - getMin();
}

/*
 * Gives the median of the sample (by component)
 */
NumericalPoint NumericalSampleImplementation::computeMedian() const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the median per component of an empty sample.";
  return computeQuantilePerComponent(0.5);
}

struct SkewnessPerComponentPolicy
{
  typedef NumericalPoint value_type;

  const value_type & mean_;
  const UnsignedInteger dimension_;

  SkewnessPerComponentPolicy( const value_type & mean)
    : mean_(mean), dimension_(mean_.getDimension()) {}

  static inline value_type GetInvariant(const NumericalSampleImplementation & nsi)
  {
    return value_type(2 * nsi.getDimension(), 0.0);
  }

  inline value_type & inplace_op( value_type & var, NSI_const_point point ) const
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      const NumericalScalar val = point[i] - mean_[i];
      const NumericalScalar val2 = val * val;
      var[i] += val2;
      var[i + dimension_] += val2 * val;
    }
    return var;
  }

  static inline value_type & inplace_op( value_type & var, const value_type & point )
  {
    return var += point;
  }

}; /* end struct SkewnessPerComponentPolicy */

/*
 * Gives the skewness of the sample (by component)
 */
NumericalPoint NumericalSampleImplementation::computeSkewness() const
{
  if (size_ < 2) throw InternalException(HERE) << "Error: cannot compute the skewness per component of a sample of size less than 2.";

  if (size_ == 2) return NumericalPoint(dimension_, 0.0);

  const NumericalPoint mean(computeMean());
  const SkewnessPerComponentPolicy policy ( mean );
  ReductionFunctor<SkewnessPerComponentPolicy> functor( *this, policy );
  TBB::ParallelReduce( 0, size_, functor );
  NumericalPoint skewness(dimension_);
  const NumericalScalar factor = size_ * sqrt(size_ - 1.0) / (size_ - 2);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    if (functor.accumulator_[i] == 0.0) throw NotDefinedException(HERE) << "Error: the sample has component " << i << " constant. The skewness is not defined.";
    skewness[i] = factor * functor.accumulator_[i + dimension_] / pow(functor.accumulator_[i], 1.5);
  }
  return skewness;
}

struct KurtosisPerComponentPolicy
{
  typedef NumericalPoint value_type;

  const value_type & mean_;
  const UnsignedInteger dimension_;

  KurtosisPerComponentPolicy( const value_type & mean)
    : mean_(mean), dimension_(mean_.getDimension()) {}

  static inline value_type GetInvariant(const NumericalSampleImplementation & nsi)
  {
    return value_type(2 * nsi.getDimension(), 0.0);
  }

  inline value_type & inplace_op( value_type & var, NSI_const_point point ) const
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      const NumericalScalar val = point[i] - mean_[i];
      const NumericalScalar val2 = val * val;
      var[i] += val2;
      var[i + dimension_] += val2 * val2;
    }
    return var;
  }

  static inline value_type & inplace_op( value_type & var, const value_type & point )
  {
    return var += point;
  }

}; /* end struct KurtosisPerComponentPolicy */

/*
 * Gives the kurtosis of the sample (by component)
 */
NumericalPoint NumericalSampleImplementation::computeKurtosis() const
{
  if (size_ < 3) throw InternalException(HERE) << "Error: cannot compute the kurtosis per component of a sample of size less than 3.";

  if (size_ == 3) return NumericalPoint(dimension_, 0.0);

  const NumericalPoint mean(computeMean());
  const KurtosisPerComponentPolicy policy ( mean );
  ReductionFunctor<KurtosisPerComponentPolicy> functor( *this, policy );
  TBB::ParallelReduce( 0, size_, functor );
  NumericalPoint kurtosis(dimension_);
  const NumericalScalar factor1 = (size_ + 1.0) * size_ * (size_ - 1.0) / ((size_ - 2.0) * (size_ - 3.0));
  const NumericalScalar factor2 = -3.0 * (3.0 * size_ - 5.0) / ((size_ - 2.0) * (size_ - 3.0));
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    if (functor.accumulator_[i] == 0.0) throw NotDefinedException(HERE) << "Error: the sample has component " << i << " constant. The kurtosis is not defined.";
    kurtosis[i] = factor1 * functor.accumulator_[i + dimension_] / (functor.accumulator_[i] * functor.accumulator_[i]) + factor2;
  }
  return kurtosis;
}

struct CenteredMomentPerComponentPolicy
{
  typedef NumericalPoint value_type;

  const value_type & mean_;
  const UnsignedInteger k_;
  const UnsignedInteger dimension_;

  CenteredMomentPerComponentPolicy( const value_type & mean, const UnsignedInteger k)
    : mean_(mean), k_(k), dimension_(mean_.getDimension()) {}

  static inline value_type GetInvariant(const NumericalSampleImplementation & nsi)
  {
    return value_type(nsi.getDimension(), 0.0);
  }

  inline value_type & inplace_op( value_type & var, NSI_const_point point ) const
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      const NumericalScalar val = point[i] - mean_[i];
      var[i] += pow(val, static_cast<int>(k_));
    }
    return var;
  }

  static inline value_type & inplace_op( value_type & var, const value_type & point )
  {
    return var += point;
  }

}; /* end struct CenteredMomentPerComponentPolicy */

/*
 * Gives the centered moment of order k of the sample (by component)
 */
NumericalPoint NumericalSampleImplementation::computeCenteredMoment(const UnsignedInteger k) const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the centered moments per component of an empty sample.";

  // Special case: order 0, return (1,...,1)
  if (k == 0) return NumericalPoint(dimension_, 1.0);
  // Special case: order 1, return (0,...,0)
  if (k == 1) return NumericalPoint(dimension_, 0.0);
  // Special case: order 2, return biased variance estimator
  if (k == 2) return computeVariance() * (size_ - 1.0) / size_;
  // General case
  const NumericalPoint mean(computeMean());

  const CenteredMomentPerComponentPolicy policy ( mean, k );
  ReductionFunctor<CenteredMomentPerComponentPolicy> functor( *this, policy );
  TBB::ParallelReduce( 0, size_, functor );
  return functor.accumulator_ / size_;
}

/*
 * Gives the raw moment of order k of the sample (by component)
 */
NumericalPoint NumericalSampleImplementation::computeRawMoment(const UnsignedInteger k) const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the centered moments per component of an empty sample.";

  if (size_ == 0) throw InvalidArgumentException(HERE) << "Cannot compute centered moments on an empty sample";

  // Special case: order 0, return (size,...,size)
  if (k == 0) return NumericalPoint(dimension_, 1.0);

  const NumericalPoint zero(dimension_);
  const CenteredMomentPerComponentPolicy policy ( zero, k );
  ReductionFunctor<CenteredMomentPerComponentPolicy> functor( *this, policy );
  TBB::ParallelReduce( 0, size_, functor );
  return functor.accumulator_ / size_;
}

/*
 * Gives the quantile per component of the sample
 */
NumericalPoint NumericalSampleImplementation::computeQuantilePerComponent(const NumericalScalar prob) const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the quantile per component of an empty sample.";

  // Special case for prob outside of the allowed bounds
  const NumericalScalar scalarIndex = prob * size_ - 0.5;
  if (scalarIndex >= size_ - 1) return getMax();
  // Special case for prob <= 0.0
  if (scalarIndex <= 0.0) return getMin();

  const UnsignedInteger index = static_cast<UnsignedInteger>( floor( scalarIndex) );
  const NumericalScalar beta = scalarIndex - index;
  const NumericalScalar alpha = 1.0 - beta;
  NumericalPoint quantile(dimension_);
  NumericalPoint component(size_);
  for (UnsignedInteger j = 0; j < dimension_; ++j)
  {
    for (UnsignedInteger i = 0; i < size_; ++i)
      component[i] = operator[](i)[j];

    TBB::ParallelSort(component.begin(), component.end());

    // Interpolation between the two adjacent empirical quantiles
    quantile[j] = alpha * component[index] + beta * component[index + 1];
  } // end for

  return quantile;
}

/*
 * Gives the N-dimension quantile of the sample
 */
NumericalPoint NumericalSampleImplementation::computeQuantile(const NumericalScalar prob) const
{
  if (size_ == 0) throw InternalException(HERE) << "Error: cannot compute the quantile of an empty sample.";

  if (getDimension() == 1) return computeQuantilePerComponent(prob);
  throw NotYetImplementedException(HERE) << "In NumericalSampleImplementation::computeQuantile(const NumericalScalar prob) const";
}

struct CDFPolicy
{
  typedef NumericalScalar value_type;

  const NumericalPoint & point_;
  const Bool tail_;
  const UnsignedInteger dimension_;

  CDFPolicy( const NumericalSampleImplementation & nsi,
             const NumericalPoint & point,
             const Bool tail )
    : point_(point), tail_(tail), dimension_(nsi.getDimension()) {}

  static inline value_type GetInvariant(const NumericalSampleImplementation &)
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
NumericalScalar NumericalSampleImplementation::computeEmpiricalCDF(const NumericalPoint & point,
    const Bool tail) const
{
  if (size_ == 0) throw InvalidArgumentException(HERE) << "Cannot compute the empirical CDF of an empty sample.";
  if (getDimension() != point.getDimension()) throw InvalidArgumentException(HERE) << "Point has incorrect dimension. Got "
        << point.getDimension() << ". Expected " << getDimension();

  const CDFPolicy policy( *this, point, tail );
  ReductionFunctor<CDFPolicy> functor( *this, policy );
  TBB::ParallelReduce( 0, size_, functor );
  return static_cast < NumericalScalar > (functor.accumulator_) / size_;
}

struct MaxPerComponentPolicy
{
  typedef NumericalPoint value_type;

  static inline value_type GetInvariant(const NumericalSampleImplementation & nsi)
  {
    return value_type(nsi.getDimension(), - std::numeric_limits<NumericalScalar>::max());
  }

  template <typename T>
  static inline value_type & inplace_op( value_type & a, const T & b)
  {
    const UnsignedInteger dim = a.getDimension();
    for (UnsignedInteger j = 0; j < dim; ++j) a[j] = std::max( a[j], b[j] );
    return a;
  }
}; /* end struct MaxPerComponentPolicy */

struct MinPerComponentPolicy
{
  typedef NumericalPoint value_type;

  static inline value_type GetInvariant(const NumericalSampleImplementation & nsi)
  {
    return value_type(nsi.getDimension(), std::numeric_limits<NumericalScalar>::max());
  }

  template <typename T>
  static inline value_type & inplace_op( value_type & a, const T & b)
  {
    const UnsignedInteger dim = a.getDimension();
    for (UnsignedInteger j = 0; j < dim; ++j) a[j] = std::min( a[j], b[j] );
    return a;
  }
}; /* end struct MinPerComponentPolicy */


/* Maximum accessor */
NumericalPoint NumericalSampleImplementation::getMax() const
{
  if (size_ == 0) throw InternalException(HERE) << "Impossible to get the maximum of an empty NumericalSample";

  ReductionFunctor<MaxPerComponentPolicy> functor( *this, MaxPerComponentPolicy() );
  functor.accumulator_ = operator[](0);
  TBB::ParallelReduce( 1, size_, functor );
  return functor.accumulator_;
}

/* Minimum accessor */
NumericalPoint NumericalSampleImplementation::getMin() const
{
  if (size_ == 0) throw InternalException(HERE) << "Impossible to get the minimum of an empty NumericalSample";

  ReductionFunctor<MinPerComponentPolicy> functor( *this, MinPerComponentPolicy() );
  functor.accumulator_ = operator[](0);
  TBB::ParallelReduce( 1, size_, functor );
  return functor.accumulator_;
}


struct TranslationPolicy
{
  const NumericalPoint & translation_;
  const UnsignedInteger dimension_;

  TranslationPolicy( const NumericalPoint & translation)
    : translation_(translation), dimension_(translation_.getDimension()) {}

  inline void inplace_op( NSI_point point ) const
  {
    for (UnsignedInteger j = 0; j < dimension_; ++j) point[j] += translation_[j];
  }

}; /* end struct TranslationPolicy */

/*
 * Translate realizations in-place
 */
void NumericalSampleImplementation::translate(const NumericalPoint & translation)
{
  if (dimension_ != translation.getDimension())
    throw InvalidArgumentException(HERE) << "Translation point has incorrect dimension. Got " << translation.getDimension()
                                         << ". Expected " << dimension_;

  if (size_ == 0) return;

  const TranslationPolicy policy( translation );
  ParallelFunctor<TranslationPolicy> functor( *this, policy );
  TBB::ParallelFor( 0, size_, functor );
}

NumericalSampleImplementation & NumericalSampleImplementation::operator += (const NumericalScalar translation)
{
  translate(NumericalPoint(dimension_, translation));
  return *this;
}

NumericalSampleImplementation & NumericalSampleImplementation::operator += (const NumericalPoint & translation)
{
  translate(translation);
  return *this;
}

struct PositiveTranslationSamplePolicy
{
  const NumericalSampleImplementation & translation_;
  NumericalSampleImplementation & output_;

  PositiveTranslationSamplePolicy( const NumericalSampleImplementation & translation,
                                   NumericalSampleImplementation & output)
    : translation_(translation), output_(output) {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i) output_[i] += translation_[i];
  }

}; /* end struct TranslationSamplePolicy */

struct NegativeTranslationSamplePolicy
{
  const NumericalSampleImplementation & translation_;
  NumericalSampleImplementation & output_;

  NegativeTranslationSamplePolicy( const NumericalSampleImplementation & translation,
                                   NumericalSampleImplementation & output)
    : translation_(translation), output_(output) {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i) output_[i] -= translation_[i];
  }

}; /* end struct TranslationSamplePolicy */

NumericalSampleImplementation & NumericalSampleImplementation::operator += (const NumericalSampleImplementation & translation)
{
  if (translation.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the dimension of the given translation=" << translation.getDimension() << " does not match the dimension of the sample=" << dimension_;
  if (translation.getSize() != size_) throw InvalidArgumentException(HERE) << "Error: the size of the given translation=" << translation.getSize() << " does not match the size of the sample=" << size_;
  const PositiveTranslationSamplePolicy policy( translation, *this );
  TBB::ParallelFor( 0, size_, policy );
  return *this;
}

NumericalSampleImplementation & NumericalSampleImplementation::operator -= (const NumericalScalar translation)
{
  return operator +=(-translation);
}

NumericalSampleImplementation & NumericalSampleImplementation::operator -= (const NumericalPoint & translation)
{
  return operator +=(translation * (-1.0));
}

NumericalSampleImplementation & NumericalSampleImplementation::operator -= (const NumericalSampleImplementation & translation)
{
  if (translation.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the dimension of the given translation=" << translation.getDimension() << " does not match the dimension of the sample=" << dimension_;
  if (translation.getSize() != size_) throw InvalidArgumentException(HERE) << "Error: the size of the given translation=" << translation.getSize() << " does not match the size of the sample=" << size_;
  const NegativeTranslationSamplePolicy policy( translation, *this );
  TBB::ParallelFor( 0, size_, policy );
  return *this;
}

NumericalSampleImplementation NumericalSampleImplementation::operator + (const NumericalScalar translation) const
{
  return operator+(NumericalPoint(dimension_, translation));
}

NumericalSampleImplementation NumericalSampleImplementation::operator + (const NumericalPoint & translation) const
{
  NumericalSampleImplementation sample(*this);
  sample += translation;
  sample.setName("");
  return sample;
}

NumericalSampleImplementation NumericalSampleImplementation::operator + (const NumericalSampleImplementation & translation) const
{
  NumericalSampleImplementation sample(*this);
  sample += translation;
  sample.setName("");
  return sample;
}

NumericalSampleImplementation NumericalSampleImplementation::operator - (const NumericalScalar translation) const
{
  return operator-(NumericalPoint(dimension_, translation));
}

NumericalSampleImplementation NumericalSampleImplementation::operator - (const NumericalPoint & translation) const
{
  NumericalSampleImplementation sample(*this);
  sample -= translation;
  sample.setName("");
  return sample;
}

NumericalSampleImplementation NumericalSampleImplementation::operator - (const NumericalSampleImplementation & translation) const
{
  NumericalSampleImplementation sample(*this);
  sample -= translation;
  sample.setName("");
  return sample;
}

struct ScalingPolicy
{
  const NumericalPoint & scale_;
  const UnsignedInteger dimension_;

  ScalingPolicy( const NumericalPoint & scale) : scale_(scale), dimension_(scale_.getDimension()) {}

  inline void inplace_op( NSI_point point ) const
  {
    for (UnsignedInteger j = 0; j < dimension_; ++j) point[j] *= scale_[j];
  }

}; /* end struct ScalingPolicy */

struct MatrixMultiplyPolicy
{
  const SquareMatrix & scale_;
  const UnsignedInteger dimension_;

  MatrixMultiplyPolicy( const SquareMatrix & scale) : scale_(scale), dimension_(scale_.getDimension()) {}

  inline void inplace_op( NSI_point point ) const
  {
    point = scale_ * point;
  }

}; /* end struct MatrixMultiplyPolicy */

/*
 * Scale realizations componentwise in-place
 */
void NumericalSampleImplementation::scale(const SquareMatrix & scaling)
{
  if (dimension_ != scaling.getDimension())
    throw InvalidArgumentException(HERE) << "Scaling point has incorrect dimension. Got " << scaling.getDimension()
                                         << ". Expected " << dimension_;

  if (size_ == 0) return;
  const MatrixMultiplyPolicy policy( scaling );
  ParallelFunctor<MatrixMultiplyPolicy> functor( *this, policy );
  TBB::ParallelFor( 0, size_, functor );
}

void NumericalSampleImplementation::scale(const NumericalPoint & scaling)
{
  if (dimension_ != scaling.getDimension())
    throw InvalidArgumentException(HERE) << "Scaling point has incorrect dimension. Got " << scaling.getDimension()
                                         << ". Expected " << dimension_;

  if (size_ == 0) return;

  const ScalingPolicy policy( scaling );
  ParallelFunctor<ScalingPolicy> functor( *this, policy );
  TBB::ParallelFor( 0, size_, functor );
}

NumericalSampleImplementation & NumericalSampleImplementation::operator *= (const NumericalScalar scaling)
{
  scale(NumericalPoint(dimension_, scaling));
  return *this;
}

NumericalSampleImplementation & NumericalSampleImplementation::operator *= (const NumericalPoint & scaling)
{
  scale(scaling);
  return *this;
}

NumericalSampleImplementation & NumericalSampleImplementation::operator *= (const SquareMatrix & scaling)
{
  scale(scaling);
  return *this;
}

NumericalSampleImplementation & NumericalSampleImplementation::operator /= (const NumericalScalar scaling)
{
  return operator/=(NumericalPoint(dimension_, scaling));
}

NumericalSampleImplementation & NumericalSampleImplementation::operator /= (const NumericalPoint & scaling)
{
  NumericalPoint inverseScaling(getDimension());
  for (UnsignedInteger i = 0; i < getDimension(); ++ i)
  {
    if (scaling[i] == 0.0) throw InvalidArgumentException(HERE) << "Error: the scaling must have nonzero components, here scaling=" << scaling;
    inverseScaling[i] = 1.0 / scaling[i];
  }
  scale(inverseScaling);
  return *this;
}

NumericalSampleImplementation & NumericalSampleImplementation::operator /= (const SquareMatrix & scaling)
{
  SquareMatrix tmp(scaling);
  SquareMatrix inverseScaling(tmp.solveLinearSystem(IdentityMatrix(getDimension())).getImplementation());
  scale(inverseScaling);
  return *this;
}

NumericalSampleImplementation NumericalSampleImplementation::operator * (const NumericalScalar scaling) const
{
  return operator*(NumericalPoint(dimension_, scaling));
}

NumericalSampleImplementation NumericalSampleImplementation::operator * (const NumericalPoint & scaling) const
{
  NumericalSampleImplementation nsi(*this);
  nsi *= scaling;
  nsi.setName("");
  return nsi;
}

NumericalSampleImplementation NumericalSampleImplementation::operator * (const SquareMatrix & scaling) const
{
  NumericalSampleImplementation nsi(*this);
  nsi *= scaling;
  nsi.setName("");
  return nsi;
}

NumericalSampleImplementation NumericalSampleImplementation::operator / (const NumericalScalar scaling) const
{
  return operator/(NumericalPoint(dimension_, scaling));
}

NumericalSampleImplementation NumericalSampleImplementation::operator / (const NumericalPoint & scaling) const
{
  NumericalSampleImplementation nsi(*this);
  nsi /= scaling;
  nsi.setName("");
  return nsi;
}

NumericalSampleImplementation NumericalSampleImplementation::operator / (const SquareMatrix & scaling) const
{
  NumericalSampleImplementation nsi(*this);
  nsi /= scaling;
  nsi.setName("");
  return nsi;
}

/* Get the i-th marginal distribution */
NumericalSampleImplementation NumericalSampleImplementation::getMarginal(const UnsignedInteger index) const
{
  if (index >= dimension_) throw InvalidArgumentException(HERE) << "The index of a marginal sample must be in the range [0, dim-1]";

  // Special case for dimension 1
  if (dimension_ == 1) return *this;

  // General case
  NumericalSampleImplementation marginalSample(size_, 1);

  // If the sample has a description, extract the marginal description
  if (!p_description_.isNull())
    marginalSample.setDescription(Description(1, getDescription()[index]));
  for (UnsignedInteger i = 0; i < size_; ++i)
    marginalSample[i][0] = operator[](i)[index];

  return marginalSample;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
NumericalSampleImplementation NumericalSampleImplementation::getMarginal(const Indices & indices) const
{
  if (!indices.check(dimension_ - 1)) throw InvalidArgumentException(HERE) << "The indices of a marginal sample must be in the range [0, dim-1] and  must be different";

  // Special case for dimension 1
  if (dimension_ == 1) return *this;

  // General case
  const UnsignedInteger outputDimension = indices.getSize();
  NumericalSampleImplementation marginalSample(size_, outputDimension);

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
      // We access directly to the component of the NumericalPoint for performance reason
      marginalSample[i][j] = operator[](i)[indices[j]];
    }
  }

  return marginalSample;
}

/* Save to CSV file */
void NumericalSampleImplementation::exportToCSVFile(const FileName & filename,
    const String & csvSeparator) const
{

  std::ofstream csvFile(filename.c_str());
  if (csvFile.fail())
    throw FileOpenException(HERE) << "Could not open file " << filename;
  csvFile.imbue(std::locale("C"));
  csvFile.precision(16);
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
    csvFile << std::endl;
  }

  // Write the data
  for(UnsignedInteger i = 0; i < size_; ++i)
  {
    String separator;
    for(UnsignedInteger j = 0; j < dimension_; ++j, separator = csvSeparator)
    {
      csvFile << separator << std::scientific << operator[](i)[j];
    }
    csvFile << std::endl;
  }

  // Close the file
  csvFile.close();
}



/* Method save() stores the object through the StorageManager */
void NumericalSampleImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "size_", size_);
  adv.saveAttribute( "dimension_", dimension_);
  adv.saveAttribute( "data_", data_);
  if (!p_description_.isNull())
    adv.saveAttribute( "description_", *p_description_ );
}


/* Method load() reloads the object from the StorageManager */
void NumericalSampleImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "size_", size_);
  adv.loadAttribute( "dimension_", dimension_);
  adv.loadAttribute( "data_", data_);
  Description description;
  adv.loadAttribute( "description_", description );
  if (description.getSize() != 0) setDescription(description);
}


END_NAMESPACE_OPENTURNS
