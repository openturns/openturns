
#include "openturns/MelissaStudy.hxx"
#include "openturns/IterativeAlgorithm.hxx"
#include "openturns/Log.hxx"
#include "openturns/Catalog.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MelissaStudy)

MelissaStudy::MelissaStudy()
  : map_(),
    labelMap_()
{
}


/* Define a label for an object */
void MelissaStudy::defineLabel(Id id, const String & label)
{
  if (! label.empty()) labelMap_[ label ] = id;
}


/* Add a PersistentObject to the study */
void MelissaStudy::add(const IterativeAlgorithm & io)
{
  map_[ io.getId() ] = io.getImplementationAsIterativeAlgorithm();
}

/* Add a PersistentObject to the study */
void MelissaStudy::add(const String & label, const IterativeAlgorithm & io, Bool force)
{
  if ( hasObject( label ) )
  {
    if (force) remove( label );
    else throw InvalidArgumentException(HERE) << "Label '" << label << "' already defined in study. Use 'force = true' to remove previously saved element before saving this one";
  }
  map_[ io.getId() ] = io.getImplementationAsIterativeAlgorithm();
  defineLabel( io.getId(), label );
}

/* Add a IterativeAlgorithm to the study */
void MelissaStudy::add(const IterativeAlgorithmImplementation & po)
{
  add(po.clone());
}

/* Add a IterativeAlgorithm to the study */
void MelissaStudy::add(const String & label, const IterativeAlgorithmImplementation & po, Bool force)
{
  add(label, po.clone(), force);
}

/* Add a IterativeAlgorithm to the study (any map) */
void MelissaStudy::add(const IterativeAlgorithmImplementation * po)
{
  if (! po) throw InvalidArgumentException(HERE) << "Null pointer passed to method";
  map_[ po->getShadowedId() ] = const_cast<IterativeAlgorithmImplementation *>(po);
}

/* Add a IterativeAlgorithm to the study (any map) */
void MelissaStudy::add(const String & label, const IterativeAlgorithmImplementation * po, Bool force)
{
  if (! po) throw InvalidArgumentException(HERE) << "Null pointer passed to method";
  if ( hasObject( label ) )
  {
    if (force) remove( label );
    else throw InvalidArgumentException(HERE) << "Label '" << label << "' already defined in study. Use 'force = true' to remove previously saved element before saving this one";
  }
  map_[ po->getShadowedId() ] = const_cast<IterativeAlgorithmImplementation *>(po);
  defineLabel( po->getShadowedId(), label );
}

/* Remove a PersistentObject from the study */
void MelissaStudy::remove(const InterfaceObject & io)
{
  Map::iterator it = map_.find( io.getId() );
  map_.erase(it);
}

/* Remove a PersistentObject from the study */
void MelissaStudy::remove(const String & label)
{
  LabelMap::iterator it_label = labelMap_.find( label );
  if (it_label == labelMap_.end()) throw InvalidArgumentException(HERE) << "No object with label '" << label << "' in study";
  Map::iterator it_obj = map_.find( it_label->second );
  map_.erase(it_obj);
  labelMap_.erase( it_label );
}




/* Query if object is stored in study */
Bool MelissaStudy::hasObject(Id id) const
{
  Map::const_iterator it = map_.find( id );
  return ( it != map_.end() );
}

/* Get object whose id is given */
MelissaStudy::MapElement MelissaStudy::getObject(Id id) const
{
  MapElement element;
  Map::const_iterator it = map_.find( id );
  if (it != map_.end())
  {
    element = (*it).second;
  }
  return element;
}

/* Get object whose id is given */
MelissaStudy::MapElement MelissaStudy::getObject(const String & label) const
{
  LabelMap::const_iterator it_label = labelMap_.find( label );
  if (it_label == labelMap_.end()) throw InvalidArgumentException(HERE) << "No object with label '" << label << "' in melissa study";
  return getObject( it_label->second );
}




/* Local class for the following method. Should have been declared inside the method but find_if crashes */
struct element_whose_class_and_name_are
{
  const String & className_;
  const String & name_;
  element_whose_class_and_name_are(const String & className, const String & name) : className_(className), name_(name) {}
  Bool operator()(const MelissaStudy::Map::value_type & element) const
  {
    std::cout << " classe name : " << element.second->getClassName() << " needed " << className_;
    return (element.second->getClassName() == className_) && (element.second->getName() == name_);
  }
};


