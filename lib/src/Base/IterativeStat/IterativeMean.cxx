
#include "openturns/IterativeMean.hxx"
#include "openturns/IterativeAlgorithm.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"
// #include "openturns/PersistentCollection.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IterativeMean)

static const Factory<IterativeMean> Factory_IterativeMean;

IterativeMean::IterativeMean(const UnsignedInteger dimension)
  : IterativeAlgorithmImplementation(dimension)
  , data_(dimension, 0.0)
{
  // Nothing to do
}

/* Virtual constructor */
IterativeMean * IterativeMean::clone() const
{
  return new IterativeMean(*this);
}

/* String converter */
String IterativeMean::__repr__() const
{
  OSS oss(true);
  oss << "class=" << IterativeMean::GetClassName()
      << " iteration=" << iteration_
      << " dimension=" << dimension_
      << " values=" << data_.__repr__();
  return oss;
}

String IterativeMean::__str__(const String & offset) const
{
  return data_.__str__(offset);
}

Point IterativeMean::getMean() const
{
  return data_;
}

void IterativeMean::increment(const Point & newData)
{
  if (newData.getSize() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given Point is not compatible with the dimension of the iterative mean.";
  iteration_ += 1;
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    Scalar temp = data_[i];
    data_[i] = temp + (newData[i] - temp)/iteration_;
  }
}

void IterativeMean::increment(const Sample & newData)
{
  if (newData.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given Sample is not compatible with the dimension of the iterative mean.";

  for (UnsignedInteger j = 0; j < newData.getSize(); ++j)
  {
    Point tempData = newData[j];
    iteration_ += 1;
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      Scalar temp = data_[i];
      data_[i] = temp + (tempData[i] - temp)/iteration_;
    }
  }
}

/* Method save() stores the object through the StorageManager */
void IterativeMean::save(Advocate & adv) const
{
  IterativeAlgorithmImplementation::save(adv);
  adv.saveAttribute( "data_", data_);
}


/* Method load() reloads the object from the StorageManager */
void IterativeMean::load(Advocate & adv)
{
  IterativeAlgorithmImplementation::load(adv);
  adv.loadAttribute( "data_", data_);
}

void IterativeMean::finalize()
{
  // Nothing to do
};

// AlgoRegister IterativeMean::reg("IterativeMean", &IterativeMean::create);

END_NAMESPACE_OPENTURNS
