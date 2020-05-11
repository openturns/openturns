
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

  /** Default constructor */
  IterativeAlgorithmImplementation::IterativeAlgorithmImplementation(const IterativeAlgorithmImplementation & other)
    : PersistentObject(other)
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

END_NAMESPACE_OPENTURNS
