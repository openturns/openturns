//                                               -*- C++ -*-
/**
 * @brief The class that implements an event based on the evaluation of the
 *  domain appartenance of a stochastic process using a boolean function
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

#ifndef OPENTURNS_EVENTPROCESS_HXX
#define OPENTURNS_EVENTPROCESS_HXX

#include "RandomVectorImplementation.hxx"
#include "Domain.hxx"
#include "Process.hxx"
#include "Field.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class EventProcess
 *
 *
 */
class OT_API EventProcess
  : public RandomVectorImplementation
{
  CLASSNAME;

public:


  /** Default constructor */

  EventProcess();

  /** Constructor from RandomVector */
  EventProcess(const Process & process,
               const Domain & domain);

  /** Virtual constructor */
  virtual EventProcess * clone() const;

  /** String converter */
  String __repr__() const;

  /** Dimension accessor */
  virtual UnsignedInteger getDimension() const;

  /** Domain accessor */
  Domain getDomain() const;

  /** Process accessor */
  Process getProcess() const;

  /** Realization accessor */
  NumericalPoint getRealization() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


private:

  friend class Factory<EventProcess>;

  /** The process of the eventProcess */
  Process process_;

  /** The domain of the evenProcess */
  Domain domain_;

  /** Private method - check that values of a time serie are/are not in the domain */
  Bool checkTimeSerieValuesInDomain(const Field & ts) const;
}; /* class EventProcess */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EVENTPROCESS_HXX */
