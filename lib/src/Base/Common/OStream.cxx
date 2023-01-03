//                                               -*- C++ -*-
/**
 *  @brief The class Ostream is an helper for pretty printing of Objects
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/OTprivate.hxx"
#include "openturns/OStream.hxx"

BEGIN_NAMESPACE_OPENTURNS


// This operator writes Object derived objects to OStream

OStream & operator << (OStream & OS, const Object & obj)
{
  OS.getStream() << obj.__repr__();
  return OS;
}


OStream & operator << (OStream & OS, const String & st)
{
  OS.getStream() << st;
  return OS;
}


OStream & operator << (OStream & OS, const char * ch)
{
  OS.getStream() << ch;
  return OS;
}


OStream & operator << (OStream & OS, int val)
{
  OS.getStream() << val;
  return OS;
}


OStream & operator << (OStream & OS, long val)
{
  OS.getStream() << val;
  return OS;
}


#ifndef OPENTURNS_UNSIGNEDLONG_SAME_AS_UINT64
OStream & operator << (OStream & OS, int64_t val)
{
  OS.getStream() << val;
  return OS;
}
#endif


OStream & operator << (OStream & OS, short val)
{
  OS.getStream() << val;
  return OS;
}


OStream & operator << (OStream & OS, unsigned int val)
{
  OS.getStream() << val;
  return OS;
}


OStream & operator << (OStream & OS, unsigned long val)
{
  OS.getStream() << val;
  return OS;
}


#ifndef OPENTURNS_UNSIGNEDLONG_SAME_AS_UINT64
OStream & operator << (OStream & OS, uint64_t val)
{
  OS.getStream() << val;
  return OS;
}
#endif


OStream & operator << (OStream & OS, unsigned short val)
{
  OS.getStream() << val;
  return OS;
}


OStream & operator << (OStream & OS, char val)
{
  OS.getStream() << val;
  return OS;
}


OStream & operator << (OStream & OS, bool val)
{
  OS.getStream() << (val ? "true" : "false");
  return OS;
}


OStream & operator << (OStream & OS, float val)
{
  OS.getStream() << val;
  return OS;
}


OStream & operator << (OStream & OS, double val)
{
  OS.getStream() << val;
  return OS;
}


OStream & operator << (OStream & OS, long double val)
{
  OS.getStream() << val;
  return OS;
}


OStream & operator << (OStream & OS, const Complex & c)
{
  OS.getStream() << c;
  return OS;
}


OStream & operator << (OStream & OS, const void * ptr)
{
  OS.getStream() << ptr;
  return OS;
}


OStream & operator << (OStream & OS, std::ostream & (*manip)(std::ostream &))
{
  OS.getStream() << manip;
  return OS;
}


OStream & operator << (OStream & OS, std::ios_base & (*manip)(std::ios_base &))
{
  OS.getStream() << manip;
  return OS;
}

#if defined(__GNUC__) && !defined(_LIBCPP_VERSION)

OStream & operator << (OStream & OS, std::_Setw manip)
{
  OS.getStream() << manip;
  return OS;
}


OStream & operator << (OStream & OS, std::_Setprecision manip)
{
  OS.getStream() << manip;
  return OS;
}


OStream & operator << (OStream & OS, std::_Setbase manip)
{
  OS.getStream() << manip;
  return OS;
}



OStream & operator << (OStream & OS, std::_Resetiosflags manip)
{
  OS.getStream() << manip;
  return OS;
}


OStream & operator << (OStream & OS, std::_Setiosflags manip)
{
  OS.getStream() << manip;
  return OS;
}


OStream & operator << (OStream & OS, std::_Setfill<char> manip)
{
  OS.getStream() << manip;
  return OS;
}

#endif

END_NAMESPACE_OPENTURNS

