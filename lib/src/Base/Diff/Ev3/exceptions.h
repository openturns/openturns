/**********************************************************************
 * Author:      Leo Liberti                                            *
 * Name:        exceptions.h                                           *
 * Source:      GNU C++                                                *
 * Purpose:     Expression v3 exceptions                               *
 * History:     010622 0.0 work started                                *
 * License:     (C) Leo Liberti, all rights reserved. Code published under the
               Common Public License.
***********************************************************************/

#ifndef __EV3EXCEPTIONSH__
#define __EV3EXCEPTIONSH__

#include <string>
#include <iostream>

#define HELPURL "http://liberti.dhs.org/"
#define NONE "[none]"
#define STDACTION //std::cerr << interface_ << "::" << scope_ << ": in [" << operation_ << "]: " << description_ << ", code = " << code_ << ", see " << moreinfo_ << std::endl

namespace Ev3
{
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
    STDACTION;
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
    STDACTION;
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
    STDACTION;
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
    STDACTION;
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
    STDACTION;
  }
};
} /* namespace Ev3 */
#endif /* __EV3EXCEPTIONSH__ */
