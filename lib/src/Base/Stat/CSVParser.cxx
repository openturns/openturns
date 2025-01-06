//                                               -*- C++ -*-
/**
 *  @brief CSV parser
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include <cstring>       // std::strerror

#include "openturns/CSVParser.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/OTconfig.hxx"

#ifdef OPENTURNS_ENABLE_CXX17
#include <filesystem>
#endif
#include "rapidcsv.h"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CSVParser
 */

CLASSNAMEINIT(CSVParser)

/* Constructor without parameters */
CSVParser::CSVParser()
  : Object()
{
  // Nothing to do
}

CSVParser::CSVParser(const String & fileName)
: Object()
, fileName_(fileName) {}


/* String converter */
String CSVParser::__repr__() const
{
  OSS oss(true);
  oss << "class= " << CSVParser::GetClassName();
  return oss;
}

/* String converter */
String CSVParser::__str__(const String & ) const
{
  OSS oss(false);
  oss << CSVParser::GetClassName() << "(separator = " << fieldSeparator_ << ")";
  return oss;
}

void CSVParser::setFieldSeparator(const char fieldSeparator)
{
  fieldSeparator_ = fieldSeparator;
}

void CSVParser::setAllowComments(const Bool allowComments)
{
  allowComments_ = allowComments;
}

void CSVParser::setAllowEmptyLines(const Bool allowEmptyLines)
{
  allowEmptyLines_ = allowEmptyLines;
}

void CSVParser::setSkippedLinesNumber(const UnsignedInteger skippedLinesNumber)
{
  skippedLinesNumber_ = skippedLinesNumber;
}

void CSVParser::setNumericalSeparator(const char decimalSeparator)
{
  decimalSeparator_ = decimalSeparator;
}

Sample CSVParser::load() const
{
#ifdef OPENTURNS_ENABLE_CXX17
  if (!std::ifstream(std::filesystem::u8path(fileName_)).good())
#else
  if (!std::ifstream(fileName_).good())
#endif
    throw FileNotFoundException(HERE) << "Cannot open file '" << fileName_ << "'. Reason: " << std::strerror(errno);
  if (fieldSeparator_ == decimalSeparator_)
    throw InvalidArgumentException(HERE) << "The field separator must be different from the decimal separator";
  const int pColumnNameIdx = skippedLinesNumber_ != 0 ? skippedLinesNumber_ - 1 : -1;
  rapidcsv::LabelParams pLabelParams(pColumnNameIdx, -1);
  rapidcsv::SeparatorParams pSeparatorParams(fieldSeparator_, true);
  rapidcsv::ConverterParams pConverterParams;
  rapidcsv::LineReaderParams pLineReaderParams;
  pLineReaderParams.mSkipCommentLines = allowComments_;
  const String commentMarkers(ResourceMap::GetAsString("Sample-CommentMarker"));
  if (commentMarkers.size() != 1)
    throw InvalidArgumentException(HERE) << "The entry Sample-CommentMarker must be a string of size 1";
  if (allowEmptyLines_ && (commentMarkers[0] == fieldSeparator_ || commentMarkers[0] == decimalSeparator_))
    throw InvalidArgumentException(HERE) << "The comment marker must be different from the field and decimal separators";
  pLineReaderParams.mCommentPrefix = commentMarkers[0];
  pLineReaderParams.mSkipEmptyLines = allowEmptyLines_;
  Description description;
  rapidcsv::Document doc(fileName_, pLabelParams, pSeparatorParams, pConverterParams, pLineReaderParams);
  Sample result(doc.GetRowCount(), doc.GetColumnCount());
  Bool oneOk = false;

  auto convLambda = [this, &oneOk](const std::string& pStr, Scalar& pVal)
  {
    std::istringstream iss(pStr);
    iss.imbue(std::locale(std::locale::classic(), new CSVParserFormat(decimalSeparator_)));
    iss >> pVal;
    if (iss.fail() || iss.bad() || !iss.eof())
    {
      std::map<String, Scalar> infMap;
      infMap["inf"] = SpecFunc::Infinity;
      infMap["+inf"] = SpecFunc::Infinity;
      infMap["-inf"] = -SpecFunc::Infinity;
      infMap["Inf"] = SpecFunc::Infinity;
      infMap["+Inf"] = SpecFunc::Infinity;
      infMap["-Inf"] = -SpecFunc::Infinity;
      infMap["INF"] = SpecFunc::Infinity;
      infMap["+INF"] = SpecFunc::Infinity;
      infMap["-INF"] = -SpecFunc::Infinity;
      // handle inf values
      if (infMap.count(pStr))
        pVal = infMap[pStr];
      else
        // invalid values are set to nan
        pVal = std::numeric_limits<Scalar>::quiet_NaN();
    }
    else
      oneOk = true;
    return pVal;
  };

  for (UnsignedInteger i = 0; i < doc.GetRowCount(); ++ i)
  {
    for (UnsignedInteger j = 0; j < doc.GetColumnCount(); ++ j)
    {
      try
      {
        result(i, j) = doc.GetCell<Scalar>(j, i, convLambda);
      }
      catch (const std::out_of_range &)
      {
        // GetCell fails when line is incomplete
        result(i, j) = std::numeric_limits<Scalar>::quiet_NaN();
      }
    }
  }

  // headers if unparsable values on the first row
  Bool haveHeaders = false;
  for (UnsignedInteger j = 0; j < doc.GetColumnCount(); ++ j)
    if (std::isnan(result(0, j)))
    {
      haveHeaders = true;
      break;
    }

  // consider failed when no value has been successfully parsed (outside header)
  if (doc.GetRowCount() > (haveHeaders ? 1 : 0) && !oneOk)
    throw InvalidArgumentException(HERE) << "Could not parse CSV file '" << fileName_ << "' using delimiter '" << fieldSeparator_ << "'";

  if (haveHeaders)
  {
    description.resize(doc.GetColumnCount());
    for (UnsignedInteger j = 0; j < doc.GetColumnCount(); ++ j)
    {
      try
      {
        description[j] = doc.GetCell<std::string>(j, 0);
      }
      catch (const std::out_of_range &)
      {
        // line may be incomplete
      }
    }
    // reject empty components
    for (UnsignedInteger j = 0; j < description.getSize(); ++ j)
      if (description[j].empty())
      {
        description = Description::BuildDefault(result.getDimension(), "data_");
        break;
      }
    result.setDescription(description);
    result.erase(0, 1);
  }
  else
    result.setDescription(Description::BuildDefault(result.getDimension(), "data_"));
  result.setName(fileName_);
  return result;
}


END_NAMESPACE_OPENTURNS
