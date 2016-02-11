//                                               -*- C++ -*-
/**
 *  @brief This file provides a definition for char traits for old versions of GCC
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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

#ifndef OPENTURNS_XMLCHAR_TRAITS_HXX
#define OPENTURNS_XMLCHAR_TRAITS_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/OTconfig.hxx"
#include <string>

#if defined OPENTURNS_HAVE_LIBXML2

#if defined(__GNUC__) && GCC_VERSION < 30400 /* below GCC 3.4.0 */
#include <libxml/xmlstring.h>

namespace std
{

template<typename _CharT>
static void
char_traits<_CharT>::
assign(char_type& __c1, const char_type& __c2)
{
  __c1 = __c2;
}

template<typename _CharT>
static bool
char_traits<_CharT>::
eq(const char_type& __c1, const char_type& __c2)
{
  return __c1 == __c2;
}

template<typename _CharT>
static bool
char_traits<_CharT>::
lt(const char_type& __c1, const char_type& __c2)
{
  return __c1 < __c2;
}

template<typename _CharT>
static typename char_traits<_CharT>::char_type
char_traits<_CharT>::
to_char_type(const int_type& __c)
{
  return static_cast<char_type>(__c);
}

template<typename _CharT>
static typename char_traits<_CharT>::int_type
char_traits<_CharT>::
to_int_type(const char_type& __c)
{
  return static_cast<int_type>(__c);
}

template<typename _CharT>
static bool
char_traits<_CharT>::
eq_int_type(const int_type& __c1, const int_type& __c2)
{
  return __c1 == __c2;
}

template<typename _CharT>
static typename char_traits<_CharT>::int_type
char_traits<_CharT>::
eof()
{
  return static_cast<int_type>(EOF);
}

template<typename _CharT>
static typename char_traits<_CharT>::int_type
char_traits<_CharT>::
not_eof(const int_type& __c)
{
  return !eq_int_type(__c, eof()) ? __c : to_int_type(char_type());
}

template<typename _CharT>
static int
char_traits<_CharT>::
compare(const char_type* __s1, const char_type* __s2, std::size_t __n)
{
  for (std::size_t __i = 0; __i < __n; ++__i)
    if (lt(__s1[__i], __s2[__i]))
      return -1;
    else if (lt(__s2[__i], __s1[__i]))
      return 1;
  return 0;
}

template<typename _CharT>
static std::size_t
char_traits<_CharT>::
length(const char_type* __p)
{
  std::size_t __i = 0;
  while (!eq(__p[__i], char_type()))
    ++__i;
  return __i;
}

template<typename _CharT>
static const typename char_traits<_CharT>::char_type*
char_traits<_CharT>::
find(const char_type* __s, std::size_t __n, const char_type& __a)
{
  for (std::size_t __i = 0; __i < __n; ++__i)
    if (eq(__s[__i], __a))
      return __s + __i;
  return 0;
}

template<typename _CharT>
static typename char_traits<_CharT>::char_type*
char_traits<_CharT>::
move(char_type* __s1, const char_type* __s2, std::size_t __n)
{
  return static_cast<_CharT*>(memmove(__s1, __s2,
                                      __n * sizeof(char_type)));
}

template<typename _CharT>
static typename char_traits<_CharT>::char_type*
char_traits<_CharT>::
copy(char_type* __s1, const char_type* __s2, std::size_t __n)
{
  // NB: Inline std::copy so no recursive dependencies.
  std::copy(__s2, __s2 + __n, __s1);
  return __s1;
}

template<typename _CharT>
static typename char_traits<_CharT>::char_type*
char_traits<_CharT>::
assign(char_type* __s, std::size_t __n, char_type __a)
{
  // NB: Inline std::fill_n so no recursive dependencies.
  std::fill_n(__s, __n, __a);
  return __s;
}


}

#endif

#endif /* HAS_LIXML2 */


#endif /* OPENTURNS_XMLCHAR_TRAITS_HXX */
