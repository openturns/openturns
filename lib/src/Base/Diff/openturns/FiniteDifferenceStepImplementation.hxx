//                                               -*- C++ -*-
/**
 *  @brief Implementation of finite difference step
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

#ifndef OPENTURNS_FINITEDIFFERENCESTEPIMPLEMENTATION_HXX
#define OPENTURNS_FINITEDIFFERENCESTEPIMPLEMENTATION_HXX

#include "openturns/Point.hxx"


BEGIN_NAMESPACE_OPENTURNS

class OT_API FiniteDifferenceStepImplementation
  : public PersistentObject
{

  CLASSNAME
public:

  /** Default constructor */
  FiniteDifferenceStepImplementation();

  /** Parameters constructor */
  explicit FiniteDifferenceStepImplementation(const Point & epsilon);

  /** String converter */
  virtual String __repr__() const;

  /** Epsilon accessor */
  virtual void setEpsilon(const Point & epsilon);
  virtual Point getEpsilon() const;

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  virtual FiniteDifferenceStepImplementation * clone() const;

  /** Compute value */
  virtual Point operator()(const Point & inP) const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:
  /* The small increments */
  Point epsilon_;

};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FINITEDIFFERENCESTEPIMPLEMENTATION_HXX */
