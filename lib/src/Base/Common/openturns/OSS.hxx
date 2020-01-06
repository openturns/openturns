//                                               -*- C++ -*-
/**
 *  @brief The class OSS streams out objects
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
#ifndef OPENTURNS_OSS_HXX
#define OPENTURNS_OSS_HXX

#include <sstream>
#include <iterator>
#include <functional>
#include <utility>        // for std::pair
#include "openturns/OStream.hxx"

BEGIN_NAMESPACE_OPENTURNS

template <typename U, typename V>
std::ostream & operator << (std::ostream & os, const std::pair<U, V> & p)
{
  return os << "(" << p.first << "," << p.second << ")";
}

template <typename U, typename V>
OStream & operator << (OStream & OS, const std::pair<U, V> & p)
{
  return OS << "(" << p.first << "," << p.second << ")";
}



/**
 * Struct Bulk protects the underlying object output from being
 * modified by OSSFormater
 */
template <class T>
struct Bulk
{
  T data_;
  Bulk(T data) : data_(data) {}
  operator T() const
  {
    return data_;
  }
};

/**
 * Struct OSSFormater is a helper class for OSS that adapt the
 * format of output according to type T
 */

/* Any type */
template <class T>
struct OSSFormater
{
  inline
  static
  void apply(std::ostringstream & oss, T obj, int /*precision*/)
  {
    oss << obj;
  }

  inline
  static
  void apply(OStream & OS, T obj, int /*precision*/)
  {
    OS << obj;
  }
}; /* struct OSSFormater */

/* double */
template <>
struct OSSFormater<double>
{
  inline
  static
  void apply(std::ostringstream & oss, double d, int precision)
  {
    int oldPrecision = static_cast<int>(oss.precision(precision));
    oss << d;
    oss.precision(oldPrecision);
  }

  inline
  static
  void apply(OStream & OS, double d, int precision)
  {
    int oldPrecision = static_cast<int>(OS.getStream().precision(precision));
    OS.getStream() << d;
    OS.getStream().precision(oldPrecision);
  }
};

/* float */
template <>
struct OSSFormater<float>
{
  inline
  static
  void apply(std::ostringstream & oss, float f, int precision)
  {
    int oldPrecision = static_cast<int>(oss.precision(precision));
    oss << f;
    oss.precision(oldPrecision);
  }

  inline
  static
  void apply(OStream & OS, float f, int precision)
  {
    int oldPrecision = static_cast<int>(OS.getStream().precision(precision));
    OS.getStream() << f;
    OS.getStream().precision(oldPrecision);
  }
};

/* bool */
template <>
struct OSSFormater<bool>
{
  inline
  static
  void apply(std::ostringstream & oss, bool b, int /*precision*/)
  {
    oss << (b ? "true" : "false");
  }

  inline
  static
  void apply(OStream & OS, bool b, int /*precision*/)
  {
    OS.getStream() << (b ? "true" : "false");
  }
};






/**
 * Class OSS is useful when streaming data through a function
 * that expect a string as parameter
 */
class OT_API OSS
{
private:
  std::ostringstream oss_;
  mutable int precision_;
  mutable bool full_;

public:
  explicit OSS(bool full = true);

  template <class T>
  inline
  OSS & operator << (T obj)
  {
    if (full_)
    {
      OStream OS(oss_);
      OSSFormater<T>::apply(OS, obj, precision_);
    }
    else OSSFormater<T>::apply(oss_, obj, precision_);
    return *this;
  }

  inline
  OSS & setPrecision(int precision)
  {
    precision_ = precision;
    oss_.precision(precision_);
    return *this;
  }

  inline
  int getPrecision() const
  {
    precision_ = static_cast<int>(oss_.precision());
    return precision_;
  }

  operator std::string() const;
  std::string str() const;

  void clear();

}; /* class OSS */

template <typename _Tp>
struct AllElementsPredicate : public std::unary_function<_Tp, Bool>
{
  Bool
  operator()(const _Tp&) const
  {
    return true;
  }
};

template < typename _Tp, typename _UnaryPredicate = AllElementsPredicate<_Tp>, typename _CharT = char,
           typename _Traits = std::char_traits<_CharT> >
class OSS_iterator
  : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
public:
  //@{
  /// Public typedef
  typedef _CharT                         char_type;
  typedef _Traits                        traits_type;
  typedef OSS                            ostream_type;
  //@}

private:
  ostream_type*     _M_stream;
  String            _M_string;
  String            _M_prefix;
  mutable bool      _M_first;

public:
  /**
   *  Construct from an ostream
   *
   *  The delimiter string @a c is written to the stream after every Tp
   *  written to the stream.  The delimiter is not copied, and thus must
   *  not be destroyed while this iterator is in use.
   *
   *  @param  s  Underlying ostream to write to.
   *  @param  c  CharT delimiter string to insert.
   */
  OSS_iterator(ostream_type & __s, const String & __c = "", const String & __p = "")
    : _M_stream(&__s)
    , _M_string(__c)
    , _M_prefix(__p)
    , _M_first(true)
  {}

  /// Copy constructor.
  OSS_iterator(const OSS_iterator& __obj)
    : _M_stream(__obj._M_stream)
    , _M_string(__obj._M_string)
    , _M_prefix(__obj._M_prefix)
    , _M_first(__obj._M_first)
  {}

  /// Copy assignment
  OSS_iterator&
  operator=(const OSS_iterator& obj)
  {
    _M_stream = obj._M_stream;
    _M_string = obj._M_string;
    _M_prefix = obj._M_prefix;
    _M_first  = obj._M_first;
    return *this;
  }

  /// Writes @a value to underlying ostream using operator<<.  If
  /// constructed with delimiter string, writes delimiter to ostream.
  OSS_iterator&
  operator=(const _Tp& value)
  {
    if (_UnaryPredicate()(value))
    {
      if (!_M_first) *_M_stream << _M_string;
      *_M_stream << _M_prefix << value;
      _M_first = false;
    }
    return *this;
  }

  OSS_iterator&
  operator*()
  {
    return *this;
  }

  OSS_iterator&
  operator++()
  {
    return *this;
  }

  OSS_iterator
  operator++(int)
  {
    return *this;
  }

};


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_OSS_HXX */
