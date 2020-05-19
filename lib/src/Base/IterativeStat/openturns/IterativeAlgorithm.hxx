
#ifndef OPENTURNS_ITERATIVEALGORITHM_HXX
#define OPENTURNS_ITERATIVEALGORITHM_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/IterativeAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

class IterativeAlgorithmImplementation;

class OT_API IterativeAlgorithm
  : public TypedInterfaceObject<IterativeAlgorithmImplementation>
{
  CLASSNAME

public:

  typedef TypedInterfaceObject<IterativeAlgorithmImplementation>::Implementation Implementation;

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
  IterativeAlgorithm();

  IterativeAlgorithm(const UnsignedInteger dimension);

  IterativeAlgorithm(const IterativeAlgorithmImplementation & implementation);

  IterativeAlgorithm(const Implementation & p_implementation);

#ifndef SWIG
  IterativeAlgorithm(IterativeAlgorithmImplementation * p_implementation);
#endif

  /** Return a pointer to the underlying implementation object viewed as a PersistentObject */
  Pointer<IterativeAlgorithmImplementation> getImplementationAsIterativeAlgorithm() const;

  /** Set the pointer to the underlying implementation object */
  void setImplementationAsIterativeAlgorithm(const Pointer<IterativeAlgorithmImplementation> & obj);

  /**
   * Increment methods
   *
   * @warning These methods MUST be overloaded in derived classes.
   */

  virtual void increment(const Point & newData);

  virtual void increment(const Sample & newData);

  /**
   * Iteration accessor
   *
   * This method returns the current iteration number of the algorithm.
   */
  UnsignedInteger getIteration() const;

  /**
   * Dimension accessor
   *
   * This method returns the dimension of the object.
   */
  UnsignedInteger getDimension() const;

  /* String converter */
  virtual String __repr__() const;

  /* String converter */
  virtual String __str__(const String & offset = "") const;

}; /* class IterativeAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ITERATIVEALGORITHM_HXX */
