 

#ifndef OPENTURNS_MELISSASTUDY_HXX
#define OPENTURNS_MELISSASTUDY_HXX

#include "openturns/Object.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/IterativeAlgorithm.hxx"
#include "openturns/Study.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/* Forward declarations */
// class IterativeAlgorithm;
class StorageManager;

/**
 * @class MelissaStudy
 * @brief Store and manage IterativeAlgoritm objects
 *
 * @see PersistentObject
 * @see StorageManager
 * @see IterativeAlgorithm
 */

class OT_API MelissaStudy
  : public Study
{
  CLASSNAME

  friend class StorageManager;
  
public:
    
  typedef Pointer<IterativeAlgorithmImplementation> MapElement;
  typedef std::map<Id, MapElement>  Map;

  /** Default constructor */
  MelissaStudy();

  void increment(const Scalar newData);

  void increment(const Point & newData);

  void increment(const Sample & newData);

  /** Add a PersistentObject to the study */
  void add(const IterativeAlgorithm & io);

  /** Add a PersistentObject to the study */
  void add(const String & label, const IterativeAlgorithm & io, Bool force = false);

  /** Add a IterativeAlgorithm to the study */
  /** We get the label from the name of the IterativeAlgorithm */
  void add(const IterativeAlgorithmImplementation & po);

  /** Add a IterativeAlgorithm to the study */
  void add(const String & label, const IterativeAlgorithmImplementation & po, Bool force = false);
// #ifndef SWIG
  /** Add a IterativeAlgorithm to the study
   * @throw InvalidArgumentException
   */
  void add(const IterativeAlgorithmImplementation * po);

  /** Add a IterativeAlgorithm to the study
   * @throw InvalidArgumentException
   */
  void add(const String & label, const IterativeAlgorithmImplementation * po, Bool force = false);
// #endif
  
  void finalize();


private:

  /** The map storing the saved objects */
  Map map_;

  /** The map storing the saved objects */
  LabelMap labelMap_;

  /** The StorageManager that drives the load/store operations */
  StorageManagerImplementation p_storageManager_;
    
}; /* class MelissaStudy */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_STUDY_HXX */
