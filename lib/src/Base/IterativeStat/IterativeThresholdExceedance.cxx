
#include "openturns/IterativeThresholdExceedance.hxx"
#include "openturns/IterativeAlgorithm.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"
// #include "openturns/PersistentCollection.hxx"
#include "openturns/Point.hxx"
#include "openturns/IterativeThresholdExceedance.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IterativeThresholdExceedance)

static const Factory<IterativeThresholdExceedance> Factory_IterativeThresholdExceedance;

IterativeThresholdExceedance::IterativeThresholdExceedance(const UnsignedInteger dimension, const Scalar thresholdValue)
  : IterativeAlgorithmImplementation(dimension)
  , thresholdValue_(thresholdValue)
  , data_(dimension_, 0.0)
{
  // Nothing to do
}

/* Virtual constructor */
IterativeThresholdExceedance * IterativeThresholdExceedance::clone() const
{
  return new IterativeThresholdExceedance(*this);
}

// IterativeAlgorithm * IterativeThresholdExceedance::create(const int dimension)
// {
//   return new IterativeThresholdExceedance(dimension);
// }

/* String converter */
String IterativeThresholdExceedance::__repr__() const
{
  OSS oss(true);
  oss << "class=" << IterativeThresholdExceedance::GetClassName()
      << " iteration=" << iteration_
      << " dimension=" << dimension_
      << " threshold value=" << thresholdValue_
      << " values=" << data_.__repr__();
  return oss;
}

String IterativeThresholdExceedance::__str__(const String & offset) const
{
  return data_.__str__(offset);
}

Scalar IterativeThresholdExceedance::getThresholdValue() const
{
  return thresholdValue_;
}

Point IterativeThresholdExceedance::getThresholdExceedance() const
{
  return data_;
}

void IterativeThresholdExceedance::increment(const Point & newData)
{
  if (newData.getSize() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given Point is not compatible with the dimension of the iterative threshold exceedance.";
  iteration_ += 1;
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    if (newData[i] > thresholdValue_)
    {
      data_[i] += 1;
    }
  }
}

void IterativeThresholdExceedance::increment(const Sample & newData)
{
  if (newData.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given Sample is not compatible with the dimension of the iterative threshold exceedance.";

  for (UnsignedInteger j = 0; j < newData.getSize(); ++j)
  {
    Point tempData = newData[j];
    iteration_ += 1;
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      if (tempData[i] > thresholdValue_)
      {
        data_[i] += 1;
      }
    }
  }
}

/* Method save() stores the object through the StorageManager */
void IterativeThresholdExceedance::save(Advocate & adv) const
{
  IterativeAlgorithmImplementation::save(adv);
  adv.saveAttribute( "thresholdValue_", thresholdValue_);
  adv.saveAttribute( "data_", data_);
}


/* Method load() reloads the object from the StorageManager */
void IterativeThresholdExceedance::load(Advocate & adv)
{
  IterativeAlgorithmImplementation::load(adv);
  adv.loadAttribute( "thresholdValue_", thresholdValue_);
  adv.loadAttribute( "data_", data_);
}

// AlgoRegister IterativeThresholdExceedance::reg("IterativeThresholdExceedance", &IterativeThresholdExceedance::create);

END_NAMESPACE_OPENTURNS
