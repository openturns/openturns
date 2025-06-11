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
#ifndef OPENTURNS_CSVPARSER_HXX
#define OPENTURNS_CSVPARSER_HXX

#include "openturns/Object.hxx"
#include "openturns/Sample.hxx"
#include <locale>

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CSVParser
 */

class OT_API CSVParser : public Object
{
  CLASSNAME
public:

  /** Default constructor without parameters */
  CSVParser();

  /** Parameters constructor */
  explicit CSVParser(const String & fileName);

  /** Options accessors */
  void setFieldSeparator(const char fieldSeparator);
  void setAllowComments(const Bool allowComments);
  void setAllowEmptyLines(const Bool allowEmptyLines);
  void setSkippedLinesNumber(const UnsignedInteger skippedLinesNumber);
  void setNumericalSeparator(const char decimalSeparator);

  Sample load() const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  struct CSVParserFormat : std::numpunct<char>
  {
    explicit CSVParserFormat(const char_type decimalSeparator)
      : decimalSeparator_(decimalSeparator) {}

    char_type do_decimal_point() const
    {
      return decimalSeparator_;
    }
    char_type decimalSeparator_;
  };

private:
  String fileName_;

  // options
  char fieldSeparator_ = ',';
  Bool allowComments_ = false;
  Bool allowEmptyLines_ = false;
  UnsignedInteger skippedLinesNumber_ = 0;
  char decimalSeparator_ = '.';

}; /* class CSVParser */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FILTERINWINDOWSIMPLEMENTATION_HXX */
