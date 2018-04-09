//                                               -*- C++ -*-
/**
 *  @brief The class Lockable defines an interface for the lock functionality
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_LOCKABLE_HXX
#define OPENTURNS_LOCKABLE_HXX

#include "openturns/OTprivate.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class Lockable
 * @brief Makes a class lockable for multithreading
 * @internal
 *
 * Lockable is an interface (an abstract class) intended to
 * be derived by any class that need to be run in a multithreaded
 * environment. It provides a lock mechanism that protect the object
 * from being concurrently accessed and then being corrupted.
 * The embedded Lock class is a scoped-lock, ie the lock is
 * guaranted during the lifetime of the Lock object.
 */
class OT_API Lockable
{
public:



  /**
   * @class Lock
   * @brief Implements a myltithreaded lock
   * @internal
   *
   * This class provides a mechanism of scoped lock.
   */
  class Lock
  {
  public:

    /**
     * Usual constructor
     *
     * Take the mutex of the Lockable Object
     * @param p_lockableObject A pointer to the object to be locked
     */
    Lock(const Lockable * p_lockableObject);

    /*
     * Constructor with another mutex
     */
    //Lock(pthread_mutex_t & mutex);

    /**
     * Destructor
     *
     * Release the mutex of the Lockable Object
     */
    virtual ~Lock();

  protected:

  private:
    /** A pointer to the mutex to lock/unlock */
    //pthread_mutex_t * p_mutex_;

  }; /* class Lock */




  friend class Lock;

  /**
   * Default constructor
   *
   * Initialize the mutex
   */
  Lockable();

  /**
   * Copy constructor
   *
   * Initialize the mutex
   */
  Lockable(const Lockable & other);

  /**
   * Destructor
   *
   * Destroy the mutex
   */
  virtual ~Lockable();

protected:
  /** The mutex that protect the object */
  //mutable pthread_mutex_t mutex_;

private:


}; /* class Lockable */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LOCKABLE_HXX */
