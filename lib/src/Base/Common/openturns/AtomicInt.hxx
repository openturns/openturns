//                                               -*- C++ -*-
/**
 *  @brief This file supplies some atomic functions to support multithreading
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
#ifndef OPENTURNS_ATOMICINT_HXX
#define OPENTURNS_ATOMICINT_HXX

#include <atomic>

#include "openturns/OTtypes.hxx"
#include "openturns/OTdebug.h"

BEGIN_NAMESPACE_OPENTURNS


class OT_API AtomicInt
{

  typedef std::atomic<int> Integer;
  Integer val_;

public:

  AtomicInt(int v = 0) : val_(v)
  {}

  // marked as delete in std::atomic
  AtomicInt(const AtomicInt & other) : val_(other.val_.load())
  {}

  inline
  AtomicInt & operator = (int v)
  {
    val_ = v;
    return *this;
  }

  // marked as delete in std::atomic
  inline
  AtomicInt & operator = (const AtomicInt & other)
  {
    if (this != &other)
      val_ = other.val_.load();
    return *this;
  }

  // Get p value, increment it by d and return the old value
  inline
  int fetchAndAdd(int d)
  {
    return val_.fetch_add(d);
  }

  // Get p value, increment it by d and return the old value
  inline
  int fetchOr(int d)
  {
    return val_.fetch_or(d);
  }

  inline
  void increment()
  {
    ++ val_;
  }

  inline
  void decrement()
  {
    -- val_;
  }

  inline
  int get() const
  {
    return val_.load();
  }

}; // AtomicInt

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ATOMICINT_HXX */
