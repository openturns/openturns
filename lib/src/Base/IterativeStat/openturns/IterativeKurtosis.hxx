
#ifndef OPENTURNS_ITERATIVEKURTOSIS_HXX
#define OPENTURNS_ITERATIVEKURTOSIS_HXX

#include "openturns/IterativeAlgorithmImplementation.hxx"
// #include "openturns/PersistentCollection.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API IterativeKurtosis
  : public IterativeAlgorithmImplementation
{
  CLASSNAME

public:

  explicit IterativeKurtosis(const UnsignedInteger dimension = 1);

  IterativeKurtosis * clone() const;

  void increment(const Point & newData);

  void increment(const Sample & newData);

  void finalize();

  /**
   * Comparison operator
   *
   * This method compares objects based on their content.
   */
  inline
  Bool operator ==(const IterativeKurtosis & /*other*/) const
  {
    return true;
  }

  /**
   * Comparison operator
   *
   * This method compares objects based on their content.
   */
  inline
  Bool operator !=(const IterativeKurtosis & other) const
  {
    return !operator==(other);
  }

  /* String converter */
  String __repr__() const;

  /* String converter */
  String __str__(const String & offset = "") const;

  UnsignedInteger getSize() const;

  UnsignedInteger getIteration() const;

  Point getKurtosis() const;

  Point getSkewness() const;

  Point getVariance() const;

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
  PersistentCollection<Scalar> mean1Data_;
  PersistentCollection<Scalar> mean2Data_;
  PersistentCollection<Scalar> mean3Data_;
  PersistentCollection<Scalar> mean4Data_;
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ITERATIVEKURTOSIS_HXX */
