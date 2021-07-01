//                                               -*- C++ -*-
/**
 *  @brief Study keeps all PersistentObjects in a file
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
#include <algorithm>
#include "openturns/InterfaceObject.hxx"
#include "openturns/Study.hxx"
#include "openturns/StorageManager.hxx"
#if defined OPENTURNS_HAVE_LIBXML2
#include "openturns/XMLStorageManager.hxx"
#endif
#include "openturns/Exception.hxx"
#include "openturns/Catalog.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(Study)

/*
 * Default constructor
 */
Study::Study()
  : map_(),
    labelMap_(),
    p_storageManager_(new StorageManager)
{
  p_storageManager_->setStudy(this);
}

/*
 * Parameter constructor
 */
Study::Study(const FileName & fileName,
             const UnsignedInteger compressionLevel)
  : map_(),
    labelMap_(),
#if defined OPENTURNS_HAVE_LIBXML2
    p_storageManager_(new XMLStorageManager(fileName, compressionLevel))
#else
    p_storageManager_(new StorageManager)
#endif /* OPENTURNS_HAVE_LIBXML2 */
{
#if defined OPENTURNS_HAVE_LIBXML2
  p_storageManager_->setStudy(this);
#else
  (void)fileName;
  (void)compressionLevel;
  throw NotYetImplementedException(HERE) << "Error: no XML support for Study";
#endif /* OPENTURNS_HAVE_LIBXML2 */
}

/* String converter */
String Study::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName();

  const char * separator = "  ";
  for(Map::const_iterator it = map_.begin();
      it != map_.end();
      ++it, separator = "\n  ")
  {
#if 0
    oss << separator << (*it).first << " => " << ((*it).second)->getClassName()
        << " name='" << ((*it).second)->getName()
        << "' id=" << ((*it).second)->getId();
#endif
#if 0
    if (((*it).second)->getVisibility())
      oss << separator << (*it).first << " => id=" << ((*it).second)->getId()
          << " " << ((*it).second)->__repr__();
#endif
#if 1
    oss << separator << (*it).first << " => " << (((*it).second)->getVisibility() ? "*" : " ") << " id=" << ((*it).second)->getId()
        << " " << ((*it).second)->__repr__();
#endif
  }

  oss << "\n";
  separator = "  ";
  for(LabelMap::const_iterator it = labelMap_.begin();
      it != labelMap_.end();
      ++it, separator = "\n  ")
  {
    oss << separator << "'" << (*it).first << "' is aliased to " << (*it).second;
  }
  return oss;
}

/* String converter */
String Study::__str__(const String & ) const
{
  OSS oss(false);
  const char * separator = "  ";
  for(Map::const_iterator it = map_.begin(); it != map_.end(); ++it, separator = "\n  ")
  {
    oss << separator << (*it).first << " => " << ((*it).second)->getClassName() << "\n"
        << ((*it).second)->__str__( String( 7 + int( ((*it).first != 0 ? log10(static_cast<double>((*it).first)) : 1) ), ' ') );
  }
  oss << "\n";
  separator = "  ";
  for(LabelMap::const_iterator it = labelMap_.begin();
      it != labelMap_.end();
      ++it, separator = "\n  ")
  {
    oss << separator << "'" << (*it).first << "' is aliased to " << (*it).second;
  }
  return oss;
}

/* This method saves the study through the storage manager */
void Study::save()
{
  assert(p_storageManager_);
  StorageManager & mgr = *p_storageManager_;

  mgr.initialize( SaveAction() );

  for(LabelMap::const_iterator it = labelMap_.begin();
      it != labelMap_.end(); ++it)
  {
    mgr.save(*map_[ it->second ], it->first, true);
  }

  for(Map::const_iterator it = map_.begin();
      it != map_.end(); ++it)
  {
    mgr.save(*(it->second), "", true);
  }

  mgr.write();
  mgr.finalize( SaveAction() );
}

