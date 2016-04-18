//                                               -*- C++ -*-
/**
 *  @brief The class Ostream is an helper for pretty printing of Objects
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
#ifndef OPENTURNS_OSTREAM_HXX
#define OPENTURNS_OSTREAM_HXX

#include <iostream>              // for std::ostream
#include <iomanip>               // for manipulators like setw, setprecision, etc.
#include "openturns/Object.hxx"

BEGIN_NAMESPACE_OPENTURNS



// This class defines an new ostream based stream
class OT_API OStream
{
  std::ostream & os_;
public:
  explicit OStream(std::ostream & os) : os_(os) {}
  std::ostream & getStream()
  {
    return os_;
  }
}; // end class OStream

// This operator writes Object derived objects to OStream
OT_API
OStream & operator << (OStream & OS, const Object & obj);

OT_API
OStream & operator << (OStream & OS, const String & st);

OT_API
OStream & operator << (OStream & OS, const char * ch);

OT_API
OStream & operator << (OStream & OS, int val);

OT_API
OStream & operator << (OStream & OS, long val);

OT_API
OStream & operator << (OStream & OS, long long val);

OT_API
OStream & operator << (OStream & OS, short val);

OT_API
OStream & operator << (OStream & OS, unsigned int val);

OT_API
OStream & operator << (OStream & OS, unsigned long val);

OT_API
OStream & operator << (OStream & OS, unsigned long long val);

OT_API
OStream & operator << (OStream & OS, unsigned short val);

OT_API
OStream & operator << (OStream & OS, char val);

OT_API
OStream & operator << (OStream & OS, bool val);

OT_API
OStream & operator << (OStream & OS, float val);

OT_API
OStream & operator << (OStream & OS, double val);

OT_API
OStream & operator << (OStream & OS, long double val);

OT_API
OStream & operator << (OStream & OS, const NumericalComplex & c);

OT_API
OStream & operator << (OStream & OS, const void * ptr);

OT_API
OStream & operator << (OStream & OS, std::ostream & (*manip)(std::ostream &));

OT_API
OStream & operator << (OStream & OS, std::ios_base & (*manip)(std::ios_base &));

#ifdef _MSC_VER

template<typename T>
inline OStream & operator << (OStream & OS, std::_Smanip<T> manip)
{
  OS.getStream() << manip;
  return OS;
}

template <typename T>
inline OStream & operator << (OStream & OS, std::_Fillobj<T> manip)
{
  OS.getStream() << manip;
  return OS;
}

#elif defined(__clang__) && defined(_LIBCPP_VERSION)
inline OStream & operator << (OStream & OS, std::__1::__iom_t6 manip)
{
  OS.getStream() << manip;
  return OS;
}

template <typename T>
inline OStream & operator << (OStream & OS, std::__1::__iom_t4<T> manip)
{
  OS.getStream() << manip;
  return OS;
}


inline OStream & operator << (OStream & OS, std::__1::__iom_t5 manip)
{
  OS.getStream() << manip;
  return OS;
}
#elif defined(__GNUC__)

OT_API
OStream & operator << (OStream & OS, std::_Setw manip);

OT_API
OStream & operator << (OStream & OS, std::_Setprecision manip);

OT_API
OStream & operator << (OStream & OS, std::_Setbase manip);

OT_API
OStream & operator << (OStream & OS, std::_Resetiosflags manip);

OT_API
OStream & operator << (OStream & OS, std::_Setiosflags manip);

OT_API
OStream & operator << (OStream & OS, std::_Setfill<char> manip);
#endif


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_OSTREAM_HXX */