/* Query if object is stored in study */
Bool MelissaStudy::hasObject(const String & label) const
{
  if (label.empty()) return false;
  LabelMap::const_iterator it = labelMap_.find( label );
  return (it == labelMap_.end()) ? false : hasObject( it->second );
}

/* Get object whose class and name are given */
MelissaStudy::MapElement MelissaStudy::getObjectByName(const String & className, const String & name) const
{
  MapElement element;
  Map::const_iterator it = std::find_if(map_.begin(), map_.end(), element_whose_class_and_name_are(className, name));
  std::cout << " object name : " << (*it).second << " needed " << name;
  if (it != map_.end())
  {
    element = (*it).second;
  }
  return element;
}


/* Fill an object with one got from study */
void MelissaStudy::fillObjectByName(IterativeAlgorithmImplementation & po, const String & name) const
{
  MapElement element = getObjectByName(po.getClassName(), name);
  if (! element) throw InvalidArgumentException(HERE) << "No object of name " << name << " in melissa study";
  Catalog::Get(po.getClassName()).assign(po, *element);
}

void MelissaStudy::fillObjectByName(IterativeAlgorithm  & io, const String & name) const
{
  MapElement element = getObjectByName(io.getImplementationAsIterativeAlgorithm()->getClassName(), name);
  if (! element) throw InvalidArgumentException(HERE) << "No object of name " << name << " in melissa study";
  io.setImplementationAsIterativeAlgorithm(element);
}

void MelissaStudy::fillObject(Id id, IterativeAlgorithmImplementation & po) const
{
  MapElement element = getObject(id);
  if (! element) throw InvalidArgumentException(HERE) << "No object of id " << id << " in melissa study";
  Catalog::Get(po.getClassName()).assign(po, *element);
}

void MelissaStudy::fillObject(Id id, IterativeAlgorithm & io) const
{
  MapElement element = getObject(id);
  if (! element) throw InvalidArgumentException(HERE) << "No object of id " << id << " in melissa study";
  io.setImplementationAsIterativeAlgorithm(element);
}

void MelissaStudy::fillObject(const String & label, IterativeAlgorithmImplementation & po) const
{
  MapElement element = getObject(label);
  if (! element) throw InvalidArgumentException(HERE) << "No object labelled '" << label << "' in melissa study";
  Catalog::Get(po.getClassName()).assign(po, *element);
}

void MelissaStudy::fillObject(const String & label, IterativeAlgorithm & io) const
{
  MapElement element = getObject(label);
  if (! element) throw InvalidArgumentException(HERE) << "No object labelled '" << label << "' in melissa study";
  io.setImplementationAsIterativeAlgorithm(element);
}

/* Print all the labels in the study */
String MelissaStudy::printLabels() const
{
  OSS oss;
  String separator("");
  for(LabelMap::const_iterator it = labelMap_.begin(); it != labelMap_.end(); ++it, separator = ";") oss << separator << (*it).first;
  return oss;
}

void MelissaStudy::increment(const Scalar newData)
{
  for(Map::iterator it = map_.begin();
      it != map_.end(); ++it)
  {
    ((*it).second)->increment(newData);
  }
}

void MelissaStudy::increment(const Point & newData)
{
  for(Map::iterator it = map_.begin();
      it != map_.end(); ++it)
  {
    (*((*it).second)).increment(newData);
  }
  for(LabelMap::iterator x = labelMap_.begin();
      x != labelMap_.end(); ++x)
  {
    std::cout << (*x).first << ": " << (*x).second << '\n';
//     (x.second)->increment(newData);
  }
}

void MelissaStudy::increment(const Sample & newData)
{
  for(Map::iterator it = map_.begin();
      it != map_.end(); ++it)
  {
    ((*it).second)->increment(newData);
  }
}

void MelissaStudy::finalize()
{
  IterativeAlgorithm* iteratifAlgorithmPtr;
  for(Map::iterator it = map_.begin();
      it != map_.end(); ++it)
  {
    it->second->finalize();
  }
}


END_NAMESPACE_OPENTURNS