/* This method reloads the study from the storage manager */
void Study::load()
{
  assert(p_storageManager_);
  StorageManager & mgr = *p_storageManager_;

  mgr.initialize( LoadAction() );
  mgr.read();
  mgr.load(*this);
  mgr.finalize( LoadAction() );

  cleanUnvisibleObject();
  translateId();
}

/* This method purges the study from the reloaded objects that are tagged unvisible */
void Study::cleanUnvisibleObject()
{
  Map newMap;
  for(Map::const_iterator it = map_.begin(); it != map_.end(); ++it)
  {
    if (it->second->getVisibility())
      newMap[ it->second->getShadowedId() ] = it->second;
  }
  map_ = newMap;
}

/* This method performs the translation of ids after a study load */
void Study::translateId()
{
  std::map<Id, Id> translationTable;

  Map newMap;
  for(Map::const_iterator it = map_.begin(); it != map_.end(); ++it)
  {
    translationTable[ it->first ] = it->second->getId();
    newMap[ it->second->getId() ] = it->second;
  }

  LabelMap newLabelMap;
  for(LabelMap::const_iterator it = labelMap_.begin(); it != labelMap_.end(); ++it)
    newLabelMap[ it->first ] = translationTable[ it->second ];

  map_      = newMap;
  labelMap_ = newLabelMap;
}






/* Query if object is stored in study */
Bool Study::hasObject(Id id) const
{
  Map::const_iterator it = map_.find( id );
  return ( it != map_.end() );
}

/* Get object whose id is given */
Study::MapElement Study::getObject(Id id) const
{
  MapElement element;
  Map::const_iterator it = map_.find( id );
  if (it != map_.end())
  {
    element = (*it).second;
  }
  return element;
}




/* Query if object is stored in study */
Bool Study::hasObject(const String & label) const
{
  if (label.empty()) return false;
  LabelMap::const_iterator it = labelMap_.find( label );
  return (it == labelMap_.end()) ? false : hasObject( it->second );
}

/* Get object whose id is given */
Study::MapElement Study::getObject(const String & label) const
{
  LabelMap::const_iterator it_label = labelMap_.find( label );
  if (it_label == labelMap_.end()) throw InvalidArgumentException(HERE) << "No object with label '" << label << "' in study";
  return getObject( it_label->second );
}





/* Local class for the following method. Should have been declared inside the method but find_if crashes */
struct element_whose_class_and_name_are
{
  const String & className_;
  const String & name_;
  element_whose_class_and_name_are(const String & className, const String & name) : className_(className), name_(name) {}
  Bool operator()(const Study::Map::value_type & element) const
  {
    return (element.second->getClassName() == className_) && (element.second->getName() == name_);
  }
};

/* Get object whose class and name are given */
Study::MapElement Study::getObjectByName(const String & className, const String & name) const
{
  MapElement element;
  Map::const_iterator it = std::find_if(map_.begin(), map_.end(), element_whose_class_and_name_are(className, name));
  if (it != map_.end())
  {
    element = (*it).second;
  }
  return element;
}


/* Fill an object with one got from study */
void Study::fillObjectByName(PersistentObject & po, const String & name) const
{
  MapElement element = getObjectByName(po.getClassName(), name);
  if (! element) throw InvalidArgumentException(HERE) << "No object of name " << name << " in study";
  Catalog::Get(po.getClassName()).assign(po, *element);
}

void Study::fillObjectByName(InterfaceObject  & io, const String & name) const
{
  MapElement element = getObjectByName(io.getImplementationAsPersistentObject()->getClassName(), name);
  if (! element) throw InvalidArgumentException(HERE) << "No object of name " << name << " in study";
  io.setImplementationAsPersistentObject(element);
}

void Study::fillObject(Id id, PersistentObject & po) const
{
  MapElement element = getObject(id);
  if (! element) throw InvalidArgumentException(HERE) << "No object of id " << id << " in study";
  Catalog::Get(po.getClassName()).assign(po, *element);
}

