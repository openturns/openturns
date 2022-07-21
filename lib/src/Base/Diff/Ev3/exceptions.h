//                                               -*- C++ -*-
/**
 *  @brief Expression v3 exceptions
 *
 *  Copyright (C) 2008-2010 Leo Liberti
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

#ifndef __EV3EXCEPTIONSH__
#define __EV3EXCEPTIONSH__

#include <string>
#include <iostream>

namespace Ev3
{
const std::string HELPURL = "http://liberti.dhs.org/";
const std::string NONE = "[none]";
inline void STDACTION()
{
  //std::cerr << interface_ << "::" << scope_ << ": in [" << operation_ << "]: " << description_ << ", code = " << code_ << ", see " << moreinfo_ << std::endl
}

class ErrBase
{
public:
  unsigned long code_;
  std::string interface_;
  std::string scope_;
  std::string operation_;
  std::string description_;
  std::string moreinfo_;
  ErrBase() :
    code_(0),
    interface_(NONE),
    scope_(NONE),
    operation_(NONE),
    description_(NONE),
    moreinfo_(HELPURL)
  {
    STDACTION();
  }
  ErrBase(const unsigned long mycode,
          const std::string & myif,
          const std::string & myscope,
          const std::string & myop,
          const std::string & mydesc,
          const std::string & myinfo) :
    code_(mycode),
    interface_(myif),
    scope_(myscope),
    operation_(myop),
    description_(mydesc),
    moreinfo_(myinfo)
  {
    STDACTION();
  }
};

class ErrUnknown : public ErrBase
{
public:
  ErrUnknown(const unsigned long mycode,
             const std::string & myif,
             const std::string & myscope,
             const std::string & myop,
             const std::string & mydesc,
             const std::string & myinfo) :
    ErrBase(mycode, myif, myscope, myop, mydesc, myinfo)
  {
    STDACTION();
  }
};

class ErrNotPermitted : public ErrBase
{
public:
  ErrNotPermitted(const unsigned long mycode,
                  const std::string & myif,
                  const std::string & myscope,
                  const std::string & myop,
                  const std::string & mydesc,
                  const std::string & myinfo) :
    ErrBase(mycode, myif, myscope, myop, mydesc, myinfo)
  {
    STDACTION();
  }
};

class ErrDivideByZero : public ErrBase
{
public:
  std::string dividend;
  ErrDivideByZero(const unsigned long mycode,
                  const std::string & myif,
                  const std::string & myscope,
                  const std::string & myop,
                  const std::string & mydesc,
                  const std::string & myinfo,
                  const std::string & mydiv) :
    ErrBase(mycode, myif, myscope, myop, mydesc, myinfo),
    dividend(mydiv)
  {
    STDACTION();
  }
};
} /* namespace Ev3 */
#endif /* __EV3EXCEPTIONSH__ */
