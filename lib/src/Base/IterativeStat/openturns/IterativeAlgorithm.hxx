
#ifndef OPENTURNS_ITERATIVEALGORITHM_HXX
#define OPENTURNS_ITERATIVEALGORITHM_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/IterativeAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS


class OT_API IterativeAlgorithm
  : public TypedInterfaceObject<IterativeAlgorithmImplementation>
{
  CLASSNAME
  
public:

//   typedef std::unordered_map<std::string, void*> registry_map;

//   typedef Pointer<IterativeAlgorithmImplementation>   IterativeAlgorithmImplementation_p;

  /** Return a pointer to the underlying implementation object viewed as a PersistentObject */
  Pointer<IterativeAlgorithmImplementation> getImplementationAsIterativeAlgorithm() const
  {
    return p_implementation_;
  }
    
  /**
   * Default constructor
   *
   * The constructor sets a new Id to the object,
   * so it can be later referenced by a Study object.
   * It is also declared visible if member of a study.
   *
   * The object has the default name but it does not
   * use storage for it.
   */
//   IterativeAlgorithm();
  
  /**
   * Increment methods
   *
   * @warning These methods MUST be overloaded in derived classes.
   */
  
  virtual void increment(const Scalar newData)
  {
    return getImplementation()->increment(newData);
  }
  
//   virtual void increment(const PersistentCollection<Scalar> & newData) = 0;
  
  virtual void increment(const Point & newData)
  {
    return getImplementation()->increment(newData);
  }
  
  virtual void increment(const Sample & newData)
  {
    return getImplementation()->increment(newData);
  }
  
  virtual void finalize()
  {
    return getImplementation()->finalize();
  }

  /* String converter */
  virtual
  String __repr__() const
  {
    return getImplementation()->__repr__();
  }

  /* String converter */
  virtual
  String __str__(const String & offset = "") const
  {
    return getImplementation()->__str__(offset);
  }
    
}; /* class IterativeAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ITERATIVEALGORITHM_HXX */