void Study::fillObject(Id id, InterfaceObject & io) const
{
  MapElement element = getObject(id);
  if (! element) throw InvalidArgumentException(HERE) << "No object of id " << id << " in study";
  io.setImplementationAsPersistentObject(element);
}

void Study::fillObject(const String & label, PersistentObject & po) const
{
  MapElement element = getObject(label);
  if (! element) throw InvalidArgumentException(HERE) << "No object labelled '" << label << "' in study";
  Catalog::Get(po.getClassName()).assign(po, *element);
}

void Study::fillObject(const String & label, InterfaceObject & io) const
{
  MapElement element = getObject(label);
  if (! element) throw InvalidArgumentException(HERE) << "No object labelled '" << label << "' in study";
  io.setImplementationAsPersistentObject(element);
}


/* Storage manager accessor */
void Study::setStorageManager(const StorageManager & smgr)
{
  p_storageManager_.reset(smgr.clone());
  p_storageManager_->setStudy(this);
}

/* Storage manager accessor */
Study::StorageManagerImplementation Study::getStorageManager() const
{
  return p_storageManager_;
}

/* Define a label for an object */
void Study::defineLabel(Id id, const String & label)
{
  if (! label.empty()) labelMap_[ label ] = id;
}


/* Define the visibility of an object */
void Study::defineVisibility(Id id, Bool visible)
{
  MapElement elt = getObject( id );
  elt->setVisibility( visible );
}

/* Add a PersistentObject to the study */
void Study::add(const InterfaceObject & io)
{
  map_[ io.getId() ] = io.getImplementationAsPersistentObject();
}

/* Add a PersistentObject to the study */
void Study::add(const String & label, const InterfaceObject & io, Bool force)
{
  if ( hasObject( label ) )
  {
    if (force) remove( label );
    else throw InvalidArgumentException(HERE) << "Label '" << label << "' already defined in study. Use 'force = true' to remove previously saved element before saving this one";
  }
  map_[ io.getId() ] = io.getImplementationAsPersistentObject();
  defineLabel( io.getId(), label );
}

/* Remove a PersistentObject from the study */
void Study::remove(const InterfaceObject & io)
{
  Map::iterator it = map_.find( io.getId() );
  map_.erase(it);
}

/* Remove a PersistentObject from the study */
void Study::remove(const String & label)
{
  LabelMap::iterator it_label = labelMap_.find( label );
  if (it_label == labelMap_.end()) throw InvalidArgumentException(HERE) << "No object with label '" << label << "' in study";
  Map::iterator it_obj = map_.find( it_label->second );
  map_.erase(it_obj);
  labelMap_.erase( it_label );
}

/* Add a PersistentObject to the study */
void Study::add(const PersistentObject & po)
{
  add(po.clone());
}

/* Add a PersistentObject to the study */
void Study::add(const String & label, const PersistentObject & po, Bool force)
{
  add(label, po.clone(), force);
}

/* Add a PersistentObject to the study (any map) */
void Study::add(const PersistentObject * po)
{
  if (! po) throw InvalidArgumentException(HERE) << "Null pointer passed to method";
  map_[ po->getShadowedId() ] = const_cast<PersistentObject *>(po);
}

/* Add a PersistentObject to the study (any map) */
void Study::add(const String & label, const PersistentObject * po, Bool force)
{
  if (! po) throw InvalidArgumentException(HERE) << "Null pointer passed to method";
  if ( hasObject( label ) )
  {
    if (force) remove( label );
    else throw InvalidArgumentException(HERE) << "Label '" << label << "' already defined in study. Use 'force = true' to remove previously saved element before saving this one";
  }
  map_[ po->getShadowedId() ] = const_cast<PersistentObject *>(po);
  defineLabel( po->getShadowedId(), label );
}

/* Print all the labels in the study */
String Study::printLabels() const
{
  OSS oss;
  String separator("");
  for(LabelMap::const_iterator it = labelMap_.begin(); it != labelMap_.end(); ++it, separator = ";") oss << separator << (*it).first;
  return oss;
}

END_NAMESPACE_OPENTURNS
