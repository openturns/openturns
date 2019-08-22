
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

IterativeThresholdExceedance::IterativeThresholdExceedance(const UnsignedInteger size, const UnsignedInteger thresholdValue)
  : IterativeAlgorithmImplementation()
  , iteration_(0)
  , size_(size)
  , thresholdValue_(thresholdValue)
  , data_(size_, 0.0)
{
  // Nothing to do
}

/* Virtual constructor */
IterativeThresholdExceedance * IterativeThresholdExceedance::clone() const
{
  return new IterativeThresholdExceedance(*this);
}

// IterativeAlgorithm * IterativeThresholdExceedance::create(const int size)
// {
//   return new IterativeThresholdExceedance(size);
// }

/* String converter */
String IterativeThresholdExceedance::__repr__() const
{
  OSS oss(true);
  oss << "class=" << IterativeThresholdExceedance::GetClassName()
      << " iteration=" << iteration_
      << " size=" << size_
      << " threshold value=" << thresholdValue_
      << " values=" << data_.__repr__();
  return oss;
}

String IterativeThresholdExceedance::__str__(const String & offset) const
{
  return data_.__str__(offset);
}

// // Sample & IterativeThresholdExceedance::operator[] (const UnsignedInteger index)
// // {
// //   if (index >= data_.getSize()) throw OutOfBoundException(HERE)  << " Error - index should be between 0 and " << data_.getSize() - 1;
// //   return data_[index];
// // }
// //
// const Sample & IterativeThresholdExceedance::operator[] (const UnsignedInteger index) const
// {
//   if (index >= data_.getSize()) throw OutOfBoundException(HERE)  << " Error - index should be between 0 and " << data_.getSize() - 1;
//   return data_[index];
// }

UnsignedInteger IterativeThresholdExceedance::getIteration() const
{
  return iteration_;
}

UnsignedInteger IterativeThresholdExceedance::getThresholdValue() const
{
  return thresholdValue_;
}

UnsignedInteger IterativeThresholdExceedance::getSize() const
{
  return data_.getSize();
}

Point IterativeThresholdExceedance::getThresholdExceedance() const
{
  return data_;
}

void IterativeThresholdExceedance::increment(const Scalar newData)
{
  iteration_ += 1;
  if (newData > thresholdValue_)
  {
    for (UnsignedInteger i = 0; i < size_; ++i)
    {
      data_[i] += 1;
    }
  }
}

// void IterativeThresholdExceedance::increment(PersistentCollection<Scalar> & newData)
// {
//   iteration_ += 1;
//   for (UnsignedInteger i = 0; i < size_; ++i)
//   {
//     Scalar temp = data_[i];
//     data_[i] = temp + (newData[i] - temp)/iteration_;
//   }
// }

void IterativeThresholdExceedance::increment(const Point & newData)
{
  if (newData.getSize() != size_) throw InvalidArgumentException(HERE) << "Error: the given Point is not compatible with the size of the iterative threshold exceedance.";
  iteration_ += 1;
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    if (newData[i] > thresholdValue_)
    {
      data_[i] += 1;
    }
  }
}

void IterativeThresholdExceedance::increment(const Sample & newData)
{
  if (newData.getDimension() != size_) throw InvalidArgumentException(HERE) << "Error: the given Sample is not compatible with the size of the iterative threshold exceedance.";

  for (UnsignedInteger j = 0; j < newData.getSize(); ++j)
  {
    Point tempData = newData[j];
    iteration_ += 1;
    for (UnsignedInteger i = 0; i < size_; ++i)
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
  PersistentObject::save(adv);
  adv.saveAttribute( "size_", size_);
  adv.saveAttribute( "iteration_", iteration_);
  adv.saveAttribute( "thresholdValue_", thresholdValue_);
  adv.saveAttribute( "data_", data_);
}


/* Method load() reloads the object from the StorageManager */
void IterativeThresholdExceedance::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "size_", size_);
  adv.loadAttribute( "iteration_", iteration_);
  adv.loadAttribute( "thresholdValue_", thresholdValue_);
  adv.loadAttribute( "data_", data_);
}

void IterativeThresholdExceedance::finalize()
{
  // Nothing to do
};

// AlgoRegister IterativeThresholdExceedance::reg("IterativeThresholdExceedance", &IterativeThresholdExceedance::create);

END_NAMESPACE_OPENTURNS
