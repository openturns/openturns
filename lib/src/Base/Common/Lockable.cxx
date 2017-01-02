//                                               -*- C++ -*-
/**
 *  @brief The class Lockable defines an interface for the lock functionality
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/Lockable.hxx"

BEGIN_NAMESPACE_OPENTURNS



/* Default constructor */
Lockable::Lockable()
{
  // Initialize the mutex
  //pthread_mutex_init(&mutex_, NULL);
}

/* Copy constructor */
Lockable::Lockable(const Lockable & other ///< The copied Thread
                  )
{
  // Initialize the mutex
  //pthread_mutex_init(&mutex_, NULL);
}

/* Destructor */
Lockable::~Lockable()
{
  // Destroy the mutex
  //pthread_mutex_destroy(&mutex_);
}






/* Usual constructor */
Lockable::Lock::Lock(const Lockable * p_lockableObject)
//: p_mutex_(&p_lockableObject->mutex_)
{
  // Take the mutex
  //pthread_mutex_lock(p_mutex_);
}

/* Constructor with another mutex */
//       Lockable::Lock::Lock(pthread_mutex_t & mutex)
//        : p_mutex_(&mutex)
//       {
//        // Take the mutex
//         pthread_mutex_lock(p_mutex_);
//       }

/* Destructor */
Lockable::Lock::~Lock()
{
  // Release the mutex
  //pthread_mutex_unlock(p_mutex_);
}




END_NAMESPACE_OPENTURNS
