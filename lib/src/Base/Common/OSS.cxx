//                                               -*- C++ -*-
/**
 *  @brief The class OSS streams out objects
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
#include "openturns/OSS.hxx"
#include "openturns/PlatformInfo.hxx"

BEGIN_NAMESPACE_OPENTURNS

OSS::OSS(bool full) : oss_(), precision_(PlatformInfo::GetNumericalPrecision()), full_(full) {}

OSS::operator std::string() const
{
  return oss_.str();
}

std::string OSS::str() const
{
  return oss_.str();
}

void OSS::clear()
{
  // To clear the content
  oss_.str("");
  // To clear the flags
  oss_.clear();
}

/* String formatter with blank space right padding */
String OSS::PadString(const String & string, const UnsignedInteger columnWidth)
{
  const UnsignedInteger rightPadding = string.size() < columnWidth ? columnWidth - string.size() : 0;
  OSS oss;
  oss << string << String(rightPadding, ' ');
  return oss;
}

/* Repeat a String */
String OSS::RepeatString(const UnsignedInteger repeatNb, const String & string)
{
  String result = OSS();
  for (UnsignedInteger i = 0 ; i < repeatNb; ++ i)
    result += string;
  return result;
}

END_NAMESPACE_OPENTURNS
