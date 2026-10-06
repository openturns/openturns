//                                               -*- C++ -*-
/**
 *  @brief CSV parser
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

#include "openturns/CSVParser.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/TBBImplementation.hxx"
#include "openturns/OTconfig.hxx"

#include <filesystem> // for u8path
#include <charconv> // for from_chars
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <cctype>
#include <string_view>

BEGIN_NAMESPACE_OPENTURNS

namespace {
// Trim leading/trailing spaces (isspace) from a view
inline std::string_view TrimView(std::string_view sv)
{
  std::string_view::size_type begin = 0;
  std::string_view::size_type end = sv.size();
  while (begin < end && std::isspace(static_cast<unsigned char>(sv[begin])))
    ++ begin;
  while (end > begin && std::isspace(static_cast<unsigned char>(sv[end - 1])))
    -- end;
  return sv.substr(begin, end - begin);
}

// Check if raw field contains only carriage returns (or is empty)
inline Bool RawFieldIsEmpty(std::string_view sv)
{
  for (const char c : sv)
    if (c != '\r')
      return false;
  return true;
}

// Extract trimmed/unquoted view of a raw field.
// The returned view points either into the raw field or into tmp
// (when unescaping "" is needed). tmp is overwritten when used.
inline std::string_view GetFieldView(std::string_view raw,
                                     std::string & tmp)
{
  const std::string_view trimmed = TrimView(raw);
  if (trimmed.size() >= 2 && trimmed.front() == '"' && trimmed.back() == '"')
  {
    const std::string_view inner = trimmed.substr(1, trimmed.size() - 2);
    // fast path: no inner quote, no copy
    if (inner.find('"') == std::string_view::npos)
      return inner;
    // slow path: unescape "" -> "
    tmp.clear();
    tmp.reserve(inner.size());
    for (std::string_view::size_type i = 0; i < inner.size(); ++ i)
    {
      if (inner[i] == '"' && (i + 1) < inner.size() && inner[i + 1] == '"')
      {
        tmp.push_back('"');
        ++ i;
      }
      else
        tmp.push_back(inner[i]);
    }
    return std::string_view(tmp.data(), tmp.size());
  }
  return trimmed;
}

// Parse a trimmed/unquoted field view to Scalar.
// Returns true on success (at least one char converted), false otherwise (val set to NaN).
// Trailing garbage is ignored to match the historical strtod/from_chars behavior.
inline Bool ParseDoubleView(std::string_view sv,
                            const char decimalSeparator,
                            Scalar & val,
                            std::string & tmp)
{
  if (sv.empty())
  {
    val = std::numeric_limits<Scalar>::quiet_NaN();
    return false;
  }
  std::string_view data = sv;
  if (decimalSeparator != '.')
  {
    if (sv.find(decimalSeparator) != std::string_view::npos)
    {
      tmp.assign(sv.data(), sv.size());
      std::replace(tmp.begin(), tmp.end(), decimalSeparator, '.');
      data = std::string_view(tmp.data(), tmp.size());
    }
  }
#ifdef OPENTURNS_HAVE_STD_FROM_CHARS_DOUBLE
  Scalar parsed = 0.0;
  const auto status = std::from_chars(data.data(), data.data() + data.size(), parsed);
  if (status.ec != std::errc{})
  {
    val = std::numeric_limits<Scalar>::quiet_NaN();
    return false;
  }
  val = parsed;
  return true;
#else
  tmp.assign(data.data(), data.size());
  char * strEnd = nullptr;
  val = std::strtod(tmp.data(), &strEnd);
  if (strEnd == tmp.data())
  {
    val = std::numeric_limits<Scalar>::quiet_NaN();
    return false;
  }
  return true;
#endif
}

// Unicode replacement character for malformed input
static const uint32_t ReplacementChar = 0x0000fffd;

inline uint32_t GetUtf16Unit(std::string_view data, std::string_view::size_type idx, const Bool isLE)
{
  const uint32_t byte0 = static_cast<unsigned char>(data[idx]);
  const uint32_t byte1 = static_cast<unsigned char>(data[idx + 1]);
  return isLE ? ((byte1 << 8) | byte0) : ((byte0 << 8) | byte1);
}

inline void AppendUtf8(const uint32_t codePoint, std::string & utf8)
{
  if (codePoint < 0x80)
    utf8 += static_cast<char>(codePoint);
  else if (codePoint < 0x800)
  {
    utf8 += static_cast<char>(0xc0 | (codePoint >> 6));
    utf8 += static_cast<char>(0x80 | (codePoint & 0x3f));
  }
  else if (codePoint < 0x10000)
  {
    utf8 += static_cast<char>(0xe0 | (codePoint >> 12));
    utf8 += static_cast<char>(0x80 | ((codePoint >> 6) & 0x3f));
    utf8 += static_cast<char>(0x80 | (codePoint & 0x3f));
  }
  else
  {
    utf8 += static_cast<char>(0xf0 | (codePoint >> 18));
    utf8 += static_cast<char>(0x80 | ((codePoint >> 12) & 0x3f));
    utf8 += static_cast<char>(0x80 | ((codePoint >> 6) & 0x3f));
    utf8 += static_cast<char>(0x80 | (codePoint & 0x3f));
  }
}

// Whether a quote character toggles the quoted state, given the current field content.
// Matches the historical behavior: toggle when the field is empty, starts with
// a quote, or only holds whitespace before its first quote (trim is always on).
inline Bool ShouldToggleQuote(std::string_view fieldSoFar,
                              const char quoteChar)
{
  if (fieldSoFar.empty())
    return true;
  if (fieldSoFar.front() == quoteChar)
    return true;
  const std::string_view::size_type quotePos = fieldSoFar.find(quoteChar);
  const std::string_view before = quotePos == std::string_view::npos ? fieldSoFar : fieldSoFar.substr(0, quotePos);
  for (const char c : before)
    if (!std::isspace(static_cast<unsigned char>(c)))
      return false;
  return true;
}

// Split one line into raw field views (separator-aware, quote-aware).
// Quoted separators do not split. The views point into the line.
inline void SplitLine(std::string_view line,
                      const char separator,
                      const char quoteChar,
                      std::vector<std::string_view> & fields)
{
  fields.clear();
  if (line.find(quoteChar) == std::string_view::npos)
  {
    std::string_view::size_type start = 0;
    while (true)
    {
      const std::string_view::size_type pos = line.find(separator, start);
      if (pos == std::string_view::npos)
      {
        fields.emplace_back(line.substr(start));
        break;
      }
      fields.emplace_back(line.substr(start, pos - start));
      start = pos + 1;
    }
    return;
  }
  std::string_view::size_type fieldStart = 0;
  Bool quoted = false;
  for (std::string_view::size_type i = 0; i < line.size(); ++ i)
  {
    const char c = line[i];
    if (c == quoteChar)
    {
      if (ShouldToggleQuote(line.substr(fieldStart, i - fieldStart), quoteChar))
        quoted = !quoted;
    }
    else if ((c == separator) && !quoted)
    {
      fields.emplace_back(line.substr(fieldStart, i - fieldStart));
      fieldStart = i + 1;
    }
  }
  fields.emplace_back(line.substr(fieldStart));
}

// Extract the raw first field of a line without splitting the whole line.
inline std::string_view FirstRawField(std::string_view line,
                                      const char separator,
                                      const char quoteChar)
{
  if (line.find(quoteChar) == std::string_view::npos)
  {
    const std::string_view::size_type pos = line.find(separator);
    return pos == std::string_view::npos ? line : line.substr(0, pos);
  }
  Bool quoted = false;
  for (std::string_view::size_type i = 0; i < line.size(); ++ i)
  {
    const char c = line[i];
    if (c == quoteChar)
    {
      if (ShouldToggleQuote(line.substr(0, i), quoteChar))
        quoted = !quoted;
    }
    else if ((c == separator) && !quoted)
      return line.substr(0, i);
  }
  return line;
}

// Split content into line views, newline character excluded.
// The last line needs no trailing newline. Views point into content.
inline std::vector<std::string_view> SplitLines(std::string_view content,
                                                std::string::size_type offset)
{
  std::vector<std::string_view> lines;
  if (content.size() > offset)
    lines.reserve(std::min((content.size() - offset) / 128, static_cast<std::string::size_type>(1) << 22));
  const char * const data = content.data();
  const std::string::size_type size = content.size();
  std::string::size_type lineStart = offset;
  while (lineStart < size)
  {
    const void * found = std::memchr(data + lineStart, '\n', size - lineStart);
    if (found == nullptr)
    {
      lines.emplace_back(data + lineStart, size - lineStart);
      break;
    }
    const std::string::size_type lineEnd = static_cast<const char *>(found) - data;
    lines.emplace_back(data + lineStart, lineEnd - lineStart);
    lineStart = lineEnd + 1;
  }
  return lines;
}

// Convert one data line into out[0, nCols), padding missing fields with NaN.
// Returns true when at least one field was successfully parsed.
inline Bool ConvertLine(std::string_view line,
                        const char separator,
                        const char quoteChar,
                        const char decimalSeparator,
                        const UnsignedInteger nCols,
                        Scalar * out,
                        std::string & tmpV,
                        std::string & tmpN,
                        std::vector<std::string_view> & fields)
{
  SplitLine(line, separator, quoteChar, fields);
  const UnsignedInteger nFields = static_cast<UnsignedInteger>(fields.size());
  const UnsignedInteger toConvert = std::min(nFields, nCols);
  Bool ok = false;
  for (UnsignedInteger j = 0; j < toConvert; ++ j)
  {
    const std::string_view view = GetFieldView(fields[j], tmpV);
    Scalar val = std::numeric_limits<Scalar>::quiet_NaN();
    if (ParseDoubleView(view, decimalSeparator, val, tmpN))
      ok = true;
    out[j] = val;
  }
  for (UnsignedInteger j = toConvert; j < nCols; ++ j)
    out[j] = std::numeric_limits<Scalar>::quiet_NaN();
  return ok;
}

// Convert UTF-16 data without BOM to UTF-8. Surrogate pairs are combined,
// unpaired surrogates and trailing odd bytes become U+FFFD.
inline std::string Utf16ToUtf8(std::string_view data, const Bool isLE)
{
  std::string utf8;
  utf8.reserve(data.size());
  std::string_view::size_type idx = 0;
  const std::string_view::size_type size = data.size();
  while ((idx + 1) < size)
  {
    uint32_t codePoint = GetUtf16Unit(data, idx, isLE);
    idx += 2;
    if ((codePoint >= 0xd800) && (codePoint <= 0xdbff) && ((idx + 1) < size))
    {
      const uint32_t lowSurrogate = GetUtf16Unit(data, idx, isLE);
      if ((lowSurrogate >= 0xdc00) && (lowSurrogate <= 0xdfff))
      {
        codePoint = 0x10000 + ((codePoint - 0xd800) << 10) + (lowSurrogate - 0xdc00);
        idx += 2;
      }
    }
    if ((codePoint >= 0xd800) && (codePoint <= 0xdfff))
      codePoint = ReplacementChar;
    AppendUtf8(codePoint, utf8);
  }
  if (idx < size)
    AppendUtf8(ReplacementChar, utf8);
  return utf8;
}
} // anonymous namespace

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
#if defined(__cplusplus) && (__cplusplus >= 202002L)
  const std::u8string u8FileName(reinterpret_cast<const char8_t*>(fileName_.data()),
                                 reinterpret_cast<const char8_t*>(fileName_.data() + fileName_.size()));
  if (!std::ifstream(std::filesystem::path{u8FileName}))
#else
  if (!std::ifstream(std::filesystem::u8path(fileName_)))
#endif
    throw FileNotFoundException(HERE) << "CSVParser cannot open file '" << fileName_ << "'";
  if (fieldSeparator_ == decimalSeparator_)
    throw InvalidArgumentException(HERE) << "The field separator must be different from the decimal separator";
  if (skippedLinesNumber_ > static_cast<UnsignedInteger>(std::numeric_limits<int>::max()))
    throw InvalidArgumentException(HERE) << "Too many skipped lines";
  const String commentMarkers(ResourceMap::GetAsString("Sample-CommentMarker"));
  if (commentMarkers.size() != 1)
    throw InvalidArgumentException(HERE) << "The entry Sample-CommentMarker must be a string of size 1";
  if (allowComments_ && (commentMarkers[0] == fieldSeparator_ || commentMarkers[0] == decimalSeparator_))
    throw InvalidArgumentException(HERE) << "The comment marker must be different from the field and decimal separators";
  const char commentPrefix = commentMarkers[0];
  std::ifstream stream;
  stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
#if defined(__cplusplus) && (__cplusplus >= 202002L)
  stream.open(std::filesystem::path {u8FileName}, std::ios::binary);
#else
  stream.open(std::filesystem::u8path(fileName_), std::ios::binary);
#endif
  stream.seekg(0, std::ios::end);
  const std::streamoff fileLength = stream.tellg();
  stream.seekg(0, std::ios::beg);
  // Read the whole file and parse it without storing all cells as strings.
  // UTF-16 files are converted to UTF-8 first.
  std::string content;
  const std::string::size_type rawSize = fileLength > 0 ? static_cast<std::string::size_type>(fileLength) : 0;
  if (rawSize >= 2)
  {
    char bom2[2] = {0, 0};
    stream.read(bom2, 2);
    stream.seekg(0, std::ios::beg);
    const Bool isLE = (bom2[0] == '\xff' && bom2[1] == '\xfe');
    const Bool isBE = (bom2[0] == '\xfe' && bom2[1] == '\xff');
    if (isLE || isBE)
    {
      std::string raw;
      raw.resize(rawSize);
      stream.read(raw.data(), static_cast<std::streamsize>(rawSize));
      content = Utf16ToUtf8(std::string_view(raw.data() + 2, rawSize - 2), isLE);
    }
  }
  if (content.empty())
  {
    content.resize(rawSize);
    if (rawSize > 0)
      stream.read(content.data(), static_cast<std::streamsize>(rawSize));
  }
  const char * const buf = content.data();
  const std::string::size_type n = content.size();
  std::string::size_type offset = 0;
  if (n >= 3 && static_cast<unsigned char>(buf[0]) == 0xef && static_cast<unsigned char>(buf[1]) == 0xbb && static_cast<unsigned char>(buf[2]) == 0xbf)
    offset = 3;
  const char fieldSeparator = fieldSeparator_;
  const char decimalSeparator = decimalSeparator_;
  const char quoteChar = '"';
  const Bool useComments = allowComments_;
  const Bool skipEmpty = allowEmptyLines_;
  const UnsignedInteger skippedLines = skippedLinesNumber_;
  UnsignedInteger keptSeen = 0;
  UnsignedInteger lastSkippedWidth = 0;
  std::string tmpView;
  std::string tmpNum;
  std::vector<std::string_view> fields;
  fields.reserve(32);
  const std::string_view contentView(buf, n);
  // Sequential pass over lines, dropping empty/comment/skipped ones.
  // Lines are independent as quoted line breaks are not supported.
  std::vector<std::string_view> lines = SplitLines(contentView, offset);
  std::vector<std::string_view> keptLines;
  keptLines.reserve(lines.size());
  for (std::string_view line : lines)
  {
    // skip truly empty lines (no separator and nothing but carriage returns)
    if (skipEmpty && line.find(fieldSeparator) == std::string_view::npos && RawFieldIsEmpty(line))
      continue;
    // skip comment lines from their first field (trimmed/unquoted view)
    if (useComments)
    {
      const std::string_view firstField = GetFieldView(FirstRawField(line, fieldSeparator, quoteChar), tmpView);
      if (!firstField.empty() && firstField.front() == commentPrefix)
        continue;
    }
    if (keptSeen < skippedLines)
    {
      SplitLine(line, fieldSeparator, quoteChar, fields);
      lastSkippedWidth = static_cast<UnsignedInteger>(fields.size());
      ++ keptSeen;
      continue;
    }
    keptLines.push_back(line);
    ++ keptSeen;
  }
  lines.clear();
  lines.shrink_to_fit();
  // no data rows: deduce dimension from skipped rows if possible
  if (keptLines.empty())
  {
    UnsignedInteger dim = 0;
    if (skippedLines > 0 && keptSeen >= skippedLines)
      dim = lastSkippedWidth;
    Sample result(0, dim);
    result.setDescription(Description::BuildDefault(result.getDimension(), "data_"));
    result.setName(fileName_);
    return result;
  }
  // column count from first data row, or from last skipped row
  UnsignedInteger nCols = 0;
  if (skippedLines > 0)
    nCols = lastSkippedWidth;
  else
  {
    SplitLine(keptLines[0], fieldSeparator, quoteChar, fields);
    nCols = static_cast<UnsignedInteger>(fields.size());
  }
  if (nCols == 0)
  {
    Sample result(0, 0);
    result.setDescription(Description::BuildDefault(result.getDimension(), "data_"));
    result.setName(fileName_);
    return result;
  }
  // convert first row sequentially (also feeds header detection)
  std::vector<std::string> firstRowStrings(nCols);
  std::vector<Scalar> firstRowValues(nCols, std::numeric_limits<Scalar>::quiet_NaN());
  Bool firstOk = false;
  {
    SplitLine(keptLines[0], fieldSeparator, quoteChar, fields);
    const UnsignedInteger toConvert = std::min(static_cast<UnsignedInteger>(fields.size()), nCols);
    for (UnsignedInteger j = 0; j < toConvert; ++ j)
    {
      const std::string_view view = GetFieldView(fields[j], tmpView);
      firstRowStrings[j].assign(view.data(), view.size());
      if (ParseDoubleView(view, decimalSeparator, firstRowValues[j], tmpNum))
        firstOk = true;
    }
  }
  const UnsignedInteger totalRows = static_cast<UnsignedInteger>(keptLines.size());
  Bool haveHeaders = false;
  const char * specList[] = {"inf", "-inf", "INF", "-INF", "Inf", "-Inf", "nan", "NAN", "NaN"};
  const std::string::size_type specSize = 9;
  for (UnsignedInteger j = 0; j < nCols; ++ j)
  {
    if (!firstRowStrings[j].empty() && std::isnan(firstRowValues[j]))
    {
      Bool isSpecial = false;
      for (std::string::size_type k = 0; k < specSize; ++ k)
        if (firstRowStrings[j] == specList[k])
        {
          isSpecial = true;
          break;
        }
      if (!isSpecial)
      {
        haveHeaders = true;
        break;
      }
    }
  }
  // allocate the result and convert remaining rows, possibly in parallel.
  // Lines are independent as quoted line breaks are not supported.
  const UnsignedInteger resultSize = totalRows - (haveHeaders ? 1 : 0);
  Sample result(resultSize, nCols);
  struct ConvertRowsFunctor
  {
    const std::vector<std::string_view> & lines;
    const char separator;
    const char quoteChar;
    const char decimalSeparator;
    const UnsignedInteger nCols;
    const Bool shift;
    SampleImplementation::data_iterator out;
    std::vector<char> & rowOk;
    void operator()(const TBBImplementation::BlockedRange<UnsignedInteger> & r) const
    {
      std::string tmpV;
      std::string tmpN;
      std::vector<std::string_view> localFields;
      localFields.reserve(nCols + 1);
      for (UnsignedInteger i = r.begin(); i < r.end(); ++ i)
      {
        Scalar * outRow = &out[static_cast<std::vector<Scalar>::size_type>(i - (shift ? 1 : 0)) * nCols];
        if (ConvertLine(lines[i], separator, quoteChar, decimalSeparator, nCols, outRow, tmpV, tmpN, localFields))
          rowOk[i] = 1;
      }
    }
  };
  // per-row success flags (char, as vector<Bool> bit-packing is not thread-safe)
  std::vector<char> rowOk(totalRows, 0);
  rowOk[0] = firstOk ? 1 : 0;
  if (totalRows > 1)
  {
    ConvertRowsFunctor functor = {keptLines, fieldSeparator, quoteChar, decimalSeparator, nCols, haveHeaders, result.getImplementation()->data_begin(), rowOk};
    // first data line is already converted, convert lines 1..N-1
    TBBImplementation::ParallelForIf(totalRows > 8192, 1, totalRows, functor, 1024);
    if (!haveHeaders)
    {
      SampleImplementation::data_iterator out = result.getImplementation()->data_begin();
      for (UnsignedInteger j = 0; j < nCols; ++ j)
        out[j] = firstRowValues[j];
    }
  }
  else if (!haveHeaders)
  {
    SampleImplementation::data_iterator out = result.getImplementation()->data_begin();
    for (UnsignedInteger j = 0; j < nCols; ++ j)
      out[j] = firstRowValues[j];
  }
  Bool oneOk = false;
  for (std::vector<char>::size_type i = 0; i < rowOk.size(); ++ i)
    if (rowOk[i])
    {
      oneOk = true;
      break;
    }
  if (totalRows > (haveHeaders ? 1u : 0u) && !oneOk)
    throw InvalidArgumentException(HERE) << "Could not parse CSV file '" << fileName_ << "' using delimiter '" << fieldSeparator_ << "'";
  static const std::string utf8BOM("\xef\xbb\xbf");
  if (haveHeaders)
  {
    Description description(nCols);
    for (UnsignedInteger j = 0; j < nCols; ++ j)
    {
      description[j] = firstRowStrings[j];
      String::size_type pos = 0;
      while ((pos = description[j].find(utf8BOM, pos)) != String::npos)
        description[j].erase(pos, utf8BOM.size());
      if (description[j].empty())
        description[j] = OSS() << "Unnamed_" << j;
    }
    result.setDescription(description);
    result.setName(fileName_);
    return result;
  }
  result.setDescription(Description::BuildDefault(result.getDimension(), "data_"));
  result.setName(fileName_);
  return result;
}


END_NAMESPACE_OPENTURNS
