 

#ifndef OPENTURNS_MELISSASTUDY_HXX
#define OPENTURNS_MELISSASTUDY_HXX

#include "openturns/Object.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/IterativeAlgorithm.hxx"
#include "openturns/Study.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MelissaStudy
 * @brief Store and manage IterativeAlgoritm objects
 *
 * @see PersistentObject
 * @see IterativeAlgorithm
 */

class OT_API MelissaStudy
  : public Object
{
  CLASSNAME
  
public:
    
  typedef Pointer<IterativeAlgorithmImplementation> MapElement;
  typedef std::map<Id, MapElement>  Map;
  typedef std::map<String, Id>      LabelMap;

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
  
  /** Remove a PersistentObject from the study */
  void remove(const InterfaceObject & io);

  /** Remove a PersistentObject from the study */
  void remove(const String & label);

  /** Query if object is stored in study
   * @param id The Id of the object
   * @return True if the object is stored in the study
   */
  Bool hasObject(Id id) const;

  /** Query if object is stored in study
   * @param label The label of the object
   * @return True if the object is stored in the study
   */
  Bool hasObject(const String & label) const;


  /** Get object whose id is given
   * @param id The Id of the object
   * @return The object saved in the study
   */
  MapElement getObject(Id id) const;

  /** Get object whose id is given
   * @param label The label of the object
   * @return The object saved in the study
   */
  MapElement getObject(const String & label) const;


  /** Fill an object with one got from study
   * @param po An object to be refilled (may be empty, ie default constructed)
   * @param name The name of the object stored in the study
   * @throw InvalidArgumentException
   */
  void fillObjectByName(IterativeAlgorithmImplementation & po, const String & name) const;

  /** Fill an object with one got from study
   * @param io An object to be refilled (may be empty, ie default constructed)
   * @param name The name of the object stored in the study
   * @throw InvalidArgumentException
   */
  void fillObjectByName(IterativeAlgorithm  & io, const String & name) const;

  /** Fill an object with one got from study
   * @param id The Id of the object stored in the study
   * @param po An object to be refilled (may be empty, ie default constructed)
   * @throw InvalidArgumentException
   */
  void fillObject(Id id, IterativeAlgorithmImplementation & po) const;

  /** Fill an object with one got from study
   * @param id The Id of the object stored in the study
   * @param io An object to be refilled (may be empty, ie default constructed)
   * @throw InvalidArgumentException
   */
  void fillObject(Id id, IterativeAlgorithm  & io) const;

  /** Fill an object with one got from study
   * @param label The label of the object stored in the study
   * @param po An object to be refilled (may be empty, ie default constructed)
   * @throw InvalidArgumentException
   */
  void fillObject(const String & name, IterativeAlgorithmImplementation & po) const;

  /** Fill an object with one got from study
   * @param label The label of the object stored in the study
   * @param io An object to be refilled (may be empty, ie default constructed)
   * @throw InvalidArgumentException
   */
  void fillObject(const String & name, IterativeAlgorithm  & io) const;

  /** Print all the labels in the study */
  String printLabels() const;
  
  void finalize();
  
protected:

  /** Define a label for an object */
  void defineLabel(Id id, const String & label);
  
  /** Get object whose class and name are given */
  MapElement getObjectByName(const String & className, const String & name) const;


private:

  /** The map storing the saved objects */
  Map map_;

  /** The map storing the saved objects */
  LabelMap labelMap_;
    
}; /* class MelissaStudy */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_STUDY_HXX */
