
#ifndef OPENTURNS_ITERATIVEMEAN_HXX
#define OPENTURNS_ITERATIVEMEAN_HXX

#include "openturns/IterativeAlgorithmImplementation.hxx"
// #include "openturns/PersistentCollection.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API IterativeMean
  : public IterativeAlgorithmImplementation
{
  CLASSNAME

public:

  explicit IterativeMean(const UnsignedInteger dimension = 1);

  IterativeMean * clone() const;

  void increment(const Point & newData);

  void increment(const Sample & newData);

  void finalize();

  /**
   * Comparison operator
   *
   * This method compares objects based on their content.
   */
  inline
  Bool operator ==(const IterativeMean & /*other*/) const
  {
    return true;
  }

  /**
   * Comparison operator
   *
   * This method compares objects based on their content.
   */
  inline
  Bool operator !=(const IterativeMean & other) const
  {
    return !operator==(other);
  }

  /* String converter */
  String __repr__() const;

  /* String converter */
  String __str__(const String & offset = "") const;

  Point getMean() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

//   static AlgoRegister reg;

private:
  PersistentCollection<Scalar> data_;
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ITERATIVEMEAN_HXX */
