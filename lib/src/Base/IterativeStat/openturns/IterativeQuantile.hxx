

#ifndef OPENTURNS_ITERATIVEQUANTILE_HXX
#define OPENTURNS_ITERATIVEQUANTILE_HXX

#include "openturns/IterativeAlgorithmImplementation.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API IterativeQuantile
  : public IterativeAlgorithmImplementation
{
  CLASSNAME

public:

  explicit IterativeQuantile(const UnsignedInteger dimension = 1,
                             const Scalar order = 0.5,
                             const UnsignedInteger nmax = 1000);

  IterativeQuantile * clone() const;

  void increment(const Point & newData);

  void increment(const Sample & newData);

  /**
   * Comparison operator
   *
   * This method compares objects based on their content.
   */
  inline
  Bool operator ==(const IterativeQuantile & /*other*/) const
  {
    return true;
  }

  /**
   * Comparison operator
   *
   * This method compares objects based on their content.
   */
  inline
  Bool operator !=(const IterativeQuantile & other) const
  {
    return !operator==(other);
  }

  /* String converter */
  String __repr__() const;

  /* String converter */
  String __str__(const String & offset = "") const;

  UnsignedInteger getOrder() const;

  UnsignedInteger getNmax() const;

  Point getQuantile() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

//   static AlgoRegister reg;

private:
  Scalar          order_;
  UnsignedInteger nmax_;
  Point           data_;
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ITERATIVEQUANTILE_HXX */
