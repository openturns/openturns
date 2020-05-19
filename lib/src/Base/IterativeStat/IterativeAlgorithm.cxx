
#include "openturns/IterativeAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IterativeAlgorithm)

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
  IterativeAlgorithm::IterativeAlgorithm()
    : TypedInterfaceObject<IterativeAlgorithmImplementation>(new IterativeAlgorithmImplementation())
  {
    // Nothing to do
  }

  IterativeAlgorithm::IterativeAlgorithm(const UnsignedInteger dimension)
    : TypedInterfaceObject<IterativeAlgorithmImplementation>(new IterativeAlgorithmImplementation(dimension))
  {
    // Nothing to do
  }

  IterativeAlgorithm::IterativeAlgorithm(const IterativeAlgorithmImplementation & implementation)
    : TypedInterfaceObject<IterativeAlgorithmImplementation>(implementation.clone())
  {
    // Nothing to do
  }

  IterativeAlgorithm::IterativeAlgorithm(const Implementation & p_implementation)
    : TypedInterfaceObject<IterativeAlgorithmImplementation>(p_implementation)
  {
    // Nothing to do
  }

#ifndef SWIG
  IterativeAlgorithm::IterativeAlgorithm(IterativeAlgorithmImplementation * p_implementation)
    : TypedInterfaceObject<IterativeAlgorithmImplementation>(p_implementation)
  {
    // Nothing to do
  }
#endif

  /** Return a pointer to the underlying implementation object viewed as a PersistentObject */
  Pointer<IterativeAlgorithmImplementation> IterativeAlgorithm::getImplementationAsIterativeAlgorithm() const
  {
    return p_implementation_;
  }

  /** Set the pointer to the underlying implementation object */
  void IterativeAlgorithm::setImplementationAsIterativeAlgorithm(const Pointer<IterativeAlgorithmImplementation> & obj)
  {
    p_implementation_.assign(obj);
  }

  /**
   * Increment methods
   *
   * @warning These methods MUST be overloaded in derived classes.
   */

  void IterativeAlgorithm::increment(const Point & newData)
  {
    return getImplementation()->increment(newData);
  }

  void IterativeAlgorithm::increment(const Sample & newData)
  {
    return getImplementation()->increment(newData);
  }

  /**
   * Iteration accessor
   *
   * This method returns the current iteration number of the algorithm.
   */
  UnsignedInteger IterativeAlgorithm::getIteration() const
  {
    return getImplementation()->getIteration();
  }

  /**
   * Dimension accessor
   *
   * This method returns the dimension of the object.
   */
  UnsignedInteger IterativeAlgorithm::getDimension() const
  {
    return getImplementation()->getDimension();
  }

  /* String converter */
  String IterativeAlgorithm::__repr__() const
  {
    return getImplementation()->__repr__();
  }

  /* String converter */
  String IterativeAlgorithm::__str__(const String & offset) const
  {
    return getImplementation()->__str__(offset);
  }

END_NAMESPACE_OPENTURNS
