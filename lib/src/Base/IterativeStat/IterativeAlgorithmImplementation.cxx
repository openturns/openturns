
#include "openturns/IterativeAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IterativeAlgorithmImplementation)

  /** Default constructor */
  IterativeAlgorithmImplementation::IterativeAlgorithmImplementation(const UnsignedInteger dimension)
    : PersistentObject()
    , iteration_(0)
    , dimension_(dimension)
  {
    // Nothing to do
  }

  /* String converter */
  String IterativeAlgorithmImplementation::__repr__() const
  {
    return OSS(true) << "class=" << getClassName();
  }

  /* String converter */
  String IterativeAlgorithmImplementation::__str__(const String &) const
  {
    return __repr__();
  }

  /**
   * Iteration accessor
   *
   * This method returns the current iteration number of the algorithm.
   */
  UnsignedInteger IterativeAlgorithmImplementation::getIteration() const
  {
    return iteration_;
  }

  /**
   * Dimension accessor
   *
   * This method returns the dimension of the object.
   */
  UnsignedInteger IterativeAlgorithmImplementation::getDimension() const
  {
    return dimension_;
  }

  /** Method save() stores the object through the StorageManager
   *
   * @warning This method MUST be overloaded in derived classes.
   * @internal
   */
  void IterativeAlgorithmImplementation::save(Advocate & adv) const
  {
    PersistentObject::save(adv);
    adv.saveAttribute( "dimension_", dimension_);
    adv.saveAttribute( "iteration_", iteration_);
  }

  /** Method load() reloads the object from the StorageManager
   *
   * @warning This method MUST be overloaded in derived classes.
   * @internal
   */
  void IterativeAlgorithmImplementation::load(Advocate & adv)
  {
    PersistentObject::load(adv);
    adv.loadAttribute( "dimension_", dimension_);
    adv.loadAttribute( "iteration_", iteration_);
  }

END_NAMESPACE_OPENTURNS
