//                                               -*- C++ -*-
/**
 * @brief The class that implements an event based on the evaluation of the
 *  domain appartenance of a stochastic process using a boolean function
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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

#ifndef OPENTURNS_PROCESSEVENT_HXX
#define OPENTURNS_PROCESSEVENT_HXX

#include "openturns/RandomVectorImplementation.hxx"
#include "openturns/Domain.hxx"
#include "openturns/Process.hxx"
#include "openturns/Field.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class ProcessEvent
 *
 *
 */
class OT_API ProcessEvent
  : public RandomVectorImplementation
{
  CLASSNAME

public:


  /** Default constructor */
  ProcessEvent();

  /** Constructor from RandomVector */
  ProcessEvent(const Process & process,
               const Domain & domain);

  /** Virtual constructor */
  virtual ProcessEvent * clone() const;

  /** String converter */
  String __repr__() const;

  /** Dimension accessor */
  virtual UnsignedInteger getDimension() const;

  /** Domain accessor */
  Domain getDomain() const;

  /** Process accessor */
  Process getProcess() const;

  /** Realization accessor */
  Point getRealization() const;

  /** Whether it is an event */
  virtual Bool isEvent() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


private:

  /** The process of the eventProcess */
  Process process_;

  /** The domain of the evenProcess */
  Domain domain_;

  /** Private method - check that values of a time serie are/are not in the domain */
  Bool checkTimeSerieValuesInDomain(const Field & ts) const;
}; /* class ProcessEvent */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PROCESSEVENT_HXX */
