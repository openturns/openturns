//                                               -*- C++ -*-
/**
 * @brief The class that implements an event domain based on a composite random vector
 *        built upon a boolean function which is described by a domain appartenance.
 *        The antecedent of the event must be a "classical" composite random
 *        vector in the context of OpenTURNS.
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_DOMAINEVENT_HXX
#define OPENTURNS_DOMAINEVENT_HXX

#include "openturns/CompositeRandomVector.hxx"
#include "openturns/Domain.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class DomainEvent
 *
 *
 */
class OT_API DomainEvent
  : public CompositeRandomVector
{
  CLASSNAME

public:

  /** Default constructor */
  DomainEvent();

  /** Constructor from RandomVector */
  DomainEvent(const RandomVector & antecedent,
              const Domain & domain);

  /** Virtual constructor */
  DomainEvent * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Dimension accessor */
  UnsignedInteger getDimension() const override;

  /** Domain accessor */
  Domain getDomain() const override;

  /* Here is the interface that all derived class may implement */

  /** Realization accessor */
  Point getRealization() const override;

  /** Numerical sample accessor */
  Sample getSample(const UnsignedInteger size) const override;

  /** Whether it is an event */
  Bool isEvent() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** The domain of the eventDomainImplementation */
  Domain domain_;


}; /* class DomainEvent */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DOMAINEVENT_HXX */
