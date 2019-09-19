
#ifndef OPENTURNS_ITERATIVEVARIANCE_HXX
#define OPENTURNS_ITERATIVEVARIANCE_HXX

#include "openturns/IterativeAlgorithmImplementation.hxx"
// #include "openturns/PersistentCollection.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API IterativeVariance
  : public IterativeAlgorithmImplementation
{
  CLASSNAME

public:

  explicit IterativeVariance(const UnsignedInteger dimension = 1);

  IterativeVariance * clone() const;

  void increment(const Point & newData);

  void increment(const Sample & newData);

  void finalize();

  /**
   * Comparison operator
   *
   * This method compares objects based on their content.
   */
  inline
  Bool operator ==(const IterativeVariance & /*other*/) const
  {
    return true;
  }

  /**
   * Comparison operator
   *
   * This method compares objects based on their content.
   */
  inline
  Bool operator !=(const IterativeVariance & other) const
  {
    return !operator==(other);
  }

  /* String converter */
  String __repr__() const;

  /* String converter */
  String __str__(const String & offset = "") const;

  UnsignedInteger getSize() const;

  UnsignedInteger getIteration() const;

  Point getVariance() const;

  Point getStandardErrorOfTheMean() const;

  Point getCoeficientOfVariation() const;

  Point getStandardDeviation() const;

  Point getMean() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:
  UnsignedInteger              iteration_;
  UnsignedInteger              dimension_;
  PersistentCollection<Scalar> meanData_;
  PersistentCollection<Scalar> varData_;
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ITERATIVEVARIANCE_HXX */
