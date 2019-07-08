
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
    labelMap_(),
    p_storageManager_(new StorageManager)
{
  p_storageManager_->setStudy(this);
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

void MelissaStudy::increment(const Scalar newData)
{
  for(Map::iterator it = map_.begin();
      it != map_.end(); ++it)
  {
//     ((*it).second)->increment(newData);
  }
}

void MelissaStudy::increment(const Point & newData)
{
  for(LabelMap::iterator it = labelMap_.begin();
      it != labelMap_.end(); ++it)
  {
//     (*((*it).second)).increment(newData);
  }
  for (auto& x: map_) {
    std::cout << x.first << ": " << x.second << '\n';
//     (x.second)->increment(newData);
  }
}

void MelissaStudy::increment(const Sample & newData)
{
  for(Map::iterator it = map_.begin();
      it != map_.end(); ++it)
  {
//     ((*it).second)->increment(newData);
  }
}

void MelissaStudy::finalize()
{
  IterativeAlgorithm* iteratifAlgorithmPtr;
  for(Map::iterator it = map_.begin();
      it != map_.end(); ++it)
  {
//     it->second->finalize();
  }
}


END_NAMESPACE_OPENTURNS
