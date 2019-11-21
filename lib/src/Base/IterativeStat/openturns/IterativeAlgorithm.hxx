
#ifndef OPENTURNS_ITERATIVEALGORITHM_HXX
#define OPENTURNS_ITERATIVEALGORITHM_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/IterativeAlgorithmImplementation.hxx"
#include "openturns/IterativeMean.hxx"

BEGIN_NAMESPACE_OPENTURNS


class OT_API IterativeAlgorithm
  : public TypedInterfaceObject<IterativeAlgorithmImplementation>
{
  CLASSNAME

public:

//   typedef std::unordered_map<std::string, void*> registry_map;

//   typedef Pointer<IterativeAlgorithmImplementation>   IterativeAlgorithmImplementation_p;

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
  IterativeAlgorithm()
    : TypedInterfaceObject<IterativeAlgorithmImplementation>()
  {
    // Nothing to do
  }

  IterativeAlgorithm(const UnsignedInteger dimension)
    : TypedInterfaceObject<IterativeAlgorithmImplementation>(new IterativeMean(dimension))
  {
    // Nothing to do
  }

  IterativeAlgorithm(const IterativeAlgorithmImplementation & implementation)
    : TypedInterfaceObject<IterativeAlgorithmImplementation>(implementation.clone())
  {
    // Nothing to do
  }

  IterativeAlgorithm(const Implementation & p_implementation)
    : TypedInterfaceObject<IterativeAlgorithmImplementation>(p_implementation)
  {
    // Nothing to do
  }

#ifndef SWIG
  IterativeAlgorithm(IterativeAlgorithmImplementation * p_implementation)
    : TypedInterfaceObject<IterativeAlgorithmImplementation>(p_implementation)
  {
    // Nothing to do
  }
#endif

  /** Return a pointer to the underlying implementation object viewed as a PersistentObject */
  Pointer<IterativeAlgorithmImplementation> getImplementationAsIterativeAlgorithm() const
  {
    return p_implementation_;
  }

  /** Set the pointer to the underlying implementation object */
  void setImplementationAsIterativeAlgorithm(const Pointer<IterativeAlgorithmImplementation> & obj)
  {
    p_implementation_.assign(obj);
  }

//   /** Set the pointer to the underlying implementation object */
//   inline virtual void setIterativeAlgorithm(const Pointer<IterativeAlgorithm> & obj)
//   {
//     p_implementation_.assign(obj);
//   }

  /**
   * Increment methods
   *
   * @warning These methods MUST be overloaded in derived classes.
   */

  virtual void increment(const Point & newData)
  {
    return getImplementation()->increment(newData);
  }

  virtual void increment(const Sample & newData)
  {
    return getImplementation()->increment(newData);
  }

//   virtual void finalize()
//   {
//     return getImplementation()->finalize();
//   }

  /**
   * Iteration accessor
   *
   * This method returns the current iteration number of the algorithm.
   */
  UnsignedInteger getIteration() const
  {
    return getImplementation()->getIteration();
  }

  /**
   * Dimension accessor
   *
   * This method returns the dimension of the object.
   */
  UnsignedInteger getDimension() const
  {
    return getImplementation()->getDimension();
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
