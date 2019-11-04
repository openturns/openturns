//                                               -*- C++ -*-
/**
 * @brief The class that implements an event random vector (not necessary composite)
 * built upon a process and a domain.
 * The class check that a realization of a stochastic process is in a domain \mathcal{D}
 * using contains method
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/ProcessEvent.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ProcessEvent)

static const Factory<ProcessEvent> Factory_ProcessEvent;

/* Default constructor */
ProcessEvent::ProcessEvent()
  : RandomVectorImplementation()
{
  // Nothing to do
}

/* Constructor from RandomVector */
ProcessEvent::ProcessEvent(const Process & process,
                           const Domain & domain)
  : RandomVectorImplementation()
  , process_(process)
  , domain_(domain)
{
  // Set the description
  setDescription(Description(1, OSS() << process.getName() << " in " << domain.getName()));
}

ProcessEvent * ProcessEvent::clone() const
{
  return new ProcessEvent(*this);
}

/* String converter */
String ProcessEvent::__repr__() const
{
  OSS oss;
  oss << "class=" << ProcessEvent::GetClassName()
      << " domain=" << domain_
      << " process=" << process_ ;
  return oss;
}

/* Dimension accessor */
UnsignedInteger ProcessEvent::getDimension() const
{
  return 1;
}

/* Domain accessor */
Domain ProcessEvent::getDomain() const
{
  return domain_;
}

/* Process accessor */
Process ProcessEvent::getProcess() const
{
  return process_;
}

/* check that a values of a field are/are not in the domain */
Bool ProcessEvent::checkTimeSerieValuesInDomain(const Field & fld) const
{
  for (UnsignedInteger i = 0; i < fld.getSize(); ++i) if (domain_.contains(fld.getValueAtIndex(i))) return true;
  return false;
}

/* Realization accessor */
Point ProcessEvent::getRealization() const
{
  return Point(1, checkTimeSerieValuesInDomain(process_.getRealization()));
}

Bool ProcessEvent::isEvent() const
{
  return true;
}

/* Method save() stores the object through the StorageManager */
void ProcessEvent::save(Advocate & adv) const
{
  RandomVectorImplementation::save(adv);
  adv.saveAttribute( "domain_", domain_ );
  adv.saveAttribute( "process_", process_ );
}

/* Method load() reloads the object from the StorageManager */
void ProcessEvent::load(Advocate & adv)
{
  RandomVectorImplementation::load(adv);
  adv.loadAttribute( "domain_", domain_ );
  adv.loadAttribute( "process_", process_ );
}

END_NAMESPACE_OPENTURNS
