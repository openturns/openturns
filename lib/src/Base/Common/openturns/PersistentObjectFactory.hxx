//                                               -*- C++ -*-
/**
 *  @brief Class PersistentObjectFactory reloads a PersistentObject from a storage manager
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
#ifndef OPENTURNS_PERSISTENTOBJECTFACTORY_HXX
#define OPENTURNS_PERSISTENTOBJECTFACTORY_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Catalog.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class PersistentObjectFactory
 *
 * @brief Reloads a PersistentObject from a storage manager
 * @see PersistentObject
 * @see StorageManager
 */

class OT_API PersistentObjectFactory
{
public:


  /** @copydoc PersistentObject::clone() const */
  virtual PersistentObjectFactory * clone() const = 0;

  /** Destructor */
  virtual ~PersistentObjectFactory() {};

  /** Method build() creates a new PersistentObject from the storage manager
   * @param mgr The storage manager that points to the object to be reloaded
   * @return A pointer to a newly allocated object
   * @internal
   */
  virtual PersistentObject * build(StorageManager & mgr) const = 0;

  /** Method assign() fills a PersistentObject with another one (must have same type)
   *
   * This method performs an assignment between the two objects after having casted them to the same type
   * @param po The persistent object to be refilled (may be empty, ie default constructed)
   * @param other The persistent object of the same type as \em po that should be copied to \em po
   * @internal
   */
  virtual void assign(PersistentObject & po, const PersistentObject & other) const = 0;

  /** This method register the factory into the Catalog
   * @see Catalog
   * @internal
   */
  void registerMe(const String & className) const
  {
    Catalog::Add(className, this);
  }

  /** Accessor to PersistentObject's shadowed id */
  void setShadowedId(PersistentObject & obj, Id id) const
  {
    obj.setShadowedId(id);
  }

  Id getShadowedId(PersistentObject & obj) const
  {
    return obj.getShadowedId();
  }

protected:

private:

}; /* class PersistentObjectFactory */


/**
 * @class Factory
 * @brief Ease the creation of factories for any PersistentObject based class
 * @tparam PERSISTENT The class which the %Factory is bounded to
 */
template <class PERSISTENT>
class Factory
  : PersistentObjectFactory
{
public:
  /** Constructor
   *
   * @internal See the CLASSNAME_INIT macro.
   */
  Factory()
  {
    registerMe(PERSISTENT::GetClassName());
  }

  /** Virtual constructor */
  virtual Factory * clone() const
  {
    return new Factory(*this);
  }

  /** Method build() creates a new PersistentObject from the storage manager */
  virtual PersistentObject * build(StorageManager & mgr) const
  {
    Advocate adv ( mgr.readObject() );

    PERSISTENT * p_rebuildObject = new PERSISTENT();
    assert(p_rebuildObject && "PersistentObject not allocated");
    //try {

    p_rebuildObject->load(adv);

    //} catch (Exception & ex) {
    // Nothing to do
    // The rebuild object remains the default one
    //}
    return p_rebuildObject;
  }

  /** Method assign() fills a PersistentObject with another one (must have same type) */
  virtual void assign(PersistentObject & po, const PersistentObject & other) const
  {
    PERSISTENT & ref_po          = static_cast<PERSISTENT &>(po);
    const PERSISTENT & ref_other = static_cast<const PERSISTENT &>(other);
    ref_po = ref_other;
  }

}; /* end class Factory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PERSISTENTOBJECTFACTORY_HXX */
