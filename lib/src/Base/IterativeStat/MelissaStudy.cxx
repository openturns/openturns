
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

/* Add a IterativeAlgorithm to the study */
void MelissaStudy::add(const IterativeAlgorithm & po)
{
  add(po.clone());
}

/* Add a IterativeAlgorithm to the study */
void MelissaStudy::add(const String & label, const IterativeAlgorithm & po, Bool force)
{
  add(label, po.clone(), force);
}

/* Add a IterativeAlgorithm to the study (any map) */
void MelissaStudy::add(const IterativeAlgorithm * po)
{
  if (! po) throw InvalidArgumentException(HERE) << "Null pointer passed to method";
  map_[ po->getShadowedId() ] = const_cast<IterativeAlgorithm *>(po);
}

/* Add a IterativeAlgorithm to the study (any map) */
void MelissaStudy::add(const String & label, const IterativeAlgorithm * po, Bool force)
{
  if (! po) throw InvalidArgumentException(HERE) << "Null pointer passed to method";
  if ( hasObject( label ) )
  {
    if (force) remove( label );
    else throw InvalidArgumentException(HERE) << "Label '" << label << "' already defined in study. Use 'force = true' to remove previously saved element before saving this one";
  }
  map_[ po->getShadowedId() ] = const_cast<IterativeAlgorithm *>(po);
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
  LabelMap::iterator it = labelMap_.begin();
//   (*((*it).second)).increment(newData);
  std::cout << (*it).first << '\n';
  it = labelMap_.end();
  std::cout << (*it).first << '\n';
  for(LabelMap::iterator it = labelMap_.begin();
      it != labelMap_.end(); ++it)
  {
    throw InvalidArgumentException(HERE) << " increment in study";
    std::cout << (*it).first;
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
